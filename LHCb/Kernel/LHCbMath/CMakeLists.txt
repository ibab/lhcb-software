################################################################################
# Package: LHCbMath
################################################################################
gaudi_subdir(LHCbMath v3r37)

gaudi_depends_on_subdirs(GaudiGSL
                         GaudiKernel
                         GaudiPython)

find_package(Boost)
find_package(GSL)
find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_add_library(LHCbMathLib
                  src/*.cpp
                  PUBLIC_HEADERS LHCbMath
                  INCLUDE_DIRS Boost GSL ROOT
                  LINK_LIBRARIES Boost GSL ROOT GaudiGSLLib GaudiKernel)

gaudi_add_dictionary(LHCbMath
                     dict/LHCbMathDict.h
                     dict/LHCbMathDict.xml
                     INCLUDE_DIRS Boost GSL ROOT
                     LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib
                     OPTIONS "-U__MINGW32__")

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestPow
                       tests/pow.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestBit
                       tests/bit.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestDigit
                       tests/digit.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestKine
                       tests/kinematics.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestLomont
                       tests/TestLomontCompare.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestErrors
                       tests/withErrors.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestRound
                       tests/TestRound.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestLomont2
                       tests/TestLomontCPU.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestEigen
                       tests/TestEigen.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestBlind
                       tests/blind.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestAbs
                       tests/testAbs.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestChi2
                       tests/testChi2.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestHesse
                       tests/TestHesse.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestChi2Fit
                       tests/TestChi2Fit.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
endif()

gaudi_install_python_modules()

