################################################################################
# Package: GaussUMC
################################################################################
gaudi_subdir(GaussUMC v2r0)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(GaussUMC
                 src/*.cpp
                 LINK_LIBRARIES VPDetLib MCEvent GaudiAlgLib)

