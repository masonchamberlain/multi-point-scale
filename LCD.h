#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

const int LCD_COLS = 20;
const int LCD_ROWS = 4;

void splashScreen() {
  lcd.setCursor(1, 0);
  lcd.print("Multi-point scale");
  lcd.setCursor(3, 1);
  lcd.print("Version 0.1.9");
  lcd.setCursor(3, 2);
  lcd.print("Designed by");
  lcd.setCursor(6, 3);
  lcd.print("Mason C");
}

void print4Background()
{
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

void printLC1(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(5,0);
  lcd.print("     ");
  lcd.setCursor(5, 0);
  lcd.print(rounded,0);
}

void printLC2(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(15,0);
  lcd.print("      ");
  lcd.setCursor(15, 0);
  lcd.print(rounded,0);
}

void printLC3(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(5,1);
  lcd.print("     ");
  lcd.setCursor(5, 1);
  lcd.print(rounded,0);
}

void printLC4(float reading) {
  int rounded = (int) reading;
  lcd.setCursor(15,1);
  lcd.print("    ");
  lcd.setCursor(15, 1);
  lcd.print(rounded,0);
}

void printLR(float left, float right, float total) {
  lcd.setCursor(0, 2);
  lcd.print("L/R:     ");
  lcd.setCursor(4, 2);
  if (total > 2) {
    lcd.print(left,0);
    lcd.print("/");
    lcd.print(right,0);
  } else {
    lcd.print("n/a");
  }
}

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

void printTotal(float total) {
  int rounded = (int) total;
  lcd.setCursor(12, 3);
  lcd.print("      ");
  lcd.setCursor(12, 3);
  lcd.print(rounded,0);
}

void printTear1() {
  lcd.setCursor(2, 0);
  lcd.print("Tearing 1:");
}

void printTear1Complete(){
  lcd.setCursor(14, 0);
  lcd.print("Done");
}

void printTear2() {
  lcd.setCursor(2, 1);
  lcd.print("Tearing 2:");
}

void printTear2Complete(){
  lcd.setCursor(14, 1);
  lcd.print("Done");
}

void printTear3() {
  lcd.setCursor(2, 2);
  lcd.print("Tearing 3:");
}

void printTear3Complete(){
  lcd.setCursor(14, 2);
  lcd.print("Done");
}

void printTear4() {
  lcd.setCursor(2, 3);
  lcd.print("Tearing 4:");
}

void printTear4Complete(){
  lcd.setCursor(14, 3);
  lcd.print("Done");
  delay(500);
}
