/*
 * Shell.h
 *
 *  Created on: 23.05.2011
 *      Author: Erich Styger
 */

#ifndef SHELL_H_
#define SHELL_H_

/*! \brief Serial driver initialization */
void SHELL_Init(void);
unsigned int sendHost(const char *fmt, ...);
unsigned int DEBUG(const char *fmt, ...);

#endif /* SHELL_H_ */
