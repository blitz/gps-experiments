CXXFLAGS=-std=c++11 -Os -flto -ffast-math -g

gps: main.o
	$(CXX) $(CXXFLAGS) -o $@ $<

all: gps

