/*
  Arduino code to control the SONOFF module via UDP
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define SSID "YOUR_SSID"
#define PSK "YOUR_PSK"
#define SERIAL

unsigned int localPort = 8888;  // local port to listen on
unsigned int LED = 13; // Led on board
unsigned int RELAY = 12; // Power relay
unsigned int RELAY_ON = HIGH; // Energise the relay
unsigned int RELAY_OFF = LOW; // De-energise the relay
unsigned int LED_OFF = HIGH; // Turn the LED on
unsigned int LED_ON = LOW; // Turn the LED off

// Set this unique to each Sonoff module
unsigned int MODULE = 1; // Each Sonoff has a different module number


// buffers for receiving and sending data
char packetBuffer[ UDP_TX_PACKET_MAX_SIZE + 1 ];  // buffer to hold incoming packet,
char replyBuffer[] = "ack\r\n";        // Notify sender that the packet was received OK

WiFiUDP Udp;

void setup() 
{
  pinMode( 13, OUTPUT ); 
  pinMode( 12, OUTPUT ); 
  digitalWrite( LED, LED_ON ); // Turn the status LED on while initialising
  digitalWrite( RELAY, RELAY_OFF ); // Make sure that tho relay is off at the start
  WiFi.mode( WIFI_STA );
#ifdef SERIAL
    Serial.begin( 115200 ); 
    delay( 5000 );
    Serial.println( "Started.." );
#endif
  WiFi.begin( SSID, PSK );
  while ( WiFi.status() != WL_CONNECTED ) 
  {
    delay( 500 );
  }

#ifdef SERIAL
    Serial.println( "WiFi Connected" );
#endif

  Udp.begin( localPort );
  digitalWrite( LED, LED_OFF ); // Now turn the LED off. No point wasting power
}

void loop() 
{
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if ( packetSize > 1 ) 
  {
    // read the packet into packetBufffer
    int n = Udp.read( packetBuffer, UDP_TX_PACKET_MAX_SIZE );
    packetBuffer[ n ] = 0; // Terminate buffer
#ifdef SERIAL
    Serial.println(  packetBuffer  ); // Let's see what was sent  
#endif
    if (  packetBuffer[ 0 ] == '*' ) // Valid message with states
    {
      if (  packetBuffer[  MODULE  ] == '1' ) // Indicates turn on
      {
          digitalWrite( LED, LED_ON ); 
          digitalWrite( RELAY, RELAY_ON );  
      }
      if (  packetBuffer[  MODULE  ] == '0' ) // Indicates turn off
      {
          digitalWrite( LED, LED_OFF );
          digitalWrite( RELAY, RELAY_OFF ); 
      }

    // Not strictly needed by the other end but it doesn't hurt
      Udp.beginPacket( Udp.remoteIP(), Udp.remotePort() );
      Udp.write( replyBuffer );
      Udp.endPacket();
    }
  }
}
