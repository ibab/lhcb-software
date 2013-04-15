#######################################################################
# Rules.mk
#
# try to help physicists avoid writing Makefiles
#
# see below for usage instructions
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
# v0.6 2012-01-27 Manuel Schiller <manuel.schiller@nikhef.nl>
# 	optimise more agressively for the processor the library is
# 	built on (e.g. check /proc/cpuinfo for instruction setting
# 	extensions)
# 	some more work towards supporting multiple compilers (building
# 	with Gnu, Open64, Intel compilers works, the rest is work in
# 	progress, either because it does not like root's headers (Sun
# 	compiler) or the version installed on my system is too ancient
# 	(clang))
# v0.7 2012-02-08 Manuel Schiller <manuel.schiller@nikhef.nl>
# 	fix bug in cpu feature detection and how they get translated
# 	to optimization flags
# v0.8 2012-02-23 Manuel Schiller <manuel.schiller@nikhef.nl
# 	more work towards compiler independence, recognise some
# 	Fortran compilers and set up (hopefully) reasonable defaults
# v0.9 2012-09-03 Manuel Schiller <manuel.schiller@nikhef.nl>
# 	fix issue with per-target make variable overrides
# 	fix postprocessing of rootcint to be even more tolerant
# 	get reflex dictionaries to work
# 	add documentation on how to write dictionaries
# v0.10 2012-09-06 Manuel Schiller <manuel.schiller@nikhef.nl>
# 	fix automatic dependency tracking in standalone build system
# 	for Reflex dictionaries (make could miss occasions when the
# 	dictionaries need to be remade)
#######################################################################

#######################################################################
# USAGE
#######################################################################
# typical usage in a Makefile is as follows:
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
#
#
#######################################################################
# DICTIONARY HOWTO
#######################################################################
# If you need dictionaries to be able to write your objects to and
# read them from ROOT files or to make the ROOT interpreter aware of
# your classes, you need to set up files which describe for which
# classes and how dictionaries are to be built.
#
# You have the choice among two technologies: rootcint and Reflex.
# The former is easier to use, but is difficult to use with difficult
# preprocessor constructs and templates becauses it does not
# understand the entire C++ standard. Reflex dictionaries are more
# powerful, need less memory and should support whatever constructs
# gcc supports, but they are slightly less convenient to set up and use.
#
#
#######################################################################
# ROOTCINT DICTIONARIES
#######################################################################
# rootcint dictionaries are generated from a file which contains a
# list of classes from which to generate dictionaries which ends in
# *LinkDef.h. In it, you list all classes for which you want
# dictionaries to be generated. Let's assume you write a library for
# use in ROOT which contains a single class, YourClass, and a function,
# yourFunc, for which you need a dictionary. Your YourLibLinkDef.h files
# would then contain:
#
# #ifndef _YOURLIBLINKDEF_H
# #ifdef __CINT__
#
# #pragma link off all globals;
# #pragma link off all classes;
# #pragma link off all functions;
#
# #pragma link C++ class YourClass+;
# #pragma link C++ function yourFunc;
#
# #endif
# #endif
#
# Your Makefile might then look something like this:
#
# TARGETS += YourLibDict.cxx libYourLib.so
# alldicts += YourLibDict.cxx
#
# include Rules.mk
#
# YourLibDict.cxx: YourClass.h yourFunc.h YourLibLinkDef.h
# libYourLib.so: YourClass.os yourFunc.os YourLibDict.os
#
# You can find more information about rootcint dictionaries in ROOT
# documentation, e.g.

