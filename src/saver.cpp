/*
How EEPROM looks like:
0: found
1: AP1_sz
2 - 2+AP1_sz: AP1
...
*/

#include "saver.h"

void prepareSave() {
    EEPROM.begin(4096);
}

void importAP(AccessPoint *aps, uint8_t found) {
    EEPROM.writeBytes(START_SAVE_ADDR+1, (uint8_t*)aps, found * AP_SZ);
    EEPROM.put(START_SAVE_ADDR, found);
    EEPROM.commit();
}

void exportAP(AccessPoint **aps, uint8_t *found) {
    uint8_t found_ap;
    found_ap = EEPROM.get(START_SAVE_ADDR, found_ap);
    void *tmp = malloc(found_ap * AP_SZ);
    *found = found_ap;
    for (int i = 0; i < found_ap; ++i) {
        EEPROM.get(i * AP_SZ + START_SAVE_ADDR + 1, *(AccessPoint*)(tmp + i * AP_SZ));
    }
    free(*aps);
    *aps = (AccessPoint*)tmp;
}