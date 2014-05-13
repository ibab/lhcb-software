ROOTCFLAGS   := $(shell root-config --cflags --libs)

CXX	    = g++
CXXFLAGS    = -g -fPIC -Wall $(ROOTCFLAGS)

SOURCES     = read_options.cpp Multi.cpp Filereader.cpp

PRINT=@echo "<*** $@ compiled ***>"

TargetAnalysis.out: $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@
	$(PRINT)
	
clean:
	rm -f *.out
