#######################################################################
# Rules.mk
#
# try to help physicists avoid writing Makefiles
#
# release history
# v0.1	2011-09-28 Manuel Schiller <manuel.schiller@nikhef.nl>
# 	initial release
# v0.2	2011-09-30 Manuel Schiller <manuel.schiller@nikhef.nl>
# 	fixed problem with multiple subdirectories in build
# v0.3	2011-11-03 Manuel Schiller <manuel.schiller@nikhef.nl>
# 	modularise so that we have a chance to become more compiler
# 	independent in the future
# v0.4 2011-11-08 Manuel Schiller <manuel.schiller@nikhef.nl>
# 	further modularise and beautify output using templates,
# 	generating pattern rules programmatically
# v0.5 2011-11-11 Manuel Schiller <manuel.schiller@nikhef.nl>
# 	change some make variables from recursively expanded to simple
# 	expanded to avoid reevaluation of functions and calls to the
# 	shell (saves quite a bit of time)
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
# While this will certainly not cover every concievable case, the
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
# Note: If you need to see exactly what is being executed, you can
#       append "COLORMAKE=" to the make command, e.g.
#
#       "make COLORMAKE= all".
#       
#       For most of the time, the full output just clutters your
#       terminal, and you will be fine with the slightly shortened
#       colorized version (which is activated by setting COLORMAKE=YES).
#######################################################################

#######################################################################
# setup toolchain
#
# try to provide reasonable defaults
#
# these can all be set from the command line, if needed, and overriding
# on a per-target basis is also possible (see above)
#######################################################################
# use ROOT config to get reasonable defaults for current environment
# (we use it to detect the toolchain used to build ROOT)
ROOTCONFIG ?= root-config

# other tools used in the build process
AWK ?= awk
SED ?= sed
ECHO ?= /bin/echo
CP ?= cp
MV ?= mv
RM ?= rm
MKDIR ?= mkdir
RMDIR ?= rmdir
TEST ?= test
TRUE ?= true
FALSE ?= false
TOUCH ?= touch

# get compiler version(s) used to compile ROOT
CC ?= $(shell $(ROOTCONFIG) --cc)
CXX ?= $(shell $(ROOTCONFIG) --cxx)
LD ?= $(shell $(ROOTCONFIG) --ld)
CPP ?= $(CC) -E
ROOTCONFIG_HASF77 := $(strip $(shell $(ROOTCONFIG) --help|tr ' ' '\n'|grep -- '--f77'))
ifneq ($(ROOTCONFIG_HASF77),)
ifneq ($(FC),/usr/bin/f77)
# if FC points to /usr/bin/f77 (which is usually f2c in disguise), we
# just take whatever ROOT used, because f2c s*cks...
FC = $(shell $(ROOTCONFIG) --f77)
else
FC ?= $(shell $(ROOTCONFIG) --f77)
endif
endif
# static libraries
AR ?= ar
RANLIB ?= ranlib
# to remove debugging symbols
STRIP ?= strip

# in case this did not work, we fall back to cc, c++, ld, cpp, f77
CC ?= cc
CXX ?= c++
LD ?= ld
CPP ?= cpp
FC ?= f77

# tune for native processor, but generate generic code (works different in
# different versions of gcc/g++)
TUNEFLAG ?= $(shell $(CXX) --version 2>&1 | $(AWK) '// { for (i = 1; \
	    i <= NF; ++i) if ($$i ~ /[0-9]*\.[0-9]*\.[0-9]*$$/) { \
	    if ($$i < "4.") tunefl = "-mtune=opteron"; else tunefl = \
	    "-mtune=native"; }; } END { print tunefl; }')

# assemble ROOT libraries
ROOTLIBDIR ?= $(shell $(ROOTCONFIG) --libdir)
# default set of ROOT libraries
ROOTLIBS ?= $(shell $(ROOTCONFIG) --libs)
# default ROOT compiler flags
ROOTINCLUDES ?= -I$(shell $(ROOTCONFIG) --incdir)
ROOTCFLAGS ?= $(shell $(ROOTCONFIG) --auxcflags)
# some versions of ROOT have libGenVector, others libMathCore
LIBGENVECTOR ?= $(shell $(TEST) -e $(ROOTLIBDIR)/libGenVector.so && $(ECHO) \
		-lGenVector || $(ECHO) -lMathCore)

