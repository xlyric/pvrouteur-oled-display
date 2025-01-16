#ifndef FUNCTION_MDNS
#define FUNCTION_MDNS

#include <ESP8266mDNS.h>
#include <Arduino.h>
#include "config/config.h"
#include "config/enums.h"

extern IPAddress routeur_ip;
extern Configmqtt configmqtt; 

void mdns_hello(String esp_name) {
    
    if (!MDNS.begin(esp_name.c_str())) {   
    Serial.println("Error setting up MDNS responder!");
    while(1) {
      delay(1000);
    }
     }
    Serial.println("mDNS responder started");
    MDNS.addService("sunstain", "tcp", 80);
    MDNS.addServiceTxt("sunstain", "tcp", "name", esp_name.c_str());
    MDNS.addServiceTxt("sunstain", "tcp", "version", VERSION);
    MDNS.addServiceTxt("sunstain", "tcp", "fonction", "Oled");
    MDNS.addServiceTxt("sunstain", "tcp", "url", "https://www.sunstain.fr");
    MDNS.addServiceTxt("sunstain", "tcp", "update_url", "https://ota.apper-solaire.org/ota.php");
    MDNS.addService("http", "tcp", 80);

}

void mdns_discover_routeur() {
  int nrOfServices = MDNS.queryService("sunstain", "tcp");
   
  if (nrOfServices == 0) {
    Serial.println("No services were found.");
  } 
   
  else {
     
    Serial.print("Number of services found: ");
    Serial.println(nrOfServices);

    for (int i = 0; i < nrOfServices; i++) {
      Serial.print("Service ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(MDNS.hostname(i));
      Serial.print(" (");
      Serial.print(MDNS.IP(i));
      Serial.print(":");
      Serial.print(MDNS.port(i));
      Serial.println(")");
      
      // récupération de l'adresse mac du routeur PV-ROUTER-xxxx.local -> xxxx pour le MQTT

      configmqtt.mac_routeur = MDNS.hostname(i).substring(10,14);
      Serial.println(configmqtt.mac_routeur);

      // si le nom contient la chaine routeur on sort ( en lowercase)
        if (MDNS.hostname(i).indexOf("ROUTER") != 0) {
            Serial.println("Routeur trouvé");
            routeur_ip = MDNS.IP(i);
            Serial.println(routeur_ip);
            break;
        }
    }     
  }

}

#endif