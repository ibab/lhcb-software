
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
                               CloneBTaggingInfo,
                               ReFitAndClonePVs,
                               CloneLHCbIDs,
                               CloneRawBanks,
                               PackStrippingReports,
                               PackParticlesAndVertices,
                               PackRecObjects,
                               CleanEmptyEventNodes,
                               PackTrackingClusters )

##from GaudiConfUtils.ConfigurableGenerators import LoKi__CounterAlg as CounterAlg
## _gecConfig = CounterAlg(
##     Preambulo = [
##     "from LoKiTracks.decorators  import *" ,
##     "from LoKiCore.functions     import *" ] ,
##     Variables = { }
##     )

def stripMicroDSTElements(pack=True,saveTrackClusters=True) :
    '''
    Add the elements required on the Stripping MicroDST
    NOTE: This requires Brunel v41r0 SDSTs or higher
    '''
    elements = [
        ClonePVs( RecVertexCloner = "RecVertexClonerNoTracks",
                  ClonePVWeights  = False ),
        CloneParticleTrees(),
        ClonePVRelations( location = "Particle2VertexRelations",
                          clonePVs = True,
                          RecVertexCloner = "VertexBaseFromRecVertexClonerNoTracks" ),
        ReFitAndClonePVs(RecVertexCloner = "VertexBaseFromRecVertexClonerNoTracks")
        ]
    if pack :
        elements += [ PackStrippingReports() ]
        if saveTrackClusters : elements += [ PackTrackingClusters() ]
        elements += [ PackParticlesAndVertices(),
                      PackRecObjects(),
                      CleanEmptyEventNodes() ]
    return elements

def stripMicroDSTStreamConf(pack=True) :
    eItems = [ '/Event/Rec/Header#1',
               '/Event/Rec/Status#1',
               '/Event/Rec/Summary#1',
               '/Event/Trigger/RawEvent#1' ]
    if pack :
        eItems += ['/Event/Strip/pPhys/DecReports#1']
    else :
        eItems += ['/Event/Strip/Phys/DecReports#1']
    return OutputStreamConf( streamType    = OutputStream,
                             fileExtension = '.mdst',
                             extraItems    = eItems )

def stripCalibMicroDSTStreamConf(pack=True) :
    conf = stripMicroDSTStreamConf(pack)
    conf.extraItems += [ '/Event/Muon/RawEvent#1' ]
    return conf

def microDSTElements(pack=True,saveTrackClusters=True) :
    return stripMicroDSTElements(pack,saveTrackClusters)

def microDSTStreamConf(pack=True) :
    return stripMicroDSTStreamConf(pack)
