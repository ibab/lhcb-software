################################################################################
# Package: GauchoAppl
################################################################################
gaudi_subdir(GauchoAppl v1r26)

gaudi_depends_on_subdirs(Online/Gaucho
                         Online/GaudiOnline)

find_package( ROOT COMPONENTS Core Cling Thread Minuit )

gaudi_add_library(GauchoApplLib src/*.cpp
                  PUBLIC_HEADERS GauchoAppl
                  LINK_LIBRARIES GauchoLib ROOT)

gaudi_add_module(GauchoAppl src/components/*.cpp
                 LINK_LIBRARIES GauchoApplLib GaudiOnline ROOT)
macro(ga_test name)
  gaudi_add_executable(${name} ${ARGN}
                       LINK_LIBRARIES GauchoApplLib)
endmacro()

ga_test(Addertst src/Test/Addertst.cpp)
ga_test(RPCTest src/Test/RPCTest.cpp)
ga_test(HistTest src/Test/HistTest.cpp)
ga_test(HistHammer src/Test/HistHammer.cpp)
ga_test(CounterTest src/Test/CounterTest.cpp)
ga_test(Cycler src/Test/Cycler.cpp)
ga_test(RateTest src/Test/RateTest.cpp)
ga_test(segvtst src/Test/segvtst.cpp)
#ga_test(procfstst src/Test/procfstst.cpp)
