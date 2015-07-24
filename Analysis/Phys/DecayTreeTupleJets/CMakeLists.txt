################################################################################
# Package: DecayTreeTupleJets
################################################################################
gaudi_subdir(DecayTreeTupleJets v1r3)

gaudi_depends_on_subdirs(Event/HltEvent
                         Phys/DecayTreeTupleBase
                         Phys/LoKiArrayFunctors
                         Phys/JetTagging)

gaudi_add_module(DecayTreeTupleJets
                 src/*.cpp
                 LINK_LIBRARIES HltEvent DecayTreeTupleBaseLib LoKiArrayFunctorsLib)
