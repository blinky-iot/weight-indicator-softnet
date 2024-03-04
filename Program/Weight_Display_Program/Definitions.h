#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Q2HX711.h>
#include <Preferences.h>
#include <math.h>
#include "BluetoothSerial.h"
#include <TFT_eSPI.h>


TaskHandle_t BackgroundProcessor;

//These are variables for setting up and controlling bluetooth
String bluetoothStatus = "OFF";
String connectBluetooth = "NO"; //If turned to No, background will not connect bluetooth, if turned to yes, background will start connecting to bluetooth.
String deviceName = "Blink weight indicator";

//These are variables for navigating through the screens.
int Screen = 0;
bool ResetScreen = false;

#define TFT_RST 4
#define TFT_CS 15
#define TFT_DC 12
#define TFT_SCLK 14
#define TFT_MOSI 13
#define TFT_LED 27
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,TFT_DC,TFT_MOSI,TFT_SCLK,TFT_RST);

//TFT_eSPI object
TFT_eSPI tftespi = TFT_eSPI();

#define TareButton 18
#define HoldUnitButton 19
#define PowerButton 26
#define ChargingPin 23 //This pin goes high when the scale is being charged.
#define BatteryPin 33 //This is the analog pin from which to read the charge in the battery;

ushort CalibratingWeight = 1; //(known mass)
const ushort CalibrationMassChangeValue = 10; //When the user increments/decrements Calibrating mass, this is value to change with. Eg 10 => 1..10..20..30..40; 20=> 1..20..40..50
String CalibrationStatus = "OFF";

int DisplayBrightness = 50;
const char* NonVolatileBrightnessNamespace = "brightnamespace";
const char* PreferencesBrightnessKey = "brightness";
float LiveWeight = 0;
long TareHxReading = 0; //this is the tare weight in the raw HX711 long format
long CalibrationHxReading = 0; //this is the calibration weight in the raw HX711 long format
float LiveBatteryValue = 0;
bool ZeroTare = false; //When this is set to true, the background core will zero tare the current weight
bool IsCharging = false;

//Preferences Data
//Preferences namespace
const char* NonVolatileDataNamespace = "Weightnamespace";
//Preferences Keys
const char* PreferencesCalibrationKey = "Calibration";
const char* PreferencesHxCalibrationKey = "Calibrationhx";
const char* PreferencesTareWeightKey = "TareWeight";
//Preferences object
Preferences preferences;

//hx711 variables
#define HX711_DATAPIN 16
#define HX711_CLOCKPIN 17

Q2HX711 hx711(HX711_DATAPIN, HX711_CLOCKPIN);

int Tamper = 0;
bool TransmitData = false;
bool ReceiveBufferRead = true;
String TransmitBuffer = "";
String ReceiveBuffer = "";
#define TamperPin 5 //This is the pin for detecting tampering. It is used in the background core
bool Tampered = false;

const char* NonVolatileThemeNameSpace = "ThemeNamespace";
const char* PreferenceThemeKey = "Theme";
String Theme = "Light";
bool ShowBatteryLow = false;
int WeightReadingsGradient = 0;