/** \file sptglgmain.h
 *  \brief header file for sptglgmain.c
 * \author Created: Thomas Aziz
 * \date 20MAR019
*/

#ifndef SPTGLGMAIN_H
#define SPTGLGMAIN_H
#include "GlgApi.h"

// CONSTANTS
#define UPDATE_INTERVAL 30000	// Initially 5 second update - now 30 seconds
#define STON 1
#define STOFF 0

// Function Prototypes
void Input(GlgObject viewport,GlgAnyType client_data,GlgAnyType call_data);
void UpdateControls(GlgAnyType data,GlgIntervalID * id);

#endif
