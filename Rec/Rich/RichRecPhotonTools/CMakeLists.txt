################################################################################
# Package: RichRecPhotonTools
################################################################################
gaudi_subdir(RichRecPhotonTools v1r9)

gaudi_depends_on_subdirs(Det/RichDet
                         GaudiKernel
                         Kernel/LHCbKernel
                         Rich/RichKernel
                         Rich/RichRecBase)

find_package(Boost)
find_package(GSL)

gaudi_add_module(RichRecPhotonTools
                 src/*.cpp
                 INCLUDE_DIRS Boost GSL
                 LINK_LIBRARIES Boost GSL RichDetLib GaudiKernel LHCbKernel RichKernelLib RichRecBase)

