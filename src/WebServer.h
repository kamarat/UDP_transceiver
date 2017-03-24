#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <utility/w5100.h>
#include <utility/socket.h>

#define DEBUG 1

extern void webServer( EthernetServer * s );

#endif  // #define WEB_SERVER_H
