
##############################################################################
# File for running Brunel on real data with default 2008 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#
#   gaudirun.py $ESCHEROPTS/AlignOTCosmics.py $ESCHEROPTS/2008-Cosmic-Data.py
#
# or
#
#   $ESCHEROPTS/gaudiiter.py -n NUMITER -e NUMEVENTS $ESCHEROPTS/AlignOTCosmics.py $ESCHEROPTS/2008-Cosmic-Data.py
#
##############################################################################

from Configurables import ( Escher, TrackSys, RecSysConf )

Escher().DataType     = "2008"
Escher().Kalman = True
Escher().Millepede = False
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = 100000
Escher().SpecialData = [ "fieldOff", "cosmics"]
TrackSys().ExpertTracking += [ "noDrifttimes", "noMaterialCorrections", "simplifiedGeometry" ]
TrackSys().TrackPatRecAlgorithms = [ "PatSeed" ]
TrackSys().TrackExtraInfoAlgorithms = ['']

RecSysConf().RecoSequence = ["TT","IT","OT","Tr"]
#RecSysConf().RecoSequence = ["OT","Tr"]


###############################################################


from GaudiKernel.SystemOfUnits import GeV, mm
from Gaudi.Configuration import *

useDriftTime        = False #
FixedStations       = [] #['M2','M4'] #'M1M5'
mergeOTBX           = True
uselocalframe       = True

chisqconstraints    = [] # un numero in corrispondenza di ogni constraint
constraints         = []

outputlevel         = INFO #DEBUG #INFO
newNNET             = True
maskNoise           = True # per eliminare i canali rumorosi
dof                 = 'TxTy'
#nameCond            = 'Cosmics_MuonOT_TxTy_newNNET_0.05_cutChi2_wouter1' # job 47  (no XTalk)
nameCond            = 'prova'
# matching ntuple

MuonMatching        = True #selezione delle tracce con MuonNNet e richiesta di match con OT/Calo
OTMuonMatching      = True #selezione delle tracce con MuonNNet e richiesta di match con OT/Calo
OTMuon_ntuple       = False #True

# configure for half-Stations
from TAlignment.Alignables import *
elements           = Alignables()
#elements.Muon('Tx') ;

#elements.MuonHalfStations(dof) ;

elements.MuonHalfStationsCside(dof) ;
elements.MuonHalfStationsAside(dof) ;


# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment

from Configurables import TAlignment,GetElementsToBeAligned
TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
TAlignment().Constraints          = constraints
TAlignment().SurveyConstraints    = chisqconstraints
TAlignment().WriteCondSubDetList  = ['Muon','OT']
TAlignment().CondFilePrefix       = 'MyXml/'
TAlignment().OutputLevel                  = outputlevel # ok
TAlignment().UseLocalFrame                = uselocalframe 

elementtool = GetElementsToBeAligned( "GetElementsToBeAligned" ) # aggiunta di Silvia ??
elementtool.UseLocalFrame = False # aggiunta di Silvia ??

# add all 'private' reconstruction to this sequence which is automatically added to the alignment sequence.
from Configurables import GaudiSequencer
trackFilterSeq = GaudiSequencer("TrackFilterSeq")


# nuova geometria OT -------------------------------------------------------------------------

# per agganciare il CondDB con le info sulle costanti di allineamento -------------------------

from Configurables import ( CondDBAccessSvc,CondDB )
AlignmentCondition = CondDBAccessSvc("AlignmentCondition")
#AlignmentCondition.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/OTHalfLayers.db/LHCBCOND"
#######AlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/scratch0/LHCBCOND-surveyfinal1.db/LHCBCOND'
#AlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/spozzi/scratch0/Databases/prove/nuovoGlobalCONDITIONS2.db/LHCBCOND'
#AlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/lhcb/scratch/vol2/svecchi/LHCBCOND-new2stefi.db/LHCBCOND'
#AlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/LHCBCOND-last.db/LHCBCOND'
AlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/LHCBCOND-new5.db/LHCBCOND'
#   /lhcbcondPROVA.db/LHCBCOND'
CondDB().addLayer( AlignmentCondition )

