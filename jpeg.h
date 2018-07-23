/**
* Let's Build a JPEG Decoder: Segment lister
* JPEG class definition [jpeg.h]
* Imran Nazar, Jan 2013
*/

#ifndef __JPEG_H_
#define __JPEG_H_

#include "inttypes.h"
#include <string>
#include <vector>
#include <map>
#include <stdio.h>

// How to read a 16-bit word from the JPEG file
#define READ_WORD() ((fgetc(fp)<<8)|fgetc(fp))

// Segment parsing error codes
#define JPEG_SEG_ERR  0
#define JPEG_SEG_OK   1
#define JPEG_SEG_EOF -1

class JPEG {
private:
  // Defines a tuple of length and code, for use in the Huffman maps
  typedef std::pair<int, u16> huffKey;
  
  // The array of Huffman maps: (length, code) -> value
  std::map<huffKey, u8> huffData[32];
  
  // DHT segment handler
  int DHT();
  
  // Names of the possible segments
  std::string segNames[64];
  
  // The file to be read from, opened by constructor
  FILE *fp;
  
  // Segment parsing dispatcher
  int parseSeg();
  
public:
  // Construct a JPEG object given a filename
  JPEG(std::string);
};

#endif//__JPEG_H_

