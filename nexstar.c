#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdint.h>
#include <conio.h>          // getch()

#include "serialport.h"
#include "CSerialPort.h"    // We use this source

#define PREAMBLE  0x3b
#define MAINBOARD 0x01
#define SOURCE    0x03
#define HANDC     0x04
#define AZM       0x10
#define ALT       0x11
#define GPS       0xb0
#define POSITIVE  0x24
#define NEGATIVE  0x25

//--------------------------------------------------------
// Some test(s):

uint8_t test[16] = { 0x03, 0x04, 0x10, 0xfe };

void dump( uint8_t *buf, int len )
{
    while( len-- )
    {
        printf("%02x ", *buf++);
    }
    printf("\n");
}

//--------------------------------------------------------
// Low level message functions

struct speed_t
{
    int  run;
    PORT port;    // Serial port handle
    int  azm;
    int  alt;
} speed;


static uint8_t csum8( uint8_t *cmd, int len )
{
    uint8_t csum = 0;

    while( len-- )
    {
        csum += *cmd++;
    }
    return 0 - csum;
}


// Valid speed range is:  -9 ... +9, 0 is stop
static int check_valid_speed( int speed )
{
    if ( speed < -9 ) return -9;
    if ( speed >  9 ) return  9;
    return speed;
}


// Destination is: AZM, ALT or GPS
static int mk_AUX_msg( uint8_t *buffer, uint8_t source, uint8_t destination, uint8_t *cmd, int cmdlen )
{
    buffer[0] = PREAMBLE;
    buffer[1] = 2 + cmdlen;
    buffer[2] = source;
    buffer[3] = destination;
    memcpy( &buffer[4], cmd, cmdlen );
    buffer[ 4 + cmdlen ] = csum8( &buffer[1], 3 + cmdlen );
    return  5 + cmdlen;
}


// Axis is: AZM or ALT
int mk_AUX_speed( uint8_t *buffer, int axis, int speed )
{
    uint8_t cmd[8];

    if( speed >= 0 ) cmd[0] = POSITIVE;
    else             cmd[0] = NEGATIVE;

    speed  = check_valid_speed( speed );
    cmd[1] = abs( speed );

    return mk_AUX_msg( buffer, SOURCE, axis, cmd, 2 );
}


void set_speed( PORT com_port, uint8_t axis, int speed )
{
    uint8_t buffer[16];

    int len = mk_AUX_speed( buffer, axis, speed );

    printf("TX: ");
    dump( buffer, len );

    SendData( com_port, (char*)buffer, len );

    Sleep( 100 ); // Delay [ms] between multiple commands
}


// Test to monitor: Do we get any reply messages?
DWORD WINAPI ThreadFunc(void* data)
{
    // Do stuff.  This will be the first function called on the new thread.
    // When this function returns, the thread goes away.  See MSDN for more details.

    printf("start RX\n");
    while ( speed.run )
    {
        uint8_t databuffer[80];

        // Recive character data that have been sent throw the com port.
        int len = ReciveData( speed.port, (char*)databuffer, sizeof(databuffer) );

        if ( len > 0 ) {
            printf("RX: ");
            dump( databuffer, len );
        }
    }
    printf("exit  RX\n");  // ReciveData() is blocking, we newer arrive here
    return 0;
}

//--------------------------------------------------------

#define CtrlC     0x03
#define KEYUP     0x48
#define KEYDN     0x50
#define KEYLEFT   0x4b
#define KEYRIGHT  0x4d
#define KEYESC    0x1b


void userif( void )
{
    printf("start UI\n");

    speed.run = 1;
    speed.azm = 0;
    speed.alt = 0;

    while ( speed.run )
    {
        uint8_t ch = getch();

        if ( (ch == KEYESC) || (ch == '0') || (ch == CtrlC) )
        {
            set_speed( speed.port, ALT, 0 );
            set_speed( speed.port, AZM, 0 );
            if ( ch == CtrlC ) {
                 break;
            }
        }
        else if ( ch == 0xe0 )
        {
            ch = getch();

            if ( ch == KEYUP ) {
                speed.alt = check_valid_speed( ++speed.alt );
                set_speed( speed.port, ALT, speed.alt);
            }
            if ( ch == KEYDN ) {
                speed.alt = check_valid_speed( --speed.alt );
                set_speed( speed.port, ALT, speed.alt);
            }
            if ( ch == KEYRIGHT ) {
                speed.azm = check_valid_speed( ++speed.azm );
                set_speed( speed.port, AZM, speed.azm);
            }
            if ( ch == KEYLEFT ) {
                speed.azm = check_valid_speed( --speed.azm );
                set_speed( speed.port, AZM, speed.azm);
            }
        }
    }
    speed.run = 0;
    printf("exit  UI\n");
}

//--------------------------------------------------------

int main( int argc, char *argv[] )
{
    HANDLE thread = NULL;

    int idx  = 7;
    int rate = CP_BOUD_RATE_19200;

    #if  0
    if ( argc <= 1) {
        printf("ERROR: Require COM serial port number (example 20)\n");
        return  1;
    }
    idx = atoi( argv[1] );
    #endif
    speed.port = OpenPort( idx );

    if ( speed.port == NULL ) {
        printf("ERROR: Can not open COM port %d\n", idx);
        return 1;
    }
    if ( argc > 2 ) {
        rate = CP_BOUD_RATE_9600;
    }
    SetPortBoudRate(speed.port, rate);
    SetPortDataBits(speed.port, CP_DATA_BITS_8);
    SetPortStopBits(speed.port, CP_STOP_BITS_TWO);
    SetPortParity(  speed.port, CP_PARITY_NOPARITY);

    EscapeCommFunction( speed.port, SETRTS );  // CLRRTS / SETRTS

//  thread = CreateThread( NULL, 0, ThreadFunc, NULL, 0, NULL );
    userif();

    if ( thread ) {
        TerminateThread( thread, 0 );  // Force stop RX thread
    }
    ClosePort( speed.port );
    return 0;
}
