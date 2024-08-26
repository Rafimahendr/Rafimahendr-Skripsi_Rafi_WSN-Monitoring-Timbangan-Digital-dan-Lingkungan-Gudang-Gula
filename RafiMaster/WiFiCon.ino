void wifiConTask() {
  task.setInitCoreID(1);
  disableLoopWDT();
  disableCore0WDT();
  disableCore1WDT();

  task.createTask(10000, [](void* pvParameter) {
    auth.apiKey = " your apiKey ";
    auth.databaseURL = " your databaseURL ";
    auth.projectID = " your projekID ";

    auth.user.email = " your email firebase ";
    auth.user.password = " your user password firebase ";
 }
    firebase.connectToWiFi(" ", " ");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    firebase.init(&auth);

    var.isWiFiConnected = 1;
    Serial.println(F("| Firebase Init"));
    for (;;) {
      if (firebase.isConnect()) {
        uint32_t epoch = getTime();
        epoch += gmtOffset_sec;
        DateTimeNTP currentTime(epoch);
        static String timestampPrint;
        timestampPrint = currentTime.timestamp();

        static uint8_t secondBefore = 0;
        static uint8_t secondNow = 0;

        uint8_t hoursNow = currentTime.hour();
        uint8_t minutesNow = currentTime.minute();

        secondNow = currentTime.second();
        if (hoursNow == 22 && minutesNow == 0 && secondNow != secondBefore) {
          // if (hoursNow == 9 && minutesNow == 30 && secondNow != secondBefore) {

          if (secondNow >= 0 && secondNow <= 10) {
            var.getDataNode1 = FIREBASE_LOAD_NODE_DATA;
          } else if (secondNow >= 20 && secondNow <= 30) {
            var.getDataNode2 = FIREBASE_LOAD_NODE_DATA;
          } else if (secondNow == 40) {
            firebase.pushJson(
              "/data/node1", [](JsonVariant pushJson) -> JsonVariant {
                pushJson["temperature"] = String(var.node1.temperature, 1);
                pushJson["humidity"] = String(var.node1.humidity, 1);
                pushJson["weight"] = String(var.node1.weight, 1);
                pushJson["quantity"] = String(var.node1.quantity);
                pushJson["timestamp"] = timestampPrint;
                return pushJson;
              },
              FirebaseModule::resultStatusCallback);

            firebase.pushJson(
              "/data/node2", [](JsonVariant pushJson) -> JsonVariant {
                pushJson["weight"] = String(var.node2.weight, 1);
                pushJson["quantity"] = String(var.node2.quantity);
                pushJson["timestamp"] = timestampPrint;
                return pushJson;
              },
              FirebaseModule::resultStatusCallback);
          } else {
            Serial.print(hoursNow);
            Serial.print("|");
            Serial.print(minutesNow);
            Serial.print("|");
            Serial.print(secondNow);
            Serial.print("|");
            Serial.println();
          }
          secondBefore = secondNow;
        }

        if (var.getDataNode1 == FIREBASE_LOAD_DATA) {
          firebase.getJson(
            "/data/node1", [](JsonVariant getJson) {
              var.node1.doc = getJson;
              // for (JsonPair kv : getJson.as<JsonObject>()) {
              //   JsonObject data = kv.value().as<JsonObject>();
              // }
              // serializeJsonPretty(data, Serial);
            },
            FirebaseModule::resultStatusCallback);
          var.getDataNode1 = FIREBASE_DATA_READY;
        }

        if (var.getDataNode2 == FIREBASE_LOAD_DATA) {
          firebase.getJson(
            "/data/node2", [](JsonVariant getJson) {
              var.node2.doc = getJson;
              // for (JsonPair kv : getJson.as<JsonObject>()) {
              //   JsonObject data = kv.value().as<JsonObject>();
              // }
              // serializeJsonPretty(data, Serial);
            },
            FirebaseModule::resultStatusCallback);
          var.getDataNode2 = FIREBASE_DATA_READY;
        }
      }
      task.delay(20);
    }
  });

  task.createTask(10000, [](void* pvParameter) {
    while (!var.isWiFiConnected) {
      ledOrange.toggle();
      delay(100);
    }
    ledOrange.off();

    bot.begin([]() {
      client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
      bot.setChatId(" your ChatID Telegram ");
    });

    String welcomeMsg = "Welcome.\n";
    welcomeMsg += "Please use the following commands to display information:\n\n";
    welcomeMsg += "/start to show main information.\n";
    bot.sendMessages(welcomeMsg);
    Serial.println(F("| Telegram Init"));
    for (;;) {

      bot.onReceiveMessages(2000, [](const String& name, const String& message) {
        Serial.println(message);
        onReceiveTelegramMessage(name, message);
      });

      onSendDataToTelegram();

      static uint32_t ledOrangeTimer;
      if (millis() - ledOrangeTimer >= 2000) {
        ledOrange.on();
        ledOrange.offDelay(150);
        ledOrangeTimer = millis();
      }
      // ledOrange.toggleAsync(250);

      // Serial.print("| var.getDataNode1: ");
      // Serial.print(var.getDataNode1);
      // Serial.print("| var.getDataNode2: ");
      // Serial.print(var.getDataNode2);
      // Serial.println();

      task.delay(20);
    }
  });
}

bool isNumeric(String str) {
  bool decimalPoint = false;
  for (int i = 0; i < str.length(); i++) {
    if (isDigit(str[i])) {
      continue;
    } else if (str[i] == '.' && !decimalPoint) {
      decimalPoint = true;
    } else
      return false;
  }
  return true;
}
