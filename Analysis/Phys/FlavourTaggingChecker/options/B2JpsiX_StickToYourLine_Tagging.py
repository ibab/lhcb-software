"""
 Script to create a DecayTreeTuple for b2JpsiX analyses:
  Bs2JpsiPhi , Bu2JpsiKplus, Bd2JpsiKstar ,
  (one can easly add Bd2JpsiKshort or Lambda_b2JpsiLambda but one should provide the proper selection
   for those channels in the selection file). 

  Input: run on the stripping locations: please check the exact location from one stripping to another!
         currently tested on DST. Should be tested on microDST
  Output: many information useful for analyse:
   - event related variables are stored in EventTuple
   - candidates related information are stored in Trees:
     kinematic, trigger, tagging, tracking, ...
     Names are the ones provided by the DecayTreeTuple framework or via LoKi Hybrid tuple.

  Options:
   1- job = 'Grid' to pass on grid mode, where there is a special recepie so the job pass with no problems,
      job = ''     to pass interactivly with real data dsts
      job = 'MC'   to pass on MC dsts: check the correct B inputLocation!
   2- Outputlevel = 1 , 2 , 3, 4 ,5 or 6 verbosity level (1 to show all messages from all tools , 6 shows just FATAL errors )
      to be set to 6 on grid, avoid unnecessary waste of time/space.
   3- Trigger(TisTos or not) tool is configuered to take a specific lines, those lines change with tcks,
      this list should be an input variable       
"""
# __author__ = "Basem Khanji, Emilie Maurice, Olivier Leroy (khanji@cppm.in2p3.fr)"


import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import (GaudiSequencer,
                           FilterDesktop,
                           CombineParticles,
                           OfflineVertexFitter,
			   FitDecayTrees
                           )
from Configurables import DaVinci

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *
from Configurables import CheckPV
from Configurables import (DecayTreeTuple,TupleToolDecay,TupleToolVtxIsoln ,
                           LoKi__Hybrid__TupleTool,TupleToolTrigger,TupleToolTagging,
                           TupleToolP2VV,TupleToolTISTOS,BTagging,
                           PrintDecayTree,PrintDecayTreeTool,TupleToolTrackPosition, TupleToolRecoStats  )


#job = 'Grid'
job = ''
#data = 'mc10'
data = 'data'
outputlevel = 6

##############################################################################################
##### SELECTIONS
##############################################################################################
seqB2JpsiX       = GaudiSequencer('seqB2JpsiX')
##############################################################################################
from Configurables import (SingleBCandidateSelection_BuJpsiK)

myselBu = SingleBCandidateSelection_BuJpsiK("SingleBCandidateSelection_BuJpsiK")
if data=='mc10':
    myselBu.InputLocation =  "/Event/AllStream/Phys/Bu2JpsiKUnbiasedLine/Particles" # mc10
elif data!='mc10':
    #myselBu.InputLocation =  "/Event/Dimuon/Phys/BetaSBu2JpsiKUnbiasedLine/Particles" # Strip 13,15
    myselBu.InputLocation =  "/Event/Dimuon/Phys/BetaSBu2JpsiKDetachedLine/Particles" # Strip 16, Strip17

Budecaydcr        = "[B+ -> (^J/psi(1S) => ^mu+ ^mu-) ^K+]cc"
BuPartVtxlocation = "Phys/SingleBCandidateSelection_BuJpsiK"
##BuPartVtxlocation =  "/Event/Dimuon/Phys/BetaSBu2JpsiKDetachedLine/Particles" # Strip 16, Strip17

##############################################################################################

# ==========================================================
# Flavour tagging Analysis
# BTaggingAnalysis ntuple creation

from Configurables import BTaggingAnalysis, TriggerTisTos, TaggingUtilsChecker

taganaBu = BTaggingAnalysis("TagAnaBu")
taganaBu.Inputs = [ BuPartVtxlocation,
                    "Phys/TaggingElectrons",
                    "Phys/TaggingMuons",
                    "Phys/TaggingPions"
                    ]

taganaBu.TagOutputLocation =  BuPartVtxlocation + "/FlavourTags"

taganaBu.ChoosePVCriterium = "bestPV"  # to be = to the FT
taganaBu.PVReFit = True
taganaBu.BHypoCriterium = "MinChi2"
taganaBu.RequireTisTos = True
if data!='mc10':
    taganaBu.EnableMC = False

