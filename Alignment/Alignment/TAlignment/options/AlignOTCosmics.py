raise DeprecationWarning, "This file needs to be migrated to IOHelper for persistency"

from Gaudi.Configuration import *

nIter               = 5
nEvents             = 100000
minNumHits          = 10
eigenvalueThreshold = 0
maxNumOutliers      = 0
useDriftTime        = True
granularity         = 'halflayers'
granularity         = 'modules'
#granularity         = 'cframes'
#granularity         = 'layers'
#granularity         = 'modulesAndCFrames'
preloadalignment    = False
simplifiedGeometry  = True
muonMatching        = False
readTracksFromDST   = False
useCorrelations     = True
mergeOTBX           = True
loadnewgeometry     = True
uselocalframe       = True
chisqconstraints    = []
caloMatching = False

# configure for half-layers
from TAlignment.Alignables import *
elements           = Alignables()
if granularity=='layers':
   #elements.OTLayers("Tx") ;
   #   elements.OTStations("Tx") ;
   constraints = []

   elements += [ '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T1/X1layer:Tx', '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T1/Ulayer:Tx', '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T1/Vlayer:Tx', '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T1/X2layer:Tx']

   #elements = [ 'Group:/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T1/(X1|U|V|X2)layer:TxTyTzRxRyRz']
   
   #elements = [ 'Group:/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T1/(X1|U)layer:Tx',
   #             'Group:/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T1/(V|X2)layer:Tx']
   elements += [ '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T1:TxTyTzRxRyRz']
  

   elements += [ '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T2/X1layer:Tx', '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T2/Ulayer:Tx', '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T2/Vlayer:Tx', '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T2/X2layer:Tx']
   elements += [ '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T2:TxTyTzRxRyRz']
   
   #elements += [ '/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T1:Tx']
    
elif granularity=='halflayers':
   elements.OTLayersCSide("Tx")
   elements.OTLayersASide("Tx")
   constraints = [
      "T1X1C : /.*?/OT/T1/X1/Q(0|2)/.*? : Tx",
      "T1UC  : /.*?/OT/T1/U/Q(0|2)/.*?  : Tx",
      "T1X1A : /.*?/OT/T1/X1/Q(1|3)/.*? : Tx",
      "T1UA  : /.*?/OT/T1/U/Q(1|3)/.*?  : Tx",
      "T3X2C : /.*?/OT/T3/X2/Q(0|2)/.*? : Tx",
      "T3UC  : /.*?/OT/T3/V/Q(0|2)/.*?  : Tx",
      "T3X2A : /.*?/OT/T3/X2/Q(1|3)/.*? : Tx",
      "T3UA  : /.*?/OT/T3/V/Q(1|3)/.*?  : Tx"
      ]
   
elif granularity=='cframes':
   elements.OTCFramesASide("TxTyTzRz")
   elements.OTCFramesCSide("TxTyTzRz")
   constraints = [
      "T1X1C : T1X1UCSide : Tx Ty Tz Rz",
      "T1X1A : T1X1UASide : Tx Ty Tz Rz",
      "T3X2C : T3X1UCSide : Tx Ty Tz Rz",
      "T3X2A : T3X1UASide : Tx Ty Tz Rz"
      ]
   
elif granularity=='modulesAndCFrames':
   elements.OTCFramesASide("TzRzRxRy")
   elements.OTCFramesCSide("TzRzRxRy")
   constraints = [
      #"T1X1C : /.*?/OT/T1/(X1|U)layer/Quarter(0|2)/.*? : Tx Ty Tz Rx Ry Rz",
      #"T1X1A : /.*?/OT/T1/(X1|U)layer/Quarter(1|3)/.*? : Tx Ty Tz Rx Ry Rz",
      #"T3X2C : /.*?/OT/T3/(X1|U)layer/Quarter(0|2)/.*? : Tx Ty Tz Rx Ry Rz",
      #"T3X2A : /.*?/OT/T3/(X1|U)layer/Quarter(1|3)/.*? : Tx Ty Tz Rx Ry Rz"
      #"T1X1C : T/OT/T1/(X1|U)layer/Quarter(0|2)/Module. : Tx Ty Tz Rx Ry Rz",
      #"T1X1A : T/OT/T1/(X1|U)layer/Quarter(1|3)/Module. : Tx Ty Tz Rx Ry Rz",
      #"T3X2C : T/OT/T3/(X1|U)layer/Quarter(0|2)/Module. : Tx Ty Tz Rx Ry Rz",
      #"T3X2A : T/OT/T3/(X1|U)layer/Quarter(1|3)/Module. : Tx Ty Tz Rx Ry Rz"
      ]
   
   moduleelements = Alignables()
   moduleelements.OTModules("Tx")
   del moduleelements[0:36]
   n = len(moduleelements) ;
   del moduleelements[n-72:n-36]
   elements += list(moduleelements)
   
   #chisqconstraints = [ "/.*?/OT/T./.{1,2}layer/Quarter./Module. : Tx : 0.001" ]
   #chisqconstraints = [ "/.*?/OT/T3/X1layer/Quarter(1|3)/.*?: Tx : 0.001" ]
   #chisqconstraints = [ "T/OT/T./(X1|U|V|X2)layer/Quarter./Module. : Tx : 0.001" ]
   
