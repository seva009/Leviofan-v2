#ifndef __ATK_H__
#define __ATK_H__
#include "types.h"
void sendDeauthPacket(AccessPoint* data, uint8_t target[6]) ;
void spammerPreparer();
void beaconSpammer(bool *isRunning);
void arpAttack(uint8_t *gateway);
// void skibidiSpammer(bool *isRunning);
// void skibidiPreparer();
#endif