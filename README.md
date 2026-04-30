# Electronics-Projects
📡 IoT-Based RFID Security Patrol Monitoring System

📌 Overview

This project is an IoT-based Security Patrol Monitoring System designed to ensure that security guards properly complete their patrol rounds in multi-floor buildings such as colleges, offices, and campuses.

The system uses RFID technology and ESP32 to track guard movement and logs data in real-time to the cloud (Google Sheets) using Wi-Fi.

🎯 Problem Statement

In many institutions, security guards record their patrols manually in logbooks. This method:

Is unreliable
Can be manipulated
Does not provide real-time monitoring

This can lead to security risks such as theft or unauthorized access.

💡 Proposed Solution

I developed a system where:

RFID checkpoints are installed at different locations
The guard carries an RFID card
Each scan logs data automatically

This ensures transparent, automated, and real-time monitoring.

⚙️ System Architecture
Guard taps RFID card
RFID reader reads UID
ESP32 verifies the card
Time fetched using NTP
Data sent via Wi-Fi
Stored in Google Sheets

🧩 Components Used
ESP32 Microcontroller
RFID Module (MFRC522)
RFID Cards
Power Supply
LEDs

🌐 Features
✅ Real-time monitoring
✅ Cloud data logging
✅ Low cost
✅ Easy installation
✅ Scalable system

📊 Applications
Colleges & Campuses
Offices
Industries
Hospitals
Residential Areas

📈 Advantages
Reduces manual errors
Improves security
Easy to use
Reliable system

⚠️ Limitations
Requires Wi-Fi
No offline storage
🔮 Future Scope
Mobile app
Alert system
Offline storage
Face recognition

💰 Cost
₹600–₹800 per device
₹4000–₹5000 full system

🛠️ Setup
Upload code to ESP32
Set Wi-Fi credentials
Configure Google Script
Power ON
Tap RFID

👨‍💻 Contributors
Swastik Shewatkar
⭐ Give a Star if you like this project!
