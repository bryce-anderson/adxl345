
#include "ADXL345PiI2C.hpp"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <string>
#include <sstream>
#include <iostream>

#include <linux/i2c-dev.h>

// some forward declarations
int set_slave_addr(int file, int address, int force);

ADXL345PiI2C::ADXL345PiI2C(int i2cbus, Scale scale) {
  char filename[64];
  int file;

  sprintf(filename, "/dev/i2c-%d", i2cbus);

  file = open(filename, O_RDWR);

  if (file < 0) {
    throw std::string("Error opening I2C device");
  }

  // need to set the device into slave mode
  int failed = set_slave_addr(file, DEVADDR, 0);
  if (failed) {
    close(file);
    throw std::string("Failed to open device!");
  } 

  this->handle = file;

  // successfully acquired a handle.. Finish setting up.
  initialize();
}

ADXL345PiI2C::~ADXL345PiI2C() {
  if (handle != 0) {
    close(handle);
    handle = 0;
  }
}

size_t ADXL345PiI2C::readRegisters(uint8_t start, uint8_t* buff, size_t size, bool all)  {
  writeAddress(start);
  size_t size_read = read(handle, buff, size);
 
  if (all && size != size_read) {
    throw std::string("Failed to read all bytes");
  }
  else return size;
}

void ADXL345PiI2C::writeRegisters(uint8_t reg, uint8_t* buff, size_t size) {
  uint8_t towrite[24];
  bool noalloc = size < 24;

  uint8_t* all;

  if (noalloc) {
    all = towrite;
  }
  else {
    all = new uint8_t[size+1];
  }

  all[0] = reg;

  for (size_t i = 0; i < size; i++) {
    all[i+1] = buff[i];
  }

  size_t written = write(handle, all, size+1);
  
  if (!noalloc) {
    delete all;
  }
  all = 0;
  
  if (written != size+1) {
    throw std::string("Failed to write data.");
  }
}

void ADXL345PiI2C::writeAddress(uint8_t reg) {
  writeRegisters(reg, NULL, 0);
}

void ADXL345PiI2C::fatalError(std::string error) {
  throw error;
}

void ADXL345PiI2C::debug(std::string msg) {
  std::cout << "DEBUG: " << msg << std::endl;
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

