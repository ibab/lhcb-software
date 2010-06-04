##############################################################################
# File for running Brunel on real data with default 2008 geometry, as defined in
# DDDB/Configuration.py
# Syntax is:
#
#   gaudirun.py $ESCHEROPTS/2010-Collisions-Muon.py
#
# or
#
# $ESCHEROPTS/gaudiiter.py -n NUMITER -e NUMEVENTS $ESCHEROPTS/2010-Collisions-Muon.py
#
# Run list of stripped data included
##############################################################################
from Configurables import ( Escher, TrackSys, RecSysConf )

Escher().DataType     = "2010"
Escher().Kalman       = True 
Escher().Millepede    = False
Escher().TrackContainer = "Rec/Track/AlignTracks" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax       = 5000
Escher().SkipEvents   = 0
Escher().SpecialData  = ["earlyData"] # ,"veloOpen"] 
Escher().InputType    = 'DST'
TrackSys().ExpertTracking += [ "simplifiedGeometry"]
RecSysConf().RecoSequence = ["Decoding","VELO","TT","IT","OT","Tr","MUON"]

###############################################################

from GaudiKernel.SystemOfUnits import GeV, mm
from Gaudi.Configuration import *

uselocalframe       = True
chisqconstraints    = [] # one number for each constraint
constraints         = []
surveyconstraints   = []
dof                 = 'TxTy'

outputlevel         = ERROR# INFO
outputlevel2        = ERROR# INFO

nameCond            = 'RunBdownVeloClosed'
OTMuonMatching      = True    # Matches the OT tracks to the Muon tracks
OTMuon_ntuple       = False   # if True writes an Ntuple for debugging purposes
filterMuonTracks    = True    # filters the Muon track sample


from TAlignment.Alignables import *
from TAlignment.SurveyConstraints import *

elements           = Alignables()
elements.MuonHalfStations(dof) ;            # configure for Muon half-Stations

surveyconstraints = SurveyConstraints()
surveyconstraints.MUON()
#surveyconstraints.append("Muon/M1/M1ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
#surveyconstraints.append("Muon/M1/M1CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
#surveyconstraints.append("Muon/M2/M2ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
#surveyconstraints.append("Muon/M2/M2CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
#surveyconstraints.append("Muon/M3/M3ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
#surveyconstraints.append("Muon/M3/M3CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
#surveyconstraints.append("Muon/M4/M4ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
#surveyconstraints.append("Muon/M4/M4CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
#surveyconstraints.append("Muon/M5/M5ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
#surveyconstraints.append("Muon/M5/M5CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")

   
#constraints.append("Muon/M2ASide : Muon/M2/M2ASide : Tx Ty")
#constraints.append("Muon/M2CSide : Muon/M2/M2CSide : Tx Ty")
print ' List of elements '
print list(elements)
print ' List of constraints '
print list(constraints)
print ' List of survey_constraints '
print list(surveyconstraints)

data          = []
tileVetoLIST = []
BField = True

sample = '2010-Collisions-Bdown'
#sample = '2010-Collisions-Boff'
#sample = '2010-Collisions-Bup'
if sample =='2010-Collisions-Bdown' :
   runnrs = ['7835','7836',
             '7825','7826'] # VeloClosed_MagDown
   path = '/castor/cern.ch/grid/lhcb/user/s/svecchi/2010_04/'

elif sample =='2010-Collisions-Boff':
   BField = False
   Escher().SpecialData +=["fieldOff"]
   runnrs = ['7847']
   path = '/castor/cern.ch/grid/lhcb/user/s/svecchi/2010_04/'

elif sample =='2010-Collisions-Bup':
   runnrs = [ '69614_69623'] # '69625','69626','69627']
   path = '/castor/cern.ch/user/s/svecchi/Allineamento/Collisions2010/StrippedDST/'
else :
   import sys
   print ' Check the sample name : no valid data selected'
   sys.exit()
   
