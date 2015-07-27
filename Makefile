CXX = g++

CXXFLAGS= -Wall -Werror

OBJ = main.o ADXL345.o ADXL345PiI2C.o ADXL345PiSPI.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ)
	g++ -o target/$@ $^ $(CFLAGS)

clean:
	rm *.o
	rm target/*
