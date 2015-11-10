/*
 * Application.c
 *      Author: Erich Styger
 */
#include "Application.h"
#include "LED1.h"
#include "FRTOS1.h"
#include "Shell.h"
#include "Infrarot.h"



void APP_Run(void) {
  SHELL_Init();
  US_Init();
  INF_Init();

  FRTOS1_vTaskStartScheduler();
}

