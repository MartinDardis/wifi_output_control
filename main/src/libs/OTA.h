#ifndef OTA_H
#define OTA_H
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoOTA.h>
#include "config.h"
extern ESP8266WebServer server;
extern ESP8266HTTPUpdateServer httpUpdater;
extern char* WEB_OTA_NAME;//Default changes with username to login
extern char* WEB_OTA_PASS; //Default changes with password to login

void startOTA();

#endif