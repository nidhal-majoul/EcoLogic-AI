# EcoLogic AI: Intelligent Energy Sustainability 🌍💡
**A Cross-Platform AIoT System for Smart Building Efficiency**

Developed by Nidhal Majoul.  
*First-year as a Student at the University of Burgos (UBU)*

---

## 📌 Project Vision
Buildings often consume maximum energy regardless of actual human presence. **EcoLogic AI** is a cyber-physical solution that uses Artificial Intelligence to bridge the gap between high-level Computer Vision and low-level Hardware Control. By sensing occupancy in real-time, the system automatically switches between **ECO** and **NORMAL** modes to optimize energy consumption.

## 🛠️ System Architecture
This project demonstrates **Systems Integration** across three different programming environments:

1. **The Eyes (Python + YOLOv8):** - Uses Computer Vision to capture frames and detect human presence.
   - Streams occupancy data via TCP/IP Sockets.

2. **The Brain (C++ Perceptron):** - A custom-trained Supervised Learning model built from scratch in C++.
   - Processes occupancy and simulated energy data to make a high-speed binary decision (Eco vs. Normal).

3. **The Face (Flask Web Dashboard):** - A real-time UI built with HTML/CSS/JavaScript.
   - Visualizes live occupancy counts and the AI’s current decision state.

4. **The Physical Actuator (ESP32-S3):** - An IoT microcontroller connected via Wi-Fi.
   - Receives HTTP requests from the Python bridge to physically signal the energy mode change.

## 📂 Repository Structure
* `brain_engine/` - Contains the C++ source code for the Perceptron and Socket Server.
* `vision_bridge/` - Python scripts for YOLOv8 detection and the Socket client.
* `web_ui/` - HTML,CSS and JS files for the live monitoring dashboard.
* `firmware/` - Arduino code for the ESP32-S3 microcontroller.

## 🎓 Academic Context
This project was developed during my first year at the **University of Burgos**. My goal was to move beyond classroom theory and explore how different technologies can be integrated to solve real-world sustainability problems.

**Special thanks to my professor,Jaime Andres Rincon Arango**, for providing the inspiration to integrate the web dashboard, the vision system, and the physical hardware component. His guidance was essential in turning this code into a complete functional system.

---

## 🚀 How to Run
1.  **Hardware:** Upload the `.ino` code to your ESP32-S3 and note the IP address.
2.  **Intelligence:** Compile and run the C++ `main.cpp` to start the brain server.
3.  **Vision:** Update the ESP32 IP in `vision.py` and run the script to start the camera and web server.
4.  **Dashboard:** Access `http://localhost:5000` in your browser.

---
*“I believe that the best way to learn is by building.”*
