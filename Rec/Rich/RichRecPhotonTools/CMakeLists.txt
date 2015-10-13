################################################################################
# Package: RichRecPhotonTools
################################################################################
gaudi_subdir(RichRecPhotonTools v1r18)

gaudi_depends_on_subdirs(Det/RichDet
                         GaudiKernel
                         Kernel/LHCbKernel
                         Kernel/LHCbMath
                         Kernel/VectorClass
                         Rich/RichKernel
                         Rich/RichRecBase)

find_package(Boost)
find_package(GSL)
find_package(Eigen)
find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_add_module(RichRecPhotonTools
                 src/component/*.cpp
                 INCLUDE_DIRS Eigen Boost GSL VectorClass src
                 LINK_LIBRARIES Boost GSL RichDetLib GaudiKernel LHCbKernel LHCbMathLib RichKernelLib RichRecBase)

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(RichPhotonRecoTest
                       src/application/PhotonReco/*.cpp
                       INCLUDE_DIRS ROOT Eigen GSL VectorClass src
                       LINK_LIBRARIES LHCbMathLib GSL GaudiKernel ROOT)
endif()

#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -msse4")
