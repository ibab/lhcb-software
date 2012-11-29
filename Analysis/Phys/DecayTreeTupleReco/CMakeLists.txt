################################################################################
# Package: DecayTreeTupleReco
################################################################################
gaudi_subdir(DecayTreeTupleReco v1r0)

gaudi_depends_on_subdirs(Event/HltEvent
                         Phys/DecayTreeTupleBase)

gaudi_add_module(DecayTreeTupleReco
                 src/*.cpp
                 LINK_LIBRARIES HltEvent DecayTreeTupleBaseLib)

