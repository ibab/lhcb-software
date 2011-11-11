# which subdirectories and targets need to be built
SUBDIRS = Zoo
TARGETS = ZooReader ZooReader-complicated ZooOptionsReader

include Rules.mk

# override tools and flags as needed
$(TARGETS): CC = $(CXX)
$(TARGETS): LDLIBS += $(ROOTLIBS) -lMinuit $(LIBGENVECTOR)

# you can either link your executables against a statically compiled
# libZooROOT or use the libZooROOT shared library. the former has the
# advantage that all Zoo code is contained within the executable itself,
# so you just need ROOT to be able to run it. linking libZooROOT as a
# shared library keeps all Zoo routines in a separate file which has to
# be found by the dynamic linker in order to run your program, i.e. your
# might have to do
#
# 	export LD_LIBRARY_PATH=/path/to/libZooROOT.so:$LD_LIBRARY_PATH
#
# before you can run your executable
#
# to link your code against the statically compiled libZooROOT, uncomment
#$(TARGETS): Zoo/libZooROOT.a
# to link your code against the shared libZooROOT, uncomment
$(TARGETS): LDLIBS += -LZoo -lZooROOT
