################################################################################
# Package: BlsMoniSim
################################################################################
gaudi_subdir(BlsMoniSim v1r1)

gaudi_depends_on_subdirs(Event/MCEvent
                         GaudiAlg)

gaudi_add_module(BlsMoniSim
                 src/*.cpp
                 LINK_LIBRARIES MCEvent GaudiAlgLib)

