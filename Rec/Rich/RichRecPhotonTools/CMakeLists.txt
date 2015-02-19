################################################################################
# Package: RichRecPhotonTools
################################################################################
gaudi_subdir(RichRecPhotonTools v1r15)

gaudi_depends_on_subdirs(Det/RichDet
                         GaudiKernel
                         Kernel/LHCbKernel
                         Kernel/VectorClass
                         Rich/RichKernel
                         Rich/RichRecBase)

find_package(Boost)
find_package(GSL)
find_package(Eigen)

gaudi_add_module(RichRecPhotonTools
                 src/component/*.cpp
                 INCLUDE_DIRS Eigen Boost GSL VectorClass src
                 LINK_LIBRARIES Boost GSL RichDetLib GaudiKernel LHCbKernel RichKernelLib RichRecBase)

#if(GAUDI_BUILD_TESTS)
#  gaudi_add_executable(RichPhotonRecoTest
#                       src/application/PhotonReco/*.cpp
#                       INCLUDE_DIRS GSL VectorClass src
#                       LINK_LIBRARIES GSL GaudiKernel)
#endif()

#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -msse4")
