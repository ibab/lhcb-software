#######################################################################
# Rules.mk
#
# try to help physicists avoid writing Makefiles
#
# release history
# v0.1	2011-09-28 Manuel Schiller <manuel.schiller@nikhef.nl>
#
# todo
# - tested my examples, wait for feedback from other people
# - check parallel build safety
#######################################################################

#######################################################################
# usage:
#
# typical usage is as follows:
# 1. specify TARGETS, e.g.
# 
#	TARGETS = MyAnalysis test
#
# 2. if the current directory has subdirectories to be built, specify
#    them with SUBDIRS, e.g.
#
#	SUBDIRS = Zoo Foo bar baz
#
# 3. include Rules.mk
#
# 	include Rules.mk
#
# 4. specify how to build each target (if you just have to take the
#    corresponding object file and link it, you don't have to do
#    anything), e.g.
#
#	MyAnalysis: MyAnalysis.o flummi.o f77crapcode.o
#	# test just needs test.o, so we don't do anything
#
# 5. if neccessary, override tools/flags to build targets, e.g.
#
# 	# MyAnalysis should link against ZooROOT, Foo, and bar
# 	MyAnalysis: LDLIBS += -LZoo -lZooROOT -LFoo -lFoo -Lbar -lbar
# 	# since MyAnalysis is C++ code, we use $(CXX) to link it
# 	# because that will automatically include the C++ libs
# 	MyAnalysis: CC=$(CXX)
#
# while this will certainly not cover every concievable case, the
# majority of ROOT-based codes should be fine (depending on what you
# use, you may need to add a few libraries as shown above)...
#
# Rules.mk should understand the following targets:
#
# 	all		build everything
# 	strip		strip debugging info from all targets
# 	dep		calculate dependencies
# 	clean		clean object and backup files (but not targets)
# 	distclean	clean everything
#
# Note: When building ROOT dictionaries, Rules.mk assumes that C++
#       header files end in .h
#
#######################################################################

#######################################################################
# setup toolchain and flags
#
# try to provide reasonable defaults
#
# these can all be set from the command line, if needed, and overriding
# on a per-target basis is also possible (see above)
#######################################################################
# use ROOT config to get reasonable defaults for current environment
ROOTCONFIG ?= root-config
# get compiler version(s) used to compile ROOT
CXX ?= $(shell $(ROOTCONFIG) --cxx)
ifneq ($(strip $(shell $(ROOTCONFIG) --help|tr ' ' '\n'|grep -- '--f77')),)
ifneq ($(FC),/usr/bin/f77)
# if FC points to /usr/bin/f77 (which is usually f2c in disguise), we
# just take whatever ROOT used, because f2c s*cks...
FC = $(shell $(ROOTCONFIG) --f77)
else
FC ?= $(shell $(ROOTCONFIG) --f77)
endif
endif
CC ?= $(shell $(ROOTCONFIG) --cc)
CPP ?= $(CC) -E
LD ?= $(shell $(ROOTCONFIG) --ld)
AWK ?= $(shell which awk)
SED ?= $(shell which sed)
TUNEFLAG ?= $(shell $(CXX) --version | $(AWK) '// { for (i = 1; \
	    i <= NF; ++i) if ($$i ~ /[0-9]*\.[0-9]*\.[0-9]*$$/) { \
	    if ($$i < "4.") print "-mtune=opteron"; else print \
	    "-mtune=native"; }; }')
CFLAGS ?= -pipe -O2 $(TUNEFLAG) -g -Wall -Wextra -pedantic -fmessage-length=78
CXXFLAGS ?= -pipe -O2 -g -Wall -Wextra $(TUNEFLAG) \
	    $(shell $(ROOTCONFIG) --auxcflags) \
	    -DBOOST_DISABLE_ASSERTS -fmessage-length=78
CPPFLAGS ?= -I$(shell $(ROOTCONFIG) --incdir)
FFLAGS ?= -pipe -O2 $(TUNEFLAG) -Wall -Wextra -pedantic -g -std=legacy
# assemble ROOT libraries
ROOTLIBDIR ?= $(shell $(ROOTCONFIG) --libdir)
# default set of ROOT libraries
ROOTLIBS ?= $(shell $(ROOTCONFIG) --libs)
# some versions of ROOT have libGenVector, others libMathCore
LIBGENVECTOR ?= $(shell test -e $(ROOTLIBDIR)/libGenVector.so && echo \
		-lGenVector || echo -lMathCore)

