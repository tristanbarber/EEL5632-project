#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(15); // Chip-Select (CS) pin on MCP2515 to D15 pin on ESP32

void setup() {
  Serial.begin(115200);
  SPI.begin(); // SCK=D18, MISO=D19, MOSI=D23
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ); // This configuration must match the configuration in receiver.ino
  mcp2515.setNormalMode();

  Serial.println("MCP2515 Sender Initialized (125kbps)");
}

void loop() {
  // Sample CAN frame
  canMsg.can_id  = 0x100;  // CAN ID
  canMsg.can_dlc = 8;      // Data length
  canMsg.data[0] = 0x11;
  canMsg.data[1] = 0x22;
  canMsg.data[2] = 0x33;
  canMsg.data[3] = 0x44;
  canMsg.data[4] = 0x55;
  canMsg.data[5] = 0x66;
  canMsg.data[6] = 0x77;
  canMsg.data[7] = 0x88;

  mcp2515.sendMessage(&canMsg);

  Serial.println("Sent CAN message: ID 0x100");
  delay(1000);
}