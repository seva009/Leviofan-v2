#include <EEPROM.h>
#include "types.h"

void prepareSave();
void importAP(AccessPoint *aps, uint8_t found);
void exportAP(AccessPoint **aps, uint8_t *found);