
__all__ = ('microDSTElements',
           'microDSTStreamConf',
           'stripMicroDSTElements',
           'stripMicroDSTStreamConf',
           'stripCalibMicroDSTStreamConf')

from Configurables import OutputStream

from streamconf import OutputStreamConf

from microdstelements import ( CloneRecHeader,
                               CloneRecSummary,
                               CloneODIN,
                               GlobalEventCounters,
                               ClonePVs,
                               CloneParticleTrees,
                               ClonePVRelations,
                               CloneBackCat,
                               CloneParticleMCInfo,
                               CloneChargedMCInfo,
                               CloneBTaggingInfo,
                               ReFitAndClonePVs,
                               CloneRawBanks,
                               PackStrippingReports,
                               PackParticlesAndVertices,
                               PackRecObjects,
                               PackMCInfo,
                               CleanEmptyEventNodes,
                               PackTrackingClusters,
                               PrintTESContents,
                               FindDuplicates )

def stripMicroDSTElements( pack=True, saveTrackClusters=True, isMC=False ) :
    '''
    Add the elements required on the Stripping MicroDST
    NOTE: This requires Brunel v41r0 SDSTs or higher
    '''
    elements = [
        ClonePVs( RecVertexCloner = "RecVertexClonerNoTracks",
                  ClonePVWeights  = False ),
        FindDuplicates(),
        CloneParticleTrees(),
        CloneBTaggingInfo( CloneTaggerParticles = False ),
        ClonePVRelations( location = "Particle2VertexRelations",
                          clonePVs = True,
                          RecVertexCloner = "VertexBaseFromRecVertexClonerNoTracks" ),
        ReFitAndClonePVs( RecVertexCloner = "VertexBaseFromRecVertexClonerNoTracks" )
        ]
    if isMC :
        elements += [ CloneParticleMCInfo(),
                      CloneBackCat()
                      #CloneChargedMCInfo()
                      ]
    if pack :
        elements += [ PackStrippingReports() ]
        if saveTrackClusters : elements += [ PackTrackingClusters() ]
        elements += [ PackParticlesAndVertices(),
                      PackRecObjects() ]
        if isMC :
            elements += [ PackMCInfo() ]
        elements += [ CleanEmptyEventNodes() ] # Must be last
        #elements += [ PrintTESContents() ] # For debugging
    return elements

def stripMicroDSTStreamConf( pack=True, isMC=False ) :
    eItems = [ '/Event/Rec/Header#1',
               '/Event/Rec/Status#1',
               '/Event/Rec/Summary#1',
               '/Event/Trigger/RawEvent#1' ]
    if pack :
        eItems += ['/Event/Strip/pPhys/DecReports#1']
    else :
        eItems += ['/Event/Strip/Phys/DecReports#1']
    if isMC :
        eItems += ["/Event/MC/Header#1",
                   "/Event/MC/DigiHeader#1",
                   "/Event/Gen/Header#1"]
    return OutputStreamConf( streamType    = OutputStream,
                             fileExtension = '.mdst',
                             extraItems    = eItems )

def stripCalibMicroDSTStreamConf( pack=True, isMC=False ) :
    conf = stripMicroDSTStreamConf(pack,isMC)
    conf.extraItems += [ '/Event/Muon/RawEvent#1' ]
    return conf

def microDSTElements( pack=True, saveTrackClusters=True, isMC=False ) :
    return stripMicroDSTElements(pack,saveTrackClusters,isMC)

def microDSTStreamConf( pack=True, isMC=False ) :
    return stripMicroDSTStreamConf(pack,isMC)
