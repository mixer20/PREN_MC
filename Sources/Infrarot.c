/*
 * Infrarot.c
 *
 *  Created on: 08.11.2015
 *      Author: kilian
 */

#include "Infrarot.h"

volatile bool AD_finished;

static portTASK_FUNCTION(InfTask, pvParameters) {
	static uint16_t value;
	for(;;) {
		AD_finished = FALSE; /* reset flag */
		(void)INF_AD1_Measure(FALSE); /* AD_finished will be set to TRUE once */
		while(!AD_finished) {
			FRTOS1_vTaskDelay(5/portTICK_RATE_MS);
		}
		/* AD_finished set to TRUE by the interrupt to indicate the result is ready */
		(void)INF_AD1_GetValue16(&value); /* get the result into value variable */
		DEBUG("Distanz Distanzsensor:%i\r\n",value);
		FRTOS1_vTaskDelay(500/portTICK_RATE_MS);
	}
}

void INF_Init(void) {
	 if (FRTOS1_xTaskCreate(InfTask, "Inf", configMINIMAL_STACK_SIZE+200, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
	    for(;;){} /* error */
	 }
}




