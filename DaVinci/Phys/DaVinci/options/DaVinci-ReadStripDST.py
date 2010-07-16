#$Id: DaVinci-ReadStripDST.py,v 1.2 2010-07-16 11:59:02 jpalac Exp $
#
# Example for running on a stripping DST.
# @author Juan Palacios
# @date 2009-10-13
#
#############################################################
from Gaudi.Configuration import *
#############################################################
from Configurables import DaVinci
import GaudiKernel.SystemOfUnits as Units
#############################################################
# import predefined sequences as options file
importOptions("$STDOPTS/PreloadUnits.opts")
importOptions("$STDOPTS/LHCbApplication.opts")
#############################################################

from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence

# Need to know the address, since this is an old-style selection.
# Treat particles already on the DST as data-on-demand
JpsiSel = AutomaticData(Location = "/Event/Dimuon/Phys/StrippingJpsiInclusiveCombine")

# Filter the J/Psi
from Configurables import FilterDesktop
_jpsiFilter = FilterDesktop('jpsiFilter', Code = 'ALL')

# Add some histograms. See DaVinci tutorial 4.
from Configurables import LoKi__Hybrid__PlotTool as PlotTool

_jpsiFilter.HistoProduce = True
_jpsiFilter.addTool( PlotTool("InputPlots") )
_jpsiFilter.InputPlots.Histos = { "P/1000"  : ('momentum',0,150) ,
                                  "PT/1000" : ('pt_%1%',0,5,750) ,
                                  "M"       : ('mass in MeV_%1%_%2%_%3%',2.8*Units.GeV,3.2*Units.GeV) }
_jpsiFilter.addTool( PlotTool("OutputPlots") )
_jpsiFilter.OutputPlots.Histos = { "P/1000"  : ('momentum',0,150) ,
                                   "PT/1000" : ('pt_%1%',0,5,750) ,
                                   "M"       : ('mass in MeV_%1%_%2%_%3%',2.8*Units.GeV,3.2*Units.GeV) }
#

JpsiFilterSel = Selection(name = 'JpsiFilterSel',
                          Algorithm = _jpsiFilter,
                          RequiredSelections = [JpsiSel] )

JpsiSeq = SelectionSequence('SeqJpsi',
                            TopSelection = JpsiFilterSel)
seq = JpsiSeq.sequence()

dv=DaVinci()
dv.PrintFreq = 10
dv.HistogramFile = "DVHistos_Strip.root"
dv.EvtMax=-1
dv.DataType="MC09"
dv.Simulation=True
dv.RedoMCLinks=False
dv.UserAlgorithms = [seq]
dv.InputType='DST'
# MC09
#dv.Input = ["   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
dv.Input   = ["DATAFILE='PFN:/castor/cern.ch/user/j/jpalac/StripDST/MC09/DaVinci_v24r3p1/Sel.Jpsi.dst'  TYP='POOL_ROOTTREE' OPT='READ'" ]

ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
ApplicationMgr().HistogramPersistency = "ROOT"
