from os import environ
from Gaudi.Configuration import *
from Configurables import CombineParticles
from Configurables import DaVinci

from GaudiKernel.SystemOfUnits import MeV, GeV


########################################################################
#
# J/psi -> mu mu
#
from Configurables import CombineParticles
JPsi                  = CombineParticles("JPsi")
JPsi.DecayDescriptor  = "J/psi(1S) -> mu+ mu-"
JPsi.DaughtersCuts    = {"mu+" : "(TRCHI2DOF<10) & (ISMUON)" }
JPsi.CombinationCut   = "(AM>2.9*GeV) & (AM< 3.3*GeV)"
JPsi.MotherCut        = "(VFASPF(VCHI2/VDOF)<25) &(MM > 3.0*GeV) & (MM < 3.2*GeV)"

MuMu                  = CombineParticles("MuMu")
MuMu.DecayDescriptor  = "J/psi(1S) -> mu+ mu-"
MuMu.DaughtersCuts    = {"mu+" : "(TRCHI2DOF<10) & (ISMUON)" }
MuMu.CombinationCut   = "(AM>0.0*GeV) & (AM< 12.1*GeV)"
MuMu.MotherCut        = "(VFASPF(VCHI2/VDOF)<25) &(MM > 0.0*GeV) & (MM < 12.0*GeV)"

########################################################################
#
# NeuroBayes
#
from Configurables import StrippingNBMuMu
jPsiNB                    = StrippingNBMuMu("jPsiNB")
#jPsiNB.OutputLevel        = 2
jPsiNB.PlotHisto          = True
jPsiNB.PlotMassMin        =  3.0
jPsiNB.PlotMassMax        =  3.2
jPsiNB.PlotNBins          = 120
jPsiNB.Expertise          = 'Muon/mumu_net_noip.nb' 
jPsiNB.NetworkVersion     = "TuneSep2010"
jPsiNB.NetworkCut         = 0.75


mumuNB                    = StrippingNBMuMu("mumuNB")
#mumuNB.OutputLevel        = 2
mumuNB.PlotHisto          = True
mumuNB.PlotMassMin        =  0.0
mumuNB.PlotMassMax        = 12.0
mumuNB.PlotNBins          = 1000
mumuNB.Expertise          = 'Muon/mumu_net_full.nb' 
mumuNB.NetworkVersion     = "TuneSep2010"
mumuNB.NetworkCut         = 0.95


########################################################################
#
# Selections
#
from PhysSelPython.Wrappers import Selection, DataOnDemand
from PhysSelPython.Wrappers import MergedSelection
from PhysSelPython.Wrappers import SelectionSequence

muons        = DataOnDemand('Phys/StdLooseMuons')
jPsiSel      = Selection("jPsiSel"   , Algorithm = JPsi   , RequiredSelections = [ muons   ] )
mumuSel      = Selection("mumuSel"   , Algorithm = MuMu   , RequiredSelections = [ muons   ] )
jPsiNBSel    = Selection("jPsiNBSel" , Algorithm = jPsiNB , RequiredSelections = [ jPsiSel ] )
mumuNBSel    = Selection("mumuNBSel" , Algorithm = mumuNB , RequiredSelections = [ mumuSel ] )

SelSeqJPsi   = SelectionSequence("SeqJPsi"    , TopSelection = jPsiNBSel)
SelSeqMuMu   = SelectionSequence("SeqMuMu"    , TopSelection = mumuNBSel)

SeqJPsi      = SelSeqJPsi.sequence()
SeqMuMu      = SelSeqMuMu.sequence()



########################################################################
#
# DaVinci
#

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"


from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

DaVinci().HistogramFile              = "DVHistosNeuroBayes.root"
DaVinci().TupleFile                  = "DVTupleNeuroBayes.root"
DaVinci().EventPreFilters            = [ filterHLT ]
#DaVinci().EvtMax                     = 5000;
DaVinci().EvtMax                     = -1;
DaVinci().DataType                   = "2010"

DaVinci().Simulation                 = False
DaVinci().UserAlgorithms             = [ SeqJPsi, SeqMuMu ]

DaVinci().DataType                   = "2010"
DaVinci().InputType                  = 'SDST'

DaVinci().DDDBtag                    = "head-20101003" #"head-20100518" # "head-20100407"
DaVinci().CondDBtag                  = "head-20101010" #"head-20100730" # "head-20100509"

#EventSelector().Input   = [
#    "   DATAFILE='LFN:/lhcb/data/2010/SDST/00007951/0000/00007951_00001135_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
#    "   DATAFILE='LFN:/lhcb/data/2010/SDST/00007951/0000/00007951_00001136_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
#    "   DATAFILE='LFN:/lhcb/data/2010/SDST/00007951/0000/00007951_00001137_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'"
#]
#FileCatalog().Catalogs = [ 'xmlcatalog_file:$STRIPPINGSELECTIONSROOT/tests/data/pool_xml_catalog_run79646_mu2.23_tck1F0029.xml' ]
