#include <stdlib.h>

#ifdef WIRINGPI
#include <wiringPi.h>
#include <wiringPiSPI.h>
#endif

#ifdef PIGPIO
#include "pigpio.h"
#endif

#ifdef PIGPIOIF2
#include <pigpiod_if2.h>
#endif

#include "max6675.h"


#define MAX6675_CLOCK_SPEED 4000000


///////////////////////////////////////////////////////////////////////////////
// MAX6675Setup
//
//


#if defined(WIRINGPI) || defined(PIGPIO)
MAX6675 MAX6675Setup( int spi_channel ) 
#else
MAX6675 MAX6675Setup( int pi, int spi_channel ) 
#endif	
{
    int h = 0;	
#if defined(WIRINGPI)
    if ( wiringPiSPISetup( spi_channel, MAX6675_CLOCK_SPEED ) == -1 )  {
#elif defined(PIGPIO)
    gpioInitialise();
    if ( ( h = spiOpen(  spi_channel, MAX6675_CLOCK_SPEED , 0 ) ) < 0 ) {
#elif defined(PIGPIOIF2)
    if ( ( h = spi_open( pi, spi_channel, MAX6675_CLOCK_SPEED, 0 ) ) < 0 ) {
#endif	    
        return 0;
    }

    MAX6675 max6675 = (MAX6675)malloc( sizeof( struct MAX6675) );

    max6675->m_SpiChannel = spi_channel;
    max6675->m_scale = MAX6675_CELSIUS;
    max6675->m_handle = h;

    return max6675;
}

///////////////////////////////////////////////////////////////////////////////
// MAX6675Free
//
//

void MAX6675Free( MAX6675 max6675 ) 
{

#if defined(PIGPIO)
    gpioTerminate();
#elif defined(PIGPIOIF2)

#endif

    if ( max6675 ) {
        free( max6675 );
    }
}

///////////////////////////////////////////////////////////////////////////////
// MAX6675SetScale
//
//

void MAX6675SetScale( MAX6675 max6675, MAX6675TempScale scale ) 
{
    if ( max6675 ) {
        max6675->m_scale = scale;
    }
}

///////////////////////////////////////////////////////////////////////////////
// MAX6675GetScale
//
//

MAX6675TempScale MAX6675GetScale( MAX6675 max6675 ) 
{
    if ( max6675 ) {
        return max6675->m_scale;
    }

    return MAX6675_CELSIUS;
}

///////////////////////////////////////////////////////////////////////////////
// MAX6675GetTempC
//
//

float MAX6675GetTempC( MAX6675 max6675 ) 
{
    if ( max6675 == 0 ) {
        return 0.0f;
    }

#if defined (WIRINGPI)
    unsigned char buf[2] = {0, 0};
    int ret = wiringPiSPIDataRW( max6675->m_SpiChannel, buf, 2 );
#elif defined (PIGPIO)
    char buf[2] = {0, 0};
    int ret = spiRead( max6675->m_handle, buf, 2 );
#elif defined (PIGPIOID2)
    char buf[2] = {0, 0};
    int ret = spi_read( max6675->m_pi, max65675->m_handle, buf, 2 );
#endif    

    if ( ret != 2 ) {
        return 0.0f;
    }

    short reading = (buf[0] << 8) + buf[1];
    reading >>= 3;

    return reading * 0.25;
}

///////////////////////////////////////////////////////////////////////////////
// MAX6675GetTempK
//
//

float MAX6675GetTempK( MAX6675 max6675 ) 
{
    return MAX6675GetTempC(max6675) + 273.15;
}
///////////////////////////////////////////////////////////////////////////////
// MAX6675GetTempF
//
//

float MAX6675GetTempF( MAX6675 max6675 ) 
{
    return (MAX6675GetTempC(max6675) * 1.8) + 32.0;
}

///////////////////////////////////////////////////////////////////////////////
// MAX6675GetTemp
//
//

float MAX6675GetTemp( MAX6675 max6675 ) 
{
    if ( max6675 ) {

        switch ( max6675->m_scale ) {
			
            case MAX6675_KELVIN:
                return MAX6675GetTempK( max6675 );

            case MAX6675_FAHRENHEIT:
                return MAX6675GetTempF(max6675);

            default:
                return MAX6675GetTempC(max6675);
        }

    } 
    else {
        return 0.0f;
    }
}

