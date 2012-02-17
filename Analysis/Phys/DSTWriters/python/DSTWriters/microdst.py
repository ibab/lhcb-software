"""

"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

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
                               CleanEmptyEventNodes )

from GaudiConfUtils.ConfigurableGenerators import LoKi__CounterAlg as CounterAlg

_gecConfig = CounterAlg(
    Preambulo = [
    "from LoKiTracks.decorators  import *" ,
    "from LoKiCore.functions     import *" ,
    ] ,
    ##
    Variables = {
    #
    ## Reco information, now all exists in RecSummary
    #
    ## "nPV"           : "CONTAINS ( 'Rec/Vertex/Primary'            ) " ,
##     "nBest"         : "CONTAINS ( 'Rec/Track/Best'                ) " ,
##     "nVelo"         : "TrNUM    ( 'Rec/Track/Best' , TrVELO       ) " ,
##     "nLong"         : "TrNUM    ( 'Rec/Track/Best' , TrLONG       ) " ,
##     "nUpstream"     : "TrNUM    ( 'Rec/Track/Best' , TrUPSTREAM   ) " ,
##     "nDownstream"   : "TrNUM    ( 'Rec/Track/Best' , TrDOWNSTREAM ) " ,    
##     "nTTrack"       : "TrNUM    ( 'Rec/Track/Best' , TrTTRACK     ) " ,
##     "nBack"         : "TrNUM    ( 'Rec/Track/Best' , TrBACKWARD   ) " ,
##     "nMuon"         : "CONTAINS ( 'Rec/Track/Muon'                ) " ,
##     "nEcalClusters" : "CONTAINS ( 'Rec/Calo/EcalClusters'         ) " ,
##     #
##     ## some ``raw'' information, now all exists in RecSummary
##     #
##     "nSpd"          : "recSummary ( LHCb.RecSummary.nSPDhits      , 'Raw/Spd/Digits'    ) " ,
##     "nOTClusters"   : "recSummary ( LHCb.RecSummary.nOTClusters   , 'Raw/OT/Times'      ) " ,
##     "nITClusters"   : "recSummary ( LHCb.RecSummary.nITClusters   , 'Raw/IT/Clusters'   ) " ,
##     "nTTClusters"   : "recSummary ( LHCb.RecSummary.nITClusters   , 'Raw/TT/Clusters'   ) " ,
##     "nVeloClusters" : "recSummary ( LHCb.RecSummary.nVeloClusters , 'Raw/Velo/Clusters' ) " 
    }
    )

def microDSTElements(pack=True) :
    elements = [ #CloneRecHeader(),
                 #CloneRecSummary(),
                 #CloneODIN(),
                 #GlobalEventCounters(configGenerator=_gecConfig),
                 ClonePVs( RecVertexCloner = "RecVertexClonerNoTracks",
                           ClonePVWeights  = False ),
                 CloneParticleTrees(ProtoParticleConer = "ProtoParticleCloner"),
                 ClonePVRelations(location = "Particle2VertexRelations",
                                  clonePVs = True,
                                  RecVertexCloner = "VertexBaseFromRecVertexClonerNoTracks"),
                 CloneLHCbIDs(fullDecayTree = True),
                 ReFitAndClonePVs(RecVertexCloner = "VertexBaseFromRecVertexClonerNoTracks")
                 #CloneRawBanks( banks = ['ODIN'] )
                 ]
    if pack : elements += [ PackStrippingReports(),
                            PackParticlesAndVertices(),
                            PackRecObjects(),
                            CleanEmptyEventNodes() ]
    return elements

def microDSTStreamConf() :
    return OutputStreamConf(streamType = OutputStream,
                            fileExtension = '.mdst',
                            extraItems = ['/Event/Rec/Header#1'])

def stripMicroDSTElements(pack=True) :
    '''
    Add the elements required on the Stripping MicroDST
    NOTE: This requires Brunel v41r0 SDSTs or higher
    '''
    elements = [ ClonePVs( RecVertexCloner = "RecVertexClonerNoTracks",
                           ClonePVWeights  = False ),
                 CloneParticleTrees(ProtoParticleConer = "ProtoParticleCloner"),
                 ClonePVRelations(location = "Particle2VertexRelations",
                                  clonePVs = True,
                                  RecVertexCloner = "VertexBaseFromRecVertexClonerNoTracks"),
                 CloneLHCbIDs(fullDecayTree = True),
                 ReFitAndClonePVs(RecVertexCloner = "VertexBaseFromRecVertexClonerNoTracks")
                 ]
    if pack : elements += [ PackStrippingReports(),
                            PackParticlesAndVertices(),
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
    eItems = [ '/Event/Rec/Header#1',
               '/Event/Rec/Status#1',
               '/Event/Rec/Summary#1',
               '/Event/Trigger/RawEvent#1',
               '/Event/Muon/RawEvent#1' ]
    if pack :
        eItems += ['/Event/Strip/pPhys/DecReports#1']
    else :
        eItems += ['/Event/Strip/Phys/DecReports#1']
    return OutputStreamConf( streamType    = OutputStream,
                             fileExtension = '.mdst',
                             extraItems    = eItems )
