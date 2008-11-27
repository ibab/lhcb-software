##############################################################################
#$Id: DVTestB2DiMuon.py,v 1.7 2008-11-27 15:35:53 jpalac Exp $
#
# Example Qm test option using configurables.
#
# Author: Juan Palacios <juan.palacios@nikhef.nl>
#
##############################################################################
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer,PhysDesktop, MakeResonances, OldFilterDesktop, TrueMCFilterCriterion, MCDecayFinder, AlgorithmCorrelationsAlg, AlgorithmCorrelations, FilterDesktop

decayDescriptor = "J/psi(1S) -> mu- mu+"
mcDecayDescriptor = "J/psi(1S) => ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}"
particles = "Muons"
trueNoPIDsName = "SelectTrueNoPIDsDecay"
trueLooseName = "SelectTrueLooseDecay"

# Truth checking for Bd -> MuMuX
findTrueDecay = GaudiSequencer("FindTrueDecay")
findTrueDecay.IgnoreFilterPassed = True
findTrueDecay.Members += [ OldFilterDesktop(trueNoPIDsName) ]

# Truth filter for Bd -> MuMuK* Loose PID : copy of above with NoPIDs -> Loose
MCTruthFilter = TrueMCFilterCriterion("Filter")
MCTruthFilter.addTool(MCDecayFinder(), name="MCDecayFinder")
MCTruthFilter.MCDecayFinder.Decay = mcDecayDescriptor

selectTrueNoPIDsDecay = OldFilterDesktop(trueNoPIDsName)
selectTrueNoPIDsDecay.addTool( PhysDesktop() )
selectTrueNoPIDsDecay.PhysDesktop.InputLocations = ["StdDC06NoPIDs"+particles]
selectTrueNoPIDsDecay.addTool( MCTruthFilter, name="FilterCriterion" )

allTrueNoPIDsDecay = MakeResonances("AllTrueNoPIDsDecay")
allTrueNoPIDsDecay.addTool( PhysDesktop() )
allTrueNoPIDsDecay.PhysDesktop.InputLocations = [trueNoPIDsName]
allTrueNoPIDsDecay.DecayDescriptor = decayDescriptor
findTrueDecay.Members += [ allTrueNoPIDsDecay ]

selectTrueLooseDecay = OldFilterDesktop(trueLooseName)
selectTrueLooseDecay.addTool( PhysDesktop() )
selectTrueLooseDecay.PhysDesktop.InputLocations = [ "StdDC06Loose"+particles ]
selectTrueLooseDecay.addTool( MCTruthFilter, name = "FilterCriterion" )
findTrueDecay.Members += [ selectTrueLooseDecay ]

allTrueLooseDecay = MakeResonances("AllTrueLooseDecay")
allTrueLooseDecay.addTool( PhysDesktop() )
allTrueLooseDecay.PhysDesktop.InputLocations = [trueLooseName]
allTrueLooseDecay.DecayDescriptor = decayDescriptor
findTrueDecay.Members += [ allTrueLooseDecay ]

# Correlations
testCorrelations = AlgorithmCorrelationsAlg("TestCorrelations")
testCorrelations.addTool(AlgorithmCorrelations())
testCorrelations.AlgorithmCorrelations.OnlyNonZero = False
testCorrelations.Algorithms = [ trueNoPIDsName,
                                "AllTrueNoPIDsDecay",
                                trueLooseName,
                                "AllTrueLooseDecay",
                                "PreselB2DiMuon"]

##############################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
EventSelector().PrintFreq = 100
##############################################################################
from DaVinci.Configuration import DaVinci
##############################################################################
DaVinci().EvtMax = 1000
# DaVinci().SkipEvents = 0
# DaVinci().DataType = "DC06"
# DaVinci().DDDBtag      = ""
# DaVinci().CondDBtag    = ""
# DaVinci().UseOracle = False
DaVinci().MainOptions  = "$B2DILEPTONROOT/options/DVPreselB2DiMuon.opts"
DaVinci().UserAlgorithms = [findTrueDecay, testCorrelations]
##############################################################################
"""
 GAUDI data cards generated on 11/6/08 4:52 PM
 For Event Type = 11144103 / Data type = DST 1
     Configuration = DC06 - phys-v2-lumi2
     DST 1 datasets output 
     From DIGI 1 datasets produced by Boole - v12r10
     From SIM 1 datasets produced by ANY
     Database version = v30r14
     Cards content = physical-physical
     
 Datasets replicated at CERN
 200 dataset(s) - NbEvents = 99391
"""
DaVinci().Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001668/DST/0000/00001668_00000010_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]
##############################################################################
