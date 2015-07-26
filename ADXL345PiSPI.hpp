#ifndef ADXL345PI_HPP_
#define ADXL345PI_HPP_

#include "ADXL345.hpp"

class ADXL345PiSPI: public ADXL345 {
public:
  ADXL345PiSPI(int bus, Scale scale = Scale_8G);
  virtual ~ADXL345PiSPI();

private:
  int handle;
  virtual size_t readRegisters(uint8_t startreg, uint8_t* buff, size_t size, bool all = true);
  virtual void writeRegisters(uint8_t reg, uint8_t* buff, size_t size);
  virtual void fatalError(std::string error);
  virtual void debug(std::string msg);

};


#endif // ADXL345PI_HPP_

