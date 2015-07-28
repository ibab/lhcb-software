
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
                               CloneSignalMCParticles,
                               CloneBTaggingInfo,
                               CloneRelatedInfo,
                               ReFitAndClonePVs,
                               CloneRawBanks,
                               PackStrippingReports,
                               PackParticlesAndVertices,
                               PackRecObjects,
                               PackMCInfo,
                               CleanEmptyEventNodes,
                               PackTrackingClusters,
                               PrintTESContents,
                               PrintDecayTrees,
                               FindDuplicates )

def stripMicroDSTElements( pack              = True ,
                           saveTrackClusters = True ,
                           isMC        = False    ,
                           refit       = False    ,
                           notracks    = True     ) :
    '''
    Add the elements required on the Stripping MicroDST
    NOTE: This requires Brunel v41r0 SDSTs or higher
    '''
    #
    if notracks :
        _vpv_cloner_ = "VertexBaseFromRecVertexClonerNoTracks"
        _rpv_cloner_ = "RecVertexClonerNoTracks"
    else        :
        _vpv_cloner_ = "VertexBaseFromRecVertexCloner"
        _rpv_cloner_ = "RecVertexCloner"
    #
    elements    = [
        ClonePVs( RecVertexCloner = _rpv_cloner_ ,
                  ClonePVWeights  = False ),
        FindDuplicates(),
        #PrintDecayTrees(), # For debugging
        CloneParticleTrees( isMC = isMC ),
        CloneBTaggingInfo( CloneTaggerParticles = False, isMC = isMC ),
        CloneRelatedInfo( ),
        ClonePVRelations( location = "Particle2VertexRelations",
                          clonePVs = True,
                          RecVertexCloner = _vpv_cloner_ )
        ]

    if refit : 
        elements += [ ReFitAndClonePVs( RecVertexCloner = _vpv_cloner_ ) ] 

    if isMC :
        elements += [ CloneParticleMCInfo(),
                      CloneSignalMCParticles(),
                      CloneBackCat() ]
     
    if pack :
        elements += [ PackStrippingReports() ]
        if saveTrackClusters : elements += [ PackTrackingClusters() ]
        elements += [ PackParticlesAndVertices(),
                      PackRecObjects() ]
        if isMC :
            elements += [ PackMCInfo() ]
        elements += [ CleanEmptyEventNodes() ] # Must be last in the packing

    #elements += [ PrintTESContents() ] # For debugging
        
    return elements

def stripMicroDSTStreamConf( pack = True,
                             isMC = False,
                             selectiveRawEvent = False,
                             killTESAddressHistory = True ) :
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
                             extraItems    = eItems,
                             selectiveRawEvent = selectiveRawEvent,
                             killTESAddressHistory = killTESAddressHistory )

def stripCalibMicroDSTStreamConf( pack=True, isMC=False, selectiveRawEvent=False ) :
    return stripMicroDSTStreamConf(pack,isMC,selectiveRawEvent)

def microDSTElements( pack=True, saveTrackClusters=True, isMC=False, refit=False ) :
    return stripMicroDSTElements(pack,saveTrackClusters,isMC,refit)

def microDSTStreamConf( pack=True, isMC=False ) :
    return stripMicroDSTStreamConf(pack,isMC)
