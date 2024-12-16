#ifndef CONFIG
#define CONFIG

// oled display I2C    
#define OLED_SDA D1
#define OLED_SCL D2

const char* HOSTNAME= "Sunstain-OLED";

#define VERSION "1.0"

// config mqtt
#define MQTT_SERVER "192.168.1.22"
#define MQTT_PORT 1883
#define MQTT_USER "mosquitto"
#define MQTT_PASSWORD "test-123"

#endif  // CONFIG