# http://root.cern.ch/drupal/content/selecting-dictionary-entries-linkdefh
#
#
#######################################################################
# REFLEX DICTIONARIES
#######################################################################
# Reflex dictionaries are generated using a proper C++ compiler and
# are therefore a little safer to use in the presence of templates and
# other fancy C++ constructs. Two files are needed to tell reflex for
# which classes you want dictionaries generated. They are typically
# called *Rflx.xml and *Rflx.h. Let's again assume you write a library
# for use in ROOT which contains a single class, YourClass, and a
# function,  yourFunc, for which you need a dictionary. YourLibRflx
# YourLibRflx.h file would then contain:
#
# #ifndef _YOURLIBRFLX_H
#
# #include "YourClass.h"
# #include "yourFunc.h"
# 
# #endif
# 
# The corresponding YourRflxLib.xml file would look like this:
#
# <lcgdict>
#     <selection>
#         <class name="YourClass"/>
#         <function name="yourFunc"/>
#     </selection>
# </lcgdict>
#
# Your Makefile might look something like this:
#
# TARGETS += YourLibRflx.cxx libYourLib.so
# alldicts += YourLibRflx.cxx
#
# include Rules.mk
#
# YourLibRflx.cxx: YourLibRflx.h YourLibRflx.xml
# libYourLib.so: YourClass.os yourFunc.os YourLibRflx.os
#
# That should be all that is needed to compile the code. The headers
# file for the Reflex dictionary needs to be called the same as the XML
# file and should include headers for all classes mentioned in the XML
# file.
#
# To use Reflex dictionaries in your code, you'll have to do a bit
# more. If you are using your library from the ROOT prompt, you need to
# run
#
# gSystem->Load("libCintex.so");
# Cintex::Enable();
#
# before loading your library. If you're compiling a standalone
# application, you need to link it with -lCintex, and you need to call
#
# Cintex::Enable()
#
# at the beginning of your main routine.
#
# Again, for more detailed information, please refer to the ROOT
# documentation,
#
# http://root.cern.ch/drupal/content/reflex
#
# seems like a good starting place
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
UNAME ?= uname
HEAD ?= head
TAIL ?= tail
GREP ?= grep

# get compiler version(s) used to compile ROOT
CC ?= $(shell $(ROOTCONFIG) --cc | $(SED) -e 's/ -[-a-zA-Z0-9_=:,]\\+//g')
CXX ?= $(shell $(ROOTCONFIG) --cxx | $(SED) -e 's/ -[-a-zA-Z0-9_=:,]\\+//g')
LD ?= $(shell $(ROOTCONFIG) --ld)
CPP ?= $(CC) -E
ROOTCONFIG_HASF77 := \
    $(strip $(shell $(ROOTCONFIG) --help | tr ' ' '\n' | grep -- '--f77'))
ifneq ($(ROOTCONFIG_HASF77),)
ifneq ($(FC),/usr/bin/f77)
# if FC points to /usr/bin/f77 (which is usually f2c in disguise), we
# just take whatever ROOT used, because f2c s*cks...
FC = $(shell $(ROOTCONFIG) --f77 | $(SED) -e 's/ -[-a-zA-Z0-9_=:,]\\+//g')
else
FC ?= $(shell $(ROOTCONFIG) --f77 | $(SED) -e 's/ -[-a-zA-Z0-9_=:,]\\+//g')
endif
endif
# locate genreflex, rootcint, gccxml for dictionaries
GENREFLEX ?= $(shell $(ROOTCONFIG) --bindir)/genreflex
ROOTCINT ?= $(shell $(ROOTCONFIG) --bindir)/rootcint
GCCXML ?= $(shell which gccxml)
GCCXMLDIR ?= $(shell dirname $(GCCXML))
ifneq ($(GENREFLEX),)
GENREFLEX = $(shell which genreflex)
endif
ifneq ($(ROOTCINT),)
ROOTCINT = $(shell which rootcint)
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

# assemble ROOT libraries
ROOTLIBDIR ?= $(shell $(ROOTCONFIG) --libdir)
# default set of ROOT libraries
ROOTLIBS ?= $(shell $(ROOTCONFIG) --libs)
# default ROOT compiler flags
ROOTINCLUDES ?= -I$(shell $(ROOTCONFIG) --incdir)
ROOTCFLAGS ?= $(shell $(ROOTCONFIG) --auxcflags)
# some versions of ROOT have libGenVector, others libMathCore
LIBGENVECTOR ?= $(shell $(TEST) -e $(ROOTLIBDIR)/libGenVector.so && \
		$(ECHO) '-lGenVector' || $(ECHO) '-lMathCore')

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
UNAME := $(UNAME)
HEAD := $(HEAD)
TAIL := $(TAIL)
GREP := $(GREP)
TUNEFLAGS := $(TUNEFLAGS)
ROOTLIBDIR := $(ROOTLIBDIR)
ROOTLIBS := $(ROOTLIBS)
ROOTINCLUDES := $(ROOTINCLUDES)
ROOTCFLAGS := $(ROOTCFLAGS)
LIBGENVECTOR := $(LIBGENVECTOR)
GCCXML := $(GCCXML)
GCCXMLDIR := $(GCCXMLDIR)
GENREFLEX := $(GENREFLEX)
ROOTCINT := $(ROOTCINT)

