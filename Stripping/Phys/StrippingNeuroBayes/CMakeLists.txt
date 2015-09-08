################################################################################
# Package: StrippingNeuroBayes
################################################################################
gaudi_subdir(StrippingNeuroBayes v1r10)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Phys/DaVinciTools
                         Phys/LoKiPhys)

find_package(NeuroBayesExpert)

gaudi_add_module(StrippingNeuroBayes
                 src/*.cpp
                 INCLUDE_DIRS NeuroBayesExpert AIDA
                 LINK_LIBRARIES NeuroBayesExpert DaVinciKernelLib LoKiPhysLib)

