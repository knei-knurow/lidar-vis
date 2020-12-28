ROOT := ${CURDIR}

CXX := g++
CPPFLAGS := -std=c++17
LIBS := -lsfml-graphics -lsfml-window -lsfml-system
LDFLAGS := -L$(ROOT)/sfml/lib
INCLUDES := -I$(ROOT)/include -I$(ROOT)/sfml/include

all: main.o app.o cloud.o cloud-grabbers.o gui.o scenarios.o
	$(CXX) --output lidar-vis \
	main.o \
	app.o \
	cloud.o \
	cloud-grabbers.o \
	gui.o \
	scenarios.o \
	$(LDFLAGS) \
	$(LIBS)

main.o: src/main.cpp
	$(CXX) $(CPPFLAGS) -c src/main.cpp $(INCLUDES)

app.o: src/app.cpp
	$(CXX) $(CPPFLAGS) -c src/app.cpp $(INCLUDES)

cloud.o: src/cloud.cpp
	$(CXX) $(CPPFLAGS) -c src/cloud.cpp $(INCLUDES)

cloud-grabbers.o: src/cloud-grabbers.cpp
	$(CXX) $(CPPFLAGS) -c src/cloud-grabbers.cpp $(INCLUDES)

gui.o: src/gui.cpp
	$(CXX) $(CPPFLAGS) -c src/gui.cpp $(INCLUDES)

scenarios.o: src/scenarios.cpp
	$(CXX) $(CPPFLAGS) -c src/scenarios.cpp $(INCLUDES)

clean:
	rm -f *.o src/*.o lidar-vis
