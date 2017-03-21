/*== UDP transciever ==
 * ================================
 *
 * UDP_transciever.ino
 *
 * Code for transmit state of pin from a one Arduino to another.
 *
 * @author: mr.nobody
 * @date:   January 2017 - March 2017
 *
 * @version: [0.1.0-alfa] - 2017-03-18
 *
 * mr.nobody (cleft) 2017
 *
 */

/*== PINS CONNECTION ==
 *=====================
 *
 * ARDUINO --- PERIFERY
 *
 * Arduino --- Ethernet shield
 * D  4 - SS SD card reader
 * D 10 - SS
 * D 11 - MOSI
 * D 12 - MISO
 * D 13 - SCK
 *
 * A  0 - Input (pin change interrupt)
 * A  1 - Input (pin change interrupt)
 * A  2 - Input (pin change interrupt)
 * A  3 - Input (pin change interrupt)
 * A  4 - Input (pin change interrupt)
 * A  5 - Input (pin change interrupt)
 *
 * D  2 - Output
 * D  3 - Output
 * D  5 - Output
 * D  6 - Output
 * D  7 - Output
 * D  8 - Output
 *
 */

#include <SPI.h>            // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>

#include "SimpleProtocol.hpp"
#include "CircularBuffer.hpp"
#include "WebServer.h"

#define DEBUG 1
#define MODUL 0

// MAC address, IP address and local port to listen on
#if MODUL == 0
  uint8_t MAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  const IPAddress IP_LOCAL( 172, 16, 1, 100 );
  const uint16_t LOCAL_PORT = 1234;
  const IPAddress REMOTE_IP( 172, 16, 1, 101 );
  const uint16_t REMOTE_PORT = 1234;
#elif MODUL == 1
  uint8_t MAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
  const IPAddress IP_LOCAL( 172, 16, 1, 101 );
  const uint16_t LOCAL_PORT = 1234;
  const IPAddress REMOTE_IP( 172, 16, 1, 100 );
  const uint16_t REMOTE_PORT = 1234;
#endif

const IPAddress GATEWAY( 172,16,1,100 );
const IPAddress SUBNET( 255,255,255,0 );

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
IPAddress ipRemote;

EthernetServer Server(80);

const uint8_t INPUT_PINS[] = { A0, A1, A2, A3, A4, A5 };
const uint8_t INPUT_PINS_MASK = 0x3F;
const uint8_t INPUT_PINS_PORTC[] = { PORTC0, PORTC1, PORTC2, PORTC3, PORTC4, PORTC5 };
const uint8_t INPUT_PINS_PINC[] = { PINC0, PINC1, PINC2, PINC3, PINC4, PINC5 };
const uint8_t INPUT_PINS_PCMSK1[] = { PCINT8, PCINT9, PCINT10, PCINT11, PCINT12, PCINT13 };
const uint8_t OUTPUT_PINS[] = { 2, 3, 5, 6, 7, 8 };
const uint8_t SD_CARD_SS = 4;
volatile uint8_t pinChangeFlag = 0;

SimpleProtocol protocol;
Packet_t packet;
Packet_t receivePacket;
uint8_t buffer[ sizeof( packet )];
uint8_t receiveBuffer[ sizeof( packet )];   // buffer to hold incoming packet
Ack_t ackFlag = ACK_RECEIVED;
uint8_t data;
uint16_t sequence;

const uint8_t CIRCULAR_BUFFER_SIZE = 10;
CircularBuffer< Packet_t, CIRCULAR_BUFFER_SIZE > sendBuffer;

//const uint16_t DELIVERY_TIMEOUT = 10000;  // time for delivery confirmation in ms
const uint16_t DELIVERY_DELAY = 2000;        // time delay between attempting on delivery
const uint8_t DELIVERY_ATTEMPT = 20;        // number of attempt on delivery
uint16_t currentDeliveryTime;
uint8_t currentDeliveryAttempt = 0;

/*== DECLARATION OF FUNCTION ==
 *=============================
 */
void pinChangeIrqEnable();
void analysePacket( void );
void pinActivate( uint8_t * data );

void setup()
{
  #if DEBUG >= 1
    Serial.begin( 57600 );
    Serial.println( F( "Start initialization !"));
  #endif

  // Start the Ethernet UDP and Web Server
  Ethernet.begin( MAC, IP_LOCAL );
  Udp.begin( LOCAL_PORT );
  Server.begin();

  for ( uint8_t pin = 0; pin < sizeof(INPUT_PINS); pin++ )
    pinMode( INPUT_PINS[ pin ], INPUT_PULLUP);
  pinChangeIrqEnable();

  for ( uint8_t pin = 0; pin < sizeof(OUTPUT_PINS); pin++ ) {
    pinMode( OUTPUT_PINS[ pin ], OUTPUT);
    digitalWrite( OUTPUT_PINS[ pin ], HIGH );
  }

  // Pin D4 is using the Ethernet shield SD card reader like slave select pin
  pinMode( SD_CARD_SS, OUTPUT );
  digitalWrite( SD_CARD_SS, HIGH );

  #if DEBUG >= 1
    Serial.println( F( "End initialization !" ));
  #endif
}

