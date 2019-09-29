#include <stdio.h>
#include <inttypes.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "tsl2561.h"

/** \file tsl2561.c
 *  \brief Luminosity sensor functions
 * \author Created: Thomas Aziz
 * \date 24JAN019
*/

static int TSL2561fd;

/** \brief Setup TSL2561 sensor
 *
 * \param void
 * \return void
 * \author Thomas Aziz
 * \date 24JAN2019
 */
void tsl2561Setup(void)
{
    TSL2561fd = wiringPiI2CSetup(TSL2561_ADDR_FLOAT);
}

/** \brief get luminosity value from TSL2561 sensor
 *
 * \param void
 * \return int - luminosity value
 * \author Thomas Aziz
 * \date 24JAN2019
 */
int tsl2561GetLux(void)
{
    uint16_t visible_and_ir = 0;

    //Enable device
    wiringPiI2CWriteReg8(TSL2561fd,TSL2561_COMMAND_BIT,TSL2561_CONTROL_POWERON);

    //Set timing (101ms)
    wiringPiI2CWriteReg8(TSL2561fd,TSL2561_REGISTER_TIMING,TSL2561_GAIN_AUTO);
    delay(LUXDELAY);

    //reads visible +IR diode
    visible_and_ir = wiringPiI2CReadReg16(TSL2561fd,TSL2561_REGISTER_CHAN0_LOW);

    //disable device
    wiringPiI2CWriteReg8(TSL2561fd,TSL2561_COMMAND_BIT,TSL2561_CONTROL_POWEROFF);


    return visible_and_ir*2;
}

void tsl2561DisplayLux(int dlux)
{
        fprintf(stdout,"Light Intensity: %6d Lux\n",dlux);
}

