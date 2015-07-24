################################################################################
# Package: DecayTreeTuple
################################################################################
gaudi_subdir(DecayTreeTuple v5r1)

gaudi_depends_on_subdirs(Phys/DecayTreeFitter
                         Phys/DecayTreeTupleBase
                         Phys/DecayTreeTupleDalitz
                         Phys/DecayTreeTupleJets
                         Phys/DecayTreeTupleMC
                         Phys/DecayTreeTupleMuonCalib
                         Phys/DecayTreeTupleReco
                         Phys/DecayTreeTupleTrigger
                         Phys/DecayTreeTupleTracking
                         Phys/DecayTreeTupleANNPID
                         Phys/LoKiPhys)

find_package(HepMC)

gaudi_add_module(DecayTreeTuple
                 src/*.cpp
                 INCLUDE_DIRS HepMC
                 LINK_LIBRARIES HepMC DecayTreeFitter DecayTreeTupleBaseLib LoKiPhysLib)

gaudi_install_python_modules()

