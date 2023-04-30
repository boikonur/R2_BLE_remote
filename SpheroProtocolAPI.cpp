#include "SpheroProtocolAPI.h"
#include <cstdint>
#include <array>


// float get_battery_voltage()
// {
//   int data = 0;
//   response = request(PowerCommand.get_battery_voltage)

//   for (int i = 0; i < response.data.size(); i++)
//   {
//     data = (data << 8) | response.data[i];
//   }
//   return (float)data / 100.0;
// }

// int get_battery_percentage()
// {
//   response = self.request(PowerCommand.get_battery_percentage, timeout = 1000)
//   return response.data[0]
// }


Packet(DeviceId device_id, uint8_t command_id, int flags = -1, int target_id = -1, int source_id = -1, int sequence = -1, std::vector<int> data = {})
  {
      this->flags = flags;
      if (flags == -1)
      {
          this->flags = (int)Flag::requests_response | (int)Flag::resets_inactivity_timeout;
      }
      if (flags == -1 && source_id != -1)
      {
          this->flags |= (int)Flag::command_has_source_id;
      }
      if (flags == -1 && target_id != -1)
      {
          this->flags |= (int)Flag::command_has_target_id;
      }

      this->target_id = target_id;
      this->source_id = source_id;
      this->device_id = device_id;
      this->command_id = command_id;
      this->sequence = sequence == -1 ? generate_sequence() : sequence;
      this->data = data;
  }



std::array<uint8_t, SpheroProtocol::maxDataSize * 2> SpheroProtocol::createPacket(const Packet& packet, size_t& packetSize) {
    std::array<uint8_t, maxDataSize * 2> rawData;
    size_t idx = 0;

    rawData[idx++] = SOP;
    rawData[idx++] = packet.flags;
    rawData[idx++] = packet.tid;
    rawData[idx++] = packet.sid;
    rawData[idx++] = packet.did;
    rawData[idx++] = packet.cid;
    rawData[idx++] = packet.seq;
    rawData[idx++] = packet.err;

    size_t encodedSize;
    std::array<uint8_t, maxDataSize * 2> encodedData = encodePayload(packet.data, packet.dataSize, encodedSize);
    for (size_t i = 0; i < encodedSize; ++i) {
        rawData[idx++] = encodedData[i];
    }

    rawData[idx++] = calculateChk(packet);
    rawData[idx++] = EOP;

    packetSize = idx;
    return rawData;
}

// uint8_t SpheroProtocol::calculateChk(uint8_t *buff, uint8_t len) {
//   uint8_t ret = 0x00;
//   for (uint8_t i = 0; i < len; i++) {
//     ret += buff[i];
//   }
//   ret = ret & 0xFF;
//   return (ret ^ 0xFF);
// }


uint8_t SpheroProtocol::calculateChk(const Packet& packet) {
    uint16_t sum = 0;

    sum += packet.flags;
    sum += packet.tid;
    sum += packet.sid;
    sum += packet.did;
    sum += packet.cid;
    sum += packet.seq;
    sum += packet.err;

    for (size_t i = 0; i < packet.dataSize; ++i) {
        sum += packet.data[i];
    }

    uint8_t checksum = static_cast<uint8_t>(sum % 256);
    return ~checksum;
}

// FLAGS related methods
bool SpheroProtocol::isResponse(const Packet& packet) {
    return packet.flags & 0x01;
}

void SpheroProtocol::setResponseFlag(Packet& packet, bool value) {
    if (value) {
        packet.flags |= 0x01;
    } else {
        packet.flags &= ~0x01;
    }
}

bool SpheroProtocol::shouldRequestResponse(const Packet& packet) {
    return packet.flags & 0x02;
}

void SpheroProtocol::setRequestResponseFlag(Packet& packet, bool value) {
    if (value) {
        packet.flags |= 0x02;
    } else {
        packet.flags &= ~0x02;
    }
}

// ... other FLAGS related methods

// Error code related methods
uint8_t SpheroProtocol::getErrorCode(const Packet& packet) {
    return packet.err;
}

void SpheroProtocol::setErrorCode(Packet& packet, uint8_t errorCode) {
    packet.err = errorCode;
}