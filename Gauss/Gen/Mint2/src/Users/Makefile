
include $(PWD)/../../Makefile.arch


HdrSuf = h
SrcSuf = cpp
MintDir = $(PWD)/../../../Mint
MojitoDir = $(PWD)/../../../Mojito

MOREINCS := $(wildcard $(PWD)/../../*/src)
MOREINCS += $(PWD)/../../../../Mint
MOREINCS += $(PWD)/../../../..
MOREINCS += $(wildcard $(MintDir)/*/src)
MOREINCS += $(wildcard $(MojitoDir)/*/src)
MOREINCS += $(wildcard $(MintDir)/../..)
MOREINCS += $(wildcard $(MintDir)/../*)
MOREINCS += $(wildcard $(MintDir)/*/*)
MOREINCSFLAGS := $(patsubst %,-I%,$(MOREINCS))
CXXFLAGS += $(MOREINCSFLAGS)
#CXXFLAGS += -pg
#LDFLAGS += -pg
LIBS += -lMinuit
#LIBS += -lprofiler
##CXXFLAGS += -fopenmp
##LDFLAGS += -fopenmp



PROG = $(shell grep -l main *.cpp | sed 's/\.cpp//g')

SRCS := $(wildcard ../../*/src/*.$(SrcSuf))
SRCS += $(wildcard $(MintDir)/*/src/*.$(SrcSuf))
SRCS += $(wildcard $(MojitoDir)/*/src/*.$(SrcSuf))
SRCS += $(wildcard *.$(SrcSuf))
HDRS := $(wildcard ../../*/src/*.$(HdrSuf))
HDRS += $(wildcard $(MintDir)/*/src/*.$(HdrSuf))
HDRS += $(wildcard $(MintDir)/../../Mint/*.$(HdrSuf))
HDRS += $(wildcard $(MojitoDir)/*/src/*.$(HdrSuf))
HDRS += $(wildcard *.$(HdrSuf))
HDRS += $(wildcard ../../../../Mint/*.$(HdrSuf))
OBJS := $(patsubst %.$(SrcSuf),%.$(ObjSuf),$(SRCS))
##OBJS := $(shell echo $(OBJS) | sed 's/\/src\//\/bin\//g')
SRCSLOCAL += $(wildcard *.$(SrcSuf))
OBJSLOCAL := $(patsubst %.$(SrcSuf),%.$(ObjSuf),$(SRCSLOCAL))

# top-level rule, to compile everything.
all: $(PROG)
	@echo ""; echo " ============== make all done ==============="; echo ""

# don't link, just compile
src: $(OBJSLOCAL)
	@echo ""; echo " ============== make src done ==============="; echo ""

depend: .depend
	@echo ""; echo " ============== make depend done ============"; echo ""

.depend: $(SRCS) $(HDRS)
	@echo ""; echo " ===== Figuring out dependencies ============"; echo ""
	@makedepend -f- -- $(CXXFLAGS) -- $(SRCS) -- $(HDRS) > .depend 2> .makedepend.err
	@echo ""; echo " result in .depend, errors in .makedepend.err"; echo ""
	@echo ""; echo " ======== Dependencies done. ================"; echo ""

clean: 
	$(RM) $(OBJS) $(PROG) .depend .makedepend.err
	@echo ""; echo " =============== make clean done ============"; echo ""

debug:
	@echo "sources " $(SRCS)
	@echo "objects " $(OBJS)
	@echo "pwd " $(PWD)
	@echo "CXXFLAGS " $(CXXFLAGS)
	@echo "MintDir " $(MintDir)
$(PROG): $(OBJS)
	@echo ""; echo " =============== linking ===================="; echo ""
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $(PROG)
	@echo ""; echo " =============== linking done ==============="; echo ""

include .depend

%.(ObjSuf): %.$(SrcSuf)
	$(CXX) $(CXXFLAGS) -c $<
##
# DO NOT DELETE
