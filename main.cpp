#include "ADXL345PiI2C.hpp"
#include "ADXL345PiSPI.hpp"

#include <iostream>
#include <stdlib.h>

#include <signal.h>

using namespace std;

bool stopI2C = false;

void handleSigInt(int param) {
  cout << "CTRL-C\n";
  stopI2C = true;
}

int main() {
  signal(SIGINT, handleSigInt);

  cout << "Attempting to measure from ADX345.\n";

  try {
    ADXL345PiI2C sensor(1, Scale_8G);
    //ADXL345PiSPI sensor(0, Scale_8G);
    sensor.activate();

    while (!stopI2C) {
      usleep(10*1000);
      cout << sensor.readData().toString() << endl;
    }

    cout << "Shutting down." << endl;
    sensor.standby();
  }
  catch (const string& str) {
    cerr << "Failed: " << str << endl;
  }

  // all done.
  return 0;
}

