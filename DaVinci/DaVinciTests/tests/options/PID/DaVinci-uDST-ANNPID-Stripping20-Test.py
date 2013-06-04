
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import DaVinci, PhysConf
########################################################################

DaVinci().EvtMax     = 1000                  # Number of events
DaVinci().PrintFreq  = 100                   # Events to skip
DaVinci().DataType   = "2012"                # Must be given
DaVinci().HistogramFile = "udst-annpid.root"
DaVinci().InputType  = "MDST"

########################################################################

from Configurables import ( ANNGlobalPID__ChargedProtoANNPIDMoni,
                            TESCheck, GaudiSequencer )
protoLoc = "/Event/Bhadron/Rec/ProtoP/Charged"
seq = GaudiSequencer("ANNPIDCheckSeq")
seq.Members += [
    TESCheck( "CheckProtos",
              Inputs = [protoLoc],
              OutputLevel = 5,
              Stop = False ),
    ANNGlobalPID__ChargedProtoANNPIDMoni( "ANNPIDMoni",
                                          ProtoParticleLocation = protoLoc )
    ]
DaVinci().UserAlgorithms = [seq]