else :
   elements.OTModules("Tx")
   del elements[0:36]
   n = len(elements) ;
   del elements[n-36:n]
      
   constraints = [
      "T1X1C : /.*?/OT/T1/X1layer/Quarter(0|2)/.*? : Tx Sxx",
      "T1UC  : /.*?/OT/T1/Ulayer/Quarter(0|2)/.*?  : Tx Sxx",
      "T1X1A : /.*?/OT/T1/X1layer/Quarter(1|3)/.*? : Tx Sxx",
      "T1UA  : /.*?/OT/T1/Ulayer/Quarter(1|3)/.*?  : Tx Sxx",
      "T3X2C : /.*?/OT/T3/X2layer/Quarter(0|2)/.*? : Tx Sxx",
      "T3UC  : /.*?/OT/T3/Vlayer/Quarter(0|2)/.*?  : Tx Sxx",
      "T3X2A : /.*?/OT/T3/X2layer/Quarter(1|3)/.*? : Tx Sxx",
      "T3UA  : /.*?/OT/T3/Vlayer/Quarter(1|3)/.*?  : Tx Sxx",
      ]
   constraints = [
      "T1/X1layer/Quarter0/Module1 : /.*?/OT/T1/X1layer/Quarter0/Module1 : Tx",
      "T1/X1layer/Quarter0/Module2 : /.*?/OT/T1/X1layer/Quarter0/Module2 : Tx",
      "T1/X1layer/Quarter0/Module3 : /.*?/OT/T1/X1layer/Quarter0/Module3 : Tx",
      "T1/X1layer/Quarter0/Module4 : /.*?/OT/T1/X1layer/Quarter0/Module4 : Tx",
      "T1/X1layer/Quarter0/Module5 : /.*?/OT/T1/X1layer/Quarter0/Module5 : Tx",
      "T1/X1layer/Quarter0/Module6 : /.*?/OT/T1/X1layer/Quarter0/Module6 : Tx",
      "T1/X1layer/Quarter0/Module7 : /.*?/OT/T1/X1layer/Quarter0/Module7 : Tx",
      "T1/X1layer/Quarter0/Module8 : /.*?/OT/T1/X1layer/Quarter0/Module8 : Tx",
      "T1/X1layer/Quarter0/Module9 : /.*?/OT/T1/X1layer/Quarter0/Module9 : Tx",
      "T1/X1layer/Quarter1/Module1 : /.*?/OT/T1/X1layer/Quarter1/Module1 : Tx",
      "T1/X1layer/Quarter1/Module2 : /.*?/OT/T1/X1layer/Quarter1/Module2 : Tx",
      "T1/X1layer/Quarter1/Module3 : /.*?/OT/T1/X1layer/Quarter1/Module3 : Tx",
      "T1/X1layer/Quarter1/Module4 : /.*?/OT/T1/X1layer/Quarter1/Module4 : Tx",
      "T1/X1layer/Quarter1/Module5 : /.*?/OT/T1/X1layer/Quarter1/Module5 : Tx",
      "T1/X1layer/Quarter1/Module6 : /.*?/OT/T1/X1layer/Quarter1/Module6 : Tx",
      "T1/X1layer/Quarter1/Module7 : /.*?/OT/T1/X1layer/Quarter1/Module7 : Tx",
      "T1/X1layer/Quarter1/Module8 : /.*?/OT/T1/X1layer/Quarter1/Module8 : Tx",
      "T1/X1layer/Quarter1/Module9 : /.*?/OT/T1/X1layer/Quarter1/Module9 : Tx",
      "T1/Ulayer/Quarter0/Module1 : /.*?/OT/T1/Ulayer/Quarter0/Module1 : Tx",
      "T1/Ulayer/Quarter0/Module2 : /.*?/OT/T1/Ulayer/Quarter0/Module2 : Tx",
      "T1/Ulayer/Quarter0/Module3 : /.*?/OT/T1/Ulayer/Quarter0/Module3 : Tx",
      "T1/Ulayer/Quarter0/Module4 : /.*?/OT/T1/Ulayer/Quarter0/Module4 : Tx",
      "T1/Ulayer/Quarter0/Module5 : /.*?/OT/T1/Ulayer/Quarter0/Module5 : Tx",
      "T1/Ulayer/Quarter0/Module6 : /.*?/OT/T1/Ulayer/Quarter0/Module6 : Tx",
      "T1/Ulayer/Quarter0/Module7 : /.*?/OT/T1/Ulayer/Quarter0/Module7 : Tx",
      "T1/Ulayer/Quarter0/Module8 : /.*?/OT/T1/Ulayer/Quarter0/Module8 : Tx",
      "T1/Ulayer/Quarter0/Module9 : /.*?/OT/T1/Ulayer/Quarter0/Module9 : Tx",
      "T1/Ulayer/Quarter1/Module1 : /.*?/OT/T1/Ulayer/Quarter1/Module1 : Tx",
      "T1/Ulayer/Quarter1/Module2 : /.*?/OT/T1/Ulayer/Quarter1/Module2 : Tx",
      "T1/Ulayer/Quarter1/Module3 : /.*?/OT/T1/Ulayer/Quarter1/Module3 : Tx",
      "T1/Ulayer/Quarter1/Module4 : /.*?/OT/T1/Ulayer/Quarter1/Module4 : Tx",
      "T1/Ulayer/Quarter1/Module5 : /.*?/OT/T1/Ulayer/Quarter1/Module5 : Tx",
      "T1/Ulayer/Quarter1/Module6 : /.*?/OT/T1/Ulayer/Quarter1/Module6 : Tx",
      "T1/Ulayer/Quarter1/Module7 : /.*?/OT/T1/Ulayer/Quarter1/Module7 : Tx",
      "T1/Ulayer/Quarter1/Module8 : /.*?/OT/T1/Ulayer/Quarter1/Module8 : Tx",
      "T1/Ulayer/Quarter1/Module9 : /.*?/OT/T1/Ulayer/Quarter1/Module9 : Tx",
      "T3/X1layer/Quarter0/Module1 : /.*?/OT/T3/X1layer/Quarter0/Module1 : Tx",
      "T3/X1layer/Quarter0/Module2 : /.*?/OT/T3/X1layer/Quarter0/Module2 : Tx",
      "T3/X1layer/Quarter0/Module3 : /.*?/OT/T3/X1layer/Quarter0/Module3 : Tx",
      "T3/X1layer/Quarter0/Module4 : /.*?/OT/T3/X1layer/Quarter0/Module4 : Tx",
      "T3/X1layer/Quarter0/Module5 : /.*?/OT/T3/X1layer/Quarter0/Module5 : Tx",
      "T3/X1layer/Quarter0/Module6 : /.*?/OT/T3/X1layer/Quarter0/Module6 : Tx",
      "T3/X1layer/Quarter0/Module7 : /.*?/OT/T3/X1layer/Quarter0/Module7 : Tx",
      "T3/X1layer/Quarter0/Module8 : /.*?/OT/T3/X1layer/Quarter0/Module8 : Tx",
      "T3/X1layer/Quarter0/Module9 : /.*?/OT/T3/X1layer/Quarter0/Module9 : Tx",
      "T3/X1layer/Quarter1/Module1 : /.*?/OT/T3/X1layer/Quarter1/Module1 : Tx",
      "T3/X1layer/Quarter1/Module2 : /.*?/OT/T3/X1layer/Quarter1/Module2 : Tx",
      "T3/X1layer/Quarter1/Module3 : /.*?/OT/T3/X1layer/Quarter1/Module3 : Tx",
      "T3/X1layer/Quarter1/Module4 : /.*?/OT/T3/X1layer/Quarter1/Module4 : Tx",
      "T3/X1layer/Quarter1/Module5 : /.*?/OT/T3/X1layer/Quarter1/Module5 : Tx",
      "T3/X1layer/Quarter1/Module6 : /.*?/OT/T3/X1layer/Quarter1/Module6 : Tx",
      "T3/X1layer/Quarter1/Module7 : /.*?/OT/T3/X1layer/Quarter1/Module7 : Tx",
      "T3/X1layer/Quarter1/Module8 : /.*?/OT/T3/X1layer/Quarter1/Module8 : Tx",
      "T3/X1layer/Quarter1/Module9 : /.*?/OT/T3/X1layer/Quarter1/Module9 : Tx",
      "T3/Vlayer/Quarter0/Module1 : /.*?/OT/T3/Vlayer/Quarter0/Module1 : Tx",
      "T3/Vlayer/Quarter0/Module2 : /.*?/OT/T3/Vlayer/Quarter0/Module2 : Tx",
      "T3/Vlayer/Quarter0/Module3 : /.*?/OT/T3/Vlayer/Quarter0/Module3 : Tx",
      "T3/Vlayer/Quarter0/Module4 : /.*?/OT/T3/Vlayer/Quarter0/Module4 : Tx",
      "T3/Vlayer/Quarter0/Module5 : /.*?/OT/T3/Vlayer/Quarter0/Module5 : Tx",
      "T3/Vlayer/Quarter0/Module6 : /.*?/OT/T3/Vlayer/Quarter0/Module6 : Tx",
      "T3/Vlayer/Quarter0/Module7 : /.*?/OT/T3/Vlayer/Quarter0/Module7 : Tx",
      "T3/Vlayer/Quarter0/Module8 : /.*?/OT/T3/Vlayer/Quarter0/Module8 : Tx",
      "T3/Vlayer/Quarter0/Module9 : /.*?/OT/T3/Vlayer/Quarter0/Module9 : Tx",
      "T3/Vlayer/Quarter1/Module1 : /.*?/OT/T3/Vlayer/Quarter1/Module1 : Tx",
      "T3/Vlayer/Quarter1/Module2 : /.*?/OT/T3/Vlayer/Quarter1/Module2 : Tx",
      "T3/Vlayer/Quarter1/Module3 : /.*?/OT/T3/Vlayer/Quarter1/Module3 : Tx",
      "T3/Vlayer/Quarter1/Module4 : /.*?/OT/T3/Vlayer/Quarter1/Module4 : Tx",
      "T3/Vlayer/Quarter1/Module5 : /.*?/OT/T3/Vlayer/Quarter1/Module5 : Tx",
      "T3/Vlayer/Quarter1/Module6 : /.*?/OT/T3/Vlayer/Quarter1/Module6 : Tx",
      "T3/Vlayer/Quarter1/Module7 : /.*?/OT/T3/Vlayer/Quarter1/Module7 : Tx",
      "T3/Vlayer/Quarter1/Module8 : /.*?/OT/T3/Vlayer/Quarter1/Module8 : Tx",
      "T3/Vlayer/Quarter1/Module9 : /.*?/OT/T3/Vlayer/Quarter1/Module9 : Tx"
      ]
   constraints = []

