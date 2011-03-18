#--- Mimimal Variables ---#
PROJECT = library
OBJECTS = ResonanceMassFit.o
CXXFLAGS     = -O3 -g -fPIC -funroll-loops -D__ROOFIT_NOBANNER -shared# -Wall
CXX     = g++
SOFLAGS = -shared
LDFLAGS = -O

#--- ROOT Variables ---#
ROOTCFLAGS   = $(shell root-config --cflags)
ROOTLIBS     = $(shell root-config --nonew --libs)
ROOTGLIBS    = $(shell root-config --nonew --glibs)

#--- Update Library Variables ---#
CXXFLAGS += $(ROOTCFLAGS)
LIBS     += $(ROOTLIBS) -L. -lHtml -lThread -lMinuit -lRooFit -lRooFitCore -lRooStats
GLIBS    += $(ROOTGLIBS) -L. -lHtml -lThread -lMinuit -lRooFit -lRooFitCore -lRooStats

INCLUDE_DIRECTORIES = ../CalibDataProduction
vpath %.h $(INCLUDE_DIRECTORIES)
#CXXFLAGS += $(addprefix -I ,$(INCLUDE_DIRECTORIES))
CXXFLAGS += -I../
vpath %.cpp ../src

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@.so
	mv *.so libCalibDataProduction.so
	rm -f *.o

clean : 
	rm -f *.{o,so}
	rm -f lib/*.{o,so}

