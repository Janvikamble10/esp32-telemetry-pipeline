#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

struct SensorSample
{
  float temperatureC;
  int32_t pressurePa;
};

Adafruit_BMP085 bmp;

QueueHandle_t sampleQueue = nullptr;

void sensorTask(void *parameter)
{
  while (true)
  {
    SensorSample sample;

    sample.temperatureC = bmp.readTemperature();
    sample.pressurePa = bmp.readPressure();

    if (xQueueSend(sampleQueue, &sample, pdMS_TO_TICKS(100)) != pdPASS)
    {
      Serial.println("Warning: sample queue full");
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void reportTask(void *parameter)
{
  SensorSample sample;

  while (true)
  {
    if (xQueueReceive(sampleQueue, &sample, portMAX_DELAY) == pdPASS)
    {
      Serial.print("Temperature: ");
      Serial.print(sample.temperatureC);
      Serial.println("C");

      Serial.print("Pressure: ");
      Serial.print(sample.pressurePa / 100.0);
      Serial.println("hPa");

      Serial.println("---");
    }
  }
}

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

  sampleQueue = xQueueCreate(10, sizeof(SensorSample));
  if (sampleQueue == nullptr)
  {
    Serial.println("Failed to create sample queue");
    while (true)
    {
      delay(100);
    }
  }

  TaskHandle_t reportTaskHandle = nullptr;

  BaseType_t reportTaskResult = xTaskCreate(reportTask, "ReportTask", 4096, nullptr, 1, &reportTaskHandle);
  if (reportTaskResult != pdPASS)
  {
    Serial.println(" Failed to create repot task");
    while (true)
    {
      delay(100);
    }
  }

  BaseType_t sensorTaskResult = xTaskCreate(sensorTask, "SensorTask", 4096, nullptr, 1, nullptr);

  if (sensorTaskResult != pdPASS)
  {
    vTaskDelete(reportTaskHandle);
    Serial.println("failed to create sensor task");
    while (true)
    {
      delay(100);
    }
  }
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}
