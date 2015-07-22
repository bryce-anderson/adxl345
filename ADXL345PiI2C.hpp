#ifndef ADXL345PI_HPP_
#define ADXL345PI_HPP_

#include "ADXL345.hpp"

#include <linux/i2c-dev.h>

class ADXL345Pi: public ADXL345 {
public:
  ADXL345Pi(int bus, Scale scale = Scale_8G);
  virtual ~ADXL345Pi();

private:
  int handle;
  void writeAddress(char reg);
  virtual int readRegisters(char start, char* buff, int size, bool all = true);
  virtual void writeRegisters(char reg, char* buff, int size);
};


#endif // ADXL345PI_HPP_