print list(elements)

# reconfigure some tracking opts
from TrackSys.Configuration import *
TrackSys().SpecialData += [ 'fieldOff' ]
TrackSys().ExpertTracking += ['noDrifttimes','kalmanSmoother' ]

from Configurables import MagneticFieldSvc
MagneticFieldSvc().UseConstantField = True
MagneticFieldSvc().UseConditions    = False

from Configurables import ( DataOnDemandSvc )
ApplicationMgr().ExtSvc.append( DataOnDemandSvc() )

ApplicationMgr().HistogramPersistency = 'ROOT'
#HistogramPersistencySvc().OutputFile = 'TMuonMatchingHistos.root'
#HistogramPersistencySvc().OutputFile = "ModuleAlignment.root"
HistogramPersistencySvc().OutputFile = 'alignhistos.root'

from Gaudi.Configuration import NTupleSvc
NTupleSvc().Output = [ "FILE1 DATAFILE='aligntuple.root' TYPE='ROOT' OPT='NEW' " ]

## Need this to read raw data
importOptions('$STDOPTS/DecodeRawEvent.py')

## Open Files; Also initialises Application Manager
## Specify the data
data = [
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34120/034120_0000085567.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34120/034120_0000085568.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34120/034120_0000085569.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34120/034120_0000085570.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34120/034120_0000085572.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34120/034120_0000085574.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34117/034117_0000085559.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34117/034117_0000085560.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34117/034117_0000085561.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34117/034117_0000085562.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34117/034117_0000085563.raw',
   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34117/034117_0000085564.raw'
   ]

