# Arduino & C++ Climate Control System

This project implements a serial communication system between a Linux C++ application and an Arduino UNO. It allows for real-time monitoring of temperature and humidity, as well as remote control of a fan and an alarm buzzer.

## 📋 Features

*   **Real-time Monitoring**: Reads Temperature and Humidity from a DHT11 sensor.
*   **Remote Control**: Toggle a Fan and an Alarm (Buzzer) from the PC.
*   **LCD Feedback**: Displays current status and sensor readings on a 16x2 I2C LCD.
*   **Menu-Driven Interface**: Simple console-based UI on the Linux side.
*   **Robust Serial Communication**: Handles timeouts and connection stability.

## 🛠️ Hardware Requirements

*   Arduino UNO (or compatible)
*   DHT11 Temperature & Humidity Sensor
*   I2C LCD Display (16x2)
*   Fan (connected via transistor/relay)
*   Active Buzzer
*   Jumper Wires & Breadboard

### Pinout Configuration

| Component | Arduino Pin |
| :--- | :--- |
| **DHT11 Data** | D7 |
| **Fan** | D8 |
| **Buzzer** | D9 |
| **LCD SDA** | A4 |
| **LCD SCL** | A5 |

## 🚀 Installation & Usage

### 1. Arduino Setup
1.  Open `temperatra/temperatura/temperatura.ino` in the Arduino IDE.
2.  Install the required libraries via the Library Manager:
    *   `LiquidCrystal_I2C`
    *   `DHT sensor library`
    *   `Adafruit Unified Sensor`
3.  Select your board and port, then **Upload** the sketch.

### 2. C++ Application (Linux)
1.  Open a terminal in the project root.
2.  Compile the application:
    ```bash
    g++ main.cpp -o app
    ```
3.  Run the application (ensure your Arduino is connected):
    ```bash
    ./app
    ```
    *Note: You may need `sudo ./app` if your user lacks serial port permissions.*

## 🎮 How to Use

Once the application is running, you will see a menu:

1.  **Toggle Monitoring**: Starts/Stops reading sensor data.
2.  **Toggle Fan**: Turns the fan ON or OFF.
3.  **Toggle Alarm**: Turns the buzzer ON or OFF.
4.  **RECEIVE DATA**: Fetches the latest temperature and humidity reading from the Arduino.
0.  **Exit**: Closes the connection and application.

## 📁 Project Structure

*   `main.cpp`: The Linux client application handling serial communication and user interface.
*   `temperatra/temperatura/`: Contains the Arduino source code.
*   `sketch_jan28a/`: Legacy/Test code (experimental).

## ⚠️ Troubleshooting

*   **Permission Denied**: If you can't open the port, run:
    ```bash
    sudo usermod -a -G dialout $USER
    ```
    Then log out and log back in.
*   **Port Not Found**: Check which port your Arduino is using (`ls /dev/tty*`) and update the `PORT_NAME` constant in `main.cpp`.

<footer align="center">
  Diego Santos
<br/>
  Product Engineer
</footer>