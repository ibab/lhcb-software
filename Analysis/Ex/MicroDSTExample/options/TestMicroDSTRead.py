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

from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

particleLocation = selSequence.outputLocation()
print 'PARTICLE LOCATION: ', particleLocation

BsSel = DataOnDemand('DOD_Bs',
                     Location = particleLocation)

from Configurables import FilterDesktop
_bsFilter = FilterDesktop('_bsFilter',
                          Code = 'ALL',
                          RootInTES = '/Event/MicroDST/')
from Configurables import LoKi__Hybrid__PlotTool as PlotTool

_bsFilter.HistoProduce = True
_bsFilter.addTool( PlotTool("InputPlots") )
# Note that it's using the same functors as above. Hence the same syntax.
_bsFilter.InputPlots.Histos = { "P/1000"  : ('momentum',0,500) ,
                                "PT/1000" : ('pt_%1%',0,5,1000) ,
                                "M"       : ('mass in MeV_%1%_%2%_%3%',5.2*Units.GeV,5.6*Units.GeV) }
_bsFilter.addTool( PlotTool("OutputPlots") )
_bsFilter.OutputPlots.Histos = { "P/1000"  : ('momentum',0,500) ,
                                   "PT/1000" : ('pt_%1%',0,5,1000) ,
                                   "M"       : ('mass in MeV_%1%_%2%_%3%',5.2*Units.GeV,5.6*Units.GeV) }
_bsFilter.OutputLevel = 1

BsFilterSel = Selection('HelloWorld',
                        Algorithm = _bsFilter,
                        RequiredSelections = [BsSel] )

from Configurables import CheckPV
BsSeq = SelectionSequence('Bs', TopSelection = BsFilterSel,
                          EventPreSelector = [CheckPV()])
seq = BsSeq.sequence()
seq.RootInTES = "/Event/MicroDST/"
seq.OutputLevel=4

dv=DaVinci()
dv.EvtMax=-1
dv.DataType="MC09"
dv.HistogramFile = "DVHistos_MDST.root"
dv.Simulation=True
dv.RedoMCLinks=False
dv.UserAlgorithms = [seq]
dv.InputType='MDST'
mainSeq = GaudiSequencer("DaVinciMainSequence")
mainSeq.RootInTES = "/Event/MicroDST/"
# some necessary framework stuff

ecs = EventClockSvc()
ecs.addTool(OdinTimeDecoder)
ecs.OdinTimeDecoder.RootInTES = "/Event/MicroDST/"


dv.Input   = ["DATAFILE='PFN:/afs/cern.ch/user/j/jpalac/w0/nightlies/lhcb3/DaVinci_HEAD/Ex/MicroDSTExample/cmt/Test.SeqBs2Jpsi2MuMuPhi2KK.mdst'  TYP='POOL_ROOTTREE' OPT='READ'" ]

ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
ApplicationMgr().HistogramPersistency = "ROOT"
