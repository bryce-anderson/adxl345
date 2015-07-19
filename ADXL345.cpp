
#include "ADXL345.hpp"

#include <linux/i2c-dev.h>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <string>
#include <sstream>
#include <iostream>


// some forward declarations
int set_slave_addr(int file, int address, int force);

static int dev_addr = 0x53;

std::string AccelData::toString() {
  std::stringstream ss;
  ss << "Accel x: " << x << " y: " << y << " z: " << z;
  return ss.str();
}

void ADXL345::setScale(Scale scale) {
  char old = readRegister(DATA_FORMAT);
  char bits = scale | (old & (~0x3));
  writeRegister(DATA_FORMAT, bits);
}

ADXL345::ADXL345(int i2cbus, Scale scale) {
  char filename[64];
  int file;

  sprintf(filename, "/dev/i2c-%d", i2cbus);

  file = open(filename, O_RDWR);

  if (file < 0) {
    throw std::string("Error opening I2C device");
  }

  // need to set the device into slave mode
  int failed = set_slave_addr(file, dev_addr, 0);
  if (failed) {
    close(file);
    throw std::string("Failed to open device!");
  } 

  // successfully acquired a handle.. Finish setting up.
  this->handle = file;

  // Check the identity of the device
  if (readRegister(DEVID) != 0xe5) {
    throw std::string("This address doesn't belong to an ADXL345 accelerometer.");
  }

  setScale(scale);

  this->scale = scale;
}

ADXL345::~ADXL345() {
  if (handle != 0) {
    close(handle);
    handle = 0;
  }
}

void ADXL345::activate() {
  char old = readRegister(POWER_CTL);
  std::cout << "DEBUG: old reg: " << (int)old << std::endl;
  char next = old | (1 << 3);
  std::cout << "DEBUG: next reg: " << (int)next << std::endl;
  writeRegister(POWER_CTL, next);

  old = readRegister(POWER_CTL);
  std::cout << "DEBUG: old reg: " << (int)old << std::endl;
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

void ADXL345::writeAddress(char reg) {
  if (write(handle, &reg, 1) != 1) {
    throw std::string("Failed to write register address.");
  }
}

int ADXL345::readRegisters(char start, char* buff, int size, bool all)  {
  writeAddress(start);
  int size_read = read(handle, buff, size);
 
  if (all && size != size_read) {
    throw std::string("Failed to read all bytes");
  }
  else return size;
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

void ADXL345::writeRegisters(char reg, char* buff, int size) {
  char* all = new char[size+1];
  all[0] = reg;

  for (int i = 0; i < size; i++) {
    all[i+1] = buff[i];
  }

  int written = write(handle, all, size+1);
  delete all;
  all = 0;
  
  if (written != size+1) {
    throw std::string("Failed to write data.");
  }
}

void ADXL345::writeRegister(char reg, char value) {
  writeRegisters(reg, &value, 1);
}

int set_slave_addr(int file, int address, int force) {
  if (ioctl(file, force ? I2C_SLAVE_FORCE : I2C_SLAVE, address) < 0) {
    fprintf(stderr,
            "Error: Couldn't set address to 0x%02x: %s\n",
            address, strerror(errno));
    return -errno;
  } else {
    return 0;
  }
}


