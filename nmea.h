/** \file nmea.h
 *  \brief header file for nmea.h
 * \author Created: Thomas Aziz
 * \date 24JAN019
*/

#ifndef NMEA_H
#define NMEA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define _EMPTY 0x00
#define NMEA_GPRMC 0x01
#define NMEA_GPRMC_STR "$GPRMC"
#define NMEA_GPGGA 0x02
#define NMEA_GPGGA_STR "$GPGGA"
#define NMEA_UNKNOWN 0x00
#define _COMPLETED 0x03

#define NMEA_CHECKSUM_ERR 0x80
#define NMEA_MESSAGE_ERR 0xC0

#define NMEAMSGSZ 82

struct gpgga
{
	// UTC Time
	double utc;
    // Latitude eg: 4124.8963 (XXYY.ZZKK.. DEG, MIN, SEC.SS)
    double latitude;
    // Latitude eg: N
    char lat;
    // Longitude eg: 08151.6838 (XXXYY.ZZKK.. DEG, MIN, SEC.SS)
    double longitude;
    // Longitude eg: W
    char lon;
    // Quality 0, 1, 2
    uint8_t quality;
    // Number of satellites: 1,2,3,4,5...
    uint8_t satellites;
    // Altitude eg: 280.2 (Meters above mean sea level)
    double altitude;
};

typedef struct gpgga gpgga_t;

struct gprmc
{
	double utc;
    double latitude;
    char lat;
    double longitude;
    char lon;
    double speed;
    double course;
	double date;
};
typedef struct gprmc gprmc_t;

struct nmeamsg
{
    char msgstr[NMEAMSGSZ+1];
};
typedef struct nmeamsg nmeamsg_s;

uint8_t nmea_get_message_type(const char *);
uint8_t nmea_valid_checksum(const char *);
void nmea_parse_gpgga(char *, gpgga_t *);
void nmea_parse_gprmc(char *, gprmc_t *);

#endif