OTAlignmentCondition = CondDBAccessSvc("OTAlignmentCondition")
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/ScenarioSurveyXYZ.db/LHCBCOND'
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/ScenarioSurveyAll.db/LHCBCOND'
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/OTCosmicsCFrameAlignmentTxTyTz2009.db/LHCBCOND'
#OTAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/OTCosmicsModulesTxCFramesTxTyTzRz2009.db/LHCBCOND'
#CondDB().addLayer(OTAlignmentCondition)

myDDDB = CondDBAccessSvc( 'myDDDB' )
#myDDDB.ConnectionString = 'sqlite_file:/afs/cern.ch/lhcb/scratch/vol2/svecchi/myDDDB-WanderFinal.db/DDDB'
myDDDB.ConnectionString = 'sqlite_file:/afs/cern.ch/lhcb/scratch/vol2/svecchi/myDDDB-WanderDefinitive.db/DDDB'
CondDB().addLayer(myDDDB)
   
# create a reconstruction sequence  -----------------------------------------

### ????????????????? che serve ????????????????????????????????????
#from Configurables import (PatSeeding, PatSeedingTool, TrackRemoveOddOTClusters,
#                           TrackMonitor,TrackContainerCopy)
#PatSeeding = PatSeeding("PatSeeding") # ok
#if TrackSys().getProp("fieldOff"): # ok
#   PatSeeding.addTool(PatSeedingTool, name="PatSeedingTool") # ok
#   PatSeeding.PatSeedingTool.zMagnet = 0. # ok
#   PatSeeding.PatSeedingTool.FieldOff = True # ok
#importOptions('$PATALGORITHMSROOT/options/PatSeedingTool-Cosmics.opts') #ok
#PatSeeding.PatSeedingTool.MaxOTOccupancy         = 0.01
#PatSeeding.PatSeedingTool.MaxITOccupancy         = 0.01
#PatSeeding.PatSeedingTool.CommonXFraction        = 0.
#PatSeeding.PatSeedingTool.MaxUsedFractPerRegion  = 0.
#PatSeeding.PatSeedingTool.MaxUsedFractITOT       = 0.
#PatSeeding.PatSeedingTool.MaxUsedFractLowQual    = 0.
#PatSeeding.PatSeedingTool.OutputLevel    = outputlevel


# ================================================================================================
# fit the track with straight line fit
from TrackFitter.ConfiguredFitters import (ConfiguredStraightLineFitter) # ok
from Configurables import (TrackKalmanFilter, TrajOTCosmicsProjector,TrajOTProjector,TrackProjectorSelector,
                           MeasurementProvider) # ok


# ================================================================================================
# make a selection based on the chisquare

from Configurables import (TrackSelector,TrackContainerCopy)
seedselectoralg  = TrackContainerCopy("SeedSelection")
seedselectoralg.addTool( TrackSelector("SeedSelector"), name = "Selector") ;
seedselectoralg.inputLocation =   'Rec/Track/Seed'
seedselectoralg.outputLocation =   'Rec/Track/SelectedSeed'
seedselectoralg.Selector.MaxChi2Cut = 5
#seedselectoralg.Selector.MinNOTHits = 12
trackFilterSeq.Members.append( seedselectoralg )

# ======================================================
# Muon cosmics track reconstruction
from TrackFitter.ConfiguredFitters import ( ConfiguredStraightLineFitter )
from Configurables import MuonMonHitDecode, MuonPadRec  #MuonMonHitDecode
from  Configurables import ( MuonNNetRec, MuonClusterRec, CopyMuonNNetTracks, MeasurementProvider,   # MuonID
                             TrackMasterExtrapolator, TrackKalmanFilter, 
                             TrackChi2Calculator, OTMuonCosmicsMatchingSilvia )
# Copy MuonNNet tracks to LHCb tracks
copy                                         = CopyMuonNNetTracks( 'CopyMuonNNetTracks' )
trackFilterSeq.Members.append( copy )  # nella sequenza

