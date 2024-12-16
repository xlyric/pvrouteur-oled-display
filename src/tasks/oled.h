#ifndef TASK_OLED
#define TASK_OLED

#include <OLEDDisplay.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include <ESPAsyncWiFiManager.h>

SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL);
#define defaultFont ArialMT_Plain_16

extern float dallas_temperature;
extern int puissance;
extern uint16 routage;

struct Oled {

// init oled
    void init() {
        // Initialising the UI will init the display too.
        display.init();
        display.flipScreenVertically();
        display.setFont(defaultFont);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setContrast(50, 100, 30);
    }   

// affichage de la température
    void display_temperature(float temperature) {
        display.drawString(0, 20, "Temp:");
        display.drawString(64, 20, String(temperature));
    }

// affichage de la tension
    void display_puissance(int puissance) {
        display.drawString(64, 40, "P: " + String(puissance));
    }

    void display_routage(uint32 routage) {
        display.drawString(0, 40, "R: " + String(routage));
    }

// affichage de l'IP
    void display_ip() {
        display.setFont(ArialMT_Plain_10);
        String ip = WiFi.localIP().toString();
        String rssi = String(WiFi.RSSI());
        display.drawString(0, 0, ip);
        display.drawString(100, 0, rssi + "dB");
        display.setFont(defaultFont);
    }

};

Oled oled; 
// task oled
void oled_task() {
    display.clear();
    oled.display_ip();
    oled.display_temperature(dallas_temperature);
    oled.display_puissance(puissance);
    oled.display_routage(routage);
    display.display();

}


#endif  // TASK_OLED