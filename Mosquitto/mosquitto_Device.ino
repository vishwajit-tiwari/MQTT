/**
 * @file mosquitto_Device.ino
 * @author Vishwajit Kuamr Tiwari (tvishwajit@cdac.in)
 * @brief MQTT (Mosquitto Broker) to control & monitor Appliences  
 * @version 0.1
 * @date 2022-07-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ThingSpeak.h"
#include "WiFi.h"
#include <PubSubClient.h>
#include <Ethernet.h>

/*******************DEFINES************************/
#define DEBUG
#define LED_Pin 23
#define LM35_Temp 39   /*Analog pin*/
/**************************************************/

/*********** Enter you Wi-Fi Details **************/
char ssid[] = "your Hotspot name"; //SSID
char pass[] = "Password"; // Password
/**************************************************/

/*******************************Constants***********************************/

/*The input pin the LDR is connected to. Must be an analogue pin
  (i.e. pins A0-A5 on Arduino UNO).*/
const byte ldrPin = 2;//A0;

/*This digital pin will be driven low to release a lock when puzzle solved
can be any pin capable of digital output*/ 
const byte lockPin = 22;//A1; //D2;

/*The base reading is the resistance of the LDR measure
in normal lighting of the room, and will be calibrated
each time the puzzle is reset*/
int baseReading = 512;

/*Current reading is the resistance of the LDR, which
changese when a light is falling on it (resistance falls)
or light block from it (resistance rises)*/
int currentReading;

/*The LDR reading will naturally fluctuate slightly. This
sensitivity value will be used as threshold to determine
when it has changed enough for the puzzle to be considered
to have been solved.*/
const int sensitivity = 30;

/*Track state of overall puzzle*/
enum PuzzleState {Initialising, Running, Solved};
PuzzleState puzzleState = Initialising;

/*Returns true if the puzzle has been solved,
false otherwise*/
bool checkIfPuzzleSolved() {
  return (currentReading > baseReading + sensitivity);
}

/*Ultrasonic Trig and Echo Pins*/
const int trigger = 5;//16;
const int echo = 18;//5;
long T;
float distanceCM;
/***************************************************************************/

/**********************ThingsPeak Details***********************************/
unsigned long myChannelField =  1794204; // Channel ID
const int ChannelField1 = 1; // Which channel to write data
const char * myWriteAPIKey = "TE5ARSKRZA361F1W"; // Your write API Key
/***************************************************************************/


/***************************MQTT Constants**********************************/
/*MAC Address of the Brocker*/
/*If not known, make up a unique 6-hexadecimal value.*/
const byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED};
/* Unique ip address to assign to this decive (if DHCP fails)*/
const IPAddress deviceIP(192,168,1,100);
/*IP Address of machine on the network running the MQTT broker*/
const IPAddress mqttServerIP(192,168,7,5);
/*Unique name of this device, used as client ID to connect to MQTT server*/
/*and also topic name for messages published to this device */
const char* deviceID = "Arduino";
/***************************************************************************/


/*********************MQTT Global Variables********************************/
/*Create an instance of Ethernet client*/
//EthernetClient ethernetClient;
/*Create an instance of MQTT Client based on the ethernet client*/
//PubSubClient MQTTclient(ethernetClient);

/*Create an instance of WiFi client*/
WiFiClient  client;
/*Create an instance of MQTT Client based on Wifi client*/
PubSubClient MQTTclient(client);

/*The time (from millis()) at which last message was published*/
long lastMsgTime = 0;
/*A buffer to hold message to be sent/have been received*/
char msg[64];
/*The topic in which to publish a message*/
char topic[32];
/*Counter for number of heartbeat pulses sent*/
int pulseCount = 0;
/*************************************************************************/

/****************************WiFi Setup***********************************/
void wifiSetup(void)
{
    /*Start the serial connection*/
    if(!Serial) 
    {
      Serial.begin(115200);
    }

    /*Attempting to connect to the specified  WiFi network*/
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        
        /*Show a simple progress counter while we wait to connect*/
        while (WiFi.status() != WL_CONNECTED)
        {
            WiFi.begin(ssid, pass);
            Serial.print(".");
            delay(5000);
        }
        /*Give the WiFi a couple of seconds to initialize*/
        delay(2000);

        /*Print debug info about the connection*/
        Serial.print("Connected!\nIP address: ");
        Serial.println(WiFi.localIP());
    }
}
/*************************************************************************/


