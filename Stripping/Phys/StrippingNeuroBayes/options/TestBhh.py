from os import environ
from Gaudi.Configuration import *
from Configurables import CombineParticles
from Configurables import DaVinci

from GaudiKernel.SystemOfUnits import MeV, GeV


########################################################################
#
# Bs -> KK
#
from Configurables import CombineParticles
Bhh                  = CombineParticles("Bhh")
Bhh.DecayDescriptor  = "B_s0 -> K+ K-"
Bhh.DaughtersCuts    = {"K+" : "PPCUT(PP_RICHTHRES_K) & ISLONG & (TRCHI2DOF < 5) & (PT > 1.5 *GeV) & (P> 10.0*GeV) & ((PIDK < -0.1) | (PIDK > 0.1))" }
Bhh.CombinationCut   = "((AM> 5.0*GeV) & (AM< 5.9*GeV) & (AMAXDOCA('LoKi::DistanceCalculator')<0.1))"
Bhh.MotherCut        = "(VFASPF(VCHI2/VDOF) < 25) & (MAXTREE(('K+'==ABSID) ,PT) > 2.0*GeV) & (MAXTREE(('K+'==ABSID) , PIDK) > 0.1 )"


########################################################################
#
# NeuroBayes
#
from Configurables import StrippingNBBhh
BhhNB                    = StrippingNBBhh("BhhNB")
#BhhNB.OutputLevel        = 2
BhhNB.PlotHisto          = True
BhhNB.PlotMassMin        =  5.0
BhhNB.PlotMassMax        =  5.6
BhhNB.PlotNBins          = 120
BhhNB.Expertise          = 'bshhet.nb' 
BhhNB.NetworkVersion     = "TuneMC10"
BhhNB.NetworkCut         = -1.0


########################################################################
#
# Selections
#
from PhysSelPython.Wrappers import Selection, DataOnDemand
from PhysSelPython.Wrappers import MergedSelection
from PhysSelPython.Wrappers import SelectionSequence

noPIDsKaons = DataOnDemand(Location = 'Phys/StdNoPIDsKaons')

BhhSel       = Selection("BhhSel"       , Algorithm = Bhh       , RequiredSelections = [noPIDsKaons])
BhhNBSel     = Selection("BhhNBSel"     , Algorithm = BhhNB     , RequiredSelections = [ BhhSel ] )

SelSeqBhh    = SelectionSequence("SeqBhh"     , TopSelection = BhhNBSel)
SeqBhh       = SelSeqBhh.sequence()

########################################################################
#
# NTupling
#
from Configurables import DecayTreeTuple
tupleBhh                =  DecayTreeTuple("tupleBhh")
tupleBhh.InputLocations = [SelSeqBhh.outputLocation()]
tupleBhh.Decay          = "B_s0 -> K+ K-"
tupleBhh.ToolList      += [
    "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPrimaries"
    , "TupleToolTrackInfo"
    , "TupleToolPid"
    , "TupleToolEventInfo"
    , "TupleToolPropertime"
    ,"LoKi::Hybrid::TupleTool/LoKiTupleBhh" 
     ]
from Configurables import LoKi__Hybrid__TupleTool
LoKiTupleBhh = LoKi__Hybrid__TupleTool("LoKiTupleBhh")
LoKiTupleBhh.Variables = {
    "Charge"  : "Q" ,
    "DOCA"    : "DOCA(1,2)",
    "DOCAMAX" : "DOCAMAX",
    "NetOut"  : "INFO(LHCb.Particle.LastGlobal+1, -999)",
    "NetProb" : "INFO(LHCb.Particle.LastGlobal+2, -999)"
    }
tupleBhh.addTool(LoKiTupleBhh)

########################################################################
#
# DaVinci
#

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True
#CondDB(UseOracle = True)
#importOptions("$APPCONFIGOPTS/DisableLFC.py")


# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"


from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

DaVinci().HistogramFile              = "DVHistosBhhNB.root"
DaVinci().TupleFile                  = "DVTupleBhhNB.root"
DaVinci().EventPreFilters            = [ filterHLT ]
DaVinci().EvtMax                     = 5000;
#DaVinci().EvtMax                     = -1;
DaVinci().DataType                   = "2010"

DaVinci().Simulation                 = False
DaVinci().UserAlgorithms             = [ SeqBhh, tupleBhh]

DaVinci().DataType                   = "2010"
#DaVinci().InputType                  = 'SDST'
DaVinci().InputType                  = 'DST'

#MC10, Sim01
#DaVinci().DDDBtag                    = "head-20101206"
#DaVinci().CondDBtag                  = "sim-20101210-vc-md100"

#my own reconstruction        
DaVinci().DDDBtag                    = "head-20101026"
DaVinci().CondDBtag                  = "head-20101112"

# MC2010: BsKK
EventSelector().Input = [
    "DATAFILE='PFN:/afs/cern.ch/project/lbcern/vol8/ukerzel/DSTs/Brunel_81349_0x002a_MBNB_L0Phys.dst' TYP='POOL_ROOT' OPT='READ'"
    , "DATAFILE='PFN:/afs/cern.ch/project/lbcern/vol8/ukerzel/DSTs/Brunel_80881_0x002a_MBNB_L0Phys.dst' TYP='POOL_ROOT' OPT='READ'"
    , "DATAFILE='PFN:/afs/cern.ch/project/lbcern/vol8/ukerzel/DSTs/Brunel_79646_0x002a_MBNB_L0Phys.dst' TYP='POOL_ROOT' OPT='READ'"
    , "DATAFILE='PFN:/afs/cern.ch/project/lbcern/vol8/ukerzel/DSTs/Brunel_79647_0x002a_MBNB_L0Phys.dst' TYP='POOL_ROOT' OPT='READ'"
            ]


#EventSelector().Input   = [
#    "   DATAFILE='LFN:/lhcb/data/2010/SDST/00007951/0000/00007951_00001135_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
#    "   DATAFILE='LFN:/lhcb/data/2010/SDST/00007951/0000/00007951_00001136_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
#    "   DATAFILE='LFN:/lhcb/data/2010/SDST/00007951/0000/00007951_00001137_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'"
#]
#FileCatalog().Catalogs = [ 'xmlcatalog_file:$STRIPPINGSELECTIONSROOT/tests/data/pool_xml_catalog_run79646_mu2.23_tck1F0029.xml' ]
