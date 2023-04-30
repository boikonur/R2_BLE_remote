/* 
for R2D2 & R2Q5
protocol structure ->
SOP, FLAGS, TID, SID, DID, CID, SEQ, ERR, DATA..., CHK, EOP
*/

#include <bluefruit.h>

static constexpr char droidAddress[] = "AA:BB:CC:DD:EE:FF";

//api V2 Control Service
static constexpr char apiV2ControlServiceUUID[] = "00010001-574f-4f20-5370-6865726f2121";
static constexpr char apiV2MainCharacteristicUUID[] = "00010002-574f-4f20-5370-6865726f2121";  //MAIN Characteristic

//nordic Dfu Service
static constexpr char nordicConnectServiceUUID[] "00020001-574f-4f20-5370-6865726f2121";
static constexpr char dfuControlCharacteristicUUID[] = "00020002-574f-4f20-5370-6865726f2121";
static constexpr char subsCharacteristicUUID[] = "00020003-574f-4f20-5370-6865726f2121";
static constexpr char dfuInfoCharacteristicUUID[] = "00020004-574f-4f20-5370-6865726f2121";
static constexpr char forceBandCharacteristicUUID[] = "00020005-574f-4f20-5370-6865726f2121";  //CONNECT Characteristic  (write_value("usetheforce...band"))




BLEService connectService(nordicConnectServiceUUID);  // Create service objects
BLECharacteristic connectChar(forceBandCharacteristicUUID);

BLEService mainService(apiV2ControlServiceUUID);
BLECharacteristic mainChar(apiV2MainCharacteristicUUID);
//R2
//  float maxVoltage = 3.65;
//  float minVoltage = 3.4;


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
  characteristic.write(packetData, packetLength);
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