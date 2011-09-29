# which subdirectories and targets need to be built
SUBDIRS = Zoo
TARGETS = ZooReader ZooReader-complicated ZooOptionsReader

include Rules.mk

# override tools and flags as needed
$(TARGETS): CC = $(CXX)
$(TARGETS): LDLIBS += $(ROOTLIBS) -lMinuit $(LIBGENVECTOR) -LZoo -lZooROOT
