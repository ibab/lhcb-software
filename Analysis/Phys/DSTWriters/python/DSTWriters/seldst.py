"""
Default configuration parameters for selection DST.
"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('selDSTElements',
           'selDSTStreamConf')

from Configurables import InputCopyStream
from streamconf import OutputStreamConf
from microdstelements import CloneParticleTrees, ClonePVRelations

def selDSTElements() :
    vetoTESList = ["/Event/Rec/Vertex/Primary",
                   "/Event/Rec/ProtoP/Charged",
                   "/Event/Rec/ProtoP/Neutrals",
                   "/Event/Rec/Track/Best",
                   "/Event/Rec/Track/FittedHLT1VeloTracks",
                   "/Event/Rec/Rich/PIDs",
                   "/Event/Rec/Track/Muon",
                   "/Event/Rec/Muon/MuonPID",
                   "/Event/Rec/Calo/Electrons",
                   "/Event/Rec/Calo/Photons",
                   "/Event/Rec/Calo/MergedPi0s",
                   "/Event/Rec/Calo/SplitPhotons"]
    return [ CloneParticleTrees( TESVetoList = vetoTESList ),
             ClonePVRelations( location = "Particle2VertexRelations",
                               clonePVs = True,
                               TESVetoList = vetoTESList )
             ]

def selDSTStreamConf() :
    return OutputStreamConf( streamType = InputCopyStream )