void loop() {
  if ( pinChangeFlag ) {
    packet.data = PINC;
    packet.type = TYPE_POST_DATA;
    packet.sequence = (uint16_t)( millis() >> 1 );
    sendBuffer.push( packet );
    pinChangeFlag = 0;
  }

  if ( sendBuffer.isNonEmpty() ) {
    if ( ackFlag == ACK_RECEIVED ) {
      protocol.sendPacket( &Udp, sendBuffer.read(), REMOTE_IP, REMOTE_PORT );
      ackFlag = ACK_NO_RECEIVED;
      currentDeliveryTime = (uint16_t)millis();
      currentDeliveryAttempt++;
    }

    if (( ackFlag == ACK_NO_RECEIVED ) && (( ( uint16_t )millis() - currentDeliveryTime ) >= DELIVERY_DELAY )) {
      #if DEBUG >= 1
        Serial.println( F( "ACK packet no received !!!!!!!!!!" ));
        Serial.println();
      #endif

      protocol.sendPacket( &Udp, sendBuffer.read(), REMOTE_IP, REMOTE_PORT );
      ackFlag = ACK_NO_RECEIVED;
      currentDeliveryTime = (uint16_t)millis();
      currentDeliveryAttempt++;
      if ( currentDeliveryAttempt >= DELIVERY_ATTEMPT )
        ackFlag = ACK_TIMEOUT;

      #if DEBUG >= 1
        Serial.print( currentDeliveryAttempt );
        Serial.println( F( ". attempt for delivery packet !!!!!!!!!!!!!!!!" ));
      #endif
    }

    if ( ackFlag == ACK_TIMEOUT ) {
      // send request packet
    }
  }

  // If there's data available, read a packet
  uint8_t sizeReceivePacket = Udp.parsePacket();
  if ( sizeReceivePacket == sizeof( receiveBuffer )) {

    #if DEBUG >= 1
      IPAddress ipRemote = Udp.remoteIP();
      Serial.print( F( "Received packet of size " ));
      Serial.print( sizeReceivePacket );
      Serial.print( F( " from " ));
      for ( uint8_t i = 0; i < 4; i++ ) {
        Serial.print( ipRemote[ i ], DEC );
        if ( i < 3)
          Serial.print( F( "." ));
      }
      Serial.print( F( ", port " ));
      Serial.println( Udp.remotePort() );
    #endif

    if ( REMOTE_IP == Udp.remoteIP() )
      protocol.receivePacket( &Udp, receiveBuffer, &receivePacket, sizeReceivePacket );

    #if DEBUG >= 1
      Serial.println( F( "<<<<<<<<<< Received packet:" ));
      Serial.print( F( "type: " ));
      Serial.println( receivePacket.type, HEX );
      Serial.print( F( "sequence: " ));
      Serial.println( receivePacket.sequence );
      Serial.print( F( "data: " ));
      Serial.println( receivePacket.data, BIN );
      Serial.println();
    #endif

    analysePacket();
  }
  delay(10);

  webServer( &Server );
}

ISR( PCINT1_vect )
{
  if ( pinChangeFlag == 0 )
    pinChangeFlag = 1;
}

void analysePacket( void )
{
  switch ( receivePacket.type) {
    case TYPE_HELLO: {
      //add code
    } break;

    case TYPE_POST_DATA: {
      pinActivate( &receivePacket.data );
      protocol.sendAckPacket( &Udp, receivePacket );

    } break;

    case TYPE_ACK: {
      if ( sendBuffer.isNonEmpty() ) {
        packet = sendBuffer.read();
        if ( receivePacket.sequence == packet.sequence ) {
          sendBuffer.pop();
          ackFlag = ACK_RECEIVED;
          currentDeliveryAttempt = 0;
          #if DEBUG >= 1
            Serial.println( F( "********** Received ACK packet." ));
            Serial.println();
          #endif
        }
      }
    } break;
  }
}

void pinActivate( uint8_t * data )
{
  for ( uint8_t i = 0; i < ( sizeof( OUTPUT_PINS ) / sizeof( OUTPUT_PINS[ 0 ])); i++ ) {
    digitalWrite( OUTPUT_PINS[ i ], ( *data >> i ) & 0x01 );
  }
}

void pinChangeIrqEnable()
{
  // Set interrupt for pins A0 - A5 (PCINT[13:8])
  PCMSK1 = 0;
  for ( uint8_t i = 0; i < sizeof(INPUT_PINS_PCMSK1); i++ ) {
    PCMSK1 |= 1 << INPUT_PINS_PCMSK1[ i ];
  }
  PCIFR |= 1 << PCIF1;  // clear any outstanding interrupts
  PCICR = 1 << PCIE1;   // enable pin change interrupts for PCINT[15:8]
}