data = [
   'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34083.dst',
   'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34117.dst',
   'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34120.dst'
   ]

if readTracksFromDST:
   data = ['PFN:/castor/cern.ch/user/w/wouter/otcosmics/run34120last.dst']

# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment

import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.PrintOff()

from AlConfigurable import *
alignment = AlConfigurable()
## AlternativeDB
#alignment.AlternativeCondDB           = "/afs/cern.ch/lhcb/software/releases/DBASE/Det/SQLDDDB/v4r3/db/LHCBCOND.db/LHCBCOND"
#alignment.AlternativeCondDBTag        = "DC06-20080407"
#alignment.AlternativeCondDBTag       = "MisA-OTL-1"
#alignment.AlternativeCondDBOverlays   = [ "/Conditions/IT", "/Conditions/OT", "Conditions/Velo" ]
alignment.DataType                     = '2008'
alignment.Pat                          = True
alignment.OutputLevel                  = INFO
alignment.ElementsToAlign              = list(elements)
alignment.UseLocalFrame                = uselocalframe
alignment.NumIterations                = nIter
alignment.AlignInputTrackCont          = "Alignment/AlignmentTracks"
alignment.UseCorrelations              = useCorrelations
alignment.Constraints                  = constraints
alignment.ChisqConstraints             = chisqconstraints
alignment.MinNumberOfHits              = minNumHits
alignment.SolvTool                     = "DiagSolvTool"
alignment.WriteCondSubDetList          = [ "OT","IT" ]
alignment.Chi2Outlier = 10000
alignment.SimplifiedGeom = True

