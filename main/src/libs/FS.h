#ifndef FS_LOC_H
#define FS_LOC_H

#include <FS.h>
#include <ESP8266WebServer.h>
#include "config.h"

extern ESP8266WebServer server;
extern String USERNAME;//Default username
extern String PASSWORD;//Default password
extern bool logged; //Init without logged session
extern char* WEB_OTA_NAME;//Default changes with username to login
extern char* WEB_OTA_PASS; //Default changes with password to login

void startFS();
void read_login_config();
bool write_wifi_config_file(String rssid,String rpass);
bool change_user_pass();
bool write_login_config(String new_username, String new_pass);


#endif