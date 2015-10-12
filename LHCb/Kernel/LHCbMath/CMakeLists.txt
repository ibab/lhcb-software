################################################################################
# Package: LHCbMath
################################################################################
gaudi_subdir(LHCbMath v3r59)

gaudi_depends_on_subdirs(GaudiGSL
                         GaudiKernel
                         GaudiPython
                         Kernel/VectorClass)

find_package(Boost)
find_package(GSL)
find_package(ROOT COMPONENTS MathCore GenVector)
find_package(VDT)
find_package(Eigen)
find_package(pyanalysis)

gaudi_install_headers(LHCbMath)

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

set_property(SOURCE src/Similarity_AVX.cpp PROPERTY COMPILE_FLAGS " -mavx -O3 -fabi-version=0 " )
set_property(SOURCE src/Similarity_SSE3.cpp PROPERTY COMPILE_FLAGS " -msse3 -O3 " )
set_property(SOURCE src/Similarity_generic.cpp PROPERTY COMPILE_FLAGS " -O3 " )

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(TestPow
                       tests/pow.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestBit
                       tests/bit.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestDigit
                       tests/digit.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestKine
                       tests/kinematics.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestLomont
                       tests/TestLomontCompare.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestErrors
                       tests/withErrors.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestRound
                       tests/TestRound.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestLomont2
                       tests/TestLomontCPU.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestEigen
                       tests/TestEigen.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestBlind
                       tests/blind.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestAbs
                       tests/testAbs.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestChi2
                       tests/testChi2.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestHesse
                       tests/TestHesse.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestChi2Fit
                       tests/TestChi2Fit.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestLABug
                       tests/TestLABug.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestBloomFilter
                       tests/TestBloomFilter.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestSimilarity
                       tests/TestSimilarity.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestSimilarityTiming
                       tests/TestSimilarityTiming.cpp
                       INCLUDE_DIRS Boost GSL ROOT
                       LINK_LIBRARIES Boost GSL ROOT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestEigenGeometry   
                       tests/TestEigenGeometry.cpp
                       INCLUDE_DIRS Boost GSL ROOT VDT Eigen
                       LINK_LIBRARIES Boost GSL ROOT VDT GaudiKernel LHCbMathLib)
  gaudi_add_executable(TestXYZTypes   
                       tests/TestXYZTypes.cpp
                       INCLUDE_DIRS Boost GSL ROOT VDT Eigen
                       LINK_LIBRARIES Boost GSL ROOT VDT GaudiKernel LHCbMathLib)
endif()

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)

