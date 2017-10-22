#include "Map.hpp"


Map::Map(size_t _width, size_t _height)
  : width {_width}
  , height  {_height}
  , data  {new byte[width * height]} {

  if(data != nullptr) {
    memset(data, 0x00, width*height);
  }
}

Map::~Map() {
  if(data != nullptr) {
    delete[] data;
  }
}

Map::operator bool() const {
  return data != nullptr;
}

const byte* Map::getRaw() const {
  return data;
}

byte Map::get(size_t x, size_t y) const {
  if( (x >= width) || (y >= height) ) {
    return 0;
  }
  else {
    return data[width*y + x];
  }
}

