#include "WebServerHandlers.h"

DynamicJsonDocument jsonDoc(2048);

void startServer(){
  if (DEBUG){
    server.on("/erase",[](){      //Coment this code section, only for dev use.
      SPIFFS.remove(LOG_CONF_PATH);
      ESP.restart();
      });
  }
  server.on("/disconnect",[](){
    logged = false;
    handlelogin();
  });
  server.on("/",[](){  !logged ? handlelogin() : handleindex();  });
  server.on("/user_set",handle_user_set);
  server.on("/info",handle_report_status);
  server.on("/status",handle_status_json);
  server.on("/auth",handle_auth);
  server.on("/change_pass.html",handlechange);
  server.on("/index.html",handleindex);
  server.on("/wifi.html",handlewifi);
  server.on("/style.css",handlecss);
  server.on("/bootstrap.css",handlebootstrap);
  server.begin();
  Serial.printf("\n********** HTTP server started **********\n\n");
}

void startWiFi(){
  Serial.printf("\n********** Starting WiFi **********\n\n");
  if( !SPIFFS.exists(WPA_PATH) ){
    WiFi.mode(WIFI_AP);
    delay(20);
    WiFi.softAP(AP_SSID,AP_PASS);
    Serial.printf("No WiFi config. Using default config \n");
    return;
  }
  WiFi.mode(WIFI_STA);
  delay(20);
  char ssid[MAX_LONG_STR];
  char pass[MAX_LONG_STR];
  File wifi = SPIFFS.open(WPA_PATH,"r");
  Serial.printf("Reading wireless config file ... OK\n");
  while(wifi.position() < wifi.size()){// Not function now, future use DONT DELETE
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

void handlebootstrap(){
  Serial.print("-> /bootstrap.css");
  if (WiFi.status() == 3){
    Serial.print(" USING CDN!");
    server.sendHeader("Location", String(BOOTSTRAP_CDN), true);
    server.send ( 302, "text/css", "");
  }else {
    File file = SPIFFS.open("/bootstrap.css", "r");  // Open the file
    size_t sent = server.streamFile(file,"text/css");    // Send it to the client
    file.close();
  }
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

void handle_report_status(){
  String message;
  StaticJsonDocument<255> wifi;
  StaticJsonDocument<255> info;
  wifi["SSID"] = WiFi.SSID();
  wifi["IP"] = WiFi.localIP().toString();
  wifi["MAC"] = WiFi.macAddress();
  wifi["status"] = String(WiFi.status());
  info["FW"] = FW_VERSION;
  info["free_space"] = String(ESP.getFreeSketchSpace());
  info["free_heap"] = String(ESP.getFreeHeap());
  info["free_stack"] = String(ESP.getFreeContStack());
  info["blocked"] = String(!logged);
  jsonDoc["wifi_data"] = wifi;
  jsonDoc["info"] = info;
  serializeJson(jsonDoc,message);
  server.send(200,"text/plain",message);
  jsonDoc.clear();
}

void handle_auth(){
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
}

void handle_user_set(){
      logged = false;
    if(!change_user_pass()){
        handle_change_error();
    }
    handlelogin();
}

void handle_status_json(){
    String message;
    StaticJsonDocument<128> doc;
    doc["gpio0"] =  gpio0_state?"true":"false";
    doc["gpio1"] =  gpio1_state?"true":"false";
    jsonDoc["outputs"] = doc;
    serializeJson(jsonDoc,message);
    server.send(200,"application/json",message);
    jsonDoc.clear();
}

