#include <LCD.h>

// Set in configuration menu while running
int loadCellCount = 0;

// Button 4 ISR and setup
#define button4_pin 12
int button4Presses = 0;
unsigned long button4_time = 0;
unsigned long last_button4_time = 0;

void IRAM_ATTR button4ISR() {
  button4_time = millis();
  if (button4_time - last_button4_time > 250) {
    last_button4_time = button4_time;
    button4Presses++;
  }
}

// Button 3 ISR and setup
#define button3_pin 14
int button3Presses = 0;
unsigned long button3_time = 0;
unsigned long last_button3_time = 0;

void IRAM_ATTR button3ISR() {
  button3_time = millis();
  if (button3_time - last_button3_time > 250) {
    last_button3_time = button3_time;
    button3Presses++;
  }
}

// Button 2 ISR and setup
#define button2_pin 26
int button2Presses = 0;
unsigned long button2_time = 0;
unsigned long last_button2_time = 0;

void IRAM_ATTR button2ISR() {
  button2_time = millis();
  if (button2_time - last_button2_time > 250) {
    last_button2_time = button2_time;
    button2Presses++;
  }
}

// Button 1 ISR and setup
#define button1_pin 25
int button1Presses = 0;
unsigned long button1_time = 0;
unsigned long last_button1_time = 0;

void IRAM_ATTR button1ISR() {
  button1_time = millis();
  if (button1_time - last_button1_time > 250) {
    last_button1_time = button1_time;
    button1Presses++;
  }
}


//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU  : Arduino Nano
//-------------------------------------------------------------------------------------
// This is an example sketch on how to use this library for two ore more HX711 modules
// Settling time (number of samples) and data filtering can be adjusted in the config.h file

#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

const bool debug = false;

//pins:
const int HX711_dout_1 = 5; //mcu > HX711 no 1 dout pin
const int HX711_sck_1 = 4; //mcu > HX711 no 1 sck pin
const int HX711_dout_2 = 16; //mcu > HX711 no 2 dout pin
const int HX711_sck_2 = 19; //mcu > HX711 no 2 sck pin
const int HX711_dout_3 = 32;
const int HX711_sck_3 = 33;
const int HX711_dout_4 = 23;
const int HX711_sck_4 = 27;

const int samples = 16; //Samples to take from each load cell
const int updateTime = 250; //milliseconds to wait between each update

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1); //HX711 1
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2); //HX711 2
HX711_ADC LoadCell_3(HX711_dout_3, HX711_sck_3); //HX711 3
HX711_ADC LoadCell_4(HX711_dout_4, HX711_sck_4); //HX711 4

const int calVal_eepromAdress_1 = 0; // eeprom adress for calibration value load cell 1 (4 bytes)
const int calVal_eepromAdress_2 = 4; // eeprom adress for calibration value load cell 2 (4 bytes)
const int calVal_eepromAdress_3 = 8; // eeprom adress for calibration value load cell 3
const int calVal_eepromAdress_4 = 12; //eeprom adress for calibration value load cell 4
unsigned long t = 0;


