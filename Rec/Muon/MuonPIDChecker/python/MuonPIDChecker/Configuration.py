from GaudiConf.Configuration  import *
from Configurables import ( LHCbConfigurableUser, ProcessPhase, GaudiSequencer )

class MuonPIDChecker(LHCbConfigurableUser):

    # Steering options
    __slots__ = {
        "HistoTopDir":  "Muon/" 
       ,"DLLCut":       -1.0     
       ,"NSHCut":       1.     
       ,"RunningMC":    False
       ,"HistoDir":     "MuonPID/Monitor"
    }
    def __apply_configuration__(self):
       self.propagateProperties()
