
#include "ADXL345.hpp"
#include <sstream>
#include <climits>


std::string AccelData::toString() {
  std::stringstream ss;
  ss << "Accel x: " << gx() << " y: " << gy() << " z: " << gz();
  return ss.str();
}

float tog(Scale scale, int16_t g) {
  float conversion = 0.0f;
  switch (scale) {
    // these values come from the spec
    case Scale_FullRes: // fall through to 2G
    case Scale_2G:  conversion = 3.9f/1000;  break;
    case Scale_4G:  conversion = 7.8f/1000;  break;
    case Scale_8G:  conversion = 15.6f/1000; break;
    case Scale_16G: conversion = 31.2f/1000; break;
  }

  return (float)g*conversion;
}

float AccelData::gx() { return tog(scale_g, x); }
float AccelData::gy() { return tog(scale_g, y); }
float AccelData::gz() { return tog(scale_g, z); }
  

ADXL345::~ADXL345() {}

void ADXL345::setScale(Scale scale) {
  uint8_t old = readRegister(DATA_FORMAT);
  uint8_t bits = scale | (old & (~0x3));
  writeRegister(DATA_FORMAT, bits);
  this->scale = scale;
}

Scale ADXL345::getScale() {
  return scale;
}

void ADXL345::setRate(Rate rate) {
  uint8_t reg = readRegister(BW_RATE);
  reg &= ~0xf;
  reg |= rate;
  writeRegister(BW_RATE, reg);
}

Rate ADXL345::getRate() {
  uint8_t reg = readRegister(BW_RATE);
  return Rate(reg & 0xf);
}

void ADXL345::setPowerMode(PowerMode mode) {
  uint8_t reg = readRegister(BW_RATE);
  const uint8_t bit = 0x1 << 4;
  switch (mode) {
    case LowPower: reg |= bit; break;
    case NormalPower: reg &= ~bit; break;
  }
  writeRegister(BW_RATE, reg);
}

PowerMode ADXL345::getPowerMode() {
  uint8_t reg = readRegister(BW_RATE);
  const uint8_t bit = 0x1 << 4;
  return PowerMode(reg & bit);
}

void ADXL345::zeroOffsets() {
  uint8_t zeros[3] = { 0x0, 0x0, 0x0 };
  writeRegisters(OFSX, zeros, sizeof(zeros));
}

void ADXL345::setFIFO(FIFOMode mode) {
  uint8_t reg = readRegister(FIFO_CTL);
  reg &= ~(0x3 << 6);
  reg |= mode << 6;
  writeRegister(FIFO_CTL, reg);
}

uint8_t ADXL345::getFIFOBytes() {
  uint8_t reg = readRegister(FIFO_CTL);
  return reg & 0x1f;
}


void ADXL345::initialize(Scale scale) {

  // Check the identity of the device
  uint8_t reg = readRegister(DEVID);
  if (reg != 0xe5) {
    std::stringstream ss;
    ss << "This address doesn't belong to an ADXL345 accelerometer. "
       << "Wrong device id: " 
       << (int)reg 
       << std::endl;
    fatalError(ss.str());
  }

  setActive(false);
  setFIFO(Bypass);
  setScale(scale);
  setPowerMode(NormalPower);
  setRate(RATE_25);
}

int8_t ADXL345::checkoverflow(int32_t value) {
  if (value > SCHAR_MAX) {
    debug("WARN: offset overflow: MAX");
    return SCHAR_MAX;
  }
  else if (value < SCHAR_MIN) {
    debug("WARN: offset overflow: MIN");
    return SCHAR_MIN;
  }
  else                        return (int8_t)value;
}

void ADXL345::initOffset() {
  return; // this is broken
  const float offsetscale = 1.0f/3.9f; // LSB/mg
  const bool active = getActive();
  zeroOffsets();

  if (!active)
    setActive(true);

  float x = 0.0f, y = 0.0f, z = 0.0f;

  for (size_t i = 0; i < 33; i++)
    readData(); // burn the fifo

  for (size_t i = 0; i < 100; i++) {
    AccelData r = readData();
    x += r.x;
    y += r.y;
    z += r.z;
  }
  x /= 100.0f;
  y /= 100.0f;
  z /= 100.0f;

  int32_t off32[3];
  off32[0] = int32_t((-x)*offsetscale);
  off32[1] = int32_t((-y)*offsetscale);
  off32[2] = int32_t((-z)*offsetscale);

  uint8_t buff[3];

  for (size_t i=0; i < 3; i++) {
    buff[i] = checkoverflow(off32[i]);
  }

  writeRegisters(OFSX, buff, sizeof(buff));

  if (!active)
    setActive(false);
}

bool ADXL345::getActive() {
  uint8_t old = readRegister(POWER_CTL);
  return 0 != (old & (1 << 3));
}

void ADXL345::setActive(bool active) {
  uint8_t reg = readRegister(POWER_CTL);
  const uint8_t bit = 0x1 << 3;

  if (active) {
    reg |= bit;
  }
  else {
    reg &= ~bit;
  }
  writeRegister(POWER_CTL, reg);
}

AccelData ADXL345::readData() {
  AccelData result;
  result.scale_g = scale;
  uint8_t buff[6];

  size_t read = readRegisters(DATAX0, buff, 6);
  if (read != 6) {
    fatalError("Failed to read all data bytes.");
  }
  
  result.x = ((int)buff[0]) | ((int)buff[1] << 8);
  result.y = ((int)buff[2]) | ((int)buff[3] << 8);
  result.z = ((int)buff[4]) | ((int)buff[5] << 8);

  return result;
}

uint8_t ADXL345::readRegister(uint8_t reg) {
  uint8_t buff = 0;
  int numread = readRegisters(reg, &buff, 1);
  if (numread == 1) 
    return buff;
  else {
    fatalError("Failed to read register.");
    return 0;
  }
}

void ADXL345::writeRegister(uint8_t reg, uint8_t value) {
  writeRegisters(reg, &value, 1);
}

