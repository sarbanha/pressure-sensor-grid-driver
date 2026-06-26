#include <Wire.h>

const int switchPin = 21;

#define WIRE Wire
#define MMR920_CMD_RESET        0x72
#define MMR920_CMD_ACTIVE1      0xA0
#define MMR920_CMD_RESULT_PRESS 0xC0

uint8_t sensor_addr[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

uint8_t pressResultData[3];
float pressConvData;

uint32_t startTimer;
const uint32_t waitTime = 401;

void setup() {
  Serial.begin(1000000);
  while (!Serial);
  delay(1000);

  Serial.println("READY");

  WIRE.begin();
  WIRE.setClock(400000);
  WIRE.setTimeout(10);

  Serial.println("Initializing sensors...");

  for (uint8_t i = 0; i < sizeof(sensor_addr) / sizeof(sensor_addr[0]); i++) {
    sensor_reset(sensor_addr[i]);
    delay(10);
    sensor_start(sensor_addr[i], MMR920_CMD_ACTIVE1);
  }

  startTimer = micros();
  Serial.println("Micros,Addr,Pressure[bar],Flag");
}

void loop() {
  if (micros() - startTimer < waitTime) return;
  startTimer = micros();

  for (uint8_t i = 0; i < sizeof(sensor_addr) / sizeof(sensor_addr[0]); i++) {
    if (!sensor_read_result(sensor_addr[i], MMR920_CMD_RESULT_PRESS, pressResultData, 3)) {
      pressConvData = sensor_conv_pressure(pressResultData);

      Serial.print(micros());
      Serial.print(",0x");
      Serial.print(sensor_addr[i], HEX);
      Serial.print(",");
      Serial.print(((pressConvData * 0.554) + 0.2273), 5);
      Serial.print(",");
      Serial.println(digitalRead(switchPin));
    }
  }
}

bool sensor_reset(uint8_t addr) {
  WIRE.beginTransmission(addr);
  WIRE.write(MMR920_CMD_RESET);
  return (WIRE.endTransmission() != 0);
}

bool sensor_start(uint8_t addr, uint8_t mode) {
  WIRE.beginTransmission(addr);
  WIRE.write(mode);
  return (WIRE.endTransmission() != 0);
}

bool sensor_read_result(uint8_t addr, uint8_t target, uint8_t* data, uint8_t dataLen) {
  WIRE.beginTransmission(addr);
  WIRE.write(target);
  if (WIRE.endTransmission(false) != 0) return true;

  if (WIRE.requestFrom(addr, (size_t)dataLen) != dataLen) return true;

  for (uint8_t i = 0; i < dataLen; i++) {
    data[i] = WIRE.read();
  }
  return false;
}

float sensor_conv_pressure(uint8_t* result) {
  int32_t raw = ((int32_t)result[0] << 16) |
                ((int32_t)result[1] << 8) |
                ((int32_t)result[2]);
  if (raw & 0x800000) {
    raw |= 0xFF000000;
  }
  return (float)raw / 100000.0;
}
