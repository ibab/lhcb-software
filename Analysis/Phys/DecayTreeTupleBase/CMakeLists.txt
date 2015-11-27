################################################################################
# Package: DecayTreeTupleBase
################################################################################
gaudi_subdir(DecayTreeTupleBase v1r5)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Kernel/MCInterfaces
                         Phys/DaVinciKernel
                         Phys/LoKiPhys
                         Phys/LoKiPhysMC)

find_package(Boost)

gaudi_add_library(DecayTreeTupleBaseLib
                  src/lib/*.cpp
                  PUBLIC_HEADERS DecayTreeTupleBase
                  INCLUDE_DIRS Boost Kernel/MCInterfaces
                  LINK_LIBRARIES Boost CaloUtils DaVinciKernelLib LoKiPhysLib LoKiPhysMCLib)

gaudi_add_module(DecayTreeTupleBase
                 src/component/*.cpp
                 INCLUDE_DIRS Boost Kernel/MCInterfaces
                 LINK_LIBRARIES Boost CaloUtils DaVinciKernelLib LoKiPhysLib LoKiPhysMCLib DecayTreeTupleBaseLib)

