# ROOT part
ROOTCONFIG = root-config
# get compiler version(s) used to compile ROOT
CXX = $(shell $(ROOTCONFIG) --cxx)
CXXso = $(shell $(ROOTCONFIG) --cxx)
FC = $(shell $(ROOTCONFIG) --f77)
CC = $(shell $(ROOTCONFIG) --cc)
LD = $(shell $(ROOTCONFIG) --ld)
TUNEFLAG = $(shell $(CXX) --version | \
	   awk '// { for (i = 1; i <= NF; ++i) if ($$i ~ /[0-9]*\.[0-9]*\.[0-9]*$$/) {if ($$i < "4.") print "-mtune=opteron"; else print "-mtune=native"; }; }')
CXXFLAGS = -O2 -g -Wall -Wextra $(TUNEFLAG)\
	   $(shell $(ROOTCONFIG) --cflags) -I$(shell pwd) \
	   -DBOOST_DISABLE_ASSERTS -fmessage-length=78 \
	   -DCOMBBGPARABOLA -g
CPPFLAGS= $(shell $(ROOTCONFIG) --cflags)
FFLAGS = -O2 $(TUNEFLAG) -Wall -Wextra -pedantic -g -std=legacy
# assemble root libraries
ROOTLIBDIR = $(shell $(ROOTCONFIG) --libdir)
# some versions of ROOT have libGenVector, others libMathCore
LIBGENVECTOR = $(shell test -e $(ROOTLIBDIR)/libGenVector.so && echo -lGenVector || echo -lMathCore)
#LDFLAGS = -O2 $(shell $(ROOTCONFIG) --libs) $(LIBGENVECTOR) -lMinuit -lstdc++ -lboost_regex -rdynamic -L../Zoo -lZooROOT
LDFLAGS = -O2 $(shell $(ROOTCONFIG) --libs) $(LIBGENVECTOR) -lMinuit -static-libgcc -lboost_regex -rdynamic -L../Zoo -lZooROOT

# source files
ccsrc = $(wildcard *.cc)
f77src = $(wildcard *.f)
allsrc = $(ccsrc) $(f77src)
SUBDIRS = Zoo
TARGETS = ZooReader ZooOptionsReader

# general targets
all: dep subdirs $(TARGETS)

dep: $(patsubst %.cc,%.d,$(allsrc) $(alldicts))

clean: subdirsclean
	rm -f *.o *~

distclean: clean subdirsdistclean
	rm -f *.d $(TARGETS)

doxy: $(allsrc) Doxyfile
	doxygen Doxyfile

.PHONY: subdirs subdirsclean subdirsdistclean $(SUBDIRS)

subdirs: $(SUBDIRS)
	for i in $<; do $(MAKE) -C $$i || exit $$?; done
subdirsclean: $(SUBDIRS)
	for i in $<; do $(MAKE) -C $$i clean || exit $$?; done
subdirsdistclean: $(SUBDIRS)
	for i in $<; do $(MAKE) -C $$i distclean || exit $$?; done


# General rules
%.d: %.cc
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

# include dependencies
include $(patsubst %.cc,%.d,$(ccsrc))