// Setup of all device, define pins, LCD, HX711 configurations, etc.
void setup() {
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");

  pinMode(button1_pin, INPUT_PULLUP);
  attachInterrupt(button1_pin, button1ISR, FALLING);
  pinMode(button2_pin, INPUT_PULLUP);
  attachInterrupt(button2_pin, button2ISR, FALLING);
  pinMode(button3_pin, INPUT_PULLUP);
  //attachInterrupt(button3_pin, button3ISR, FALLING);
  pinMode(button4_pin, INPUT_PULLUP);
  attachInterrupt(button4_pin, button4ISR, FALLING);


  int status;

  status = lcd.begin(LCD_COLS, LCD_ROWS);

  lcd.clear();
  splashScreen();

  lcd.clear();
  lcd.setCursor(6, 1);
  lcd.print("Starting");

  //pinMode(button4.PIN, INPUT);

  float calibrationValue_1; // calibration value load cell 1
  float calibrationValue_2; // calibration value load cell 2
  float calibrationValue_3;
  float calibrationValue_4;

  //calibrationValue_1 = 696.0; // uncomment this if you want to set this value in the sketch
  //calibrationValue_2 = 733.0; // uncomment this if you want to set this value in the sketch
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
  #endif
  EEPROM.get(calVal_eepromAdress_1, calibrationValue_1); // uncomment this if you want to fetch the value from eeprom
  EEPROM.get(calVal_eepromAdress_2, calibrationValue_2); // uncomment this if you want to fetch the value from eeprom
  EEPROM.get(calVal_eepromAdress_3, calibrationValue_3);
  EEPROM.get(calVal_eepromAdress_3, calibrationValue_4);

  LoadCell_1.begin();
  LoadCell_2.begin();
  LoadCell_3.begin();
  LoadCell_4.begin();

  //LoadCell_1.setReverseOutput();
  //LoadCell_2.setReverseOutput();
  unsigned long stabilizingtime = 5000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  byte loadcell_1_rdy = 0;
  byte loadcell_2_rdy = 0;
  byte loadcell_3_rdy = 0;
  byte loadcell_4_rdy = 0;
  while ((loadcell_1_rdy + loadcell_2_rdy + loadcell_3_rdy + loadcell_4_rdy) < 4) { //run startup, stabilization and tare, both modules simultaniously
    if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilizingtime, _tare);
    if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilizingtime, _tare);
    if (!loadcell_3_rdy) loadcell_3_rdy = LoadCell_3.startMultiple(stabilizingtime, _tare);
    if (!loadcell_4_rdy) loadcell_4_rdy = LoadCell_4.startMultiple(stabilizingtime, _tare);
  }

  if (LoadCell_1.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 no.1 wiring and pin designations");
  }
  if (LoadCell_2.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 no.2 wiring and pin designations");
  }
  if (LoadCell_3.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 no.3 wiring and pin designations");
  }
  if (LoadCell_4.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 no.4 wiring and pin designations");
  }
  LoadCell_1.setCalFactor(calibrationValue_1); // user set calibration value (float)
  LoadCell_2.setCalFactor(calibrationValue_2); // user set calibration value (float)
  LoadCell_3.setCalFactor(calibrationValue_3); // user set calibration value (float)
  LoadCell_4.setCalFactor(calibrationValue_4); // user set calibration value (float)

  LoadCell_1.setSamplesInUse(samples);
  LoadCell_2.setSamplesInUse(samples);
  LoadCell_3.setSamplesInUse(samples);
  LoadCell_4.setSamplesInUse(samples);

  cellCountSelection();
}

// Turn on the interrupt buttons
void enableInterrupts() {
  attachInterrupt(button1_pin, button1ISR, FALLING);
  attachInterrupt(button2_pin, button2ISR, FALLING);
  //attachInterrupt(button3_pin, button3ISR, FALLING);
  attachInterrupt(button4_pin, button4ISR, FALLING);

}

// Turn off the interrupt buttons
void disableInterrupts() {
  detachInterrupt(digitalPinToInterrupt(button1_pin));
  detachInterrupt(digitalPinToInterrupt(button2_pin));
  //detachInterrupt(digitalPinToInterrupt(button3_pin));
  detachInterrupt(digitalPinToInterrupt(button4_pin));
}

// setBackground calls the appropriate background in LCD.h to show the background for main screen based on the number of load cells that were configured.
void setBackground() {
  switch (loadCellCount) {
    case 1:
      print1Background();
      break;
    case 2:
      print2Background();
      break;
    case 3:
      print3Background();
      break;
    case 4:
      print4Background();
      break;
  }
}

