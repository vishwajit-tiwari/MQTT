/**
 * @file Sniric_Pro_GoogleAsistance_Alexa_Smart_Home.ino
 * @author Vishwajit Kumar Tiwari (vishwajit16897@gmail.com)
 * @brief Smart Home using Web Schokets
 * @version 0.1
 * @date 2022-08-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

 /**********************************************************************************
 *  Download the libraries 
 *  SinricPro 2.9.14 Library:     https://github.com/sinricpro/esp8266-esp32-sdk
 *  ArduinoJson 6.19.1 Library:   https://github.com/bblanchon/ArduinoJson
 *  WebSockets 2.3.5 Library:     https://github.com/Links2004/arduinoWebSockets
 *  IRremote 3.5.2 Library:       https://github.com/Arduino-IRremote/Arduino-IRremote
 *  Arduino-timer 2.3.1 Library:  https://github.com/contrem/arduino-timer
 **********************************************************************************/

#include <Arduino.h>
//#include <ESP8266WiFi.h>
//#include <Servo.h>
#include "WiFi.h"
#include "SinricPro.h"
#include "SinricProSwitch.h"


// Uncomment the following line to enable serial debug output
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 


/****************************GPIO Pins*******************************/
#define ROOMLIGHT 23
#define NIGHTLAMP 22
#define KITCHENLIGHT 21
/********************************************************************/


/*************************************WiFi, APP_KEY & Switch_ID*****************************************/
#define WIFI_SSID         "SSID"    
#define WIFI_PASS         "Password"
#define APP_KEY           "70b45622-f437-43b0-8f78-63e8657bcbd0"
#define APP_SECRET        "4d252303-bfba-44c0-86ce-1d7d5562bd00-2c8c6345-ee59-465c-9696-20a1d0f1dbe2"

#define SWITCH_ID_1       "6303c019da0cc63243735b30"  /*Room Light*/  
#define SWITCH_ID_2       "6303c0e4db270037341d7efa"  /*Night Lamp*/ 
#define SWITCH_ID_3       "6303c0beda0cc63243735c71"  /*Kitchen Light*/  
#define SWITCH_ID_4       "YOUR-DEVICE-ID"    
/******************************************************************************************************/


#define BAUD_RATE   115200                // Change baudrate to your need
#define WIFI_LED       2    // WiFi LED

bool connectedFlag = false;
String pinStatus = "0000";
String pinStatus_OLD = "0000";


/*******************************************Different Switch Control***********************************************/
bool onPowerState1(const String &deviceId, bool &state) {
  Serial.printf("Device 1 turned %s\r\n", deviceId.c_str(), state?"on":"off");
  Serial.printf("%s", state?"A1":"A0");  
  if((state?"on":"off") == "on") {
    digitalWrite(ROOMLIGHT,HIGH);
  }
  else {
    digitalWrite(ROOMLIGHT,LOW);
  }
  return true; // request handled properly
}

bool onPowerState2(const String &deviceId, bool &state) {
  Serial.printf("Device 2 turned %s\r\n", deviceId.c_str(), state?"on":"off");
  Serial.printf("%s", state?"B1":"B0");
  if((state?"on":"off") == "on") {
    digitalWrite(NIGHTLAMP,HIGH);
  }
  else {
    digitalWrite(NIGHTLAMP,LOW);
  }
  return true; // request handled properly
}

bool onPowerState3(const String &deviceId, bool &state) {
  Serial.printf("Device 3 turned %s\r\n", deviceId.c_str(), state?"on":"off");
  Serial.printf("%s", state?"C1":"C0");
  if((state?"on":"off") == "on") {
    digitalWrite(KITCHENLIGHT,HIGH);
  }
  else {
    digitalWrite(KITCHENLIGHT,LOW);
  }
  return true; // request handled properly
}

bool onPowerState4(const String &deviceId, bool &state) {
  Serial.printf("Device 4 turned %s\r\n", deviceId.c_str(), state?"on":"off");
  Serial.printf("%s", state?"D1":"D0");
  return true; // request handled properly
}
/*******************************************************************************************************/


/*******************************setup function for WiFi connection**************************************/
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }

  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}
/********************************************************************************************************/



/*********************************setup function for SinricPro********************************************/
void setupSinricPro() {
  // add devices and callbacks to SinricPro
  SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  mySwitch1.onPowerState(onPowerState1);

  SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];
  mySwitch2.onPowerState(onPowerState2);

  SinricProSwitch& mySwitch3 = SinricPro[SWITCH_ID_3];
  mySwitch3.onPowerState(onPowerState3);

  SinricProSwitch& mySwitch4 = SinricPro[SWITCH_ID_4];
  mySwitch4.onPowerState(onPowerState4);

  // setup SinricPro
  SinricPro.onConnected([](){ 
    Serial.printf("SinricPro Connected\r\n");
    Serial.printf("W1");
    digitalWrite(WIFI_LED, LOW);
    }); 
  SinricPro.onDisconnected([](){ 
    Serial.printf("SinricPro Disconnected\r\n");
    Serial.printf("W0");
    digitalWrite(WIFI_LED, HIGH);
    });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void updateState(String state, String pre_state)
{
  SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];
  SinricProSwitch& mySwitch3 = SinricPro[SWITCH_ID_3];
  SinricProSwitch& mySwitch4 = SinricPro[SWITCH_ID_4];

  if(state.substring(0,1) != pre_state.substring(0,1)){
    if (state.substring(0,1) == "1"){
      mySwitch1.sendPowerStateEvent(HIGH);
    }
    else{
      mySwitch1.sendPowerStateEvent(LOW);
    }
  }
  if(state.substring(1,2) != pre_state.substring(1,2)){
    if (state.substring(1,2) == "1"){
      mySwitch2.sendPowerStateEvent(HIGH);
    }
    else{
      mySwitch2.sendPowerStateEvent(LOW);
    }
  }
  if(state.substring(2,3) != pre_state.substring(2,3)){
    if (state.substring(2,3) == "1"){
      mySwitch3.sendPowerStateEvent(HIGH);
    }
    else{
      mySwitch3.sendPowerStateEvent(LOW);
    }
  }
  if(state.substring(3,4) != pre_state.substring(3,4)){
    if (state.substring(3,4) == "1"){
      mySwitch4.sendPowerStateEvent(HIGH);
    }
    else{
      mySwitch4.sendPowerStateEvent(LOW);
    }
  }
}
/**********************************************************************************************/


void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  pinMode(WIFI_LED, OUTPUT);
  /*Setup the LED Pin*/
  pinMode(ROOMLIGHT,OUTPUT);
  pinMode(NIGHTLAMP,OUTPUT);
  pinMode(KITCHENLIGHT,OUTPUT);
  setupWiFi();
  setupSinricPro();
}

void loop() {
  while(Serial.available()) 
  {
    pinStatus = Serial.readString();
    if(pinStatus != pinStatus_OLD) {      
      updateState(pinStatus, pinStatus_OLD);
      pinStatus_OLD = pinStatus;
    }
    //connectedFlag = true;
  }
  SinricPro.handle();
}