#######################################################################
# detect OS/compiler flavour
#
# the idea is that you can override values here which do not work
# if you use some special OS/compiler (and not Linux/gcc/g++)
#######################################################################
# guess OS
ifndef UNAME_SYS
UNAME_SYS := $(shell $(UNAME) -s)
endif
# guess CC CXX FC implementation
ifndef CC_FLAVOUR
# use preprocessor for C/C++
FLAVOURDET1:='__GNUC__ __clang__ __OPENCC__ __PATHCC__ __INTEL_COMPILER __SUNPRO_C'
FLAVOURDET2:='/^[^\#]/ { if ("__SUNPRO_C" != $$6) { print "SunPro"; } else { \
    if ("__INTEL_COMPILER" != $$5) { print "Intel"; } else { \
    if ("__PATHCC__" != $$4) { print "PathScale"; } else { \
    if ("__OPENCC__" != $$3) { print "Open64"; } else { \
    if ("__clang__" != $$2) { print "Clang"; } else { \
    if ("__GNUC__" != $$1) { print "GNU"; } else print "Unknown" } } } } } }'
CC_FLAVOUR:=$(shell $(ECHO) $(FLAVOURDET1) | $(CC) -E - | $(AWK) $(FLAVOURDET2))
CXX_FLAVOUR:=$(shell $(ECHO) $(FLAVOURDET1) | $(CXX) -E - | $(AWK) $(FLAVOURDET2))
# Fortran detection relies on flags
FC_FLAVOUR:=$(shell $(TEST) -n "$(FC)" && $(FC) --version 2>&1 | $(SED) -e 's/ /\n/g' | \
	   $(GREP) -E '(GNU|Intel|Open64|PathScale)' | $(HEAD) -1)
ifeq ($(FC_FLAVOUR),)
# unsure so far, check for SunPro, else Unknown
FC_FLAVOUR:=$(shell $(TEST) -n "$(FC)" && $(FC) -V 2>&1 | $(GREP) -q 'Sun' && $(ECHO) 'SunPro' || \
    $(ECHO) 'Unknown')
endif
endif
# on Linux, we know how to optimize at link time, and we have a nice
# and well-behaved /bin/echo
ifeq ($(UNAME_SYS),Linux)
TMPLINKOPT := -Wl,-O3
ECHO := /bin/echo
ECHOMSG := $(ECHO) -e
ifeq ($(filter $(TMPLINKOPT),$(LDFLAGS)),)
LDFLAGS += $(TMPLINKOPT)
endif
CPUFLAGS := $(shell $(GREP) "flags" /proc/cpuinfo | $(HEAD) -1)
endif
# echo on Darwin/MacOS X works a bit differently
ifeq ($(UNAME_SYS),Darwin)
ECHO := echo
ECHOMSG := $(ECHO)
CPUFLAGS := ""
endif

