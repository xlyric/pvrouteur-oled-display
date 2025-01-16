#include <Arduino.h>

// Web services
#include <ESPAsyncWiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
 #include <ESP8266mDNS.h>
// ota mise à jour sans fil
#include <ElegantOTA.h>

// Json
#include <ArduinoJson.h> // ArduinoJson v6

// Config
#include "config/config.h"

//tasks
#include <TaskScheduler.h> // gestion des taches
#include "tasks/get_data.h"
#include "tasks/oled.h"

#include "functions/mdns.h"

// NTP
#include "functions/ntp.h"

// functions
#include "functions/web.h"
#include "config/enums.h"

WiFiClient espClient;
Configmqtt configmqtt;
void callback(char* topic, byte* payload, unsigned int length);

Task Task_oled(990, TASK_FOREVER, &oled_task); // tache oled
//Task Task_getdata(10001, TASK_FOREVER, &getdata_task); // tache de récupération des data
Task meteo_data(3600003, TASK_FOREVER, &meteo_task); // tache de récupération des données météo toute les heures
Scheduler runner;

float dallas_temperature = 0.0;
int puissance = 0;
uint16 routage = 0;
IPAddress routeur_ip;

#define TZ              1       // (utc+) TZ in hours
#define DST_MN          60      // use 60mn for summer time in some countries
#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)
time_t now;
Configmeteo configmeteo;
bool connect_mqtt = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RX, INPUT);

  if (!configmeteo.recup_map()) {
    Serial.println("Erreur de récupération de la configuration météo");
  }
  
  OPEN_WEATHER_MAP_APP_ID = configmeteo.MAP_ID;
  OPEN_WEATHER_MAP_LOCATION = configmeteo.MAP_LOCATION;
  /// init des tasks 
  runner.init();
  runner.addTask(Task_oled); // ajout de la tache oled
  
  oled.init();
  oled.wait_for_wifi(0);
  runner.enableAll();
  
  // doit être placé avant la connexion sinon sous ESP32, l'hostname n'est pas pris en compte
  WiFi.setHostname(HOSTNAME);
  wifiManager.autoConnect(HOSTNAME);
  // Afficher le nom :
  wifiManager.setConfigPortalTimeout(600);


  // Wait for Wifi connection
  oled.wait_for_wifi(1);
  ntpinit();
  WiFi.setAutoReconnect(true);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //***********************************
  //************* Setup - OTA
  //***********************************
  ElegantOTA.begin(&server);      // Start ElegantOTA
  ElegantOTA.setAutoReboot(true);

  // ***********************************
  //************* Setup - mdns

  mdns_hello(HOSTNAME); 

  //***********************************
  //************* Setup - pages web
  Serial.println("start server");
  call_pages();
  mdns_discover_routeur();
  init_ui();  

  configmqtt.recup_config();

  if (configmqtt.recup_mqtt()) {
    Serial.println("MQTT_PASSWORD présent");
    connect_mqtt = true;
  }
  else {
    Serial.println("MQTT_PASSWORD absent");
  }

  if (connect_mqtt) {
    configmqtt.connect_mqtt();
  }

  //runner.addTask(Task_getdata); // ajout de la tache voltage
  runner.addTask(meteo_data); // ajout de la tache météo
  runner.enableAll();

  // Inital UI takes care of initalising the display too.
  //updateData(&display);
    
}


void loop() {
  runner.execute();
  if (connect_mqtt) {
      configmqtt.loop_mqtt();
  }

  delay(500);
  // put your main code here, to run repeatedly:

  // detection de RX = 0 
  if (digitalRead(RX) == LOW) {
    Serial.println("RX = 0");
    delay(1000);
    // suppresion de la configuration Wifi 
    // arret du wifi 
    WiFi.disconnect();
    wifiManager.resetSettings();
    ESP.restart();

  }

}

    void callback(char* topic, byte* payload, unsigned int length) {
        //Serial.print("Message arrived [");
        //Serial.print(topic);
        //Serial.print("] ");
        for (int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
        }
        //Serial.println();
        if (strcmp(topic,(configmqtt.topic+"/statepower").c_str()) == 0) {
            //parse du json 
            JsonDocument doc;
            deserializeJson(doc, payload);
            //récupération de la valeur 
            puissance = doc["power"];
        }
        if (strcmp(topic,(configmqtt.topic+"/stateRouted").c_str()) == 0) {
            //parse du json 
            JsonDocument doc;
            deserializeJson(doc, payload);
            //récupération de la valeur 
            routage = doc["routed"];
        }
        if (strcmp(topic,(configmqtt.topic+"/stateDallas").c_str()) == 0) {
            //parse du json 
            JsonDocument doc;
            deserializeJson(doc, payload);
            //récupération de la valeur 
            dallas_temperature = doc["Dallas"];
        }

    };