taganaBu.OutputLevel = 6 #2
taganaBu.addTool( TriggerTisTos )
taganaBu.TriggerTisTos.OutputLevel = 6
taganaBu.addTool( TaggingUtilsChecker )
taganaBu.TaggingUtilsChecker.OutputLevel = 6


##############################################################################################
#TUPLE
TupleSeq = GaudiSequencer('TupleSeq')
################################################################################################
BuTuple = DecayTreeTuple("BuTuple")
BuTuple.ToolList +=  [
    "TupleToolTrigger"
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPropertime"
    , "TupleToolPrimaries"
    , "TupleToolTagging"
    , "TupleToolEventInfo"
    , "TupleToolTrackInfo"
    , "TupleToolRecoStats"
    , "TupleToolTrackPosition"
    ]
BuTuple.Inputs = [BuPartVtxlocation] #
BuTuple.Decay    = Budecaydcr
BuTuple.ReFitPVs = True

TupleToolTrackPosition = TupleToolTrackPosition('TupleToolTrackPosition')
TupleToolTrackPosition.Z = 7500.
BuTuple.addTool(TupleToolTrackPosition)

TupleToolRecoStats = TupleToolRecoStats('TupleToolRecoStats')
TupleToolRecoStats.Verbose = True
BuTuple.addTool(TupleToolRecoStats)


LoKiVariables = LoKi__Hybrid__TupleTool('LoKiVariables')
LoKiVariables.Variables = {
	"LOKI_ENERGY"     : "E",
	"LOKI_ETA"        : "ETA",
	"LOKI_PHI"        : "PHI",
	 }

BuTuple.addTool(LoKiVariables , name = 'LoKiVariables' )
BuTuple.ToolList   += [ 'LoKi::Hybrid::TupleTool/LoKiVariables']

from Configurables import TupleToolGeometry, TupleToolTrackInfo
TupleToolGeometry = TupleToolGeometry('TupleToolGeometry')
TupleToolGeometry.RefitPVs = True
BuTuple.addTool(TupleToolGeometry)

TupleToolTrackInfo = TupleToolTrackInfo('TupleToolTrackInfo')
TupleToolTrackInfo.Verbose = True
BuTuple.addTool(TupleToolTrackInfo)

# Add per Tagger infos ...:
TupleToolTagging = TupleToolTagging('TupleToolTagging')
TupleToolTagging.Verbose = True
BuTuple.addTool(TupleToolTagging)
TupleSeq.Members += [ BuTuple ]

from FlavourTagging.Tunings import TuneTool
TuneTool(BuTuple,"MC10") #for example