data = []
for runnr in runnrs:
   if path == '/castor/cern.ch/grid/lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/' :
      put, get = os.popen4("rfdir " + path  + runnr)
      for line in get.readlines():
         splittedline = line.split(' ')
         filestring = splittedline[len(splittedline)-1].strip(' ')
         filestring = filestring.replace('\n','')
         print 'adding file: ', path + runnr + "/" + filestring + " ==== "
         data.append( path + runnr + '/'+  filestring)
   elif  path == '/castor/cern.ch/grid/lhcb/user/s/svecchi/2010_04/' :
      put, get = os.popen4("rfdir " + path  + runnr)
      for line2 in get.readlines():
         splittedline2 = line2.split(' ')
         filestring2 = splittedline2[len(splittedline2)-1].strip(' ')
         filestring2 = filestring2.replace('\n','')
         print " == ", filestring2
         put, get = os.popen4("rfdir " + path  + runnr + '/' + filestring2 )
         for line in get.readlines():
            splittedline = line.split(' ')
            filestring = splittedline[len(splittedline)-1].strip(' ')
            filestring = filestring.replace('\n','')
            print 'adding file: ', path + runnr + "/" + filestring2 + "/" + filestring + " ==== "
            data.append( path + runnr + '/'+ filestring2 + '/'+ filestring)
   else:
      data.append( path +  runnr +".dst")
print data


# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment

from Configurables import TAlignment,GetElementsToBeAligned
TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
TAlignment().Constraints          = constraints
TAlignment().SurveyConstraints    = list(surveyconstraints)
TAlignment().WriteCondSubDetList  = ['Muon']
TAlignment().CondFilePrefix       = 'xml/' 
TAlignment().OutputLevel          = outputlevel 
TAlignment().UseLocalFrame        = uselocalframe 

elementtool = GetElementsToBeAligned( "GetElementsToBeAligned" ) 
elementtool.UseLocalFrame = False 


# ================================================================================================
# add all 'private' reconstruction to this sequence which is automatically added to the alignment sequence.
# ================================================================================================
from Configurables import GaudiSequencer
trackFilterSeq = GaudiSequencer("TrackFilterSeq")

from TrackFitter.ConfiguredFitters import (ConfiguredStraightLineFitter,ConfiguredEventFitter,ConfiguredFastEventFitter) 
from Configurables import (TrackKalmanFilter, TrajOTCosmicsProjector,TrajOTProjector,TrackProjectorSelector,
                           MeasurementProvider) 

# ======================================================
# Muon cosmics track reconstruction
from TrackFitter.ConfiguredFitters import ( ConfiguredStraightLineFitter )

from  Configurables import ( MuonNNetRec, MuonCombRec, MuonPadRec, MuonClusterRec, MakeMuonTracks, MeasurementProvider,
                             TrackMasterExtrapolator, TrackKalmanFilter, 
                             TrackChi2Calculator, TrackMuonMatching )
muonTrackTool = MuonNNetRec(name="MuonNNetRec",
                            TrackSpanCut                = 2.0, ## cut on span for selected tracks
                            StationFiringCut            = 2.0, ## cut on # of stations firing in the track
                            AllowHoles                  = True, ## allow holes in tracks
                            MaxNeurons                  = 3000, ## (def. 3000) max number of possible track segments
                            MaxIterations               = 100, ## (def. 100) max number of NN iterations
                            AssumeCosmics               = False,
                            PhysicsTiming               = False,
                            AssumePhysics               = True, ## assume that tracks come from IP (only forward)
                            AddXTalk                    = True,
                            XtalkRadius                 = 1.5,
                            SkipStation                 = 0,
                            #DecodingTool = "MuonMonHitDecode", # you can also try "MuonMonHitDecode"
                            PadRecTool   = "MuonPadRec",
                            ClusterTool  = "MuonClusterRec", # default is "MuonFakeClustering"
                            OutputLevel                 = outputlevel2
                            )
# MuonClusterRec tool specific options   ------ > for public implementations
clusterTool = MuonClusterRec(name="MuonClusterRec",OutputLevel=outputlevel2, MaxPadsPerStation=300)
# MuonPadRec tool specific options   ------ > for public implementations
padRecTool = MuonPadRec(name = "PadRecTool",OutputLevel=outputlevel2,TileVeto=tileVetoLIST)