#######################################################################
# set up roughly equivalent options for different compiler flavours
#######################################################################
# how to use pipes between compiler stages (if possible)
PIPEFLAG.Unknown ?=
PIPEFLAG.SunPro ?=
PIPEFLAG.Open64 ?= -pipe
PIPEFLAG.PathScale ?= -pipe
PIPEFLAG.Intel ?= -pipe
PIPEFLAG.Clang ?= -pipe
PIPEFLAG.GNU ?= -pipe
# turn on warnings
WARNFLAGS.GNU ?= -Wall -Wextra -fmessage-length=78
WARNFLAGS.Clang ?= -Wall -Wextra -fmessage-length=78
WARNFLAGS.Intel ?= -Wall -Wextra -fmessage-length=78
WARNFLAGS.Open64 ?= -Wall -Wextra -fmessage-length=78
WARNFLAGS.PathScale ?= -Wall
WARNFLAGS.SunPro ?= +w +w2
WARNFLAGS.Unknown ?=
# standard optimization flags
STDOPTFLAGS.GNU ?= -O3 # -DBOOST_DISABLE_ASSERTS -DNDEBUG
STDOPTFLAGS.Clang ?= -O3 # -DBOOST_DISABLE_ASSERTS -DNDEBUG
STDOPTFLAGS.Intel ?= -O3 # -DBOOST_DISABLE_ASSERTS -DNDEBUG
STDOPTFLAGS.Open64 ?= -O3 # -DBOOST_DISABLE_ASSERTS -DNDEBUG
STDOPTFLAGS.PathScale ?= -O3 # -DBOOST_DISABLE_ASSERTS -DNDEBUG
STDOPTFLAGS.SunPro ?= -O5 # -DBOOST_DISABLE_ASSERTS -DNDEBUG
STDOPTFLAGS.Unknown ?= -O2 # -DBOOST_DISABLE_ASSERTS -DNDEBUG
# stanard debugging flags
STDDEBUGFLAGS.Unknown ?= -g
STDDEBUGFLAGS.GNU ?= -g
STDDEBUGFLAGS.Clang ?= -g
STDDEBUGFLAGS.Open64 ?= -g
STDDEBUGFLAGS.PathScale ?= -g
STDDEBUGFLAGS.Intel ?= -g
STDDEBUGFLAGS.SunPro ?= -g0
# set language standard(s) - should encourage users to write clean code
# C language standard
CSTD.Unknown ?=
CSTD.GNU ?= -std=c9x -pedantic
CSTD.Clang ?= -std=c9x -pedantic
CSTD.Open64 ?= -std=c9x -pedantic
CSTD.PathScale ?= -std=c9x -pedantic
CSTD.Intel ?= -std=c9x
CSTD.SunPro ?= -xc99
# C++ language standard
CXXSTD.Unknown ?=
CXXSTD.GNU ?= -std=gnu++98
CXXSTD.Clang ?= -std=gnu++98
CXXSTD.Intel ?= -std=gnu++98
CXXSTD.Open64 ?= -std=gnu++98
CXXSTD.PathScale ?= -std=gnu++98
CXXSTD.SunPro ?= -compat=g
# Fortran language standard - default to Fortran 77 where possible
FSTD.Unknown ?=
FSTD.GNU ?= -std=legacy
FSTD.Open64 ?= -ff77
FSTD.PathScale ?=
FSTD.Intel ?=
FSTD.SunPro ?=
# flags to generate position independent code
PICFLAGS.Unknown ?=
PICFLAGS.GNU ?= -fPIC
PICFLAGS.Intel ?= -fPIC
PICFLAGS.Clang ?= -fPIC
PICFLAGS.PathScale ?= -fPIC
PICFLAGS.Open64 ?= -fPIC
PICFLAGS.SunPro ?= -KPIC
# tuning flags - default is to tune for current machine and get the
# maximal amount of floating point performance, even if that means
# we do ugly things such as not setting errno
TUNEFLAGS.Unknown ?=
TUNEFLAGS.GNU ?= -ffast-math -fno-math-errno \
    $(shell $(CXX) --version 2>&1 | $(AWK) '// { for (i = 1; \
    i <= NF; ++i) if ($$i ~ /[0-9]*\.[0-9]*\.[0-9]*$$/) { \
    if ($$i < "4.") tunefl = "-mtune=opteron"; else tunefl = \
    "-mtune=native"; }; } END { print tunefl; }') \
    $(shell $(ECHO) $(CPUFLAGS) | \
    $(SED) -e 's/ mmx / -mmmx /g' -e 's/ sse/ -msse/g' \
    -e 's/ ssse/ -mssse/g' -e 's/ avx/ -mavx/g' \
    -e 's/4_1/4.1/g' -e 's/4_2/4.2/g' -e 's/ /\n/g' | \
    $(GREP) -- '-m')