copy.OutputLevel                             = outputlevel
copy.addTool( MuonNNetRec, name              = 'MuonRecTool' )
copy.MuonRecTool.TrackSpanCut                = 2.0 ## cut on span for selected tracks
copy.MuonRecTool.StationFiringCut            = 2.0 ## cut on # of stations firing in the track
copy.MuonRecTool.AllowHoles                  = True ## allow holes in tracks
copy.MuonRecTool.MaxNeurons                  = 1000 ## (def. 3000) max number of possible track segments
copy.MuonRecTool.MaxIterations               = 100 ## (def. 100) max number of NN iterations
copy.MuonRecTool.AssumeCosmics               = True
copy.MuonRecTool.PhysicsTiming               = False
copy.MuonRecTool.AssumePhysics               = False ## assume that tracks come from IP (only forward)
copy.MuonRecTool.AddXTalk                    = False
#copy.MuonRecTool.SkipStation      = 0
copy.MuonRecTool.OutputLevel = outputlevel

if newNNET: #-------------------------------------------------------------------
   copy.MuonRecTool.addTool(MuonClusterRec, name= "ClusterTool") # Nuovo NNET

#--------------------------------------------------------------------------------
copy.MuonRecTool.addTool(MuonMonHitDecode, name = "DecodingTool" )
copy.MuonRecTool.DecodingTool.OutputLevel = WARNING

if maskNoise:
   copy.MuonRecTool.DecodingTool.LocalVeto = [] # 21 Ottobre
   
copy.MuonRecTool.addTool(MuonPadRec, name = "PadRecTool")
copy.MuonRecTool.PadRecTool.TileVeto = [ 137476416, 137476417, 137476418, 137476419, 137476432, 137476433, 137476434, 137476435, 137490509, 137490525, 137490749, 137490765, 137490781, 137490813, 137492510, 137492526, 137492542, 137492558, 137492574, 137492702, 137492718, 137492734, 137492750, 137492781, 137492782, 137492797, 137492798, 137492813, 137492814, 137492829, 137492830, 137492845, 137492861, 137511189, 137517068, 137517084, 137517100, 137517116, 137517180, 137517194, 137517210, 137517226, 137517242, 137521347, 137523516, 137523532, 137523548, 137525275, 137525291, 137525307, 137525323, 137525339, 137525375, 137525391, 137525536, 137525546, 137525552, 137525562, 137525568, 137525578, 137525584, 137525594, 137525600, 137525610, 146885786, 146885987  ] # maskNoisy.opts_2Nov_0.05


# ------------------------------------------------------------------
   
# Refit ================================================================================
muonTrackFit =  ConfiguredStraightLineFitter( 'MuonTrackFit' , TracksInContainer = 'Rec/Track/Muon' ) # fitta le tracce MuonNNET
muonTrackFit.Fitter.OutputLevel             = outputlevel
muonTrackFit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
muonTrackFit.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
muonTrackFit.Fitter.MeasProvider.IgnoreMuon = False 
muonTrackFit.Fitter.ErrorX                  = 1000
muonTrackFit.Fitter.ErrorY                  = 10000
muonTrackFit.Fitter.MaxNumberOutliers       = 0

# ------------------------------------------------------------------
trackFilterSeq.Members.append( muonTrackFit )
# ------------------------------------------------------------------

