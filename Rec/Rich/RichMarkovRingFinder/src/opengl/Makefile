
.PHONEY: depend dep default clean

default:
	make canvasTest
	make StreamCanvas

#-include makefile.dep

incdirs = ${HOME}/proj
incflags = $(incdirs:%=-I%)
spaceincflags = $(incdirs:%=-I %)

cfiles = $(wildcard *.c) 
cppfiles = $(wildcard *.cpp) 
cobjects = $(cfiles:%.c=%.o) 
cppobjects = $(cppfiles:%.cpp=%.o) 
objects = $(cobjects) $(cppobjects)
hfiles = $(wildcard *.h)
CCFLAGS =  -Wall -g -I.. -I/atlas/lester/usr/local/include -I/atlas/lester/usr/local/include/boost-1_31
CPPFLAGS =  $(CCFLAGS)

OPENGLLDFLAGS = -L/atlas/lester/usr/local/lib -lGL -lGLU -lm
LDFLAGS = $(OPENGLLDFLAGS)

ex1: ex1.o
	gcc -o $@ $^ $(OPENGLLDFLAGS)

%.o : %.cpp
	g++ -c $< $(incflags) $(CCFLAGS)

%.o : %.c
	gcc -c $< $(incflags) $(CCFLAGS)

dep depend:
	make makefile.dep

makefile.dep: $(cfiles) $(cppfiles) $(hfiles) Makefile
	touch makefile.dep.tmp
	makedepend $(spaceincflags) -I /usr/include/g++-3 -f makefile.dep.tmp *.cpp  $(CCFLAGS) -pthread
	#cat makefile.dep.tmp | grep -v ' /usr/\(lib\)\|\(include\)' > makefile.dep
	cat makefile.dep.tmp > makefile.dep
	rm -f makefile.dep.tmp

clean:
	rm -f *.o *~ core.*

canvasTest: canvasTest.c++ Canvas.cpp
	g++ -o $@ $^ $(LDFLAGS) $(CPPFLAGS)

ostreamBufferCanvasTest: ostreamBufferCanvasTest.cpp
	g++ -o $@ $^ $(LDFLAGS) $(CPPFLAGS)

StreamCanvas: CPPFLAGS += -pthread
StreamCanvas: LDFLAGS += -lboost_thread-gcc-mt -L/atlas/lester/usr/local/lib -lCLHEP

StreamCanvas: $(wildcard *.h) $(wildcard *.cpp)

StreamCanvas:
	g++ -o $@ StreamCanvas.cpp $(LDFLAGS) $(CPPFLAGS)