LDFLAGS ?= -rdynamic
# flags to link and produce shared libs
SHFLAGS ?= -shared

# export these to make subprocesses
export ROOTCONFIG CC CXX FC LD CFLAGS CXXFLAGS FFLAGS LDFLAGS ROOTLIBS \
    ROOTLIBDIR LIBGENVECTOR SHFLAGS AWK SED CPP LDLIBS LOADLIBES

#######################################################################
# collect source files in current directory
#######################################################################
# C sources
csrc = $(wildcard *.c)
# C++ sources (not all variations in use)
ccsrc = $(wildcard *.cc) $(wildcard *.C) $(wildcard *.cpp) \
	$(wildcard *.cxx) $(wildcard *.c++)
# ROOT dictionaries
alldicts = $(wildcard *Dict.cc) $(wildcard *Dict.C) $(wildcard *Dict.cpp) \
	   $(wildcard *Dict.cxx) $(wildcard *Dict.c++)
# Fortran sources (not all variations)
f77src = $(wildcard *.[fF])
# dependency files (.d) for make
alldeps = $(patsubst %.c,%.d,$(csrc)) $(patsubst %.cc,%.d,$(wildcard *.cc)) \
	  $(patsubst %.C,%.d,$(wildcard *.C)) \
	  $(patsubst %.cpp,%.d,$(wildcard *.cpp)) \
	  $(patsubst %.cxx,%.d,$(wildcard *.cxx)) \
	  $(patsubst %.c++,%.d,$(wildcard *.c++))
# source files without dictionaries
allsrc = $(filter-out $(alldicts), $(csrc) $(ccsrc) $(f77src))

#######################################################################
# general targets
#
# this is the logic driving the build process
#######################################################################
.PHONY: all dep clean local-all local-dep local-clean distclean \
    local-distclean doxy subdirs-all subdirs-dep subdirs-clean \
    subdirs-distclean strip strip-local subdirs-strip
ifneq ($(strip $(SUBDIRS)),)
.PHONY: $(SUBDIRS)-all $(SUBDIRS)-dep $(SUBDIRS)-clean \
    $(SUBDIRS)-distclean $(SUBDIRS)-strip
endif

all:
	$(MAKE) dep
	$(MAKE) subdirs-all
	$(MAKE) local-all

dep: subdirs-dep local-dep

strip: subdirs-strip local-strip

clean: subdirs-clean local-clean

local-all: $(TARGETS)

local-dep: $(alldeps)

local-strip:
	strip $(TARGETS)

ifeq ($(strip $(filter %.so,$(TARGETS))),)
local-clean:
	rm -f *.o *~
else
local-clean:
	rm -f *.o *.os *~
endif

distclean: subdirs-distclean local-distclean local-clean

local-distclean:
	rm -f *.d $(TARGETS)

doxy: $(allsrc) Doxyfile
	doxygen Doxyfile

ifneq ($(strip $(SUBDIRS)),)
subdirs-all: $(SUBDIRS)-all
subdirs-dep: $(SUBDIRS)-dep
subdirs-clean: $(SUBDIRS)-clean
subdirs-distclean: $(SUBDIRS)-distclean
subdirs-strip: $(SUBDIRS)-strip

$(SUBDIRS)-all:
	$(MAKE) -C $(patsubst %-all,%,$@) all
$(SUBDIRS)-dep:
	$(MAKE) -C $(patsubst %-dep,%,$@) dep
$(SUBDIRS)-clean:
	$(MAKE) -C $(patsubst %-clean,%,$@) clean
$(SUBDIRS)-distclean:
	$(MAKE) -C $(patsubst %-distclean,%,$@) distclean
$(SUBDIRS)-strip:
	$(MAKE) -C $(patsubst %-strip,%,$@) strip
else
subdirs-all:
subdirs-dep:
subdirs-clean:
subdirs-distclean:
subdirs-strip:
endif

#######################################################################
# General rules
#
# this section should contain (generalized) rules for most build tasks,
# we rely on GNU make to do something sensible in most cases
#######################################################################
# automatic prerequisites (inter-source file dependencies)
%.d: %.c
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
		$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$