#######################################################################
# freeze toolchain variables
#
# "freeze" the following variables at their current values, i.e. demote
# them from recursively expanded variables to singly expanded variables
#
# this saves make from the trouble of having to call the shell every
# time to evaluate certain variables, e.g. $(CXX) would get expanded to
# $(shell root-config --cxx) which involves spawning an unneccessary
# shell process for every compiler invocation...
#
# we do not do that for CFLAGS and friends since we want to keep the
# recursively expanded semantics so we can override the variables which
# make up CFLAGS on a per target basis and have the CFLAGS used to
# compile the target use our overrides
#######################################################################
ROOT_CONFIG := $(ROOTCONFIG)
CPP := $(CPP)
CC := $(CC)
CXX := $(CXX)
FC := $(FC)
LD := $(LD)
AR := $(AR)
RANLIB := $(RANLIB)
STRIP := $(STRIP)
AWK := $(AWK)
SED := $(SED) 
ECHO := $(ECHO)
CP := $(CP)
MV := $(MV)
RM := $(RM)
MKDIR := $(MKDIR)
RMDIR := $(RMDIR)
TEST := $(TEST)
TRUE := $(TRUE)
FALSE := $(FALSE)
TOUCH := $(TOUCH)
TUNEFLAG := $(TUNEFLAG)
ROOTLIBDIR := $(ROOTLIBDIR)
ROOTLIBS := $(ROOTLIBS)
ROOTINCLUDES := $(ROOTINCLUDES)
ROOTCFLAGS := $(ROOTCFLAGS)
LIBGENVECTOR := $(LIBGENVECTOR)

#######################################################################
# compiler flags
#
# try to provide reasonable defaults for most HEP users
#######################################################################
# build default skeletal set of compiler flags
CFLAGS ?= $(PIPEFLAG) $(STDOPTFLAGS) $(TUNEFLAG) $(WARNFLAGS) \
	  $(STDDEBUGFLAGS) $(CSTD)
CXXFLAGS ?= $(PIPEFLAG) $(STDOPTFLAGS) $(TUNEFLAG) $(WARNFLAGS) \
	    $(STDDEBUGFLAGS) $(CXXSTD) $(ROOTCFLAGS)
FFLAGS ?= $(PIPEFLAG) $(STDOPTFLAGS) $(TUNEFLAG) $(WARNFLAGS) \
	  $(STDDEBUGFLAGS) $(FSTD)

LDFLAGS += -rdynamic
# flags to link and produce shared libs
SHFLAGS ?= -shared
# flags needed to produce dependency files
CPPDEPFLAGS ?= -MM
# produce nicer warning messages
WARNFLAGS ?= -Wall -Wextra -fmessage-length=78
# produce position independent code
PICFLAGS ?= -fPIC
# standard optimization level, no debugging code
STDOPTFLAGS ?= -Os # -DBOOST_DISABLE_ASSERTS -DNDEBUG
# standard debugging flags: write symbol information
STDDEBUGFLAGS ?= -g
# use pipes instead of temp files
PIPEFLAG ?= -pipe
# set language standard(s) - should encourage users to write clean code
CSTD ?= -std=c9x -pedantic
CXXSTD ?= -std=gnu++98
FSTD ?= -std=legacy -pedantic

#######################################################################
# colorful make messages
#
# set COLORMAKE to anything but "YES" to get the default make behaviour
#######################################################################
COLORMAKE ?= YES
ifeq ($(COLORMAKE),YES)
# we use our own rules, and want make a little less chatty
MAKEFLAGS += -rs
ECHOMSG := $(ECHO) -e
else
MAKEFLAGS += -r
ECHOMSG := $(TRUE)
endif

#######################################################################
# compiler specific flags
#
# the idea is that you can override values here which do not work
# if you use some special compiler (and not gcc/g++ and friends)
#######################################################################

#######################################################################
# export these variables to make subprocesses
#
# advantage is that make subprocesses will not have to reevaluate a
# large portion of these variables
#######################################################################
# toolchain (compiler, linker)
export CPP CC CXX FC LD AR RANLIB
# compiler/linker flags
export CFLAGS CXXFLAGS FFLAGS CPPFLAGS \
    LDFLAGS SHFLAGS LDLIBS LOADLIBES \
    CPPDEPFLAGS WARNFLAGS PICFLAGS STDOPTFLAGS STDDEBUGFLAGS PIPEFLAG \
    CSTD CXXSTD FSTD
