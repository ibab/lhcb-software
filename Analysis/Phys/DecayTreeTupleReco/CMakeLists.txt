################################################################################
# Package: DecayTreeTupleReco
################################################################################
gaudi_subdir(DecayTreeTupleReco v1r3)

gaudi_depends_on_subdirs(Phys/DecayTreeTupleBase)

gaudi_add_module(DecayTreeTupleReco
                 src/*.cpp
                 LINK_LIBRARIES DecayTreeTupleBaseLib)
