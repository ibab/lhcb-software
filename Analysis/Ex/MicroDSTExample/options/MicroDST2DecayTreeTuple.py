from Gaudi.Configuration import *
####################################
from Configurables import DaVinci
import GaudiKernel.SystemOfUnits as Units
############################################
from Configurables import OdinTimeDecoder, EventClockSvc
####################################
# import predefined sequences as options file
importOptions("$STDOPTS/PreloadUnits.opts")
importOptions("$STDOPTS/LHCbApplication.opts")
#############################################################

from MicroDSTExample.Selections import SeqBs2Jpsi2MuMuPhi2KK
selSequence = SeqBs2Jpsi2MuMuPhi2KK.SeqBs2Jpsi2MuMuPhi2KK
location = selSequence.outputLocation()

from Configurables import DecayTreeTuple
tuple = DecayTreeTuple("MyDTT")
tuple.InputLocations = [ location ]
tuple.ToolList +=  ["TupleToolMCTruth",
                    "TupleToolGeometry",
                    "TupleToolKinematic",
                    "TupleToolPropertime",
                    "TupleToolPrimaries",
                    "TupleToolEventInfo",
                    "TupleToolTrackInfo",
                    "TupleToolTagging"]
tuple.Decay = "[B_s0 -> (^J/psi(1S) => ^mu+ ^mu-) (^phi(1020) -> ^K+ ^K-)]cc"
# need to configure TupleMCTruth's MC association tool
from Configurables import TupleToolMCTruth
tuple.addTool(TupleToolMCTruth)
from Configurables import MCMatchObjP2MCRelator
tuple.TupleToolMCTruth.IP2MCPAssociatorType = 'MCMatchObjP2MCRelator'
tuple.TupleToolMCTruth.addTool(MCMatchObjP2MCRelator,
                               name="IP2MCPAssociatorType")
tuple.TupleToolMCTruth.IP2MCPAssociatorType.RelTableLocations = [location+'/P2MCPRelations']


seq = GaudiSequencer("TupleSeq",
                     Members = [tuple],
                     RootInTES = "/Event/MicroDST/")
seq.OutputLevel=1

dv=DaVinci()
dv.EvtMax=-1
dv.DataType="MC09"
dv.HistogramFile = "DVHistos_DTT.root"
DaVinci().TupleFile = "DVNtuples.root" 
dv.Simulation=True
dv.RedoMCLinks=False
dv.UserAlgorithms = [seq]
dv.InputType='MDST'

dv.Input   = ["DATAFILE='PFN:/afs/cern.ch/user/j/jpalac/w0/nightlies/lhcb3/DaVinci_HEAD/Ex/MicroDSTExample/cmt/Test.SeqBs2Jpsi2MuMuPhi2KK.mdst'  TYP='POOL_ROOTTREE' OPT='READ'" ]

ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
ApplicationMgr().HistogramPersistency = "ROOT"
