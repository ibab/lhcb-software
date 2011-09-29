TARGETS = ZooDict.cpp libZooROOT.a libZooROOT.so

include Rules.mk

# make sure we include the dictionary when linking
alldicts += ZooDict.cpp

# what belongs in which file
ZooDict.cpp: $(patsubst %.cpp,%.h,$(allsrc)) ZooLinkDef.h
libZooROOT.a: libZooROOT.a($(patsubst %.cpp,%.o,$(allsrc) $(alldicts)))
libZooROOT.so: $(patsubst %.cpp,%.os,$(allsrc) $(alldicts))
