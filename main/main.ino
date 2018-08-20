#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <FS.h>   // Include the SPIFFS library
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPUpdateServer.h>

#include "config.h"

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void startOTA();
void startWiFi();
void startFS();
void startServer();
void handleindex();
void handlewifi();
void handlecss();
void handlelogin();
void handle_nolog();
void handleerror();
void handlechange();
void handle_change_error();
bool write_wifi_config_file(String ssid,String pass);
bool write_login_config(String new_username, String new_pass);
void read_login_config();
bool change_user_pass();

void setup() {
  Serial.begin(115200);         
  Serial.println('\n');
  delay(1000);
  Serial.print("\nESP8266 OUT CONTROL - MARTIN DARDIS 2018\n");
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(GPIO0,OUTPUT);
  pinMode(GPIO1,OUTPUT);
  startFS();
  read_login_config();
  startWiFi();
  startOTA();
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

void startWiFi(){
  Serial.printf("\n********** Starting WiFi **********\n\n");
  if( SPIFFS.exists(WPA_PATH) ){
    WiFi.mode(WIFI_STA);
    char ssid[MAX_LONG_STR];
    char pass[MAX_LONG_STR];
    File wifi = SPIFFS.open(WPA_PATH,"r");
    Serial.printf("Reading wireless config file ... OK\n");
    while(wifi.position() < wifi.size()){
      String rssid = wifi.readStringUntil(';');
      rssid.toCharArray(ssid,MAX_LONG_STR);
      Serial.printf("SSID: %s",ssid);
      String rpass = wifi.readStringUntil(';');
      rpass.toCharArray(pass,MAX_LONG_STR);
      Serial.printf("\tPass: %s",pass);
      wifi.seek(1,SeekCur);
    }
    wifi.close();
    WiFi.begin(ssid,pass);
  }
  else{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID,AP_PASS);
    Serial.printf("Using default WiFi config \n");
  }
  Serial.print("\nStarting Wifi \nConnecting  ");
  int i = 0;
  for (int i=0 ;WiFi.status() != WL_CONNECTED && i< (WAIT_TIME*4); i++){ // Wait for the Wi-Fi to connect
    delay(250);
    Serial.printf("*");
    if(digitalRead(LED_BUILTIN)==LOW)
      digitalWrite(LED_BUILTIN,HIGH);
    else
      digitalWrite(LED_BUILTIN,LOW);
  }
  if ( WiFi.status() != WL_CONNECTED ){
    digitalWrite(LED_BUILTIN,LOW);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID,AP_PASS);
    Serial.printf("\nFAIL. Using default WiFi config \n");
  }
  else{
    digitalWrite(LED_BUILTIN,HIGH);
    Serial.println('\n');
    Serial.print("Connected to: ");
    Serial.print(WiFi.SSID());              // Tell us what network we're connected to
    Serial.print("\tIP Address: ");
    Serial.print(WiFi.localIP());
    Serial.print("\t MAC Address: ");
    Serial.print(WiFi.macAddress());
  }
}

void startOTA(){
   ArduinoOTA.setPort(8266);
   ArduinoOTA.setHostname(OTA_NAME);
   ArduinoOTA.setPassword(OTA_PASS);
   ArduinoOTA.setPasswordHash(OTA_MD5);
 ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
 ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
 ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
 ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
 ArduinoOTA.begin();
 Serial.println("\nOTA ENABLED\n");
}

void startFS(){
  SPIFFS.begin();
  Serial.print("\n");
  Serial.printf("********** Iniciando sistema de Archivos **********\n\n");
  Serial.printf("Listado de archivos\n");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.print("-> "+dir.fileName()+"\n");
  }
}

void startServer(){
  server.on("/status",[](){
    String message = "SSID: "+WiFi.SSID()+"\tIP: "+ String(WiFi.localIP())+"\tMAC: "+WiFi.macAddress()+"\tSTATUS:"+String(WiFi.status())+"\n";
    message += "GPIO0 ="+ String(gpio0_state)+" GPIO1="+String(gpio1_state)+"\n";
    message += "Logged = " + String(logged) +"\n";
    server.send(200,"text/plain",message);
    });
  server.on("/config",[](){//Delete this code 
      if(!logged){
        handlelogin();
        return;
      }
      File wifi = SPIFFS.open(WPA_PATH,"r");
      size_t sent = server.streamFile(wifi,"text/html");
      wifi.close();
    });
  server.on("/",[](){
    if (!logged)
      handlelogin();
    else
      handleindex();
  });
  server.on("/disconnect",[](){
    logged = false;
    handlelogin();
    });
  server.on("/auth",[](){
    if(logged){
      handleindex();
    }
    else if(server.hasArg("user") && server.hasArg("pass")){
      if(server.arg("user") == USERNAME && server.arg("pass") == PASSWORD){
        Serial.printf("AUTH OK\n");
        logged = true;
        handleindex();
      }}
    else
        Serial.printf("AUTH ERROR\n");
        handleerror();
  });
  server.on("/user_set",[](){
     if(!change_user_pass()){
        logged = false;
        handle_change_error();
     }
     logged = false;
     handlelogin();
    });
  server.on("/change_pass.html",handlechange);
  server.on("/index.html",handleindex);
  server.on("/wifi.html",handlewifi);
  server.on("/style.css",handlecss);
  httpUpdater.setup(&server,WEB_OTA_PATH,WEB_OTA_NAME,WEB_OTA_PASS);
  server.begin();
  Serial.printf("\n********** HTTP server started **********\n\n");
}

