################################################################################
# Package: DecayTreeTupleANNPID
################################################################################
gaudi_subdir(DecayTreeTupleANNPID v1r2)

gaudi_depends_on_subdirs(Phys/DecayTreeTupleBase
                         Rec/RecInterfaces)

gaudi_add_module(DecayTreeTupleANNPID
                 src/*.cpp
                 LINK_LIBRARIES DecayTreeTupleBaseLib)
