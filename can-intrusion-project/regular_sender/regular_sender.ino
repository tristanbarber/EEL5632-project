#include <SPI.h>
#include <mcp2515.h>

#define NUM_ECUS 10

struct can_frame canMsg;
MCP2515 mcp2515(15);

const unsigned long message_rate = 3000;

struct ECUProfile {
  const char* name;
  uint32_t id;
  uint8_t dlc;
  uint8_t minVal[8];
  uint8_t maxVal[8];
};

// Valid list of ECUs to send data from.
ECUProfile ecus[NUM_ECUS] = {
  { "Engine ECU", 0x100, 8, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } },
  { "Transmission TCM", 0x110, 8, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } },
  { "ABS Module", 0x120, 6, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } },
  { "Airbag Module", 0x130, 4, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } },
  { "Body Control", 0x140, 3, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } },
  { "Climate Control", 0x150, 6, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } },
  { "Infotainment", 0x160, 8, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } },
  { "Instrument Cluster", 0x170, 7, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } },
  { "Door Module", 0x180, 5, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } },
  { "Steering Control", 0x190, 8, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 255, 255, 255, 255, 255, 255, 255, 255 } }
};

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  randomSeed(analogRead(0));
}

void loop() {
  int idx = random(0, NUM_ECUS);
  ECUProfile& ecu = ecus[idx];

  canMsg.can_id = ecu.id;
  canMsg.can_dlc = ecu.dlc;

  for (int i = 0; i < ecu.dlc; i++)
    canMsg.data[i] = random(ecu.minVal[i], ecu.maxVal[i] + 1);

  // This code is hard to follow, but essentially it sends the CAN frame, and prints out
  // the contents (ID, DLC, and Data) of the frame if there were no errors when sending.
  if (mcp2515.sendMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print("Sent CAN frame from ");
    Serial.print(ecu.name);
    Serial.print(" (ID 0x");
    Serial.print(ecu.id, HEX);
    Serial.print(") DLC=");
    Serial.print(ecu.dlc);
    Serial.print(" Data: ");
    for (int i = 0; i < ecu.dlc; i++) {
      Serial.print("0x");
      if (canMsg.data[i] < 16) Serial.print("0");
      Serial.print(canMsg.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.println("Send failed");
  }

  delay(1000);
}