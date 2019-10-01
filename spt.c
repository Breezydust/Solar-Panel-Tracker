/** \file spt.c
 *  \brief Main weatherstation file
 * \author Created: Thomas Aziz
 * \date 24JAN019
*/


#include <stdio.h>
#include <time.h>
#include "tsl2561.h"
#include "wxstn.h"
#include "panel.h"

/** \brief Initializes Weather panel and loops through and displays readings
 *
 * \param void
 * \return 1
 * \author Thomas Aziz
 * \date 24JAN2019
 */
 int main(void)
{
    int clux = 0;
    reading_s readings = {0};
    ldrsensor_s cldr = {0};
    panelpos_s selaz;
    int az[19] = {45,60,75,90,105,120,135,150,165,180,195,210,225,240,255,270,285,300,315};
    int el[19] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90};

    int i =0;

    StPanelInitialization();
    printf("\nWeather Station");

    while(1)
    {
        readings = WsGetReadings();
        WsDisplayReadings(readings);
        clux = tsl2561GetLux();
        tsl2561DisplayLux(clux);
        cldr = StGetLdrReadings();
        StDisplayLdrReadings(cldr);
        selaz.Elevation = el[i];
        selaz.Azimuth = az[i];
        printf("AA: %3d EA: %d\n",az[i],el[i]);
        StSetPanelPosition(selaz);
        //selaz = StGetPanelPosition();
        //printf("Azimuth: %3.0f Elevation %3.0f\n",selaz, Azimuth, selaz, Elevation);
        i++;
        if(i>18) { i=0; }
        WsDelay(5000);
    }

    tsl2561Setup();
    clux = tsl2561getLux();
    tsl2561DisplayLux(clux);

    cldr = StGetLdrReadings();
    StDisplayLdrReadings(cldr);

    return 1;
}


