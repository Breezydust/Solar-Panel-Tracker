/** \file wxstn.c
 *  \brief weatherstation functions
 * \author Created: Thomas Aziz
 * \date 24JAN019
*/

//
// Weather Station Control
//
//
#include <stdlib.h>
#include <wiringPi.h>
#include <pcf8591.h>
#include "wxstn.h"
#include "hshbme280.h"

/** \brief Calls a setup function for BME280
 *
 * \param void
 * \return 1
 * \author Thomas Aziz
 * \date 24JAN2019
 */
int WsInit(void)
{
	BME280Setup();

	return 1;
}

/** \brief Displays the readings from weatherstation
 *
 * \param structure reading_s dreads
 * \return 1
 * \author Thomas Aziz
 * \date 24JAN2019
 */
void WsDisplayReadings(reading_s dreads)
{
    time_t now;

    now = time(NULL);
    printf("\n%s",ctime(&now));
    printf("Readings\t T: %3.1lfC\t H: %3.0lf%%\tP: %5.1lfmb\t WS: %3.0lfkmh\t WD: %3.0lfdegrees\n",
    dreads.temperature,dreads.humidity,dreads.pressure,dreads.windspeed,dreads.winddirection);
}



reading_s WsGetReadings(void)
{
	reading_s now = {0};

	now.rtime = time(NULL);
	now.temperature = WsGetTemperature();
	now.humidity = WsGetHumidity();
	now.pressure = WsGetPressure();
	now.light = WsGetLight();
	now.windspeed = WsGetWindspeed();
	now.winddirection = WsGetWinddirection();
	return now;
}

/** \brief Get current temperature from weather sensor
 *
 * \param void
 * \return double
 * \author Thomas Aziz
 * \date 24JAN2019
 */
double WsGetTemperature(void)
{
#if SIMTEMP
	return (double) WsGetRandom(USTEMP-LSTEMP)+LSTEMP;
#else
	return GetBME280TempC();
#endif
}

/** \brief Get current humidity from weather sensor
 *
 * \param void
 * \return double
 * \author Thomas Aziz
 * \date 24JAN2019
 */
double WsGetHumidity(void)
{
#if SIMHUMID
	return (double) WsGetRandom(USHUMID-LSHUMID)+LSHUMID;
#else
	return GetBME280Humidity();
#endif
}

/** \brief Get current pressure from weather sensor
 *
 * \param void
 * \return double
 * \author Thomas Aziz
 * \date 24JAN2019
 */
double WsGetPressure(void)
{
#if SIMPRESS
	return (double) WsGetRandom(USPRESS-LSPRESS)+LSPRESS;
#else
	return PaTomB(GetBME280Pressure());
#endif
}

/** \brief Get current light from weather sensor
 *
 * \param void
 * \return double
 * \author Thomas Aziz
 * \date 24JAN2019
 */
double WsGetLight(void)
{
#if SIMLIGHT
	return (double) WsGetRandom(USLIGHT-LSLIGHT)+LSLIGHT;
#else
	return 0.0;
#endif
}

/** \brief Get current windspeed from weather sensor
 *
 * \param void
 * \return double
 * \author Thomas Aziz
 * \date 24JAN2019
 */
double WsGetWindspeed(void)
{
#if SIMWINDSPD
    return (double) WsGetRandom(USWINDSPD-LSWINDSPD)+LSWINDSPD;
#else
	return 20.0;
#endif
}

/** \brief Get current wind direction from weather sensor
 *
 * \param void
 * \return double
 * \author Thomas Aziz
 * \date 24JAN2019
 */
double WsGetWinddirection(void)
{
#if SIMWINDDIR
    return (double) WsGetRandom(USWINDDIR-LSWINDDIR)+LSWINDDIR;
#else
	return 320.0;
#endif
}

/** \brief Get random value for weather station simulations
 *
 * \param int - range of numbers for random
 * \return int - random number
 * \author Thomas Aziz
 * \date 24JAN2019
 */
int WsGetRandom(int range)
{
	return rand() % range;
}

/** \brief Controls delay for weatherstation
 *
 * \param int - delay time in milliseconds
 * \return void
 * \author Thomas Aziz
 * \date 24JAN2019
 */
void WsDelay(int milliseconds)
{
    long wait;
    clock_t now,start;

    wait = milliseconds*(CLOCKS_PER_SEC/1000);
    start = clock();
    now = start;

    while((now-start) < wait)
    {
        now = clock();
    }
}

/** \brief converts value to percent
 *
 * \param double value
 * \return double percent value
 * \author Thomas Aziz
 * \date 24JAN2019
 */
double PaTomB(double pa)
{
	return pa/100.0;
}