if loadnewgeometry:
   from Configurables import ( CondDB, CondDBAccessSvc )
   #otGeom = CondDBAccessSvc( 'OTGeom' )
   #otGeom.ConnectionString = 'sqlite_file:/afs/cern.ch/user/j/janos/dbase/OTDDDBtest.db/DDDB'
   #CondDB().addLayer( otGeom )
   #otAlign = CondDBAccessSvc("AlignmentCondition")
   #otAlign.ConnectionString = 'sqlite_file:/afs/cern.ch/user/j/janos/dbase/OTLHCBCONDtest.db/LHCBCOND'
   #CondDB().addLayer( otAlign )
   otGeom = CondDBAccessSvc( 'OTGeom' )
   otGeom.ConnectionString = 'sqlite_file:/afs/cern.ch/user/j/janos/dbase/OTDDDBCroissant.db/DDDB'
   CondDB().addLayer( otGeom )
   otCond = CondDBAccessSvc( 'OTCond' )
   otCond.ConnectionString = 'sqlite_file:/afs/cern.ch/user/j/janos/dbase/LHCBCOND_changes.db/LHCBCOND'
   otCond.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/OTCFramesTxTyTzRz.db/LHCBCOND"
   CondDB().addLayer( otCond )

if preloadalignment:
   from Configurables import ( CondDBAccessSvc,CondDB )
   AlignmentCondition = CondDBAccessSvc("AlignmentCondition")
   AlignmentCondition.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/OTHalfLayers.db/LHCBCOND"
   CondDB().addLayer( AlignmentCondition )

## create a reconstruction sequence
   
patseq = GaudiSequencer("PatSeq")
patseq.Members = []

from Configurables import (PatSeeding, PatSeedingTool, TrackRemoveOddOTClusters,
                           TrackMonitor,TrackOTTimeMonitor,TrackContainerCopy)
PatSeeding = PatSeeding("PatSeeding")
if TrackSys().getProp("fieldOff"):
   PatSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
   PatSeeding.PatSeedingTool.zMagnet = 0.
   PatSeeding.PatSeedingTool.FieldOff = True
importOptions('$PATALGORITHMSROOT/options/PatSeedingTool-Cosmics.opts')
PatSeeding.PatSeedingTool.MaxOTOccupancy         = 0.01
PatSeeding.PatSeedingTool.MaxITOccupancy         = 0.01
PatSeeding.PatSeedingTool.CommonXFraction        = 0.
PatSeeding.PatSeedingTool.MaxUsedFractPerRegion  = 0.
PatSeeding.PatSeedingTool.MaxUsedFractITOT       = 0.
PatSeeding.PatSeedingTool.MaxUsedFractLowQual    = 0.


if readTracksFromDST:
   patseq.Members.append(TrackContainerCopy('copyDstTracks',
                                            outputLocation = 'Rec/Track/Seed',
                                            inputLocation = 'Rec/Track/Best'))
else:
   patseq.Members.append(PatSeeding)

patseq.Members.append(TrackMonitor(name = 'PatMonitor',
                                   TracksInContainer = 'Rec/Track/Seed',FullDetail = True))

# fit the track with straight line fit
from TrackFitter.ConfiguredFitters import (ConfiguredStraightLineFit)
from Configurables import (TrackKalmanFilter, TrajOTCosmicsProjector,TrajOTProjector,TrackProjectorSelector,
                           MeasurementProvider)

defaultOTNoDriftTimeProjector = TrajOTProjector( 'OTNoDrifttimesProjector' )
defaultOTNoDriftTimeProjector.UseDrift = False

trackprefit = ConfiguredStraightLineFit('SeedPreFit', TracksInContainer = 'Rec/Track/Seed')
#trackprefit = ConfiguredStraightLineFit('SeedPreFit', TracksInContainer = 'Rec/Track/TStationTracks')

from Configurables import (TrackEventFitter,TrackMasterFitter)
print TrackEventFitter( 'UniqueName' )


#trackprefit.OutputLevel = DEBUG
trackprefit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
trackprefit.Fitter.NodeFitter.addTool( TrackProjectorSelector, "Projector" )
trackprefit.Fitter.NodeFitter.Projector.OT = defaultOTNoDriftTimeProjector
trackprefit.Fitter.NodeFitter.BiDirectionalFit = False
trackprefit.Fitter.ErrorX2 = 10000
trackprefit.Fitter.ErrorY2 = 10000
trackprefit.Fitter.ErrorTx2 = 0.01
trackprefit.Fitter.ErrorTy2 = 0.01
trackprefit.Fitter.NumberFitIterations = 4
trackprefit.Fitter.MaxNumberOutliers = maxNumOutliers
trackprefit.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
trackprefit.Fitter.MeasProvider.IgnoreIT = True
patseq.Members.append( trackprefit )
patseq.Members.append( TrackRemoveOddOTClusters('RemoveOddOTClusters',TrackLocation = 'Rec/Track/Seed') )

