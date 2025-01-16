#ifndef CONFIG
#define CONFIG

// oled display I2C    
#ifdef D1_MINI
#define OLED_SDA D1
#define OLED_SCL D2
#elif ESP01
#define OLED_SDA 0
#define OLED_SCL 2
#define RX 3
#endif

//#define RX 3

const char* HOSTNAME= "Sunstain-OLED";

#define VERSION "1.0"

// config mqtt
//#define MQTT_SERVER "192.168.1.22"
//#define MQTT_PORT 1883
//#define MQTT_USER "mosquitto"
//#define MQTT_PASSWORD "test-123"

// OpenWeatherMap Settings
// Sign up here to get an API key:
// https://docs.thingpulse.com/how-tos/openweathermap-key/

const boolean IS_METRIC = true;
String OPEN_WEATHER_MAP_APP_ID = "";

String OPEN_WEATHER_MAP_LOCATION = "" ;


// Pick a language code from this list:
// Arabic - ar, Bulgarian - bg, Catalan - ca, Czech - cz, German - de, Greek - el,
// English - en, Persian (Farsi) - fa, Finnish - fi, French - fr, Galician - gl,
// Croatian - hr, Hungarian - hu, Italian - it, Japanese - ja, Korean - kr,
// Latvian - la, Lithuanian - lt, Macedonian - mk, Dutch - nl, Polish - pl,
// Portuguese - pt, Romanian - ro, Russian - ru, Swedish - se, Slovak - sk,
// Slovenian - sl, Spanish - es, Turkish - tr, Ukrainian - ua, Vietnamese - vi,
// Chinese Simplified - zh_cn, Chinese Traditional - zh_tw.

String OPEN_WEATHER_MAP_LANGUAGE = "fr";
const uint8_t MAX_FORECASTS = 4;

// Adjust according to your language
const String WDAY_NAMES[] = {"DIM", "LUN", "MAR", "MER", "JEU", "VEN", "SAM"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

#endif  // CONFIG