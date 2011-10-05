########################################################################
#
# $Id: DaVinci.py,v 1.36 2009/05/20 14:18:09 pkoppenb Exp $
#
# Options for a typical DaVinci job
#
# @date 2008-08-06
#
########################################################################
################ First define all things to run ########################
########################################################################
from DaVinci.Configuration import *
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, DecayTreeTuple, TupleToolDecay, TupleToolTrigger, TupleToolTISTOS
from Configurables import FilterDesktop

muID = True

Usegrid = False
myEvents = 100

from Configurables import EventCountHisto,  LoKi__Hybrid__EvtTupleTool,  LoKi__Hybrid__TupleTool
DaVinci().MoniSequence += [EventCountHisto("DaVinciMonitor") ] 

if Usegrid:
    from Configurables import CondDB
    CondDB(UseOracle = True)
    importOptions("$APPCONFIGOPTS/DisableLFC.py")
    myEvents = -1
    
from Configurables import HltDecReportsDecoder, HltSelReportsDecoder, HltVertexReportsDecoder
DataOnDemandSvc().AlgMap["Hlt/DecReports"] = HltDecReportsDecoder(OutputLevel = WARNING)
DataOnDemandSvc().AlgMap["Hlt/SelReports"] = HltSelReportsDecoder(OutputLevel = WARNING)
DataOnDemandSvc().AlgMap["Hlt/VertexReports"] = HltVertexReportsDecoder( OutputLevel = WARNING)


mySequencer = GaudiSequencer('mySequencer')

## ---- Define  Input locations  -----
B2HHInput = "/Event/Dimuon/Phys/Bs2MuMuLinesNoMuIDLine"
B2MuMuInput = "/Event/Dimuon/Phys/Bs2MuMuLinesWideMassLine"
B2JpsiKInput = "/Event/Unblind/Phys/SelBu2JPsiK"
B2JpsiKstInput = "/Event/Unblind/Phys/SelBd2JPsiKstar" 
B2JpsiPhiInput = "/Event/Unblind/Phys/SelBs2JPsiPhi" 

##IT clusters decoding
from Configurables import RawBankToSTClusterAlg
createITClusters = RawBankToSTClusterAlg("CreateITClusters")
createITClusters.DetType     = "IT"

from PhysSelPython.Wrappers import AutomaticData, DataOnDemand, Selection, SelectionSequence
from Configurables import LoKi__HDRFilter as StripFilter


L0Triggers = ["L0MuonDecision" , "L0HadronDecision", "L0DiMuonDecision" ]
Hlt1Triggers = [  "Hlt1SingleMuonNoIPDecision ", 'Hlt1TrackAllL0Decision', 'Hlt1TrackMuonDecision','Hlt1TrackPhotonDecision', "Hlt1DiMuonLowMassDecision", "Hlt1DiMuonHighMassDecision", "Hlt1SingleMuonHighPTDecision", "Hlt1SingleElectronNoIPDecision" ,  ]
Hlt2Triggers = [ ## muon lines
    "Hlt2TopoMu2BodyBBDTDecision",
    "Hlt2DiMuonJPsiDecision",
    "Hlt2DiMuonLowMassDecision",
    "Hlt2SingleMuonDecision",
    "Hlt2DiMuonDetachedJPsiDecision",
    "Hlt2DiMuonDetachedHeavyDecision",
    "Hlt2DiMuonPsi2SDecision",
    "Hlt2DiMuonDetachedDecision",
    "Hlt2DiMuonDecision",
    "Hlt2DiMuonJPsiHighPTDecision",
    "Hlt2SingleMuonLowPTDecision",
    "Hlt2SingleMuonHighPTDecision",
    "Hlt2TriMuonTauDecision",
    ## hadron lines
    "Hlt2B2HHDecision",
    "Hlt2Topo3BodySimpleDecision",
    "Hlt2Topo3BodyBBDTDecision",
    "Hlt2TopoMu3BodyBBDTDecision",
    "Hlt2Topo2BodyBBDTDecision",
    "Hlt2Topo2BodySimpleDecision",
    "Hlt2DiMuonBDecision",
    "Hlt2Topo4BodyBBDTDecision",
    "Hlt2Topo4BodySimpleDecision",
    "Hlt2TopoMu4BodyBBDTDecision",
    ##others
    "Hlt2PassThroughDecision",
    "Hlt2TransparentDecision",
    "Hlt2IncPhiDecision"  ]

myTriggerList = L0Triggers + Hlt1Triggers + Hlt2Triggers

### Configure tuple
B2HHTuple = DecayTreeTuple('B2HHTuple')   
B2HHTuple.Inputs = [B2HHInput ]
B2HHTuple.IgnoreP2PVFromInputLocations = True # ignore all stored Particle -> PV relations
B2HHTuple.ReFitPVs = True # re-fit the PVs
B2HHTuple.Decay = 'B_s0 -> ^mu+ ^mu-'
B2HHTuple.ToolList +=  [  "TupleToolGeometry"
                          , "TupleToolKinematic"
                          , "TupleToolEventInfo"
                          , "TupleToolPid"
                          , "TupleToolPropertime"
                          , "TupleToolRecoStats"
                          , "TupleToolTrigger"
                          , "TupleToolTISTOS"
                          , "TupleToolTrackInfo"
                          , "TupleToolMuonVariables"
                          ,"LoKi::Hybrid::EvtTupleTool/LoKiEvent"
                          ]

if muID:
    B2HHTuple.ToolList+= ["TupleToolMuonIDCalib"]

