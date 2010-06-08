########################################################################
#
# Options for exercise 8
#
# @author Patrick Koppenburg
# @date 2010-06-07
#
########################################################################

from Gaudi.Configuration import *

location = '/Event/Radiative/Phys/SelBd2KstGamma' 

MessageSvc().Format = "% F%80W%S%7W%R%T %0W%M"
from Configurables import DaVinci, PrintDecayTree, GaudiSequencer
from Configurables import LoKi__HDRFilter   as StripFilter
MySequencer = GaudiSequencer('Sequence')
sf = StripFilter( 'StripPassFilter', Code="HLT_PASS('StrippingBd2KstGammaDecision')", Location="/Event/Strip/Phys/DecReports" )
pt = PrintDecayTree(InputLocations = [ location ])

# get classes to build the SelectionSequence
from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence
# Get the Candidates from the DST.
# Treat particles already on the DST as data-on-demand, but give the full path.
KstgammaSel = AutomaticData(Location = location)
# Filter the Candidate. Replace 'ALL' by some cuts.
from Configurables import FilterDesktop
_kstgammaFilter = FilterDesktop('kstgammaFilter', Code = 'ALL')

# make a Selection
KstgammaFilterSel = Selection(name = 'KstgammaFilterSel',
                          Algorithm = _kstgammaFilter,
                          RequiredSelections = [ KstgammaSel])

# build the SelectionSequence
KstgammaSeq = SelectionSequence('SeqKstgamma',
                                TopSelection = KstgammaFilterSel,
                                EventPreSelector = [ sf ])

############# DecayTreeTuple
from Configurables import DecayTreeTuple, TupleToolTrigger, TupleToolDecay, TupleToolTISTOS
tuple = DecayTreeTuple("KstarGamma_Tuple")
tuple.ToolList +=  [
    "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "TupleToolTrackInfo"
    , "TupleToolTrigger"
    , "TupleToolTISTOS"
    , "TupleToolAngles"
    , "TupleToolPid"
    , "TupleToolPropertime"
    ]
tuple.Decay = "[B0 -> (^K*(892)0 -> ^K+ ^pi-) ^gamma]cc"
tuple.InputLocations = [ 'KstgammaFilterSel' ]
tuple.addTool(TupleToolTrigger)
tuple.TupleToolTrigger.VerboseL0 = True
tuple.TupleToolTrigger.VerboseHlt1 = True
tuple.TupleToolTrigger.UseAutomaticTriggerList = True
tuple.addTool(TupleToolTISTOS)
tuple.TupleToolTISTOS.TriggerList = [ "Hlt1DiHadronDecision", "Hlt1PhotonDiTrackDecision", "Hlt1SingleHadronDecision" ]
tuple.TupleToolTISTOS.VerboseHlt1 = True
##################

DaVinci().appendToMainSequence( [ KstgammaSeq.sequence(), tuple ] ) # pt
EventSelector().Input   = [
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000001_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000004_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000005_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000007_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000008_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000009_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000017_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000019_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000022_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000028_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000029_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000034_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000036_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000039_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000043_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000044_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000045_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000050_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000052_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000054_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/RADIATIVE.DST/00006591/0000/00006591_00000055_1.radiative.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'"]

DaVinci().DataType = "2010"
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 10
DaVinci().TupleFile = "KstGamma.root"

"""
TTree* T = _file0->Get("KstarGamma_Tuple/DecayTree")
SandB(T,T,"log(Kplus_IP_OWNPV)/log(10.)","KplusHlt1SingleHadronDecision_TOS","KplusHlt1SingleHadronDecision_TIS","Hlt1SingleHadronDecision")
SandB(T,T,"max(Kplus_PT,piminus_PT)","B0Hlt1SingleHadronDecision_TOS","B0Hlt1SingleHadronDecision_TOS==0","B0Hlt1SingleHadronDecision_TOS || B0Hlt1SingleHadronDecision_TIS")
"""