# make a selection based on the chisquare
from Configurables import (TrackSelector,TrackContainerCopy)
seedselectoralg  = TrackContainerCopy("SeedSelection")
seedselectoralg.addTool( TrackSelector("SeedSelector"), name = "Selector") ;
seedselectoralg.inputLocation =   'Rec/Track/Seed'
seedselectoralg.outputLocation =   'Rec/Track/SelectedSeed'
seedselectoralg.Selector.MaxChi2Cut = 5
#seedselectoralg.Selector.MinNOTHits = 12

if useDriftTime:
   # load a calibration
   from Configurables import (OTCalibrationIO)
   patseq.Members += [ OTCalibrationIO() ]
   
   # initialize event t0
   from Configurables import (TrackSeedT0Alg)
   patseq.Members += [ TrackSeedT0Alg('TrackSeedT0',TrackLocation = 'Rec/Track/Seed') ]
   
   # refit the tracks with drift times
   trackfit = ConfiguredStraightLineFit('SeedFit',
                                        TracksInContainer = 'Rec/Track/Seed')
   #print trackfit
   trackfit.Fitter.MaxNumberOutliers = 0
   cosmicsOTProjector = TrajOTCosmicsProjector('OTCosmicsProjector')
   cosmicsOTProjector.UseDrift = True
   cosmicsOTProjector.FitEventT0 = True
   cosmicsOTProjector.UseConstantDriftVelocity = True
   trackfit.Fitter.addTool(TrackKalmanFilter(),'NodeFitter')
   trackfit.Fitter.NodeFitter.addTool( TrackProjectorSelector, "Projector" )
   trackfit.Fitter.NodeFitter.Projector.OT = cosmicsOTProjector
   trackfit.Fitter.ErrorX2 = 10000
   trackfit.Fitter.ErrorY2 = 10000
   trackfit.Fitter.ErrorTx2 = 0.01
   trackfit.Fitter.ErrorTy2 = 0.01
   trackfit.Fitter.ErrorP = [ 0, 100 ]
   trackfit.Fitter.NumberFitIterations = 5
   trackfit.Fitter.MaxNumberOutliers = 0
   trackfit.Fitter.Chi2Outliers = 25
   patseq.Members.append( trackfit )
   seedselectoralg.Selector.MaxChi2Cut = 10
else:
   trackfit = trackprefit.clone("AfterPrefit")
   trackfit.Fitter.MaxNumberOutliers = 0
   patseq.Members.append( trackfit )

from Configurables import TrackMonitor

patseq.Members += [ seedselectoralg ]

