#include <EEPROM.h>
#include "types.h"

#define START_SAVE_ADDR 65

void prepareSave();
void importAP(AccessPoint *aps, uint8_t found);
void exportAP(AccessPoint **aps, uint8_t *found);