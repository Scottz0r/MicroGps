# Makefile for MicroGps test program.
OBJECTS = ../MicroGps.o ../MicroGpsFormat.o test_main.o MicroGps_tests.o MicroGpsFormat_tests.o
TARGET = TestMicroGps

program: $(OBJECTS)
	g++ $(OBJECTS) -o $(TARGET)

../MicroGps.o: ../MicroGps.h ../MicroGpsTypes.h
	g++ -g -c ../MicroGps.cpp -o $@

../MicroGpsFormat.o: ../MicroGpsFormat.h ../MicroGpsTypes.h
	g++ -g -c ../MicroGpsFormat.cpp -o $@

MicroGps_tests.o: MicroGps_tests.cpp ../MicroGps.h
	g++ -g -c -I.. MicroGps_tests.cpp -o $@

MicroGpsFormat_tests.o: MicroGpsFormat_tests.cpp ../MicroGpsFormat.h
	g++ -g -c -I.. MicroGpsFormat_tests.cpp -o $@

test_main:
	g++ -g -c test_main.cpp -o $@

.PHONY: clean run
clean:
	rm $(TARGET) $(OBJECTS)

run:
	./$(TARGET)
