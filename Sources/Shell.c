/*
 * Shell.c
 *
 *  Created on: 04.08.2011
 *      Author: Erich Styger
 */

#include "FRTOS1.h"
#include "Shell.h"
#include "CLS1.h"
#include "LED1.h"
#include "SERVO1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "XF1.h"
#include "Application.h"

static const CLS1_ParseCommandCallback CmdParserTable[] =
{
  CLS1_ParseCommand,
#if LED1_PARSE_COMMAND_ENABLED
  LED1_ParseCommand,
#endif
#if SERVO1_PARSE_COMMAND_ENABLED
  SERVO1_ParseCommand,
#endif
#if FRTOS1_PARSE_COMMAND_ENABLED
  FRTOS1_ParseCommand,
#endif
  NULL /* sentinel */
};

static void MyPutChar(void *arg, char c) {
  CLS1_StdIO_OutErr_FctType fct = arg;
  
  fct(c);
}

unsigned int sendHost(const char *fmt, ...) {
  va_list args;
  unsigned int count;

  va_start(args,fmt);
  count = XF1_xvformat(MyPutChar, CLS1_GetStdio()->stdOut, fmt, args);
  va_end(args);
  return count;
}

unsigned int DEBUG(const char *fmt, ...) {
#if DEBUG_MODE
  va_list args;
  unsigned int count;

  va_start(args,fmt);
  count = XF1_xvformat(MyPutChar, CLS1_GetStdio()->stdOut, fmt, args);
  va_end(args);
  return count;
#endif
}

static portTASK_FUNCTION(ShellTask, pvParameters) {
  unsigned char buf[48];

  (void)pvParameters; /* not used */
  buf[0] = '\0';
  (void)CLS1_ParseWithCommandTable((unsigned char*)CLS1_CMD_HELP, CLS1_GetStdio(), CmdParserTable);
  for(;;) {
    (void)CLS1_ReadAndParseWithCommandTable(buf, sizeof(buf), CLS1_GetStdio(), CmdParserTable);
    FRTOS1_vTaskDelay(50/portTICK_RATE_MS);
  }
}

void SHELL_Init(void) {
  if (FRTOS1_xTaskCreate(ShellTask, "Shell", configMINIMAL_STACK_SIZE+200, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
    for(;;){} /* error */
  }
}