if MuonMatching:   
   # OT Matching
   matching = OTMuonCosmicsMatchingSilvia( 'OTMuonCosmicsMatchingSilvia' )
   matching.AllCombinations                                = False
   matching.MatchAtFirstMuonHit                            = True
   matching.FullDetail                                     = True
   matching.OutputLevel                                    = outputlevel
   matching.TTracksLocation                                = 'Rec/Track/Seed' # patSeeding
   matching.MuonTracksLocation                             = 'Rec/Track/Muon' # MuonNNet
   matching.TracksOutputLocation                           = 'Rec/Track/Best/TMuon'
   matching.addTool( TrackMasterExtrapolator, name         = 'Extrapolator' )
   matching.Extrapolator.ApplyMultScattCorr                = False
   matching.Extrapolator.ApplyEnergyLossCorr               = False
   matching.Extrapolator.ApplyElectronEnergyLossCorr       = False
   matching.addTool( TrackChi2Calculator, name             = 'Chi2Calculator' )
   matching.MatchChi2Cut                                   = 100.0 #
   matching.WriteNtuple                                    = OTMuon_ntuple
   
   
   # Fit TMuon tracks
   tmuonfit = ConfiguredStraightLineFitter( 'TMuonFit', TracksInContainer = 'Rec/Track/Best/TMuon') # contiene OT+Muon
   tmuonfit.OutputLevel                                    = outputlevel
   tmuonfit.Fitter.MakeNodes                               = True
   tmuonfit.Fitter.MakeMeasurements                        = True
   tmuonfit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
   tmuonfit.Fitter.ErrorX                                 = 100
   tmuonfit.Fitter.ErrorY                                 = 100
   tmuonfit.Fitter.MaxNumberOutliers                       = 0
   tmuonfit.Fitter.addTool( MeasurementProvider(), name    = 'MeasProvider')
   tmuonfit.Fitter.MeasProvider.IgnoreMuon = False    
   
   if OTMuonMatching :
      trackFilterSeq.Members.append( matching )
      trackFilterSeq.Members.append( tmuonfit )
   # ------------------------------------------------------------------

from Configurables import (TrackSelector,TrackContainerCopy)
tmuonselectoralg  = TrackContainerCopy( "TmuonSelection" )
tmuonselectoralg.addTool( TrackSelector, name = "Selector")
tmuonselectoralg.Selector.MaxChi2Cut = 10
tmuonselectoralg.inputLocation =   'Rec/Track/Best/TMuon'
tmuonselectoralg.outputLocation =   'Rec/Track/AlignTracks'

trackFilterSeq.Members.append( tmuonselectoralg )

##==============================================================

#-- Use latest database tags for real data
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "head-20091120"
LHCbApp().CondDBtag = "head-20091120"
Escher().DatasetName = 'Cosmics'
EventSelector().PrintFreq = 100
Escher().PrintFreq = 100


#ApplicationMgr().HistogramPersistency = 'ROOT'  # ok
#HistogramPersistencySvc().OutputFile = 'job/'+nameCond+'-Histos.root' # ok

## Need this to read raw data
#importOptions('$STDOPTS/DecodeRawEvent.py') # ok

## Open Files; Also initialises Application Manager
## Specify the data
data = []
readTracksFromDST = False

if readTracksFromDST:
   #   data = ['PFN:/castor/cern.ch/user/w/wouter/otcosmics/run34120last.dst']
   data = [
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run58739.dst', 
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run58744.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59051.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59052.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59054.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59075.dst',# Muon
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59077.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59081.dst',# muon
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59088.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59092.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59093.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59096.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59097.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59195.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59198.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59306.dst',
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59307.dst',
      # Muon M1 ok
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59565.dst',# muon
      'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59568.dst' # muon
      ]

data = [
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/59565/059565_0000000001.raw',
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/59565/059565_0000000002.raw',
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/59568/059568_0000000001.raw',
   # 2 Novembre
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/60664/060664_0000000001.raw',
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/60668/060668_0000000001.raw',
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/60668/060668_0000000002.raw',
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/60676/060676_0000000001.raw',
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/60676/060676_0000000002.raw',
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/60678/060678_0000000001.raw',
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/60678/060678_0000000002.raw',
   'PFN:rfio:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/60678/060678_0000000003.raw'
   ]
data = [
   # Muon M1 ok
   'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59565.dst',# muon
   'PFN:rfio:/castor/cern.ch/user/w/wouter/otcosmics/run59568.dst' # muon
]      

data = [ '59565','59568' ]

Escher().InputType = 'DST'
EventSelector().Input = []
for d in data:
    name = "DATAFILE='PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run" + d + ".dst' TYP='POOL_ROOTTREE' OPT='READ'" 
    EventSelector().Input.append( name )

# do not do the TES check because these data do not have the required lists
# use this when reading a DST without tracks
GaudiSequencer("InitReprocSeq").Enable = False

