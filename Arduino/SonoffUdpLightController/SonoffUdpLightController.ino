/*
  Arduino code to control the SONOFF module via UDP
*/

#include "WiFi.h"
#include <WiFiUdp.h>


#define SSID "YOUR_SSID"
#define PSK "YOUR_PSK"
#undef SERIALON

unsigned int localPort = 8899;  // local port to listen on

// For three zones. Add more zones with on and off
unsigned int Zone1On = 1; 
unsigned int Zone2On = 2; 
unsigned int Zone3On = 3; 
unsigned int Zone1Off = 4; 
unsigned int Zone2Off = 5; 
unsigned int Zone3Off = 6; 

unsigned int PRESSED = LOW; // Ground pin when button pressed
unsigned int Changed = 1; // If one of the buttons has been pressed
char packetBuffer[] = "*0000000000";  // buffer to hold outgoing packet, we currently only use 4 chars but a few extra won't hurt
IPAddress Broadcast( 192,168,0,255 );  // UDP Broadcast address
WiFiUDP Udp; // Create the UDP object

void setup() 
{
// Set the GPIO pins as inputs with pullup so the buttons ground the input
pinMode( 1, INPUT_PULLUP ); 
pinMode( 2, INPUT_PULLUP ); 
pinMode( 3, INPUT_PULLUP ); 
pinMode( 4, INPUT_PULLUP ); 
pinMode( 5, INPUT_PULLUP ); 
pinMode( 6, INPUT_PULLUP ); 

#ifdef SERIALON
    Serial.begin( 115200 ); 
    delay( 1000 );
    Serial.println( "Started.." );
#endif

  // Start a WiFi connection
  WiFi.begin( SSID, PSK );
  while ( WiFi.status() != WL_CONNECTED ) 
  {
    delay( 100 );
  }

#ifdef SERIALON
    Serial.println( "WiFi Connected" );
#endif

  // Start a UDP session
  Udp.begin( localPort );
}

// Now we loop looking at the six input GPIO pins for a pressed button
// If found we set the appropriate character in the buffer and set the Changed
// Flag to indicate that the buffer should be sent
// Note we are only interested in if the button has been pressed so we don't need to bother
// with de-bounce. The worst that will happen is that it will send more packets
void loop() 
{

  if ( digitalRead( Zone1On ) == PRESSED )
  {
    packetBuffer[ 1 ] = '1';
    Changed = 1;
  }

  if ( digitalRead( Zone2On ) == PRESSED )
  {
    packetBuffer[ 2 ] = '1';
    Changed = 1;
  }

  if ( digitalRead( Zone3On ) == PRESSED )
  {
    packetBuffer[ 3 ] = '1';
    Changed = 1;
  }

  if ( digitalRead( Zone1Off ) == PRESSED )
  {
    packetBuffer[ 1 ] = '0';
    Changed = 1;
  }
  
  if ( digitalRead( Zone2Off ) == PRESSED )
  {
    packetBuffer[ 2 ] = '0';
    Changed = 1;
  }

  if ( digitalRead( Zone3Off ) == PRESSED )
  {
    packetBuffer[ 3 ] = '0';
    Changed = 1;
  }

#ifdef SERIALON
   Serial.println( Changed );
#endif

  // The Changed flag indicated that a button has been pressed 
  if ( Changed == 1 )
  {
    // Send the buffer and reset the changed flag
    Udp.beginPacket( Broadcast, 8888) ;
    Udp.write( packetBuffer, 5 );
    Udp.endPacket();
    Changed = 0;
    delay( 500 ); // A small delay to prevent UDP flooding while the button is pressed
  }

}
