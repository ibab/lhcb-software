TARGETS = ZooDict.cpp libZooROOT.a libZooROOT.so
# make sure we include the dictionary when linking
alldicts += ZooDict.cpp

include Rules.mk

# what belongs in which file
ZooDict.cpp:   $(filter-out %Dict.h,$(allsrc:%.cpp=%.h)) ZooLinkDef.h
libZooROOT.a:  libZooROOT.a($(alldicts:%.cpp=%.o) $(allsrc:%.cpp=%.o))
libZooROOT.so: $(alldicts:%.cpp=%.os) $(allsrc:%.cpp=%.os)
