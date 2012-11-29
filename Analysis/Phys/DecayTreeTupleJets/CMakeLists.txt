################################################################################
# Package: DecayTreeTupleJets
################################################################################
gaudi_subdir(DecayTreeTupleJets v1r0)

gaudi_depends_on_subdirs(Phys/DecayTreeTupleBase
                         Phys/LoKiArrayFunctors)

gaudi_add_module(DecayTreeTupleJets
                 src/*.cpp
                 LINK_LIBRARIES DecayTreeTupleBaseLib LoKiArrayFunctorsLib)

