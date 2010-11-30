#------------------------------------------------------------------------------
# Makefile for Tagging Analysis and Tuning
#
# Author: Marco Musy (Universitat de Barcelona), 07/09/2010
#------------------------------------------------------------------------------

ROOTCONFIG   := root-config

PLATFORM     := $(shell $(ROOTCONFIG) --platform)
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --glibs)

CXX           = g++
CXXFLAGS      = -O2 -fPIC -W 
CXXFLAGS_DBG  = -fPIC -Wall -g3 -gdwarf-2 
LD            = g++
LDFLAGS       = -O2
SOFLAGS       = -shared
EXTRALIBS     = -lMLP  -lRGL -lGeom -lEve -lTreePlayer -lGed -lEG
EXTRAFLAGS    = -Iinclude -Isrc -Innet 

#------------------------------------------------------------------------------
CXXFLAGS     += $(ROOTCFLAGS) $(EXTRAFLAGS)
CXXFLAGS_DBG += $(ROOTCFLAGS) $(EXTRAFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)
LIBS          = $(ROOTLIBS)  $(SYSLIBS) $(EXTRALIBS) 
COMPILE       = $(CXX) $(CXXFLAGS)     -c $< -o $@
COMPILE_DBG   = $(CXX) $(CXXFLAGS_DBG) -c $< -o $@
DEPEND        = makedepend $(EXTRAFLAGS)
NROFINSTANCES = $(shell ls -1 ../tagpackage*.zip | wc -l)

SRCS          = tag.cxx src/*.cxx nnet/*.cxx 
OBJETOS       = lib/tag.o $(shell ls src/*.cxx nnet/*.cxx \
                | sed s/src/lib/| sed s/nnet/lib/ | sed s/cxx/o/ ) 

EXECUTABLE    = tag.exe
OUTPUTLOG     = output/logfile.txt

#------------------------------------------------------------------------------
all:  $(EXECUTABLE)
	make run
	make plots

#------------------------------------------------------------------------------
$(EXECUTABLE): $(OBJETOS)
	@echo "Linking for platform: $(PLATFORM)"
	@$(LD) $(LDFLAGS) ./lib/*.o $(LIBS) -o $(EXECUTABLE)

#------------------------------------------------------------------------------
debug:  
	@rm -f ./lib/*.o $(EXECUTABLE)
	@echo "DEBUG compile and link for platform: $(PLATFORM)"
	$(CXX) $(CXXFLAGS_DBG) -c $(SRCS) 
	@mv *.o lib
	$(LD) $(ROOTLDFLAGS) ./lib/*.o $(LIBS) -o $(EXECUTABLE)
	@echo "gdb ./tag.exe program OK."
	@echo " *** Type r and then bt inside gdb to dump debug info!"
	gdb ./$(EXECUTABLE) | tee $(OUTPUTLOG)
#	valgrind --leak-check=yes ./$(EXECUTABLE) &> $(OUTPUTLOG)
#	valgrind --tool=callgrind --dump-instr=yes --trace-jump=yes ./tag.exe
#	kcachegrind callgrind.out.*

#------------------------------------------------------------------------------
run:  $(EXECUTABLE)
	@./$(EXECUTABLE) | tee $(OUTPUTLOG)

#------------------------------------------------------------------------------
plots:  
	root -l plot.C

#------------------------------------------------------------------------------
edit:  
	emacs tag.cxx tag.h tag_histo.h tag.opts plot.C 2> /dev/null &

#------------------------------------------------------------------------------
clean:
	@rm -f *.exe ./lib/*.o *.o *.so *.d core* tag_macro_h.* \
	output/*root output/*gif *~ */*~ $(OUTPUTLOG)

