#include "Definitions.h"
#include "Security.h"

class MemoryLoader
{
  public:
  void SetFlashMemorySavedVariables()
  {
    preferences.begin(NonVolatileDataNamespace,true);
    CalibratingWeight = preferences.getLong(PreferencesCalibrationKey,0);
    CalibrationHxReading = preferences.getLong(PreferencesHxCalibrationKey,0);
    TareHxReading = preferences.getLong(PreferencesTareWeightKey,0);
    preferences.end();
  }
  void SetTheme()
  {
    preferences.begin(NonVolatileThemeNameSpace,true);
    Theme = preferences.getString(PreferenceThemeKey,"Dark");
    preferences.end();
    Serial.println(Theme);
  }
  void SetDisplayBrightness()
  {
    preferences.begin(NonVolatileBrightnessNamespace,true);
    DisplayBrightness = preferences.getInt(PreferencesBrightnessKey,50);
    preferences.end();
  }
};
class Initializer
{
  protected:
  byte HOLDPIN = 25;
  public:
  void InitializeSystem()
  {
    InitializeHoldPin();
    InitializeBatteryAndChargingPin();
    InitializeButtons();
    InitializeScreen();
    InitializeTFTeSPI();
  }
  private:
  void InitializeHoldPin()
  {
    pinMode(HOLDPIN,OUTPUT);
    digitalWrite(HOLDPIN,HIGH);
  }
  void InitializeBatteryAndChargingPin()
  {
    pinMode(ChargingPin,INPUT);
    pinMode(BatteryPin,INPUT);
  }
  void InitializeScreen()
  {
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(3);
    pinMode(TFT_LED,OUTPUT);
    analogWrite(TFT_LED,DisplayBrightness);
  }
  void InitializeButtons()
  {
    pinMode(TareButton,INPUT);
    pinMode(HoldUnitButton,INPUT);
    pinMode(PowerButton,INPUT);
  }
  void InitializeTFTeSPI()
  {
    tftespi.init();
    tftespi.setRotation(5);
  }
};
class PowerOnScreen
{
  int status = 0;
  bool displayingPowerOnMessage = false;
  ulong powerOnDisplayTime = 3000; //This is the time it will take to display the power on message
  ulong powerOnDisplayStartTime = millis();

  bool displayingConnectingMessage = false;
  ulong connectingMessageDotsTime = 1000; //This is the time for the number of dots being displayed to change.
  ulong connectingMessageDotsLastStatusChangeTime = millis(); //This is the last time the number of dots being displayed was changed
  int dotsToDisplay = 1;

  bool displayingConnectedMessage = false;
  public:
  void Display()
  {
    if(status == 0)
    {
      if(millis() - powerOnDisplayStartTime >= powerOnDisplayTime) 
      {
        status = 1;
        connectBluetooth = "YES";
        return;
      }
      else if(!displayingPowerOnMessage)
      {
        if(Theme == "Dark")
        {
          tftespi.fillRect(0,0,160,128,TFT_BLACK);
          tftespi.setTextColor(TFT_GREEN);
        }
        else if(Theme == "Light")
        {
          tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
          tftespi.setTextColor(TFT_BLACK);
        }
        tftespi.setTextSize(2);
        tftespi.drawString("POWER ON",20,50,2);
        displayingPowerOnMessage = true;
      }
      else return;
    }
    else if(status == 1)
    {
      if(digitalRead(PowerButton)) Screen = 7;
      if(bluetoothStatus == "OFF")
      {
        if(displayingConnectedMessage) displayingConnectedMessage = false;
        if(!displayingConnectingMessage)
        {
          if(Theme == "Dark")
          {
            tftespi.fillRect(0,0,160,128,TFT_BLACK);
            tftespi.setTextColor(TFT_GREEN);
            tftespi.setTextSize(2);
            tftespi.drawString("CONNECTING",25,40,1);
            tftespi.drawString("TO BLUETOOTH",10,60,1);
            tftespi.drawString(".",60,90,1);
          }
          else if(Theme == "Light")
          {
            tftespi.fillRect(0,0,160,128,TFT_WHITE);
            tftespi.setTextColor(TFT_BLACK);
            tftespi.setTextSize(2);
            tftespi.drawString("CONNECTING",25,40,1);
            tftespi.drawString("TO BLUETOOTH",10,60,1);
            tftespi.drawString(".",60,90,1);
          }
          dotsToDisplay = 2;
          displayingConnectingMessage = true;
          connectingMessageDotsLastStatusChangeTime = millis();
        }
        else
        {
          if(millis() - connectingMessageDotsLastStatusChangeTime >= connectingMessageDotsTime)
          {
            if(Theme == "Dark")
            {
              tftespi.fillRect(55, 90, 50, 25, TFT_BLACK);
              tftespi.setTextColor(TFT_GREEN);
            }
            else if(Theme == "Light")
            {
              tftespi.fillRect(55, 90, 50, 25, TFT_WHITE);
              tftespi.setTextColor(TFT_BLACK);
            }
            tftespi.setTextSize(2);
            switch(dotsToDisplay)
            {
              case 0:
              tftespi.drawString("",60,90,1);
              break;
              case 1:
              tftespi.drawString(".",60,90,1);
              break;
              case 2:
              tftespi.drawString("..",60,90,1);
              break;
              case 3:
              tftespi.drawString("...",60,90,1);
              break;
              default:
              tftespi.drawString(".",60,90,1);
              dotsToDisplay = 2;
              break;
            }
            dotsToDisplay ++;
            if(dotsToDisplay > 3) dotsToDisplay = 0;
            connectingMessageDotsLastStatusChangeTime = millis();
          }
        }
      }
      else if(bluetoothStatus == "UNAUTHORISED")
      {
        if(displayingConnectingMessage) displayingConnectingMessage = false;
        ResetScreen = true;
        Screen = 1;
      }
      else if(bluetoothStatus == "ON")
      {
        if(displayingConnectingMessage) displayingConnectingMessage = false;
        if(!displayingConnectedMessage)
        {
          if(Theme == "Dark")
          {
            tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
            tftespi.setTextColor(TFT_GREEN);
          }
          else if(Theme == "Light")
          {
            tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
            tftespi.setTextColor(TFT_BLACK);
          }
          tftespi.setTextSize(2);
          tftespi.drawString("CONNECTED", 30, 40, 1);
          displayingConnectedMessage = true;
          delay(1000);
          if(CalibrationHxReading == 0) 
          {
            ResetScreen = true;
            Screen = 2;
          }
          else 
          {
            ResetScreen = true;
            Screen = 5;
          }
        }
      }
    }
  }
};
class UnauthorizedScreen
{
  bool displayedUnauthorizedMessage = false;
  bool bluetoothOffStartedTiming = false;
  uint bluetoothOffTimeout = 1000;
  long bluetoothOffStartTime = millis();
  public:
  void Display()
  {
    if(digitalRead(PowerButton)) Screen = 7;
    if(bluetoothStatus == "ON") //bluetooth status is on, go to next screen
    {
      if(CalibrationHxReading == 0) 
      {
        ResetScreen = true;
        Screen = 2;
      }
      else 
      {
        ResetScreen = true;
        Screen = 5;
      }
    }
    else if(bluetoothStatus == "OFF")
    {
      if(!bluetoothOffStartedTiming)
      {
        bluetoothOffStartTime = millis();
        bluetoothOffStartedTiming = true;
      }
      else if(millis() - bluetoothOffStartTime > bluetoothOffTimeout)
      {
        Reset();
        Screen = 1;
      }
    }
    else bluetoothOffStartTime = millis();
    if(!displayedUnauthorizedMessage)
    {
      if(Theme == "Dark") tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
      else if (Theme == "Light") tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
      tftespi.fillRect(15,61,20,6,TFT_RED);
      tftespi.fillRect(65,61,20,6,TFT_RED);
      tftespi.fillRect(115,61,20,6,TFT_RED);
      displayedUnauthorizedMessage = true;
    }
  }
  void Reset()
  {
    displayedUnauthorizedMessage = false;
    bluetoothOffStartedTiming = false;
  }
};
class UnCalibratedScreen
{
  bool displayingUnCalibratedMessage = false;
  
