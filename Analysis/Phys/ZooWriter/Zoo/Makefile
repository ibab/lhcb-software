# ROOT part
ROOTCONFIG = root-config
# get compiler version(s) used to compile ROOT
CXX = $(shell $(ROOTCONFIG) --cxx)
CXXso = $(shell $(ROOTCONFIG) --cxx)
FC = $(shell $(ROOTCONFIG) --f77)
CC = $(shell $(ROOTCONFIG) --cc)
LD = $(shell $(ROOTCONFIG) --ld)
TUNEFLAG = $(shell $(CXX) --version | \
	   awk '// { for (i = 1; i <= NF; ++i) if ($$i ~ /[0-9]*\.[0-9]*\.[0-9]*$$/) {if ($$i < "4.2") print "-mtune=opteron"; else print "-mtune=native"; }; }')
CXXFLAGS = -Wall -Wextra -O3 $(TUNEFLAG) $(shell $(ROOTCONFIG) --cflags)
LDFLAGS =
SOFLAGS = -fPIC -shared $(LDFLAGS)

# source and dictionary files
alldicts = ZooDict.cpp
allsrc = $(wildcard *.cpp) $(alldicts)

# general targets
all: dep libZooROOT.a libZooROOT.so

dep: dicts $(patsubst %.cpp,%.d,$(allsrc) $(alldicts))

dicts: $(alldicts)

clean: cleandicts
	rm -f *.o *.os *~

cleandicts:
	rm -f *Dict.{cpp,h}

cleandeps:
	rm -f *.d

distclean: cleandeps clean
	rm -f libZooROOT.so libZooROOT.a

# how to link libZooROOT.so
libZooROOT.so: $(patsubst %.cpp,%.os,$(allsrc))
	$(CXXso) $(SOFLAGS) $^ -o $@

libZooROOT.a: libZooROOT.a($(patsubst %.cpp,%.o,$(allsrc)))

ZooDict.cpp: $(filter-out ZooDict.h,$(wildcard Zoo*.h))

# General rules
%.d: %.cpp
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

%.os: %.cpp
	$(CXXso) -fPIC $(CXXFLAGS) -c $< -o $@

# ROOT rules (??? should I include them from somewhere in ROOT ???)
%Dict.cpp:
	$(shell $(ROOTCONFIG) --bindir)/rootcint -f $@ -c -p $(filter-out %LinkDef.h,$^) $(filter %LinkDef.h,$^) || rm -f $@
	mv $@ $@.tmp
	awk '/.*<.*>::fgIsA = 0/ { if (!($$0 ~ /^template *<>/)) $$0 = "template <> " $$0; } // { print; }' < $@.tmp > $@
	rm $@.tmp

# include dependencies
include $(patsubst %.cpp,%.d,$(allsrc))
