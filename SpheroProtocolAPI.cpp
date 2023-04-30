#include "SpheroProtocolAPI.h"
#include <cstdint>
#include <array>



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