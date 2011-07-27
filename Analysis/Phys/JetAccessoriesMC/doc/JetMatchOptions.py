# =============================================================================
# $URL$
# =============================================================================
## @file JetMatchOptions
#  @author  Albert Bursche bursche@cern.ch with many input from Victor Coco
#  @date 2010-11-05
#
#  Last modification $Date: 2011-07-26 $
#                 by $Author: bursche $
#
#
#
# Configuration example for most of the configured cases. 
# Don't run this in a large scale without removing all TupleTools that 
# are not needed. The corresponding algorithms are only executed if it is used
# so removing a TupleTool can speed things up.
# Tools can be configured using the dict returned from ConfigureDoD or using the TES
# locations (from TESmap.pdf)
#
#
# =============================================================================
__author__  = " Albert Bursche  bursche@cern.ch"
__date__    = " 2011-07-26 " 
__version__ = " Version $Revision: 1.0 $ "
# =============================================================================
from Configurables import DaVinci
def ConfigureDaVinci():
    DaVinci (
        DataType   = '2010' , 
        Simulation = True,
        Lumi            = False  , 
        HistogramFile   = 'ZjetMC_Histos.root',
        TupleFile   = 'ZjetMC_Tuple.root',
        CondDBtag = "sim-20101210-vc-mu100",
        DDDBtag = "head-20101206",
        EvtMax = 200,        
#        PrintFreq = 1
        )

def ConfigureTuples():
    from JetAccessoriesMC.ConfigurationHelpers import ConfigureDoD
    tables = ConfigureDoD()
    from Configurables import TupleToolJetRelations,DecayTreeTuple
    HepMCAllJetTuple = DecayTreeTuple("HepMCAllJetTuple")

    HepMCAllJetTuple.InputLocations = ['Phys/StdHepMCAllJets/Particles']
    HepMCAllJetTuple.Decay = "CELLjet"
    HepMCAllJetTuple.ToolList=['TupleToolKinematic', 'TupleToolEventInfo',
                               'LoKi__Hybrid__TupleTool/LoKiTool'
                               ]
    from JetAccessoriesMC.ConfigurationHelpers import addRelationTuple
    
    addRelationTuple (HepMCAllJetTuple,tables['StdHepMCAllJetsPartonic'] ,InvertTable = True,ReverseMatching = True)
    addRelationTuple (HepMCAllJetTuple,tables['StdHepMCAllJetsPartonic'] ,InvertTable = True,ReverseMatching = False)

    addRelationTuple (HepMCAllJetTuple,tables['StdHepMCJets2StdJetsDeltaR'] ,InvertTable = False,ReverseMatching = True,extraName ='DeltaR')
    addRelationTuple (HepMCAllJetTuple,tables['StdHepMCJets2StdJetsDeltaR'] ,InvertTable = False,ReverseMatching = False,extraName ='DeltaR')
  
    addRelationTuple (HepMCAllJetTuple,tables['StdHepMCAllJets2StdJetsmeasured'] ,InvertTable = False,ReverseMatching = True)
    addRelationTuple (HepMCAllJetTuple,tables['StdHepMCAllJets2StdJetsmeasured'] ,InvertTable = False,ReverseMatching = False)
    addRelationTuple (HepMCAllJetTuple,tables['StdHepMCAllJets2StdJetsMCtrue'] ,InvertTable = False,ReverseMatching = True)
    addRelationTuple (HepMCAllJetTuple,tables['StdHepMCAllJets2StdJetsMCtrue'] ,InvertTable = False,ReverseMatching = False)

 
    from Configurables import LoKi__Hybrid__TupleTool
    LoKiTool = LoKi__Hybrid__TupleTool('LoKiTool')
    LoKiTool.Variables = {
        "ETA":"ETA",
        "PHI":"PHI",
        "Y":"Y"
        }
    HepMCAllJetTuple.addTool(LoKiTool)
    DaVinci().UserAlgorithms  += [HepMCAllJetTuple]


    HepMCJetTuple = DecayTreeTuple("HepMCJetTuple")#,OutputLevel = VERBOSE)
    HepMCJetTuple.InputLocations = ['Phys/StdHepMCJets/Particles']
    HepMCJetTuple.Decay = "CELLjet"
    HepMCJetTuple.ToolList=['TupleToolKinematic', 'TupleToolEventInfo',
                            'LoKi__Hybrid__TupleTool/LoKiTool'
                            ]


    addRelationTuple (HepMCJetTuple,tables['StdHepMCJets2StdJetsmeasured']   ,InvertTable = False,ReverseMatching = True,extraName ='Measured')
    HepMCJetTuple.StdHepMCJets2StdJetsRevMeasured.ExtraWeights = {'OverlapMeasured':tables['StdHepMCJets2StdJetsmeasured'].OutputTable,
                                                                  'OverlapMCtrue'  :tables['StdHepMCJets2StdJetsMCtrue'].OutputTable}

    addRelationTuple (HepMCJetTuple,tables['StdHepMCJets2StdJetsMCtrue']   ,InvertTable = False,ReverseMatching = True,extraName ='MCtrue')
    HepMCJetTuple.StdHepMCJets2StdJetsRevMCtrue.ExtraWeights = {'OverlapMeasured':tables['StdHepMCJets2StdJetsmeasured'].OutputTable,
                                                                'OverlapMCtrue'  :tables['StdHepMCJets2StdJetsMCtrue'].OutputTable}


    addRelationTuple (HepMCJetTuple,tables['StdHepMCJets2StdJetsDeltaR'] ,InvertTable = False,ReverseMatching = False,extraName ='DeltaR')
   
    HepMCJetTuple.StdHepMCJets2StdJetsDeltaR.ExtraWeights = {'OverlapMeasured':tables['StdHepMCJets2StdJetsmeasured'].OutputTable,
                                                             'OverlapMCtrue'  :tables['StdHepMCJets2StdJetsMCtrue'].OutputTable}

    HepMCJetTuple.addTool(LoKiTool)
    DaVinci().UserAlgorithms  += [HepMCJetTuple]

ConfigureDaVinci()
ConfigureTuples()

# =============================================================================
# The END
# =============================================================================
