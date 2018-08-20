#ifndef CONFIG_H
#define CONFIG_H

#define MAX_LONG_STR 100

#define WEB_OTA_PATH "/update"
char* WEB_OTA_NAME = "admin";
char* WEB_OTA_PASS = "admin"; 

#define OTA_NAME "OUT_CONTROL"
#define OTA_PASS "mdardis" 
#define OTA_MD5 "d4d0f48c29fa625f6acc2bf70a353a0b"

bool logged = false; //Init without logged session

const int WAIT_TIME = 40; //Time (in seconds) to wait wifi before enable AP mode

#define GPIO0 D1
bool gpio0_state=false;
#define GPIO1 D2
bool gpio1_state=false;

#define WPA_PATH "/wireless.txt"
#define AP_SSID "ESP_8266"  
#define AP_PASS "control"

#define LOG_CONF_PATH "/logging.dat"
String USERNAME = "admin";//Default username
String PASSWORD = "admin";//Default password

#endif 