/****************************Ethernet Setup*******************************/
void ethernetSetup() 
{
//  if(!Serial) 
//  {
//    /*Start the serial connection*/
//    Serial.begin(115200);
//  }
//  /*Attempting to connect to the specified network*/
//  Serial.print("Connecting to network ");
//  /*First, try to create a conncetion using DHCP */
//  if(Ethernet.begin(mac) == 0) 
//  {
//    /*Try to connect using speified IP address instead of DHCP*/
//    Ethernet.begin(mac,deviceIP);
//  }
//  /*Give the Ethernet sheild a couple of seconds to initialize*/
//  delay(2000);
//  /*Print debug info about the conncetion*/
//  Serial.print("Connceted! IP address: ");
//  Serial.println(Ethernet.localIP());  
}
/************************************************************************/

/*************************MQTT setup function***************************/
void mqttSetup()
{
  /*Define some setting for the MQTT client*/
  MQTTclient.setServer(mqttServerIP, 1883);
  MQTTclient.setCallback(mqttCallback);
}
/***********************************************************************/


/***********************Void Setup function*****************************/
void setup()
{
  #ifdef DEBUG
  /*Initialise serial communication channel with the PC*/
  Serial.begin(115200);
  #endif
  /*Set the LDR pin as input*/
  pinMode(ldrPin,INPUT);

  /*Setup the Ultrasonic Pins*/
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);

  /*Setup the LED Pin*/
  pinMode(LED_Pin,OUTPUT);

  /*Set the lock pin as output and secure the lock*/
  pinMode(lockPin,OUTPUT);
  digitalWrite(lockPin,HIGH);

  /*Set the Temperature as input*/
  pinMode(LM35_Temp,INPUT);

  /*Setup the Ethernet network connection*/
  ethernetSetup();

  /*Setup the WiFi connection*/
  WiFi.mode(WIFI_STA);
  wifiSetup();
  
  /*Setup the MQTT Services*/
  mqttSetup();

  /*Setup the Thingspeak Services*/
  ThingSpeak.begin(client);
}
/***********************************************************************/


/************************Void Loop Function*****************************/
void loop()
{
  wifiSetup();
  /*Take the current measurement from the sensor
  when the sensor is covered, this value rises. 
  when it receives more light, it falls.*/
//  currentReading = analogRead(ldrPin);
  /*Log the output on the serial terminal if debugging enabled*/
//  #ifdef DEBUG
//  Serial.print("LDR Sensor Data = ");
//  Serial.println(currentReading);
//  delay(500);
//  #endif

  currentReading = analogRead(LM35_Temp);
  /*Log the output on the serial terminal if debugging enabled*/
  #ifdef DEBUG
  Serial.print("Temp = ");
  Serial.println(currentReading);
  delay(1000);
  #endif

  publish_dashboard(currentReading);

  /*Switch action based on the current state of the puzzle*/
  switch(puzzleState) {

    case Initialising: 
        calibrate();
        puzzleState = Running;
        break;

    case Running:
        if(checkIfPuzzleSolved()) {
          onSolve();
        }
        break;

    case Solved:
        if(checkIfPuzzleSolved()) {
          onUnsolve();
        }
        break;
    default:
        Serial.println("Invalid Option");
        break;
  }

  /*Call the MQTT loop function*/
  mqttLoop();
  delay(100);
  
  //ultrasonic();
}

/***********************************************************************/


/***************************Calibrate function**************************/
void calibrate() 
{
  /*Calculate a mean average reading over 10 samples*/
  int numSamples = 10;
  int total = 0;
  for(int i =0; i<numSamples; i++){
    delay(100);
    total += analogRead(/*ldrPin*/LM35_Temp);
  }
  baseReading = total / numSamples;

  /*Print debug information to the serial connection*/
  #ifdef DEBUG
  Serial.print("Sensor Initialised at base lavel: ");
  Serial.println(baseReading);
  #endif
}
/***********************************************************************/


/**************************Ultrasonic function**************************/
void ultrasonic(void)
{
  /*Utrasonic functionality*/
  digitalWrite(trigger, LOW);
  delay(1);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  T = pulseIn(echo, HIGH);
  distanceCM = T * 0.034;
  distanceCM = distanceCM / 2;

  if(distanceCM <= 50) 
  {
    Serial.println("LED is ON");
    digitalWrite(LED_Pin,HIGH);    
  }
  else
  {
    digitalWrite(LED_Pin,LOW);  
  }
  
  Serial.print("Distance in cm: ");
  Serial.println(distanceCM);
  ThingSpeak.writeField(myChannelField, ChannelField1, distanceCM, myWriteAPIKey);
  delay(1000);
}
/***********************************************************************/


