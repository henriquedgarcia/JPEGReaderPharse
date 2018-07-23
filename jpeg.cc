/**
* Let's Build a JPEG Decoder: Segment lister
* JPEG class implementation [jpeg.cpp]
* Imran Nazar, Jan 2013
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "jpeg.h"

//-------------------------------------------------------------------------
// Function: Parse JPEG file segment (parseSeg)
// Purpose: Retrieves 16-bit block ID from file, shows name

int JPEG::DHT()
{
  int i, j;
  
  // A counter of how many bytes have been read
  int ctr = 0;
  
  // The incrementing code to be used to build the map
  u16 code = 0;
  
  // First byte of a DHT segment is the table ID, between 0 and 31
  u8 table = fgetc(fp);
  ctr++;
  
  // Next sixteen bytes are the counts for each code length
  u8 counts[16];
  for (i = 0; i < 16; i++) 
    {
      counts[i] = fgetc(fp);
      ctr++;
    }
  
  // Remaining bytes are the data values to be mapped
  // Build the Huffman map of (length, code) -> value
  for (i = 0; i < 16; i++) {
      for (j = 0; j < counts[i]; j++) 
        {
          huffData[table][huffKey(i + 1, code)] = fgetc(fp);
          code++;
          ctr++;
        }
      code <<= 1;
    }
  
  // Once the map has been built, print it out
  printf("Huffman table #%02X:\n", table);
  
  std::map<huffKey, u8>::iterator iter;
  for (iter = huffData[table].begin();
       iter != huffData[table].end();
       iter++
       ) {
      printf("    %04X at length %d = %02X\n",
             iter->first.second, iter->first.first, iter->second);
    }
  return ctr;
}

int JPEG::parseSeg()
{
  if (!fp) {
      printf("File failed to open.\n");
      return JPEG_SEG_ERR;
    }
  
  u32 fpos = ftell(fp);
  u16 id = READ_WORD(), size;    
  if(id < 0xFFC0)
    {
      printf("Segment ID expected, not found.\n");
      return JPEG_SEG_ERR;
    }
  
  printf("Found segment at file position %d: %s\n", fpos, segNames[id-0xFFC0].c_str());
  
  switch (id) {
    // The DHT segment defines a Huffman table. The handler should
    // read exactly as many bytes from the file as are in the
    // segment; if not, something's gone wrong
    case 0xFFC4:
      size = READ_WORD() - 2;
      if (DHT() != size) {
          printf("Unexpected end of DHT segment\n");
          return JPEG_SEG_ERR;
        }
      break;
      
      // The SOI and EOI segments are the only ones not to have
      // a length, and are always a fixed two bytes long; do
      // nothing to advance the file position
    case 0xFFD9:
      return JPEG_SEG_EOF;
    case 0xFFD8:
      break;
      
      // An SOS segment has a length determined only by the
      // length of the bitstream; for now, assume it's the rest
      // of the file less the two-byte EOI segment
    case 0xFFDA:
      fseek(fp, -2, SEEK_END);
      break;
      
      // Any other segment has a length specified at its start,
      // so skip over that many bytes of file
    default:
      size = READ_WORD();
      fseek(fp, size-2, SEEK_CUR);
      break;
    }
  
  return JPEG_SEG_OK;
}

//-------------------------------------------------------------------------
// Function: Array initialisation (constructor)
// Purpose: Fill in arrays used by the decoder, decode a file
// Parameters: filename (string) - File to decode

JPEG::JPEG(std::string filename)
{
  // Debug messages used by parseSeg to tell us which segment we're at
  segNames[0x00] = std::string("Baseline DCT; Huffman");
  segNames[0x01] = std::string("Extended sequential DCT; Huffman");
  segNames[0x02] = std::string("Progressive DCT; Huffman");
  segNames[0x03] = std::string("Spatial lossless; Huffman");
  segNames[0x04] = std::string("Huffman table");
  segNames[0x05] = std::string("Differential sequential DCT; Huffman");
  segNames[0x06] = std::string("Differential progressive DCT; Huffman");
  segNames[0x07] = std::string("Differential spatial; Huffman");
  segNames[0x08] = std::string("[Reserved: JPEG extension]");
  segNames[0x09] = std::string("Extended sequential DCT; Arithmetic");
  segNames[0x0A] = std::string("Progressive DCT; Arithmetic");
  segNames[0x0B] = std::string("Spatial lossless; Arithmetic");
  segNames[0x0C] = std::string("Arithmetic coding conditioning");
  segNames[0x0D] = std::string("Differential sequential DCT; Arithmetic");
  segNames[0x0E] = std::string("Differential progressive DCT; Arithmetic");
  segNames[0x0F] = std::string("Differential spatial; Arithmetic");
  segNames[0x10] = std::string("Restart");
  segNames[0x11] = std::string("Restart");
  segNames[0x12] = std::string("Restart");
  segNames[0x13] = std::string("Restart");
  segNames[0x14] = std::string("Restart");
  segNames[0x15] = std::string("Restart");
  segNames[0x16] = std::string("Restart");
  segNames[0x17] = std::string("Restart");
  segNames[0x18] = std::string("Start of image");
  segNames[0x19] = std::string("End of image");
  segNames[0x1A] = std::string("Start of scan");
  segNames[0x1B] = std::string("Quantisation table");
  segNames[0x1C] = std::string("Number of lines");
  segNames[0x1D] = std::string("Restart interval");
  segNames[0x1E] = std::string("Hierarchical progression");
  segNames[0x1F] = std::string("Expand reference components");
  segNames[0x20] = std::string("JFIF header");
  segNames[0x21] = std::string("[Reserved: application extension]");
  segNames[0x22] = std::string("[Reserved: application extension]");
  segNames[0x23] = std::string("[Reserved: application extension]");
  segNames[0x24] = std::string("[Reserved: application extension]");
  segNames[0x25] = std::string("[Reserved: application extension]");
  segNames[0x26] = std::string("[Reserved: application extension]");
  segNames[0x27] = std::string("[Reserved: application extension]");
  segNames[0x28] = std::string("[Reserved: application extension]");
  segNames[0x29] = std::string("[Reserved: application extension]");
  segNames[0x2A] = std::string("[Reserved: application extension]");
  segNames[0x2B] = std::string("[Reserved: application extension]");
  segNames[0x2C] = std::string("[Reserved: application extension]");
  segNames[0x2D] = std::string("[Reserved: application extension]");
  segNames[0x2E] = std::string("[Reserved: application extension]");
  segNames[0x2F] = std::string("[Reserved: application extension]");
  segNames[0x30] = std::string("[Reserved: JPEG extension]");
  segNames[0x31] = std::string("[Reserved: JPEG extension]");
  segNames[0x32] = std::string("[Reserved: JPEG extension]");
  segNames[0x33] = std::string("[Reserved: JPEG extension]");
  segNames[0x34] = std::string("[Reserved: JPEG extension]");
  segNames[0x35] = std::string("[Reserved: JPEG extension]");
  segNames[0x36] = std::string("[Reserved: JPEG extension]");
  segNames[0x37] = std::string("[Reserved: JPEG extension]");
  segNames[0x38] = std::string("[Reserved: JPEG extension]");
  segNames[0x39] = std::string("[Reserved: JPEG extension]");
  segNames[0x3A] = std::string("[Reserved: JPEG extension]");
  segNames[0x3B] = std::string("[Reserved: JPEG extension]");
  segNames[0x3C] = std::string("[Reserved: JPEG extension]");
  segNames[0x3D] = std::string("[Reserved: JPEG extension]");
  segNames[0x3E] = std::string("Comment");
  segNames[0x3F] = std::string("[Invalid]");
  
  // Open the requested file, keep parsing blocks until we run
  // out of file, then close it.
  fp = fopen(filename.c_str(), "rb");
  if (fp) {
      while(parseSeg() == JPEG_SEG_OK);
      fclose(fp);
    }
  else
    {
      perror("JPEG");
    }
}

