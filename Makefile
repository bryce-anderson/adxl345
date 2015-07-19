CXX = g++

CXXFLAGS= -Wall -Werror

OBJ = main.o ADXL345.o ADXL345Pi.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

adxltest: $(OBJ)
	g++ -o target/$@ $^ $(CFLAGS)
