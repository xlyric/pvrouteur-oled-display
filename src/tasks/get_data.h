#ifndef TASK_VOLTAGE
#define TASK_VOLTAGE


#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

HTTPClient httprouter;
extern WiFiClient espClient;

extern IPAddress routeur_ip;

extern float dallas_temperature;
extern int puissance;
extern uint16 routage;

struct Voltage {
    float voltage;
    float get_voltage() {
        float voltage = 0;
        return voltage;
    }
};

Voltage voltage;

// task voltage
void getdata_task() {
    String payload; 
    // récupération du json sur la page web routeur_ip/status
    httprouter.begin(espClient,"http://" + routeur_ip.toString() + "/stateshort");
    int httpCode = httprouter.GET();
    if (httpCode > 0) {
        payload = httprouter.getString();
        Serial.println(payload);
    }
    httprouter.end();

    //découper le json
    JsonDocument doc;
    deserializeJson(doc, payload);
    dallas_temperature = doc["temperature"];
    puissance = doc["watt"];
    routage = doc["dimmer"];

}

#endif  // TASK_VOLTAGE