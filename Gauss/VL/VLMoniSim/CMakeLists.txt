################################################################################
# Package: VLMoniSim
################################################################################
gaudi_subdir(VLMoniSim v1r0)

gaudi_depends_on_subdirs(Det/VLDet
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(VLMoniSim
                 src/*.cpp
                 LINK_LIBRARIES VLDetLib MCEvent GaudiAlgLib)

