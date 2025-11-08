#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(15);

#define FLOOD_DETECT_WINDOW 2000
#define FLOOD_RATE_THRESHOLD 5
const uint8_t NUM_ECUS = 10;

struct ECUProfile {
  const char* name;
  uint32_t id;
  uint8_t expected_dlc;
};

struct ECUState {
  uint32_t id;
  unsigned long windowStart;
  uint16_t countInWindow;
};

ECUProfile ecus[NUM_ECUS];
ECUState states[NUM_ECUS];
int ecuCount = 0;

// Function to add an ecu to the list of allowed ecus.
void addECU(const char* name, uint32_t id, uint8_t dlc) {
  if (ecuCount >= NUM_ECUS) return;
  ecus[ecuCount].name = name;
  ecus[ecuCount].id = id;
  ecus[ecuCount].expected_dlc = dlc;

  states[ecuCount].id = id;
  states[ecuCount].windowStart = 0;
  states[ecuCount].countInWindow = 0;

  ecuCount++;
}

// Helper function to find the ecu by it's index in the CAN frame.
int findECUIndex(uint32_t id) {
  for (int i = 0; i < ecuCount; i++) {
    if (ecus[i].id == id) return i;
  }
  return -1;
}

// This function handles the flood detection. A confgurable FLOOD_DETECT_WINDOW 
// is constantly being checked for messages from each ecu. If an ecu transmits 
// more messages than FLOOD_RATE_THRESHOLD within the window, an alert is printed.
void handleRateAndWindow(int idx) {
  unsigned long now = millis();
  ECUState &s = states[idx];

  if (s.windowStart == 0 || now - s.windowStart > FLOOD_DETECT_WINDOW) {
    s.windowStart = now;
    s.countInWindow = 1;
  } else {
    s.countInWindow++;
    if (s.countInWindow > FLOOD_RATE_THRESHOLD) {
      Serial.print("FLOOD DETECTED: ");
      Serial.print(ecus[idx].name);
      Serial.print(" (ID 0x"); Serial.print(ecus[idx].id, HEX);
      Serial.print(") count="); Serial.print(s.countInWindow);
      Serial.print(" threshold="); Serial.println(FLOOD_RATE_THRESHOLD);
      s.windowStart = now;
      s.countInWindow = 0;
    }
  }
}

// Checks the CAN payload for unknown ID and prints an alert if so.
bool checkUnknownID(int &idx, uint32_t id) {
  idx = findECUIndex(id);
  if (idx < 0) {
    Serial.print("ALERT: Unknown CAN ID 0x"); 
    Serial.println(id, HEX);
    return true;
  }
  return false;
}

// Checks for a DLC mismatch and prints an alert if so.
bool checkDLC(int idx, uint8_t dlc) {
  if (dlc != ecus[idx].expected_dlc) {
    Serial.print("ALERT: DLC MISMATCH for ");
    Serial.print(ecus[idx].name);
    Serial.print(" (ID 0x"); Serial.print(ecus[idx].id, HEX);
    Serial.print(") got "); Serial.print(dlc);
    Serial.print(" expected "); Serial.println(ecus[idx].expected_dlc);
    return true;
  }
  return false;
}

// Print the received CAN message.
void printCANMessage(int idx) {
  Serial.print("Received from ");
  Serial.print(ecus[idx].name);
  Serial.print(" (ID 0x"); Serial.print(ecus[idx].id, HEX);
  Serial.print(") DLC: "); Serial.print(ecus[idx].expected_dlc);
  Serial.print(" Data:");
  for (int i = 0; i < ecus[idx].expected_dlc; i++) {
    Serial.print(" 0x");
    if (canMsg.data[i] < 16) Serial.print("0");
    Serial.print(canMsg.data[i], HEX);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  addECU("Engine ECU",         0x100, 8);
  addECU("Transmission TCM",   0x110, 8);
  addECU("ABS Module",         0x120, 6);
  addECU("Airbag Module",      0x130, 4);
  addECU("Body Control",       0x140, 3);
  addECU("Climate Control",    0x150, 6);
  addECU("Infotainment",       0x160, 8);
  addECU("Instrument Cluster", 0x170, 7);
  addECU("Door Module",        0x180, 5);
  addECU("Steering Control",   0x190, 8);
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    int idx;
    if (!checkUnknownID(idx, canMsg.can_id) && !checkDLC(idx, canMsg.can_dlc)) {
      printCANMessage(idx);
      handleRateAndWindow(idx);
    }
  }
}