if muonMatching:
   from TrackFitter.ConfiguredFitters import ( ConfiguredStraightLineFit )
   from  Configurables import ( MuonID, MuonNNetRec,  CopyMuonNNetTracks, MeasurementProvider,  
                                TrackMasterExtrapolator, TrackKalmanFilter, 
                                TrackChi2Calculator, OTMuonCosmicsMatching )
   # Copy MuonNNet tracks to LHCb tracks
   copy = CopyMuonNNetTracks( 'CopyMuonNNetTracks' )
   #copy.OutputLevel = DEBUG
   copy.addTool( MuonNNetRec, name = 'MuonRecTool' )
   copy.MuonRecTool.TrackSpanCut     = 2.0 ## cut on span for selected tracks
   copy.MuonRecTool.StationFiringCut = 2.0 ## cut on # of stations firing in the track
   copy.MuonRecTool.AllowHoles       = False ## allow holes in tracks
   copy.MuonRecTool.MaxNeurons       = 400 ## (def. 3000) max number of possible track segments
   copy.MuonRecTool.MaxIterations    = 30 ## (def. 100) max number of NN iterations
   copy.MuonRecTool.UsePosTool       = False
   copy.MuonRecTool.AssumeCosmics    = True
   copy.MuonRecTool.PhysicsTiming    = False
   copy.MuonRecTool.AssumePhysics    = False ## assume that tracks come from IP (only forward)
   # Refit
   muonTrackFit =  ConfiguredStraightLineFit( 'MuonTrackFit' , TracksInContainer = 'Rec/Track/Muon' )
   #muonTrackFit.OutputLevel = DEBUG
   muonTrackFit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
   muonTrackFit.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
   muonTrackFit.Fitter.MeasProvider.IgnoreVelo = True 
   muonTrackFit.Fitter.MeasProvider.IgnoreTT   = True 
   muonTrackFit.Fitter.MeasProvider.IgnoreIT   = True 
   muonTrackFit.Fitter.MeasProvider.IgnoreOT   = True 
   muonTrackFit.Fitter.MeasProvider.IgnoreMuon = False 
   muonTrackFit.Fitter.NodeFitter.BiDirectionalFit = False
   muonTrackFit.Fitter.ErrorX2  = 10000
   muonTrackFit.Fitter.ErrorY2  = 10000
   muonTrackFit.Fitter.ErrorTx2 = 0.5
   muonTrackFit.Fitter.ErrorTy2 = 0.5
   muonTrackFit.Fitter.NumberFitIterations = 4
   muonTrackFit.Fitter.MaxNumberOutliers   = 2
   # Matching
   matching = OTMuonCosmicsMatching( 'OTMuonCosmicsMatching' )
   matching.FullDetail = True
   #matching.OutputLevel = DEBUG
   matching.TTracksLocation    = 'Rec/Track/Seed'
   matching.MuonTracksLocation = 'Rec/Track/Muon'
   matching.addTool( TrackMasterExtrapolator, name = 'Extrapolator' )
   matching.Extrapolator.ApplyMultScattCorr          = False
   matching.Extrapolator.ApplyEnergyLossCorr         = False
   matching.Extrapolator.ApplyElectronEnergyLossCorr = False
   matching.addTool( TrackChi2Calculator, name = 'Chi2Calculator' )
   matching.MatchChi2Cut = 10000.0 #
   
   # Fit TMuon tracks
   tmuonfit = ConfiguredStraightLineFit( 'TMuonFit', TracksInContainer = 'Rec/Track/Best/TMuon')
   #tmuonfit.OutputLevel = DEBUG
   tmuonfit.Fitter.MakeNodes = True
   tmuonfit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
   tmuonfit.Fitter.NodeFitter.addTool( TrackProjectorSelector, "Projector" )
   tmuonfit.Fitter.NodeFitter.Projector.OT = defaultOTNoDriftTimeProjector
   tmuonfit.Fitter.NodeFitter.BiDirectionalFit = False
   tmuonfit.Fitter.ErrorX2 = 10000
   tmuonfit.Fitter.ErrorY2 = 10000
   tmuonfit.Fitter.ErrorTx2 = 0.5
   tmuonfit.Fitter.ErrorTy2 = 0.5
   tmuonfit.Fitter.NumberFitIterations = 4
   tmuonfit.Fitter.MaxNumberOutliers = 2
   tmuonfit.Fitter.addTool( MeasurementProvider(), name = 'MeasProvider')
   tmuonfit.Fitter.MeasProvider.IgnoreIT   = True ;
   tmuonfit.Fitter.MeasProvider.IgnoreVelo = True ;
   
   patseq.Members.append( copy )
   patseq.Members.append( muonTrackFit )
   patseq.Members.append( matching )
   patseq.Members.append( tmuonfit )
   
# add the alignment. this is a bit tricky, but for now I just want the histograms
from Configurables import (AlignAlgorithm, GetElementsToBeAligned,Al__AlignConstraintTool,
                           Al__AlignChisqConstraintTool,
                           Al__AlignUpdateTool,gslSVDsolver,DiagSolvTool,AlRobustAlignAlg )

# First apply everything that was set before
alignment.alignmentSeq()
alignAlg = AlignAlgorithm( 'Alignment' )

alignAlg.TracksLocation  = 'Rec/Track/SelectedSeed'
#alignAlg.UpdateInFinalize = True
#updatetool = Al__AlignUpdateTool("Al::AlignUpdateTool")
#updatetool.OutputLevel=2 
#solver = gslSVDsolver("MatrixSolverTool")
#solver.EigenValueThreshold = 100
#updatetool.addTool( gslSVDsolver(), name = "MatrixSolverTool" )
#DiagSolvTool().EigenValueThreshold = eigenvalueThreshold
#DiagSolvTool().WriteMonNTuple = True
#updatetool.addTool( solver, name = "MatrixSolverTool" )
#updatetool.MatrixSolverTool.EigenValueThreshold = 100
#updatetool.MatrixSolverTool.WriteMonNTuple = True

## introduce some monitoring for first and last iteration
from Configurables import TrackOTTimeMonitor
preMonitorSeq = GaudiSequencer("PreMonitorSeq") ;
postMonitorSeq = GaudiSequencer("PostMonitorSeq") ;
patseq.Members += [preMonitorSeq,postMonitorSeq]

preMonitorSeq.Members.append(TrackMonitor(name = "SeedPreMonitor",
                                          TracksInContainer = 'Rec/Track/Seed',
                                          FullDetail = True))
