#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>


#include "FS.h"   // Include the SPIFFS library
#include "libs/WebServerHandlers.h"
#include "config.h"
#include "libs/OTA.h"

bool gpio1_state=false;
bool gpio0_state=false;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
const int WAIT_TIME = WAIT_TIME_CT; //Time (in seconds) to wait wifi before enable AP mode
String USERNAME = USERNAME_CT;//Default username
String PASSWORD = PASSWORD_CT;//Default password
bool logged = LOGGED; //Init without logged session
char* WEB_OTA_NAME = WEB_OTA_NAME_CT;//Default changes with username to login
char* WEB_OTA_PASS = WEB_OTA_PASS_CT; //Default changes with password to login

void initMDNS();

void setup() {
  Serial.begin(SERIAL_SPEED);         
  Serial.println('\n');
  delay(1000);
  Serial.print("\nESP8266 OUT CONTROL - MARTIN DARDIS 2018\n");
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(GPIO0,OUTPUT);
  pinMode(GPIO1,OUTPUT);
  if( digitalRead(GPIO0) == HIGH)
    gpio0_state = true;
  if( digitalRead(GPIO1) == HIGH)
    gpio0_state = true;
  startFS();
  read_login_config();
  startWiFi();
  startOTA();
  initMDNS();
  startServer();
  Serial.print("\n READY. SERVER REQUESTs \n");
}

void loop(void) {
  ArduinoOTA.handle();
  server.handleClient();
  if(logged){
    String gp0=server.arg("gpio0");
    if (gp0 == "ON" && !gpio0_state){
      Serial.println("GPIO 0 = ON" );
      digitalWrite(GPIO0, HIGH);
      gpio0_state = true;
    }
    else if (gp0 == "OFF" && gpio0_state){
      Serial.println("GPIO 0 = OFF");
      digitalWrite(GPIO0, LOW);
      gpio0_state = false;
    }
    String gp1=server.arg("gpio1");
    if (gp1 == "ON" && !gpio1_state){
      Serial.println("GPIO 1 = ON" );
      digitalWrite(GPIO1, HIGH);
      gpio1_state = true;
    }
    else if (gp1 == "OFF" && gpio1_state){
      Serial.println("GPIO 1 = OFF");
      digitalWrite(GPIO1, LOW);
      gpio1_state = false;
    }
  }
}

void initMDNS(){
  if (!MDNS.begin(mDNS)) {             
     Serial.println("Error iniciando mDNS");
     return;
  }
  Serial.print("mDNS iniciado: ");
  Serial.print(mDNS);
  Serial.print(".local\n");
}