## ##### TRIGGER :
## #######################################################
TupleToolTrigger = TupleToolTrigger('TupleToolTrigger')
TupleToolTrigger.Verbose = True
#TupleToolTrigger.UseAutomaticTriggerList = True
#TupleToolTrigger.CollateTriggerList    = True
TupleToolTrigger.TriggerList  =[
	 "L0PhysicsDecision"
	,"L0MuonDecision"
	,"L0DiMuonDecision"
	,"L0MuonHighDecision"
	,"L0HadronDecision"
	,"L0ElectronDecision"
	,"L0PhotonDecision"
	,"Hlt1DiMuonHighMassDecision"
	,"Hlt1DiMuonLowMassDecision"
	,"Hlt1SingleMuonNoIPDecision"
	,"Hlt1SingleMuonHighPTDecision"
	,"Hlt1TrackAllL0Decision"
	,"Hlt1TrackMuonDecision"
	,"Hlt1TrackPhotonDecision"
	,"Hlt1MBNoBiasDecision"
	,"Hlt1MBMicroBiasVeloDecision"
	,"Hlt1MBMicroBiasTStationDecision"
	,"Hlt1L0AnyDecision"
	,"Hlt1DiProtonDecision"
	,"Hlt1DiProtonLowMultDecision"
	,"Hlt2SingleElectronTFLowPtDecision"
	,"Hlt2SingleElectronTFHighPtDecision"
	,"Hlt2DiElectronHighMassDecision"
	,"Hlt2DiElectronBDecision"
	,"Hlt2B2HHLTUnbiasedDecision"
	,"Hlt2Topo2BodySimpleDecision"
	,"Hlt2Topo3BodySimpleDecision"
	,"Hlt2Topo4BodySimpleDecision"
	,"Hlt2Topo2BodyBBDTDecision"
	,"Hlt2Topo3BodyBBDTDecision"
	,"Hlt2Topo4BodyBBDTDecision"
	,"Hlt2TopoMu2BodyBBDTDecision"
	,"Hlt2TopoMu3BodyBBDTDecision"
	,"Hlt2TopoMu4BodyBBDTDecision"
	,"Hlt2TopoE2BodyBBDTDecision"
	,"Hlt2TopoE3BodyBBDTDecision"
	,"Hlt2TopoE4BodyBBDTDecision"
	,"Hlt2IncPhiDecision"
	,"Hlt2IncPhiSidebandsDecision"
	,"Hlt2MuonFromHLT1Decision"
	,"Hlt2SingleMuonDecision"
	,"Hlt2SingleMuonHighPTDecision"
	,"Hlt2SingleMuonLowPTDecision"
	,"Hlt2DiMuonDecision"
	,"Hlt2DiMuonLowMassDecision"
	,"Hlt2DiMuonJPsiDecision"
	,"Hlt2DiMuonJPsiHighPTDecision"
	,"Hlt2DiMuonPsi2SDecision"
	,"Hlt2DiMuonBDecision"
	,"Hlt2DiMuonZDecision"
	,"Hlt2DiMuonDY1Decision"
	,"Hlt2DiMuonDY2Decision"
	,"Hlt2DiMuonDY3Decision"
	,"Hlt2DiMuonDY4Decision"
	,"Hlt2DiMuonDetachedDecision"
	,"Hlt2DiMuonDetachedHeavyDecision"
	,"Hlt2DiMuonDetachedJPsiDecision"
	,"Hlt2DiMuonNoPVDecision"
	,"Hlt2TriMuonDetachedDecision"
	,"Hlt2TriMuonTauDecision"
	,"Hlt2DiMuonDetachedJPsiDecision"
	,"Hlt2DiMuonDetachedDecision"
	]

TupleToolTrigger.OutputLevel = 6
BuTuple.addTool(TupleToolTrigger)

## --------------------------------- commentato perche' non va piu'
## # fine tunning to add variables to Bplus particle
TupleToolTISTOS = TupleToolTISTOS('TupleToolTISTOS')
TupleToolTISTOS.Verbose = True

#TupleToolTISTOS.UseAutomaticTriggerList = True
TupleToolTISTOS.TriggerList=[
	"Hlt1DiMuonHighMassDecision"
	,"Hlt1DiMuonLowMassDecision"
	,"Hlt1SingleMuonNoIPDecision"
	,"Hlt1SingleMuonHighPTDecision"
	,"Hlt1TrackAllL0Decision"
	,"Hlt1TrackMuonDecision"
	,"Hlt2TopoMu2BodyBBDTDecision"
	,"Hlt2TopoMu3BodyBBDTDecision"
	,"Hlt2TopoMu4BodyBBDTDecision"
	,"Hlt2MuonFromHLT1Decision"
	,"Hlt2SingleMuonDecision"
	,"Hlt2SingleMuonHighPTDecision"
	,"Hlt2SingleMuonLowPTDecision"
	,"Hlt2DiMuonDecision"
	,"Hlt2DiMuonLowMassDecision"
	,"Hlt2DiMuonJPsiDecision"
	,"Hlt2DiMuonJPsiHighPTDecision"
	,"Hlt2DiMuonPsi2SDecision"
	,"Hlt2DiMuonBDecision"
	,"Hlt2DiMuonDetachedJPsiDecision"
	,"Hlt2DiMuonDetachedDecision"
        ]

