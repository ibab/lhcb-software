
from Gaudi.Configuration import *
from Configurables import DaVinci
from Configurables import MCTupleToolHierarchy, TupleToolMCTruth, TupleToolMCBackgroundInfo
from StrippingConf.Configuration import StrippingConf
from StrippingConf.StrippingStream import StrippingStream
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection

SL_stream = StrippingStream("_SL_DST")

from StrippingSelections import StrippingTrackEffD0ToK3Pi 
SL_stream.appendLines( StrippingTrackEffD0ToK3Pi.TrackEffD0ToK3PiAllLinesConf("TrackEffD0ToK3Pi", StrippingTrackEffD0ToK3Pi.confdict).lines() )

##### CONFIGURE THE STRIPPING
from Configurables import  ProcStatusCheck
ActiveStreams = [SL_stream]
sc = StrippingConf( Streams = ActiveStreams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = ProcStatusCheck() )

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())
sr.OnlyPositive = False
sr.ReportFrequency = 100

################ tuples

from Configurables import LoKi__Hybrid__TupleTool
from Configurables import TupleToolDecay
LoKi_KIN=LoKi__Hybrid__TupleTool("LoKi_KIN")
LoKi_KIN.Variables =  {
    "DTF_D0_M" : "DTF_FUN ( CHILD(1,M) ,True)",
    "DTFD0_D0_M" : "DTF_FUN ( CHILD(1,M) ,True, 'D0')",
    "DTFD0_Dst_M" : "DTF_FUN ( M ,True, 'D0')",
    "DTFD0_CHI2NDF" : "DTF_CHI2NDOF(True,'D0')"}

tupletools = []
tupletools.append("TupleToolKinematic")                                                                                                                                 
tupletools.append("TupleToolPid")                                                                                                                                                

triglist=['Hlt1TrackAllL0Decision',
          'Hlt2CharmHadD02HH_D02KPiDecision']
from Configurables import TupleToolTISTOS
TISTOSTool = TupleToolTISTOS('TISTOSTool')
TISTOSTool.VerboseL0 = False
TISTOSTool.VerboseHlt1 = True
TISTOSTool.VerboseHlt2 = True
TISTOSTool.TriggerList = triglist[:]

from Configurables import DecayTreeTuple
DecayDescr = '[D*(2010)+ ->  ^(D0 ->  ^(K*(892)+ -> ^K- ^pi+ ^pi+) ^[pi-]CC)  ^pi+]CC'
Branches = {'Dst'   : '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K-  pi+  pi+)  [pi-]CC)   pi+]CC',
            'D0'    : '[D*(2010)+ ->  ^(D0 ->   (K*(892)+ ->  K-  pi+  pi+)  [pi-]CC)   pi+]CC',
            'Kst'   : '[D*(2010)+ ->   (D0 ->  ^(K*(892)+ ->  K-  pi+  pi+)  [pi-]CC)   pi+]CC',
            'K'     : '[D*(2010)+ ->   (D0 ->   (K*(892)+ -> ^K-  pi+  pi+)  [pi-]CC)   pi+]CC',
            'Pi1'   : '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K- ^pi+  pi+)  [pi-]CC)   pi+]CC',
            'Pi2'   : '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K-  pi+ ^pi+)  [pi-]CC)   pi+]CC',
            'Pi'    : '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K-  pi+  pi+) ^[pi-]CC)   pi+]CC',
            'Slowpi': '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K-  pi+  pi+)  [pi-]CC)  ^pi+]CC'}
TupleDst = DecayTreeTuple("Dst")
TupleDst.Decay = DecayDescr
TupleDst.Inputs = ['Phys/TrackEffD0ToK3PiDstarLine/Particles']

TupleDst.ToolList =  tupletools[:]
TupleDst.Branches = Branches
TupleDst.addTool(TupleToolDecay, name="Dst")
TupleDst.Dst.addTool(LoKi_KIN)
TupleDst.Dst.ToolList+=["LoKi::Hybrid::TupleTool/LoKi_KIN"]
TupleDst.addTool(TISTOSTool, name="TISTOSTool")
TupleDst.ToolList += [ "TupleToolTISTOS/TISTOSTool" ]
TupleDst.addTool(TupleToolMCTruth,name="TruthTool")
TupleDst.addTool(TupleToolMCBackgroundInfo,name="BackgroundInfo")
TupleDst.ToolList+=["TupleToolMCTruth/TruthTool"]
TupleDst.ToolList+=["TupleToolMCBackgroundInfo/BackgroundInfo"]
TupleDst.addTool(MCTupleToolHierarchy,name="Hierarchy")
TupleDst.ToolList+=["MCTupleToolHierarchy/Hierarchy"]
TupleDst.TruthTool.addTool(MCTupleToolHierarchy())
TupleDst.TruthTool.ToolList+=["MCTupleToolHierarchy/Hierarchy"]
#rellocations= [ "Relations/Rec/ProtoP/Charged",        
#                "Relations/Rec/ProtoP/Upstream",   
#                "Relations/Rec/ProtoP/Neutrals",       
#                "Relations/"+VeloProtoOutputLocation]
#TupleDst.TruthTool.addTool(DaVinciSmartAssociator)
#TupleDst.TruthTool.DaVinciSmartAssociator.addTool(P2MCPFromProtoP) 
#TupleDst.TruthTool.DaVinciSmartAssociator.P2MCPFromProtoP.Locations=rellocations  
#TupleDst.Hierarchy.addTool(DaVinciSmartAssociator)
#TupleDst.Hierarchy.DaVinciSmartAssociator.addTool(P2MCPFromProtoP) 
#TupleDst.Hierarchy.DaVinciSmartAssociator.P2MCPFromProtoP.Locations=rellocations  
    
########### main sequence
from Configurables import FilterDesktop
StdParticles = ["StdAllLoosePions","StdNoPIDsPions","StdLoosePions","StdLooseKaons"]
MakePionsEtc = FilterDesktop('MakePionsEtc')
MakePionsEtc.Inputs=StdParticles
MakePionsEtc.Code="ALL"

DaVinci().PrintFreq = 1000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 10000
DaVinci().TupleFile = 'DVTuples.root'
DaVinci().appendToMainSequence( [MakePionsEtc] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
MODE="DATA"
if MODE == "MC":
    DaVinci().DataType = "2011"
    DaVinci().Simulation = True
    DaVinci().InputType = 'DST'
    from GaudiConf import IOHelper
    IOHelper().inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/MC/MC11a/ALLSTREAMS.DST/00014704/0000/00014704_00000001_1.allstreams.dst'])
else:
    DaVinci().DataType = "2012"
    DaVinci().Simulation = False
    DaVinci().InputType = 'SDST'
    importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")
    DaVinci().DDDBtag  = "dddb-20120831"
    DaVinci().CondDBtag = "cond-20121008"
