# Raspberrypi-MPU6050-Website
Design a website that receives and transmits signals with RaspberyPi from MPU6050 by MQTT 
# Project Overview
Note: This is just a simple sample code file, you can refer to it and customize it for your own purposes.
The software for this project consists of 3 folder:
* HTML_forPC: This is a folder containing html files and php files to access and display data on the website from MYSQL.
* for_Raspberry: This is the folder containing the rasp2.c file and the mysql note file. In rasp.2 file I write to read temperature and acceleration values from MPU6050 with delay t seconds. The t value is changed by the Website from the PC.
* Images: Some illustrations
# Descirbe: 
With this project, the PC acts as a server with the website displaying the temperature values in the form of a graph and the pitch, roll, yaw acceleration values.

RaspberryPi acts as Slave, it reads temperature and accelerometer signals and then sends the value back to PC via MQTT communication protocol.

On the website, we can customize the sampling time and send it to rasp for change.
# Pins Configuration
    
    + Connect to MPU6050:
        - VCC   3.3V        1 Physical
        - GND   0v          6
        - SDA               3
        - SCL               5
        - INT               7
# Install/Compile/Run
1. Install Apache2: 
