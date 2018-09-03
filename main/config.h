#ifndef CONFIG_H
#define CONFIG_H

#define FW_VERSION "1.3.1"
#define SERIAL_SPEED 115200
/* OUTPUTS SETTINGS*/
#define GPIO0 D1
#define GPIO1 D2

/* WIFI SETTINGS*/
#define WPA_PATH "/wireless.txt"
const int WAIT_TIME = 120; //Time (in seconds) to wait wifi before enable AP mode
#define AP_SSID "ESP_8266"  
#define AP_PASS "control"

/* LOGGING SETTINGS*/
#define LOG_CONF_PATH "/logging.dat"
String USERNAME = "admin";//Default username
String PASSWORD = "admin";//Default password
bool logged = false; //Init without logged session

/* OTA SETTINGS*/

#define WEB_OTA_PATH "/update"
char* WEB_OTA_NAME = "admin";//Default changes with username to login
char* WEB_OTA_PASS = "admin"; //Default changes with password to login

#define OTA_NAME "OUT_CONTROL"
#define OTA_PASS "mdardis" 
#define OTA_MD5 "d4d0f48c29fa625f6acc2bf70a353a0b"

#endif 
