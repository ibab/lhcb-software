### test the phi->KsKs stripping module
### Mika Vesterinen

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }
# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
Stream = StrippingStream("TEST")

from StrippingSelections import StrippingPhiToKSKS
confPhiToKSKS = StrippingPhiToKSKS.PhiToKSKSAllLinesConf("PhiToKSKS", StrippingPhiToKSKS.confdict)
Stream.appendLines( confPhiToKSKS.lines() )

mod_conf = StrippingPhiToKSKS.confdict.copy()
mod_conf["DoDTF"] = False
confPhiToKSKS_noDTF = StrippingPhiToKSKS.PhiToKSKSAllLinesConf("PhiToKSKS_NODTF", mod_conf)
Stream.appendLines( confPhiToKSKS_noDTF.lines() )

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ Stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())
sr.OnlyPositive = False
sr.ReportFrequency = 1000

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

from Configurables import FilterDesktop

StdParticles = []
StdParticles.append("Phys/StdLooseKaons")
StdParticles.append("Phys/StdLooseKsLL")
StdParticles.append("Phys/StdLooseKsDD")
MakePionsEtc = FilterDesktop('MakePionsEtc')
MakePionsEtc.Inputs=StdParticles
MakePionsEtc.Code="ALL"


tupletools = []
tupletools.append("TupleToolKinematic")                                                                                                                                        
tupletools.append("TupleToolGeometry")                                                                                                                                         
tupletools.append("TupleToolTrackInfo")                                                                                                                                        
tupletools.append("TupleToolPid")                                                                                                                                              
tupletools.append("TupleToolRecoStats")  
from Configurables import DecayTreeTuple
tuple_PhiToKK = DecayTreeTuple("Tuple_PhiToKK")
tuple_PhiToKK.Inputs = [ 'Phys/PhiToKSKS_PhiToKKLine/Particles']
tuple_PhiToKK_NODTF = DecayTreeTuple("Tuple_PhiToKK_NODTF")
tuple_PhiToKK_NODTF.Inputs = [ 'Phys/PhiToKSKS_NODTF_PhiToKKLine/Particles']
for tuple in tuple_PhiToKK,tuple_PhiToKK_NODTF:
    tuple.ToolList = tupletools[:]
    tuple.Decay = "phi(1020) -> ^K+ ^K-"
    tuple.Branches = {"phi" : "phi(1020) : phi(1020) -> K+ K-",
                      "Kplus" : "phi(1020) -> ^K+ K-",
                      "Kminus" : "phi(1020) -> K+ ^K-"}

DaVinci().PrintFreq = 10000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().TupleFile = 'DVTuples.root'
DaVinci().EvtMax = -1
DaVinci().Lumi = True
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [MakePionsEtc] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ tuple_PhiToKK])
DaVinci().appendToMainSequence( [ tuple_PhiToKK_NODTF])
DaVinci().DataType = "2012"
DaVinci().InputType = 'SDST'
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"

