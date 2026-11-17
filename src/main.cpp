#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup()
{
  Serial.begin(115200);
  if (!bmp.begin())
  {
    Serial.println("BMP180 not detected");
    while (true)
    {
      delay(100);
    }
  }
  Serial.println("BMP180 detected");
}

void loop()
{
  delay(1000);
}