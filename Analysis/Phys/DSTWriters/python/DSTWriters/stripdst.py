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
                               PackTrackingClusters,
                               FindDuplicates )

def stripDSTElements( pack = True,
                      stripPrefix = 'Strip',
                      saveTrackClusters = True ) :
    vetoTESList = ["/Event/Rec/Vertex/Primary",
                   "/Event/Rec/ProtoP/Charged",
                   "/Event/Rec/ProtoP/Neutrals",
                   "/Event/Rec/Track/Best",
                   "/Event/Rec/Rich/PIDs",
                   "/Event/Rec/Track/Muon",
                   "/Event/Rec/Muon/MuonPID",
                   "/Event/Rec/Calo/Electrons",
                   "/Event/Rec/Calo/Photons",
                   "/Event/Rec/Calo/MergedPi0s",
                   "/Event/Rec/Calo/SplitPhotons"]
    elements = [ FindDuplicates(),
                 CloneParticleTrees( TESVetoList = vetoTESList ),
                 CloneBTaggingInfo( CloneTaggerParticles = True,
                                    TESVetoList = vetoTESList ),
                 CloneRelatedInfo( ), 
                 ClonePVRelations( location = "Particle2VertexRelations",
                                   clonePVs = True,
                                   RecVertexCloner = "VertexBaseFromRecVertexCloner",
                                   TESVetoList = vetoTESList )
                 ]
    if pack :
        if isinstance(stripPrefix, list) : 
          for p in stripPrefix : 
            elements += [ PackStrippingReports( prefix = p ) ] 
        else : 
          elements += [ PackStrippingReports( prefix = stripPrefix ) ]
        if saveTrackClusters : elements += [ PackTrackingClusters(useAllTracks=True) ]
        elements += [ PackParticlesAndVertices(),
                      PackRecObjects(),
                      CleanEmptyEventNodes() ]
    
    #elements += [ PrintTESContents() ] # For debugging
        
    return elements

def stripDSTStreamConf( pack = True,
                        vetoItems = [ ], 
                        stripPrefix = 'Strip',
                        selectiveRawEvent = False,
                        killTESAddressHistory = True,
                        fileExtension = '.dst' ) :
    
    phys = 'Phys'
    if pack : phys = 'pPhys'

    eItems = [ '/Event/Trigger/RawEvent#1' ]
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
                                       "/Event/Other/RawEvent",
                                       "/Event/Tracker/RawEvent",
                                       "/Event/HC/RawEvent" ]
        
    return OutputStreamConf( streamType = InputCopyStream,
                             extraItems = eItems,
                             fileExtension = fileExtension,
                             vetoItems  = localVetoItems,
                             selectiveRawEvent = selectiveRawEvent,
                             killTESAddressHistory = killTESAddressHistory )
