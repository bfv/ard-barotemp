
#define ARDUINO 101

#include <stdlib.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MPL3115A2.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

void setup()
{
  lcd.init();                      // initialize the lcd 

  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("p:");
  lcd.setCursor(0, 1);
  lcd.print("T:");

  pinMode(LED_BUILTIN, OUTPUT);

  // lcd.setCursor(0, 3);
  // lcd.print("MPL3115A2 sensor");
}


void loop()
{
  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    return;
  }

  Serial.println("init ok...");

  float pressure = baro.getPressure() / 100;
  float temperature = baro.getTemperature();

  writePressure(pressure);
  writeTemperature(temperature);
  blink(25);
  delay(975);
}

void writePressure(float pressure) {
  char value[8];
  dtostrf(pressure, 6, 1, value);

  lcd.setCursor(2, 0);
  lcd.print(value);
  lcd.print(" hPa");
}

void writeTemperature(float temperature) {
  char value[8];
  dtostrf(temperature, 6, 1, value);
  lcd.setCursor(2, 1);
  lcd.print(value);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");
}

void blink(int millis) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(millis);
  digitalWrite(LED_BUILTIN, LOW);
}
