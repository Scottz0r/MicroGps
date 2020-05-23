# Makefile for MicroGps test program.
OBJECTS = ../MicroGps.o tests.o
TARGET = TestMicroGps

program: $(OBJECTS)
	g++ $(OBJECTS) -o $(TARGET)

../MicroGps.o: ../MicroGps.h
	g++ -g -DDEBUG -c ../MicroGps.cpp -o $@

# Due to Catch2, this requires special assembler arguments, else it fails with "too large".
tests.o: ../MicroGps.cpp ../MicroGps.h
	g++ -g -DDEBUG -Wa,-mbig-obj -I.. -c tests.cpp -o $@

.PHONY: clean run
clean:
	rm $(TARGET) $(OBJECTS)

run:
	./$(TARGET)