# accept only last floating point feature (assume it's best)
TUNEFLAGS.Intel ?= \
    $(shell $(ECHO) $(CPUFLAGS) | \
    $(SED) -e 's/ mmx / -mmmx /g' -e 's/ sse/ -msse/g' \
    -e 's/ ssse/ -mssse/g' -e 's/ avx/ -mavx/g' \
    -e 's/4_1/4.1/g' -e 's/4_2/4.2/g' -e 's/ /\n/g' | \
    $(GREP) -- '-m' | $(TAIL) -1)
TUNEFLAGS.Clang ?= -march=native -mtune=native -ffast-math \
    -fno-math-errno \
    $(shell $(ECHO) $(CPUFLAGS) | \
    $(SED) -e 's/ mmx / -mmmx /g' -e 's/ sse/ -msse/g' \
    -e 's/ ssse/ -mssse/g' -e 's/ avx/ -mavx/g' \
    -e 's/4_1/4.1/g' -e 's/4_2/4.2/g' -e 's/ /\n/g' | \
    $(GREP) -- '-m')
# Open64's CPU feature detection does not work for sse4/avx
TUNEFLAGS.Open64 ?= -march=auto -OPT:Ofast -OPT:ro=3 \
    −fno−math−errno −ffast−math \
    $(shell $(ECHO) $(CPUFLAGS) | \
    $(SED) -e 's/ mmx / -mmmx /g' -e 's/ sse/ -msse/g' \
    -e 's/ ssse/ -mssse/g' -e 's/ avx/ -mavx/g' \
    -e 's/4_1/4.1/g' -e 's/4_2/4.2/g' -e 's/ /\n/g' | \
    $(GREP) -- '-m' | $(GREP) -E -v '(sse4|avx)')
# accept only last floating point feature (assume it's best)
TUNEFLAGS.PathScale ?= -march=auto -OPT:Ofast -OPT:ro=3 \
    −fno−math−errno −ffast−math \
    $(shell $(ECHO) $(CPUFLAGS) | \
    $(SED) -e 's/ mmx / -mmmx /g' -e 's/ sse/ -msse/g' \
    -e 's/ ssse/ -mssse/g' -e 's/ avx/ -mavx/g' \
    -e 's/ /\n/g' | $(GREP) -- '-m' | $(TAIL) -1)
TUNEFLAGS.SunPro ?= -xtarget=native -xarch=native -xbuiltin -fsimple=2

#######################################################################
# compiler flags
#
# try to provide reasonable defaults for most HEP users
# use the compiler flavour to switch between alternatives
#######################################################################
# build default skeletal set of compiler flags
CFLAGS ?= $(PIPEFLAG.$(CC_FLAVOUR)) $(STDOPTFLAGS.$(CC_FLAVOUR)) \
	  $(TUNEFLAGS.$(CC_FLAVOUR)) $(WARNFLAGS.$(CC_FLAVOUR)) \
	  $(STDDEBUGFLAGS.$(CC_FLAVOUR)) $(CSTD.$(CC_FLAVOUR)) \
	  $(PICFLAGS.$(CC_FLAVOUR))
CXXFLAGS ?= $(PIPEFLAG.$(CXX_FLAVOUR)) $(STDOPTFLAGS.$(CXX_FLAVOUR)) \
	    $(TUNEFLAGS.$(CXX_FLAVOUR)) $(WARNFLAGS.$(CXX_FLAVOUR)) \
	    $(STDDEBUGFLAGS.$(CXX_FLAVOUR)) $(CXXSTD.$(CXX_FLAVOUR)) \
	    $(ROOTCFLAGS) $(PICFLAGS.$(CXX_FLAVOUR))
FFLAGS ?= $(PIPEFLAG.$(FC_FLAVOUR)) $(STDOPTFLAGS.$(FC_FLAVOUR)) \
	  $(TUNEFLAGS.$(FC_FLAVOUR)) $(WARNFLAGS.$(FC_FLAVOUR)) \
	  $(STDDEBUGFLAGS.$(FC_FLAVOUR)) $(FSTD.$(FC_FLAVOUR)) \
	  $(PICFLAGS.$(FC_FLAVOUR))

