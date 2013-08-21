################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v33r6p1)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci
                         Phys/DecayTreeTupleBase
                         Phys/DecayTreeTupleReco
                         Phys/DecayTreeTupleDalitz
                         Phys/DecayTreeTupleJets
                         Phys/DecayTreeTupleMC
                         Phys/DecayTreeTupleTrigger
                         Phys/DecayTreeTupleMuonCalib
                         Phys/DecayTreeTuple)

gaudi_add_test(QMTest QMTEST)
