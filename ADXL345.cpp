
#include "ADXL345.hpp"
#include <sstream>


std::string AccelData::toString() {
  std::stringstream ss;
  ss << "Accel x: " << x << " y: " << y << " z: " << z;
  return ss.str();
}

ADXL345::~ADXL345() {}

void ADXL345::setScale(Scale scale) {
  char old = readRegister(DATA_FORMAT);
  char bits = scale | (old & (~0x3));
  writeRegister(DATA_FORMAT, bits);
}

void ADXL345::initialize(Scale scale) {

  // Check the identity of the device
  if (readRegister(DEVID) != 0xe5) {
    throw std::string("This address doesn't belong to an ADXL345 accelerometer.");
  }

  setScale(scale);

  this->scale = scale;
}

void ADXL345::activate() {
  char old = readRegister(POWER_CTL);
  char next = old | (1 << 3);
  writeRegister(POWER_CTL, next);
}

void ADXL345::standby() {
  char old = readRegister(POWER_CTL);
  char next = old & (~(1 << 3));
  writeRegister(POWER_CTL, next);
}

AccelData ADXL345::readData() {
  AccelData result;
  char buff[6];

  readRegisters(DATAX0, buff, 6, true); 
  
  result.x = ((int)buff[0]) | ((int)buff[1] << 8);
  result.y = ((int)buff[2]) | ((int)buff[3] << 8);
  result.z = ((int)buff[4]) | ((int)buff[5] << 8);

  return result;
}

char ADXL345::readRegister(char reg) {
  char buff = 0;
  int numread = readRegisters(reg, &buff, 1);
  if (numread == 1) 
    return buff;
  else {
    throw std::string("Failed to read register.");
  }
}

void ADXL345::writeRegister(char reg, char value) {
  writeRegisters(reg, &value, 1);
}



