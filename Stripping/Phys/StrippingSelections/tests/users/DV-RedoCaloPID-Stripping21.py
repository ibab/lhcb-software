#================================================================
# Rerun PID Reco + Remake ProtoParticles
#================================================================
from Configurables import ( DaVinci, RecSysConf, GaudiSequencer,
                            ProcessPhase, PhysConf )

from STTools import STOfflineConf
STOfflineConf.DefaultConf().configureTools()

# Create the top level Conf object and set some general options from DV
rConf = RecSysConf("RecSysConf")
DaVinci().setOtherProps(rConf,["Simulation","DataType"])

# Only run PID + Protoparticles
rConf.RecoSequence = ["CALO","PROTO"]
rConf.SkipTracking = True
PhysConf().CaloReProcessing = True

# list of algs to prepend to DV
palgs = [ ]

# Create the Reco process phase
reco = ProcessPhase("Reco")
palgs += [reco]

# Re-pack the new CALO output
from Configurables import CaloDstPackConf
caloPackSeq = GaudiSequencer("CaloPacking")
caloPack = CaloDstPackConf ( Enable = True )
caloPack.Sequence      = caloPackSeq
caloPack.AlwaysCreate  = True
caloPack.EnableChecks  = False
caloPack.ClearRegistry = False
palgs += [caloPackSeq]

# Pack the new ProtoParticles
from Configurables import PackProtoParticle
packChargedPs = PackProtoParticle( name               = "PackChargedProtos",
                                   AlwaysCreateOutput = True,
                                   ClearRegistry      = False,
                                   InputName          = "/Event/Rec/ProtoP/Charged",
                                   OutputName         = "/Event/pRec/ProtoP/Charged",
                                   EnableCheck        = False )
packNeutralPs = PackProtoParticle( name               = "PackNeutralProtos",
                                   AlwaysCreateOutput = True,
                                   ClearRegistry      = False,
                                   InputName          = "/Event/Rec/ProtoP/Neutrals",
                                   OutputName         = "/Event/pRec/ProtoP/Neutrals",
                                   EnableCheck        = False )
palgs += [packChargedPs,packNeutralPs]

# Add to the start of the DV main sequence
#DaVinci().prependToMainSequence(palgs)
from Configurables import GaudiSequencer
init = GaudiSequencer("PhysInitSeq")
init.Members+= palgs

# Disable on-demand unpacking of locations created by the above
from Gaudi.Configuration import appendPostConfigAction
def removeUnpacking():
    from Configurables import DataOnDemandSvc, CaloProcessor
    DataOnDemandSvc().AlgMap.pop( "/Event/Rec/ProtoP/Neutrals", None )
    DataOnDemandSvc().AlgMap.pop( "/Event/Rec/ProtoP/Charged", None )
    DataOnDemandSvc().AlgMap.pop( "/Event/Rec/Calo/Electrons", None )
    DataOnDemandSvc().AlgMap.pop( "/Event/Rec/Calo/Photons", None )
    DataOnDemandSvc().AlgMap.pop( "/Event/Rec/Calo/MergedPi0s", None )
    DataOnDemandSvc().AlgMap.pop( "/Event/Rec/Calo/SplitPhotons", None )
appendPostConfigAction( removeUnpacking )
