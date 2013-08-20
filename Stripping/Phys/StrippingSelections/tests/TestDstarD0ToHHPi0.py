
MODE = "DATA"
NUM = -1

from DSTWriters.microdstelements import *
from DSTWriters.Configuration import (SelDSTWriter,
                                      stripDSTStreamConf,
                                      stripDSTElements
                                      )
import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import  DaVinci, DecayTreeTuple, TupleToolTrigger
from Configurables import  GaudiSequencer
from Configurables import  DecayTreeTuple, CheckPV
from Configurables import  CombineParticles, FilterDesktop,  OfflineVertexFitter, TupleToolDecay, LoKi__Hybrid__TupleTool, TupleToolPid, TupleToolTrackInfo, TupleToolKinematic, TupleToolPropertime, TupleToolPrimaries, TupleToolTrigger, TupleToolEventInfo, TupleToolGeometry, TupleToolTISTOS, TupleToolPid, TupleToolRecoStats, TupleToolTrackPosition
from Configurables import TupleToolDecayTreeFitter , TupleToolPhotonInfo
from Configurables import LoKi__Hybrid__EvtTupleTool as MyTool 
from Configurables import DecayTreeTuple, LoKi__Hybrid__TupleTool, TupleToolDecay, TupleToolTrigger, TupleToolTISTOS, TupleToolTrackPosition
from Configurables import LoKi__Hybrid__TupleTool
from Configurables import FitDecayTrees
#from Configurables import Selection
from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence
# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ]}
                                #"GhostProbCut" : [-2.0, 0.5 ] }

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

from StrippingSelections import StrippingDstarD0ToHHPi0
default_config = StrippingDstarD0ToHHPi0.confdict.copy()

configs = []
configs.append(default_config)
### so that we can trivially loop over all of them and make common changes
if MODE == "MC":
    for config in configs:
        config["useTOS"] = False
        config["useHLT"] = False

confDstarD0ToHHPi0 = StrippingDstarD0ToHHPi0.DstarD0ToHHPi0AllLinesConf("DstarD0ToHHPi0",default_config)
stream.appendLines( confDstarD0ToHHPi0.lines() )

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

tupletoollist = ["TupleToolKinematic",
                 "TupleToolGeometry",
                 "TupleToolPrimaries",
                 "TupleToolRecoStats",
                 "TupleToolEventInfo",
                 "TupleToolTrackInfo",
                 "TupleToolTrigger",
                 "TupleToolPid"]

triglist=['L0HadronDecision',
          'Hlt1TrackAllL0Decision',
          'Hlt2CharmHadD02HHXDst_hhXDecision']

TISTOSTool = TupleToolTISTOS('TISTOSTool')
TISTOSTool.VerboseL0 = True
TISTOSTool.VerboseHlt1 = True
TISTOSTool.VerboseHlt2 = True
TISTOSTool.TriggerList = triglist[:]

TupleKpipi0RS = DecayTreeTuple("Kpipi0RS")
TupleKpipi0WS = DecayTreeTuple("Kpipi0WS")
TupleKKpi0 = DecayTreeTuple("KKpi0")
Tuplepipipi0 = DecayTreeTuple("pipipi0")
TupleKpipi0RS_M = DecayTreeTuple("Kpipi0RS_M")
TupleKKpi0_M  = DecayTreeTuple("KKpi0_M")
Tuplepipipi0_M  = DecayTreeTuple("pipipi0_M")
TupleKpipi0RS_pi0_WIDEMASS = DecayTreeTuple("Kpipi0RS_pi0_WIDEMASS")

TupleKpipi0RS.Inputs = ["Phys/DstarD0ToHHPi0_Kpipi0_R_Line/Particles"]
TupleKpipi0RS.Decay = "[D*(2010)+ -> (^D0 -> (K*(892)0 -> ^K- ^pi+) ^pi0) ^pi+]cc"
TupleKpipi0RS.Branches = {"Dstar" : "[D*(2010)+]cc : [D*(2010)+ -> (D0 -> (K*(892)0 -> K- pi+) pi0) pi+]cc",
                          "D0" : "[D*(2010)+ -> (^D0 -> (K*(892)0 -> K- pi+) pi0) pi+]cc",
                          "H1" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> ^K- pi+) pi0) pi+]cc",
                          "H2" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K- ^pi+) pi0) pi+]cc",
                          "pi0" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K- pi+) ^pi0) pi+]cc",
                          "slowpi" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K- pi+) pi0) ^pi+]cc"}
TupleKpipi0RS.addTool(TupleToolDecay, name="Dstar")
TupleKpipi0RS.Dstar.addTool(TISTOSTool, name="TISTOSTool")

