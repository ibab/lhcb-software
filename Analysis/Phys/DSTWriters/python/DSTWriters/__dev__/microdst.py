"""

"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('microDSTElements',
           'microDSTStreamConf',
           'stripMicroDSTElements',
           'stripMicroDSTStreamConf')

from GaudiConf.Configuration import *
from Configurables import OutputStream

from streamconf import OutputStreamConf

from microdstelements import  (CloneRecHeader,
                               CloneRecSummary,
                               CloneODIN,
                               GlobalEventCounters,
                               ClonePVs,
                               CloneParticleTrees,
                               ClonePVRelations,
                               CloneBackCat,
                               CloneBTaggingInfo,
                               ReFitAndClonePVs,
                               CloneLHCbIDs,
                               CloneRawBanks)

from GaudiConfUtils.ConfigurableGenerators import LoKi__CounterAlg as CounterAlg
_gecConfig = CounterAlg(Preambulo = ["from LoKiTracks.decorators import *"],
                        Variables = {
    "nSpd"          : "CONTAINS ( 'Raw/Spd/Digits' )          " ,
    "nVelo"         : "TrNUM    ( 'Rec/Track/Best' , TrVELO ) " ,
    "nLong"         : "TrNUM    ( 'Rec/Track/Best' , TrLONG ) " ,
    "nMuon"         : "CONTAINS ( 'Rec/Track/Muon' ) "          ,
    "nOT"           : "CONTAINS ( 'Raw/OT/Times'    )"          ,
    "nITClusters"   : "CONTAINS ( 'Raw/IT/Clusters' )"          ,
    "nTTClusters"   : "CONTAINS ( 'Raw/TT/Clusters' )"          ,
    "nVeloClusters" : "CONTAINS ( 'Raw/Velo/Clusters' )"
    })

def microDSTElements() :
    return [CloneRecHeader(),
            CloneRecSummary(),
            CloneODIN(),
            GlobalEventCounters(configGenerator=_gecConfig),
            ClonePVs(),
            CloneParticleTrees(copyProtoParticles = True),
            ClonePVRelations("Particle2VertexRelations",True)]

def microDSTStreamConf() :
    return OutputStreamConf(streamType = OutputStream,
                            fileExtension = '.mdst',
                            extraItems = ['/Event/DAQ/ODIN#1',
                                          '/Event/Rec/Header#1'])

def stripMicroDSTElements() :
    return [CloneRecHeader(),
            CloneRecSummary(),
            CloneODIN(),
            GlobalEventCounters(configGenerator=_gecConfig),
            ClonePVs(),
            CloneParticleTrees(copyProtoParticles = True),
            ClonePVRelations("Particle2VertexRelations", True),
            CloneLHCbIDs(fullDecayTree = True),
            ReFitAndClonePVs(),
            CloneRawBanks( banks = [ 'HltSelReports' ,
                                     'HltDecReports',
                                     'L0Calo',
                                     'L0CaloFull',
                                     'L0DU',
                                     'L0Muon',
                                     'L0MuonProcCand',
                                     'L0PU'           ] ) ]

def stripMicroDSTStreamConf() :
    return OutputStreamConf(streamType = OutputStream,
                            fileExtension = '.mdst',
                            extraItems = ['/Event/DAQ/ODIN#1',
                                          '/Event/Rec/Header#1',
                                          '/Event/Rec/Status#1',
                                          '/Event/Strip/Phys/DecReports#1' ])
