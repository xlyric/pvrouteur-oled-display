#include <Arduino.h>

// Web services
#include <ESPAsyncWiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
 #include <ESP8266mDNS.h>
// ota mise à jour sans fil
#include <ElegantOTA.h>

// MQTT
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

WiFiClient espClient;

Task Task_oled(990, TASK_FOREVER, &oled_task); // tache oled
Task Task_getdata(5000, TASK_FOREVER, &getdata_task); // tache de récupération des data
Task meteo_data(60003, TASK_FOREVER, &meteo_task); // tache de récupération des données météo
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  configmeteo.recup_map();
  OPEN_WEATHER_MAP_APP_ID = configmeteo.MAP_ID;
  OPEN_WEATHER_MAP_LOCATION = configmeteo.MAP_LOCATION;
  Serial.println(configmeteo.MAP_ID);
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

  runner.addTask(Task_getdata); // ajout de la tache voltage
  runner.addTask(meteo_data); // ajout de la tache météo
  runner.enableAll();

  // Inital UI takes care of initalising the display too.
  //updateData(&display);
    
}

void loop() {
  runner.execute();
  delay(500);
  // put your main code here, to run repeatedly:
}

