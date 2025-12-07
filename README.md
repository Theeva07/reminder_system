Smart Room Comfort Reminder System

This project uses a XIAO ESP32-C3 and a DHT11 sensor to monitor room temperature and humidity.
If the values exceed comfort limits, an LED and buzzer activate as a reminder.
A push button is used to reset the alarm.

Features

- Reads temperature & humidity every 2 seconds

- Activates alarm when:

   - Temperature > 31°C
   - Humidity < 40%

- LED blinks + buzzer ON during alarm

- Button press clears the alarm

- Simple, fully functional ESP-IDF project

Hardware Connections

- DHT11 → D0 (GPIO2)

- LED → D2 (GPIO4)

- Buzzer → D3 (GPIO5)

- Button → D1 (GPIO3)

- 3V3 and GND for power

How to Run

- Open the project in VS Code with ESP-IDF

- Build → Flash → Monitor

- Observe readings and alarm behaviour in the terminal

- Press button to silence alarm

Files

- main/main.c – program logic

- CMakeLists.txt

- sdkconfig

- README.md