// Configuration menu. Lets user choose how many load cells are connected. Sets loadCellCount to the selection. 
void cellCountSelection() {
  LoadCell_1.powerDown();
  LoadCell_2.powerDown();
  LoadCell_3.powerDown();
  LoadCell_4.powerDown();

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Select number of");
  lcd.setCursor(0, 1);
  lcd.print("connected load cells");
  lcd.setCursor(0, 3);
  lcd.print("1     2      3     4");

  button1Presses = 0;
  button2Presses = 0;
  button3Presses = 0;
  button4Presses = 0;

  int buttons = digitalRead(button1_pin) + digitalRead(button2_pin) + digitalRead(button3_pin) + digitalRead(button4_pin);

  while (buttons < 4) {
    buttons = digitalRead(button1_pin) + digitalRead(button2_pin) + digitalRead(button3_pin) + digitalRead(button4_pin);
  }

  delay(250);

  bool cycle = true;
  bool cancel = false;

  while (cycle) {
    int button1State = digitalRead(button1_pin);
    int button2State = digitalRead(button2_pin);
    int button3State = digitalRead(button3_pin);
    int button4State = digitalRead(button4_pin);

    delay(100);
    
    if (button1State == LOW) {
      while (digitalRead(button1_pin) == LOW){
        continue;
      }
      delay(150);
      cancel = true;
      loadCellCount = 1;
      LoadCell_1.powerUp();
      button1Presses = 0;
      break;
    }

    if (button2State == LOW) {
      while (digitalRead(button1_pin) == LOW) {
        continue;
      }
      delay(150);
      cancel = true;
      loadCellCount = 2;
      LoadCell_1.powerUp();
      LoadCell_2.powerUp();
      button2Presses = 0;
      break;
    }

    if (button3State == LOW) {
      while (digitalRead(button3_pin) == LOW) {
        continue;
      }
      delay(150);
      cancel = true;
      loadCellCount = 3;
      LoadCell_1.powerUp();
      LoadCell_2.powerUp();
      LoadCell_3.powerUp();
      button3Presses = 0;
      break;
    }

    if (button4State == LOW) {
      while (digitalRead(button4_pin) == LOW){
        continue;
      }
      delay(150);
      cancel = true;
      loadCellCount = 4;
      LoadCell_1.powerUp();
      LoadCell_2.powerUp();
      LoadCell_3.powerUp();
      LoadCell_4.powerUp();
      button4Presses = 0;
      break;
    }
  }

  tearAll();

  setBackground();
}

// Tear load cell 1, prints on LCD.
void tear1() {
  lcd.clear();
  printTear1();
  LoadCell_1.tare();
  if (LoadCell_1.getTareStatus() == true) {
    Serial.println("Tare load cell 1 complete");
    printTear1Complete();
  }
}

// Tear load cell 2, prints on LCD.
void tear2() {
  printTear2();
  LoadCell_2.tare();  
  if (LoadCell_2.getTareStatus() == true) {
    Serial.println("Tare load cell 2 complete");
    printTear2Complete();
  }
}

// Tear load cell 3, prints on LCD.
void tear3() {
  printTear3();
  LoadCell_3.tare();
  if (LoadCell_3.getTareStatus() == true) {
    Serial.println("Tare load cell 3 complete");
    printTear3Complete();
  }
}

// Tear load cell 4, prints on LCD.
void tear4() {
  printTear4();
  LoadCell_4.tare();
  if (LoadCell_4.getTareStatus() == true) {
    Serial.println("Tare load cell 4 complete");
    printTear4Complete();
  } 
}

// Tears the number of load cells that are connected based on loadCellCount. Prints on LCD, when returning prints main screen.
void tearAll() {
  switch (loadCellCount) {
    case 1:
      tear1();
      break;
    case 2:
      tear1();
      tear2();
      break;
    case 3:
      tear1();
      tear2();
      tear3();
      break;
    case 4:
      tear1();
      tear2();
      tear3();
      tear4();
  }

  delay(375);

  lcd.clear();
  setBackground();
}

