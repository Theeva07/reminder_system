Smart Room Comfort Reminder System (ESP32-C3 XIAO)

A simple IoT-based room comfort monitoring system using the Seeed Studio XIAO ESP32-C3, DHT11 sensor, LED, Buzzer, and Push Button.
This system monitors temperature and humidity and alerts the user when the room becomes uncomfortable.

Project Overview

This project measures room temperature and humidity using a DHT11 sensor.
If the values exceed a defined threshold, the system:

Activates LED blinking

Activates buzzer alarm

Logs warning messages to the serial monitor

Allows alarm silence using a push button

This serves as a simple embedded system, sensor integration, and alert mechanism demonstration suitable for coursework.

Hardware Used
Component	Purpose
Seeed Studio XIAO ESP32-C3	Microcontroller running the system
DHT11 sensor (Grove-type)	Reads temperature & humidity
LED	Visual alert (blinks on alarm)
Buzzer	Audible alarm
Push Button	User input to silence alarm
Jumper wires	Connections
🛠 Pin Connections
Component	XIAO ESP32-C3 Pin
DHT11 - Signal	D0 → GPIO2
LED	D2 → GPIO4
Buzzer	D3 → GPIO5
Push Button	D1 → GPIO3
3V3 / GND	To respective components
Threshold Settings

You can adjust the alert levels in the code:

#define TEMP_LIMIT_C      31   // Trigger alarm above 31°C
#define HUMIDITY_LOW_PCT  40   // Trigger alarm below 40% RH

System Behaviour
Normal Mode

LED OFF

Buzzer OFF

Sensor readings logged every 2 seconds

Alarm Mode

Triggered when:

Temperature > threshold, OR

Humidity < threshold

System actions:

LED blinks every 200 ms

Buzzer ON

Warning printed in Serial Monitor

Silencing the Alarm

Press the button → alarm resets.

Code Structure

Main logic located in:

/main/main.c


Contains:

Custom DHT11 driver

GPIO configuration

Alert logic

Main loop (100ms cycle)

How to Build & Flash

Open the project in VS Code with ESP-IDF extension

Connect XIAO ESP32-C3 via USB

Click:

ESP-IDF: Build
ESP-IDF: Flash
ESP-IDF: Monitor


Reset board → system starts automatically

Sample Log Output
I (1234) ROOM_REMINDER: DHT11 OK - Temp: 32 C, Humidity: 38 %
W (1235) ROOM_REMINDER: Comfort limit exceeded! Alarm ON
I (2345) ROOM_REMINDER: Button pressed: Alarm cleared

Demo Video

A video demonstrating hardware setup and functionality is included separately as part of the submitted coursework.

Repository Structure
reminder_system/
 ├── main/
 │   ├── main.c
 │   └── CMakeLists.txt
 ├── README.md
 ├── sdkconfig
 └── .gitignore

Author

Theevashini Thankaraj
Master of Electronic Engineering Systems

