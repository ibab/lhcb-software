################################################################################
# Package: BcmDigi
################################################################################
gaudi_subdir(BcmDigi v1r1)

gaudi_depends_on_subdirs(Det/BcmDet
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(BcmDigi
                 src/*.cpp
                 LINK_LIBRARIES BcmDetLib MCEvent GaudiAlgLib)

