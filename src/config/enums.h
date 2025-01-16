#ifndef ENUMS
#define ENUMS

#include <Preferences.h> 
// Json
#include <ArduinoJson.h>
// Web services
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <PubSubClient.h>

PubSubClient client(espClient);

extern IPAddress routeur_ip;
void callback(char* topic, byte* payload, unsigned int length);

struct Configmqtt {
  Preferences preferences;
  char MQTT_SERVER[65]; // NOSONAR
  char MQTT_USER[65]; // NOSONAR
  char MQTT_PASSWORD[65]; // NOSONAR
  uint16_t MQTT_PORT;
  String mac_routeur;
  String topic;
  
    public:bool sauve_mqtt() {
    preferences.begin("MQTT", false);
    preferences.putString("password",MQTT_PASSWORD);
    preferences.end();
    return true; 
    }

    public:bool recup_mqtt() {
    preferences.begin("MQTT", false);
    String tmp;
    tmp = preferences.getString("password", "");
    tmp.toCharArray(MQTT_PASSWORD,65);
    preferences.end();
    if (strcmp(MQTT_PASSWORD,"") == 0) { return false; }
    return true;
    }

    // récupération de la configuration mqtt depuis la page routeur /getmqtt
    public:void recup_config() {
        // récupération du json sur /getmqtt
        HTTPClient httpmqtt;
        String payload;
        httpmqtt.begin(espClient,"http://" + routeur_ip.toString() + "/getmqtt");
        int httpCode = httpmqtt.GET();
        if (httpCode > 0) {
            payload = httpmqtt.getString();
            Serial.println(payload);
        }
        httpmqtt.end();
        //découper le json
        JsonDocument doc;
        deserializeJson(doc, payload);
        strcpy(MQTT_SERVER,doc["server"]);
        strcpy(MQTT_USER,doc["user"]);
        MQTT_PORT = doc["port"];
        Serial.println("MQTT_SERVER : ");
        Serial.println(MQTT_SERVER);
        recup_mqtt(); 
    }

    public:bool connect_mqtt() {
        u_int8_t timeout=0;
        Serial.println("Connexion au serveur MQTT");
        client.setServer(MQTT_SERVER, MQTT_PORT);
        client.setCallback(callback);
        const String pvname = String("PV-Oled-") + WiFi.macAddress().substring(12,14)+ WiFi.macAddress().substring(15,17);
        while (!client.connected()) {
        Serial.println("-----------------------------");
        Serial.println("Attempting MQTT reconnection...");
        // Attempt to connect

        if (client.connect(pvname.c_str(), MQTT_USER, MQTT_PASSWORD)) {
          client.setKeepAlive(15);
          Serial.println("MQTT_connected");
          subscribe_mqtt();
        } else {
          Serial.print("MQTT failed, retcode="); 
          Serial.print(client.state());
          Serial.println(" try again in 2 seconds");
          ///dans le doute si le mode AP est actif on le coupe
          Serial.println(WiFi.status());
          // Wait 2 seconds before retrying
          delay(2000);  // 24/01/2023 passage de 5 à 2s 
          timeout++;
          if (timeout > 10) {
            Serial.println("Timeout MQTT");
            return false;
          }
        }
      }
        return true;
    }

    public:void subscribe_mqtt() {
        // subscribe aux topics homeassistant/sensor/PvRouter-xxxx/statepower
        topic = "homeassistant/sensor/PvRouter-" + mac_routeur; 
        client.subscribe((topic + "/statepower").c_str());
        // et homeassistant/sensor/PvRouter-xxxx/stateRouted
        client.subscribe((topic + "/stateRouted").c_str());
        // et homeassistant/sensor/PvRouter-xxxx/stateDallas
        client.subscribe((topic+ "/stateDallas").c_str());
        client.subscribe("test");
        Serial.println("Subscribed to : ");
        Serial.println((topic + "/statepower").c_str());
        client.loop();
    }

    public:void loop_mqtt() {
        client.loop();
    }
};


   



#endif