CPPFLAGS=-MD
CXXFLAGS=-std=c++11 -Os -flto -ffast-math -g

SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all clean

all: gps

clean:
	rm -f *.d gps $(OBJECTS)

gps: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $<

-include *.d

# EOF
