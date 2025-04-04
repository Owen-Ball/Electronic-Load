#ifndef _SYMBOLS_
#define _SYMBOLS_

//Font library doesn't have Omega so it is loaded into a sprite as an image and displayed
const uint16_t ohmWidth = 28;
const uint16_t ohmHeight = 36;

//mAh drawn out with individual pixels since there is not enough room to write out "mAh" normally
const uint16_t mahWidth = 28;
const uint16_t mahHeight = 36;

PROGMEM const unsigned char omega[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x03, 0x00, 
  0x80, 0xFF, 0x1F, 0x00, 0xC0, 0xFF, 0x3F, 0x00, 0xE0, 0xFF, 0x3F, 0x00, 
  0xF0, 0xFF, 0xFF, 0x00, 0xF0, 0x07, 0xFE, 0x00, 0xFC, 0x01, 0xF8, 0x03, 
  0xFC, 0x00, 0xF0, 0x03, 0x7C, 0x00, 0xE0, 0x07, 0x7E, 0x00, 0xE0, 0x03, 
  0x7E, 0x00, 0xC0, 0x07, 0x3E, 0x00, 0xC0, 0x07, 0x3E, 0x00, 0xC0, 0x07, 
  0x3E, 0x00, 0xC0, 0x07, 0x3E, 0x00, 0xC0, 0x07, 0x3E, 0x00, 0xC0, 0x07, 
  0x3E, 0x00, 0xC0, 0x07, 0x3C, 0x00, 0xC0, 0x03, 0x7C, 0x00, 0xC0, 0x03, 
  0x7C, 0x00, 0xE0, 0x03, 0x78, 0x00, 0xE0, 0x01, 0xF0, 0x00, 0xF0, 0x00, 
  0xF0, 0x01, 0xF8, 0x00, 0xE0, 0x07, 0x7E, 0x00, 0xE0, 0x0F, 0x3F, 0x00, 
  0xFF, 0x9F, 0xFF, 0x07, 0xFF, 0x9F, 0xFF, 0x07, 0xFE, 0x0F, 0xFF, 0x07, 
  0xFF, 0x9F, 0xFF, 0x07, 0xFF, 0x9F, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  };


PROGMEM const unsigned char mah[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x18, 0x00, 0x00, 0x00, 0xF8, 0x0F, 0x00, 0x00, 0xF8, 0x1F, 0x00, 0x00, 
  0x98, 0x19, 0x00, 0x00, 0x98, 0x19, 0x00, 0x00, 0x98, 0x19, 0x00, 0x00, 
  0x98, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xF0, 0x01, 0x00, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x18, 0x03, 0x00, 
  0x00, 0x18, 0x03, 0x00, 0x00, 0xF8, 0x03, 0x00, 0x00, 0xF8, 0x03, 0x00, 
  0x00, 0x18, 0x03, 0x00, 0x00, 0x18, 0x03, 0x00, 0x00, 0x18, 0x03, 0x00, 
  0x00, 0x18, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 
  0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0xFE, 0x00, 
  0x00, 0x00, 0xC6, 0x00, 0x00, 0x00, 0xC6, 0x00, 0x00, 0x00, 0xC6, 0x00, 
  0x00, 0x00, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };


  #endif // _SYMBOLS_
