#ifndef NTP_FUNCTIONS
#define NTP_FUNCTIONS

#include <Arduino.h>
#include <WiFiUdp.h>

struct tm timeinfo;
#define NTP_SERVER "europe.pool.ntp.org"

/// @brief /////// init du NTP
void ntpinit() {
  // Configurer le serveur NTP et le fuseau horaire
  // Voir Time-Zone: https://sites.google.com/a/usapiens.com/opnode/time-zones
  configTzTime("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", NTP_SERVER);
  getLocalTime( &timeinfo );
  Serial.println(asctime(&timeinfo));
}


#endif // NTP_FUNCTIONS