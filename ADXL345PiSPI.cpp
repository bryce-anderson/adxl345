
// See http://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/plain/Documentation/spi/spidev_test.c for some examples of how to use the linux SPI interface

#include "ADXL345PiSPI.hpp"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <string>
#include <sstream>
#include <iostream>
#include <cstdint>

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


// some forward declarations
static bool configBus(int file);

static void adxtrans(int fd, bool isread, uint8_t reg, uint8_t *buf, size_t len);
static void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len);

static const uint32_t speed = 500000; // frequency in Hz
static const uint8_t bits = 8;
static const uint16_t delay = 2; // us

ADXL345PiSPI::ADXL345PiSPI(int spibus, Scale scale) {
  char filename[64];
  int file;

  sprintf(filename, "/dev/spidev0.%d", spibus);

  file = open(filename, O_RDWR);

  if (file < 0) {
    throw std::string("Error opening SPI device");
  }

  // need to set the device into slave mode
  int success = configBus(file);
  if (!success) {
    close(file);
    throw std::string("Failed to open device!");
  } 

  this->handle = file;

  // successfully acquired a handle.. Finish setting up.
  initialize();
}

ADXL345PiSPI::~ADXL345PiSPI() {
  if (handle != 0) {
    close(handle);
    handle = 0;
  }
}

size_t ADXL345PiSPI::readRegisters(uint8_t reg, uint8_t* buff, size_t size, bool all)  {
  adxtrans(handle, true, reg, buff, size);
  return size;
}

void ADXL345PiSPI::writeRegisters(uint8_t reg, uint8_t* buff, size_t size) {
  adxtrans(handle, false, reg, buff, size);
}

void ADXL345PiSPI::fatalError(std::string error) {
  throw error;
}

void ADXL345PiSPI::debug(std::string msg) {
  std::cout << "DEBUG: " << msg << std::endl;
}

static bool configBus(int file) {
  const uint32_t mode = SPI_MODE_3;
  const uint32_t speed = speed;

  // set the mode
  if (ioctl(file, SPI_IOC_WR_MODE, &mode) == -1)
    throw std::string("Error setting spi WR mode");

  if (ioctl(file, SPI_IOC_RD_MODE, &mode) == -1)
    throw std::string("Error setting spi RD mode");

  // bits per word
  uint8_t wrbits = bits;
  if (ioctl(file, SPI_IOC_WR_BITS_PER_WORD, &wrbits) == -1)
    throw std::string("Error setting spi WR bits per word");

  uint8_t rdbits = bits;
  if (ioctl(file, SPI_IOC_RD_BITS_PER_WORD, &rdbits) == -1) {
    throw std::string("Error setting spi RD bits per word");
  }

  // se the max speed
  if (ioctl(file, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
    throw std::string("Error setting spi WR speed");

  if (ioctl(file, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1)
    throw std::string("Error setting spi RD speed");


  return true;
}

static void adxtrans(int fd, bool isread, uint8_t reg, uint8_t *buf, size_t len)
{
  uint8_t towrite[24];
  const bool noalloc = len < 24;

  uint8_t* all;

  if (noalloc) {
    all = towrite;
  }
  else {
    all = new uint8_t[len+1];
  }

  memset(all, 0, len+1);

  all[0] = reg & 0x3f;

  if (isread) {
    all[0] |= (0x1 << 7);
  }

  // Set the multi byte bit if necessary
  if (len > 1) {
    all[0] |= (0x1 << 6);
  }

  // copy to the buffer to the one going to the kernel
  if (!isread)
    memcpy(all+1, buf, len);

  transfer(fd, all, all, len+1);

  // copy to the buffer back
  if (isread)
    memcpy(buf, all+1, len);
 
  if (!noalloc) {
    delete all;
  }
  all = 0;
}

static void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	struct spi_ioc_transfer tr;
	memset(&tr, 0, sizeof(tr));

        tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = len;
	tr.cs_change = 0;
	tr.delay_usecs = delay;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;


	int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		throw std::string("can't send spi message");
}
