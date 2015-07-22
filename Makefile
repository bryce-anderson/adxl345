CXX = g++

CXXFLAGS= -Wall -Werror

OBJ = main.o ADXL345.o ADXL345PiI2C.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

adxltest: $(OBJ)
	g++ -o target/$@ $^ $(CFLAGS)

clean:
	rm *.o
