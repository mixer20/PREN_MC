/*
 * Ultraschall.h
 *
 *  Created on: 03.11.2015
 *      Author: kilian
 */

#ifndef SOURCES_ULTRASCHALL_H_
#define SOURCES_ULTRASCHALL_H_

#include "ULTRA_TRIG.h"
#include "ULTRA_TU2.h"

void US_EventEchoOverflow(LDD_TUserData *UserDataPtr);
void US_EventEchoCapture(LDD_TUserData *UserDataPtr);

typedef enum {
  ECHO_IDLE, /* device not used */
  ECHO_TRIGGERED, /* started trigger pulse */
  ECHO_MEASURE, /* measuring echo pulse */
  ECHO_OVERFLOW, /* measurement took too long */
  ECHO_FINISHED /* measurement finished */
} US_EchoState;

typedef struct {
  LDD_TDeviceData *trigDevice; /* device handle for the Trigger pin */
  LDD_TDeviceData *echoDevice; /* input capture device handle (echo pin) */
  volatile US_EchoState state; /* state */
  ULTRA_TU2_TValueType capture; /* input capture value */
} US_DeviceType;



#endif /* SOURCES_ULTRASCHALL_H_ */
