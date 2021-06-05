#include <System/serialPrintf.h>
#include "../ds18b20/scan_devices.h"
#include "stdio.h"


/**
 * \brief           Scan for 1-Wire devices on specific 1-Wire port
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_ids: pointer to array of \ref ow_rom_t structures
 * \param[in]       rtf: Number of roms to find
 * \param[out]      rf: Number of roms found after scan
 */
owr_t
scan_onewire_devices(ow_t* ow, ow_rom_t* rom_ids, size_t rtf, size_t* rf) {
    owr_t res;
    size_t found;
    
    /* Search for devices and save it to array */
    res = ow_search_devices(ow, rom_ids, rtf, &found);

    if (found > 0) {
        /* Print all devices */
        for (size_t i = 0; i < found; i++) {
            tx_printf("Device ROM addr: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",
                rom_ids[i].rom[0], rom_ids[i].rom[1], rom_ids[i].rom[2], rom_ids[i].rom[3],
                rom_ids[i].rom[4], rom_ids[i].rom[5], rom_ids[i].rom[6], rom_ids[i].rom[7]
            );
        }
    }
    if (rf != NULL) {
    	*rf = found;
    }
    if (res == owERRNODEV && found) {
    	return owOK;
    }
    return res;
}