  public:
  void Display()
  {
    if(!digitalRead(TareButton)) 
    {
      Screen = 3; 
      Reset();
    }
    if(!displayingUnCalibratedMessage)
    {
      if(Theme == "Dark")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
        tftespi.setTextSize(2);
        tftespi.setTextColor(TFT_GREEN);
        tftespi.drawString("System not", 5, 10, 1);
        tftespi.drawString("Calibrated", 5, 35, 1);
        tftespi.drawString("Press    to", 5, 65, 1);
        tftespi.drawString("Calibrate", 5, 90, 1);
        //Draw tare button
        tftespi.setTextSize(1);
        tftespi.drawString("TARE", 75, 65, 1);
        tftespi.drawString("ZERO", 75, 75, 1);
        tftespi.drawLine(75, 73, 100, 73, TFT_GREEN);
        tftespi.drawRect(70, 60, 34, 26, TFT_RED);
        //Tare button drawn
      }
      else if(Theme == "Light")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
        tftespi.setTextSize(2);
        tftespi.setTextColor(TFT_BLACK);
        tftespi.drawString("System not", 5, 10, 1);
        tftespi.drawString("Calibrated", 5, 35, 1);
        tftespi.drawString("Press    to", 5, 65, 1);
        tftespi.drawString("Calibrate", 5, 90, 1);
        //Draw tare button
        tftespi.setTextSize(1);
        tftespi.drawString("TARE", 75, 65, 1);
        tftespi.drawString("ZERO", 75, 75, 1);
        tftespi.drawLine(75, 73, 100, 73, TFT_GREEN);
        tftespi.drawRect(70, 60, 34, 26, TFT_RED);
        //Tare button drawn
      }
      displayingUnCalibratedMessage = true;
    }
  }
  void Reset()
  {
    displayingUnCalibratedMessage = false;
  }
};
class CalibratingMassScreen
{
  bool displayingCalibratingMassEntry = false;
  ulong tareButtonLastPressTime = millis();
  ulong holdUnitButtonLastPressTime = millis();
  ushort buttonsRefreshTime = 200;
  public:
  void Display()
  {
    if(!digitalRead(TareButton))
    {
      if(millis() - tareButtonLastPressTime >= buttonsRefreshTime)
      {
        if(CalibratingWeight == 1) CalibratingWeight += CalibrationMassChangeValue - 1;
        else CalibratingWeight += CalibrationMassChangeValue;
        tareButtonLastPressTime = millis();
        DisplayCalibrationMassValue();
      }
    }
    else if(!digitalRead(HoldUnitButton))
    {
      if(millis() - holdUnitButtonLastPressTime >= buttonsRefreshTime)
      {
        if(CalibratingWeight < (1 + CalibrationMassChangeValue)) CalibratingWeight = 1;
        else CalibratingWeight -= CalibrationMassChangeValue;
        holdUnitButtonLastPressTime = millis();
        DisplayCalibrationMassValue();
      }
    }
    else if(digitalRead(PowerButton)) 
    {
      Screen = 4;
      ResetScreen = true;
      Reset();
      CalibrationStatus = "WAITING";
    }
    if(!displayingCalibratingMassEntry)
    {
      if (Theme == "Dark")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
        tftespi.setTextColor(TFT_GREEN);
        tftespi.setTextSize(2);
        tftespi.drawString("Enter", 45, 5, 1);
        tftespi.drawString("calibrating", 10, 25, 1);
        tftespi.drawString("mass", 45, 45, 1);
        DisplayCalibrationMassValue();
        //Draw increase, decrease and enter icons

        //Increase
        tftespi.drawLine(76, 110, 76, 123, TFT_WHITE);
        tftespi.drawLine(75, 110, 75, 123, TFT_WHITE);
        tftespi.drawLine(75, 110, 70, 115, TFT_WHITE);
        tftespi.drawLine(76, 111, 71, 116, TFT_WHITE);
        tftespi.drawLine(77, 110, 82, 115, TFT_WHITE);
        tftespi.drawLine(76, 111, 81, 116, TFT_WHITE);

        //Decrease
        tftespi.drawLine(10, 110, 10, 123, TFT_WHITE);
        tftespi.drawLine(11, 110, 11, 123, TFT_WHITE);
        tftespi.drawLine(10, 123, 5, 118, TFT_WHITE);
        tftespi.drawLine(11, 122, 6, 117, TFT_WHITE);
        tftespi.drawLine(12, 123, 17, 118, TFT_WHITE);
        tftespi.drawLine(11, 122, 16, 117, TFT_WHITE);

        //Enter
        tftespi.drawLine(150, 110, 150, 117, TFT_WHITE);
        tftespi.drawLine(151, 110, 151, 117, TFT_WHITE);
        tftespi.drawLine(150, 117, 135, 117, TFT_WHITE);
        tftespi.drawLine(150, 118, 135, 118, TFT_WHITE);
        tftespi.drawLine(135, 117, 140, 112, TFT_WHITE);
        tftespi.drawLine(136, 117, 141, 112, TFT_WHITE);
        tftespi.drawLine(135, 118, 140, 123, TFT_WHITE);
        tftespi.drawLine(136, 118, 141, 123, TFT_WHITE);
      }
      else if (Theme == "Light") 
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
        tftespi.setTextColor(TFT_BLACK);
        tftespi.setTextSize(2);
        tftespi.drawString("Enter", 45, 5, 1);
        tftespi.drawString("calibrating", 10, 25, 1);
        tftespi.drawString("mass", 45, 45, 1);
        DisplayCalibrationMassValue();
        //Draw increase, decrease and enter icons

        //Increase
        tftespi.drawLine(76, 110, 76, 123, TFT_BLACK);
        tftespi.drawLine(75, 110, 75, 123, TFT_BLACK);
        tftespi.drawLine(75, 110, 70, 115, TFT_BLACK);
        tftespi.drawLine(76, 111, 71, 116, TFT_BLACK);
        tftespi.drawLine(77, 110, 82, 115, TFT_BLACK);
        tftespi.drawLine(76, 111, 81, 116, TFT_BLACK);

        //Decrease
        tftespi.drawLine(10, 110, 10, 123, TFT_BLACK);
        tftespi.drawLine(11, 110, 11, 123, TFT_BLACK);
        tftespi.drawLine(10, 123, 5, 118, TFT_BLACK);
        tftespi.drawLine(11, 122, 6, 117, TFT_BLACK);
        tftespi.drawLine(12, 123, 17, 118, TFT_BLACK);
        tftespi.drawLine(11, 122, 16, 117, TFT_BLACK);

        //Enter
        tftespi.drawLine(150, 110, 150, 117, TFT_BLACK);
        tftespi.drawLine(151, 110, 151, 117, TFT_BLACK);
        tftespi.drawLine(150, 117, 135, 117, TFT_BLACK);
        tftespi.drawLine(150, 118, 135, 118, TFT_BLACK);
        tftespi.drawLine(135, 117, 140, 112, TFT_BLACK);
        tftespi.drawLine(136, 117, 141, 112, TFT_BLACK);
        tftespi.drawLine(135, 118, 140, 123, TFT_BLACK);
        tftespi.drawLine(136, 118, 141, 123, TFT_BLACK);
      }
      //End of icons drawings.
      displayingCalibratingMassEntry = true;
      tareButtonLastPressTime = millis() + 2000; //Since to get to this screen the tare button is being held, a delay of 1000 is being added to prevent confusion with increment of Calibrating mass value
    }
  }
  void Reset()
  {
    displayingCalibratingMassEntry = false;
    buttonsRefreshTime = 200;
  }
  private:
  void DisplayCalibrationMassValue()
  {
    //Start of drawing input field for Calibrating mass
    int textBoxWidth = 70; //The width of the red rectangle being drawn
    int textBoxHeight = 35; //The height of the red rectangle being drawn
    if(CalibratingWeight >= 1000) textBoxWidth += 20;
    else if(CalibratingWeight >= 100) textBoxWidth += 10;
    
    if (Theme == "Dark")
    {
      tftespi.fillRect(39, 69, textBoxWidth + 50, textBoxHeight + 1, TFT_BLACK);
      tftespi.drawRect(40, 70, textBoxWidth, textBoxHeight, TFT_BLUE);
      tftespi.setTextColor(TFT_WHITE);
    }
    else if (Theme == "Light")
    {
      tftespi.fillRect(39, 69, textBoxWidth + 50, textBoxHeight + 1, TFT_WHITE);
      tftespi.drawRect(40, 70, textBoxWidth, textBoxHeight, TFT_BLUE);
      tftespi.setTextColor(TFT_BLACK);
    }
    tftespi.setTextSize(2);
    if(CalibratingWeight > 99) tftespi.drawString(String(CalibratingWeight) + " kg", 45, 78, 1);
    else if(CalibratingWeight > 9) tftespi.drawString(String(CalibratingWeight) + " kg", 48, 78, 1);
    else tftespi.drawString(String(CalibratingWeight) + " kg", 50, 78, 1);
    //End of drawing input field for Calibrating mass
  }
};
class CalibratingScreen
{
  private:
  bool displayedContent = false;
  int displayStatus = -1;
  ulong calibrationCompleteDisplayStartTime = millis();
  uint calibrationCompleteDisplayDuration = 1000;
  public:
  void Display()
  {
    if(CalibrationStatus == "WAITING") 
    {
      if(displayStatus != 0) 
      {
        displayStatus = 0; 
        displayedContent = false; 
      }
    }
    else if(CalibrationStatus == "CALIBRATING") 
    {
      if(displayStatus != 1) 
      { 
        displayStatus = 1;
        displayedContent = false;
      }
    }
    else if(CalibrationStatus == "CALIBRATED") 
    {
      if(displayStatus != 2) 
      {
        displayStatus = 2;
        displayedContent = false;
        calibrationCompleteDisplayStartTime =  millis();
      }
    }
    if(displayStatus == 0 && !displayedContent)
    {
      if (Theme == "Dark")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
        tftespi.setTextColor(TFT_GREEN);
      }
      else if (Theme == "Light")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
        tftespi.setTextColor(TFT_BLACK);
      }
      tftespi.setTextSize(2);
      tftespi.drawString("Waiting for", 10, 30, 1);
      tftespi.drawString("mass", 50, 50, 1);
      displayedContent = true;
    }
    else if(displayStatus == 1 && !displayedContent)
    {
      if (Theme == "Dark")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
        tftespi.setTextColor(TFT_GREEN);
      }
      else if (Theme == "Light")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
        tftespi.setTextColor(TFT_BLACK);
      }
      tftespi.setTextSize(2);
      tftespi.drawString("Calibrating", 10, 60, 1);
      displayedContent = true;
    }
    else if(displayStatus == 2)
    {
      if(!displayedContent)
      {
        if (Theme == "Dark")
        {
          tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
          tftespi.setTextColor(TFT_GREEN);
        }
        else if (Theme == "Light")
        {
          tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
          tftespi.setTextColor(TFT_BLACK);
        }
        tftespi.setTextSize(2);
        tftespi.drawString("Calibration", 10, 40, 1);
        tftespi.drawString("complete", 30, 60, 1);
        displayedContent = true;
      }
      if(millis() - calibrationCompleteDisplayStartTime >= calibrationCompleteDisplayDuration) 
      {
        CalibrationStatus ="OFF";
        Reset();
        Screen = 5;
        ResetScreen = true;
      }
    }
  }
  void Reset()
  {
    displayedContent = false;
    displayStatus = -1;
    calibrationCompleteDisplayDuration = 1000;
  }
};
class MainScreen
{
  private:
  bool displayedContent = false;
  //Variables for displayed values
  float displayedWeight = 0;
  float displayedBatteryValue = 0;
  bool displayedBluetoothConnected = false;
  bool displayedCharging = false;
  long transmitDelayTime = 10;
  long transmitStartTime = 0;
  long powerOffButtonDelayTime = 4000;
  long powerOffPressTime = millis();
  bool bluetoothStatusOffDisplayCountDown = false;
  ushort bluetoothStatusOffDisplayCountDownDuration = 3000;
  long bluetoothStatusOffDisplayCountDownStartTime = 0;
  bool showingLowBattery = false;
  long PowerButtonLastPressTime = millis();
  int PowerButtonRepressDuration = 1000;
  public:
  void Display()
  {
    if(bluetoothStatus == "UNAUTHORISED")
    {
      ResetScreen = true;
      Screen = 1;
      WaitingForSecurityReset = false;
      return;
    }
    if (!displayedContent) 
    {
      powerOffPressTime = millis();
      if(Theme == "Dark") tftespi.fillScreen(TFT_BLACK);
      else if(Theme == "Light") tftespi.fillScreen(TFT_WHITE);
      DisplayLiveWeight();
      DisplayUnits();
      ShowBluetoothIcon();
      ShowBattery();
      UpdateBatteryDisplay(LiveBatteryValue);
      ShowBluetoothDisconnectedIcon();
      if (IsCharging) ShowCharging();
      displayedContent = true;
    }
    else
    {
      if(!ReceiveBufferRead)
      {
        if(ReceiveBuffer == "Calibrate@blink") 
        {
          ReceiveBuffer = "";
          Screen = 6;
          ResetScreen = true;
          ReceiveBufferRead = true;
          return;
        }
        ReceiveBufferRead = true;
      }
      if(displayedWeight != LiveWeight) DisplayLiveWeight();
      //if(displayedBatteryValue != LiveBatteryValue) UpdateBatteryDisplay(LiveBatteryValue);
      if((ShowBatteryLow && !showingLowBattery) || showingLowBattery)
      {
        ShowLowBattery();
        showingLowBattery = true;
      }
      if(!displayedBluetoothConnected && bluetoothStatus == "ON") ShowBluetoothConnectedIcon();
      else if(displayedBluetoothConnected && bluetoothStatus == "OFF") 
      {
        if(!bluetoothStatusOffDisplayCountDown)
        {
          bluetoothStatusOffDisplayCountDownStartTime = millis();
          bluetoothStatusOffDisplayCountDown = true;
        }
        else
        {
          if(millis() - bluetoothStatusOffDisplayCountDownDuration >= bluetoothStatusOffDisplayCountDownStartTime)
          {
            bluetoothStatusOffDisplayCountDown = false;
            ShowBluetoothDisconnectedIcon();
          }
        }
      }
      if (!displayedCharging && IsCharging) ShowCharging();
      else if (displayedCharging && !IsCharging) HideCharging();
    }
    if(millis() - transmitStartTime > transmitDelayTime)
    {
      //TransmitBuffer = "{\"Weight\":" + String(LiveWeight) + ",\"Tamper\":" + String(Tamper) + "}";
      TransmitBuffer = String(LiveWeight) + "\t" + String(Tamper) + "\t" + String(float(LiveBatteryValue)) + "\n";
      TransmitData = true;
      transmitStartTime = millis();
    }
    if(!digitalRead(TareButton)) ZeroTare = true;
    if(digitalRead(PowerButton) && millis() - PowerButtonLastPressTime >= PowerButtonRepressDuration) Screen = 7;
    if(!digitalRead(HoldUnitButton)) 
    {
      Screen = 8;
      ResetScreen = true;
    }
  }
  void Reset()
  {
    displayedContent = false;
    displayedWeight = 0;
    displayedBatteryValue = 0;
    displayedBluetoothConnected = false;
    displayedCharging = false;
    transmitDelayTime = 10;
    transmitStartTime = 0;
    PowerButtonLastPressTime = millis();
    if(Theme == "Dark") tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
    else if(Theme == "Light") tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
  }
  private:
  void DisplayUnits()
  {
    tftespi.setTextSize(1);
    tftespi.setTextColor(TFT_MAGENTA);
    tftespi.drawString("KG",5,5,2);
  }
  void DisplayLiveWeight()
  {
    if(Theme == "Dark")
    {
      tftespi.fillRect(0, 18, 160, 100, TFT_BLACK);
      tftespi.setTextColor(TFT_WHITE);
      tftespi.setTextSize(1);
      tftespi.drawString(FormatWeightOutput(LiveWeight), 10, 50, 7);
    }
    else if(Theme == "Light")
    {
      tftespi.fillRect(0, 18, 160, 100, TFT_WHITE);
      tftespi.setTextColor(TFT_BLACK);
      tftespi.setTextSize(1);
      tftespi.drawString(FormatWeightOutput(LiveWeight), 10, 50, 7);
    }
    displayedWeight = LiveWeight;
  }
  String FormatWeightOutput(float weight)
  {
    weight *= 10;
    float dispWeight = round(weight);
    String dispString = String(dispWeight / 10);
    String output = "";
    int j = 0;
    for(int i = 0; i < dispString.length();i++)
    {
      if(j > 1) break;
      output += dispString[i];
      if(dispString[i] == '.' || j != 0) j++;
    }
    return output;
  }
  void ShowBluetoothIcon()
  {
    if(Theme == "Dark")
    {
      tftespi.drawLine(130, 4, 130, 17, TFT_WHITE);
      tftespi.drawLine(130, 4, 135, 7, TFT_WHITE);
      tftespi.drawLine(135, 7, 126, 12, TFT_WHITE);
      tftespi.drawLine(130, 17, 135, 14, TFT_WHITE);
      tftespi.drawLine(135, 14, 126, 8, TFT_WHITE);
    }
    else if(Theme == "Light")
    {
      tftespi.drawLine(130, 4, 130, 17, TFT_BLACK);
      tftespi.drawLine(130, 4, 135, 7, TFT_BLACK);
      tftespi.drawLine(135, 7, 126, 12, TFT_BLACK);
      tftespi.drawLine(130, 17, 135, 14, TFT_BLACK);
      tftespi.drawLine(135, 14, 126, 8, TFT_BLACK);
    }
  }
  void ShowBluetoothConnectedIcon()
  {
    ClearBluetoothStatusIcons();
    if(Theme == "Dark")
    {
      tftespi.drawLine(140, 12, 143, 14, TFT_GREEN);
      tftespi.drawLine(143, 14, 146, 8, TFT_GREEN);
    }
    else if(Theme == "Light")
    {
      tftespi.drawLine(140, 12, 143, 14, TFT_BLACK);
      tftespi.drawLine(143, 14, 146, 8, TFT_BLACK);
    }
    displayedBluetoothConnected = true;
  }
  void ShowBluetoothDisconnectedIcon()
  {
    ClearBluetoothStatusIcons();
    tftespi.drawLine(140, 8, 146, 14, TFT_BLUE);
    tftespi.drawLine(146, 8, 140, 14, TFT_BLUE);
    displayedBluetoothConnected = false;
  }
  void ClearBluetoothStatusIcons()
  {
    if(Theme == "Dark") tftespi.fillRect(139, 0, 10, 20, TFT_BLACK);
    else if(Theme == "Light") tftespi.fillRect(139, 0, 10, 20, TFT_WHITE);
  }
  void ShowBattery()
  {
    if(Theme == "Dark")
    {
      tftespi.drawRect(90, 4, 8, 14, TFT_WHITE);
      tftespi.drawRect(92, 2, 4, 2, TFT_WHITE);
    }
    else if(Theme == "Light")
    {
      tftespi.drawRect(90, 4, 8, 14, TFT_BLACK);
      tftespi.drawRect(92, 2, 4, 2, TFT_BLACK);
    }
  }
  void ShowLowBattery()
  {
    tftespi.fillRect(90, 4, 8, 14, TFT_BLUE);
    tftespi.fillRect(92, 2, 4, 2, TFT_BLUE);
  }
  void UpdateBatteryDisplay(float Percentage)
  {
    if(Percentage > 100) Percentage = 100;
    if(Theme == "Dark")
    {
      tftespi.fillRect(91, 5, 6, 12, TFT_BLACK);
      tftespi.fillRect(99, 2, 25, 15, TFT_BLACK);
    }
    else if(Theme == "Light")
    {
      tftespi.fillRect(91, 5, 6, 12, TFT_WHITE);
      tftespi.fillRect(99, 2, 25, 15, TFT_WHITE);
    }
    if(Percentage > 20 || IsCharging)
    {
      int height = Percentage/100 * 13;
      tftespi.fillRect(91, 17 - height, 6, height, TFT_GREEN);
      if(Theme == "Dark") tftespi.setTextColor(TFT_GREEN);
      else if(Theme == "Light") tftespi.setTextColor(TFT_BLACK);
    }
    else
    {
      int height = round(Percentage/100 * 8);
      tftespi.fillRect(91, 17 - height, 6, height, TFT_BLUE);
      tftespi.setTextColor(TFT_BLUE);
    }
    return;
    int displayPercentage = round(Percentage);
    tftespi.setTextSize(1);
    tftespi.setCursor(100, 7);
    tftespi.println(String(displayPercentage) + "%");
    displayedBatteryValue = LiveBatteryValue;
  }
  void ShowCharging()
  {
    if(Theme == "Dark")
    {
      tftespi.fillRect(74, 5, 9, 7, TFT_WHITE);
      tftespi.drawLine(76, 2, 76, 5, TFT_WHITE);
      tftespi.drawLine(80, 2, 80, 5, TFT_WHITE);
      tftespi.fillCircle(78, 10, 4, TFT_WHITE);
      tftespi.drawLine(78, 10, 78, 17, TFT_WHITE);
    }
    else if(Theme == "Light")
    {
      tftespi.fillRect(74, 5, 9, 7, TFT_BLACK);
      tftespi.drawLine(76, 2, 76, 5, TFT_BLACK);
      tftespi.drawLine(80, 2, 80, 5, TFT_BLACK);
      tftespi.fillCircle(78, 10, 4, TFT_BLACK);
      tftespi.drawLine(78, 10, 78, 17, TFT_BLACK);
    }
    displayedCharging = true;
  }
  void HideCharging()
  {
    if(Theme == "Dark") tftespi.fillRect(73, 0, 11, 18, TFT_BLACK);
    if(Theme == "Light") tftespi.fillRect(73, 0, 11, 18, TFT_WHITE);
    displayedCharging = false;
  }
};
class ConfirmCalibrateScreen
{
  bool displayedContent = false;
  public:
  void Display()
  {
    if(!displayedContent)
    {
      if(Theme == "Dark")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
        tftespi.setTextColor(TFT_WHITE);
        tftespi.drawString("Do you want",5,30,4);
        tftespi.drawString("to calibrate?",5,60,4);
        //Yes
        tftespi.drawRect(5, 105, 30, 20, TFT_WHITE);
        tftespi.textsize = 1;
        tftespi.setTextColor(TFT_GREEN);
        tftespi.drawString("YES", 8, 107, 2);
        //No
        tftespi.drawRect(56, 105, 30, 20, TFT_WHITE);
        tftespi.setTextColor(TFT_BLUE);
        tftespi.drawString("NO", 63, 107, 2);
        displayedContent = true;
      }
      else if(Theme == "Light")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
        tftespi.setTextColor(TFT_BLACK);
        tftespi.drawString("Do you want",5,30,4);
        tftespi.drawString("to calibrate?",5,60,4);
        //Yes
        tftespi.drawRect(5, 105, 30, 20, TFT_GREEN);
        tftespi.textsize = 1;
        tftespi.setTextColor(TFT_BLACK);
        tftespi.drawString("YES", 8, 107, 2);
        //No
        tftespi.drawRect(56, 105, 30, 20, TFT_BLUE);
        tftespi.setTextColor(TFT_BLACK);
        tftespi.drawString("NO", 63, 107, 2);
        displayedContent = true;
      }
    }
    if(!digitalRead(HoldUnitButton))
    {
      Screen = 3;
      ResetScreen = true;
    }
    else if(!digitalRead(TareButton))
    {
      Screen = 5;
      ResetScreen = true;
    }
  }
  void Reset()
  {
    displayedContent = false;
  }
};
class PowerOffScreen:public Initializer
{
  public:
  void Display()
  {
    if (Theme == "Dark")
    {
      tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
      tftespi.setTextColor(TFT_BLUE);
    }
    else if (Theme == "Light")
    {
      tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
      tftespi.setTextColor(TFT_BLUE);
    }
    tftespi.setTextSize(2);
    tftespi.drawString("POWERING OFF", 10, 50, 1);
    delay(3000);
    digitalWrite(HOLDPIN, LOW);
  }
};
class SettingsScreen
{
  bool displayingContent = false;
  int selectedItem = 0,previousSelectedItem = 0;
  long HoldUnitButtonLastPressTime = millis();
  long TareButtonLastPressTime = millis();
  long PowerButtonLastPressTime = millis();
  int HoldUnitButtonRepressDuration = 500;
  int TareButtonRepressDuration = 500;
  int PowerButtonRepressDuration = 1000;
  public:
  void Display()
  {
    if(!digitalRead(HoldUnitButton) && selectedItem < 2 && millis() - HoldUnitButtonLastPressTime >= HoldUnitButtonRepressDuration)
    {
      selectedItem ++;
      HoldUnitButtonLastPressTime = millis();
    }
    else if(!digitalRead(TareButton) && selectedItem > 0 && millis() - TareButtonLastPressTime >= TareButtonRepressDuration)
    {
      selectedItem --;
      TareButtonLastPressTime = millis();
    }
    else if(digitalRead(PowerButton) && millis() - PowerButtonLastPressTime >= PowerButtonRepressDuration)
    {
      switch(selectedItem)
      {
        case 0:
          Screen = 10;
          ResetScreen = true;
          break;
        case 1:
          Screen = 9;
          ResetScreen = true;
          break;
        case 2:
          Screen = 5;
          ResetScreen = true;
          break;
      }
      return;
    }
    if(!displayingContent)
    {
      if(Theme == "Dark")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
        tftespi.setTextSize(2);
        tftespi.setTextColor(TFT_GREEN);
        tftespi.drawString("SETTINGS",30,5,1);
        tftespi.fillRect(0, 25, 128, 25, TFT_YELLOW);
        tftespi.setTextColor(TFT_WHITE);
        tftespi.drawString("Brightness",5,30,1);
        tftespi.drawString("Theme",5,60,1);
        tftespi.drawString("Back",5,90,1);
        //Draw up, down and enter icons

        //Up
        tftespi.drawLine(76, 110, 76, 123, TFT_WHITE);
        tftespi.drawLine(75, 110, 75, 123, TFT_WHITE);
        tftespi.drawLine(75, 110, 70, 115, TFT_WHITE);
        tftespi.drawLine(76, 111, 71, 116, TFT_WHITE);
        tftespi.drawLine(77, 110, 82, 115, TFT_WHITE);
        tftespi.drawLine(76, 111, 81, 116, TFT_WHITE);

        //Down
        tftespi.drawLine(10, 110, 10, 123, TFT_WHITE);
        tftespi.drawLine(11, 110, 11, 123, TFT_WHITE);
        tftespi.drawLine(10, 123, 5, 118, TFT_WHITE);
        tftespi.drawLine(11, 122, 6, 117, TFT_WHITE);
        tftespi.drawLine(12, 123, 17, 118, TFT_WHITE);
        tftespi.drawLine(11, 122, 16, 117, TFT_WHITE);

        //Enter
        tftespi.drawLine(150, 110, 150, 117, TFT_WHITE);
        tftespi.drawLine(151, 110, 151, 117, TFT_WHITE);
        tftespi.drawLine(150, 117, 135, 117, TFT_WHITE);
        tftespi.drawLine(150, 118, 135, 118, TFT_WHITE);
        tftespi.drawLine(135, 117, 140, 112, TFT_WHITE);
        tftespi.drawLine(136, 117, 141, 112, TFT_WHITE);
        tftespi.drawLine(135, 118, 140, 123, TFT_WHITE);
        tftespi.drawLine(136, 118, 141, 123, TFT_WHITE);
      }
      else if(Theme == "Light")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
        tftespi.setTextSize(2);
        tftespi.setTextColor(TFT_BLUE);
        tftespi.drawString("SETTINGS",30,5,1);
        tftespi.fillRect(0, 25, 128, 25, TFT_YELLOW);
        tftespi.setTextColor(TFT_BLACK);
        tftespi.drawString("Brightness",5,30,1);
        tftespi.drawString("Theme",5,60,1);
        tftespi.drawString("Back",5,90,1);
        //Draw up, down and enter icons

        //Up
        tftespi.drawLine(76, 110, 76, 123, TFT_BLACK);
        tftespi.drawLine(75, 110, 75, 123, TFT_BLACK);
        tftespi.drawLine(75, 110, 70, 115, TFT_BLACK);
        tftespi.drawLine(76, 111, 71, 116, TFT_BLACK);
        tftespi.drawLine(77, 110, 82, 115, TFT_BLACK);
        tftespi.drawLine(76, 111, 81, 116, TFT_BLACK);

        //Down
        tftespi.drawLine(10, 110, 10, 123, TFT_BLACK);
        tftespi.drawLine(11, 110, 11, 123, TFT_BLACK);
        tftespi.drawLine(10, 123, 5, 118, TFT_BLACK);
        tftespi.drawLine(11, 122, 6, 117, TFT_BLACK);
        tftespi.drawLine(12, 123, 17, 118, TFT_BLACK);
        tftespi.drawLine(11, 122, 16, 117, TFT_BLACK);

        //Enter
        tftespi.drawLine(150, 110, 150, 117, TFT_BLACK);
        tftespi.drawLine(151, 110, 151, 117, TFT_BLACK);
        tftespi.drawLine(150, 117, 135, 117, TFT_BLACK);
        tftespi.drawLine(150, 118, 135, 118, TFT_BLACK);
        tftespi.drawLine(135, 117, 140, 112, TFT_BLACK);
        tftespi.drawLine(136, 117, 141, 112, TFT_BLACK);
        tftespi.drawLine(135, 118, 140, 123, TFT_BLACK);
        tftespi.drawLine(136, 118, 141, 123, TFT_BLACK);
      }
      displayingContent = true;
    }
    else if(selectedItem != previousSelectedItem)
    {
      switch(selectedItem)
      {
        case 0:
          if(Theme == "Dark")
          {
            tftespi.fillRect(0, 25, 128, 25, TFT_YELLOW);
            tftespi.fillRect(0, 55, 128, 25, TFT_BLACK);
            tftespi.setTextColor(TFT_WHITE);
            tftespi.drawString("Brightness",5,30,1);
            tftespi.drawString("Theme",5,60,1);
          }
          else if(Theme == "Light")
          {
            tftespi.fillRect(0, 25, 128, 25, TFT_YELLOW);
            tftespi.fillRect(0, 55, 128, 25, TFT_WHITE);
            tftespi.setTextColor(TFT_BLACK);
            tftespi.drawString("Brightness",5,30,1);
            tftespi.drawString("Theme",5,60,1);
          }
          break;
        case 1:
          if(Theme == "Dark")
          {
            if(previousSelectedItem == 0)
            {
              tftespi.fillRect(0, 55, 128, 25, TFT_YELLOW);
              tftespi.fillRect(0, 25, 128, 25, TFT_BLACK);
              tftespi.setTextColor(TFT_WHITE);
              tftespi.drawString("Brightness",5,30,1);
              tftespi.drawString("Theme",5,60,1);
            }
            else if(previousSelectedItem == 2)
            {
              tftespi.fillRect(0, 55, 128, 25, TFT_YELLOW);
              tftespi.fillRect(0, 85, 128, 25, TFT_BLACK);
              tftespi.setTextColor(TFT_WHITE);
              tftespi.drawString("Theme",5,60,1);
              tftespi.drawString("Back",5,90,1);
            }
          }
          else if(Theme == "Light")
          {
            if(previousSelectedItem == 0)
            {
              tftespi.fillRect(0, 55, 128, 25, TFT_YELLOW);
              tftespi.fillRect(0, 25, 128, 25, TFT_WHITE);
              tftespi.setTextColor(TFT_BLACK);
              tftespi.drawString("Brightness",5,30,1);
              tftespi.drawString("Theme",5,60,1);
            }
            else if(previousSelectedItem == 2)
            {
              tftespi.fillRect(0, 55, 128, 25, TFT_YELLOW);
              tftespi.fillRect(0, 85, 128, 25, TFT_WHITE);
              tftespi.setTextColor(TFT_BLACK);
              tftespi.drawString("Theme",5,60,1);
              tftespi.drawString("Back",5,90,1);
            }
          }
          break;
        case 2:
          if(Theme == "Dark")
          {
            tftespi.fillRect(0, 55, 128, 25, TFT_BLACK);
            tftespi.fillRect(0, 85, 128, 25, TFT_YELLOW);
            tftespi.setTextColor(TFT_WHITE);
            tftespi.drawString("Theme",5,60,1);
            tftespi.drawString("Back",5,90,1);
          }
          else if(Theme == "Light")
          {
            tftespi.fillRect(0, 55, 128, 25, TFT_WHITE);
            tftespi.fillRect(0, 85, 128, 25, TFT_YELLOW);
            tftespi.setTextColor(TFT_BLACK);
            tftespi.drawString("Theme",5,60,1);
            tftespi.drawString("Back",5,90,1);
          }
          break;
      }
      previousSelectedItem = selectedItem;
    }
  }
  void Reset()
  {
    displayingContent = false;
    selectedItem = 0;
    HoldUnitButtonLastPressTime = millis();
    TareButtonLastPressTime = millis();
    PowerButtonLastPressTime = millis();
  }
};
class ThemeScreen
{
  bool displayingContent = false;
  String CurrentTheme = "Light";
  long PowerButtonLastPressTime = millis();
  int PowerButtonRepressDuration = 1000;
  public:
  void Display()
  {
    if(!displayingContent)
    {
      if(Theme == "Dark")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
        tftespi.setTextSize(2);
        tftespi.setTextColor(TFT_GREEN);
        tftespi.drawString("Select theme:",5,15,1);
        tftespi.fillRect(0, 45, 128, 25, TFT_YELLOW);
        tftespi.setTextColor(TFT_WHITE);
        tftespi.drawString("Light mode",5,50,1);
        tftespi.drawString("Dark mode",5,80,1);
        //Draw up, down and enter icons

        //Up
        tftespi.drawLine(76, 110, 76, 123, TFT_WHITE);
        tftespi.drawLine(75, 110, 75, 123, TFT_WHITE);
        tftespi.drawLine(75, 110, 70, 115, TFT_WHITE);
        tftespi.drawLine(76, 111, 71, 116, TFT_WHITE);
        tftespi.drawLine(77, 110, 82, 115, TFT_WHITE);
        tftespi.drawLine(76, 111, 81, 116, TFT_WHITE);

        //Down
        tftespi.drawLine(10, 110, 10, 123, TFT_WHITE);
        tftespi.drawLine(11, 110, 11, 123, TFT_WHITE);
        tftespi.drawLine(10, 123, 5, 118, TFT_WHITE);
        tftespi.drawLine(11, 122, 6, 117, TFT_WHITE);
        tftespi.drawLine(12, 123, 17, 118, TFT_WHITE);
        tftespi.drawLine(11, 122, 16, 117, TFT_WHITE);

        //Enter
        tftespi.drawLine(150, 110, 150, 117, TFT_WHITE);
        tftespi.drawLine(151, 110, 151, 117, TFT_WHITE);
        tftespi.drawLine(150, 117, 135, 117, TFT_WHITE);
        tftespi.drawLine(150, 118, 135, 118, TFT_WHITE);
        tftespi.drawLine(135, 117, 140, 112, TFT_WHITE);
        tftespi.drawLine(136, 117, 141, 112, TFT_WHITE);
        tftespi.drawLine(135, 118, 140, 123, TFT_WHITE);
        tftespi.drawLine(136, 118, 141, 123, TFT_WHITE);
      }
      else if(Theme == "Light")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
        tftespi.setTextSize(2);
        tftespi.setTextColor(TFT_BLACK);
        tftespi.drawString("Select theme:",5,15,1);
        tftespi.fillRect(0, 45, 128, 25, TFT_YELLOW);
        tftespi.setTextColor(TFT_BLACK);
        tftespi.drawString("Light mode",5,50,1);
        tftespi.drawString("Dark mode",5,80,1);
        //Draw up, down and enter icons

        //Up
        tftespi.drawLine(76, 110, 76, 123, TFT_BLACK);
        tftespi.drawLine(75, 110, 75, 123, TFT_BLACK);
        tftespi.drawLine(75, 110, 70, 115, TFT_BLACK);
        tftespi.drawLine(76, 111, 71, 116, TFT_BLACK);
        tftespi.drawLine(77, 110, 82, 115, TFT_BLACK);
        tftespi.drawLine(76, 111, 81, 116, TFT_BLACK);

        //Down
        tftespi.drawLine(10, 110, 10, 123, TFT_BLACK);
        tftespi.drawLine(11, 110, 11, 123, TFT_BLACK);
        tftespi.drawLine(10, 123, 5, 118, TFT_BLACK);
        tftespi.drawLine(11, 122, 6, 117, TFT_BLACK);
        tftespi.drawLine(12, 123, 17, 118, TFT_BLACK);
        tftespi.drawLine(11, 122, 16, 117, TFT_BLACK);

        //Enter
        tftespi.drawLine(150, 110, 150, 117, TFT_BLACK);
        tftespi.drawLine(151, 110, 151, 117, TFT_BLACK);
        tftespi.drawLine(150, 117, 135, 117, TFT_BLACK);
        tftespi.drawLine(150, 118, 135, 118, TFT_BLACK);
        tftespi.drawLine(135, 117, 140, 112, TFT_BLACK);
        tftespi.drawLine(136, 117, 141, 112, TFT_BLACK);
        tftespi.drawLine(135, 118, 140, 123, TFT_BLACK);
        tftespi.drawLine(136, 118, 141, 123, TFT_BLACK);
      }
      displayingContent = true;
    }
    else
    {
      if(!digitalRead(HoldUnitButton))
      {
        if(Theme == "Dark")
        {
          tftespi.fillRect(0, 45, 128, 25, TFT_BLACK);
          tftespi.fillRect(0, 75, 128, 25, TFT_YELLOW);
          tftespi.setTextSize(2);
          tftespi.setTextColor(TFT_WHITE);
          tftespi.drawString("Light mode",5,50,1);
          tftespi.setTextColor(TFT_BLACK);
          tftespi.drawString("Dark mode",5,80,1);
          CurrentTheme = "Dark";
        }
        else if(Theme == "Light")
        {
          tftespi.fillRect(0, 45, 128, 25, TFT_WHITE);
          tftespi.fillRect(0, 75, 128, 25, TFT_YELLOW);
          tftespi.setTextSize(2);
          tftespi.setTextColor(TFT_BLACK);
          tftespi.drawString("Light mode",5,50,1);
          tftespi.drawString("Dark mode",5,80,1);
          CurrentTheme = "Dark";
        }
      }
      else if(!digitalRead(TareButton))
      {
        if(Theme == "Dark")
        {
          tftespi.fillRect(0, 75, 128, 25, TFT_BLACK);
          tftespi.fillRect(0, 45, 128, 25, TFT_YELLOW);
          tftespi.setTextSize(2);
          tftespi.setTextColor(TFT_BLACK);
          tftespi.drawString("Light mode",5,50,1);
          tftespi.setTextColor(TFT_WHITE);
          tftespi.drawString("Dark mode",5,80,1);
          CurrentTheme = "Light";
        }
        else if(Theme == "Light")
        {
          tftespi.fillRect(0, 75, 128, 25, TFT_WHITE);
          tftespi.fillRect(0, 45, 128, 25, TFT_YELLOW);
          tftespi.setTextSize(2);
          tftespi.setTextColor(TFT_BLACK);
          tftespi.drawString("Light mode",5,50,1);
          tftespi.drawString("Dark mode",5,80,1);
          CurrentTheme = "Light";
        }
      }
      else if(digitalRead(PowerButton) && millis() - PowerButtonLastPressTime >= PowerButtonRepressDuration)
      {
        preferences.begin(NonVolatileThemeNameSpace,false);
        preferences.putString(PreferenceThemeKey, CurrentTheme);
        preferences.end();
        Theme = CurrentTheme;
        ResetScreen = true;
        Screen = 8;
      }
    }
  }
  void Reset()
  {
    displayingContent = false;
    PowerButtonLastPressTime = millis();
  }
};
class BrightnessScreen
{
  bool displayingContent = false;
  int lastDisplayBrightness;
  long HoldUnitButtonLastPressTime = 0;
  long TareButtonLastPressTime = 0;
  long PowerButtonLastPressTime = millis();
  int HoldUnitButtonRepressDuration = 100;
  int TareButtonRepressDuration = 100;
  int PowerButtonRepressDuration = 1000;
  public:
  void Display()
  {
    if(!digitalRead(HoldUnitButton) && millis() - HoldUnitButtonLastPressTime >= HoldUnitButtonRepressDuration && DisplayBrightness > 10) 
    {
      DisplayBrightness --;
      HoldUnitButtonLastPressTime = millis();
    }
    if(!digitalRead(TareButton) && millis() - TareButtonLastPressTime >= TareButtonRepressDuration && DisplayBrightness < 255)
    {
      DisplayBrightness++;
      TareButtonLastPressTime = millis();
    }
    if(digitalRead(PowerButton) && millis() - PowerButtonLastPressTime >= PowerButtonRepressDuration)
    {
      preferences.begin(NonVolatileBrightnessNamespace, false);
      preferences.putInt(PreferencesBrightnessKey, DisplayBrightness);
      preferences.end();
      Screen = 8;
      ResetScreen = true;
      return;
    }
    if(!displayingContent)
    {
      if(Theme == "Dark")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_BLACK);
        tftespi.setTextSize(2);
        tftespi.setTextColor(TFT_WHITE);
        tftespi.drawString("Brightness:",5,15,1);
        tftespi.drawString(String((DisplayBrightness * 100) / 256) + " %",80,50,1);
        tftespi.drawRect(5,75,150,20,TFT_WHITE);
        int DisplayBrightnessRectangleWidthValue = (DisplayBrightness * 150) / 256; //150 is the max width of the rectangle showing the brightness value
        tftespi.fillRect(6, 76, DisplayBrightnessRectangleWidthValue, 18, TFT_RED);
        //Draw increase, decrease and enter icons

        //Increase
        tftespi.drawLine(76, 110, 76, 123, TFT_WHITE);
        tftespi.drawLine(75, 110, 75, 123, TFT_WHITE);
        tftespi.drawLine(75, 110, 70, 115, TFT_WHITE);
        tftespi.drawLine(76, 111, 71, 116, TFT_WHITE);
        tftespi.drawLine(77, 110, 82, 115, TFT_WHITE);
        tftespi.drawLine(76, 111, 81, 116, TFT_WHITE);

        //Decrease
        tftespi.drawLine(10, 110, 10, 123, TFT_WHITE);
        tftespi.drawLine(11, 110, 11, 123, TFT_WHITE);
        tftespi.drawLine(10, 123, 5, 118, TFT_WHITE);
        tftespi.drawLine(11, 122, 6, 117, TFT_WHITE);
        tftespi.drawLine(12, 123, 17, 118, TFT_WHITE);
        tftespi.drawLine(11, 122, 16, 117, TFT_WHITE);

        //Enter
        tftespi.drawLine(150, 110, 150, 117, TFT_WHITE);
        tftespi.drawLine(151, 110, 151, 117, TFT_WHITE);
        tftespi.drawLine(150, 117, 135, 117, TFT_WHITE);
        tftespi.drawLine(150, 118, 135, 118, TFT_WHITE);
        tftespi.drawLine(135, 117, 140, 112, TFT_WHITE);
        tftespi.drawLine(136, 117, 141, 112, TFT_WHITE);
        tftespi.drawLine(135, 118, 140, 123, TFT_WHITE);
        tftespi.drawLine(136, 118, 141, 123, TFT_WHITE);
      }
      else if(Theme == "Light")
      {
        tftespi.fillRect(0, 0, 160, 128, TFT_WHITE);
        tftespi.setTextSize(2);
        tftespi.setTextColor(TFT_BLACK);
        tftespi.drawString("Brightness:",5,15,1);
        tftespi.drawString(String((DisplayBrightness * 100) / 256) + " %",100,50,1);
        tftespi.drawRect(5,75,150,20,TFT_BLACK);
        int DisplayBrightnessRectangleWidthValue = (DisplayBrightness * 150) / 256; //150 is the max width of the rectangle showing the brightness value
        tftespi.fillRect(6, 76, DisplayBrightnessRectangleWidthValue, 18, TFT_RED);
        //Draw increase, decrease and enter icons

        //Increase
        tftespi.drawLine(76, 110, 76, 123, TFT_BLACK);
        tftespi.drawLine(75, 110, 75, 123, TFT_BLACK);
        tftespi.drawLine(75, 110, 70, 115, TFT_BLACK);
        tftespi.drawLine(76, 111, 71, 116, TFT_BLACK);
        tftespi.drawLine(77, 110, 82, 115, TFT_BLACK);
        tftespi.drawLine(76, 111, 81, 116, TFT_BLACK);

        //Decrease
        tftespi.drawLine(10, 110, 10, 123, TFT_BLACK);
        tftespi.drawLine(11, 110, 11, 123, TFT_BLACK);
        tftespi.drawLine(10, 123, 5, 118, TFT_BLACK);
        tftespi.drawLine(11, 122, 6, 117, TFT_BLACK);
        tftespi.drawLine(12, 123, 17, 118, TFT_BLACK);
        tftespi.drawLine(11, 122, 16, 117, TFT_BLACK);

        //Enter
        tftespi.drawLine(150, 110, 150, 117, TFT_BLACK);
        tftespi.drawLine(151, 110, 151, 117, TFT_BLACK);
        tftespi.drawLine(150, 117, 135, 117, TFT_BLACK);
        tftespi.drawLine(150, 118, 135, 118, TFT_BLACK);
        tftespi.drawLine(135, 117, 140, 112, TFT_BLACK);
        tftespi.drawLine(136, 117, 141, 112, TFT_BLACK);
        tftespi.drawLine(135, 118, 140, 123, TFT_BLACK);
        tftespi.drawLine(136, 118, 141, 123, TFT_BLACK);
      }
      displayingContent = true;
    }
    else if(lastDisplayBrightness != DisplayBrightness)
    {
      analogWrite(TFT_LED,DisplayBrightness);
      if(Theme == "Dark")
      {
        tftespi.fillRect(0, 70, 160, 30, TFT_BLACK);
        tftespi.fillRect(80, 45, 80, 20, TFT_BLACK);
        tftespi.drawString(String((DisplayBrightness * 100) / 255) + " %",100,50,1);
        tftespi.drawRect(5,75,150,20,TFT_WHITE);
        int DisplayBrightnessRectangleWidthValue = (DisplayBrightness * 150) / 256; //150 is the max width of the rectangle showing the brightness value
        tftespi.fillRect(6, 76, DisplayBrightnessRectangleWidthValue, 18, TFT_RED);
      }
      else if(Theme == "Light")
      {
        tftespi.fillRect(0, 70, 160, 30, TFT_WHITE);
        tftespi.fillRect(80,45,80,20,TFT_WHITE);
        tftespi.drawString(String((DisplayBrightness * 100) / 255) + " %",100,50,1);
        tftespi.drawRect(5,75,150,20,TFT_BLACK);
        int DisplayBrightnessRectangleWidthValue = (DisplayBrightness * 150) / 255; //150 is the max width of the rectangle showing the brightness value
        tftespi.fillRect(6, 76, DisplayBrightnessRectangleWidthValue, 18, TFT_RED);
      }
      lastDisplayBrightness = DisplayBrightness;
    }
  }
  void Reset()
  {
    displayingContent = false;
    PowerButtonLastPressTime = millis();
  }
};
MemoryLoader memoryLoader;
Initializer systemInitializer;
PowerOnScreen powerOnScreen;
UnauthorizedScreen unauthorizedScreen;
UnCalibratedScreen unCalibratedScreen;
CalibratingMassScreen calibratingMassScreen;
CalibratingScreen calibratingScreen;
MainScreen mainScreen;
ConfirmCalibrateScreen confirmCalibrateScreen;
PowerOffScreen powerOffScreen;
SettingsScreen settingsScreen;
ThemeScreen themeScreen;
BrightnessScreen brightnessScreen;
void setup()
{
  memoryLoader.SetFlashMemorySavedVariables();
  memoryLoader.SetTheme();
  memoryLoader.SetDisplayBrightness();
  Serial.begin(115200);
  systemInitializer.InitializeSystem();
  xTaskCreatePinnedToCore(BackgroundManager,"Background",10000,NULL,1,&BackgroundProcessor,1);
}
void loop() 
{
  switch(Screen)
  {
    case 0:
    powerOnScreen.Display();
    break;
    case 1:
      if(ResetScreen)
      {
        unauthorizedScreen.Reset();
        ResetScreen = false;
      }
      unauthorizedScreen.Display();
      break;
    case 2:
      if(ResetScreen)
      {
        unCalibratedScreen.Reset();
        ResetScreen = false;
      }
      unCalibratedScreen.Display();
      break;
    case 3:
      if(ResetScreen) 
      {
        calibratingMassScreen.Reset();
        ResetScreen = false;
      }
      calibratingMassScreen.Display();
      break;
    case 4:
      if(ResetScreen) 
      {
        calibratingScreen.Reset();
        ResetScreen = false;
      }
      calibratingScreen.Display();
      break;
    case 5:
      if(ResetScreen)
      {
        mainScreen.Reset();
        ResetScreen = false;
      }
      mainScreen.Display();
      break;
    case 6:
      if(ResetScreen)
      {
        confirmCalibrateScreen.Reset();
        ResetScreen = false;
      }
      confirmCalibrateScreen.Display();
      break;
    case 7:
      powerOffScreen.Display();
      break;
    case 8:
      if(ResetScreen)
      {
        settingsScreen.Reset();
        ResetScreen = false;
      }
      settingsScreen.Display();
      break;
    case 9:
      if(ResetScreen)
      {
        themeScreen.Reset();
        ResetScreen = false;
      }
      themeScreen.Display();
      break;
    case 10:
      if(ResetScreen)
      {
        brightnessScreen.Reset();
        ResetScreen = false;
      }
      brightnessScreen.Display();
      break;
    default:
    break;
  }
}
//Background classes
class BluetoothManager
{
  #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run 'make menuconfig' to and enable it
  #endif

