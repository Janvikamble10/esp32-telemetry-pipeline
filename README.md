# ESP32 FreeRTOS Sensor Telemetry Pipeline

A simulated ESP32 firmware project that reads temperature and pressure from a BMP180 sensor over I2C. A FreeRTOS sensor task sends numbered readings through a bounded queue to a reporting task, which prints them and labels temperatures at or above 30 °C as high. If the queue stays full, the new reading is dropped and the sensor task prints a warning with its sample number. Both tasks use a mutex to keep their Serial output from overlapping.

The circuit runs in Wokwi. The project has not been tested on physical hardware.

## Architecture
**Data flow**

1. The BMP180 sends temperature and pressure readings to `sensorTask` over I2C.
2. `sensorTask` numbers each reading and sends it to the queue.
3. `reportTask` receives queued readings, checks the temperature threshold, and prints the results to Serial.
4. If the queue stays full for 100 ms, `sensorTask` drops that reading and prints a warning with its sample number.
5. Both tasks use `serialMutex` while printing, so their Serial messages do not overlap.

## Build and run

- Open this project folder in VS Code.
- Use PlatformIO's **Build** button to compile the firmware.
- After a successful build, press **Ctrl+Shift+P** and select **Wokwi: Start Simulator**. If the simulator is already running, stop and restart it to load the new firmware.
- Read the output in the Wokwi terminal. The serial speed is 115200 baud.
- Use the BMP180 controls in Wokwi to change the simulated temperature and pressure.

PlatformIO downloads the libraries listed in `platformio.ini`. No physical ESP32 or sensor is used for this simulation.

## Verification performed

- The current version built successfully with PlatformIO.
- After adding the Serial mutex, Wokwi detected the BMP180 and printed consecutive numbered samples with temperature and pressure readings.
- Earlier manual Wokwi checks confirmed that changing the BMP180 controls changed the readings, the temperature status changed across the 30 °C threshold, and disconnecting then restoring the SDA line changed the sensor detection result as expected.
- Before adding the Serial mutex, a temporary delay in the reporting task filled the queue and produced dropped-sample warnings. The delay was removed afterward. The queue-full scenario has not been retested with the mutex change.

These checks were performed manually in Wokwi. The project does not yet include automated tests or a physical-hardware test.

## Design settings and limitations

- The sensor task takes a reading about every 2 seconds.
- The queue holds up to 10 `SensorSample` items.
- Sending waits up to 100 ms for queue space. If the queue remains full, that sample is dropped and a warning is printed.
- Both tasks use `serialMutex` while printing, so their messages do not overlap.
- Temperatures at or above 30 °C are labeled high. This is a demonstration threshold, not a calibrated application requirement.
- The project has been built and exercised in Wokwi simulation; it has not been tested on physical hardware.
- Checks were performed manually. There is no automated test suite or CI build yet.