# ROOT libs, compiler flags etc
export ROOTCONFIG ROOTLIBS ROOTLIBDIR ROOTINCLUDES ROOTCFLAGS LIBGENVECTOR
# *nix like tools for text processing, copying/moving files etc.
export AWK SED CPP CP MV MKDIR RMDIR TEST ECHO TRUE FALSE TOUCH
# communicate make(1) related options to subprocesses
export MAKEFLAGS COLORMAKE

#######################################################################
# collect source files in current directory
#
# we append here to whatever the user has prelisted (because the user
# ultimately knows best)
#######################################################################
# C sources
csrc += $(wildcard *.c)
# ROOT dictionaries
alldicts += $(wildcard *Dict.cc) $(wildcard *Dict.C) $(wildcard *Dict.cpp) \
	   $(wildcard *Dict.cxx) $(wildcard *Dict.c++)
# C++ sources (not all variations in use)
ccsrc += $(sort $(wildcard *.cc) $(wildcard *.C) $(wildcard *.cpp) \
	$(wildcard *.cxx) $(wildcard *.c++) $(alldicts))
# make sure the dictionaries are among the C++ sources
ccsrc.C = $(filter %.C,$(ccsrc))
ccsrc.cc = $(filter %.cc,$(ccsrc))
ccsrc.cpp = $(filter %.cpp,$(ccsrc))
ccsrc.cxx = $(filter %.cxx,$(ccsrc))
ccsrc.c++ = $(filter %.c++,$(ccsrc))
# Fortran sources (not all variations)
f77src += $(wildcard *.[fF])
# dependency files (.deps/*.d and .deps/*.dd) for make
alldeps += $(csrc:%.c=.deps/%.d) $(ccsrc.C:%.C=.deps/%.dd) \
    $(ccsrc.cc:%.cc=.deps/%.dd) $(ccsrc.cpp:%.cpp=.deps/%.dd) \
    $(ccsrc.cxx:%.cxx=.deps/%.dd) $(ccsrc.c++:%.c++=.deps/%.dd)
# source files without dictionaries
allsrc += $(filter-out $(alldicts),$(sort $(csrc) $(ccsrc) $(f77src)))

#######################################################################
# general targets
#
# this is the logic driving the build process
#######################################################################
.PHONY: all dep clean local-all local-dep local-clean \
    distclean local-distclean do-local-distclean doxy \
    subdirs-all subdirs-dep subdirs-clean subdirs-distclean \
	strip strip-local subdirs-strip

# propagate all/dep/clean/strip/distclean to subdirectories
all:: subdirs-all
	$(MAKE) local-all

dep:: subdirs-dep local-dep

strip:: subdirs-strip local-strip

clean:: subdirs-clean local-clean

distclean:: subdirs-distclean local-distclean

local-all:: subdirs-all local-dep $(TARGETS)

local-dep:: .deps/dummy.txt $(alldeps)
$(alldeps): .deps/dummy.txt
.deps/dummy.txt:
	$(TEST) -d .deps || ($(ECHOMSG) "\x1b[32m[RMDIR]\x1b[m\t\t"`pwd`/.deps; $(RM) -fr .deps)
	@ $(ECHOMSG) "\x1b[32m[MKDIR]\x1b[m\t\t"`pwd`/.deps
	$(MKDIR) -p .deps
	$(TOUCH) .deps/dummy.txt

local-strip::
	$(STRIP) $(TARGETS)

ifeq ($(strip $(filter %.so,$(TARGETS))),)
local-clean::
	@ $(ECHOMSG) "\x1b[32m[CLEAN]\x1b[m"
	$(RM) -f *.o *~
else
local-clean::
	@ $(ECHOMSG) "\x1b[32m[CLEAN]\x1b[m"
	$(RM) -f *.o *.os *~
endif

