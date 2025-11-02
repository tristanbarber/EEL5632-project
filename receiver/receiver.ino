#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(15); // Chip-Select (CS) pin on MCP2515 to D15 pin on ESP32

void setup() {
  Serial.begin(115200);
  SPI.begin(); // SCK=D18, MISO=D19, MOSI=D23
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ); // This configuration must match the configuration in sender.ino
  mcp2515.setNormalMode();

  Serial.println("MCP2515 Receiver Initialized (125kbps)");
}

void loop() {
  // Checks for received CAN message and prints to serial
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print("Received CAN ID: 0x");
    Serial.print(canMsg.can_id, HEX);
    Serial.print(" Data: ");
    for (int i = 0; i < canMsg.can_dlc; i++) {
      Serial.print(canMsg.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}