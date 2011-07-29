DEPDIR = .deps
df = $(DEPDIR)/$(*F)

BOOSTDIR = /project/bfys/lhcb/sw/lcg/external/Boost/1.42.0_python2.5/${CMTCONFIG}
BOOST_CPPFLAGS = -I$(BOOSTDIR)/include/boost-1_42
BOOST_LDFLAGS = -L$(BOOSTDIR)/lib -lboost_filesystem-gcc43-mt -lboost_system-gcc43-mt -lboost_program_options-gcc43-mt

ROOT_CPPFLAGS = -I${ROOTSYS}/include
ROOT_LDFLAGS = -L${ROOTSYS}/lib -lCore -lRIO -lTree -lMathMore -lCint -lThread -lNet \
	-ldl -lm -pthread

CPP = lcg-g++-4.3.2
LD = lcg-g++-4.3.2

CPPFLAGS := -ggdb -pipe -march=core2 -O2 ${ROOT_CPPFLAGS} ${BOOST_CPPFLAGS} -I..
LDFLAGS := ${LDFLAGS} ${ROOT_LDFLAGS} ${BOOST_LDFLAGS}

.phone: all clean

all: find_mismatches

%.o : %.cpp
	$(CPP) $(CPPFLAGS) -MMD -c $<
	@cp $*.d $(df).P; \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
              -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(df).P; \
          rm -f $*.d

find_mismatches: find_mismatches.o DecisionMap.o
	$(LD) -ggdb -o $@ $(LDFLAGS) $^

clean:
	@-rm *.o find_mismatches
