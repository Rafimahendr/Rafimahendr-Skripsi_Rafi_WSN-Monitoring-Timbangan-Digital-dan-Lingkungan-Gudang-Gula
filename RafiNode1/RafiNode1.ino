#include "Header.h"

void setup() {
  usbSerial.begin(&Serial, 115200);
  lora.init(433E6, []() {
    Serial.println("LoRa init success");
  });

  menu.initialize(true);
  menu.setLen(16, 2);

  sensor.addModule("dht", new DHTSens(5, DHT22));
  sensor.addModule("lCell", []() -> BaseSens* {
    auto loadCell = new HX711Sens(6, 7, HX711Sens::KG);
    loadCell->filter([](float sensorValue) -> float {
      filter.addMeasurement(sensorValue);
      float filteredValue = filter.getFilteredValue();
      return filteredValue;
    });
    return loadCell;
  });
  sensor.init([]() {
    auto loadCell = sensor.getModule<HX711Sens>("lCell");
    float cal = eeprom.readFloat(10);
    float offset = eeprom.readFloat(20);
    var.weightOffset = isnan(offset) ? 0.f : offset;
    loadCell->setScale(cal);
    loadCell->tare();
  });

  // eeprom.writeInt(0, ADDRESS_NODE_1);
  var.thisDeviceAddress = eeprom.readInt(0);

  Serial.print("| var.thisDeviceAddress: 0x0");
  Serial.print(var.thisDeviceAddress, HEX);
  Serial.println();
}

void lcdMenu() {
  static uint32_t lcdTimer = 0;
  static bool lcdState = true;

  if (millis() - lcdTimer >= 3000) {
    lcdState = !lcdState;
    lcdTimer = millis();
  }

  char buffer1[8], buffer2[8];
  char bufferUp[20], bufferDown[20];

  if (lcdState) {
    dtostrf(var.weight, 6, 2, buffer1);
    int pcs = convertWeightToPieces(var.weight);

    sprintf(bufferUp, "Wgt: %6s Kg", buffer1);
    sprintf(bufferDown, "Qty: %2d pcs", pcs);
  } else {
    dtostrf(var.temperature, 6, 2, buffer1);
    dtostrf(var.humidity, 6, 2, buffer2);

    sprintf(bufferUp, "Tem: %6s *C", buffer1);
    sprintf(bufferDown, "Hum: %6s %", buffer2);
  }

  menu.setCursor(0, 0);
  menu.print(bufferUp);
  menu.setCursor(0, 1);
  menu.print(bufferDown);
}

void loop() {
  usbSerial.receive(usbCommunicationTask);
  lora.receiveString(loraCommunicationTask);

  if (var.isCalibrate) return;

  sensor.update([]() {
    var.temperature = sensor["dht"]["temp"];
    var.humidity = sensor["dht"]["hum"];
    var.weight = sensor["lCell"];
    var.weight += var.weightOffset;
    if (eepromTimer.triggered() && var.weight > 0) {
      eeprom.writeFloat(20, (float)var.weight);
    }
  });

  lcdMenu();

  DigitalIn::updateAll(&buttonRst, &buttonBoot, DigitalIn::stop());
}

int convertWeightToPieces(float weight) {
  int pieces = weight / 10;
  if (fmod(weight, 10.0) > 5.0) {
    pieces += 1;
  }
  return pieces;
}
