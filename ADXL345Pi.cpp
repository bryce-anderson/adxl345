
#include "ADXL345Pi.hpp"

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

ADXL345Pi::ADXL345Pi(int i2cbus, Scale scale) {
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

ADXL345Pi::~ADXL345Pi() {
  if (handle != 0) {
    close(handle);
    handle = 0;
  }
}

int ADXL345Pi::readRegisters(char start, char* buff, int size, bool all)  {
  writeAddress(start);
  int size_read = read(handle, buff, size);
 
  if (all && size != size_read) {
    throw std::string("Failed to read all bytes");
  }
  else return size;
}

void ADXL345Pi::writeRegisters(char reg, char* buff, int size) {
  char towrite[24];
  bool noalloc = size < 24;

  char* all;

  if (noalloc) {
    all = towrite;
  }
  else {
    all = new char[size+1];
  }

  all[0] = reg;

  for (int i = 0; i < size; i++) {
    all[i+1] = buff[i];
  }

  int written = write(handle, all, size+1);
  
  if (!noalloc) {
    delete all;
  }
  all = 0;
  
  if (written != size+1) {
    throw std::string("Failed to write data.");
  }
}

void ADXL345Pi::writeAddress(char reg) {
  writeRegisters(reg, NULL, 0);
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

