#ifndef CONFIG_H
#define CONFIG_H

#define FW_VERSION "1.4.1"
#define SERIAL_SPEED 115200
/* OUTPUTS SETTINGS*/
#define GPIO0 D1
#define GPIO1 D2

/* WIFI SETTINGS*/
#define WPA_PATH "/wireless.txt"
#define WAIT_TIME_CT 30; //Time (in seconds) to wait wifi before enable AP mode
#define AP_SSID "ESP_8266"
#define AP_PASS "control"

/* LOGGING SETTINGS*/
#define LOG_CONF_PATH "/logging.dat"
#define USERNAME_CT "admin";//Default username
#define PASSWORD_CT "admin";//Default password
#define LOGGED true; //Init without logged session

/* OTA SETTINGS*/

#define WEB_OTA_PATH "/update"
#define WEB_OTA_NAME_CT USERNAME_CT;//Default changes with username to login
#define WEB_OTA_PASS_CT PASSWORD_CT; //Default changes with password to login

#define OTA_NAME "OUT_CONTROL"
#define OTA_PASS "mdardis"
#define OTA_MD5 "d4d0f48c29fa625f6acc2bf70a353a0b"

/* RANDOM STUFF*/
#define MAX_LONG_STR 50
#define mDNS "esp8266"
#define BOOTSTRAP_CDN "https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css"
#define DEBUG true
#endif