if muID:
    from Configurables import TupleToolMuonIDCalib
    B2HHTuple.addTool(TupleToolMuonIDCalib)
    B2HHTuple.TupleToolMuonIDCalib.XFOIParameter1 = [5.5, 4.0, 3.3, 2.8,
                         5.2, 3.6, 2.4, 2.4,
                         5.7, 4.4, 2.8, 2.3,
                         5.1, 3.1, 2.3, 2.1,
                         5.8, 3.4, 2.6, 2.8]
    B2HHTuple.TupleToolMuonIDCalib.XFOIParameter2 = [11., 3., 1., 1.,
                         31., 28., 21., 17.,
                         30., 31., 27., 22.,
                         28., 33., 35., 47.,
                         31., 39., 56., 151.]
    B2HHTuple.TupleToolMuonIDCalib.XFOIParameter3 = [0.2, 0.08, 0.03, 0.1,
                         0.06, 0.08, 0.10, 0.15,
                         0.04, 0.06, 0.09, 0.12,
                         0.08, 0.15, 0.23, 0.36,
                         0.07, 0.14, 0.24, 0.49]
    B2HHTuple.TupleToolMuonIDCalib.YFOIParameter1 = [2.8, 1.7, -153., 1.9,
                         3.3, 2.1, 1.7, 1.6,
                         3.6, 2.8, 1.9, 1.8,
                         4.4, 3.3, 2.2, 2.2,
                         4.8, 3.9, 2.6, 2.3]
    B2HHTuple.TupleToolMuonIDCalib.YFOIParameter2 = [3., 2., 156., 0.,
                         17., 15., 9., 5.,
                         26., 25., 16., 15.,
                         30., 49., 57., 92.,
                         32., 55., 96., 166.]
    B2HHTuple.TupleToolMuonIDCalib.YFOIParameter3 = [0.03, 0.02, 0.00, 0.09,
                         0.13, 0.19, 0.19, 0.24,
                         0.11, 0.19, 0.21, 0.32,
                         0.10, 0.22, 0.30, 0.52,
                         0.08, 0.20, 0.34, 0.52]
    
B2HHTuple.addTool(TupleToolTISTOS)
B2HHTuple.TupleToolTISTOS.VerboseL0 = True
B2HHTuple.TupleToolTISTOS.VerboseHlt1 = True
B2HHTuple.TupleToolTISTOS.VerboseHlt2 = True
B2HHTuple.TupleToolTISTOS.OutputLevel = WARNING
B2HHTuple.TupleToolTISTOS.TriggerList = myTriggerList


LoKiEventTuple = LoKi__Hybrid__EvtTupleTool("LoKiEvent")
LoKiEventTuple.Preambulo = [ 
    "from LoKiTrigger.decorators import *",
    "from LoKiCore.functions import *"
    ]
LoKiEventTuple.VOID_Variables =  {
    "nTTCls"   :  " CONTAINS('Raw/TT/Clusters') " , ## number of Clusters in TT 
    "nVeloTrks"   :   " TrSOURCE('Rec/Track/Best' , TrVELO) >> TrSIZE " , ## number of Velo tracks
    "nLongTrks"   :   " TrSOURCE('Rec/Track/Best', TrLONG) >> TrSIZE " , ## number of Long tracks
    "nDownTrks"   :   " TrSOURCE('Rec/Track/Best', TrDOWNSTREAM) >> TrSIZE " , ## number of Down tracks
    "nTTrks"      :   " TrSOURCE('Rec/Track/Best', TrTTRACK) >> TrSIZE ",  ## number of T tracks
    }

B2HHTuple.addTool(LoKiEventTuple)


B2MuMuTuple = B2HHTuple.clone("B2MuMuTuple")
B2MuMuTuple.Inputs = [B2MuMuInput]

B2JpsiKTuple = B2HHTuple.clone("B2JpsiKTuple")
B2JpsiKTuple.Inputs = [B2JpsiKInput]
B2JpsiKTuple.Decay = "[B+ -> (^J/psi(1S) -> ^mu+ ^mu-) ^K+ ]cc"

B2JpsiKstTuple = B2HHTuple.clone("B2JpsiKstTuple")
B2JpsiKstTuple.Inputs = [ B2JpsiKstInput]
B2JpsiKstTuple.Decay = "[B0 -> (^J/psi(1S) -> ^mu+ ^mu-) (^ K*(892)0 -> ^K+ ^pi-) ]cc"

B2JpsiPhiTuple = B2HHTuple.clone("B2JpsiPhiTuple")
B2JpsiPhiTuple.Inputs = [ B2JpsiPhiInput ]
B2JpsiPhiTuple.Decay = "B_s0 -> (^J/psi(1S) -> ^mu+ ^mu-) (^phi(1020) -> ^K+ ^K-) "

mySequencer.Members += [ createITClusters ]


########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = myEvents  
DaVinci().SkipEvents = 0
DaVinci().DataType = "2011"
DaVinci().Simulation   = False
DaVinci().Lumi   = True
DaVinci().UserAlgorithms = [ mySequencer ]
DaVinci().MoniSequence += [B2HHTuple, B2MuMuTuple, B2JpsiKTuple, B2JpsiKstTuple, B2JpsiPhiTuple] 
DaVinci().TupleFile = 'myTuple.root'

DaVinci().DDDBtag = 'head-20110302'
DaVinci().CondDBtag = 'head-20110407'

DaVinci().InputType = "DST"
if not(Usegrid):
    DaVinci().Input  = [  "DATAFILE='PFN:/home/fsoomro/Test.Unblind.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
## DaVinci().Input = [ "DATAFILE= 'PFN:/castor/cern.ch/grid/lhcb/user/m/mbettler/2011_06/22050/22050079/Test.Unblind.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
    
    
    
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
########################################################################
#
# To run : gaudirun.py options/DaVinci.py options/DaVinciTestData.py
#
########################################################################


