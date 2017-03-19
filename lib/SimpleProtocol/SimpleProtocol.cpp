#include "SimpleProtocol.hpp"

/*******************************************************************************
 *    Function: SimpleProtocol
 * Description: constructor
 *   Parameter: none
 *      Return: none
 ******************************************************************************/
SimpleProtocol::SimpleProtocol(){}

/*******************************************************************************
 *    Function: ~SimpleProtocol
 * Description: destructor
 *   Parameter: none
 *      Return: none
 ******************************************************************************/
SimpleProtocol::~SimpleProtocol(){}

/*******************************************************************************
 *    Function: sendHelloPacket
 * Description: sendHelloPacket
 *   Parameter: [IN] EthernetUDP *udp - udp instance
 *      Return: none
 ******************************************************************************/
/*void SimpleProtocol::sendHelloPacket( EthernetUDP *udp )
{
  Packet_t packet;
  packet.type = TYPE_HELLO;
  packet.sequence = 0;
  packet.data = 0;
  sendPacket( udp, packet );
}*/

/*******************************************************************************
 *    Function: sendAckPacket
 * Description: sendAckPacket
 *   Parameter: [IN] EthernetUDP *udp - udp instance
 *              [IN] uint8_t buffer - receive packet
 *      Return: none
 ******************************************************************************/
void SimpleProtocol::sendAckPacket( EthernetUDP *udp, Packet_t packet )
{
  packet.type = TYPE_ACK;
  sendPacket( udp, packet );
}

/*******************************************************************************
 *    Function: sendPostDataPacket
 * Description: sendPostDataPacket
 *   Parameter: [IN] EthernetUDP *udp - udp instance
 *              [IN] uint8_t data - post data
 *              [IN] uint16_t seq - time sequence
 *              [IN] IPAddress ip - remote IP address
 *              [IN] uint8_t port - remote port
 *      Return: none
 ******************************************************************************/
uint8_t SimpleProtocol::sendPostDataPacket( EthernetUDP *udp, uint8_t data, uint16_t seq, IPAddress ip, uint16_t port )
{
  packet_.type = TYPE_POST_DATA;
  packet_.sequence = seq;
  packet_.data = data;

  //#if DEBUG >= 1
    Serial.println( F( "Sent packet:" ));
    Serial.print( F( "type: " ));
    Serial.println( packet_.type, HEX );
    Serial.print( F( "sequence: " ));
    Serial.println( packet_.sequence );
    Serial.print( F( "data: " ));
    Serial.println( packet_.data, BIN );
  //#endif

  sendPacket( udp, packet_, ip, port );
  return 0;
}

/*******************************************************************************
 *    Function: sendPostDataPacketBuffer
 * Description: sendPostDataPacketBuffer
 *   Parameter: [IN] EthernetUDP *udp - udp instance
 *              [IN] uint8_t data - post data
 *              [IN] IPAddress ip - remote IP address
 *              [IN] uint8_t port - remote port
 *      Return: 0 - all OK
 *              1 - full buffer
 ******************************************************************************/
/*uint8_t SimpleProtocol::sendPostDataPacketBuffer( EthernetUDP *udp, uint8_t data, IPAddress ip, uint16_t port )
{
  if ( sendBuffer_.currentAmount == BUFFER_SIZE ) {
    return 1;
  }
  else if ( sendBuffer_.currentIndex == ( BUFFER_SIZE - 1 )) {
    sendBuffer_.currentIndex = 0xFF;
  }
  sendBuffer_.currentIndex++;
  sendBuffer_.currentAmount++;
  sendBuffer_.packet[ sendBuffer_.currentIndex ].type = TYPE_POST_DATA;
  sendBuffer_.packet[ sendBuffer_.currentIndex ].sequence = (uint16_t)millis();
  sendBuffer_.packet[ sendBuffer_.currentIndex ].data = data;
  sendPacket( udp, sendBuffer_.packet[ sendBuffer_.currentIndex ] );
  return 0;
}*/

/*******************************************************************************
 *    Function: sendPacket
 * Description: sendPacket
 *   Parameter: [IN] EthernetUDP *udp - udp instance
 *              [IN] Packet_t packet - post packet
 *              [IN] IPAddress ip - remote IP address
 *              [IN] uint8_t port - remote port
 *      Return: none
 ******************************************************************************/
void SimpleProtocol::sendPacket( EthernetUDP *udp, Packet_t packet, IPAddress ip, uint16_t port )
{
  uint8_t buffer[ sizeof( packet )] = {0};
  memcpy( buffer, &packet, sizeof( buffer ));

  udp->beginPacket( ip, port );
  udp->write( buffer, sizeof( buffer ));
  udp->endPacket();

  //#if DEBUG >= 1
    Serial.println( F( "Sent packet:" ));
    Serial.print( F( "type: " ));
    Serial.println( packet_.type, HEX );
    Serial.print( F( "sequence: " ));
    Serial.println( packet_.sequence );
    Serial.print( F( "data: " ));
    Serial.println( packet_.data, BIN );
  //#endif
}
/*******************************************************************************
 *    Function: sendPacket
 * Description: sendPacket
 *   Parameter: [IN] EthernetUDP udp - *udp instance
 *              [IN] Packet_t packet - post packet
 *      Return: none
 ******************************************************************************/
void SimpleProtocol::sendPacket( EthernetUDP *udp, Packet_t packet )
{
  uint8_t buffer[ sizeof( packet )] = {0};
  memcpy( buffer, &packet, sizeof( buffer ));

  udp->beginPacket( udp->remoteIP(), udp->remotePort() );
  udp->write( buffer, sizeof( buffer ));
  udp->endPacket();
}

/*******************************************************************************
 *    Function: receivePacket
 * Description: receive packet
 *   Parameter: [IN] EthernetUDP *udp - udp instance
 *              [IN] uint8_t *buffer - pointer to recieve buffer
 *              [OUT] Packet_t * packet - pointer to struct received packet
 *              [IN] uint8_t size - size of receive data
 *      Return: none
 ******************************************************************************/
void SimpleProtocol::receivePacket( EthernetUDP *udp, uint8_t * buffer, Packet_t * packet, uint8_t size)
{
  //IPAddress ipRemote = udp.remoteIP();
  udp->read( buffer, size );
  memcpy( packet, buffer, size );
}