# Copy MuonTracks to LHCb tracks
copy                                         = MakeMuonTracks( 'MakeMuonTracks' )
trackFilterSeq.Members.append( copy )  
copy.OutputLevel                             = outputlevel2 #DEBUG
copy.BField                                  = BField
copy.SkipBigClusters                         = True # set True to cut tracks with large clusters
copy.MaxNTiles                               = 6     # max number of pads per cluster to accept the track
copy.addTool( muonTrackTool, name            = 'MuonRecTool' )

# Refit ================================================================================
muonTrackFit =  ConfiguredStraightLineFitter( 'MuonTrackFit' , TracksInContainer = 'Rec/Track/Muon' ) # fits the Muon-only tracks
muonTrackFit.Fitter.OutputLevel             = outputlevel
muonTrackFit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
muonTrackFit.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
muonTrackFit.Fitter.MeasProvider.IgnoreMuon = False 
muonTrackFit.Fitter.MeasProvider.MuonProvider.clusterize = True 
muonTrackFit.Fitter.MeasProvider.MuonProvider.OutputLevel = outputlevel
muonTrackFit.Fitter.ErrorX                  = 1000
muonTrackFit.Fitter.ErrorY                  = 10000
muonTrackFit.Fitter.MaxNumberOutliers       = 0

# ------------------------------------------------------------------
trackFilterSeq.Members.append( muonTrackFit )

from Configurables import TrackMonitor
trackFilterSeq.Members.append(TrackMonitor("MyMuonMonitor",TracksInContainer='Rec/Track/Muon'))

# ------------------------------------------------------------------
if filterMuonTracks :
   from Configurables import TAlignment,GetElementsToBeAligned
   from Configurables import TrackFilterAlg
   trackFilterAlg = TrackFilterAlg("FilterTracks")
   trackFilterAlg.OutputLevel               = outputlevel
   trackFilterAlg.TracksInputContainer      = "Rec/Track/Muon"          # MuonTrackRec
   trackFilterAlg.TracksOutputContainer     = "Rec/Track/SelectedMuon"  # Filtered tracks
   
   trackFilterAlg.addTool(GetElementsToBeAligned (OutputLevel   = outputlevel,
                                                  Elements      = TAlignment().ElementsToAlign   ),
                                                  name = "GetElementsToBeAligned" )   
   trackFilterAlg.MuonFilter                = True   ###------------------------------ MUONTRACKSELECTOR
   trackFilterAlg.MuonPcut                  = 6.*GeV
   trackFilterAlg.MuonChisquareCut          = 5
   trackFilterAlg.inCaloAcceptance          = True
   trackFilterAlg.noOverlap                 = False
   trackFilterAlg.TheRegion                 = 10
   trackFilterAlg.minHitStation             = 3  # >3
   trackFilterAlg.OutputLevel               = outputlevel #DEBUG   
   trackFilterSeq.Members.append( trackFilterAlg )