LoKi_SpecificToB=LoKi__Hybrid__TupleTool("LoKi_SpecificToB")
LoKi_SpecificToB.Variables =  {
    "LOKI_FDCHI2"          : "BPVVDCHI2",
    "LOKI_FDS"             : "BPVDLS",
    "LOKI_DIRA"            : "BPVDIRA",
    ###### DecayTreeFitVariables
    "LOKI_DTF_CTAU"        : "DTF_CTAU( 0, True )",
    "LOKI_DTF_CTAUS"       : "DTF_CTAUSIGNIFICANCE( 0, True )",
    "LOKI_DTF_CHI2NDOF"    : "DTF_CHI2NDOF( True )",
    "LOKI_DTF_CTAUERR"     : "DTF_CTAUERR( 0, True )",
    "LOKI_MASS_JpsiConstr" : "DTF_FUN ( M , True , 'J/psi(1S)' )" ,
    "LOKI_DTF_VCHI2NDOF"   : "DTF_FUN ( VFASPF(VCHI2/VDOF) , True )",
    #"LOKI_DTF_TAU"         : "DTF_FUN ( BPVLTIME('PropertimeFitter/properTime:PUBLIC') , True)"
        }


###########################################################
##################### BuTuple #############################
BuTuple.Branches={
    "Bplus" : "[B+]cc : [B+ -> (J/psi(1S) => mu+ mu-) K+]cc"
            }
BuTuple.addTool(TupleToolDecay, name="Bplus")
BuTuple.Bplus.ToolList+=["LoKi::Hybrid::TupleTool/LoKi_SpecificToB"]
BuTuple.Bplus.addTool(LoKi_SpecificToB)

BuTuple.addTool(TupleToolGeometry)

BuTuple.Bplus.addTool(TupleToolTISTOS)
BuTuple.Bplus.TupleToolTISTOS.addTool( TriggerTisTos, "TriggerTisTos" )
BuTuple.Bplus.TupleToolTISTOS.TriggerTisTos.TOSFracMuon = 0.
BuTuple.Bplus.ToolList+=["TupleToolTISTOS"]
###########################################################
###########################################################

TupleSeq.ModeOR          = True
TupleSeq.ShortCircuit    = False

###########################################################
seqB2JpsiX.Members      += [ TupleSeq]
seqB2JpsiX.ModeOR        =   True
seqB2JpsiX.ShortCircuit  =   False

from Configurables import EventTuple , TupleToolTrigger
evtTuple                = EventTuple()
evtTuple.ToolList      += [ "TupleToolEventInfo", "TupleToolTrigger"]
#DaVinci().EvtMax        =  -1
DaVinci().EvtMax        =  5000
DaVinci().SkipEvents    =  -1
DaVinci().DataType      = '2011'

from Configurables import CondDB
if job=='Grid':
    CondDB(UseOracle = True)
    if data=='data':
        CondDB().UseLatestTags = ["2011"]

# MC
if data=='mc10':
    DaVinci().DataType      = '2010'
    DaVinci().Simulation = True
    DaVinci().DDDBtag   = "head-20100407"
    DaVinci().CondDBtag = "sim-20100429-vc-md100"
    DaVinci().Lumi = False

elif data=='data':
    DaVinci().Lumi=True
    # New condidtion Data base look at the bookeeping to check :
    # Strip 13
    # DaVinci().DDDBtag         = "head-20110302"
    # DaVinci().CondDBtag       = "head-20110331"
    # Strip 13b
    #DaVinci().DDDBtag         = "head-20110302"
    #DaVinci().CondDBtag       = "head-20110524"
    # Strip 15 - Reco11
    #DaVinci().DDDBtag         = "head-20110302"
    #DaVinci().CondDBtag       = "head-20110622"
    # Strip 17 - Reco12
    DaVinci().DDDBtag   = "head-20110914"
    DaVinci().CondDBtag = "head-20110914"
    # run HLT2 on Data
#    DaVinci().Hlt             = True
#    DaVinci().Hlt2Requires    = 'L0+Hlt1'
#    DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10'


DaVinci().TupleFile       = "DTT.root"  # Ntuple
DaVinci().UserAlgorithms += [myselBu, evtTuple , seqB2JpsiX, taganaBu ]
from Gaudi.Configuration import *

EventSelector().Input   = [
#    " DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008932/0000/00008932_00000001_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000001_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

"""
EventSelector().Input   = [
  #Dimuon Strip13b
 #"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00010829/0000/00010829_00003474_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
 #"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00010829/0000/00010829_00003475_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
 #"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00010829/0000/00010829_00003476_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'"


    ## Strip 17
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000001_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000002_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000003_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000004_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000005_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000006_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000007_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000008_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000009_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000010_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000011_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000012_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000013_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000014_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000015_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000016_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000017_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000018_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000019_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00012549/0000/00012549_00000020_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'"

  ]

"""
