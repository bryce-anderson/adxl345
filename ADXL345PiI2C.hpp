#ifndef __ADXL345PI_I2C_HPP__
#define __ADXL345PI_I2C_HPP__

#include "ADXL345.hpp"


class ADXL345PiI2C: public ADXL345 {
public:
  ADXL345PiI2C(int bus, Scale scale = Scale_8G);
  virtual ~ADXL345PiI2C();

private:
  int handle;
  void writeAddress(uint8_t reg);
  virtual size_t readRegisters(uint8_t start, uint8_t* buff, size_t size, bool all = true);
  virtual void writeRegisters(uint8_t reg, uint8_t* buff, size_t size);
  virtual void fatalError(std::string error);
  virtual void debug(std::string msg);
};


#endif // __DXL345PI_I2C_HPP__
