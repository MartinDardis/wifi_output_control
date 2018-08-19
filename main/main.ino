#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <FS.h>   // Include the SPIFFS library
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

#define MAX_LONG_STR 100
#define OTA_PASS "mdardis"
#define OTA_MD5 "d4d0f48c29fa625f6acc2bf70a353a0b"
bool logged = false;


#define GPIO0 D1
bool gpio0_state=false;
#define GPIO1 D2
bool gpio1_state=false;

#define WPA_PATH "/wireless.txt"
#define AP_SSID "ESP_8266"  
#define AP_PASS "control"
#define LOG_CONF_PATH "/log.dat"
String USERNAME = "admin";
String PASSWORD = "admin";

ESP8266WebServer server(80);

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
bool write_wifi_config_file(String ssid,String pass);
bool write_login_config(String new_username, String new_pass);

void setup() {
  Serial.begin(115200);         
  Serial.println('\n');
  delay(1000);
  Serial.print("\nESP8266 OUT CONTROL - MARTIN DARDIS 2018\n");
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(GPIO0,OUTPUT);
  pinMode(GPIO1,OUTPUT);
  startFS();
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
  Serial.printf("\n********** Iniciando WiFi **********\n\n");
  if(SPIFFS.exists(WPA_PATH)){
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
  Serial.print("\nIniciando Wifi \nConectando  ");
  int i = 0;
  for (int i=0 ;WiFi.status() != WL_CONNECTED && i< 181; i++){ // Wait for the Wi-Fi to connect
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
    Serial.print("Conectado a: ");
    Serial.print(WiFi.SSID());              // Tell us what network we're connected to
    Serial.print("\tDireccion IP: ");
    Serial.print(WiFi.localIP());
  }
}

void startOTA(){
   ArduinoOTA.setPort(8266);
   ArduinoOTA.setHostname("OUT_CONTROL");
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
  server.on("/config",[](){
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
  server.on("/index.html",handleindex);
  server.on("/wifi.html",handlewifi);
  server.on("/style.css",handlecss);
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
  }
}

void handlecss(){
  Serial.print("-> /style.css");
  File file = SPIFFS.open("/style.css", "r");  // Open the file
  size_t sent = server.streamFile(file,"text/css");    // Send it to the client
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
  login.close();
}

