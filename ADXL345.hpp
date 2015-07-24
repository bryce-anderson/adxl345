
#ifndef ADXL345_HPP_
#define ADXL345_HPP_

#include <string>

#define DEVADDR 0x53



enum Scale {
  Scale_2G  = 0x0,
  Scale_4G  = 0x1,
  Scale_8G  = 0x2,
  Scale_16G = 0x3,
};

enum {
  DEVID       = 0x00,
  POWER_CTL   = 0x2d,
  DATA_FORMAT = 0x31,
  DATAX0      = 0x32,
  DATAX1      = 0x33,
  DATAY0      = 0x34,
  DATAY1      = 0x35,
  DATAZ0      = 0x36,
  DATAZ1      = 0x37,
  FIFO_CTL    = 0x38,
  FIFO_STATUS = 0x39
};


struct AccelData {
  short x;
  short y;
  short z;

  std::string toString();
};

class ADXL345 {
public:
  virtual ~ADXL345();

  void setScale(Scale scale);
  AccelData readData();
  void activate();
  void standby();

protected:
  void initialize(Scale scale = Scale_8G);

private:
  Scale scale;


  uint8_t readRegister(uint8_t register);
  void writeRegister(uint8_t reg, uint8_t value);
  
  virtual size_t readRegisters(uint8_t startreg, uint8_t* buff, size_t size, bool all = true) = 0;
  virtual void writeRegisters(uint8_t reg, uint8_t* buff, size_t size) = 0;
};


#endif // ADXL345_HPP_
