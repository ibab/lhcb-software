#######################################################################
# D0 -> K pi
#
# @author Jack Wimberley
# @date 2013-06-06
########################################################################

from Gaudi.Configuration import *
from Configurables import DaVinci
from GaudiKernel.SystemOfUnits import *

# OPTIONS
MessageSvc().OutputLevel = INFO
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1000
DaVinci().HistogramFile = "D02KPi_histos.root"
DaVinci().TupleFile = "D02KPi.root"

DaVinci().DataType = "2012"
from Configurables import CondDB
CondDB().UseLatestTags  = ["2012"]

# DATA
from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence
LooseD02KPiLoc = 'Phys/StdLooseD02KPi/Particles'
LooseD02KPiSel = AutomaticData(Location = LooseD02KPiLoc)

# FILTER
from PhysConf.Filters import LoKi_Filters
Hlt2Filt = LoKi_Filters(
    HLT_Code = "HLT_PASS( 'Hlt2CharmHadD02HH_D02KPiDecision' )"
    )
TriggerFilter = Hlt2Filt.sequence("TriggerFilter")

# RECONSTRUCTION
from CommonParticles.StdLooseDstarWithD2HH import StdLooseDstarWithD02KPi
StdLooseDstarWithD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (abs(M-MAXTREE('D0'==ABSID,M)-145.421)<4)"
# This could alternatively be done as a FilterDesktop on StdLooseDstarWithD02KPi
# That could be faster; but right now it works and I don't want to mess with it

from Configurables import FilterDesktop
_AlgD02KPi = FilterDesktop("AlgD02KPi")
_AlgD02KPi.Code = "(ADMASS('D0') < 50.*MeV) & (VFASPF(VCHI2) < 10.)"
_AlgD02KPi.Code += " & (PT > 3.5*GeV)" # put in a good trigger line
_AlgD02KPi.Code += " & (MINTREE('K-'==ABSID, PIDK) > 0) & (MAXTREE('pi+'==ABSID, PIDK) < 0)"
D0Sel = Selection("D0Sel",
                  Algorithm = _AlgD02KPi,
                  RequiredSelections = [ LooseD02KPiSel ])
D0Seq = SelectionSequence("D0Seq",
                          TopSelection = D0Sel)

from Configurables import GaudiSequencer
recseq = GaudiSequencer("Sequence")
finalsel = D0Seq.sequence()
recseq.Members = [ TriggerFilter, StdLooseDstarWithD02KPi, finalsel ]

# THERE CAN ONLY BE ONE
code = "CONTAINS('" + D0Seq.outputLocation() + "')==1"
onefilt = LoKi_Filters( VOID_Code = code )
HIGHLANDER = onefilt.sequence('HIGHLANDER')
recseq.Members += [ HIGHLANDER ]

# DSTAR TUPLE
#from Configurables import DecayTreeTuple
#from DecayTreeTuple.Configuration import *
#DstarTuple = DecayTreeTuple("Dstar_Tuple")
#DstarTuple.Inputs = [ "Phys/StdLooseDstarWithD02KPi/Particles" ]
#DstarTuple.Decay = "[D*(2010)+ -> (^D0 => ^K- ^pi+) ^pi+]cc"
#recseq.Members += [ DstarTuple ]

# D0 TUPLE
#D0Tuple = DecayTreeTuple("D0_Tuple")
#D0Tuple.Inputs = [ D0Seq.outputLocation() ]
#D0Tuple.Decay = "[D0 -> ^K- ^pi+]cc"
#recseq.Members += [ D0Tuple ]

# PRE MONITOR
from Configurables import MassBiasesMonitor
premon = MassBiasesMonitor('PREMON',
                           InputLocation = D0Seq.outputLocation(),
                           MinMass = 1.82*GeV, MaxMass = 1.91*GeV,
                           BinsMom = 10, MinMom = 0*GeV, MaxMom = 200*GeV,
                           MomDistr = "GAUSEXPO", MomParams = [39.3457*GeV,8.74601*GeV,0.0292012/GeV],
                           BinsPt = 10, MinPt = 3.5*GeV, MaxPt = 20*GeV, PtDistr = "EXPO", PtParams = [0.428957/GeV],
                           BinsMomDif = 10, MaxMomDif = 100*GeV, MomDifDistr = "EXPO", MomDifParams = [0.0485564/GeV],
                           BinsAsym = 10, AsymDistr = "ASYM", AsymParams = [-1.06162,-1.6415,1.80802],
                           BinsEta = 10, MinEta = 2, MaxEta = 5, EtaDistr = "GAMMA", EtaParams = [47.2643,0.0668159,0],
                           BinsTx = 10, MaxTx = 0.2, TxDistr = "GAMMA", TxParams = [1.96466,3.30509e-2,0],
                           BinsTy = 10, MaxTy = 0.2, TyDistr = "GAMMA", TyParams = [1.96466,3.30509e-2,0],
                           BinsOpenAngle = 10, MinOpenAngle = 0, MaxOpenAngle = 0.2,
                           OpenAngleDistr = "GAMMA", OpenAngleParams = [5.5199,1.10887e-2,0]
                           )

# REFITTER
from DaVinciTrackRefitting.ParticleRefitter import ParticleRefitterSeq
updater = ParticleRefitterSeq(inputs=[ D0Seq.outputLocation() ], scale = True)

# POST MONITOR
postmon = MassBiasesMonitor('POSTMON',
                           InputLocation = D0Seq.outputLocation(),
                           MinMass = 1.82*GeV, MaxMass = 1.91*GeV,
                           BinsMom = 10, MinMom = 0*GeV, MaxMom = 200*GeV,
                           MomDistr = "GAUSEXPO", MomParams = [39.3457*GeV,8.74601*GeV,0.0292012/GeV],
                           BinsPt = 10, MinPt = 3.5*GeV, MaxPt = 20*GeV, PtDistr = "EXPO", PtParams = [0.428957/GeV],
                           BinsMomDif = 10, MaxMomDif = 100*GeV, MomDifDistr = "EXPO", MomDifParams = [0.0485564/GeV],
                           BinsAsym = 10, AsymDistr = "ASYM", AsymParams = [-1.06162,-1.6415,1.80802],
                           BinsEta = 10, MinEta = 2, MaxEta = 5, EtaDistr = "GAMMA", EtaParams = [47.2643,0.0668159,0],
                           BinsTx = 10, MaxTx = 0.2, TxDistr = "GAMMA", TxParams = [1.96466,3.30509e-2,0],
                           BinsTy = 10, MaxTy = 0.2, TyDistr = "GAMMA", TyParams = [1.96466,3.30509e-2,0],
                           BinsOpenAngle = 10, MinOpenAngle = 0, MaxOpenAngle = 0.2,
                           OpenAngleDistr = "GAMMA", OpenAngleParams = [5.5199,1.10887e-2,0]
                           )

# SEQUENCES
from Configurables import GaudiSequencer
preseq = GaudiSequencer('preseq')
preseq.Members = [ premon ]
postseq = GaudiSequencer('postseq')
postseq.Members = [ updater, postmon ]
recseq.Members += [ preseq, postseq ]
DaVinci().appendToMainSequence( [ recseq ] )
