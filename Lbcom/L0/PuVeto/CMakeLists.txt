################################################################################
# Package: PuVeto
################################################################################
gaudi_subdir(PuVeto v7r10p1)

gaudi_depends_on_subdirs(Det/VeloDet
                         Event/DAQEvent
                         Event/L0Event
                         Event/MCEvent)

gaudi_add_module(PuVeto
                 src/*.cpp
                 LINK_LIBRARIES VeloDetLib DAQEventLib L0Event MCEvent)

