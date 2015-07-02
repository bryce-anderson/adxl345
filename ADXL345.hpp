
#ifndef ADXL345_HPP_
#define ADXL345_HPP_

#include <string>

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
  ADXL345(int bus, Scale scale = Scale_8G);

  virtual ~ADXL345();

  void setScale(Scale scale);
  AccelData readData();
  void activate();
  void standby();

private:
  int handle;
  Scale scale;

  void writeAddress(char reg);

  int readRegisters(char start, char* buff, int size, bool all = true);
  char readRegister(char register);

  void writeRegister(char reg, char value);
  void writeRegisters(char reg, char* buff, int size);

};


#endif // ADXL345_HPP_
