
include Makefile.arch

HdrSuf = h
SrcSuf = C

LIBS += -lMinuit

PROG = fit
SRCS := $(wildcard *.$(SrcSuf))
HDRS := $(wildcard *.$(HdrSuf))
OBJS := $(patsubst %.$(SrcSuf),%.$(ObjSuf),$(wildcard *.$(SrcSuf)))

# top-level rule, to compile everything.
all: $(PROG)
	@echo ""; echo " ============== make all done ==============="; echo ""

# don't link, just compile
src: $(OBJS)
	@echo ""; echo " ============== make src done ==============="; echo ""

depend: .depend
	@echo ""; echo " ============== make depend done ============"; echo ""

.depend: $(SRCS) $(HDRS)
	@echo ""; echo " ===== Figuring out dependencies ============"; echo ""
	makedepend -f- -- $(CXXFLAGS) -- $(SRCS) > .depend 2> .makedepend.err
	@echo ""; echo " ======== Dependencies done. ================"; echo ""

clean: 
	$(RM) $(OBJS) $(PROG)
	@echo ""; echo " =============== make clean done ============"; echo ""

$(PROG): $(OBJS)
	@echo ""; echo " =============== linking ===================="; echo ""
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $(PROG)
	@echo ""; echo " =============== linking done ==============="; echo ""

include .depend

%.(ObjSuf): %.$(SrcSuf)
	$(CXX) $(CXXFLAGS) -c $<
# DO NOT DELETE