# ------------------------------------------------------------------
if OTMuonMatching:      # Track-Muon  Matching
   matching = TrackMuonMatching( 'TrackMuonMatching' )
   matching.AllCombinations                                = False
   matching.MatchAtFirstMuonHit                            = True
   matching.FullDetail                                     = True
   matching.OutputLevel                                    = outputlevel2
   matching.TTracksLocation                                = 'Rec/Track/Best' # Best container + chisquare < 5
   matching.MuonTracksLocation                             = 'Rec/Track/SelectedMuon' # MuonNNet
   matching.TracksOutputLocation                           = 'Rec/Track/Best/TMuon'
   matching.addTool( TrackMasterExtrapolator, name         = 'Extrapolator' )
   matching.Extrapolator.ApplyMultScattCorr                = True
   matching.Extrapolator.ApplyEnergyLossCorr               = False
   matching.Extrapolator.ApplyElectronEnergyLossCorr       = False
   matching.addTool( TrackChi2Calculator, name             = 'Chi2Calculator' )
   matching.MatchChi2Cut                                   = 10.0 #
   matching.WriteNtuple                                    = OTMuon_ntuple

   
   # Fit TMuon tracks

   #tmuonfit = ConfiguredFastEventFitter( 'TMuonFit', TracksInContainer = 'Rec/Track/Best/TMuon') # contains Best+Muon
   tmuonfit = ConfiguredEventFitter( 'TMuonFit', TracksInContainer = 'Rec/Track/Best/TMuon') # contains Best+Muon
   tmuonfit.OutputLevel                                    = outputlevel
   tmuonfit.Fitter.MakeNodes                               = True
   tmuonfit.Fitter.MakeMeasurements                        = True
   tmuonfit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
   tmuonfit.Fitter.ErrorX                                 = 1000
   tmuonfit.Fitter.ErrorY                                 = 10000
   tmuonfit.Fitter.NumberFitIterations                    = 5
   tmuonfit.Fitter.MaxDeltaChiSqConverged                 = 0.1
   tmuonfit.Fitter.MaxNumberOutliers                       = 0
   tmuonfit.Fitter.addTool( MeasurementProvider(), name    = 'MeasProvider')
   tmuonfit.Fitter.MeasProvider.MuonProvider.clusterize    = True 
   tmuonfit.Fitter.MeasProvider.MuonProvider.OutputLevel   = outputlevel
   tmuonfit.Fitter.MeasProvider.IgnoreMuon = False    
   
   if OTMuonMatching :
      trackFilterSeq.Members.append( matching )
      trackFilterSeq.Members.append( tmuonfit )
   # ------------------------------------------------------------------
from Configurables import TrackMonitor
trackFilterSeq.Members.append(TrackMonitor("MyTMuonMonitor",TracksInContainer='Rec/Track/Best/TMuon'))


from Configurables import (TrackSelector,TrackContainerCopy)
tmuonselectoralg  = TrackContainerCopy( "TmuonSelection" )
tmuonselectoralg.addTool( TrackSelector, name = "Selector")
tmuonselectoralg.Selector.MaxChi2Cut = 10
tmuonselectoralg.Selector.OutputLevel = outputlevel
tmuonselectoralg.inputLocation =   'Rec/Track/Best/TMuon'
tmuonselectoralg.outputLocation =   'Rec/Track/AlignTracks'

trackFilterSeq.Members.append( tmuonselectoralg )
##==============================================================

#-- Use latest database tags for real data

from Configurables import LHCbApp
#LHCbApp().Monitors = ["MemoryAuditor"]
Escher().DataType     = "2010"
LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag =  "default"
Escher().DatasetName = 'Collision2010'
Escher().Simulation = False
#Escher().UseOracle = True
EventSelector().PrintFreq = 1500
Escher().PrintFreq = 1
Escher().MoniSequence = ["Tr", "OT","ST"]

# To load CondDB Layers -------------
#from Configurables import ( CondDBAccessSvc,CondDB )
## Muon
#MuonAlignmentCondition = CondDBAccessSvc("MuonAlignmentCondition")
#MuonAlignmentCondition.ConnectionString = 'sqlite_file:/afs/cern.ch/user/s/svecchi/public/MuonAlignment_Bon_v3.0.db/LHCBCOND'
#CondDB().addLayer( MuonAlignmentCondition )



#  -------------- Histos and Ntuple
ApplicationMgr().HistogramPersistency = 'ROOT'  # ok
HistogramPersistencySvc().OutputFile = '/tmp/Histos.root' # ok

if OTMuon_ntuple :
   from Gaudi.Configuration import NTupleSvc
   NTupleSvc().Output = [ "FILE1 DATAFILE='/tmp/Ntuple.root' TYPE='ROOT' OPT='NEW' " ]

## Need this to read raw data
importOptions('$STDOPTS/DecodeRawEvent.py') 

EventSelector().Input = []
for d in data:
   directory           = ''
   name = "DATAFILE='PFN:rfio:"+ directory + d + "'  TYP='POOL_ROOTTREE' OPT='READ'"
   EventSelector().Input.append(name) 


# do not do the TES check because these data do not have the required lists
# use this when reading a DST without tracks
GaudiSequencer("InitReprocSeq").Enable = False








from Gaudi.Configuration import *

def redefineSequences() :
   GaudiSequencer("RecoMUONSeq").Members = [ "MuonRec"]

appendPostConfigAction( redefineSequences )