if muonMatching :
  muonMonitor = TrackMonitor( "MuonMonitor" )
  muonMonitor.TracksInContainer = 'Rec/Track/Muon'
  muonMonitor.FullDetail = True
  tmuonMonitor = TrackMonitor( "TMuonMonitor" )
  tmuonMonitor.TracksInContainer = 'Rec/Track/Best/TMuon'
  tmuonMonitor.FullDetail = True
  preMonitorSeq.Members.append( muonMonitor )
  preMonitorSeq.Members.append( tmuonMonitor )

preMonitorSeq.Members += [TrackMonitor(name = "SelectedSeedPreMonitor",
                                          TracksInContainer = 'Rec/Track/SelectedSeed',
                                          FullDetail = True),
                          TrackOTTimeMonitor(name="PreOTTimeMonitor",
                                             TrackLocation = 'Rec/Track/SelectedSeed') ]

postMonitorSeq.Members += [TrackMonitor(name = "SeedPostMonitor",
                                        TracksInContainer = 'Rec/Track/Seed',
                                        FullDetail = True),
                           TrackOTTimeMonitor(name="PostOTTimeMonitor",
                                              TrackLocation = 'Rec/Track/SelectedSeed') ]

if caloMatching:
   from Configurables import (CaloCosmicsTrackAlg, TrackCaloCosmicsMonitor)
   postMonitorSeq.Members.append(CaloCosmicsTrackAlg())
   postMonitorSeq.Members.append(TrackCaloCosmicsMonitor( TrackLocation = 'Rec/Track/Seed'))

if mergeOTBX:
   # Make sure to mereg spills from different OT events
   from Configurables import (Tf__OTHitCreator,OTMeasurementProvider)
   Tf__OTHitCreator('OTHitCreator').RawBankDecoder = 'OTMultiBXRawBankDecoder'
   OTMeasurementProvider().RawBankDecoder = 'OTMultiBXRawBankDecoder'

## Now lets run it
from GaudiPython import *
from GaudiPython import gbl

EventPersistencySvc().CnvServices.append( "LHCb::RawDataCnvSvc" )
#import Gaudi.CommonGaudiConfigurables

# Hack to overide default EvtSel open
#
# I don't understand this. What the heck is iEventSelector
#
from GaudiPython.Bindings import iEventSelector
iEventSelector.__open_orig__ = iEventSelector.open
def _my_open_(self,stream, typ = 'POOL_ROOT', opt = 'READ', sel = None, fun = None, collection = None):
   if typ == "MDF":
       if type(stream) != list : stream = [stream]
       fixpart = "TYP=\'%s\' OPT=\'%s\' SVC='LHCb::MDFSelector'" % ( typ, opt )
       if sel        : fixpart += " SEL=\'%s\'" % sel
       if fun        : fixpart += " FUN=\'%s\'" % fun
       if collection : fixpart += " COLLECTION=\'%s\'" % collection
       cstream = ["DATAFILE=\'%s\' %s" % ( s, fixpart) for s in stream]
       self.Input = cstream
       self.reinitialize()
   elif typ == 'POOL_ROOT':
       if type(stream) != list : stream = [stream]
       fixpart = "TYP=\'%s\' OPT=\'%s\'" % ( typ, opt )
       if sel        : fixpart += " SEL=\'%s\'" % sel
       if fun        : fixpart += " FUN=\'%s\'" % fun
       if collection : fixpart += " COLLECTION=\'%s\'" % collection
       cstream = ["DATAFILE=\'%s\' %s" % ( s, fixpart) for s in stream]
       self.Input = cstream
       self.reinitialize()
   else:
      self.__open_orig__(stream,typ,opt,sel,fun,collection)
iEventSelector.open = _my_open_


def update(algorithm, appMgr) :
   # get pointer to incident service
   incSvc = appMgr.service( 'IncidentSvc', 'IIncidentSvc' )
   # incident to trigger update of constants
   updateConstants = gbl.Incident( algorithm, 'UpdateConstants' )
   incSvc.fireIncident( updateConstants )

## Instantiate application manager
appMgr = AppMgr()
mainSeq = appMgr.algorithm( 'AlignmentMainSeq' )

## Print flow of application
alignment.printFlow(appMgr)
evtSel = appMgr.evtSel()

evtSel.printfreq = 100
##evtSel.FirstEvent = 604
#evtSel.open( data, typ = "MDF")
evtSel.open( data, typ = 'POOL_ROOT')

print "evtSel.input:"
print evtSel.Input
print "appMgr: "
print appMgr

for i in range( nIter ) :
    mainSeq.Enable = False
    evtSel.rewind()
    mainSeq.Enable = True
    appMgr.algorithm('PreMonitorSeq').Enable = ( i == 0)
    appMgr.algorithm('PostMonitorSeq').Enable = ( i == nIter-1)
    appMgr.run( nEvents )
    update( "Alignment", appMgr )

appMgr.algorithm('PreMonitorSeq').Enable = True



