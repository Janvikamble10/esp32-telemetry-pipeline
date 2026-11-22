# ESP32 FreeRTOS Sensor Telemetry Pipeline

A simulated ESP32 firmware project that reads temperature and pressure from a BMP180 sensor over I2C. A FreeRTOS sensor task sends sequence-numbered readings through a bounded queue to a reporting task, which prints the data and applies a demonstration 30 °C temperature threshold.

The circuit runs in Wokwi; this project has not been tested on physical hardware. 

## Architecture

BMP180 sensor --I2C--> sensorTask --SensorSample--> FreeRTOS queue --> reportTask --> Serial output

- `sensorTask` reads temperature and pressure about every two seconds. It gives each reading a sequence number and sends it to the queue.
- The queue holds up to 10 samples. If it stays full, sending times out after 100 ms and the sensor task reports a dropped sample.
- `reportTask` waits for a sample, prints it, and labels temperatures at or above 30 °C as high.

## Build and run

- Open this project folder in VS Code.
- Use PlatformIO's **Build** button to compile the firmware.
- Press **Ctrl+Shift+P**, select **Wokwi: Start Simulator**, and wait for the virtual ESP32 to start.
- Read the output in the Wokwi terminal. The serial speed is 115200 baud.
- Use the BMP180 controls in Wokwi to change the simulated temperature and pressure.

PlatformIO downloads the libraries listed in `platformio.ini`. No physical ESP32 or sensor is used for this simulation.


## Verification performed

- PlatformIO Build completed successfully.
- Wokwi detected the BMP180 and printed temperature and pressure readings.
- Changing the BMP180 controls changed the printed readings.
- The temperature status changed between normal and high when the simulated temperature crossed the 30 °C threshold.
- A temporary delay in the reporting task caused the queue to fill; the firmware showed queue-full warnings and dropped readings. The temporary delay was then removed.
- Disconnecting the BMP180 SDA line caused the firmware to print `BMP180 not detected`. Restoring the connection returned the simulation to normal operation.

These checks were performed manually in Wokwi. The project does not yet include automated tests or a physical-hardware test.

## Design settings and limitations

- The sensor task takes a reading about every 2 seconds.
- The queue holds up to 10 `SensorSample` items.
- Sending waits up to 100 ms for queue space. If it remains full, that sample is dropped and a warning is printed.
- Temperatures at or above 30 °C are labeled high. This is a demonstration threshold, not a calibrated application requirement.
- The project has been built and exercised in Wokwi simulation; it has not been tested on physical hardware.
- The checks described above were manual. There is no automated test suite or CI build yet.