#ifndef FUNCTION_SHELLY
#define FUNCTION_SHELLY

#include <Arduino.h>
#include "config/config.h"
#include "config/enums.h"

#ifdef SHELLY

struct Shelly
{

   // IPAddress shelly_ip;
    IPAddress shelly_ip;

    // fonction de récupération des valeurs sur le shelly
    // la page a récupérer est http://shelly_ip/rpc/EM1.GetStatus?id={0,1}
    int shelly_port(int port){
        HTTPClient http;
        String payload;
        http.begin(espClient,"http://" + shelly_ip.toString() + "/rpc/EM1.GetStatus?id="+port);
        int httpCode = http.GET();
        if (httpCode > 0) {
            payload = http.getString();
        }
        #ifdef DEBUG
        Serial.println(shelly_ip.toString()+"/rpc/EM1.GetStatus?id="+port);
        Serial.println(payload);
        #endif
        // le retour est un json de type {"id":0,"current":2.540,"voltage":232.8,"act_power":-22.9,"aprt_power":592.2,"pf":0.50,"freq":50.0,"calibration":"factory"} et on récuperer act_power
        JsonDocument doc;
            deserializeJson(doc, payload);

        http.end();
        return doc["act_power"].as<int>();
    };    
        
};



#endif
#endif