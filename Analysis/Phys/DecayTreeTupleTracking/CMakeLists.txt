################################################################################
# Package: DecayTreeTupleTracking
################################################################################
gaudi_subdir(DecayTreeTupleTracking v1r0)

gaudi_depends_on_subdirs(Event/TrackFitEvent
                         Phys/DecayTreeTupleBase)

gaudi_add_module(DecayTreeTupleTracking
                 src/*.cpp
                 LINK_LIBRARIES TrackFitEventLib DecayTreeTupleBaseLib)
