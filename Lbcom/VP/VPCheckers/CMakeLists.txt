################################################################################
# Package: VPCheckers
################################################################################
gaudi_subdir(VPCheckers v1r2)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/TrackEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

gaudi_add_module(VPCheckers
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent 
                 LINK_LIBRARIES VPDetLib LinkerEvent MCEvent TrackEvent GaudiAlgLib LHCbKernel)