void handlelogin(){
    if(logged){
      handleindex();
      return;
      }
    Serial.print("-> /login.html");
    File file = SPIFFS.open("/login.html", "r");  // Open the file
    size_t sent = server.streamFile(file,"text/html");    // Send it to the client
    file.close();
    Serial.print("...SENT\n");
}

void handleindex(){
  if(!logged){
    handle_nolog();
    return;
  }
  Serial.print("-> /index.html");
  File file = SPIFFS.open("/index.html", "r");  // Open the file
  size_t sent = server.streamFile(file,"text/html");    // Send it to the client
  file.close();
  Serial.print("...SENT\n");
}

void handleerror(){
    Serial.print("-> /error_log.html");
  File file = SPIFFS.open("/error_log.html", "r");  // Open the file
  size_t sent = server.streamFile(file,"text/html");    // Send it to the client
  file.close();
  Serial.print("...SENT\n");
}

void handle_nolog(){
   Serial.print("-> /no_log.html");
  File file = SPIFFS.open("/no_log.html", "r");  // Open the file
  size_t sent = server.streamFile(file,"text/html");    // Send it to the client
  file.close();
  Serial.print("...SENT\n");
}

void handlewifi(){
  if(!logged){
    handle_nolog();
    return;
  }
  Serial.print("-> /wifi.html");
  if(!SPIFFS.exists("/wifi.html")){
    Serial.print("...ERROR\n");
    return;
  }
  File file = SPIFFS.open("/wifi.html", "r");  // Open the file
  size_t sent = server.streamFile(file,"text/html");    // Send it to the client
  file.close();
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  Serial.print("...SENT\n");
  if( ssid.length()>0 || pass.length()>0){
    Serial.print("\nRecibed config SSID: "+ ssid +" PASS: "+ pass +"\n");
    write_wifi_config_file(ssid,pass);
  }
  if(server.arg("reset") == "do")
    ESP.restart();
  if(server.arg("erase") == "do"){
    if( SPIFFS.exists(WPA_PATH))
      SPIFFS.remove(WPA_PATH);
    if( SPIFFS.exists(LOG_CONF_PATH))
      SPIFFS.remove(LOG_CONF_PATH);
     ESP.restart();
  }
}

void handlecss(){
  Serial.print("-> /style.css");
  File file = SPIFFS.open("/style.css", "r");  // Open the file
  size_t sent = server.streamFile(file,"text/css");    // Send it to the client
  file.close();
  Serial.print("...SENT\n");
}
 
void handlechange(){
  if(!logged){
    handlelogin();
    return;
  }
  Serial.print("-> /change_pass.html");
  File file = SPIFFS.open("/change_pass.html", "r");  // Open the file
  size_t sent = server.streamFile(file,"text/html");    // Send it to the client
  file.close();
  Serial.print("...SENT\n");
}

void handle_change_error(){
  Serial.print("-> /change_error.html");
  File file = SPIFFS.open("/change_error.html", "r");  // Open the file
  size_t sent = server.streamFile(file,"text/html");    // Send it to the client
  file.close();
  Serial.print("...SENT\n");
}

bool write_wifi_config_file(String rssid,String rpass){
    File wifi = SPIFFS.open(WPA_PATH,"w");
    Serial.printf("\t-> Writting wireless config file");
    wifi.print(rssid+";"+rpass+";\n");
    wifi.close();
    Serial.printf("... OK \n");
}

bool write_login_config(String new_username, String new_pass){
  File login = SPIFFS.open(LOG_CONF_PATH,"w");
  Serial.printf("\t-> Writting logging config file");
  login.print(new_username+";"+new_pass+";\n");
  login.close();
  Serial.printf("... OK \n");
}

void read_login_config(){
  if(!SPIFFS.exists(LOG_CONF_PATH)){
    Serial.printf("\n NOT FOUND USER-PASS CONFIG DATA, USE DEFAULT\n");
    return;
  }
  File login = SPIFFS.open(LOG_CONF_PATH,"r");
  USERNAME = login.readStringUntil(';');
  PASSWORD = login.readStringUntil(';');
  USERNAME.toCharArray(WEB_OTA_NAME,USERNAME.length());
  PASSWORD.toCharArray(WEB_OTA_PASS,PASSWORD.length());
  login.close();
}

bool change_user_pass(){
  if(!logged)
    return false;
  if(!server.hasArg("actual_user") || ! server.hasArg("actual_pass"))
    return false;
  else if (!(server.arg("actual_user") == USERNAME) || !(server.arg("actual_pass")==PASSWORD))
    return false;
  else if (!server.hasArg("new_user") || !server.hasArg("new_pass") || !server.hasArg("new_pass_check"))
    return false;
  if( !(server.arg("new_pass") == server.arg("new_pass_check") ) )
    return false;
  write_login_config(server.arg("new_user"),server.arg("new_pass"));
  USERNAME = server.arg("new_user");
  PASSWORD = server.arg("new_pass");
  Serial.printf("\t\t USER-PASS CHANGED \n");
  return true;
}


