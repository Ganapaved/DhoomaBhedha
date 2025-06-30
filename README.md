# 🐝 Smart Beehive Monitoring System

The **Smart Beehive Monitoring System** is an IoT-based project designed to monitor environmental and hive conditions in real time. This system uses sensors to detect temperature, humidity, smoke, weight, and bee activity. It controls a fan automatically, sends live data to the **Blynk mobile app**, and logs everything to a **Flask-based web dashboard** with predictions and insights.

---

## 📌 Table of Contents

- [🔧 Project Overview](#-project-overview)
- [📦 Components Used](#-components-used)
- [🧠 System Architecture](#-system-architecture)
- [📁 Folder Structure](#-folder-structure)


---

## 🔧 Project Overview

Honeybees are essential for biodiversity and agriculture. However, changes in temperature, smoke, or congestion can cause stress and reduce hive productivity. Our project provides:

- Real-time hive monitoring
- Automatic ventilation (fan control)
- Activity tracking
- Data visualization and storage
- Predictive alerts for potential gas hazards

---

## 📦 Components Used

| Component            | Quantity | Description                                      |
|----------------------|----------|--------------------------------------------------|
| ESP32 Dev Board      | 1        | Microcontroller with Wi-Fi                      |
| DHT11 Sensor         | 1        | Measures temperature and humidity               |
| MQ2 Smoke Sensor     | 1        | Detects smoke and gas presence                  |
| HX711 Load Cell (20kg)| 1       | Measures hive weight (honey production)         |
| LDR Sensor           | 1        | Detects light fluctuations (bee movement)       |
| Relay Module         | 1        | Controls the fan based on sensor data          |
| Cooling Fan          | 1        | Helps ventilate the hive                        |
| Jumper Wires         | –        | For connections                                 |
| Power Supply         | 1        | To power the ESP32 and fan                      |

---

## 🧠 System Architecture

```plaintext
+-----------------+       Wi-Fi        +-------------------+       Charts/UI
|    ESP32 Node   | -----------------> |   Flask Backend    | ----------------> HTML Dashboard
| (Sensors & Fan) |                    |   (API + SQLite)   |
+--------+--------+                    +----------+--------+
         |                                       |
         |                                       |
         v                                       v
   Blynk App (Mobile)                      Gas Prediction Engine

---

 SmartBeehive/
         ├── smart_beehive.ino         # Arduino code for ESP32
         ├── app.py                    # Flask application
         ├── db_init.py                # Database models
         ├── model.py                  # Gas prediction logic
         ├── templates/                # HTML pages
         │   ├── home.html
         │   ├── dashboard.html
         │   ├── weight.html
         │   └── prediction.html
         ├── static/                   # (Optional) CSS or JS files
         └── README.md                 # Project documentation 
         
