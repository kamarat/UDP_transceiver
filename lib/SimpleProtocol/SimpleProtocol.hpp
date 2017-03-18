#ifndef SIMPLE_PROTOCOL_H
#define SIMPLE_PROTOCOL_H

#include <stdint.h>

#include <SPI.h>            // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>

// Types of message
typedef enum TypePacket {
  TYPE_HELLO = 0x00,
  TYPE_POST_DATA,
  TYPE_ACK,
  //TYPE_IDENTIFY,
  //TYPE_REQUEST,
  //TYPE_POST_CONFIG,
  //TYPE_CONFIG
} TypePacket_t;

typedef struct {
  TypePacket type;
  uint16_t sequence;
  uint8_t data;
} Packet_t;

const uint8_t PACKET_SIZE = sizeof( Packet_t );  // size of packet
const uint8_t PACKET_TYPE = 0;      // index of type in buffer
const uint8_t PACKET_SEQUENCE = 1;  // index of sequence in buffer
const uint8_t PACKET_DATA = 3;      // index of data in buffer
const uint8_t BUFFER_SIZE = 10;     // size of buffer

/*
typedef struct {
  IPAddress ipAddress;
  uint8_t port;
} SocketAddr_t;

typedef struct {
  Packet_t packet[ BUFFER_SIZE ];
  SocketAddr_t socket[ BUFFER_SIZE ];
  uint8_t currentIndex;
  uint8_t currentAmount;
} Buffer_t;

typedef struct {
  Packet_t packet[ BUFFER_SIZE ];
  uint8_t currentIndex;
  uint8_t currentAmount;
} sendBuffer_t;
*/

typedef enum Ack {
  ACK_RECEIVED = 0,
  ACK_NO_RECEIVED,
  ACK_TIMEOUT
} Ack_t;

class SimpleProtocol {
private:
  Packet_t packet_;
  //sendBuffer_t sendBuffer_;
  //Buffer_t receiveBuffer_;
  void sendPacket( EthernetUDP *udp, Packet_t packet );
  void sendPacket( EthernetUDP *udp, Packet_t packet, IPAddress ip, uint16_t port );

public:
  SimpleProtocol();
  ~SimpleProtocol();

  void sendHelloPacket( EthernetUDP *udp );
  void sendAckPacket( EthernetUDP *udp, Packet_t packet );
  uint8_t sendPostDataPacket( EthernetUDP *udp, uint8_t data, uint16_t seq, IPAddress ip, uint16_t port );
  uint8_t sendPostDataPacketBuffer( EthernetUDP *udp, uint8_t data, IPAddress ip, uint16_t port );
  void receivePacket( EthernetUDP *udp, uint8_t * buffer, Packet_t * packet, uint8_t size);
};

#endif  // #ifndef PROTOCOL_H
