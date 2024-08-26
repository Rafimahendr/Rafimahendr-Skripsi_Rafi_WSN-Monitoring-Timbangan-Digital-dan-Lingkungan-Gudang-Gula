#include "Header.h"

void setup() {
  usbSerial.begin(&Serial, 115200);
  task.initialize(wifiConTask);
  lora.init(5, 14, 2, 433E6, []() {
    Serial.println("LoRa init success");
  });

  preferences.begin("rafi", false);
  // preferences.putInt("address", ADDRESS_NODE_MASTER);
  var.thisDeviceAddress = preferences.getInt("address", 0);
  preferences.end();

  Serial.print("| var.thisDeviceAddress: 0x0");
  Serial.print(var.thisDeviceAddress, HEX);
  Serial.println();
}

void loop() {
  debug();

  static int stateTimeout1 = 0;
  static int stateTimeout2 = 0;
  static uint32_t stateTimeoutTimer;
  if (var.getDataNode1 != var.getDataNode1Before) {
    var.getDataNode1Before = var.getDataNode1;
    if (var.getDataNode1 == DATA_MESSAGE_WAIT) {
      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_1, ";");
      lora.addData("GET", ";");
      lora.sendData();
    }
    if (var.getDataNode1 == QUANTITY_MESSAGE_WAIT) {
      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_1, ";");
      lora.addData("QTY", ";");
      lora.sendData();
    }
    if (var.getDataNode1 == TARE_LOADCELL_WAIT) {
      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_1, ";");
      lora.addData("TARE", ";");
      lora.sendData();
    }
  }

  if (var.getDataNode2 != var.getDataNode2Before) {
    var.getDataNode2Before = var.getDataNode2;
    if (var.getDataNode2 == DATA_MESSAGE_WAIT) {
      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_2, ";");
      lora.addData("GET", ";");
      lora.sendData();
    }
    if (var.getDataNode2 == QUANTITY_MESSAGE_WAIT) {
      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_2, ";");
      lora.addData("QTY", ";");
      lora.sendData();
    }
    if (var.getDataNode2 == TARE_LOADCELL_WAIT) {
      lora.clearData();
      lora.addData(var.thisDeviceAddress, ";");
      lora.addData(ADDRESS_NODE_2, ";");
      lora.addData("TARE", ";");
      lora.sendData();
    }
  }

  static uint32_t getTemperatureTimer;
  if (millis() - getTemperatureTimer >= 5000) {
    lora.clearData();
    lora.addData(var.thisDeviceAddress, ";");
    lora.addData(ADDRESS_NODE_1, ";");
    lora.addData("DHT", ";");
    lora.sendData();
    getTemperatureTimer = millis();
  }

  if (var.getDataNode1 == FIREBASE_LOAD_NODE_DATA) {
    var.getDataNode1 = IDLE_STATE;
    lora.clearData();
    lora.addData(var.thisDeviceAddress, ";");
    lora.addData(ADDRESS_NODE_1, ";");
    lora.addData("SVE", ";");
    lora.sendDataCb([](const String & sendMsg) {
      Serial.println(sendMsg);
    });
  }

  if (var.getDataNode2 == FIREBASE_LOAD_NODE_DATA) {
    var.getDataNode2 = IDLE_STATE;
    lora.clearData();
    lora.addData(var.thisDeviceAddress, ";");
    lora.addData(ADDRESS_NODE_2, ";");
    lora.addData("SVE", ";");
    lora.sendDataCb([](const String & sendMsg) {
      Serial.println(sendMsg);
    });
  }

  usbSerial.receive(usbCommunicationTask);
  lora.receiveString(loraCommunicationTask);

  DigitalIn::updateAll(&buttonRst, &buttonBoot, DigitalIn::stop());
  DigitalOut::updateAll(&ledRed, &ledOrange, &ledGreen, DigitalOut::stop());
}

void debug() {
}
