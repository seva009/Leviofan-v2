//scanning networks
#include <WiFI.h>
#include <stdlib.h>
#include "scan.h"

uint8_t __found;

void initScan(uint8_t *found) {
  __found = WiFi.scanNetworks(false, true);
  *found = __found;
}

AccessPoint *scan() {
  AccessPoint *networks = (AccessPoint *)calloc(__found, sizeof(AccessPoint));
  for (int i = 0; i < __found; ++i) {
    // Print SSID and RSSI for each network found
    Serial.print(i + 1);  //Sr. No
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));  //SSID
    strcpy((char*)networks[i].ssid, WiFi.SSID(i).c_str());
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));  //Signal Strength
    Serial.print(") MAC:");
    Serial.print(WiFi.BSSIDstr(i));
    memcpy(networks[i].mac, WiFi.BSSID(i), 6);
    Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " Unsecured " : " Secured ");
    networks[i].data = WiFi.channel(i) << 4 | !(WiFi.encryptionType(i) == WIFI_AUTH_OPEN);
    Serial.println(WiFi.channel());
    networks[i].selfsz = AP_SZ - 32 + WiFi.SSID(i).length();
    delay(10);
  }
  return networks;
}