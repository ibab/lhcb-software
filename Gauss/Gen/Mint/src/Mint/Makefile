
include ../../Makefile.arch

HdrSuf = h
SrcSuf = cpp

MOREINCS := $(wildcard $(PWD)/../../../../)
MOREINCSFLAGS := $(patsubst %,-I%,$(MOREINCS))
CXXFLAGS += $(MOREINCSFLAGS)

LIBS += -lMinuit

PROG = $(shell grep -l main *.cpp | sed 's/\.cpp//g')

SRCS := $(wildcard ../../*/src/*.$(SrcSuf))
SRCS += $(wildcard *.$(SrcSuf))
HDRS := $(wildcard ../../*/src/*.$(HdrSuf))
HDRS += $(wildcard ../../../../Mint/*.$(HdrSuf))
HDRS += $(wildcard *.$(HdrSuf))
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
