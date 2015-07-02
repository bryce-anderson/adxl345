#include "ADXL345.hpp"

#include <iostream>
#include <stdlib.h>

using namespace std;

int main() {
  cout << "Hello world!\n";

  try {
    ADXL345 sensor(1);
    sensor.activate();

    sleep(1);

    cout << sensor.readData().toString() << endl;

    sensor.standby();


  }
  catch (const string& str) {
    cerr << "Failed: " << str << endl;
  }
  catch (const char* err) {
    cerr << "Failed: " << err << endl;
  }

  return 0;
}

