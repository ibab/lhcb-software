ROOTCFLAGS   := $(shell root-config --cflags --libs)

CXX	    = g++
CXXFLAGS    = -Wall $(ROOTCFLAGS)

SOURCES     = drawPlanes.cpp

#all: Main.out

PRINT=@echo "<*** $@ compiled ***>"
MAKEDIR=@mkdir -p plots

drawPlanes.out: $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@;
	$(MAKEDIR)
	$(PRINT)
	
clean:
	rm -f *.out
