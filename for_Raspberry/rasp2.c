
//    Complie: gcc rasp2.c $(mariadb_config --libs) -o rasp2 $(mariadb_config --cflags) -lpaho-mqtt3c -lwiringPi -lm

// NOTE: thay doi ten file tuy y theo cach ban dat ten.

/*
    + Gui du lieu tu Rasp len MQTT
    + Connect to Max7219:
        - VCC   3.3V        17 phys
        - GND   0v          20
        - Din   MOSI        19
        - CS    CE0,CE1     24,26, using Pin 24
        - SCLK              23
    + Connect to MPU6050:
        - VCC   3.3V        1 Physical
        - GND   0v          6
        - SDA               3
        - SCL               5
        - INT               7
*/


/* Include library -----------------------------------------------------------------------------------*/
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"
#include <mysql.h>
#include <math.h>


/* END Include library -----------------------------------------------------------------------------------*/

/* Define -----------------------------------------------------------------------------------*/
#define ADDRESS     "tcp://broker.hivemq.com:1883"
#define CLIENTID    "subscriber_demo"
#define SUB_TOPIC   "mec20/CT6"    // recieve data
#define PUB_TOPIC  "mec21/CT6"  //send data
// #define QOS         1
#define INT_pin  7    //MPU INT

#define ACC_X 		  59 //3B
#define ACC_Y 		  61 //3D
#define ACC_Z		  63 //3F

/* End Define -----------------------------------------------------------------------------------*/



/* Init Function for MPU6050 -----------------------------------------------------------------------------------*/
int mpu;

void initMpu(void){
	wiringPiI2CWriteReg8(mpu, 0x19, 1);		//sample rate 25 register
	wiringPiI2CWriteReg8(mpu, 0x1A, 0x02);	//DLPF 0x02    26 register
	wiringPiI2CWriteReg8(mpu, 0x1B, 0x08);	//Gyro
	wiringPiI2CWriteReg8(mpu, 0x1C, 0x10);	//ACC
	wiringPiI2CWriteReg8(mpu, 0x38, 1);		//Interrupt
	wiringPiI2CWriteReg8(mpu, 0x6B, 1);		//CLK source

}
/* Read tempurature Function for MPU6050 -----------------------------------------------------------------------------------*/
uint16_t temp;
int16_t ReadTemp(uint8_t address)
{
    int16_t value,high, low;
    high = (wiringPiI2CReadReg8(mpu, address)<<8);
    low = wiringPiI2CReadReg8(mpu, address+1);
    value = high | low;
    return value;

}
/* DataReady Function for MPU6050 -----------------------------------------------------------------------------------*/
void dataReady(void){
	// clear interrupt flag
	wiringPiI2CReadReg8(mpu, 0x3A);
    // read sensor data
    //  float temp;
    //temp = ReadTemp(0x41)/340.0 + 36.53;
    ReadTemp(0x41);
}
/* Readsensor_ Function for MPU6050  AX,AY,AZ -----------------------------------------------------------------------------------*/
int16_t read_sensor(unsigned char sensor){
	int16_t high, low, data;
	high = wiringPiI2CReadReg8(mpu, sensor); 	 
	low = wiringPiI2CReadReg8(mpu, sensor + 1);			
	data = (high<<8) | low; 																							
	return data;
}


/* Send data   -----------------------------------------------------------------------------------*/
void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
    printf("Message '%s' with delivery token %d delivered\n", payload, token);
} 

/* Recieve Data -----------------------------------------------------------------------------------*/
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    // mysql connect, lan 1, de gui data xuong mysql
    MYSQL *conn;
    char *server = "localhost";
    char *user = "admin";
    char *password = "123456"; // set me first 
    char *database = "mqtt";
    char* payload = message->payload;
    printf("Received message: %s\n", payload);

    // Connect to database
    
    conn = mysql_init(NULL);
    if (mysql_real_connect(conn, server, user, password, database, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }  
    char sql[200];
   // send data xuong mysql
    sprintf(sql,"update sensor3 set data=%s",payload);
    mysql_query(conn,sql);
    mysql_close(conn);  /// close mysql
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;

}

/* Main Function -----------------------------------------------------------------------------------*/
int main(int argc, char* argv[]) 
{   
    //set MYSQL lan 2 de gui data len mqtt
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server = "localhost";
    char *user = "admin";
    char *password = "123456"; // set me second
    char *database = "mqtt";

    /* MQTT Client Set -----------------------------------------------------------------------------------*/
    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    //conn_opts.username = "your_username>>";
    //conn_opts.password = "password";
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) 
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    //listen for operation
    MQTTClient_subscribe(client, SUB_TOPIC, 0);
    /* END MQTT Client Set -----------------------------------------------------------------------------------*/

    /* MPU 6050 Set -----------------------------------------------------------------------------------*/
	// setup i2c interface
	mpu = wiringPiI2CSetup(0x68);
    wiringPiSetup();

    // ktra ket noi
	if(wiringPiI2CReadReg8(mpu, 0x75)!= 0x68)
    {
		printf("Connection fail. \n");
		exit(1);
	}

    // setup operational mode for mpu6050
	initMpu();
	// setup interrupt for INT pin
	pinMode(INT_pin, INPUT);
	wiringPiISR(INT_pin, INT_EDGE_RISING, &dataReady);
    /// tao variable nhiet do de luu gia tri
    float temp;
    temp = ReadTemp(0x41)/340.0 + 36.53;

    /* END MPU 6050 Set -----------------------------------------------------------------------------------*/

    int num;
    // ket noi database
    conn = mysql_init(NULL);
    mysql_real_connect(conn,server,user,password,database,0,NULL,0); 
    /* While Function  -----------------------------------------------------------------------------------*/
    while(1) {
        //send x,y,z accel measurement
       	float Ax = (float)read_sensor(ACC_X)/4096.0;
		float Ay = (float)read_sensor(ACC_Y)/4096.0;
		float Az = (float)read_sensor(ACC_Z)/4096.0;
		
		//float pitch = atan2(Ax,sqrt(pow(Ay,2)+pow(Az,2)))*180/pi;
		//float roll = atan2(Ay,sqrt(pow(Ax,2)+pow(Az,2)))*180/pi;
		//float yaw = atan2(Az,sqrt(pow(Ax,2)+pow(Ay,2)))*180/pi;

		//printf("AX:%.2f,AY:%.2f,AZ: %.2f\n",Ax,Ay,Az);
        char msg [200];
        sprintf(msg,"mpu2, temp: %.1f,Ax: %.2f,Ay: %.2f,Az: %.2f",temp,Ax,Ay,Az);
        //send temperature measurement
        publish(client, PUB_TOPIC, msg);

        /* MYSQL Function  -----------------------------------------------------------------------------------*/

        // kiem tra cot isUpdated
        char sql[200];
        sprintf(sql, "select * from sensor3"); /// lay data tu table
        //sprintf(sql,"select * from sensor2 where id>(select max(id) from sensor2)-1");

        mysql_query(conn,sql);
        res = mysql_store_result(conn); 
        row = mysql_fetch_row(res); //row[0]-> red; row[1]->green
        // NOTES: row la bien dang chuoi ky tu

        //"num" la sample time nhan duoc tu web.
        num = atoi(row[1]);
         /// sample time
        //delay(num*1000); // hoac co the su dung sleep(num)
        //printf("num: %d\n",num);
        sleep(num);

        
        /* MYSQL Function  -----------------------------------------------------------------------------------*/

         
    }
    mysql_close(conn);
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rc;
}