# this helps for stack traces from within the application
ifndef LDFLAGS
LDFLAGS += -rdynamic
endif
# flags to link and produce shared libs
SHFLAGS ?= -shared
# flags needed to produce dependency files
CPPDEPFLAGS ?= -MM

#######################################################################
# colorful make messages
#
# set COLORMAKE to anything but "YES" to get the default make behaviour
#######################################################################
COLORMAKE ?= YES
ifeq ($(COLORMAKE),YES)
# we use our own rules, and want make a little less chatty
MAKEFLAGS += -rs
else
MAKEFLAGS += -r
# no COLORMAKE - do not echo anything
ECHOMSG := $(TRUE)
endif

#######################################################################
# export these variables to make subprocesses
#
# advantage is that make subprocesses will not have to reevaluate a
# large portion of these variables
#######################################################################
# toolchain (compiler, linker)
export CPP CC CXX FC LD AR RANLIB
export GCCXML GCCXMLDIR ROOTCINT GENREFLEX
# compiler/linker flags - we do not export those to retain the ability to
# override them on a per target basis
#export CFLAGS CXXFLAGS FFLAGS CPPFLAGS \
#    LDFLAGS SHFLAGS LDLIBS LOADLIBES \
#    CPPDEPFLAGS
export PIPEFLAG.Unknown PIPEFLAG.GNU PIPEFLAG.Clang PIPEFLAG.Open64 \
    PIPEFLAG.PathScale PIPEFLAG.Intel PIPEFLAG.SunPro
export WARNFLAGS.Unknown WARNFLAGS.GNU WARNFLAGS.Clang WARNFLAGS.Open64 \
    WARNFLAGS.PathScale WARNFLAGS.Intel WARNFLAGS.SunPro
export STDOPTFLAGS.Unknown STDOPTFLAGS.GNU STDOPTFLAGS.Clang \
    STDOPTFLAGS.Open64 STDOPTFLAGS.PathScale STDOPTFLAGS.Intel \
    STDOPTFLAGS.SunPro
export STDDEBUGFLAGS.Unknown STDDEBUGFLAGS.GNU STDDEBUGFLAGS.Clang \
    STDDEBUGFLAGS.Open64 STDDEBUGFLAGS.PathScale STDDEBUGFLAGS.Intel \
    STDDEBUGFLAGS.SunPro
export CSTD.Unknown CSTD.GNU CSTD.Clang CSTD.Open64 CSTD.PathScale \
    CSTD.Intel CSTD.SunPro
export CXXSTD.Unknown CXXSTD.GNU CXXSTD.Clang CXXSTD.Open64 CXXSTD.PathScale \
    CXXSTD.Intel CXXSTD.SunPro
export FSTD.Unknown FSTD.GNU FSTD.Open64 FSTD.PathScale FSTD.Intel CSTD.SunPro
export PICFLAGS.Unknown PICFLAGS.GNU PICFLAGS.Clang PICFLAGS.Open64 \
    PICFLAGS.PathScale PICFLAGS.Intel PICFLAGS.SunPro
export TUNEFLAGS.Unknown TUNEFLAGS.GNU TUNEFLAGS.Clang TUNEFLAGS.Open64 \
    TUNEFLAGS.PathScale TUNEFLAGS.Intel TUNEFLAGS.SunPro
# ROOT libs, compiler flags etc
export ROOTCONFIG ROOTLIBS ROOTLIBDIR ROOTINCLUDES ROOTCFLAGS LIBGENVECTOR
# *nix like tools for text processing, copying/moving files etc.
export AWK SED CPP CP MV MKDIR RMDIR TEST ECHO TRUE FALSE TOUCH UNAME \
    HEAD TAIL GREP
# communicate make(1) related options to subprocesses
export MAKEFLAGS COLORMAKE UNAME_SYS CC_FLAVOUR CXX_FLAVOUR FC_FLAVOUR

#######################################################################
# collect source files in current directory
#
# we append here to whatever the user has prelisted (because the user
# ultimately knows best)
#######################################################################
# C sources
csrc += $(wildcard *.c)
# ROOT/genreflex dictionaries
alldicts += $(sort $(wildcard *Dict.cc) $(wildcard *Dict.C) \
	    $(wildcard *Dict.cpp) $(wildcard *Dict.cxx) \
	    $(wildcard *Dict.c++) \
	    $(wildcard *Rflx.cc) $(wildcard *Rflx.C) \
	    $(wildcard *Rflx.cpp) $(wildcard *Rflx.cxx) \
	    $(wildcard *Rflx.c++))
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