/**************************MQTT Loop function***************************/
void mqttLoop()
{
  /*Ensure there's a connection to the MQTT server*/
  while(!MQTTclient.connected()) 
  {
    /*Debug info*/
    Serial.print("Attempting to connect to the MQTT broker at ");
    Serial.println(mqttServerIP);

      /*Attempt to connect*/
      if(MQTTclient.connect(deviceID)) 
      {
        /*Debug info*/
        Serial.println("Connected to MQTT broker");

        /*Once connceted, publish an announcement to the ToHost/#deviceID# topic */
        snprintf(topic, 32, "ToHost/%s",deviceID);
        snprintf(msg, 64, "CONNECT", deviceID);
        MQTTclient.publish(topic, msg);

        /*Once connceted, publish an announcement to the ToHost/#deviceID# topic */
        snprintf(topic, 32, "fromPub/%s",deviceID);
        snprintf(msg, 64, "ToDashBoard", deviceID);
        MQTTclient.publish(topic, msg);

        /*Subscribe to topics meant for this device*/
        snprintf(topic, 32, "ToDevice/%s", deviceID);
        MQTTclient.subscribe(topic);

        /*Subscribe to topics meant for all devices*/
        MQTTclient.subscribe("ToDevice/All");
      }
      else
      {
        /*Debug info why conncetion could not be made*/
        Serial.print("Connection to MQTT server faild!, rc=");
        Serial.print(MQTTclient.state());
        Serial.println("trying again in 5 seconds");

        /*Wait 5 seconds before retrying*/
        delay(5000);
      }
  } 
  /*Call the main loop to check for and publish messages*/
  MQTTclient.loop(); 
}
/***********************************************************************/

/***************************MQTT Publish function***********************/
void publish(char* message)
{
  snprintf(topic, 32, "ToHost/%s", deviceID);
  MQTTclient.publish(topic,message);
}
/***********************************************************************/

/***************************MQTT Publish function***********************/
void publish_dashboard(int currentReading)
{
  char buff[64];
  sprintf(buff, "%d", currentReading);
  snprintf(topic, 32, "fromPub/%s", deviceID);
  MQTTclient.publish(topic,buff);
}
/***********************************************************************/


/**************************onUnsolved Function*****************************/
void onUnsolve() {
 #ifdef DEBUG
    Serial.println("Puzzle has just become unsolved!");
 #endif

  /*Lock the lock again*/
  digitalWrite(lockPin, HIGH);
  /*Updating the global puzzle state*/
  puzzleState = Running;
  /*NEW! Publish a message to MQTT borker*/
  publish("Cover up puzzle unsolved!");
}
/************************************************************************/

/**************************onSolved Function*****************************/
void onSolve() {
 #ifdef DEBUG
    Serial.println("Puzzle has just been solved!");
 #endif

  /*Release the lock*/
  digitalWrite(lockPin, LOW);
  /*Updating the global puzzle state*/
  puzzleState = Solved;
  /*NEW! Publish a message to MQTT borker*/
  publish("Cover up puzzle solved!");
}
/*************************************************************************/

/**************************onReset Function*****************************/
void onReset() {
 #ifdef DEBUG
    Serial.println("Puzzle has just been reset!");
 #endif

  /*Lock the lock again*/
  digitalWrite(lockPin, HIGH);
  /*Updating the global puzzle state*/
  puzzleState = Initialising;
  /*Publish a message to MQTT borker*/
  publish("Cover up puzzle reset!");
}
/************************************************************************/


/**************************MQTT Callback Function***************************/
/*Callback function each time a message is published in any of the
  topics to which this client is subscribed*/
void mqttCallback(char* topic, byte* payload, unsigned int length) 
{
      /*The message payload passed to this function is a byte* 
      Let's first copy its contents to the msg char[] array*/
      memcpy(msg, payload,length);
      /*Add a NULL terminator to the message to make it a correct string*/
      msg[length] = '\0';

      /*Debug*/
      Serial.print("Message received in topic [");
      Serial.print(topic);
      Serial.print("] ");
      Serial.print(msg);
      Serial.println(" ");

      /*Act upon the message received*/
      if(strcmp(msg, "SOLVE") == 0) {
        onSolve();
      }
      else if(strcmp(msg,"RESET") == 0) {
        onReset();
      }

      /*For Bulb Control*/
      if(strcmp(msg,"BulbOFF") == 0) {
        digitalWrite(LED_Pin,LOW);
      }
      if(strcmp(msg,"BulbON") == 0) {
        digitalWrite(LED_Pin,HIGH);
      }
}
/********************************************************************************/