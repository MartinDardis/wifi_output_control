#ifndef WEB_SERVER_HANDLERS_H
#define WEB_SERVER_HANDLERS_H

#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include "FS.h"   // Include the SPIFFS library
#include "config.h"
#include <ArduinoJson.h>

extern ESP8266WebServer server;
extern const int WAIT_TIME; //Time (in seconds) to wait wifi before enable AP mode
extern String USERNAME;//Default username
extern String PASSWORD;//Default password
extern bool logged; //Init without logged session
extern bool gpio1_state;
extern bool gpio0_state;

void startServer();
void startWiFi();
void handleindex();
void handlewifi();
void handlecss();
void handlebootstrap();
void handlelogin();
void handle_nolog();
void handleerror();
void handlechange();
void handle_change_error();
void handle_report_status();
void handle_auth();
void handle_user_set();
void handle_status_json();
void handleNotFound();


#endif