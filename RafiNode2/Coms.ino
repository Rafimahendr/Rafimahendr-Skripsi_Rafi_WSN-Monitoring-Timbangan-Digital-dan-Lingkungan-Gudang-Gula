void usbCommunicationTask(const String& dataRecv) {
  String data = dataRecv;
  String dataHeader = usbSerial.getStrData(data, 0, "#");
  String dataValue = usbSerial.getStrData(data, 1, "#");
  if (isDigit(data[0]) || isDigit(data[1])) {
    // nums
  } else {
    dataHeader.toUpperCase();
    if (dataHeader == "R") {}
    if (dataHeader == "CAL") {  // CAL#2000
      auto loadCell = sensor.getModule<HX711Sens>("lCell");
      Serial.println("| Hilangkan Semua Objek");
      loadCell->setScaleDelay(5000);
      float calValue = dataValue.toFloat();
      Serial.println("| Letakan Objek Terukur: " + String(calValue));
      loadCell->tareDelay(5000);

      float units = loadCell->getUnits(10);
      float cal = loadCell->getCalibrateFactor(units, calValue);

      eeprom.writeFloat(10, cal);

      Serial.println("| Cal Factor: " + String(cal));
      Serial.println("| Tare");
      delay(5000);
      loadCell->setScale(cal);
      loadCell->tare();
    }
  }
}

void loraCommunicationTask(const String& dataRecvCb) {
  String data = dataRecvCb;
  // Serial.println(data);
  int address = lora.getStrData(data, 0, ";").toInt();
  int destination = lora.getStrData(data, 1, ";").toInt();
  String message = lora.getStrData(data, 2, ";");
  message.replace(";", "");
  if (address == ADDRESS_NODE_MASTER && destination == var.thisDeviceAddress) {
    Serial.println(data);
    if (message == "SVE") {
      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_MASTER, ";");
      lora.addData(message, ";");
      lora.addData(var.weight, ";");
      lora.addData(convertWeightToPieces(var.weight));
      // lora.sendData();
      lora.sendDataCb([](const String& sendMsg) {
        Serial.println(sendMsg);
      });
    } else if (message == "GET") {
      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_MASTER, ";");
      lora.addData(message, ";");
      lora.addData(var.weight, ";");
      // lora.sendData();
      lora.sendDataCb([](const String& sendMsg) {
        Serial.println(sendMsg);
      });
    } else if (message == "QTY") {
      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_MASTER, ";");
      lora.addData("QTY", ";");
      // lora.addData(floor(var.weight / 10.f), ";");
      lora.addData(convertWeightToPieces(var.weight));
      lora.sendData();
    } else if (message == "TARE") {
      auto loadCell = sensor.getModule<HX711Sens>("lCell");
      loadCell->tare();

      var.weight = 0;
      var.weightOffset = 0;
      eeprom.writeFloat(20, (float)0.0);

      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_MASTER, ";");
      lora.addData("TARE", ";");
      lora.sendData();
    }
  }
}