// Calibration procedure for LCD screen, interacts with the four buttons on the device. int LoadCell is which load cell is to be calibrated. 
// Assumed the load cell amplifier is in the powered on state.
void buttonCalibrate(int LoadCell) {
  button1Presses = 0;
  button2Presses = 0;
  button3Presses = 0;
  button4Presses = 0;

  int buttons = digitalRead(button1_pin) + digitalRead(button2_pin) + digitalRead(button3_pin) + digitalRead(button4_pin);

  while (buttons < 4) {
    buttons = digitalRead(button1_pin) + digitalRead(button2_pin) + digitalRead(button3_pin) + digitalRead(button4_pin);
  }

  delay(250);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting Calibration");
  lcd.setCursor(0, 1);
  lcd.print("Place load cell flat");
  lcd.setCursor(2, 2);
  lcd.print("Remove all loads");
  lcd.setCursor(2,3);
  lcd.print("Press - to tare");

  bool cycle = true;
  bool cancel = false;

  while (cycle) {
    int button4State = digitalRead(button4_pin);
    int button1State = digitalRead(button1_pin);

    delay(100);
    
    if (button4State == LOW) {
      while (digitalRead(button4_pin) == LOW){
        continue;
      }
      delay(150);
      break;
    }

    if (button1State == LOW) {
      while (digitalRead(button1_pin) == LOW){
        continue;
      }
      delay(150);
      cancel = true;
      break;
    }
  }

  if (cancel) {
    return;
  }

  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Tareing ");
  lcd.print(LoadCell);
  lcd.print(" ");
  Serial.print("Tareing ");
  Serial.println(LoadCell);
  switch (LoadCell) {
    case 1:
      LoadCell_1.tare();
      break;
    case 2:
      LoadCell_2.tare();
      break;
    case 3:
      LoadCell_3.tare();
      break;
    case 4:
      LoadCell_4.tare();
      break;
  }

  lcd.print("Done");
  Serial.println("Done tareing");

  delay(500);

  lcd.clear();

  lcd.setCursor(1, 0);
  lcd.print("Place a known mass");
  lcd.setCursor(2, 1);
  lcd.print("on the scale and");
  lcd.setCursor(5, 2);
  lcd.print("enter value");
  lcd.setCursor(3, 3);
  lcd.print("Menu to begin");

  cycle = true;
  while (cycle) {
    int button1State = digitalRead(button1_pin);
    
    if (button1State == LOW) {
      while (digitalRead(button1_pin) == LOW){
        continue;
      }
      delay(150);
      cycle = false;
    }
  }

  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Mass: ");
  lcd.setCursor(10, 1);
  lcd.cursor();
  lcd.blink();

  int massValue[] = {0,1,0,0,0};

  cycle = true;

  int cursorPosition = 0;

  lcd.setCursor(10, 1);
  for (int i = 0; i < 5; i++) {
      if (i == 4) {
        lcd.print(".");
      }
      lcd.print(massValue[i]);
    }

  lcd.setCursor(10,1);

  while (cycle) {

    int button1 = digitalRead(button1_pin);
    int button2 = digitalRead(button2_pin);
    int button3 = digitalRead(button3_pin);
    int button4 = digitalRead(button4_pin);

    if (button1 == LOW) {
      cycle = false;
      while (button1 == LOW) {
        button1 = digitalRead(button1_pin);
      }
      delay (150);
      break;
    }

    if (button2 == LOW) {
      lcd.moveCursorRight();
      cursorPosition++;
      if (cursorPosition == 4) {
        lcd.moveCursorRight();
      }
      if (cursorPosition == 5) {
        lcd.setCursor(10, 1);
        cursorPosition = 0;
      }
      while (button2 == LOW) {
        button2 = digitalRead(button2_pin);
      }
      delay (150);
    }

    if (button3 == LOW) {
      massValue[cursorPosition] = (massValue[(cursorPosition)] + 1) % 10;
      lcd.print(massValue[cursorPosition]);
      lcd.moveCursorLeft();
      while (button3 == LOW) {
        button3 = digitalRead(button3_pin);
      }
      delay (150);
    }

    if (button4 == LOW) {
      massValue[cursorPosition] = (massValue[cursorPosition] + 9) % 10;
      lcd.print(massValue[cursorPosition]);
      lcd.moveCursorLeft();
      while (button4 == LOW) {
        button4 = digitalRead(button4_pin);
      }
      delay (150);
    }
  }

  lcd.noBlink();
  lcd.noCursor();
  lcd.clear();

  double weightVal = 0;

  for (int i = 0; i < 4; i++) {
    weightVal = (weightVal * 10) + massValue[i];
  }
  weightVal = weightVal + (((double)massValue[4])/10);

  float newCalibrationValue = 0;
  float oldCalibrationValue = 0;

  lcd.setCursor(4, 1);
  lcd.print("Calculating");
  lcd.setCursor(1, 2);
  lcd.print("calibration value");

  switch (LoadCell) {
    case 1:
      oldCalibrationValue = LoadCell_1.getCalFactor();
      LoadCell_1.refreshDataSet();
      newCalibrationValue = LoadCell_1.getNewCalibration(weightVal);
      break;
    case 2:
      oldCalibrationValue = LoadCell_2.getCalFactor();
      LoadCell_2.refreshDataSet();
      newCalibrationValue = LoadCell_2.getNewCalibration(weightVal);
      break;
    case 3:
      oldCalibrationValue = LoadCell_3.getCalFactor();
      LoadCell_3.refreshDataSet();
      newCalibrationValue = LoadCell_3.getNewCalibration(weightVal);
      break;
    case 4:
      oldCalibrationValue = LoadCell_4.getCalFactor();
      LoadCell_4.refreshDataSet();
      newCalibrationValue = LoadCell_4.getNewCalibration(weightVal);
      break;
  }

  int calVal_eepromAdress = ((LoadCell - 1) * 4);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("New calibration");
  lcd.setCursor(2,1);
  lcd.print("factor: ");
  lcd.print(newCalibrationValue);
  lcd.setCursor(7, 2);
  lcd.print("Save?");
  lcd.setCursor(4, 3);
  lcd.print("Yes:+  No:-");

  cycle = true;

  while (cycle) {
    int button3 = digitalRead(button3_pin);
    int button4 = digitalRead(button4_pin);

    if (button3 == LOW) {
      switch (LoadCell) {
        case 1:
          LoadCell_1.setCalFactor(newCalibrationValue);
          break;
        case 2:
          LoadCell_2.setCalFactor(newCalibrationValue);
          break;
        case 3:
          LoadCell_3.setCalFactor(newCalibrationValue);
          break;
        case 4:
          LoadCell_4.setCalFactor(newCalibrationValue);
          break;
      }

      EEPROM.begin(512);
      EEPROM.put(calVal_eepromAdress, newCalibrationValue);
      EEPROM.commit();

      while (button3 == LOW) {
        button3 = digitalRead(button3_pin);
      }
      delay (150);
      cycle = false;
    }

    if (button4 == LOW) {
      switch (LoadCell) {
        case 1:
          LoadCell_1.setCalFactor(oldCalibrationValue);
          break;
        case 2:
          LoadCell_2.setCalFactor(oldCalibrationValue);
          break;
        case 3:
          LoadCell_3.setCalFactor(oldCalibrationValue);
          break;
        case 4:
          LoadCell_4.setCalFactor(oldCalibrationValue);
          break;
      }
      while (button4 == LOW) {
        button4 = digitalRead(button4_pin);
      }
      delay (150);
      cycle = false;

      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("Calibration");
      lcd.setCursor(5, 2);
      lcd.print("not saved");

      delay(500);

    }
  }
}