local-all:: local-dep $(TARGETS)

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
	$(RM) -f $(TARGETS) $(alldicts) $(alldicts:%=%.h)
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
# prototype of pattern rule without action
define PATTERNRULE_NOACT_TEMPLATE
$(1):: $(2)
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

# override PIC flags for normal objects - no need to produce position
# independent code
%.o: PICFLAGS.$(CC_FLAVOUR)=
%.o: PICFLAGS.$(CXX_FLAVOUR)=
%.o: PICFLAGS.$(FC_FLAVOUR)=

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
ROOTCINT_PP = $(AWK) \
	      '/^.*<.*>.*::(fgIsA = 0(; *\/\/.*|;)|Class_Name\(\)|ImplFileName\(\)|ImplFileLine\(\)|Dictionary\(\)|Class\(\)|Streamer\(.*\)|ShowMembers\(.*\))$$/ \
	      { if (!($$0 ~ /^template *<>/)) $$0 = "template <> " $$0; } \
	      // { print; }'
DOROOTCINT = $(ROOTCINT) -f $@.tmp -c -p \
	 $(filter-out %LinkDef.h,$^) $(filter %LinkDef.h,$^) && \
	 $(ROOTCINT_PP) < $@.tmp > $@ ; $(RM) -f $@.tmp
ROOTCINTMSG = "\\x1b[31m[ROOTCINT]\\x1b[m\\t$@"
$(foreach extension,$(EXTENSIONS_CXX),$(eval $(call \
    PATTERNRULE_TEMPLATE,%Dict.$(extension) %Dict.$(extension).h,,ROOTCINTMSG,DOROOTCINT)))
# link dictionaries without debugging info
$(foreach extension,$(OBJSUFFIXES),$(eval $(call \
    PATTERNRULE_NOACT_TEMPLATE,%Dict.$(extension),\
    STDDEBUGFLAGS.$(CXX_FLAVOUR)=)))

#
# genreflex dictionaries
#
# how to generate REFLEX dictionaries
GENREFLEXMSG = "\\x1b[31m[GENREFLEX]\\x1b[m\\t$@"
DOGENREFLEX = $(GENREFLEX) $(patsubst %.xml, %.h,$(filter %.xml,$^)) \
	      -s $(filter %.xml,$^) -o $@ \
	      --gccxmlpath=$(GCCXMLDIR) --quiet $(CPPFLAGS)
$(foreach extension,$(EXTENSIONS_CXX),$(eval $(call \
    PATTERNRULE_TEMPLATE,%Rflx.$(extension),,GENREFLEXMSG,DOGENREFLEX)))
# tell genreflex where to find ROOT libraries
$(foreach extension,$(EXTENSIONS_CXX),$(eval $(call \
        PATTERNRULE_NOACT_TEMPLATE,%Rflx.$(extension),\
	CPPFLAGS+=$(ROOTINCLUDES))))
# link dictionaries without debugging info
$(foreach extension,$(OBJSUFFIXES),$(eval $(call \
    PATTERNRULE_NOACT_TEMPLATE,%Rflx.$(extension),\
    STDDEBUGFLAGS.$(CXX_FLAVOUR)=)))

#######################################################################
# include dependency information (automatic prerequisites)
#######################################################################
# automatic prerequisites (inter-source file dependencies)
#
# this includes some mangling to get dependencies of Reflex
# dictionaries right
MKDEPEND_RFLXMANGLING := \
    's,\.deps/\([^ .]\+\)Rflx\.dd : \1Rflx.\(C\|cc\|cpp\|cxx\|c++\), : \1Rflx.\2\n\1Rflx.\2 .deps/\1Rflx.dd :,g'
MKDEPEND = $(CPP) $(CPPDEPFLAGS) $(CPPFLAGS) $< | \
	   $(SED) -e 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	   	-e $(MKDEPEND_RFLXMANGLING) > $@ || \
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
