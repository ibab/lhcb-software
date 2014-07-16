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
                               CloneRelatedInfo,
                               ClonePVRelations,
                               PackStrippingReports,
                               PackParticlesAndVertices,
                               PackRecObjects,
                               CleanEmptyEventNodes,
                               PrintTESContents,
                               FindDuplicates )

def stripDSTElements(pack=True, stripPrefix = 'Strip', relatedInfo = [] ) :
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
                 CloneRelatedInfo( extensions = relatedInfo ), 
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
                        stripPrefix = 'Strip',
                        selectiveRawEvent = False ) :
    eItems = [ '/Event/DAQ/RawEvent#1' ] # For backwards compatibility with sDSTs
    phys = 'Phys'
    if pack :
      phys = 'pPhys'
    
    if isinstance(stripPrefix, list) : 
      for p in stripPrefix : 
        eItems += [ '/Event/%s/%s/DecReports#1' % (p, phys) ]
    else : 
      eItems += [ '/Event/%s/%s/DecReports#1' % (stripPrefix, phys) ]

    localVetoItems = vetoItems
    if selectiveRawEvent :
        # Add the Raw Event locations to the veto list, as in this mode
        # They are only saved when a line explicitly requests them
        # To Do : Find a way to avoid having to maintain a RawEvent list here ...
        localVetoItems = vetoItems + [ "/Event/Velo/RawEvent",
                                       "/Event/Rich/RawEvent",
                                       "/Event/Calo/RawEvent",
                                       "/Event/Muon/RawEvent",
                                       "/Event/Other/RawEvent" ]

    return OutputStreamConf( streamType = InputCopyStream,
                             extraItems = eItems,
                             vetoItems  = localVetoItems,
                             selectiveRawEvent = selectiveRawEvent )
