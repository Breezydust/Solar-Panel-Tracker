/** \file panel.c
 *  \brief Weather panel initialisation
 * \author Created: Thomas Aziz
 * \date 24JAN019
*/

#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <pcf8591.h>
#include "spa.h"
#include "tsl2561.h"
#include "wxstn.h"
#include "panel.h"
#include "hshbme280.h"


positiondata_s positiontable[STMAXTBLSZ];


/** \brief Initialise the weather panel
 *
 * \param void
 * \return int 1
 * \author Thomas Aziz
 * \date 31JAN2019
 */
int StPanelInitialization(void)
{
    int status,i;
    StServoSetup();
    tsl2561Setup();
    StStepperSetup();
    pcf8591Setup(ST_PCF8591_PINBASE, ST_PCF8591_I2CADR);
    WsInit();
	gps_init();

	status = StRetrievePositionTable();
	if(status == STNOTABLE)
	{
        // Set up stepper motor azimuth part
        for(i=0; i<STMAXSTEPSZ;i++)
        {
            positiontable[i].apos = (int) ((double)(360-i)/STMAXAZDEG*(STPA360-STPA000))+STPA000;
            positiontable[i].cnt = (int) ((double)(360-i)/STMAXAZDEG*(STSTEPMAX-STSTEPMIN))+STSTEPMIN;
        }

        // Set up servomotor elevation part
        for(i=0; i<STMAXSERVSZ;i++)
        {
            positiontable[i].epos = (int) ((double)i/STMAXELDEG*(STPE090-STPE000))+STPE000;
            positiontable[i].pwm = (int) ((double)i/STMAXELDEG*(STPWMMAX-STPWMMIN))+STPWMMIN;
        }
        for(i=STMAXSERVSZ; i<STMAXSTEPSZ;i++)
        {
            positiontable[i].epos = (int) STPE090;
            positiontable[i].pwm = (int) STPWMMAX;
        }
        StSavePositionTable();
}

    return 1;
}


/** \brief Initialise the servomotor
 *
 * \param void
 * \return void
 * \author Thomas Aziz
 * \date 31JAN2019
 */
