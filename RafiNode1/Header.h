#define ENABLE_MODULE_TIMER_TASK
#define ENABLE_MODULE_SERIAL_HARD
#define ENABLE_MODULE_LORA_COM
#define ENABLE_MODULE_DIGITAL_INPUT
#define ENABLE_MODULE_DIGITAL_OUTPUT
#define ENABLE_MODULE_LCD_MENU

#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY
#define ENABLE_SENSOR_DHT
#define ENABLE_SENSOR_HX711

#include "Kinematrix.h"

#define ADDRESS_NODE_1 0x01
#define ADDRESS_NODE_2 0x02
#define ADDRESS_NODE_MASTER 0x03

////////// Utility //////////
TimerTask eepromTimer(30000);
EEPROMLib eeprom;

////////// Sensor //////////
SensorModule sensor;
MovingAverageFilter filter(3);

////////// Communication //////////
LoRaModule lora;
HardSerial usbSerial;

////////// Input Module //////////
DigitalIn buttonRst(-1);
DigitalIn buttonBoot(-1);

////////// Output Module //////////
LcdMenu menu(0x27, 16, 2);

////////// Global Variable //////////
struct SystemData {
  int thisDeviceAddress;
  float temperature;
  float humidity;
  float weight;
  float weightOffset;
  float calibrationFactor;
  bool isCalibrate;
};

SystemData var;