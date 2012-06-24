
include $(PWD)/../../../Makefile.arch

#BOOST = /opt/local/include
PartSearch = $(PWD)/../../..
HdrSuf = h
SrcSuf = cpp

#MOREINCS += $(wildcard $(BOOST))
MOREINCS += $(wildcard $(PartSearch)/*/src)

MOREINCSFLAGS := $(patsubst %,-I%,$(MOREINCS))
CXXFLAGS += $(MOREINCSFLAGS)

#LIBS += -lMinuit
LIBS += -lMinuit -lRooFit -lRooFitCore -lHtml -lRooStats

PROG = $(shell grep -l main *.cpp | sed 's/\.cpp//g')

HDRS += $(wildcard *.$(HdrSuf))
HDRS += $(wildcard $(PartSearch)/*/src/*.$(HdrSuf))

#HDRS += $(wildcard $(D2HHHHDir)/D2HHHH/*.$(HdrSuf))
#HDRS += $(wildcard $(RooPhysFit)/RooPhysFitter/*.$(HdrSuf))
#HDRS += $(wildcard $(BOOST)/*/*.$(HdrSuf))

SRCS += $(wildcard *.$(SrcSuf))
#SRCS += $(wildcard $(D2HHHHDir)/src/*.$(SrcSuf))
#SRCS += $(wildcard $(RooPhysFit)/src/*.$(SrcSuf))
#SRCS += $(wildcard $(BOOST)/*/*.$(SrcSuf))
SRCS += $(wildcard $(PartSearch)/*/src/*.$(SrcSuf))


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
	@makedepend -f- -- $(CXXFLAGS) -- $(SRCS) > .depend 2> .makedepend.err
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
$(PROG): $(OBJS)
	@echo ""; echo " =============== linking ===================="; echo ""
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $(PROG)
	@echo ""; echo " =============== linking done ==============="; echo ""

include .depend

%.(ObjSuf): %.$(SrcSuf)
	$(CXX) $(CXXFLAGS) -c $<
##
# DO NOT DELETE