local-distclean:: .deps local-clean do-local-distclean .deps
do-local-distclean::
	@ $(ECHOMSG) "\x1b[32m[DISTCLEAN]\x1b[m"
	$(RM) -f $(TARGETS)
	$(RM) -f .deps/*.d .deps/*.dd .deps/*.d.[0-9]* .deps/*.dd.[0-9]* .deps/dummy.txt
	@ $(ECHOMSG) "\x1b[32m[RMDIR]\x1b[m\t\t"`pwd`/.deps;
	$(RM) -fr .deps
	$(TEST) -d html -a -f Doxyfile && $(RM) -fr html || true

doxy:: $(allsrc) Doxyfile
	doxygen Doxyfile

ifneq ($(strip $(SUBDIRS)),)
# template rule for recursing into subdirectories for some rules
MAKEMSG := "\\x1b[34m[MAKE]\\x1b[m\\t\\t"
define SUBDIRRULE_TEMPLATE
.PHONY: $(2:%=%-$(1))
subdirs-$(1):: $(2:%=%-$(1))
$(2:%=%-$(1))::
	@ $$(ECHOMSG) $$(MAKEMSG)"\x1b[1;34mEntering "`pwd`/$(2)"\x1b[m"
	$$(MAKE) -C $(2) $(1)
	@ $$(ECHOMSG) $$(MAKEMSG)"\x1b[1;34mLeaving "`pwd`/$(2)"\x1b[m"
endef
# targets which work by descending into subdirectories
SUBDIRRULES = all dep clean distclean strip
# synthesize rules to descend into subdirectories for these targets
$(foreach subdirrule,$(SUBDIRRULES),$(eval \
    $(call SUBDIRRULE_TEMPLATE,$(subdirrule),$(SUBDIRS))))
else
subdirs-all::
subdirs-dep::
subdirs-clean::
subdirs-distclean::
subdirs-strip::
endif

#######################################################################
# General rules
#
# this section should contain (generalized) rules for most build tasks,
# we rely on GNU make to do something sensible in most cases
#######################################################################
# the prototype of a pattern rule to compile or link an object file or
# executable
#
# call with:
# PATTERNRULE_TEMPLATE targetext srcext MESSAGEVAR COMPILEVARNAME
define PATTERNRULE_TEMPLATE
$(1): $(2)
	@ $$(ECHOMSG) "$$($(3))"
	$$($(4))
endef
# these will be used in a few foreach loops to synthesize the required
# rules
OBJSUFFIXES    := o os
EXTENSIONS_C   := c
EXTENSIONS_CXX := C cc cpp cxx c++
EXTENSIONS_F   := f F

# how to compile source files
COMPILE.c   = $(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
COMPILE.cxx = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
COMPILE.f   = $(FC) $(FFLAGS) $(CPPFLAGS) -c $< -o $@
COMPILEMSG.c   = "\\x1b[32m[CC]\\x1b[m\\t\\t$@"
COMPILEMSG.cxx = "\\x1b[32m[CXX]\\x1b[m\\t\\t$@"
COMPILEMSG.f   = "\\x1b[32m[FC]\\x1b[m\\t\t$@"
# generate pattern rules to compile source files
$(foreach extension,$(EXTENSIONS_C),$(foreach objsuffix,$(OBJSUFFIXES),\
    $(eval $(call PATTERNRULE_TEMPLATE,%.$(objsuffix),%.$(extension),COMPILEMSG.c,COMPILE.c))))
$(foreach extension,$(EXTENSIONS_CXX),$(foreach objsuffix,$(OBJSUFFIXES),\
    $(eval $(call PATTERNRULE_TEMPLATE,%.$(objsuffix),%.$(extension),COMPILEMSG.cxx,COMPILE.cxx))))
$(foreach extension,$(EXTENSIONS_F),$(foreach objsuffix,$(OBJSUFFIXES),\
    $(eval $(call PATTERNRULE_TEMPLATE,%.$(objsuffix),%.$(extension),COMPILEMSG.f,COMPILE.f))))

# include ROOT includes for C++ files by default
$(ccsrc.C:%.C=%.o) $(ccsrc.C:%.C=%.os): CPPFLAGS += $(ROOTINCLUDES)
$(ccsrc.cc:%.cc=%.o) $(ccsrc.cc:%.cc=%.os): CPPFLAGS += $(ROOTINCLUDES)
$(ccsrc.cpp:%.cpp=%.o) $(ccsrc.cpp:%.cpp=%.os): CPPFLAGS += $(ROOTINCLUDES)
$(ccsrc.cxx:%.cxx=%.o) $(ccsrc.cxx:%.cxx=%.os): CPPFLAGS += $(ROOTINCLUDES)
$(ccsrc.c++:%.c++=%.o) $(ccsrc.c++:%.c++=%.os): CPPFLAGS += $(ROOTINCLUDES)

# patch pattern rules for position independent code (shared libraries)
%.os: CFLAGS += $(PICFLAGS)
%.os: CXXFLAGS += $(PICFLAGS)
%.os: FFLAGS += $(PICFLAGS)

# building archives
ARMSG = "\\x1b[33m[AR]\\x1b[m\\t\\t$@\($^\)"
DOARCHIVE = $(AR) rcS $@ $^
# synthesize rule to put object files into an archive
$(foreach objsuffix,$(OBJSUFFIXES),$(eval \
    $(call PATTERNRULE_TEMPLATE,(%.$(objsuffix)),%.$(objsuffix),ARMSG,DOARCHIVE)))
# at the end of the process, make sure all object files have been picked up
# and update the archive's symobol index with ranlib
%.a:
	@ $(ECHOMSG) "$(ARMSG)"
	$(AR) rucS $@ $^
	@ $(ECHOMSG) "\x1b[33m[RANLIB]\x1b[m\t$@"
	$(RANLIB) $@

# link executable(s)
LINK = $(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@
LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@
LINK.cxx = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@
LINK.f = $(FC) $(FFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@
LINKMSG      = "\\x1b[35m[LINK]\\x1b[m\\t\\t$@"
LINKSHLIBMSG = "\\x1b[35m[SHLIB]\\x1b[m\\t\\t$@"
# linking a bunch of object files together
$(eval $(call PATTERNRULE_TEMPLATE,%,%.o,LINKMSG,LINK))
# patch for building shared libraries from .os files
# (the user is required to say which .os files should go in)
%.so: LDFLAGS += $(SHFLAGS)
# linking the shared object (.so) file from an archive of position independent
# code
.SECONDARY: %.os
$(eval $(call PATTERNRULE_TEMPLATE,%.so,,LINKSHLIBMSG,LINK))
# compiling and linking from C sources
$(foreach extension,$(EXTENSIONS_C),$(eval \
    $(call PATTERNRULE_TEMPLATE,%,%.$(extension),LINKMSG,LINK.c)))
# compiling and linking from C++ sources
$(foreach extension,$(EXTENSIONS_CXX),$(eval \
    $(call PATTERNRULE_TEMPLATE,%,%.$(extension),LINKMSG,LINK.cxx)))
# compiling and linking from FORTRAN sources
$(foreach extension,$(EXTENSIONS_F),$(eval \
    $(call PATTERNRULE_TEMPLATE,%,%.$(extension),LINKMSG,LINK.f)))

# ROOT dictionaries
#
# some versions of ROOT ship a buggy rootcint, so we postprocess with awk
ROOTCINT_PP = $(AWK) '/.*<.*>::fgIsA = 0/ { if (!($$0 ~ /^template *<>/)) \
    $$0 = "template <> " $$0; } // { print; }'
DOROOTCINT = $(shell $(ROOTCONFIG) --bindir)/rootcint -f $@.tmp -c -p \
	 $(filter-out %LinkDef.h,$^) $(filter %LinkDef.h,$^) && \
	 $(ROOTCINT_PP) < $@.tmp > $@ ; $(RM) -f $@.tmp
ROOTCINTMSG = "\\x1b[31m[ROOTCINT]\\x1b[m\\t$@"
$(foreach extension,$(EXTENSIONS_CXX),$(eval $(call \
    PATTERNRULE_TEMPLATE,%Dict.$(extension) %Dict.h,,ROOTCINTMSG,DOROOTCINT)))

#######################################################################
# include dependency information (automatic prerequisites)
#######################################################################
# automatic prerequisites (inter-source file dependencies)
MKDEPEND = $(CPP) $(CPPDEPFLAGS) $(CPPFLAGS) $< | \
	   $(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@ || \
	   $(RM) -f $@
MKDEPENDMSG = "\\x1b[36m[MKDEPEND]\\x1b[m\\t$<"
# make sure we use the "right" preprocessor
.deps/%.d: CPP = $(CC) -E
.deps/%.dd: CPP = $(CXX) -E
# include ROOT includes for C++ files by default
%.dd: CPPFLAGS += $(ROOTINCLUDES)
# synthesize rules
$(foreach extension,$(EXTENSIONS_C),\
    $(eval $(call PATTERNRULE_TEMPLATE,.deps/%.d,%.$(extension),MKDEPENDMSG,MKDEPEND)))
$(foreach extension,$(EXTENSIONS_CXX),\
    $(eval $(call PATTERNRULE_TEMPLATE,.deps/%.dd,%.$(extension),MKDEPENDMSG,MKDEPEND)))

# include the dependency information
-include $(alldeps)

# vim: tw=78:sw=4:ft=make