TupleKpipi0WS.Inputs = ["Phys/DstarD0ToHHPi0_Kpipi0WS_R_Line/Particles"]
TupleKpipi0WS.Decay = "[D*(2010)+ -> (^D0 -> (K*(892)0 -> ^K+ ^pi-) ^pi0) ^pi+]cc"
TupleKpipi0WS.Branches = {"Dstar" : "[D*(2010)+]cc : [D*(2010)+ -> (D0 -> (K*(892)0 -> K+ pi-) pi0) pi+]cc",
                          "D0" : "[D*(2010)+ -> (^D0 -> (K*(892)0 -> K+ pi-) pi0) pi+]cc",
                          "H1" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> ^K+ pi-) pi0) pi+]cc",
                          "H2" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K+ ^pi-) pi0) pi+]cc",
                          "pi0" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K+ pi-) ^pi0) pi+]cc",
                          "slowpi" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K+ pi-) pi0) ^pi+]cc"}
TupleKpipi0WS.addTool(TupleToolDecay, name="Dstar")
TupleKpipi0WS.Dstar.addTool(TISTOSTool, name="TISTOSTool")

TupleKKpi0.Inputs = ["Phys/DstarD0ToHHPi0_KKpi0_R_Line/Particles"]
TupleKKpi0.Decay = "[D*(2010)+ -> (^D0 -> (K*(892)0 -> ^K+ ^K-) ^pi0) ^pi+]cc"
TupleKKpi0.Branches = {"Dstar" : "[D*(2010)+]cc : [D*(2010)+ -> (D0 -> (K*(892)0 -> K+ K-) pi0) pi+]cc",
                       "D0" : "[D*(2010)+ -> (^D0 -> (K*(892)0 -> K+ K-) pi0) pi+]cc",
                       "H1" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> ^K+ K-) pi0) pi+]cc",
                       "H2" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K+ ^K-) pi0) pi+]cc",
                       "pi0" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K+ K-) ^pi0) pi+]cc",
                       "slowpi" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K+ K-) pi0) ^pi+]cc"}
TupleKKpi0.addTool(TupleToolDecay, name="Dstar")
TupleKKpi0.Dstar.addTool(TISTOSTool, name="TISTOSTool")

Tuplepipipi0.Inputs = ["Phys/DstarD0ToHHPi0_pipipi0_R_Line/Particles"]
Tuplepipipi0.Decay = "[D*(2010)+ -> (^D0 -> (K*(892)0 -> ^pi+ ^pi-) ^pi0) ^pi+]cc"
Tuplepipipi0.Branches = {"Dstar" : "[D*(2010)+]cc : [D*(2010)+ -> (D0 -> (K*(892)0 -> pi+ pi-) pi0) pi+]cc",
                         "D0" : "[D*(2010)+ -> (^D0 -> (K*(892)0 -> pi+ pi-) pi0) pi+]cc",
                         "H1" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> ^pi+ pi-) pi0) pi+]cc",
                         "H2" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> pi+ ^pi-) pi0) pi+]cc",
                         "pi0" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> pi+ pi-) ^pi0) pi+]cc",
                         "slowpi" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> pi+ pi-) pi0) ^pi+]cc"}
Tuplepipipi0.addTool(TupleToolDecay, name="Dstar")
Tuplepipipi0.Dstar.addTool(TISTOSTool, name="TISTOSTool")

TupleKpipi0RS_M.Inputs = ["Phys/DstarD0ToHHPi0_Kpipi0_M_Line/Particles"]
TupleKpipi0RS_M.Decay = "[D*(2010)+ -> (^D0 -> (K*(892)0 -> ^K- ^pi+) ^pi0) ^pi+]cc"
TupleKpipi0RS_M.Branches = {"Dstar" : "[D*(2010)+]cc : [D*(2010)+ -> (D0 -> (K*(892)0 -> K- pi+) pi0) pi+]cc",
                            "D0" : "[D*(2010)+ -> (^D0 -> (K*(892)0 -> K- pi+) pi0) pi+]cc",
                            "H1" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> ^K- pi+) pi0) pi+]cc",
                            "H2" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K- ^pi+) pi0) pi+]cc",
                            "pi0" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K- pi+) ^pi0) pi+]cc",
                            "slowpi" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K- pi+) pi0) ^pi+]cc"}
TupleKpipi0RS_M.addTool(TupleToolDecay, name="Dstar")
TupleKpipi0RS_M.Dstar.addTool(TISTOSTool, name="TISTOSTool")

TupleKKpi0_M.Inputs = ["Phys/DstarD0ToHHPi0_KKpi0_M_Line/Particles"]
TupleKKpi0_M.Decay = "[D*(2010)+ -> (^D0 -> (K*(892)0 -> ^K+ ^K-) ^pi0) ^pi+]cc"
TupleKKpi0_M.Branches = {"Dstar" : "[D*(2010)+]cc : [D*(2010)+ -> (D0 -> (K*(892)0 -> K+ K-) pi0) pi+]cc",
                         "D0" : "[D*(2010)+ -> (^D0 -> (K*(892)0 -> K+ K-) pi0) pi+]cc",
                         "H1" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> ^K+ K-) pi0) pi+]cc",
                         "H2" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K+ ^K-) pi0) pi+]cc",
                         "pi0" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K+ K-) ^pi0) pi+]cc",
                         "slowpi" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K+ K-) pi0) ^pi+]cc"}
