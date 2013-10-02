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

def stripDSTElements(pack=True, stripPrefix = 'Strip' ) :
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
        if isinstance(stripPrefix, list) : 
          for p in stripPrefix : 
            elements += [ PackStrippingReports( prefix = p ) ] 
        else : 
          elements += [ PackStrippingReports( prefix = stripPrefix ) ]
        elements += [ PackParticlesAndVertices(),
                      PackRecObjects(),
                      CleanEmptyEventNodes() ]
        #elements += [ PrintTESContents() ] # For debugging
    return elements

def stripDSTStreamConf( pack = True,
                        vetoItems = [ ], 
                        stripPrefix = 'Strip' ) :
    eItems = [ '/Event/DAQ/RawEvent#1' ] # For backwards compatibility with sDSTs
    phys = 'Phys'
    if pack :
      phys = 'pPhys'
    
    if isinstance(stripPrefix, list) : 
      for p in stripPrefix : 
        eItems += [ '/Event/%s/%s/DecReports#1' % (p, phys) ]
    else : 
      eItems += [ '/Event/%s/%s/DecReports#1' % (stripPrefix, phys) ]

    return OutputStreamConf( streamType = InputCopyStream,
                             extraItems = eItems,
                             vetoItems  = vetoItems )
