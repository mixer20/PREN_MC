/*
 * Ultraschall.c
 *
 *  Created on: 03.11.2015
 *      Author: kilian
 */

#include "Ultraschall.h"
#include "FRTOS1.h"
#include "ULTRA_TU2.h"
#include "Shell.h"
#include "WAIT1.h"
#include "ULTRA_TRIG.h"
#include <stdlib.h>

static void Measure(void);

static US_DeviceType usDevice; /* device handle for the ultrasonic device */

static portTASK_FUNCTION(USTask, pvParameters) {
	usDevice.state = ECHO_IDLE;
	usDevice.capture = 0;
	usDevice.trigDevice = ULTRA_TRIG_Init(NULL);
	usDevice.echoDevice = ULTRA_TU2_Init(&usDevice);

	for(;;) {
	    Measure();
	    WAIT1_Waitms(50); /* wait at least for 50 ms until the next measurement to avoid echos */
	    FRTOS1_vTaskDelay(500/portTICK_RATE_MS);
	}
}

void US_Init(void) {
  if (FRTOS1_xTaskCreate(USTask, "US", configMINIMAL_STACK_SIZE+200, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
    for(;;){} /* error */
  }
}

static uint16_t calcAirspeed_dms(uint8_t temperatureCelsius) {
  /* Return the airspeed depending on the temperature, in deci-meter per second */
  unsigned int airspeed; /* decimeters per second */

  airspeed = 3313 + (6 * temperatureCelsius); /* dry air, 0% humidity, see http://en.wikipedia.org/wiki/Speed_of_sound */
  airspeed -= (airspeed/100)*15; /* factor in ~15% for a relative humidity of ~40% */
  return airspeed;
}

uint16_t US_usToCentimeters(uint16_t microseconds, uint8_t temperatureCelsius) {
  return (microseconds*100UL)/calcAirspeed_dms(temperatureCelsius)/2; /* 2 because of two way */
}

uint16_t US_Measure_us(void) {
  uint16_t us;

  /* send 10us pulse on TRIG line. */
  ULTRA_TRIG_SetVal(usDevice.trigDevice);
  WAIT1_Waitus(10);
  usDevice.state = ECHO_TRIGGERED;
  ULTRA_TRIG_ClrVal(usDevice.trigDevice);
  while(usDevice.state!=ECHO_FINISHED) {
    /* measure echo pulse */
    if (usDevice.state==ECHO_OVERFLOW) { /* measurement took too long? */
      usDevice.state = ECHO_IDLE;
      return 0; /* no echo, error case */
    }
  }
  us = (usDevice.capture*1000UL)/(ULTRA_TU2_CNT_INP_FREQ_U_0/1000);
  return us;
}

static void Measure(void) {
  uint16_t us, cm;

  us = US_Measure_us();
  cm = US_usToCentimeters(us, 22);

  DEBUG("Distanz:\t%i\r\n",cm);

}

void US_EventEchoOverflow(LDD_TUserData *UserDataPtr) {
  US_DeviceType *ptr = (US_DeviceType*)UserDataPtr;

  ptr->state = ECHO_OVERFLOW;
}

void US_EventEchoCapture(LDD_TUserData *UserDataPtr) {
	US_DeviceType *ptr = (US_DeviceType*)UserDataPtr;

	if (ptr->state==ECHO_TRIGGERED) { /* 1st edge, this is the raising edge, start measurement */
	  ULTRA_TU2_ResetCounter(ptr->echoDevice);
	  ptr->state = ECHO_MEASURE;
	} else if (ptr->state==ECHO_MEASURE) { /* 2nd edge, this is the falling edge: use measurement */
	  (void)ULTRA_TU2_GetCaptureValue(ptr->echoDevice, 0, &ptr->capture);
	  ptr->state = ECHO_FINISHED;
	}
}








