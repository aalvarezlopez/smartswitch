/** @file */

#ifndef BufferFiller_h
#define BufferFiller_h

#include "EtherCard.h"


/** This class populates network send and receive buffers.
*
*   This class provides formatted printing into memory. Users can use it to write into send buffers.
*
*   Nota: PGM_P: is a pointer to a string in program space (defined in the source code, updated to PROGMEM)
*
*   # Format string
*
*   | Format | Parameter   | Output
*   |--------|-------------|----------
*   | $D     | uint16_t    | Decimal representation
*   | $T ¤   | double      | Decimal representation with 3 digits after decimal sign ([-]d.ddd)
*   | $H     | uint16_t    | Hexadecimal value of lsb (from 00 to ff)
*   | $L     | long        | Decimal representation
*   | $S     | const char* | Copy null terminated string from main memory
*   | $F     | PGM_P       | Copy null terminated string from program space
*   | $E     | byte*       | Copy null terminated string from EEPROM space
*   | $$     | _none_      | '$'
*
*   ¤ _Available only if FLOATEMIT is defined_
*
*   # Examples
*   ~~~~~~~~~~~~~{.c}
*     uint16_t ddd = 123;
*     double ttt = 1.23;
*     uint16_t hhh = 0xa4;
*     long lll = 123456789;
*     char * sss;
*     char fff[] PROGMEM = "MyMemory";
*
*     sss[0] = 'G';
*     sss[1] = 'P';
*     sss[2] = 'L';
*     sss[3] = 0;
*     buf.emit_p( PSTR("ddd=$D\n"), ddd );  // "ddd=123\n"
*     buf.emit_p( PSTR("ttt=$T\n"), ttt );  // "ttt=1.23\n" **TO CHECK**
*     buf.emit_p( PSTR("hhh=$H\n"), hhh );  // "hhh=a4\n"
*     buf.emit_p( PSTR("lll=$L\n"), lll );  // "lll=123456789\n"
*     buf.emit_p( PSTR("sss=$S\n"), sss );  // "sss=GPL\n"
*     buf.emit_p( PSTR("fff=$F\n"), fff );  // "fff=MyMemory\n"
*   ~~~~~~~~~~~~~
*
*/
#endif
