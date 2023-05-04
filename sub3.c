//compile:   gcc sub3.c $(mariadb_config --libs) -o sub3 $(mariadb_config --cflags) -lpaho-mqtt3c -lm
// NOTE: thay doi ten file tuy theo cach ban dat ten

/* Include Function  -----------------------------------------------------------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"
#include <mysql.h>
#include <math.h>
#include <stdbool.h>
/* End Include Function  -----------------------------------------------------------------------------------*/

/* Define  -----------------------------------------------------------------------------------*/
#define ADDRESS     "tcp://broker.hivemq.com:1883"
#define CLIENTID    "subscribe_demo"
#define SUB_TOPIC   "mec21/CT6"
#define PUB_TOPIC   "mec20/CT6"
#define pi 3.14159	
// #define QOS         1

/* END Define  -----------------------------------------------------------------------------------*/

/* SEND DATA FUNCTION FOR RASP  -----------------------------------------------------------------------------------*/
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

/* Recieve DATA FUNCTION  -----------------------------------------------------------------------------------*/
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {

    char *token = strtok(message->payload, " ,:");  // nhan data va tach data theo dau phay
    //char *payload = message->payload; 
    // float temp = atof(token);  //atof chuyen kieu char --> float
    char name[50] ="";
    strcpy(name, token);
    token = strtok(NULL, ":");
    token = strtok(NULL, ":");
    float temp = atof(token);
    token = strtok(NULL, ":"); //phat hien dau phay
    float Ax = atof(token);
    token = strtok(NULL, ":");
    float Ay = atof(token);
    token = strtok(NULL, ":");
    float Az = atof(token);

    ///tinh toan pitch, toll, yaw tu Ax,Ay,Az nhan duoc
    float pitch = atan2(Ax,sqrt(pow(Ay,2)+pow(Az,2)))*180/pi;
	float roll = atan2(Ay,sqrt(pow(Ax,2)+pow(Az,2)))*180/pi;
	float yaw = atan2(Az,sqrt(pow(Ax,2)+pow(Ay,2)))*180/pi;
    ///in ra man hinh gia tri nhan duoc gom: temp,Ax,Ay,Az.
    //printf("Received message: %s\n", payload);
    //////
    printf("Gia tri nhan tu %s la: temp: %.2f,Ax: %.2f,Ay: %.2f, Az: %.2f\n",name,temp,Ax,Ay,Az);
    printf("Gia tri tinh toan: pitch: %.2f,roll: %.2f,yaw: %.2f\n",pitch,roll,yaw);
    // Connect to database
    MYSQL *conn;
    char *server = "localhost";
    char *user = "admin";
    char *password = "123456"; //set me first . tao lan 1
    char *database = "IoT_Exam";

    conn = mysql_init(NULL);
        // ktra mysql
    if (mysql_real_connect(conn, server, user, password, database, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }  
    ///tao 3 bien char de gui data xuong mysql
    char sql[200];
    char sql1[200];
    char sql2[200];
    sprintf(sql,"insert into temptable(temp,device) values(%.2f,'%s')",temp,name);
    sprintf(sql1,"insert into xyz(x,y,z,device) values (%.2f,%.2f,%.2f,'%s')",Ax,Ay,Az,name);
    sprintf(sql2,"insert into angle(pitch, roll,yaw,device) values (%.2f,%.2f,%.2f,'%s')",pitch,roll,yaw,name);
    /// gui data
    mysql_query(conn,sql);
    mysql_query(conn,sql1);
    mysql_query(conn,sql2);
    mysql_close(conn); //close mysql
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

/* Main FUNCTION  -----------------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server = "localhost"; //set me second
    char *user = "admin";
    char *password = "123456";
    char *database = "IoT_Exam";

    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    //conn_opts.username = "your_username>>";
    //conn_opts.password = "password";

    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
   
    //listen for operation
    MQTTClient_subscribe(client, SUB_TOPIC, 0);
    // ket noi database

    conn = mysql_init(NULL);
    mysql_real_connect(conn,server,user,password,database,0,NULL,0); 
    int num, num2;
    while(1) {

        /* MYSQL Function  -----------------------------------------------------------------------------------*/
        char sql[200];
        sprintf(sql, "SELECT * from tempsample"); //lay data tu table
        mysql_query(conn,sql);
        res = mysql_store_result(conn); 
        //row[0]-> red; row[1]->green
        while(row = mysql_fetch_row(res)){ 
        // NOTES: row la bien dang chuoi ky tu


        num2 = num;
        num = atoi(row[1]);

        //printf("num: %d\n",num);
        char msg [20];
        
        sprintf(msg,"%d",num);
        //tao dieu kien de chi gui thoi gian lay mau 1 lan duy nhat, neu co thay doi thi moi gui
        if(num2 !=num)
            publish(client, PUB_TOPIC, msg);
        
        }
        mysql_free_result(res); //// 
    } 
    mysql_close(conn); 
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rc;
}