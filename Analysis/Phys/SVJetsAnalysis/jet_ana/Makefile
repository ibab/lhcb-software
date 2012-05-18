ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) -lMLP -lTreePlayer -lMinuit
ROOTGLIBS     = $(shell root-config --glibs)
TMVALIB	      = `pwd`/TMVA/lib/libTMVA.so
TMVALIB	      = ./TMVA/lib/libTMVA.so
#TMVAFLAG      = -I./TMVA
TMVAFLAG      = -I./TMVA/include/

CXX           = g++
CXXFLAGS      = -g -I$(ROOTSYS)/include -O -Wall -fPIC
LD            = g++
SOFLAGS       = -shared


CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS)
GLIBS         = $(ROOTGLIBS)

all: makecuts zbbana

ZbbAnaOBJ = lib/TES.o lib/JetIDCut.o lib/ROOTOscar.o lib/evtsel.o  lib/selvars.o lib/histograms.o  lib/jet_ana.o

zbbana:	$(ZbbAnaOBJ) RunZbbAna.exe

### SELECTION OBJECTS/EXECUTABLE ###

makecuts:
	  python python/User_Cuts.py JetIDCut configfiles/jetid.cpy

lib/ROOTOscar.o: include/ROOTOscar.cc include/ROOTOscar.h
	$(CXX) -ggdb -c $(ROOTCFLAGS) $<
	mv ROOTOscar.o lib/

lib/TES.o: include/TES.cc include/TES.h
	$(CXX) -ggdb -c $(ROOTCFLAGS) $<
	mv TES.o lib/

lib/JetIDCut.o: include/JetIDCut.cc include/JetIDCut.h
	$(CXX) -ggdb -c $(ROOTCFLAGS) $<
	mv JetIDCut.o lib/

lib/evtsel.o: include/evtsel.cc include/evtsel.h include/JetIDCut.h
	$(CXX) -ggdb -c $(ROOTCFLAGS) $<
	mv evtsel.o lib/

lib/jet_ana.o: include/jet_ana.cc include/*
	$(CXX) -ggdb -c $(ROOTCFLAGS) $<
	mv jet_ana.o lib/

lib/histograms.o: include/histograms.cc include/histograms.h
	$(CXX) -ggdb -c $(ROOTCFLAGS) $<
	mv histograms.o lib/

lib/selvars.o: include/selvars.cc include/selvars.h
	$(CXX) -ggdb -c $(ROOTCFLAGS) $<
	mv selvars.o lib/

RunZbbAna.exe: $(ZbbAnaOBJ)
	$(CXX) -rdynamic -o jet_ana.exe $(ZbbAnaOBJ) $(ROOTLIBS) -L./lib64 -ldl

results:
	source results.sh


test:
	./jet_ana.exe crap ../Resources/EW_WMu_DataStrip17/48/ZJESAllMC.root output/EW_Data_WMu.root MC

run_ew_mu:
	#./jet_ana.exe crap ../Resources/EW_WMu_DataStrip17/48/ZJESAllMC.root output/EW_Data_WMu.root MC
	#./jet_ana.exe crap ../Resources/EW_WMu_DataStrip17/49/ZJESAllMC.root output/EW_Data_WMu.root MC
	#./jet_ana.exe crap ../Resources/EW_WMu_DataStrip17/61/ZJESAllMC.root output/EW_Data_WMu.root MC
	./jet_ana.exe crap ../Resources/EW_WMu_DataStrip17/79/ZJESAllMC.root output/EW_Data_WMu.root MC
	
run_inclb:
	./jet_ana.exe crap ../Resources/inclb_MC10/43/ZJESAllMC.root output/inclb_MC10.root MC

run_ttbar:
	./jet_ana.exe crap ../Resources/ttbar_MC10/32/ZJESAllMC.root output/ttbar.root MC

run_wmunujet:
	./jet_ana.exe crap ../Resources/Wmunujet/35/ZJESAllMC.root output/wmunujet.root MC
run_zmumujet:
	./jet_ana.exe crap ../Resources/Zmumujet_MC10/34/ZJESAllMC.root output/zmumu.root MC

runall:
	./runall.sh

clean:
	rm -rf lib/*.o lib/*.so core* jet_ana*exe results

