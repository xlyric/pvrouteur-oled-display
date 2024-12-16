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

// functions
#include "functions/web.h"

WiFiClient espClient;

Task Task_oled(5500, TASK_FOREVER, &oled_task); // tache oled
Task Task_getdata(10001, TASK_FOREVER, &getdata_task); // tache de récupération des data
Scheduler runner;

float dallas_temperature = 0.0;
int puissance = 0;
uint16 routage = 0;
IPAddress routeur_ip;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  /// init des tasks 
  runner.init();
  runner.addTask(Task_oled); // ajout de la tache oled
  runner.addTask(Task_getdata); // ajout de la tache voltage
  oled.init();
  runner.enableAll();

  // démarrage file system
  //LittleFS.begin();

  // doit être placé avant la connexion sinon sous ESP32, l'hostname n'est pas pris en compte
  WiFi.setHostname(HOSTNAME);
  wifiManager.autoConnect(HOSTNAME);
  // Afficher le nom :
  //wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setConfigPortalTimeout(600);

  // Wait for Wifi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
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

}

void loop() {
  runner.execute();
  delay(1000);
  // put your main code here, to run repeatedly:
}

