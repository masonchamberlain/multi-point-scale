#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

const int LCD_COLS = 20;
const int LCD_ROWS = 4;

// Screen displayed on startup
void splashScreen() {
  lcd.setCursor(1, 0);
  lcd.print("Multi-point scale");
  lcd.setCursor(3, 1);
  lcd.print("Version 0.3.1");
  lcd.setCursor(3, 2);
  lcd.print("Designed by");
  lcd.setCursor(6, 3);
  lcd.print("Mason C");
  delay(1250);
}

// Print main screen background with one connected load cell.
void print1Background() {
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.print("LDC1: ");
}

// Print reading of load cell one with one configured load cell.
void printLC11(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(11, 1);
  lcd.print("      ");
  lcd.setCursor(11, 1);
  lcd.print(rounded,0);
}

// Print main screen background with two connected load cells.
void print2Background() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LDC1: ");
  lcd.setCursor(10, 0);
  lcd.print("LDC2: ");
  lcd.setCursor(5,1);
  lcd.print("L/R: ");
  lcd.setCursor(3, 3);
  lcd.print("Total: ");
}

// Print reading of load cell one with two configured load cells.
void printLC12 (float reading) {
  int rounded = (int) reading;
  lcd.setCursor(5,0);
  lcd.print("     ");
  lcd.setCursor(5, 0);
  lcd.print(rounded,0);
}

// Print reading of load cell two with two configured load cells.
void printLC22 (float reading) {
  int rounded = (int) reading;
  lcd.setCursor(15,0);
  lcd.print("      ");
  lcd.setCursor(15, 0);
  lcd.print(rounded,0);
}

// print main screen background with three connected load cells.
void print3Background() {
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("LDC1:");
  lcd.setCursor(0, 1);
  lcd.print("LDC2:");
  lcd.setCursor(10, 1);
  lcd.print("LDC3:");
  lcd.setCursor(0, 2);
  lcd.print("L/R: ");
  lcd.setCursor(10, 2);
  lcd.print("F/B: ");
  lcd.setCursor(3, 3);
  lcd.print("Total: ");
}

// Print reading of load cell one with three configured load cells.
void printLC13(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(11, 0);
  lcd.print("     ");
  lcd.setCursor(11, 0);
  lcd.print(rounded, 0);
}

// Print reading of load cell two with three configured load cells.
void printLC23(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(5, 1);
  lcd.print("     ");
  lcd.setCursor(5, 1);
  lcd.print(rounded, 0);
}

// Print reading of load cell three with three configured load cells.
void printLC33(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(15, 1);
  lcd.print("     ");
  lcd.setCursor(15, 1);
  lcd.print(rounded, 0);
}

// Print main screen background with four connected load cells.
void print4Background() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LDC1: ");
  lcd.setCursor(10, 0);
  lcd.print("LDC2: ");
  lcd.setCursor(0, 1);
  lcd.print("LDC3: ");
  lcd.setCursor(10, 1);
  lcd.print("LDC4: ");
  lcd.setCursor(0, 2);
  lcd.print("L/R: ");
  lcd.setCursor(10, 2);
  lcd.print("F/B: ");
  lcd.setCursor(3, 3);
  lcd.print("Total: ");
}

// Print reading of load cell one with four connected load cells.
void printLC14(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(5,0);
  lcd.print("     ");
  lcd.setCursor(5, 0);
  lcd.print(rounded,0);
}

// Print reading of load cell two with four connected load cells.
void printLC24(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(15,0);
  lcd.print("      ");
  lcd.setCursor(15, 0);
  lcd.print(rounded,0);
}

// Print reading of load cell three with four connected load cells.
void printLC34(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(5,1);
  lcd.print("     ");
  lcd.setCursor(5, 1);
  lcd.print(rounded,0);
}

// Print reading of load cell four with four connected load cells.
void printLC44(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(15,1);
  lcd.print("    ");
  lcd.setCursor(15, 1);
  lcd.print(rounded,0);
}

// Print left right balance of load cells with two configured load cells.
// left is the left load cell, right is the right load cell, total is the sum of left and right load cell. Balance is printed as n/a if total < 2.
void printLR2 (float left, float right, float total) {
  lcd.setCursor(10, 1);
  lcd.print("        ");
  lcd.setCursor(10, 1);
  if (total > 2) {
    lcd.print(left,0);
    lcd.print("/");
    lcd.print(right,0);
  } else {
    lcd.print("n/a");
  }
}

// Print total mass with two connected load cells.
void print2Total(float total) {
  int rounded = (int) total;
  lcd.setCursor(10, 3);
  lcd.print("      ");
  lcd.setCursor(10, 3);
  lcd.print(rounded,0);
}

// Print reading of left/right balance with three or four connected load cells.
// left is the left load cell(s), right is the right load cell(s), total is the sum of left and right load cells. Balance is printed as n/a if total < 2.
void printLR(float left, float right, float total) {
  lcd.setCursor(0, 2);
  lcd.print("L/R:      ");
  lcd.setCursor(4, 2);
  if (total > 2) {
    lcd.print(left,0);
    lcd.print("/");
    lcd.print(right,0);
  } else {
    lcd.print("n/a");
  }
}

// Print reading of front/back balance with three or four connected load cells.
// front is the front load cell(s), right is the right load cell(s), total is the sum of left and right load cells. Balance is printed as n/a if total < 2.
void printFB(float front, float back, float total) {
  lcd.setCursor(14, 2);
  lcd.print("     ");
  lcd.setCursor(14, 2);
  if (total > 2) {
    lcd.print(front,0);
    lcd.print("/");
    lcd.print(back,0);
  } else {
    lcd.print("n/a");
  }
}

// Print total sum of connected load cells. Used with three or four connected load cells.
void printTotal(float total) {
  int rounded = (int) total;
  lcd.setCursor(12, 3);
  lcd.print("      ");
  lcd.setCursor(12, 3);
  lcd.print(rounded,0);
}

// Print actively tareing load cell one.
void printTear1() {
  lcd.setCursor(2, 0);
  lcd.print("Tearing 1:");
}

// Print load cell one tareing complete. Requires printTear1 first.
void printTear1Complete(){
  lcd.setCursor(14, 0);
  lcd.print("Done");
}

// Print actively tareing load cell two.
void printTear2() {
  lcd.setCursor(2, 1);
  lcd.print("Tearing 2:");
}

// Print load cell two tareing complete. Requires printTear2 first.
void printTear2Complete(){
  lcd.setCursor(14, 1);
  lcd.print("Done");
}

// Print actively tareing load cell three.
void printTear3() {
  lcd.setCursor(2, 2);
  lcd.print("Tearing 3:");
}

// Print load cell three tareing complete. Requires printTear3 first.
void printTear3Complete(){
  lcd.setCursor(14, 2);
  lcd.print("Done");
}

// Print actively tareing load cell four.
void printTear4() {
  lcd.setCursor(2, 3);
  lcd.print("Tearing 4:");
}

// Print load cell four tareing complete. Requires printTear4 first.
void printTear4Complete(){
  lcd.setCursor(14, 3);
  lcd.print("Done");
  delay(500);
}
