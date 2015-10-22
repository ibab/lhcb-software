################################################################################
# Package: DecayTreeTupleTracking
################################################################################
gaudi_subdir(DecayTreeTupleTracking v1r3)

gaudi_depends_on_subdirs(Tr/TrackFitEvent
                         Phys/DecayTreeTupleBase)

gaudi_add_module(DecayTreeTupleTracking
                 src/*.cpp
                 LINK_LIBRARIES TrackFitEvent DecayTreeTupleBaseLib)
