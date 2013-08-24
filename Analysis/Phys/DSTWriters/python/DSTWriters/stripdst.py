"""
Default configuration for stripping DST.
"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('stripDSTElements',
           'stripDSTStreamConf')

from Configurables import InputCopyStream
from streamconf import OutputStreamConf
from microdstelements import ( CloneParticleTrees,
                               CloneBTaggingInfo,
                               ClonePVRelations,
                               PackStrippingReports,
                               PackParticlesAndVertices,
                               PackRecObjects,
                               CleanEmptyEventNodes,
                               PrintTESContents,
                               FindDuplicates )

def stripDSTElements(pack=True) :
    elements = [ FindDuplicates(),
                 CloneParticleTrees( TESVetoList = ["/Event/Rec/ProtoP/Charged",
                                                    "/Event/Rec/ProtoP/Neutrals",
                                                    "/Event/Rec/Track/Best",
                                                    "/Event/Rec/Rich/PIDs",
                                                    "/Event/Rec/Track/Muon",
                                                    "/Event/Rec/Muon/MuonPID",
                                                    "/Event/Rec/Calo/Electrons",
                                                    "/Event/Rec/Calo/Photons",
                                                    "/Event/Rec/Calo/MergedPi0s",
                                                    "/Event/Rec/Calo/SplitPhotons"] ),
                 CloneBTaggingInfo( CloneTaggerParticles = True ),
                 ClonePVRelations( location = "Particle2VertexRelations",
                                   clonePVs = True,
                                   RecVertexCloner = "VertexBaseFromRecVertexCloner" )
                 ]
    if pack :
        elements += [ PackStrippingReports(),
                      PackParticlesAndVertices(),
                      PackRecObjects(),
                      CleanEmptyEventNodes() ]
        #elements += [ PrintTESContents() ] # For debugging
    return elements

def stripDSTStreamConf( pack = True,
                        vetoItems = [ ] ) :
    eItems = [ '/Event/DAQ/RawEvent#1' ] # For backwards compatibility with sDSTs
    if pack :
        eItems += [ '/Event/Strip/pPhys/DecReports#1' ]
    else :
        eItems += [ '/Event/Strip/Phys/DecReports#1' ]
    return OutputStreamConf( streamType = InputCopyStream,
                             extraItems = eItems,
                             vetoItems  = vetoItems )
