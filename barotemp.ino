
#include <stdlib.h>
#include <LiquidCrystal_I2C.h>
#include <SparkFunBME280.h>
#include <arduino-timer.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
BME280 bme;

auto timer = timer_create_default();
char *version = (char *)"v2.1.0";

#define TIME_SPAN 1
#define BUFFER_SIZE 13
#define SECONDS 900

float pressures[BUFFER_SIZE];
int currentTime;
bool arrayInitialized = false;
int ticks = SECONDS;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  initDisplay();
  initBME280();

  displayUnits();  
  
  initTimer();
}

void loop()
{
  timer.tick();
}

void initArray()
{
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    pressures[i] = 0;
  }
  arrayInitialized = true;
}

void initDisplay()
{
  lcd.init(); // initialize the lcd
  lcd.backlight();
}

void displayUnits()
{
  lcd.setCursor(0, 0);
  lcd.print("p:");
  lcd.setCursor(0, 1);
  lcd.print("t:");
  lcd.setCursor(0, 2);
  lcd.print("h:");

  rightAlign(3, version);
}

void initBME280()
{
  bme.setI2CAddress(0x76);
  if (!bme.begin())
  {
    printError((char *)"bme280");
  }
  /* first pressure reading is off by ~3%, reading temp & humidity first fixes this */
  bme.readTempC();
  bme.readFloatHumidity();
  readPressure();
}

void initTimer()
{
  timer.every(1000, tick);
}

bool tick(void *) {
  float pressure = readPressure();
  updateDisplay(pressure);
  measure(pressure);
  return true;
}

void writePressure(float pressure)
{
  char value[8];
  dtostrf(pressure, 6, 1, value);

  lcd.setCursor(2, 0);
  lcd.print(value);
  lcd.print(" hPa");
}

void writeTemperature(float temperature)
{
  char value[8];
  dtostrf(temperature, 6, 1, value);
  lcd.setCursor(2, 1);
  lcd.print(value);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");
}

void writeHumidity(float humidity)
{
  char value[8];
  dtostrf(humidity, 6, 1, value);
  lcd.setCursor(2, 2);
  lcd.print(value);
  lcd.print(" %");
}

void blink(int millis)
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(millis);
  digitalWrite(LED_BUILTIN, LOW);
}

void rightAlign(int row, char *text)
{
  lcd.setCursor(20 - strlen(text), 3);
  lcd.print(text);
}

void printError(char *text)
{
  lcd.setCursor(0, 3);
  lcd.print("e: ");
  lcd.print(text);
}

bool updateDisplay(float pressure)
{
  float temperature = bme.readTempC();
  float humidity = bme.readFloatHumidity();

  writeTemperature(temperature);
  writeHumidity(humidity);
  writePressure(pressure);

  blink(25);

  return true;
}

bool measure(float pressure)
{
  if (ticks < SECONDS)
  {
    ticks++;
    return true;
  }
  else
    ticks = 0;

  if (!arrayInitialized)
  {
    initArray();
  }

  currentTime = currentTime % BUFFER_SIZE;
  pressures[currentTime] = pressure;

  displayDeltas();

  currentTime++;

  return true;
}

void displayDeltas()
{

  lcd.setCursor(13, 0);
  lcd.print((char *)"1: ");
  displayDelta(calcDelta(4));

  lcd.setCursor(13, 1);
  lcd.print((char *)"3: ");
  displayDelta(calcDelta(12));
}

void displayDelta(float delta) {
  char value[5];
  dtostrf(delta, 1, 1, value);
  if (delta >= 0)
    lcd.print((char *)" ");
  lcd.print(value);
}

float calcDelta(int periods)
{
  int prev = currentTime - periods;
  if (prev < 0)
    prev += BUFFER_SIZE;

  if (pressures[prev] == 0)
  {
    return 0;
  }

  return pressures[currentTime] - pressures[prev];
}

float readPressure() {
  return bme.readFloatPressure() / 100;  
}