void StServoSetup(void)
{
    //Hardware PWM for elevation
    pinMode(STELPIN,PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(STPWMCLOCK);
    pwmSetRange(STPWMRANGE);
}

/** \brief Initialise the stepper motor
 *
 * \param void
 * \return void
 * \author Thomas Aziz
 * \date 14MAR2019
 */
void StStepperSetup(void)
{
    pinMode(STAZSTEP,OUTPUT);
    pinMode(STAZDIR,OUTPUT);
}


/** \brief Get panel information using analog feedback data
 *
 * \param void
 * \return panelpos_s structure
 * \author Thomas Aziz
 * \date 07FEB2019
 */
panelpos_s StGetPanelPosition(void)
{
    panelpos_s cpos = {0.0};
    double el,az;


    az=analogRead(ST_PCF8591_PINBASE+0);//-STPA000;
    el=analogRead(ST_PCF8591_PINBASE+1);//-STPE000;
    cpos.Elevation = STMAXELDEG * (el-STPE000)/(STPE090-STPE000);
    cpos.Azimuth = 360.0 - ((STMAXAZDEG-STMINAZDEG) * az / (STPA360-STPA000));

    return cpos;
}

/** \brief Set the position of the physical solar panel
 *
 * \param panelpos_s structure newpos
 * \return void
 * \author Thomas Aziz
 * \date 07FEB2019
 */
void StSetPanelPosition(panelpos_s newpos)
{
    panelpos_s cpos = StGetPanelPosition();
    int pwmnel, pwmnaz, stepnaz, stepcaz, dir, i;

    //pwmnel = (int) (newpos.Elevation / STSERVORANGE  * (STPWMMAX-STPWMMIN)) + STPWMMIN;
    //pwmnaz = (int) (newpos.Azimuth / STSTEPRANGE * (STPWMMAX-STPWMMIN))+STPWMMIN;

	pwmnel = positiontable[(int)newpos.Elevation].pwm;
	pwmnaz = positiontable[(int)newpos.Azimuth].cnt;

    pwmWrite(STELPIN,pwmnel);
    delay(STPWMDELAY);

    stepnaz = (int)((STMAXAZ - newpos.Azimuth)/STSTEPRANGE*(STSTEPMAX-STSTEPMIN))+STSTEPMIN;
    stepcaz = (int)((STMAXAZ - cpos.Azimuth)/STSTEPRANGE*(STSTEPMAX-STSTEPMIN))+STSTEPMIN;

    dir = ((stepnaz-stepcaz)<0) ? LOW : HIGH;

    digitalWrite(STAZDIR,dir);

    int diff = abs(stepnaz-stepcaz);

    for(i=0;i<diff;i++){
        digitalWrite(STAZSTEP,HIGH);
        delay(STSTEPDELAY);
        digitalWrite(STAZSTEP,LOW);
    }
}


/** \brief Read luminosity sensor data
 *
 * \param void
 * \return structure ldrsensor_s with readings
 * \author Thomas Aziz
 * \date 07FEB2019
 */
ldrsensor_s StGetLdrReadings(void)
{
    ldrsensor_s csens = {0};

#if SIMLDR
    csens.aset = STSACTR;
    csens.eset = STSECTR;
#else
    csens.aset = analogRead(ST_PCF8591_PINBASE + 2);
    csens.eset = analogRead(ST_PCF8591_PINBASE + 3);
#endif // SIMLDR

    return csens;
}


/** \brief Display readings from luminosity sensor
 *
 * \param struct ldrsensor_s with luminosity readings
 * \return void
 * \author Thomas Aziz
 * \date 07FEB2019
 */
void StDisplayLdrReadings(ldrsensor_s dsens)
{
    printf("Elevation LDR: %d Azimuth LDR: %d", dsens.eset, dsens.aset);
}

/** \brief Read position data from file into struct
 *
 * \param  void
 * \return int - status check
 * \author Thomas Aziz
 * \date 14FEB2019
 */
int StRetrievePositionTable(void)
{
	FILE *fp;
	int i,deg;
	char dg;

	//preprocessor conditions to check whether to use .txt or .dat
#if STPOSTBLTXT == 1
	fp = fopen("position.txt","r");

	if(fp == NULL)
	{
        fprintf(stdout,"Unable to open file");
		return 0; //return 0 if unable to open the file
	}
	for(i=0;i<STMAXTBLSZ;i++)
	{
        fscanf(fp,"%c%3d,%3d,%4d,%3d,",&dg,&deg,&positiontable[i].apos,&positiontable[i].cnt,&positiontable[i].epos,&positiontable[i].pwm); //read data from file into variables (dg/deg, struct positiontable)
	}
	fclose(fp);

#else
    fp = fopen("position.dat","rb");

    if(fp == NULL)
	{
        fprintf(stdout,"Unable to open file");
		return 0; //return 0 if unable to open the file
	}
	for(i=0;i<STMAXTBLSZ;i++)
	{
        fread(&positiontable[i],sizeof(positiontable),1,fp);
	}
	fclose(fp);
#endif

	return 1; //return one if success
}

/** \brief save position data into file
 *
 * \param  void
 * \return int - status check
 * \author Thomas Aziz
 * \date 14FEB2019
 */
int StSavePositionTable(void){
	FILE *fp;
	int i,deg;
	char dg = '$';

	//preprocessor conditions to check whether to use .txt or .dat
#if STPOSTBLTXT == 1
	fp = fopen("position.txt","w");

    if(fp == NULL)
	{
        fprintf(stdout,"Unable to open file");
		return 0; //return 0 if unable to open file
	}
    for(i=0;i<STMAXTBLSZ;i++)
	{
        fprintf(fp,"%c%3d,%3d,%4d,%3d ",dg,i,positiontable[i].apos,positiontable[i].cnt,positiontable[i].epos,positiontable[i].pwm);
    }
	fclose(fp); //close file

#else
	fp = fopen("position.dat","w");

    if(fp == NULL)
	{
        fprintf(stdout,"Unable to open file");
		return 0; //return 0 if unable to open file
	}

	fwrite(&positiontable,sizeof(positiontable),1,fp); //write positiontable to the file
	fclose(fp); //close file
#endif


	return 1;
}

/** \brief Use spa functions to return a calculated panel position
 *
 * \param void
 * \return structure panelpos_s with calculated values
 * \author Thomas Aziz
 * \date 27MAR2019
 */
panelpos_s StCalculateNewPanelPosition(void)
{
    panelpos_s newpos = {0.0};
    spa_data csp  = {0.0};
    time_t rawtime;
    struct tm *ct;
    loc_t gpspos = {0};

    // Get time in a suitable format for assigning to a spa_data structure
    time(&rawtime);
    ct = localtime(&rawtime);
	gpspos = gps_location();

    // Set up spa_data structure for calculation
    csp.year            = ct->tm_year+1900;
    csp.month           = ct->tm_mon+1;
    csp.day             = ct->tm_mday;
    csp.hour            = ct->tm_hour;
    csp.minute          = ct->tm_min;
    csp.second          = ct->tm_sec;
    csp.timezone        = DTIMEZONE;
    csp.delta_ut1       = DELTAUT1;
    csp.delta_t         = DELTAT;
    if(gpspos.latitude != 0 && gpspos.longitude != 0)
    {
        csp.longitude  = gpspos.longitude;
        csp.latitude   = gpspos.latitude;
        csp.elevation  = gpspos.altitude;
        newpos.gpsdata = gpspos;
    }
    else
    {
        csp.longitude  = DLONGITUDE;
        csp.latitude   = DLATITUDE;
        csp.elevation  = PELEVATION;
        newpos.gpsdata.latitude = csp.latitude;
        newpos.gpsdata.longitude = csp.longitude;
    }
    csp.pressure        = WsGetPressure();
    csp.temperature     = WsGetTemperature();
    csp.slope           = DSLOPE;
    csp.azm_rotation    = DAZROT;
    csp.atmos_refract   = DATMREF;
    csp.function        = SPA_ALL;


    spa_calculate(&csp);
    newpos.Azimuth = csp.azimuth;
    newpos.Elevation = 90.0 - csp.incidence;

    return newpos;
}

/** \brief Uses calculated panel position to set the panel position, then adjusts panel position using LDR sensor data
 *
 * \param void
 * \return structure panelpos_s with position data
 * \author Thomas Aziz
 * \date 27MAR2019
 */
panelpos_s StTrackSun(void)
{
    panelpos_s tpos = {0.0};
    ldrsensor_s spos = {0};
    int i;

    // Move the panel to the calculated position
    tpos = StCalculateNewPanelPosition();
    tpos.Elevation = tpos.Elevation + 90.0;
    if(tpos.Elevation <= 0.0)
    {
        tpos.Azimuth = PAZIMUTH;
        tpos.Elevation = PELEVATION;
        StSetPanelPosition(tpos);
        return tpos;
    }
    else
    {
        StSetPanelPosition(tpos);
    }
    // Adjust panel postion using LDR data
    for(i=0; i<10; i++)
    {
        spos = StGetLdrReadings();
        if(spos.eset < STSECTR - 10) {tpos.Elevation++;}
        if(spos.eset > STSECTR + 10) {tpos.Elevation--;}
        if(spos.aset < STSACTR - 10) {tpos.Azimuth--;}
        if(spos.aset > STSACTR + 10) {tpos.Azimuth++;}
        StSetPanelPosition(tpos);
    }
    return tpos;
}

/** \brief Set LED intensity based on user input
 *
 * \param cval
 * \return void
 * \author Thomas Aziz
 * \date 04APR2019
 */
void StSetCalibrationLED(unsigned short cval)
{
    if(cval >= STLEDMIN && cval <= STLEDMAX)
    {
        analogWrite(ST_PCF8591_PINBASE+0,cval);
    }
}

/** \brief Log sensor data in JSON format
 *
 * \param pdata struct, creads struct
 * \return int
 * \author Thomas Aziz
 * \date 14APR2019
 */
int StLogPanelData(paneldata_s pdata, reading_s creads)
{
    FILE *fp;
    char datetime[25];
    
    fp = fopen("paneldata.csv","a");
    if(fp == NULL){return 0;};
    
    strcpy(datetime,ctime(&creads.rtime));
    datetime[3] = ',';
    datetime[7] = ',';
    datetime[10] = ',';
    datetime[19] = ',';

    fprintf(fp,"%.24s,%3.0lf,%3.0lf,%lf,%lf,%3.1lf,%3.1lf,%5.1lf,%3.0lf\n",
            datetime,pdata.azimuth,pdata.elevation,pdata.latitude,pdata.longitude,
            creads.temperature,creads.humidity,creads.pressure,creads.light);
    
    fclose(fp);
    return 1;
}
