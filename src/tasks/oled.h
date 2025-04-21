#ifndef TASK_OLED
#define TASK_OLED

#include <OLEDDisplay.h>
#include "OLEDDisplayUi.h"
#include <Wire.h>
#include "SSD1306Wire.h"
#include <ESPAsyncWiFiManager.h>

#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "WeatherStationFonts.h"
#include "WeatherStationImages.h"

#include "config/config.h"

OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapCurrent currentWeatherClient;

OpenWeatherMapForecastData forecasts[MAX_FORECASTS];
OpenWeatherMapForecast forecastClient;

extern time_t now;

SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL);
OLEDDisplayUi   ui( &display );
#define defaultFont ArialMT_Plain_16

extern float dallas_temperature;
extern int puissance;
extern uint16 routage;

// définition des fonctions 
void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);

void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex);
void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
void drawProgress(OLEDDisplay *display, int percentage, String label);
void updateData(OLEDDisplay *display);
void display_routage(OLEDDisplay *display, OLEDDisplayUiState* state, short int x, short int y);
void display_temperature(OLEDDisplay *display, OLEDDisplayUiState* state, short int x, short int y);

void init_ui(); 

struct Oled {

// init oled
    void init() {
        // Initialising the UI will init the display too.
        display.init();
        display.clear();
        display.display();
        //display.flipScreenVertically();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setContrast(50, 100, 30);
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

    void wait_for_wifi(uint8_t counter) {
    display.clear();
    display.drawString(64, 10, "Connecting to WiFi");
    display.drawXbm(46, 30, 8, 8, counter % 3 == 0 ? activeSymbole : inactiveSymbole);
    display.drawXbm(60, 30, 8, 8, counter % 3 == 1 ? activeSymbole : inactiveSymbole);
    display.drawXbm(74, 30, 8, 8, counter % 3 == 2 ? activeSymbole : inactiveSymbole);
    display.display();
    }

    
};

Oled oled; 
#ifdef SHELLY
FrameCallback frames[] = { display_routage, drawDateTime, drawCurrentWeather, drawForecast  };
   int numberOfFrames = 4;
#else
FrameCallback frames[] = { display_temperature, display_routage, drawDateTime, drawCurrentWeather, drawForecast  };
   int numberOfFrames = 5;
#endif
OverlayCallback overlays[] = { drawHeaderOverlay };
   int numberOfOverlays = 1;



// task oled et meteo 
void oled_task() {
    ui.update();

}

void meteo_task() {
    updateData(&display);
    ui.update();
}

// affichage de l'heure
    void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    now = time(nullptr);
    struct tm* timeInfo;
    timeInfo = localtime(&now);
    char buff[16];

    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);
    String date = WDAY_NAMES[timeInfo->tm_wday];

    sprintf_P(buff, PSTR("%s, %02d/%02d/%04d"), WDAY_NAMES[timeInfo->tm_wday].c_str(), timeInfo->tm_mday, timeInfo->tm_mon+1, timeInfo->tm_year + 1900);
    display->drawString(64 + x, 5 + y, String(buff));
    display->setFont(ArialMT_Plain_24);

    sprintf_P(buff, PSTR("%02d:%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    display->drawString(64 + x, 15 + y, String(buff));
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    }

// affichage de la météo
    void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    // comparaison si OPEN_WEATHER_MAP_LOCATION != ""   
        if (OPEN_WEATHER_MAP_LOCATION != nullptr && OPEN_WEATHER_MAP_APP_ID != nullptr) {
            display->setFont(ArialMT_Plain_10);
            display->setTextAlignment(TEXT_ALIGN_CENTER);
            display->drawString(64 + x, 38 + y, currentWeather.description);

            display->setFont(ArialMT_Plain_24);
            display->setTextAlignment(TEXT_ALIGN_LEFT);
            String temp = String(currentWeather.temp, 1) + (IS_METRIC ? "°C" : "°F");
            display->drawString(60 + x, 5 + y, temp);

            display->setFont(Meteocons_Plain_36);
            display->setTextAlignment(TEXT_ALIGN_CENTER);
            display->drawString(32 + x, 0 + y, currentWeather.iconMeteoCon);
        }
        //sinon affichage des temperatures 
        else {
            display_temperature(display, state, x, y);

        }
    }

