
#ifndef ADXL345_HPP_
#define ADXL345_HPP_

#include <string>
#include <stddef.h>
#include <stdint.h>

#define DEVADDR 0x53

enum PowerMode {
  NormalPower = 0x0,
  LowPower    = 0x1<<4,
};

enum Scale {
  Scale_2G      = 0x0,
  Scale_4G      = 0x1,
  Scale_8G      = 0x2,
  Scale_16G     = 0x3,
  Scale_FullRes = 0xB
};

enum FIFOMode {
  Bypass  = 0x0,
  FIFO    = 0x1,
  Stream  = 0x2,
  Trigger = 0x3
};

enum {
  DEVID       = 0x00,
  OFSX        = 0x1e,
  OFSY        = 0x1f,
  OFSZ        = 0x20,
  BW_RATE     = 0x2c,
  POWER_CTL   = 0x2d,
  INT_SOURCE  = 0x30,
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

enum Rate {
  RATE_3200 = 0xF,
  RATE_1600 = 0xE,
  RATE_800  = 0xD,
  RATE_400  = 0xC,
  RATE_200  = 0xB,
  RATE_100  = 0xA,
  RATE_50   = 0x9,
  RATE_25   = 0x8,
  RATE_12_5 = 0x7,
  RATE_6_25 = 0x6,
  RATE_3_13 = 0x5,
  RATE_1_56 = 0x4,
  RATE_0_78 = 0x3,
  RATE_0_39 = 0x2,
  RATE_0_20 = 0x1,
  RATE_0_10 = 0x0
};


struct AccelData {
  int16_t x;
  int16_t y;
  int16_t z;

  Scale scale_g;

  float gx();
  float gy();
  float gz();

  std::string toString();
};

class ADXL345 {
public:
  virtual ~ADXL345();

  void setScale(Scale scale);
  Scale getScale();

  void setRate(Rate rate);
  Rate getRate();

  void setFIFO(FIFOMode mode);
  uint8_t getFIFOBytes();

  void setPowerMode(PowerMode mode);
  PowerMode getPowerMode();

  bool getActive();
  void setActive(bool active);

  void zeroOffsets();
  void initOffset();
  AccelData readData();

protected:
  void initialize(Scale scale = Scale_FullRes);

private:
  Scale scale;

  uint8_t readRegister(uint8_t register);
  void writeRegister(uint8_t reg, uint8_t value);
  int8_t checkoverflow(int32_t value);
  
  virtual size_t readRegisters(uint8_t startreg, uint8_t* buff, size_t size) = 0;
  virtual void writeRegisters(uint8_t reg, uint8_t* buff, size_t size) = 0;
  virtual void fatalError(std::string error) = 0;
  virtual void debug(std::string msg) = 0;
};


#endif // ADXL345_HPP_
