#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <iostream>
#include <string>
#include <vector>

#include "Reader.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace filerd {
  class Image {
    public:
      string name;
      string format;
      string cod;
      // Stores the information of the image (pixel grid)
      string buff;
      int width;
      int height;
      int aliasSamples;

      Image( void );
      Image(  string name, string format, string cod, int width, int height,
              int aliasSamples);
      /*  Returns the standard header of the current file format. For example,
          if the current Image is a PPM, then this method will return:

          P3
          width height
          255
      */
      string Header();
      string Description();
      void FromContent( vector<string> fileContent );
  };
} // namespace filerd

#include "Image.inl"
#endif