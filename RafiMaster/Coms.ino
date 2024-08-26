void usbCommunicationTask(const String& dataRecv) {
  String data = dataRecv;
  String dataHeader = usbSerial.getStrData(data, 0, "#");
  String dataValue = usbSerial.getStrData(data, 1, "#");
  if (isDigit(data[0]) || isDigit(data[1])) {
    // nums
  } else {
    dataHeader.toUpperCase();
    if (dataHeader == "R") ESP.restart();
    if (dataHeader == "NODE1") var.getDataNode1 = DATA_MESSAGE_WAIT;
    if (dataHeader == "NODE2") var.getDataNode2 = DATA_MESSAGE_WAIT;
  }
}

void loraCommunicationTask(const String& dataRecvCb) {
  String data = dataRecvCb;
  // Serial.println(data);
  int address = lora.getStrData(data, 0, ";").toInt();
  int destination = lora.getStrData(data, 1, ";").toInt();
  String message = lora.getStrData(data, 2, ";");
  message.replace(";", "");
  if (message != "DHT") Serial.println(data);
  if (address == ADDRESS_NODE_1 && destination == var.thisDeviceAddress) {
    ledRed.on();
    ledRed.offDelay(1000);
    if (message == "SVE") {
      var.node1.temperature = lora.getStrData(data, 3, ";").toFloat();
      var.node1.humidity = lora.getStrData(data, 4, ";").toFloat();
      var.node1.weight = lora.getStrData(data, 5, ";").toFloat();
      var.node1.quantity = lora.getStrData(data, 6, ";").toFloat();
    } else if (message == "DHT") {
      var.node1.temperature = lora.getStrData(data, 3, ";").toFloat();
      var.node1.humidity = lora.getStrData(data, 4, ";").toFloat();
      var.tempWarningNode1 = var.node1.temperature < 28.f || var.node1.temperature > 32.f;
      var.humWarningNode1 = var.node1.humidity < 55.f || var.node1.humidity > 75.f;
    } else if (message == "GET") {
      var.node1.temperature = lora.getStrData(data, 3, ";").toFloat();
      var.node1.humidity = lora.getStrData(data, 4, ";").toFloat();
      var.node1.weight = lora.getStrData(data, 5, ";").toFloat();
      var.getDataNode1 = DATA_MESSAGE_RECEIVED;
    } else if (message == "QTY") {
      var.node1.quantity = lora.getStrData(data, 3, ";").toFloat();
      var.getDataNode1 = QUANTITY_MESSAGE_RECEIVED;
    } else if (message == "TARE") {
      var.getDataNode1 = TARE_LOADCELL_DONE;
    }
  } else if (address == ADDRESS_NODE_2 && destination == var.thisDeviceAddress) {
    ledGreen.on();
    ledGreen.offDelay(1000);
    if (message == "SVE") {
      var.node2.weight = lora.getStrData(data, 3, ";").toFloat();
      var.node2.quantity = lora.getStrData(data, 4, ";").toFloat();
    } else if (message == "GET") {
      var.node2.weight = lora.getStrData(data, 3, ";").toFloat();
      var.getDataNode2 = DATA_MESSAGE_RECEIVED;
    } else if (message == "QTY") {
      var.node2.quantity = lora.getStrData(data, 3, ";").toFloat();
      var.getDataNode2 = QUANTITY_MESSAGE_RECEIVED;
    } else if (message == "TARE") {
      var.getDataNode2 = TARE_LOADCELL_DONE;
    }
  }
}