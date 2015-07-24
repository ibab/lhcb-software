################################################################################
# Package: DaVinciAssociators
################################################################################
gaudi_subdir(DaVinciAssociators v9r8)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/PhysEvent
                         Event/TrackEvent
                         Kernel/Relations
                         Phys/DaVinciKernel
                         Phys/DaVinciMCKernel)

gaudi_add_module(DaVinciAssociators
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES CaloUtils LinkerEvent MCEvent PhysEvent TrackEvent RelationsLib DaVinciKernelLib DaVinciMCKernelLib)

