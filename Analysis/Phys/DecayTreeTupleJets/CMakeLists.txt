################################################################################
# Package: DecayTreeTupleJets
################################################################################
gaudi_subdir(DecayTreeTupleJets v1r1p2)

gaudi_depends_on_subdirs(Phys/DecayTreeTupleBase
                         Phys/LoKiArrayFunctors
			 Phys/JetTagging)

gaudi_add_module(DecayTreeTupleJets
                 src/*.cpp
                 LINK_LIBRARIES DecayTreeTupleBaseLib LoKiArrayFunctorsLib)
