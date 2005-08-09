
.PHONEY: depend dep default clean t2 c2

default:
	make depend
	make c2
	# ./test > out.tmp


ROOTLIBS  = # -L$(ROOTSYS)/lib `rootlibs`

-include makefile.dep

##CLHEP_INC_DIR=/afs/cern.ch/sw/packages/CLHEP/1.8.0.0/rh73_gcc32/include
CLHEP_INC_DIR=/atlas/lester/usr/local/include
incdirs = .. /usr/X11R6/include ${CLHEP_INC_DIR}  #$(ROOTSYS)/include /mnt/oldlinux/oldroot/home/lester/1.8.2.0
incflags = $(incdirs:%=-I%)
spaceincflags = $(incdirs:%=-I %)

cppfiles = $(wildcard [ABCDEFGHIJKLMNOPQRSTUVWXYZ]*.cc) 
maincppfiles = $(wildcard [abcdefghijklmnopqrstuvwxyz]*.cc)
objects = $(cppfiles:%.cc=%.o) 
hfiles = $(wildcard *.h)
CCFLAGS =  -Wall -g -O2 
CCFLAGS +=   -DLESTER_USE_GRAPHICS 


##CCFLAGS =  -Wall  -DNDEBUG -O4
##LDFLAGS = $(ROOTLIBS) -L${CLHEP_LIB_DIR} -ldl -lCLHEP -lgmp -lgmpxx -lGL
#LDFLAGS = $(ROOTLIBS) -L${CLHEP_LIB_DIR} -ldl -lCLHEP -lGL
LDFLAGS = $(ROOTLIBS) -L${CLHEP_LIB_DIR} -L/atlas/lester/usr/local/lib/justCLHEP -ldl -lCLHEP -lGL -L/atlas/lester/usr/local/lib -lGLU

%.o : %.cc
	g++ -O2 -c $< $(incflags) $(CCFLAGS)

%.o: %.f
	gcc -O2 -c $^ -I${HOME}/proj 

dep depend:
	make makefile.dep

makefile.dep: $(cppfiles) $(hfiles) $(maincppfiles) Makefile
	touch makefile.dep.tmp
	makedepend $(spaceincflags) -I /usr/include/g++-3 -f makefile.dep.tmp $(cppfiles) $(maincppfiles)
	#cat makefile.dep.tmp | grep -v ' /usr/\(lib\)\|\(include\)' > makefile.dep
	cat makefile.dep.tmp > makefile.dep
	rm -f makefile.dep.tmp{,.bak}

clean:
	rm -f *.o *~ core.* *.eps crudeSampler

crudeSampler: ThreePointCircleProposer.o  NewCircleProposer.o CircleParams.o crudeSampler.o RichModels.o Rich.o RichParams.o ThreePointCircleProposerB.o RectilinearCPQuantizer.o GraphicsObjects.o
	g++ -o $@ $^ $(incflags) $(CCFLAGS) $(LDFLAGS)

c2:
	make crudeSampler
	./crudeSampler `increment`
