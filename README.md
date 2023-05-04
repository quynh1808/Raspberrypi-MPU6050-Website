# Raspberrypi-MPU6050-Website
Design a website that receives and transmits signals with RaspberyPi from MPU6050 by MQTT 
# Project Overview
Note: This is just a simple sample code file, you can refer to it and customize it for your own purposes.
The software for this project consists of 3 folder:
* HTML_forPC: This is a folder containing html files and php files to access and display data on the website from MYSQL.
* for_Raspberry: This is the folder containing the rasp2.c file and the mysql note file. In rasp.2 file I write to read temperature and acceleration values from MPU6050 with delay t seconds. The t value is changed by the Website from the PC.
* Images: Some illustrations
# Describe:: 
* MPU6050 (I2C Digital-Output) is a three-axis accelerometer and three-axis gyroscope Micro Electro-mechanical system (MEMS). It aids in the measurement of velocity, orientation, acceleration, displacement, and other motion-related features.

With this project, the PC acts as a server with the website displaying the temperature values in the form of a graph and the pitch, roll, yaw acceleration values.

RaspberryPi acts as Slave, it reads temperature and accelerometer signals and then sends the value back to PC via MQTT communication protocol.

On the website, we can customize the sampling time and send it to rasp for change.

More Details: https://drive.google.com/file/d/1RsuSEeQWmR5zi1XpNKSqcQ__YWDW6Q3j/view?usp=sharing
# Pins Configuration

    + Connect to MPU6050:
        - VCC   3.3V        1 Physical
        - GND   0v          6
        - SDA               3
        - SCL               5
        - INT               7
# Install/Compile/Run
**Note: Only use for linux operating system.
1. Install Apache2: ```sudo apt-get install apache2 -y ```
2. Install Mysql: ```sudo apt-get install mariadb-server mariadb-client ```
3. Install C library for MySQL: ```sudo apt-get install libmariadbc-dev ```
4. Install PHP MYSQL: ```sudo apt-get install php php-mysql```
5. Dowload MQTT: [ECLIPSE](https://www.eclipse.org/paho/index.php?page=clients/c/index.php)
6. Install WiringPi lib(Only RaspberryPi): ```sudo apt-get install wiringpi ``` 

**Compile:
+ PC: ``` gcc sub3.c $(mariadb_config --libs) -o sub3 $(mariadb_config --cflags) -lpaho-mqtt3c -lm ```
+ Raspberry: ``` gcc rasp2.c $(mariadb_config --libs) -o rasp2 $(mariadb_config --cflags) -lpaho-mqtt3c -lwiringPi -lm ```
# Feedback
If you have any contribution to improve the project, please contact me by email: quynh.nm1808@gmail.com

I will acknowledge and edit to make the project more complete.

Thanks!
