################################################################################
# Package: HCMoniSim
################################################################################
gaudi_subdir(HCMoniSim v1r0)

gaudi_depends_on_subdirs(Event/MCEvent
                         GaudiAlg)

gaudi_add_module(HCMoniSim
                 src/*.cpp
                 LINK_LIBRARIES MCEvent GaudiAlgLib)

