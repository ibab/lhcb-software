from Configurables import ( ProcessPhase, GaudiSequencer, RecSysConf )

RecSysConf()

appConf = ApplicationMgr()
# Set main sequence
brunelSeq = GaudiSequencer("BrunelSequencer")
appConf.TopAlg = [ brunelSeq ]
brunelSeq.Members += [ "ProcessPhase/Init", "ProcessPhase/Reco"]
ProcessPhase("Init").DetectorList += ["Brunel", "Calo"]
