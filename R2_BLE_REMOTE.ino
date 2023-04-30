/* 
for R2D2 & R2Q5
protocol structure ->
SOP, FLAGS, TID, SID, DID, CID, SEQ, ERR, DATA..., CHK, EOP
*/

#include <bluefruit.h>

// const char CONNECT_SERVICE[] = "00020001574f4f2053706865726f2121";
// const char CONNECT_CHAR[] = "00020005574f4f2053706865726f2121";

// const char MAIN_SERVICE[] = "00010001574f4f2053706865726f2121";
// const char MAIN_CHAR[] = "00010002574f4f2053706865726f2121";

//  enum ServicesUUID {
//   apiV2ControlService = '00010001 574f4f2053706865726f2121',
//   nordicDfuService = '00020001 574f4f2053706865726f2121',
// }

//  enum CharacteristicUUID {
//   apiV2Characteristic = '00010002 574f4f2053706865726f2121',
//   dfuControlCharacteristic = '00020002 574f4f2053706865726f2121',
//   dfuInfoCharacteristic = '00020004 574f4f2053706865726f2121',
//   antiDoSCharacteristic = '00020005 574f4f2053706865726f2121',
//   subsCharacteristic = '00020003 574f4f2053706865726f2121',
// }

  //  force_band = "00020005-574f-4f20-5370-6865726f2121"
  //   api_v2 = "00010002-574f-4f20-5370-6865726f2121"
  //    ch_force_band = self._find_characteristic(SpheroCharacteristic.force_band.value)
  //       ch_force_band.write_value(b"usetheforce...band")

  //       self.ch_api_v2 = self._find_characteristic(SpheroCharacteristic.api_v2.value)
  //            self._executor.submit(self.manager.run)



#define MAIN_SERVICE "00010001-574f-4f20-5370-6865726f2121"
#define MAIN_CHAR "00010002-574f-4f20-5370-6865726f2121"
#define CONNECT_SERVICE "00020001-574f-4f20-5370-6865726f2121"
#define CONNECT_CHAR "00020005-574f-4f20-5370-6865726f2121"



const uint8_t MSG_CONNECTION[] = { 0x75, 0x73, 0x65, 0x74, 0x68, 0x65, 0x66, 0x6F, 0x72, 0x63, 0x65, 0x2E, 0x2E, 0x2E, 0x62, 0x61, 0x6E, 0x64 };
const uint8_t MSG_INIT[] = { 0x0A, 0x13, 0x0D };
const uint8_t MSG_OFF[] = { 0x0A, 0x13, 0x01 };
const uint8_t MSG_ROTATE[] = {0x0A,0x17,0x0F};

static const char *droidAddress = "AA:BB:CC:DD:EE:FF";

BLEService connectService(CONNECT_SERVICE);  // Create service objects
BLECharacteristic connectChar(CONNECT_CHAR);
BLEService mainService(MAIN_SERVICE);
BLECharacteristic mainChar(MAIN_CHAR);
//R2
  // protected maxVoltage = 3.65;
  // protected minVoltage = 3.4;


#define SOP 0x8D
#define EOP 0xD8
#define ESC 0xAB
#define ESC_ESC 0x23
#define ESC_SOP 0x05
#define ESC_EOP 0x50

volatile uint8_t seq = 0;

uint8_t calculateChk(uint8_t *buff, uint8_t len) {
  uint8_t ret = 0x00;
  for (uint8_t i = 0; i < len; i++) {
    ret += buff[i];
  }
  ret = ret & 0xFF;
  return (ret ^ 0xFF);
}

void convertDegreeToHex(float degree, uint8_t *hexData) {
  float *degreePtr = (float *)hexData;
  *degreePtr = degree;
}

void buildPacket(uint8_t *init, size_t init_len, uint8_t *payload, size_t payload_len, uint8_t *packet, size_t *packet_len) {
  size_t packet_pos = 0;
  packet[packet_pos++] = SOP;
  uint8_t body[256];
  size_t body_len = 0;

  memcpy(&body[body_len], init, init_len);
  body_len += init_len;
  body[body_len++] = seq;
  memcpy(&body[body_len], payload, payload_len);
  body_len += payload_len;

  body[body_len++] = calculateChk(body, body_len);

  for (size_t i = 0; i < body_len; i++) {
    if (body[i] == ESC) {
      packet[packet_pos++] = ESC;
      packet[packet_pos++] = ESC_ESC;
    } else if (body[i] == SOP) {
      packet[packet_pos++] = ESC;
      packet[packet_pos++] = ESC_SOP;
    } else if (body[i] == EOP) {
      packet[packet_pos++] = ESC;
      packet[packet_pos++] = ESC_EOP;
    } else {
      packet[packet_pos++] = body[i];
    }
  }
  packet[packet_pos++] = EOP;
  seq++;
  *packet_len = packet_pos;
}

// Send a packet to the device
void sendPacket(BLECharacteristic &characteristic, const uint8_t *packetData, uint16_t packetLength, bool waitForResponse = false, uint32_t timeout = 0) {
  characteristic.write (packetData, packetLength);
  // if (waitForResponse) {
  //   characteristic.waitForWriteCompletion(timeout);
  // }
}
void scan_callback(ble_gap_evt_adv_report_t *report) {
  // Check if advertising contain BleUart service
  if (Bluefruit.Scanner.checkReportForService(report, mainService)) {
    Serial.print("BLE UART service detected. Connecting ... ");

    // Connect to device with mainService service in advertising
    Bluefruit.Central.connect(report);
  } else {
    // For Softdevice v6: after received a report, scanner will be paused
    // We need to call Scanner resume() to continue scanning
    Bluefruit.Scanner.resume();
  }

  // char address[18];
  // gap_address_to_string(report->peer_addr.addr, address);
  // printf("Found device: %s\n", address);
}

void connect_callback(uint16_t conn_handle) {
  Serial.println("Connected to device (conn_handle=" + String(conn_handle) + ")");
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  Serial.println("Disconnected from device (conn_handle=" + String(conn_handle) + ", reason=" + String(reason) + ")");
}



void setup() {
  Serial.begin(115200);
  Bluefruit.begin();
  // Set the device name and appearance
  Bluefruit.setName("MyCentralRemote");
  Bluefruit.setAppearance(BLE_APPEARANCE_GENERIC_PHONE);

  // Increase Blink rate to different from PrPh advertising mode
  Bluefruit.setConnLedInterval(250);
  // Callbacks for Central
  Bluefruit.Central.setConnectCallback(connect_callback);
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);
  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Don't use active scan
   * - Start(timeout) with timeout = 0 will scan forever (until connected)
   */

  Serial.println("Starting scan...");
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);  // in unit of 0.625 ms
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);
}


void loop() {
   // do nothing for now
}