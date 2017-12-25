
/* Salus RT505 TX
 *
 * Identifies thermostat on/off event
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *            00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41
 * [00] {478} 7f ff ff 80 fe 03 f8 0f e0 3f 80 fe 03 f8 0f e0 3f 80 fe 03 f8 0f e0 00 00 7f 01 ff f8 07 ff ff e0 3f 80 fe 00 00 00 00 1f c0
 *
 * 42 43 44 45 46 47 48 49(50 51)52 53 54 55 56 57 58 59
 * 00 00 00 00 00 00 00 00 00 7f 00 00 03 f8 00 00 1f fc
 *
 */

#include "rtl_433.h"
#include "data.h"
#include "util.h"
// #define FORD_BITLEN  479
// #define FORD_BYTELEN 5
// #define FORD_PACKETCOUNT 10

static int salus_callback(bitbuffer_t *bitbuffer){
	bitrow_t *bb = bitbuffer->bb;
        uint8_t *bytes = bitbuffer->bb[0];
	data_t *data;
	char time_str[LOCAL_TIME_BUFLEN];
    	local_time_str(0, time_str);
	int i;
	uint8_t id=0;
    	uint32_t device_id =0;
	unsigned bits = bitbuffer->bits_per_row[0];

	for (int i = 0; i < bitbuffer->num_rows; i++) {
		if (debug_output) {printf("Salus debug info ");}
		// bitbuffer_print(bitbuffer);
		    /* Reject codes of wrong length */
	//	if (bits != 66)
	//	 return 0;
		// printf(" Bits per row = %d\n", bits);
		//if ( 24 != bitbuffer->bits_per_row[0])
		 //     return 0;
		if (bits == 67) {
			 if (debug_output) {
				printf("Salus debug info 2 ");
                                bitbuffer_print(bitbuffer);
                        }
			device_id = ((bb[i][3]<<16)| (bb[i][4]<<8)|(bb[i][5]));
			uint16_t code = (bb[i][8]);
			char *state = "NULL";
			if (code == 32){
				state = "ON";
			}
			else {
				(state = "OFF");
			}
			/* Get time now */
			local_time_str(0, time_str);
			data = data_make(
        			"time",		"time",		DATA_STRING, time_str,
				"model",	"model",	DATA_STRING, "Salus RT505TX",
				"type",		"type",		DATA_STRING, "Wireless room thermostat",
				"device_id",	"device-id",	DATA_INT, device_id,
				"code", 	"data",		DATA_INT, code,
				"state",	"state",	DATA_STRING, state,
				NULL);
			data_acquired_handler(data);
			return 1;
		}
		return 0;
	}
	return 0;
}

static char *output_fields[] = {
	"time",
	"model",
	"type",
	"device_id",
	"code",
	"state",
	NULL
};

r_device salus = {
	.name		= "Salus RT505TX",
	.modulation     = FSK_PULSE_PCM,//OOK_PULSE_PWM_TERNARY, was FSK_PULSE_PCM
	.short_limit    = 420,       //  105 samples @250k = 420 us  (105/250*1000)
	.long_limit     = 420,       // NRZ encoding (bit width = pulse width)
	.reset_limit    = 33000,    // was 5000,1451 Package starts with a huge gap of ~18900 us 8162/2048000*1000000 => 4000 but that is too long, so use 800 µs
	.json_callback	= &salus_callback,
	.disabled	= 0,
	.demod_arg	= 0,
        .fields		= output_fields
};

