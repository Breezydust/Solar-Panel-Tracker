/** \file sptglgmain.c
 *  \brief functions responsible for building the HMI
 * \author Created: Thomas Aziz
 * \date 20MAR019
*/

//  This example demonstrates how to use GLG control widgets as they are,
//  without emdedding them into another GLG drawing, and handle user
//  interaction.
//
//  The drawing name is GhGUI.g.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GlgApi.h>
#include "sptglgmain.h"
#include "panel.h"
#include "wxstn.h"
#include "tsl2561.h"


// Top level global variables
GlgAppContext AppContext;
GlgObject SptDrawing;
GlgLong UpdateInterval = UPDATE_INTERVAL;
int TrackOn = STOFF;

// Defines a platform-specific program entry point
#include "GlgMain.h"


/** \brief Initialise the HMI drawings and interface
 *
 * \param int/char* argument count and arguments, interface context
 * \return int context
 * \author Thomas Aziz
 * \date 20MAR2019
 */
int GlgMain(int argc,char *argv[],GlgAppContext InitAppContext )
{
	// Initialization Section
	AppContext = GlgInit( False, InitAppContext, argc, argv );
	GlgSetDResource(AppContext,"$config/GlgOpenGLMode",0.); // Disable openGL

    // Initialize the Solar Panel Tracker
	StPanelInitialization();

	// Load a drawing from the file.
	SptDrawing = GlgLoadWidgetFromFile( "SPTrack.g" );

	if(!SptDrawing) { exit( 1 ); }

	// Setting widget dimensions using world coordinates [-1000;1000].
	GlgSetGResource(SptDrawing,"Point1",-600.,-600.,0.);
	GlgSetGResource(SptDrawing,"Point2",600.,600.,0.);

	// Setting the window name (title).
	GlgSetSResource(SptDrawing,"ScreenName","Thomas Aziz's Solar Panel Tracker" );

	// Add Input callback to handle user interraction in the GLG control.
	GlgAddCallback(SptDrawing,GLG_INPUT_CB,(GlgCallbackProc)Input,NULL );

	// Paint the drawing.
	GlgInitialDraw(SptDrawing);
	GlgAddTimeOut(AppContext,UpdateInterval,(GlgTimerProc)UpdateControls,NULL );
	return (int) GlgMainLoop( AppContext );
}

/** \brief Updates HMI after user input is registered
 *
 * \param The input the user interacted with, the client information, and the entered data
 * \return void
 * \author Thomas Aziz
 * \date 20MAR2019
 */
void Input(GlgObject GhDrawing,GlgAnyType client_data,GlgAnyType call_data)
{
	GlgObject message_obj;
	char * format,* action,* origin, * full_origin;
	double pstate,svalue;
	message_obj = (GlgObject) call_data;

	// Get the message's format, action and origin.
	GlgGetSResource(message_obj,"Format", &format);
	GlgGetSResource(message_obj,"Action",&action);
	GlgGetSResource(message_obj,"Origin",&origin);
	GlgGetSResource(message_obj,"FullOrigin",&full_origin);

	// Handle window closing. May use GhDrawing's name.
	if(strcmp(format,"Window") == 0 &&
		strcmp(action,"DeleteWindow") == 0)
	{ exit(0); }

    // Retrieve OnState value from Track1
    if( strcmp(format,"Button") == 0 && strcmp(action,"ValueChanged") == 0)
    {
        GlgGetDResource(SptDrawing,"Track1/OnState", &svalue );
        if(svalue == 1.0)
        {
           TrackOn = 1;
            GlgSetDResource(SptDrawing,"Elevation1/DisableInput",1);
            GlgSetDResource(SptDrawing,"Azimuth1/DisableInput",1);
            GlgSetDResource(SptDrawing,"LED1/Value",0.0);
            GlgSetDResource(SptDrawing,"LED1/DisableInput",STON);
        }
        else
        {
            TrackOn = 0;
            GlgSetDResource(SptDrawing,"Elevation1/DisableInput",0);
            GlgSetDResource(SptDrawing,"Azimuth1/DisableInput",0);
            GlgSetDResource(SptDrawing,"LED1/DisableInput",STOFF);
        }
    }
}

/** \brief Updates controls after data is changed
 *
 * \param changed data
 * \return void
 * \author Thomas Aziz
 * \date 20MAR2019
 */
void UpdateControls(GlgAnyType data,GlgIntervalID * id)
{
	reading_s rnow;
    int clux = {0};
    ldrsensor_s cldr = {0};
    panelpos_s selaz;
    double avalue = {0.0};
    double evalue = {0.0};
    double LEDvalue = {0.0};
    paneldata_s cpdata = {0,0};
    double latval = {0.0};
    double lngval = {0.0};

    rnow = WsGetReadings();
    clux = tsl2561GetLux();

    if(TrackOn)
    {
#if SIMTRACK
        evalue = (double) (rand() % 90);
        selaz.Elevation = (double) 180.0 - evalue;
        selaz.Azimuth = (double) (rand() % 180);
#else
        selaz = StTrackSun();
        avalue = selaz.Azimuth-90.0;
        evalue = selaz.Elevation;
#endif

    }
    else
    {
        GlgGetDResource(SptDrawing,"Elevation1/Value", &evalue );
        GlgGetDResource(SptDrawing,"Azimuth1/Value", &avalue );
        GlgGetDResource(SptDrawing,"LED1/Value", &LEDvalue);
        selaz.Elevation = evalue+90.0;
        selaz.Azimuth = avalue;
        selaz.gpsdata.latitude = latval;
        selaz.gpsdata.longitude = lngval;
        StSetPanelPosition(selaz);
        StSetCalibrationLED((int)LEDvalue);
    }
    cpdata.azimuth = avalue;
    cpdata.elevation = evalue;
    cpdata.latitude = selaz.gpsdata.latitude;
    cpdata.longitude = selaz.gpsdata.longitude;
    rnow.light = clux;
    StLogPanelData(cpdata,rnow);

	GlgSetDResource(SptDrawing,"Temp1/Value",rnow.temperature);
	GlgSetDResource(SptDrawing,"Humid1/Value",rnow.humidity);
	GlgSetDResource(SptDrawing,"Press1/Value",rnow.pressure);
	GlgSetDResource(SptDrawing,"Windspeed1/Value",rnow.windspeed);
	GlgSetDResource(SptDrawing,"WindDirection1/Value",rnow.winddirection);
	GlgSetDResource(SptDrawing,"Luminosity1/Value",clux);
	GlgSetDResource(SptDrawing,"Elevation1/Value",evalue);
	GlgSetDResource(SptDrawing,"Azimuth1/Value",avalue);
	GlgSetDResource(SptDrawing,"Latitude1/Value",selaz.gpsdata.latitude);
	GlgSetDResource(SptDrawing,"Longitude1/Value",selaz.gpsdata.longitude);
	GlgUpdate(SptDrawing);
	GlgAddTimeOut(AppContext,UpdateInterval,(GlgTimerProc)UpdateControls,NULL );
}
