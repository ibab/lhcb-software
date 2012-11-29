################################################################################
# Package: StrippingAlgs
################################################################################
gaudi_subdir(StrippingAlgs v1r9)

gaudi_depends_on_subdirs(GaudiAlg
                         Kernel/LHCbAlgs
                         Kernel/SelectionLine
                         Phys/DaVinciKernel
                         Phys/LoKiCore)

find_package(AIDA)
find_package(Boost)

gaudi_add_module(StrippingAlgs
                 src/*.cpp
                 INCLUDE_DIRS AIDA Boost
                 LINK_LIBRARIES Boost GaudiAlgLib SelectionLine DaVinciKernelLib LoKiCoreLib)

