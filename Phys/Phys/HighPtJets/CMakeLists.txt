################################################################################
# Package: HighPtJets
################################################################################
gaudi_subdir(HighPtJets v1r9)

gaudi_depends_on_subdirs(Event/MCEvent
                         Phys/DaVinciKernel
                         Phys/DaVinciMCKernel
                         Tr/TrackInterfaces)

gaudi_add_module(HighPtJets
                 src/*.cpp
                 INCLUDE_DIRS AIDA Tr/TrackInterfaces
                 LINK_LIBRARIES MCEvent DaVinciKernelLib DaVinciMCKernelLib)

