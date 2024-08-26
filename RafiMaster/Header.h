#define ENABLE_MODULE_TIMER_TASK
#define ENABLE_MODULE_SERIAL_HARD
#define ENABLE_MODULE_LORA_COM
#define ENABLE_MODULE_DIGITAL_INPUT
#define ENABLE_MODULE_DIGITAL_OUTPUT
#define ENABLE_MODULE_TELEGRAM_BOT
#define ENABLE_MODULE_TASK_HANDLER
#define ENABLE_MODULE_FIREBASE_HANDLER
#define ENABLE_MODULE_DATETIME_NTP

#include "Kinematrix.h"
#include "Preferences.h"
// #include "WiFiManager.h"

#define ADDRESS_NODE_1 0x01
#define ADDRESS_NODE_2 0x02
#define ADDRESS_NODE_MASTER 0x03

////////// Utility //////////
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;  // Offset for WIB (UTC+7)
const int daylightOffset_sec = 0;

// WiFiManager wm;
TaskHandle task;
FirebaseModule firebase;
FirebaseAuthentication auth;
Preferences preferences;
WiFiClientSecure client;
// TelegramBot bot("7214640769:AAEdnv6otMh-BMCIYlViIJvA_EP3LUj8QW4", client);
TelegramBot bot("7207722695:AAEmFYTXlLEZwH_w98WSSTA9ZgobAjE2fCU", client);
TimerTask tempTimer(3000);

////////// Communication //////////
LoRaModule lora;
HardSerial usbSerial;

////////// Input Module //////////
DigitalIn buttonRst(-1);
DigitalIn buttonBoot(-1);

////////// Output Module //////////
// DigitalOut ledBuiltIn(2);
DigitalOut ledRed(26);
DigitalOut ledOrange(25);
DigitalOut ledGreen(32);

////////// Global Variable //////////
enum SystemEnum {
  IDLE_STATE,
  DATA_MESSAGE_WAIT,
  DATA_MESSAGE_RECEIVED,
  QUANTITY_MESSAGE_WAIT,
  QUANTITY_MESSAGE_RECEIVED,
  TARE_LOADCELL_WAIT,
  TARE_LOADCELL_DONE,
  FIREBASE_LOAD_NODE_DATA,
  FIREBASE_LOAD_DATA,
  FIREBASE_DATA_READY,
};

struct NodeData {
  JsonDocument doc;
  float temperature;
  float humidity;
  float weight;
  int quantity;
};

struct SystemData {
  NodeData node1;
  NodeData node2;
  int thisDeviceAddress;
  int isWiFiConnected = 0;
  int getDataNode1 = 0;
  int getDataNode1Before = 0;
  int getDataNode2 = 0;
  int getDataNode2Before = 0;
  int tempWarningNode1 = 0;
  int tempWarningNode1Before = 0;
  int humWarningNode1 = 0;
  int humWarningNode1Before = 0;
};

SystemData var;
