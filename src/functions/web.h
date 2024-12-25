#ifndef WEB_FUNCTIONS
#define WEB_FUNCTIONS

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <Preferences.h>  // pour la sauvegarde des données

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
  tmp = preferences.getString("id", "AP");
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
    String message = "Create ID https://docs.thingpulse.com/how-tos/openweathermap-key/ \n"; 
    request->send(200, "text/plain", message);
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