  #if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
  #endif

  BluetoothSerial SerialBT;
  bool sentBluetoothMac = false;
  bool deviceAuthorized = false;
  bool bluetoothHasBeenReset = true;
  long lastAuthorizationTime = millis();
  public:
  void Initialize()
  {
    SerialBT.begin(deviceName);
  }
  void MonitorConnection()
  {
    if(!bluetoothHasBeenReset && !WaitingForSecurityReset)
    {
      Reset();
      bluetoothHasBeenReset = true;
    }
    else if(!bluetoothHasBeenReset && WaitingForSecurityReset)
    {
      return;
    }
    if( bluetoothStatus != "UNAUTHORISED" && deviceAuthorized)
    {
      VerifyAuthorization();
      if(!bluetoothHasBeenReset && WaitingForSecurityReset) return;
    }
    if(SerialBT.connected()) 
    {
      if(!sentBluetoothMac) 
      {
        SerialBT.println(EncodeBluetoothMac(GetBluetoothMac()));
        sentBluetoothMac = true;
        AuthorizeDevice();
      }
      else if(deviceAuthorized && bluetoothStatus == "OFF") bluetoothStatus = "ON";
      else if(!deviceAuthorized) AuthorizeDevice();
    }
    else bluetoothStatus = "OFF";
  }
  void AuthorizeDevice()
  {
    if(ReceiveBufferRead) ReadData();
    long ReceiveAuthenticationPasswordMaxDuration = 1000;
    long ReceiveAuthenticationPasswordStartTime = millis();
    while(ReceiveBuffer.length() == 0) 
    {
      ReadData();
      if(millis() - ReceiveAuthenticationPasswordStartTime > ReceiveAuthenticationPasswordMaxDuration) break;
    }
    if(ReceiveBuffer.length() > 4 && ReceiveBuffer[0] == 'B' && ReceiveBuffer[1] == '2' && DecodeConnectionPassword(ReceiveBuffer) == ConnectionPassword)
    {
      lastAuthorizationTime = millis();
      deviceAuthorized = true;
      bluetoothStatus = "ON";
      ReceiveBuffer = "";
      ReceiveBufferRead = true;
    }
    else bluetoothStatus = "UNAUTHORISED";
  }
  void SendData(String Data)
  {
    if(!bluetoothHasBeenReset && WaitingForSecurityReset) return;
    if(SerialBT.connected())
    {
      try
      {
        SerialBT.println(Data);
        throw (bluetoothStatus = "OFF");
      }
      catch (...)
      {
        bluetoothStatus = "OFF";
      }
    }
    else bluetoothStatus = "OFF";
  }
  void ReadData()
  {
    if(!bluetoothHasBeenReset && WaitingForSecurityReset) return;
    if(SerialBT.available() && ReceiveBufferRead)
    {
      ReceiveBuffer = SerialBT.readStringUntil('\n');
      SerialBT.flush();
      if(ReceiveBuffer.length() > 0) ReceiveBufferRead = false;
    }
  }
  private:
  void VerifyAuthorization()
  {
    if(ReceiveBufferRead) ReadData();
    long ReceiveAuthenticationVerificationPasswordMaxDuration = 500;
    long ReceiveAuthenticationVerificationPasswordStartTime = millis();
    while(ReceiveBuffer.length() == 0) 
    {
      ReadData();
      if(millis() - ReceiveAuthenticationVerificationPasswordStartTime > ReceiveAuthenticationVerificationPasswordMaxDuration) break;
    }
    if(ReceiveBuffer.length() > 4 && ReceiveBuffer[0] == 'E' && ReceiveBuffer[1] == '7' && DecodeConnectionPassword(ReceiveBuffer) == ConnectionPassword)
    {
      lastAuthorizationTime = millis();
      deviceAuthorized = true;
      bluetoothStatus = "ON";
      ReceiveBuffer = "";
      ReceiveBufferRead = true;
    }
    if(millis() - lastAuthorizationTime > SecurityCheckInterval)
    {
      Serial.println("Reseting");
      bluetoothStatus = "UNAUTHORISED";
      WaitingForSecurityReset = true;
      bluetoothHasBeenReset = false;
    }
  }
  void Reset()
  {
    sentBluetoothMac = false;
    deviceAuthorized = false;
    lastAuthorizationTime = millis();
  }
};
class CyclicBuffer
{
  private:
  bool Activated = false;
  #define WeightCyclicBufferSize 100
  long WeightCyclicBuffer[WeightCyclicBufferSize];
  int bufferLength = 0;
  int bufferPointer = 0;
  public:
  void Insert(long* newData,int newDataLength)
  {
    for(int i = 0;i < newDataLength;bufferPointer++,i++) 
    {
      if(bufferPointer < WeightCyclicBufferSize) WeightCyclicBuffer[bufferPointer] = newData[i];
      if(bufferPointer == WeightCyclicBufferSize - 1) bufferPointer = 0;
      if(bufferLength < WeightCyclicBufferSize) bufferLength ++;
    }
    Activated = true;
  }
  void Insert(long newData) 
  {
      if(bufferPointer < WeightCyclicBufferSize) WeightCyclicBuffer[bufferPointer] = newData; 
      bufferPointer ++;
      if(bufferPointer == WeightCyclicBufferSize) bufferPointer = 0;
      if(bufferLength < WeightCyclicBufferSize) bufferLength ++;
      Activated = true;
  }
  int Length()
  {
    return bufferLength;
  }
  void Clear()
  {
    bufferLength = 0;
    bufferPointer = 0;
  }
  double Average()
  {
    if(!Activated) return 0;
    double weightSum = 0;
    for (int i = 1; i <= bufferLength; i++) weightSum += i;
    double weightConstant = 1/weightSum;
    double average = 0;
    int i = (bufferPointer + 1) % bufferLength;
    for(int j = 0;j < bufferLength;j++) 
    {
      average += (double)WeightCyclicBuffer[i] * weightConstant * (j + 1);
      i = (i + 1) % bufferLength;
    }
    return average;
  }
  void PrintContentsToSerialTerminal()
  {
    for(int i = 0;i < bufferLength;i++) 
    {
      Serial.print(WeightCyclicBuffer[i]);
      Serial.print("\t");
    }
    Serial.println("");
  }
};
class WeightManager
{
  private:
  long previousReading;
  protected:
  CyclicBuffer cyclicBuffer;
  private:
  long lastWeightReading = 0;
  #define weightSampleSize 10
  long spikeArray[weightSampleSize];
  double spikePercentage = 0.0001;//0.001;
  protected:
  long ReadHx711()
  {
    long reading = 0;
    int weightSampleSizeSubtractValue = 0;
    for (int i = 0,j = 0; i < weightSampleSize; i++)
    {
      long immediateReading = hx711.read();
      if(i > 1)
      {
        long spikeArrayAverage = SpikeArrayAverage(j);
        long minAllowedImmediateReading = (1 - spikePercentage) * spikeArrayAverage;
        long maxAllowedImmediateReading = (1 + spikePercentage) * spikeArrayAverage;
        if(immediateReading > maxAllowedImmediateReading || immediateReading < minAllowedImmediateReading) 
        {
          weightSampleSizeSubtractValue++;
          continue;
        }
        reading += immediateReading;
        spikeArray[j] = immediateReading;
        j++;
      }
      else
      {
        reading += immediateReading;
        spikeArray[j] = immediateReading;
        j++;
      }
    }
    cyclicBuffer.Insert(spikeArray, weightSampleSize - weightSampleSizeSubtractValue);
    //cyclicBuffer.PrintContentsToSerialTerminal();
    reading = cyclicBuffer.Average();
    WeightReadingsGradient = reading - previousReading;
    previousReading = reading;
    Serial.println(reading);
    return reading;
  }
  long SpikeArrayAverage(int spikeArrayLength)
  {
    long spikeArraySum = 0;
    for(int i = 0;i < spikeArrayLength;i++) spikeArraySum += spikeArray[i];
    return spikeArraySum / spikeArrayLength;
  }
  public:
  void ReadWeight()
  {
    long hxReading = ReadHx711();
    float ratio1 =(float)(hxReading - TareHxReading);
    float ratio2 =(float)(CalibrationHxReading);
    float ratio = ratio1/ratio2;
    LiveWeight = CalibratingWeight * ratio;
  }
  void ZeroTare()
  {
    TareHxReading = ReadHx711();
    Serial.print("TareHxReading: ");
    Serial.println(TareHxReading);
    preferences.begin(NonVolatileDataNamespace,false);
    preferences.putLong(PreferencesTareWeightKey,TareHxReading);
    preferences.end();
  }
};
class CalibrationManager:public WeightManager
{
  private:
  long WaitingForMassStartTime = millis();
  ushort WaitingForMassDuration = 3000;
  bool StartedWaitingForMass = false;
  bool StartedMonitoringMassInput = false;
  long CalibratingStartTime = millis();
  ushort CalibratingDuration = 3000;
  bool StartedCalibrating = false;
  public:
  void ManageCalibration()
  {
    if(CalibrationStatus == "OFF") return;
    else if(CalibrationStatus == "WAITING")
    {
      //Serial.println(WeightReadingsGradient);
      if(!StartedWaitingForMass)
      {
        ZeroTare();
        StartedWaitingForMass = true;
      }
      else if(WeightReadingsGradient > 500 && !StartedMonitoringMassInput)
      {
        //Serial.println("Ready");
        WaitingForMassStartTime = millis();
        StartedMonitoringMassInput = true;
      }
      else if(StartedWaitingForMass && StartedMonitoringMassInput && WeightReadingsGradient <= 0)
      {
        CalibrationStatus = "CALIBRATING";
        StartedWaitingForMass = false;
        StartedMonitoringMassInput = false;
      }
    }
    else if(CalibrationStatus == "CALIBRATING")
    {
      //delay(3000); Replaced by millis() code below.
      if(!StartedCalibrating)
      {
        CalibratingStartTime = millis();
        StartedCalibrating = true;
      }
      else if(millis() - CalibratingStartTime >= CalibratingDuration)
      {
        long HXReadingAtCalibration = ReadHx711();
        CalibrationHxReading = HXReadingAtCalibration - TareHxReading;
        // Serial.print("HX reading at calibration: ");
        // Serial.println(HXReadingAtCalibration);
        // Serial.print("CalibrationHXReading: ");
        // Serial.println(CalibrationHxReading);
        // Serial.print("Calibrating weight: ");
        // Serial.println(CalibratingWeight);
        cyclicBuffer.PrintContentsToSerialTerminal();
        preferences.begin(NonVolatileDataNamespace,false);
        preferences.putLong(PreferencesCalibrationKey, CalibratingWeight);
        preferences.putLong(PreferencesHxCalibrationKey,CalibrationHxReading);
        preferences.end();
        CalibrationStatus = "CALIBRATED";
      }
    }
  }
};
class BatteryManager
{
  long x = millis();
  double MAX_BATTERY_VOLTAGE = 0.757;
  double MIN_BATTERY_VOLTAGE = 0.595;
  double MAX_ANALOG_READ_VALUE = 4095;
  public:
  void ManageBattery()
  {
    IsCharging = digitalRead(ChargingPin);
    int readBatteryValue = analogRead(BatteryPin);
    LiveBatteryValue = (readBatteryValue * 100) / MAX_ANALOG_READ_VALUE;
    if(readBatteryValue != MAX_ANALOG_READ_VALUE) ShowBatteryLow = true;
  }
};
void BackgroundManager(void *parameters)
{
  BluetoothManager bluetoothManager;
  WeightManager weightManager;
  CalibrationManager calibrationManager;
  BatteryManager batteryManager;
  bluetoothManager.Initialize();
  pinMode(TamperPin,INPUT);
  while(true)
  {
    bluetoothManager.MonitorConnection();
    weightManager.ReadWeight();
    calibrationManager.ManageCalibration();
    batteryManager.ManageBattery();
    bluetoothManager.ReadData();
    if(TransmitData)
    {
      bluetoothManager.SendData(TransmitBuffer);
      TransmitData = false;
    }
    if(!digitalRead(TamperPin) && !Tampered)
    {
      Tamper ++;
      Tampered = true;
    }
    else if(digitalRead(TamperPin))Tampered = false;
    if(ZeroTare)
    {
      weightManager.ZeroTare();
      ZeroTare = false;
    }
  }
}