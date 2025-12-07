**Smart Room Comfort Reminder System

(ESP32-C3 + DHT11 + LED + Buzzer + Push Button)**

This project is a simple embedded reminder system built using the XIAO ESP32-C3 microcontroller. It continuously monitors temperature and humidity using a DHT11 sensor and alerts the user through an LED and buzzer when comfort limits are exceeded. A push button allows the user to clear the alarm manually.

This project was developed for my AES coursework submission.

Features

Reads temperature and humidity every 2 seconds

Triggers an alarm when:

Temperature rises above the limit

Humidity drops below the limit

LED blinks and buzzer turns ON during alarm

Push button resets the alarm

Uses a custom DHT11 driver

Fully implemented in ESP-IDF (C) on XIAO ESP32-C3

Hardware Connections

DHT11 signal connected to D0 (GPIO2)

LED connected to D2 (GPIO4)

Buzzer connected to D3 (GPIO5)

Push button connected to D1 (GPIO3)

Power provided via 3V3 and GND pins

Thresholds Used

Temperature limit: 31°C

Humidity limit: 40% RH

System raises an alarm when temperature exceeds the limit or humidity drops below it

System Behaviour

Under normal conditions, LED and buzzer remain OFF

If temperature or humidity crosses the threshold:

LED begins blinking

Buzzer turns ON

A warning message is printed on the serial monitor

When the push button is pressed:

The alarm turns OFF

LED and buzzer return to idle state

How the System Works

The system initializes all GPIO pins at startup.

Every 2 seconds, the DHT11 sensor is read.

The temperature and humidity values are printed to the serial monitor.

If comfort levels are exceeded, an alarm is activated.

The push button allows the user to silence the alarm.

How to Build and Flash (ESP-IDF)

Open the project in VS Code with the ESP-IDF extension installed.

Select the correct target (ESP32-C3).

Click:

Build Project

Flash Device

Monitor Device

Reset the board to start the program.

Project Structure

main/main.c – contains the complete application logic

main/CMakeLists.txt – build configuration

sdkconfig – ESP-IDF configuration file

.gitignore and other workspace settings

README.md – documentation

Coursework Summary

The Smart Room Comfort Reminder System is designed to measure temperature and humidity and produce an audio-visual warning when comfort levels fall outside acceptable ranges. This project demonstrates sensor integration, GPIO control, and basic embedded system design using ESP-IDF.

Author

Theevashini Thankaraj
GitHub Repository:
👉 https://github.com/Theeva07/reminder_system
