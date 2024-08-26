void onReceiveTelegramMessage(const String& name, const String& message) {
  if (message == "/start") {
    String msg = "Welcome, " + name + ".\n";
    msg += "Please use the following commands to display information:\n\n";
    msg += "/start to show main information.\n";
    msg += "/node1 to show information about node 1.\n";
    msg += "/node2 to show information about node 2.\n";
    msg += "/quantity1 to show quantity about node 1.\n";
    msg += "/quantity2 to show quantity about node 2.\n";
    msg += "/tare1 to tare loadcell in node 1.\n";
    msg += "/tare2 to tare loadcell in node 2.\n";
    msg += "/log1 to show data log in node 1.\n";
    msg += "/log2 to show data log in node 2.\n";
    var.getDataNode1 = IDLE_STATE;
    var.getDataNode2 = IDLE_STATE;
    bot.sendMessages(msg);
  }
  if (message == "/node1") {
    String msg = "[INFO] Please Wait for Node 1 ...\n";
    bot.sendMessages(msg);
    var.getDataNode1 = IDLE_STATE;
    task.delay(2000);
    var.getDataNode1 = DATA_MESSAGE_WAIT;
  }
  if (message == "/node2") {
    String msg = "[INFO] Please Wait for Node 2...\n";
    bot.sendMessages(msg);
    var.getDataNode1 = IDLE_STATE;
    task.delay(2000);
    var.getDataNode2 = DATA_MESSAGE_WAIT;
  }
  if (message == "/quantity1") {
    String msg = "[INFO] Please Wait for Node 1 ...\n";
    bot.sendMessages(msg);
    var.getDataNode1 = IDLE_STATE;
    task.delay(2000);
    var.getDataNode1 = QUANTITY_MESSAGE_WAIT;
  }
  if (message == "/quantity2") {
    String msg = "[INFO] Please Wait for Node 2...\n";
    bot.sendMessages(msg);
    var.getDataNode1 = IDLE_STATE;
    task.delay(2000);
    var.getDataNode2 = QUANTITY_MESSAGE_WAIT;
  }
  if (message == "/tare1") {
    String msg = "[INFO] Please Wait for Node 1 ...\n";
    bot.sendMessages(msg);
    var.getDataNode1 = IDLE_STATE;
    task.delay(2000);
    var.getDataNode1 = TARE_LOADCELL_WAIT;
  }
  if (message == "/tare2") {
    String msg = "[INFO] Please Wait for Node 2...\n";
    bot.sendMessages(msg);
    var.getDataNode1 = IDLE_STATE;
    task.delay(2000);
    var.getDataNode2 = TARE_LOADCELL_WAIT;
  }

  if (message == "/log1") {
    String msg = "[INFO] Please Wait for Node 1 ...\n";
    bot.sendMessages(msg);
    var.getDataNode1 = IDLE_STATE;
    task.delay(2000);
    var.getDataNode1 = FIREBASE_LOAD_DATA;
  }
  if (message == "/log2") {
    String msg = "[INFO] Please Wait for Node 2 ...\n";
    bot.sendMessages(msg);
    var.getDataNode1 = IDLE_STATE;
    task.delay(2000);
    var.getDataNode2 = FIREBASE_LOAD_DATA;
  }
}

