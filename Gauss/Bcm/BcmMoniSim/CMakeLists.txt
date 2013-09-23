################################################################################
# Package: BcmMoniSim
################################################################################
gaudi_subdir(BcmMoniSim v1r2)

gaudi_depends_on_subdirs(Det/BcmDet
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(BcmMoniSim
                 src/*.cpp
                 LINK_LIBRARIES BcmDetLib MCEvent GaudiAlgLib)

