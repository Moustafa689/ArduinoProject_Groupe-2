# Automated Greenhouse System — Groupe 2 🌿
**Lebanese University — Faculty of Engineering III** **Course:** Sensors & Instrumentation  
**Instructor:** Dr. Mohamad Ahmad Aoude  


## 📂 GitHub Repository Structure

In accordance with the project guidelines, this repository is organized as follows:

* 📁 [**`README.md`**](./README.md) — This main documentation page.
* 📁 [**`/Docs`**](./Docs) — Project schematics, wiring diagrams, and technical documentation.
* 📁 [**`/code`**](./code) — The implementation source code for the Arduino.
* 📁 [**`/video`**](./video) — Project demonstration video references.
* 📁 [**`/Reports`**](./Reports) — The Main Project Report alongside all individual group member reports.

---
## 📋 Project Subsystems & Video Outline

Our integrated greenhouse automation system is broken down into five key subsystems:

1. **System Overview** – Overall architecture, hardware layout, and sensor placement.
2. **Security & Alarm System** – Intrusion detection utilizing a motion sensor and active buzzer.
3. **Climate Control** – Ambient temperature and humidity regulation via a DHT11 sensor and fan.
4. **Automated Irrigation** – Soil moisture analysis controlling a self-triggering water pump.
5. **Weather & Light Response** – Light (LDR) and rain detection driving a servo-actuated roof.

---

## 📺 Project Demonstration Video

Click the video player thumbnail below to watch our complete system run-through and see all the sensors and actuators in action:

[![Watch the Video](https://img.youtube.com/vi/gns5-Lyf4sU/0.jpg)](https://youtu.be/gns5-Lyf4sU)

*(The demonstration details can also be found in the [**`/video`**](./video) directory).*

---

## 💻 Source Code

The fully commented and modular Arduino codebase is stored in the code directory:

* 📄 [**`/code/Project_code.ino`**](./code/Project_code.ino) — Click here to view the main logic, pin mappings, and loop controls.

---

### 📦 Required Arduino Libraries
To compile the code successfully, make sure to install the following libraries included in the [**`/code/libraries`**](./code/libraries) folder:
1. **DHT Sensor Library** by Adafruit (v1.4.6 or later) — [Download Link](./code/libraries/DHT_sensor_library.zip)
2. **Adafruit Unified Sensor** (Dependency) — [Download Link](./code/libraries/Adafruit_Sensor.zip)
3. **Servo** (Built-in Arduino Library)

*To install these in the Arduino IDE, go to **Sketch > Include Library > Add .ZIP Library...** and select the files from the `/code/libraries` directory.*

---

## 👥 Project Reports & Technical Documentation

All project documentation is compiled and stored inside the [**`/report`**](./report) folder. 

### 📑 Main Project Report
* [📘 **Download Main Project Report (PDF)**](./report/Main_Project_Report.pdf) — *The comprehensive, complete group technical report outlining the entire design, circuitry, budget, and results.*

### 📄 Individual Reports
Each team member took ownership of a specific module and compiled an individual technical analysis:

| Team Member | Subsystem Responsibility | Individual Report Link |
| :--- | :--- | :--- |
| **Razane Najem** | Security Subsystem & Alarm | [📄 View Report](./report/Security_Report.pdf) |
| **Fatima Jaafar** | Climate Control (DHT11 & Fan) | [📄 View Report](./report/Climate_Report.pdf) |
| **Moustafa Alawieh** | Automated Irrigation (Soil & Pump) | [📄 View Report](./report/Irrigation_Report.pdf) |
| **Mohammad injibar** | Weather & Light (LDR/Rain/Servo) | [📄 View Report](./report/Weather_Light_Report.pdf) |

---

## 🛠️ Hardware Schematics

* The complete wiring diagram and schematic can be found in the docs directory:  
  📄 [**`/Docs/Greenhouse_System_Schematic_And_Wiring.fzz`**](./Docs/Greenhouse_System_Schematic_And_Wiring.fzz) (Open with Fritzing).


### 🧩 Custom Fritzing Libraries (`.fzpz`)
If Fritzing complains about missing parts when loading our schematic, please import our custom library parts stored in [**`/docs/fritzing_parts`**](./docs/fritzing_parts):
* **Soil Moisture Sensor Module:** [Download .fzpz](./docs/fritzing_parts/soil_moisture_sensor.fzpz)
* **Rain/Water Sensor Board:** [Download .fzpz](./docs/fritzing_parts/rain_sensor.fzpz)
* **Active Buzzer / PIR Module:** [Download .fzpz](./docs/fritzing_parts/pir_sensor.fzpz)

*To import these in Fritzing, go to **File > Open** and select the `.fzpz` files, or right-click the "Mine" parts bin and select **Import Part**.*

---  