// Menu to assist with buttonCalibrate. Lets user select which load cell to calibrate. Only allowed to select load cell if enabled by loadCellCount.
void calibrateMenu() {
  button1Presses = 0;

  Serial.println("Calibrate menu");

  int buttons = digitalRead(button1_pin) + digitalRead(button2_pin) + digitalRead(button3_pin) + digitalRead(button4_pin);

  while (buttons < 4) {
    buttons = digitalRead(button1_pin) + digitalRead(button2_pin) + digitalRead(button3_pin) + digitalRead(button4_pin);
  }

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Select which load");
  lcd.setCursor(1, 1);
  lcd.print("cell to calibrate");
  lcd.setCursor(0, 3);
  
  switch (loadCellCount) {
    case 1:
      lcd.print("1");
      break;
    case 2:
      lcd.print("1     2");
      break;
    case 3:
      lcd.print("1     2      3");
      break;
    case 4:
      lcd.print("1     2      3     4");
  }

  bool cycle = true;

  while (cycle) {
    int button1State = 1;
    int button2State = 1;
    int button3State = 1;
    int button4State = 1;

    switch (loadCellCount) {
      case 4:
        button4State = digitalRead(button4_pin);
      case 3:
        button3State = digitalRead(button3_pin);
      case 2:
        button2State = digitalRead(button2_pin);
      case 1:
        button1State = digitalRead(button1_pin);
    }

    delay(50);

    if (button1State == LOW) {
      while (digitalRead(button1_pin) == LOW){
        continue;
      }
      delay(150);
      buttonCalibrate(1);
      cycle = false;
      break;
    }
    else if (button2State == LOW) {
      while (digitalRead(button2_pin) == LOW){
        continue;
      }
      delay(150);
      buttonCalibrate(2);
      cycle = false;
      break;
    }
    else if (button3State == LOW) {
      while (digitalRead(button3_pin) == LOW){
        continue;
      }
      delay(150);
      buttonCalibrate(3);
      cycle = false;
      break;
    }
    else if (button4State == LOW) {
      while (digitalRead(button4_pin) == LOW){
        continue;
      }
      delay(150);
      buttonCalibrate(4);
      cycle = false;
      break;
    }
  }

  //Serial.print("Button 1:");
  //Serial.println(button1Presses);
  //Serial.print("Button 2:");
  //Serial.println(button2Presses);
  //Serial.print("Button 3:");
  //Serial.println(button3Presses);
  //Serial.print("Button 4:");
  //Serial.println(button4Presses);

  button1Presses = 0;
  button2Presses = 0;
  button3Presses = 0;
  button4Presses = 0;

  lcd.clear();
  setBackground();
}

