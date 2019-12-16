#include "FS.h"


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

void read_login_config(){
  if(!SPIFFS.exists(LOG_CONF_PATH)){
    Serial.printf("\n NOT FOUND USER-PASS CONFIG DATA, USE DEFAULT\n");
    return;
  }
  File login = SPIFFS.open(LOG_CONF_PATH,"r");
  USERNAME = login.readStringUntil(';');
  PASSWORD = login.readStringUntil(';');
  int user_leng = USERNAME.length()+1;
  int pass_leng = PASSWORD.length()+1;
  WEB_OTA_NAME = ((char*)malloc(sizeof(char)* (user_leng+2)));
  WEB_OTA_PASS = ((char*)malloc(sizeof(char)* (pass_leng+2)));
  USERNAME.toCharArray(WEB_OTA_NAME,user_leng);
  PASSWORD.toCharArray(WEB_OTA_PASS,pass_leng);
  login.close();
}

bool write_wifi_config_file(String rssid,String rpass){
    File wifi = SPIFFS.open(WPA_PATH,"w");
    Serial.printf("\t-> Writting wireless config file");
    wifi.print(rssid+";"+rpass+";\n");
    wifi.close();
    Serial.printf("... OK \n");
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


bool write_login_config(String new_username, String new_pass){
  File login = SPIFFS.open(LOG_CONF_PATH,"w");
  Serial.printf("\t-> Writting logging config file");
  login.print(new_username+";"+new_pass+";\n");
  login.close();
  Serial.printf("... OK \n");
}