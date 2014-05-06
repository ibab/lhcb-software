################################################################################
# Package: VPMoniSim
################################################################################
gaudi_subdir(VPMoniSim v1r0)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(VPMoniSim
                 src/*.cpp
                 LINK_LIBRARIES VPDetLib MCEvent GaudiAlgLib)

