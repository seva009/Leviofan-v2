#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define AP_SZ sizeof(AccessPoint)

struct AccessPoint {
  uint8_t selfsz;
  uint8_t data; // channel << 4 | secured 
  uint8_t mac[6];
  uint8_t ssid[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // kak mnoga budet' pamyati xavat' v eeprom...
} __attribute__((packed));

#endif