#------------------------------------------------------------------------------
zip:
	@make clean
	@rm -f   ../tagpackage.zip
	@zip -ry ../tagpackage.zip .
	@echo "Saved a zipped copy of code to" tagpackage.zip
	@ls -ohrt ../*.zip

#------------------------------------------------------------------------------
doxygen:
#	refresh doxygen documentation:
	@doxygen output/Doxyfile

#------------------------------------------------------------------------------
help:
	@firefox https://cern.ch/musy/tagging  2> /dev/null &

#------------------------------------------------------------------------------
#depend: $(SRCS)
#	$(DEPEND) $(SRCS)

#------------------------------------------------------------------------------

lib/tag.o: tag.cxx 
	$(COMPILE)
lib/CombineTaggersProbability.o: src/CombineTaggersProbability.cxx 
	$(COMPILE)
lib/CombineTaggersNN.o: src/CombineTaggersNN.cxx 
	$(COMPILE)
lib/CombineTaggersPID.o: src/CombineTaggersPID.cxx
	$(COMPILE)
lib/NNTuner.o: src/NNTuner.cxx 
	$(COMPILE)
lib/NNmuon.o: nnet/NNmuon.cxx 
	$(COMPILE)
lib/NNele.o: nnet/NNele.cxx 
	$(COMPILE)
lib/NNkaon.o: nnet/NNkaon.cxx 
	$(COMPILE)
lib/NNkaonS.o: nnet/NNkaonS.cxx 
	$(COMPILE)
lib/NNpionS.o: nnet/NNpionS.cxx 
	$(COMPILE)
lib/NNvtx.o: nnet/NNvtx.cxx 
	$(COMPILE)
lib/NNcomb.o: nnet/NNcomb.cxx 
	$(COMPILE)
lib/NNetTool_MLP.o: src/NNetTool_MLP.cxx 
	$(COMPILE)
lib/NtpTag.o: src/NtpTag.cxx 
	$(COMPILE)
lib/SVertexOneSeedTool.o: src/SVertexOneSeedTool.cxx 
	$(COMPILE)
lib/PerformanceMonitor.o: src/PerformanceMonitor.cxx 
	$(COMPILE)
lib/TaggerElectronTool.o: src/TaggerElectronTool.cxx 
	$(COMPILE)
lib/TaggerMuonTool.o: src/TaggerMuonTool.cxx 
	$(COMPILE)
lib/TaggerKaonOppositeTool.o: src/TaggerKaonOppositeTool.cxx 
	$(COMPILE)
lib/TaggerFragmentationOppositeTool.o: src/TaggerFragmentationOppositeTool.cxx 
	$(COMPILE)
lib/TaggerKaonSameTool.o: src/TaggerKaonSameTool.cxx 
	$(COMPILE)
lib/TaggerVertexChargeTool.o: src/TaggerVertexChargeTool.cxx 
	$(COMPILE)
lib/TaggerPionSameTool.o: src/TaggerPionSameTool.cxx 
	$(COMPILE)
lib/TaggerDZeroTool.o: src/TaggerDZeroTool.cxx 
	$(COMPILE)
lib/taggingutils.o: src/taggingutils.cxx 
	$(COMPILE)
lib/EventViewer.o: src/EventViewer.cxx 
	$(COMPILE)

#------------------------------------------------------------------------------
# DO NOT DELETE

lib/tag.o: Makefile
lib/tag.o: tag.h 
lib/tag.o: include/globals.h src/taggingutils.h
lib/tag.o: include/Event.h include/Particle.h include/Vertex.h
lib/tag.o: include/FlavourTag.h include/Tagger.h src/NNTuner.h
lib/tag.o: src/TaggerElectronTool.h include/GenericTool.h src/NNetTool_MLP.h
lib/tag.o: nnet/NNmuon.h nnet/NNele.h nnet/NNkaon.h nnet/NNkaonS.h nnet/NNpionS.h
lib/tag.o: nnet/NNvtx.h src/TaggerKaonOppositeTool.h src/TaggerKaonSameTool.h
lib/tag.o: nnet/NNcomb.h 
lib/tag.o: src/TaggerFragmentationOppositeTool.h 
lib/tag.o: src/TaggerMuonTool.h src/TaggerPionSameTool.h src/NtpTag.h
lib/tag.o: src/TaggerDZeroTool.h
lib/tag.o: src/EventViewer.h
lib/tag.o: src/PerformanceMonitor.h 
lib/tag.o: src/TaggerVertexChargeTool.h src/SVertexOneSeedTool.h
lib/tag.o: src/CombineTaggersProbability.h src/CombineTaggersPID.h
lib/tag.o: src/CombineTaggersNN.h tag_histo.h
lib/CombineTaggersNN.o: src/CombineTaggersNN.h include/Particle.h
lib/CombineTaggersNN.o: include/globals.h include/FlavourTag.h
lib/CombineTaggersNN.o: include/Tagger.h src/taggingutils.h
lib/CombineTaggersPID.o: src/CombineTaggersPID.h include/Particle.h
lib/CombineTaggersPID.o: include/globals.h include/FlavourTag.h
lib/CombineTaggersPID.o: include/Tagger.h src/taggingutils.h
lib/CombineTaggersProbability.o: src/CombineTaggersProbability.h
lib/CombineTaggersProbability.o: include/Particle.h include/globals.h
lib/CombineTaggersProbability.o: include/FlavourTag.h include/Tagger.h
lib/CombineTaggersProbability.o: src/taggingutils.h
lib/NNTuner.o: src/NNTuner.h include/Tagger.h include/Particle.h
lib/NNTuner.o: include/globals.h include/Event.h include/Vertex.h
lib/NNTuner.o: src/TaggerElectronTool.h include/GenericTool.h
lib/NNTuner.o: src/taggingutils.h src/NNetTool_MLP.h nnet/NNmuon.h
lib/NNTuner.o: nnet/NNele.h nnet/NNkaon.h nnet/NNkaonS.h nnet/NNpionS.h
lib/NNTuner.o: nnet/NNvtx.h nnet/NNcomb.h src/TaggerKaonOppositeTool.h
lib/NNTuner.o: src/TaggerKaonSameTool.h src/TaggerMuonTool.h
lib/NNTuner.o: src/TaggerPionSameTool.h
lib/NNTuner.o: src/TaggerDZeroTool.h
lib/NNetTool_MLP.o: src/NNetTool_MLP.h include/GenericTool.h
lib/NNetTool_MLP.o: src/taggingutils.h include/globals.h
lib/NNetTool_MLP.o: include/Particle.h nnet/NNmuon.h nnet/NNele.h
lib/NNetTool_MLP.o: nnet/NNkaon.h nnet/NNkaonS.h nnet/NNpionS.h nnet/NNvtx.h
lib/NNetTool_MLP.o: nnet/NNcomb.h 
lib/NtpTag.o: src/taggingutils.h include/globals.h src/NtpTag.h
lib/NtpTag.o: include/Event.h include/Particle.h include/Vertex.h
lib/PerformanceMonitor.o: src/PerformanceMonitor.h 
lib/PerformanceMonitor.o: include/FlavourTag.h
lib/PerformanceMonitor.o: include/Tagger.h include/Particle.h
lib/PerformanceMonitor.o: include/globals.h include/Event.h include/Vertex.h
lib/PerformanceMonitor.o: include/GenericTool.h src/taggingutils.h
lib/SVertexOneSeedTool.o: src/SVertexOneSeedTool.h include/Tagger.h
lib/SVertexOneSeedTool.o: include/Particle.h include/globals.h
lib/SVertexOneSeedTool.o: include/Event.h include/Vertex.h
lib/SVertexOneSeedTool.o: include/GenericTool.h src/taggingutils.h
lib/TaggerElectronTool.o: src/TaggerElectronTool.h include/Tagger.h
lib/TaggerElectronTool.o: include/Particle.h include/globals.h
lib/TaggerElectronTool.o: include/Event.h include/Vertex.h
lib/TaggerElectronTool.o: include/GenericTool.h src/taggingutils.h
lib/TaggerElectronTool.o: src/NNetTool_MLP.h nnet/NNmuon.h nnet/NNele.h
lib/TaggerElectronTool.o: nnet/NNkaon.h nnet/NNkaonS.h nnet/NNpionS.h
lib/TaggerElectronTool.o: nnet/NNvtx.h
lib/TaggerKaonOppositeTool.o: src/TaggerKaonOppositeTool.h include/Tagger.h
lib/TaggerKaonOppositeTool.o: include/Particle.h include/globals.h
lib/TaggerKaonOppositeTool.o: include/Event.h include/Vertex.h
lib/TaggerKaonOppositeTool.o: include/GenericTool.h src/taggingutils.h
lib/TaggerKaonOppositeTool.o: src/NNetTool_MLP.h nnet/NNmuon.h nnet/NNele.h
lib/TaggerKaonOppositeTool.o: nnet/NNkaon.h nnet/NNkaonS.h nnet/NNpionS.h
lib/TaggerKaonOppositeTool.o: nnet/NNvtx.h
lib/TaggerFragmentationOppositeTool.o: src/TaggerFragmentationOppositeTool.h include/Tagger.h
lib/TaggerFragmentationOppositeTool.o: include/Particle.h include/globals.h
lib/TaggerFragmentationOppositeTool.o: include/Event.h include/Vertex.h
lib/TaggerFragmentationOppositeTool.o: include/GenericTool.h src/taggingutils.h
lib/TaggerKaonSameTool.o: src/TaggerKaonSameTool.h include/Tagger.h
lib/TaggerKaonSameTool.o: include/Particle.h include/globals.h
lib/TaggerKaonSameTool.o: include/Event.h include/Vertex.h
lib/TaggerKaonSameTool.o: include/GenericTool.h src/taggingutils.h
lib/TaggerKaonSameTool.o: src/NNetTool_MLP.h nnet/NNmuon.h nnet/NNele.h
lib/TaggerKaonSameTool.o: nnet/NNkaon.h nnet/NNkaonS.h nnet/NNpionS.h
lib/TaggerKaonSameTool.o: nnet/NNvtx.h
lib/EventViewer.o: src/EventViewer.h include/GenericTool.h
lib/EventViewer.o: include/Event.h include/Particle.h include/Vertex.h 
lib/TaggerMuonTool.o: src/TaggerMuonTool.h include/Tagger.h
lib/TaggerMuonTool.o: include/Particle.h include/globals.h include/Event.h
lib/TaggerMuonTool.o: include/Vertex.h include/GenericTool.h
lib/TaggerMuonTool.o: src/taggingutils.h src/NNetTool_MLP.h nnet/NNmuon.h
lib/TaggerMuonTool.o: nnet/NNele.h nnet/NNkaon.h nnet/NNkaonS.h
lib/TaggerMuonTool.o: nnet/NNpionS.h nnet/NNvtx.h
lib/TaggerPionSameTool.o: src/TaggerPionSameTool.h include/Tagger.h
lib/TaggerPionSameTool.o: include/Particle.h include/globals.h
lib/TaggerPionSameTool.o: include/Event.h include/Vertex.h
lib/TaggerPionSameTool.o: include/GenericTool.h src/taggingutils.h
lib/TaggerPionSameTool.o: src/NNetTool_MLP.h nnet/NNmuon.h nnet/NNele.h
lib/TaggerPionSameTool.o: nnet/NNkaon.h nnet/NNkaonS.h nnet/NNpionS.h
lib/TaggerPionSameTool.o: nnet/NNvtx.h
lib/TaggerDZeroTool.o: src/TaggerDZeroTool.h include/Tagger.h
lib/TaggerDZeroTool.o: include/Particle.h include/globals.h
lib/TaggerDZeroTool.o: include/Event.h include/Vertex.h
lib/TaggerDZeroTool.o: include/GenericTool.h src/taggingutils.h
lib/TaggerDZeroTool.o: src/NNetTool_MLP.h 
lib/TaggerVertexChargeTool.o: src/TaggerVertexChargeTool.h include/Tagger.h
lib/TaggerVertexChargeTool.o: include/Particle.h include/globals.h
lib/TaggerVertexChargeTool.o: include/Event.h include/Vertex.h
lib/TaggerVertexChargeTool.o: include/GenericTool.h src/taggingutils.h
lib/TaggerVertexChargeTool.o: src/SVertexOneSeedTool.h src/NNetTool_MLP.h
lib/TaggerVertexChargeTool.o: nnet/NNmuon.h nnet/NNele.h nnet/NNkaon.h
lib/TaggerVertexChargeTool.o: nnet/NNkaonS.h nnet/NNpionS.h nnet/NNvtx.h
lib/taggingutils.o: include/globals.h src/taggingutils.h
lib/NNele.o: nnet/NNele.h
lib/NNkaon.o: nnet/NNkaon.h
lib/NNkaonS.o: nnet/NNkaonS.h
lib/NNmuon.o: nnet/NNmuon.h
lib/NNpionS.o: nnet/NNpionS.h
lib/NNvtx.o: nnet/NNvtx.h
lib/NNcomb.o: nnet/NNcomb.h
