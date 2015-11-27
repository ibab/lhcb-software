################################################################################
# Package: DaVinciMCTools
################################################################################
gaudi_subdir(DaVinciMCTools v10r14)

gaudi_depends_on_subdirs(Calo/CaloInterfaces
                         Calo/CaloUtils
                         Event/GenEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         Kernel/MCInterfaces
                         Phys/DaVinciKernel
                         Phys/DaVinciMCKernel
                         Phys/LoKiPhysMC)

gaudi_add_module(DaVinciMCTools
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces
                 LINK_LIBRARIES CaloInterfaces CaloUtils GenEvent LinkerEvent MCEvent PhysEvent RecEvent DaVinciKernelLib DaVinciMCKernelLib LoKiPhysMCLib)