// Operational section for main screen with one connected load cell. 
void oneLoadCell() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = updateTime;

  if (LoadCell_1.update()) newDataReady = true;

  if ((newDataReady)) {
    if (millis() > t + serialPrintInterval) {
      float LC1 = max(LoadCell_1.getData(), (float) 0);
      printLC11(LC1);
      newDataReady = 0;
      t = millis();
    }
  }
}

// Operational section for main screen with two connected load cells.
void twoLoadCells() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = updateTime;

  if (LoadCell_1.update()) {
    newDataReady = true;
    LoadCell_2.update();
  }

  if ((newDataReady)) {
    if (millis() > t + serialPrintInterval) {
      float LC1 = max(LoadCell_1.getData(), (float) 0);
      float LC2 = max(LoadCell_2.getData(), (float) 0);
      float LCT = LC1 + LC2;
      float LCL = abs(LC1/(LC1+LC2)*100);
      LCL = min(LCL, (float) 100);
      float LCR = 100-LCL;
      printLC12(LC1);
      printLC22(LC2);
      printLR2(LCL, LCR, LCT);
      print2Total(LCT);
      newDataReady = 0;
      t = millis();
    }
  }
}

// Operational section for main screen with three connected load cells.
void threeLoadCells() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = updateTime; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell_1.update()) {
    newDataReady = true;
    LoadCell_2.update();
    LoadCell_3.update();
  }

  //get smoothed value from data set
  if ((newDataReady)) {
    if (millis() > t + serialPrintInterval) {
      float LC1 = max(LoadCell_1.getData(), (float) 0);
      float LC2 = max(LoadCell_2.getData(), (float) 0);
      float LC3 = max(LoadCell_3.getData(), (float) 0);
      float LCL = abs(LC2/(LC2+LC3)) * 100;
      LCL = min(LCL, (float) 100);
      float LCR = 100-LCL;
      float LCF = abs(LC1/(LC2+LC3)*100);
      LCF = min(LCF, (float) 100);
      float LCB = 100-LCF;
      float LCT = LC1+LC2+LC3;
      printLC13(LC1);
      printLC23(LC2);
      printLC33(LC3);
      printLR(LCL,LCR,LC2+LC3);
      printFB(LCF,LCB,LCT);
      printTotal(LCT);
      newDataReady = 0;
      t = millis();
    }
  }
}

