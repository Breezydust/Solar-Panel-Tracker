/** \file serial.h
 *  \brief header file for serial.c functions
 * \author Created: Thomas Aziz
 * \date 24JAN019
*/

#ifndef SERIAL_H
#define SERIAL_H

#include <inttypes.h>

#ifndef PORTNAME
#define PORTNAME "/dev/ttyS0"
#endif

void serial_init(void);
void serial_config(void);
void serial_println(const char *, int);
void serial_readln(char *, int);
void serial_close(void);

#endif