void onSendDataToTelegram() {
  //////////////////////// GET DATA ////////////////////////
  if (var.getDataNode1 == DATA_MESSAGE_RECEIVED) {
    var.getDataNode1 = IDLE_STATE;
    String msg = "[SUCCESS] Node1, Status: \n";
    msg += "Temperature: " + String(var.node1.temperature, 1) + " °C\n";
    msg += "Humidity   : " + String(var.node1.humidity, 1) + " %\n";
    msg += "Weight     : " + String(var.node1.weight, 1) + " Kg\n";
    bot.sendMessages(msg);
  }
  if (var.getDataNode2 == DATA_MESSAGE_RECEIVED) {
    var.getDataNode2 = IDLE_STATE;
    String msg = "[SUCCESS] Node2, Status: \n";
    msg += "Weight     : " + String(var.node2.weight, 1) + " Kg\n";
    bot.sendMessages(msg);
  }

  //////////////////////// QUANTITY ////////////////////////
  if (var.getDataNode1 == QUANTITY_MESSAGE_RECEIVED) {
    var.getDataNode1 = IDLE_STATE;
    String msg = "[SUCCESS] Node1, Status: \n";
    msg += "Quantity     : " + String(var.node1.quantity) + " pcs";
    bot.sendMessages(msg);
  }
  if (var.getDataNode2 == QUANTITY_MESSAGE_RECEIVED) {
    var.getDataNode2 = IDLE_STATE;
    String msg = "[SUCCESS] Node2, Status: \n";
    msg += "Quantity     : " + String(var.node2.quantity) + " pcs";
    bot.sendMessages(msg);
  }

  //////////////////////// TARE ////////////////////////
  if (var.getDataNode1 == TARE_LOADCELL_DONE) {
    var.getDataNode1 = IDLE_STATE;
    String msg = "[SUCCESS] Node1, Tare Done \n";
    bot.sendMessages(msg);
  }
  if (var.getDataNode2 == TARE_LOADCELL_DONE) {
    var.getDataNode2 = IDLE_STATE;
    String msg = "[SUCCESS] Node2, Tare Done \n";
    bot.sendMessages(msg);
  }

  //////////////////////// LOG DATA ////////////////////////
  if (var.getDataNode1 == FIREBASE_DATA_READY) {
    String msg = "[SUCCESS] Node1, Log Data: \n";
    for (JsonPair kv : var.node1.doc.as<JsonObject>()) {
      JsonObject data = kv.value().as<JsonObject>();
      String timestamp = data["timestamp"].as<String>();
      timestamp.replace("T", " ");

      msg += "Timestamp  : " + timestamp + "\n";
      msg += "Temperature: " + String(data["temperature"].as<String>().toFloat(), 1) + " °C\n";
      msg += "Humidity   : " + String(data["humidity"].as<String>().toFloat(), 1) + " %\n";
      msg += "Weight     : " + String(data["weight"].as<String>().toFloat(), 1) + " Kg\n";
      msg += "Quantity   : " + data["quantity"].as<String>() + " pcs\n\n";
      // serializeJsonPretty(data, Serial);
    }
    bot.sendMessages(msg);
    var.getDataNode1 = IDLE_STATE;
  }
  if (var.getDataNode2 == FIREBASE_DATA_READY) {
    String msg = "[SUCCESS] Node2, Log Data: \n";
    for (JsonPair kv : var.node2.doc.as<JsonObject>()) {
      JsonObject data = kv.value().as<JsonObject>();
      String timestamp = data["timestamp"].as<String>();
      timestamp.replace("T", " ");

      msg += "Timestamp  : " + timestamp + "\n";
      msg += "Weight     : " + String(data["weight"].as<String>().toFloat(), 1) + " Kg\n";
      msg += "Quantity   : " + data["quantity"].as<String>() + " pcs\n\n";
      // serializeJsonPretty(data, Serial);
    }
    bot.sendMessages(msg);
    var.getDataNode2 = IDLE_STATE;
  }

  //////////////////////// TEMPERATURE WARNING ////////////////////////
  if (var.tempWarningNode1 != var.tempWarningNode1Before) {
    var.tempWarningNode1Before = var.tempWarningNode1;
    if (var.tempWarningNode1) {
      String msg = "[WARNING] Suhu pada Node 1 Tidak Aman. \n";
      msg += "Temperature: " + String(var.node1.temperature, 1) + " °C\n";
      bot.sendMessages(msg);
    }
  }

  //////////////////////// HUMIDITY WARNING ////////////////////////
  if (var.humWarningNode1 != var.humWarningNode1Before) {
    var.humWarningNode1Before = var.humWarningNode1;
    if (var.humWarningNode1) {
      String msg = "[WARNING] Kelembapan pada Node 1 Tidak Aman. \n";
      msg += "Kelembapan: " + String(var.node1.humidity, 1) + " %\n";
      bot.sendMessages(msg);
    }
  }
}