// Operational section for main screen with four connected load cells.
void fourLoadCells() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = updateTime; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell_1.update()) {
    newDataReady = true;
    LoadCell_2.update();
    LoadCell_3.update();
    LoadCell_4.update();
  }

  //get smoothed value from data set
  if ((newDataReady)) {
    if (millis() > t + serialPrintInterval) {

      float LC1 = max(LoadCell_1.getData(), (float) 0);
      float LC2 = max(LoadCell_2.getData(), (float) 0);
      float LC3 = max(LoadCell_3.getData(), (float) 0);
      float LC4 = max(LoadCell_4.getData(), (float) 0);
      float LCL = abs((LC1+LC3)/((LC1+LC3)+(LC2+LC4))*100);
      LCL = min(LCL, (float) 100);
      float LCR = 100-LCL;
      float LCF = abs((LC1+LC2)/((LC1+LC2)+(LC3+LC4))*100);
      LCF = min(LCF, (float) 100);
      float LCB = 100-LCF;
      float LCT = LC1+LC2+LC3+LC4;
      if (debug) {
        Serial.print("Load_cell 1 output val: ");
        Serial.print(LC1,0);
        Serial.print("    Load_cell 2 output val: ");
        Serial.print(LC2,0);
        Serial.print("   ");
        Serial.print("Load_cell 3 output val: ");
        Serial.print(LC3,0);
        Serial.print("   ");
        Serial.print("Load_cell 4 output val: ");
        Serial.print(LC4,0);
        Serial.print("     Total Weight: ");
        Serial.print(LCT,0);
        Serial.print("   ");
        Serial.print("Left/right load balance: ");
        if (abs(LCT) > 1) {
          Serial.print(LCL,0);
          Serial.print("/");
          Serial.print(LCR,0);
          Serial.print(" ");
        } else { Serial.print("N/A");}
        Serial.print("   ");
        Serial.print("Front/back load balance: ");
        if (abs(LCT) > 1) {
          Serial.print(LCF,0);
          Serial.print("/");
          Serial.print(LCB,0);
          Serial.print(" ");
        } else { Serial.print("N/A");}
        Serial.println();
      }
      printLC14(LC1);
      printLC24(LC2);
      printLC34(LC3);
      printLC44(LC4);
      printLR(LCL,LCR,LCT);
      printFB(LCF,LCB,LCT);
      printTotal(LCT);
      newDataReady = 0;
      t = millis();
    }
  }
}

// Main loop section of program. Responds to button presses, and calls oneLoadCell - fourLoadCells.
void loop() {

  // If button one is pressed, tear enabled load cells.
  if (button4Presses == 1) {
    disableInterrupts();
    tearAll();
    enableInterrupts();
    button4Presses = 0;
  }

  // Ignore button 3 presses, no current action.
  if (button3Presses == 1) {
    button3Presses = 0;
  }

  // If button two is pressed, change load cell configuration.
  if (button2Presses == 1) {
    disableInterrupts();
    cellCountSelection();
    enableInterrupts();
    button2Presses = 0;
  }

  // If button one is pressed, enter calibrateMenu.
  if (button1Presses == 1) {
    delay(50);
    disableInterrupts();
    calibrateMenu();
    enableInterrupts();
    button1Presses = 0;
  }

  // Updated enabled load cells, based on loadCellCount
  switch (loadCellCount) {
    case 1:
      oneLoadCell();
      break;
    case 2:
      twoLoadCells();
      break;
    case 3:
      threeLoadCells();
      break;
    case 4:
      fourLoadCells();
      break;
  }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      tearAll();
    }
    else if (inByte == 's') {
      getSamples();
    }
  }
}
