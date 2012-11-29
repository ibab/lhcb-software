################################################################################
# Package: AnalysisSys
################################################################################
gaudi_subdir(AnalysisSys v10r2)

gaudi_depends_on_subdirs(Analysis/PyAna
                         Ex/LoKiExample
                         Ex/MicroDSTExample
                         Hlt/HltSelChecker
                         MicroDST/MicroDSTAlgorithm
                         MicroDST/MicroDSTBase
                         MicroDST/MicroDSTConf
                         MicroDST/MicroDSTInterfaces
                         MicroDST/MicroDSTTools
                         Phys/AnalysisConf
                         Phys/AnalysisPython
                         Phys/DSTWriterTests
                         Phys/DSTWriters
                         Phys/DaVinciAssociators
                         Phys/DaVinciMCTools
                         Phys/DaVinciMonitors
                         Phys/DaVinciTrackRefitting
                         Phys/DecayTreeTuple
                         Phys/DecayTreeTupleBase
                         Phys/DecayTreeTupleDalitz
                         Phys/DecayTreeTupleJets
                         Phys/DecayTreeTupleMC
                         Phys/DecayTreeTupleMuonCalib
                         Phys/DecayTreeTupleReco
                         Phys/DecayTreeTupleTrigger
                         Phys/DisplVerticesMC
                         Phys/FlavourTagging
                         Phys/FlavourTaggingChecker
                         Phys/JetAccessories
                         Phys/JetAccessoriesMC
                         Phys/KaliCalo
                         Phys/LoKiAlgoMC
                         Phys/LoKiGenMC
                         Phys/LoKiPhysMC
                         Phys/MCStandardParticles
                         Phys/Particle2MCTruth
                         Phys/ParticleMakerChecker
                         Phys/TrackSmearing
                         Phys/VertexFitCheck
                         Rich/RichPIDQC
                         Tutorial/Analysis)

