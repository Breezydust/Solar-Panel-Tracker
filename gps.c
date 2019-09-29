/** \file gps.c
 *  \brief GPS initialisation and settings
 * \author Created: Thomas Aziz
 * \date 24JAN019
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gps.h"
#include "nmea.h"
#include "serial.h"

nmeamsg_s gpsbuffer[GPSBUFFERSZ] = {0};

/** \brief Initialize GPS
 *
 * \param void
 * \return void
 * \author Thomas Aziz
 * \date 03APR2019
 */
extern void gps_init(void)
{
#if SIMGPS
    StReadGPSData("gpslog01.txt");
	//StReadGPSData("gpslog02.txt");
#else
	//serial_init();
	//serial_config();
#endif
}

/** \brief Turn on GPS
 *
 * \param void
 * \return void
 * \author Thomas Aziz
 * \date 03APR2019
 */
extern void gps_on(void)
{
    //Write on
}

/** \brief Compute the GPS location using decimal scale
 *
 * \param void
 * \return loc_t structure
 * \author Thomas Aziz
 * \date 03APR2019
 */
loc_t gps_location(void)
{
    uint8_t status = _EMPTY;
    loc_t cloc = { 0.0 };
    gpgga_t gpgga;
    gprmc_t gprmc;
    int i = 0;
    char buffer[GPSDATASZ];

    while(status != _COMPLETED)
	{
#if SIMGPS
        strncpy(buffer,gpsbuffer[i].msgstr,NMEAMSGSZ+1);
        i++;
        if(i > GPSBUFFERSZ) { break;}
#else
        serial_readln(buffer,GPSDATASZ);
#endif
        switch (nmea_get_message_type(buffer))
		{
            case NMEA_GPGGA:
                nmea_parse_gpgga(buffer, &gpgga);
                cloc.utc = gpgga.utc;
                gps_convert_deg_to_dec(&(gpgga.latitude), gpgga.lat, &(gpgga.longitude), gpgga.lon);
                cloc.latitude = gpgga.latitude;
                cloc.longitude = gpgga.longitude;
                cloc.altitude = gpgga.altitude;
                status |= NMEA_GPGGA;
                break;
            case NMEA_GPRMC:
                nmea_parse_gprmc(buffer, &gprmc);
                cloc.speed = gprmc.speed;
                cloc.course = gprmc.course;
                cloc.date = gprmc.date;
                status |= NMEA_GPRMC;
                break;
        }
    }
    return cloc;
}


/** \brief Turn off GPS
 *
 * \param void
 * \return void
 * \author Thomas Aziz
 * \date 03APR2019
 */
extern void gps_off(void)
{
    //Write off
//    serial_close();
}

// Convert latitude and longitude to decimal from degree
void gps_convert_deg_to_dec(double *latitude, char ns,  double *longitude, char we)
{
    double lat = (ns == 'N') ? *latitude : -1 * (*latitude);
    double lon = (we == 'E') ? *longitude : -1 * (*longitude);

    *latitude = gps_deg_dec(lat);
    *longitude = gps_deg_dec(lon);
}

double gps_deg_dec(double deg_point)
{
    double ddeg;
    double sec = modf(deg_point, &ddeg)*60;
    int deg = (int)(ddeg/100);
    int min = (int)(deg_point-(deg*100));

    double absmlat = round(min * 1000000.);
    double absslat = round(sec * 1000000.);
    double absdlat = round(deg * 1000000.);

    return round(absdlat + (absmlat/60) + (absslat/3600)) /1000000;
}

/** \brief Open file in read mode, read in GPSBUFFERSZ strings into gpsbuffer
 *
 * \param string: filename
 * \return int 1
 * \author Thomas Aziz
 * \date 03APR2019
 */
int StReadGPSData(char * fname)
{
    FILE * fp;
    int i;
    fp = fopen(fname,"r");
    if(fp == NULL) { return 0; }
    for(i = 0;i < GPSBUFFERSZ; i++)
    {
        fgets(gpsbuffer[i].msgstr,NMEAMSGSZ,fp);
    }
    fclose(fp);
    return 1;
}

