#ifndef __ATK_H__
#define __ATK_H__
#include "types.h"
void sendDeauthPacket(AccessPoint* data, uint8_t target[6]) ;
void spammerPreparer();
void beaconSpammer(bool *isRunning);
void arpAttack(uint8_t *gateway);
void sendCtsPacket(AccessPoint* data, uint8_t mtarget[6]);
void freeSpammerMemory();
void send_cts_frame(const uint8_t ch, uint16_t duration);

uint8_t connect_wifi(const char* ssid, const char* password);
void sendArpPacket(ipv4_t *src_ip, mac_address_t *src_mac, ipv4_t *dest_ip, mac_address_t *dest_mac);
#endif