// affichage des prévisions météo
    void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
        if (OPEN_WEATHER_MAP_LOCATION != nullptr && OPEN_WEATHER_MAP_APP_ID != nullptr) {
            drawForecastDetails(display, x, y, 0);
            drawForecastDetails(display, x + 44, y, 1);
            drawForecastDetails(display, x + 88, y, 2);
        }
        else {
            display_routage(display, state, x, y);
        }

    }

// affichage des détails de la météo
    void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex) {
    time_t observationTimestamp = forecasts[dayIndex].observationTime;
    struct tm* timeInfo;
    timeInfo = localtime(&observationTimestamp);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);
    display->drawString(x + 20, y, WDAY_NAMES[timeInfo->tm_wday]);

    display->setFont(Meteocons_Plain_21);
    display->drawString(x + 20, y + 12, forecasts[dayIndex].iconMeteoCon);
    String temp = String(forecasts[dayIndex].temp, 0) + (IS_METRIC ? "°C" : "°F");
    display->setFont(ArialMT_Plain_10);
    display->drawString(x + 20, y + 34, temp);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    }

//// affichage des infos basse de l'écran
    void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
    now = time(nullptr);
    struct tm* timeInfo;
    timeInfo = localtime(&now);
    char buff[14];
    sprintf_P(buff, PSTR("%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min);

    display->setColor(WHITE);
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0, 54, String(buff));
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    //String temp = String(currentWeather.temp, 1) + (IS_METRIC ? "°C" : "°F");
    display->drawString(128, 54, String(puissance));
    display->drawHorizontalLine(0, 52, 128);
    }


    void drawProgress(OLEDDisplay *display, int percentage, String label) {
    display->clear();
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);
    display->drawString(64, 10, label);
    display->drawProgressBar(2, 28, 124, 10, percentage);
    display->display();
    }

    void updateData(OLEDDisplay *display) {
        // uniquement si OPEN_WEATHER_MAP_LOCATION != ""
    if (OPEN_WEATHER_MAP_LOCATION != nullptr && OPEN_WEATHER_MAP_APP_ID != nullptr) {
        drawProgress(display, 10, "Updating time...");
        drawProgress(display, 30, "Updating weather...");
        currentWeatherClient.setMetric(IS_METRIC);
        currentWeatherClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
        currentWeatherClient.updateCurrent(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION);
        drawProgress(display, 50, "Updating forecasts...");
        forecastClient.setMetric(IS_METRIC);
        forecastClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
        uint8_t allowedHours[] = {12};
        forecastClient.setAllowedHours(allowedHours, sizeof(allowedHours));
        forecastClient.updateForecasts(forecasts, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION, MAX_FORECASTS);
        //readyForWeatherUpdate = false;
        drawProgress(display, 100, "Done...");
        delay(1000);
        }
    }

    // affichage de la température
    void display_temperature(OLEDDisplay *display, OLEDDisplayUiState* state, short int x, short int y) {

        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->setFont(ArialMT_Plain_24);
        display->drawString(64 + x, 0 + y, "Temp:");
        display->setFont(ArialMT_Plain_24);
        display->drawString(64 + x, 24 + y, String( dallas_temperature ));
 
    }

    void display_routage(OLEDDisplay *display, OLEDDisplayUiState* state, short int x, short int y) {
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->setFont(ArialMT_Plain_24);
        String info_puissance = "" + String(puissance) + "W";
        display->drawString(64 + x, 0 + y, String( puissance ));
        display->setFont(ArialMT_Plain_24);
        display->drawString(64 + x, 24 + y, String( routage ));
    }

    void init_ui() {
        ui.setTargetFPS(5);

        ui.setActiveSymbol(activeSymbole);
        ui.setInactiveSymbol(inactiveSymbole);

        // TOP, LEFT, BOTTOM, RIGHT
        ui.setIndicatorPosition(BOTTOM);

        // Defines where the first frame is located in the bar.
        ui.setIndicatorDirection(LEFT_RIGHT);

        // You can change the transition that is used
        // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_TOP, SLIDE_DOWN
        ui.setFrameAnimation(SLIDE_LEFT);

        ui.setFrames(frames, numberOfFrames);

        ui.setOverlays(overlays, numberOfOverlays);

        ui.init();

    }  


#endif  // TASK_OLED