%.d: %.C
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
		$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$
%.d: %.cc
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
		$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$
%.d: %.cpp
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
		$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$
%.d: %.cxx
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
		$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$
%.d: %.c++
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
		$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

# building position independent code for shared libraries
%.os: CXXFLAGS += -fPIC
# rules to build object files for shared libraries (we need them because
# we changed the default ending .o to .os)
%.os: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
%.os: %.C
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
%.os: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
%.os: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
%.os: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
%.os: %.c++
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
%.os: %.f
	$(FC) $(FFLAGS) -c $< -o $@
%.os: %.F
	$(FC) $(CPPFLAGS) $(FFLAGS) -c $< -o $@
# these are less standard and may be missing
%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
%.o: %.c++
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# building shared libraries from .os files
# (the user is required to say which .os files should go in)
%.so: LDFLAGS += $(SHFLAGS)
%.so:
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

# ROOT dictionaries
#
# some versions of ROOT ship a buggy rootcint, so we postprocess with awk
ROOTCINT_PP = $(AWK) '/.*<.*>::fgIsA = 0/ { if (!($$0 ~ /^template *<>/)) \
    $$0 = "template <> " $$0; } // { print; }'
%Dict.C:
	$(shell $(ROOTCONFIG) --bindir)/rootcint -f $@ -c -p \
	    $(filter-out %LinkDef.h,$^) $(filter %LinkDef.h,$^) || \
	    rm -f $@ $(patsubst %.C,%.h,$@)
	mv -f $@ $@.tmp || rm -f $@ $(patsubst %.C,%.h,$@)
	$(ROOTCINT_PP) < $@.tmp > $@ || rm -f $@.tmp $(patsubst %.C,%.h,$@)
	rm -f $@.tmp
%Dict.cc:
	$(shell $(ROOTCONFIG) --bindir)/rootcint -f $@ -c -p \
	    $(filter-out %LinkDef.h,$^) $(filter %LinkDef.h,$^) || \
	    rm -f $@ $(patsubst %.cc,%.h,$@)
	mv -f $@ $@.tmp || rm -f $@ $(patsubst %.cc,%.h,$@)
	$(ROOTCINT_PP) < $@.tmp > $@ || rm -f $@.tmp $(patsubst %.cc,%.h,$@)
	rm -f $@.tmp
%Dict.cpp:
	$(shell $(ROOTCONFIG) --bindir)/rootcint -f $@ -c -p \
	    $(filter-out %LinkDef.h,$^) $(filter %LinkDef.h,$^) || \
	    rm -f $@ $(patsubst %.cpp,%.h,$@)
	mv -f $@ $@.tmp || rm -f $@ $(patsubst %.cpp,%.h,$@)
	$(ROOTCINT_PP) < $@.tmp > $@ || rm -f $@.tmp $(patsubst %.cpp,%.h,$@)
	rm -f $@.tmp
%Dict.cxx:
	$(shell $(ROOTCONFIG) --bindir)/rootcint -f $@ -c -p \
	    $(filter-out %LinkDef.h,$^) $(filter %LinkDef.h,$^) || \
	    rm -f $@ $(patsubst %.cxx,%.h,$@)
	mv -f $@ $@.tmp || rm -f $@ $(patsubst %.cxx,%.h,$@)
	$(ROOTCINT_PP) < $@.tmp > $@ || rm -f $@.tmp $(patsubst %.cxx,%.h,$@)
	rm -f $@.tmp
%Dict.c++:
	$(shell $(ROOTCONFIG) --bindir)/rootcint -f $@ -c -p \
	    $(filter-out %LinkDef.h,$^) $(filter %LinkDef.h,$^) || \
	    rm -f $@ $(patsubst %.c++,%.h,$@)
	mv -f $@ $@.tmp || rm -f $@ $(patsubst %.c++,%.h,$@)
	$(ROOTCINT_PP) < $@.tmp > $@ || rm -f $@.tmp $(patsubst %.c++,%.h,$@)
	rm -f $@.tmp

#######################################################################
# include dependencies
#######################################################################
include $(alldeps)

# vim: tw=78:sw=4:ft=Makefile
