#ifndef WEB_FUNCTIONS
#define WEB_FUNCTIONS

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <Preferences.h>  // pour la sauvegarde des données

#include "config/enums.h"
extern Configmqtt configmqtt;

DNSServer dns;
AsyncWebServer server(80);
AsyncWiFiManager wifiManager(&server,&dns);

extern float dallas_temperature;
extern int puissance;
extern uint16 routage;


String status();

/// @brief //////////
// structure pour la configuration de la partie  wifi 
struct Configmeteo {
  
  Preferences preferences;

  char MAP_ID[36]; // NOSONAR
  char MAP_LOCATION[65]; // NOSONAR

  public:bool sauve_map() {
  preferences.begin("OPEN_WEATHER", false);
  preferences.putString("id",MAP_ID);
  preferences.putString("city",MAP_LOCATION);
  preferences.end();
  return true; 
  }

  public:bool recup_map() {
  preferences.begin("OPEN_WEATHER", false);
  String tmp; 
  tmp = preferences.getString("id", "");
  tmp.toCharArray(MAP_ID,36);
  tmp = preferences.getString("city", "");
  tmp.toCharArray(MAP_LOCATION,65);
  preferences.end();
  if (strcmp(MAP_ID,"") == 0) { return false; }
  return true;
  }
};

extern Configmeteo configmeteo;
// call_pages : appel des pages web

void call_pages() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", status());
  });
  
  server.on("/set", HTTP_ANY, [] (AsyncWebServerRequest *request) {
      // si le paramètre est id on sauvegarde la valeur
      if (request->hasParam("id")) {
        configmeteo.MAP_ID[0] = '\0';
        request->getParam("id")->value().toCharArray(configmeteo.MAP_ID,36);
        configmeteo.sauve_map();
        // et on applique les changements
        OPEN_WEATHER_MAP_APP_ID = configmeteo.MAP_ID;
      }

      // si le paramètre est city on sauvegarde la valeur
      if (request->hasParam("city")) {
        configmeteo.MAP_LOCATION[0] = '\0';
        request->getParam("city")->value().toCharArray(configmeteo.MAP_LOCATION,65);
        configmeteo.sauve_map();
        // et on applique les changements
        OPEN_WEATHER_MAP_LOCATION = configmeteo.MAP_LOCATION;
      }

      // si le paramettre est mqttpass on sauvegarde la valeur
      if (request->hasParam("mqttpass")) {
        request->getParam("mqttpass")->value().toCharArray(configmqtt.MQTT_PASSWORD,65);
        configmqtt.sauve_mqtt();
      }

      // si le paramètre est shelly_ip on sauvegarde la valeur
      if (request->hasParam("shelly_ip")) {
        request->getParam("shelly_ip")->value().toCharArray(configmqtt.shelly_ip,20);
        Serial.println(configmqtt.shelly_ip);
        configmqtt.sauve_mqtt();
      }


    const char* message = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Weather Info</title>
</head>
<body>
    <h1>You can add weather information!</h1>
    <p>To do this, you'll need to create an API key by following the <a href="https://docs.thingpulse.com/how-tos/openweathermap-key/">instructions on the website</a></p>
    <p>Once you have your API key and city, you can inject it into your web pages with a GET request in this format :</p>
    <pre>
        http://myIP.oled/set?city=mycity,FR
        and:
        http://myIP.oled2/set?id=my_api_key
    </pre>
    <h1>And add MQTT Password!</h1>
    <pre>
        http://myIP.oled/set?mqttpass=my_mqtt_password
    </pre>
    <h1>For take Shelly information</h1>
    <pre>
        http://myIP.oled/set?shelly_ip=Shelly_ip
    </pre>
</body>
</html>
)";
    request->send(200, "text/html", message);
  });


  // Start server
  server.begin();
}

String status() {
  // remonté json de l état de la batterie
  String message; 
  JsonDocument doc; 
  doc["temperature"] = dallas_temperature;
  doc["tension"] = puissance;
  doc["routage"] = routage;
  serializeJson(doc, message) ;

  return message;
}





#endif  // WEB_FUNCTIONS