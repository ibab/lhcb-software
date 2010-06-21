# Basic Makefile 

LIBS = -L${ROOTSYS}/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic  -lTreePlayer -lGpad -lMLP -lXMLIO -lMinuit -lTMVA 
INC  = -I${ROOTSYS}/include
OPTS = -fPIC -Wall
EXEC = tag.exe

all: 
	g++ tag.cc $(OPTS) -o tag.exe $(LIBS) $(INC)
	./$(EXEC)
	root -l tag.C

run: tag.exe
	./$(EXEC)
	root -l tag.C

perf: tag.exe
	./$(EXEC)

tag.exe: tag.cc
	g++ tag.cc $(OPTS) -o $(EXEC) $(LIBS) $(INC)

plots:
	root -l tag.C

clean:
	rm -f $(EXEC) tag.root tag_h.so tag_h.d

root:
	source `which SetupProject.csh` Gaudi ROOT

