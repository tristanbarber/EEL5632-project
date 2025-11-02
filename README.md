# EEL5632-project

### Wiring (ESP32 ↔ MCP2515)

| MCP2515 Pin | ESP32 Pin | Description |
|--------------|------------|-------------|
| VCC          | 3.3V       | Module power |
| GND          | GND        | Ground |
| CS           | D15        | Chip select |
| SCK          | D18        | SPI clock |
| MISO         | D19        | SPI MISO |
| MOSI         | D23        | SPI MOSI |
| INT          | (Not used) | Optional interrupt pin |
| CANH         | CANH ↔ CANH | Connect CAN high lines together |
| CANL         | CANL ↔ CANL | Connect CAN low lines together |

> Ensure **both modules share the same ground** and **CANH/CANL are correctly paired**.  

---

## Software Setup

1. Follow the instructions on the **[mcp2515 Arduino library GitHub](https://github.com/autowp/arduino-mcp2515)** to install the MCP2515 library.

---

## sender.ino & receiver.ino demo setup
1. Open either `sender.ino` or `receiver.ino` in the Arduino IDE.  
2. Select the correct **ESP32 board** and **COM port** under *Tools → Board/Port*.  
3. Upload `sender.ino` to one ESP32 and `receiver.ino` to the other.
4. Open the Serial Monitor on both boards at **115200 baud**.
    - The **sender** transmits a CAN frame every second with ID `0x100`.
    - The **receiver** prints received CAN IDs and data to the Serial Monitor.
    