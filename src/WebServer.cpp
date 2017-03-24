#include "WebServer.h"

const char legalChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890/.-_~";
unsigned int requestNumber = 0;

unsigned long connectTime[ MAX_SOCK_NUM] ;  // in w5100.h is #define MAX_SOCK_NUM 4

/*******************************************************************************
 *    Function: freeRam
 * Description: There are three areas in RAM:
 *              - static data, i.e. global variables and arrays … and strings !
 *              - the “heap”, which gets used if you call malloc() and free()
 *              - the “stack”, which is what gets consumed as one function calls another
 *
 *              The heap grows up, and is used in a fairly unpredictable manner.
 *              If you release areas, then they will lead to unused gaps in the heap,
 *              which get re-used by new calls to malloc() if the requested block fits in those gaps.
 *
 *              At any point in time, there is a highest point in RAM occupied by the heap.
 *              This value can be found in a system variable called __brkval.
 *
 *              The stack is located at the end of RAM, and expands and contracts down
 *              towards the heap area. Stack space gets allocated and released as needed
 *              by functions calling other functions. That’s where local variables get stored.
 *   Parameter: none
 *      Return: int - free ram in bytes
 ******************************************************************************/
int freeRam()
{
  extern int __heap_start,*__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int) __brkval);
}

void webServer( EthernetServer * s )
{
  // Listen for incoming clients
  EthernetClient client = s->available();
  if (client) {
    #if DEBUG >= 1
      Serial.println( F( "New client." ));
    #endif

    // An http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        #if DEBUG >= 1
          Serial.write(c);
        #endif

        /* If you've gotten to the end of the line (received a newline
         * character) and the line is blank, the http request has ended,
         * so you can send a reply
         */
        if ( c == '\n' && currentLineIsBlank ) {
          // Send a standard http response header
          client.println( "HTTP/1.1 200 OK" );
          client.println( "Content-Type: text/html" );
          client.println( "Connection: close" );  // the connection will be closed after completion of the response
          client.println( "Refresh: 5" );         // refresh the page automatically every 5 sec
          client.println();
          client.println( "<!DOCTYPE HTML>" );
          client.println( "<html>" );

          // Body
          client.print( "<h2>UDP transciever base on Arduino.</h2>" );
          client.print( "<h3>State of inputs:</h3>" );
          for ( int i = 0; i < 6; i++ ) {;
            client.print( "Input " );
            client.print( i + 1 );
            client.print(" is ");
            client.println(( PINC >> i ) & 0x01 );
            client.print( "<br>" );
          }

          client.print( "<h3>Free memory in bytes: </h3>" );
          //client.print( "<br>" );
          client.println( freeRam() );
          client.println( "</html>" );
          break;
        }
        if (c == '\n') {
          // You're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // You've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // Give the web browser time to receive the data
    delay(1);
    // Close the connection:
    client.stop();

    #if DEBUG >= 1
      Serial.println( F( "Client disconnected." ));
    #endif
  }
}