TupleKKpi0_M.addTool(TupleToolDecay, name="Dstar")
TupleKKpi0_M.Dstar.addTool(TISTOSTool, name="TISTOSTool")

Tuplepipipi0_M.Inputs = ["Phys/DstarD0ToHHPi0_pipipi0_M_Line/Particles"]
Tuplepipipi0_M.Decay = "[D*(2010)+ -> (^D0 -> (K*(892)0 -> ^pi+ ^pi-) ^pi0) ^pi+]cc"
Tuplepipipi0_M.Branches = {"Dstar" : "[D*(2010)+]cc : [D*(2010)+ -> (D0 -> (K*(892)0 -> pi+ pi-) pi0) pi+]cc",
                           "D0" : "[D*(2010)+ -> (^D0 -> (K*(892)0 -> pi+ pi-) pi0) pi+]cc",
                           "H1" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> ^pi+ pi-) pi0) pi+]cc",
                           "H2" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> pi+ ^pi-) pi0) pi+]cc",
                           "pi0" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> pi+ pi-) ^pi0) pi+]cc",
                           "slowpi" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> pi+ pi-) pi0) ^pi+]cc"}
Tuplepipipi0_M.addTool(TupleToolDecay, name="Dstar")
Tuplepipipi0_M.Dstar.addTool(TISTOSTool, name="TISTOSTool")

TupleKpipi0RS_pi0_WIDEMASS.Inputs = ["Phys/DstarD0ToHHPi0_Kpipi0_R_WIDEMASS_Line/Particles"]
TupleKpipi0RS_pi0_WIDEMASS.Decay = "[D*(2010)+ -> (^D0 -> (K*(892)0 -> ^K- ^pi+) ^pi0) ^pi+]cc"
TupleKpipi0RS_pi0_WIDEMASS.Branches = {"Dstar" : "[D*(2010)+]cc : [D*(2010)+ -> (D0 -> (K*(892)0 -> K- pi+) pi0) pi+]cc",
                          "D0" : "[D*(2010)+ -> (^D0 -> (K*(892)0 -> K- pi+) pi0) pi+]cc",
                          "H1" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> ^K- pi+) pi0) pi+]cc",
                          "H2" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K- ^pi+) pi0) pi+]cc",
                          "pi0" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K- pi+) ^pi0) pi+]cc",
                          "slowpi" : "[D*(2010)+ -> (D0 -> (K*(892)0 -> K- pi+) pi0) ^pi+]cc"}
TupleKpipi0RS_pi0_WIDEMASS.addTool(TupleToolDecay, name="Dstar")
TupleKpipi0RS_pi0_WIDEMASS.Dstar.addTool(TISTOSTool, name="TISTOSTool")

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())
sr.OnlyPositive = False

#from Configurables import AlgorithmCorrelationsAlg
#ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

from Configurables import FilterDesktop

StdParticles = []
StdParticles.append("Phys/StdAllLoosePions")
StdParticles.append("Phys/StdLoosePions")
StdParticles.append("Phys/StdLooseKaons")
StdParticles.append("Phys/StdLooseResolvedPi0")
StdParticles.append("Phys/StdLooseMergedPi0")
MakePionsEtc = FilterDesktop('MakePionsEtc')
MakePionsEtc.Inputs=StdParticles
MakePionsEtc.Code="ALL"

DaVinci().PrintFreq = -1
DaVinci().HistogramFile = 'DVHistos.root'
DaVinci().TupleFile = "DVTuples.root"
DaVinci().EvtMax = NUM
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ MakePionsEtc ] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ TupleKpipi0RS ])
DaVinci().appendToMainSequence( [ TupleKpipi0WS ])
DaVinci().appendToMainSequence( [ TupleKKpi0 ])
DaVinci().appendToMainSequence( [ Tuplepipipi0 ])
DaVinci().appendToMainSequence( [ TupleKpipi0RS_M ])
DaVinci().appendToMainSequence( [ TupleKKpi0_M ])
DaVinci().appendToMainSequence( [ Tuplepipipi0_M ])
DaVinci().appendToMainSequence( [ TupleKpipi0RS_pi0_WIDEMASS ])

if MODE == "MC":
    DaVinci().DataType = "2011"
    DaVinci().InputType = 'DST'
    DaVinci().Lumi = False
    DaVinci().Simulation = True
else:
    DaVinci().DataType = "2012"
    DaVinci().InputType = 'SDST'
    DaVinci().Lumi = True
    DaVinci().Simulation = False
    DaVinci().CondDBtag = "cond-20120710"
    DaVinci().DDDBtag   = "head-20120413"
