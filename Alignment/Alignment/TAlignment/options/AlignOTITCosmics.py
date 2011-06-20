raise DeprecationWarning, "This file needs to be migrated to IOHelper for persistency"

from Gaudi.Configuration import *

nIter               = 5
nEvents             = 100000
minNumHits          = 10
eigenvalueThreshold = 50
useDriftTime        = True
granularity         = 'halflayers'
preloadalignment    = True
# if this is true, IT will not actually move: we'll only have some histograms fromm robust alignment
ignoreIT            = False
#temporrary (until OT geometry is fixed in db)
loadnewgeometry     = True
eigenvaluecut       = -1

# configure for half-layers
from Alignables import *
elements           = Alignables()
elements.OTCFramesASide("TxTyTzRz")
elements.OTCFramesCSide("TxTyTzRz")
constraints = [
   "T1X1C : /.*?/OT/T1/X1layer/Quarter(0|2)/.*? : Tx Ty Tz Rx Ry Rz",
   "T1X1A : /.*?/OT/T1/X1layer/Quarter(1|3)/.*? : Tx Ty Tz Rx Ry Rz",
   "T3X1C : /.*?/OT/T3/X1layer/Quarter(0|2)/.*? : Tx Ty Tz Rx Ry Rz",
   "T3X1A : /.*?/OT/T3/X1layer/Quarter(1|3)/.*? : Tx Ty Tz Rx Ry Rz",
   ]
elements.ITBoxes("TxTyTz")
print elements

# configure some tracking opts
from TrackSys.Configuration import *
#TrackSys().fieldOff = True
TrackSys().SpecialData += [ 'fieldOff' ]
TrackSys().ExpertTracking += ['noDrifttimes','kalmanSmoother','simplifiedGeometry' ]

from Configurables import MagneticFieldSvc
MagneticFieldSvc().UseConstantField = True
MagneticFieldSvc().UseConditions    = False

from Configurables import ( DataOnDemandSvc )
ApplicationMgr().ExtSvc.append( DataOnDemandSvc() )

ApplicationMgr().HistogramPersistency = 'ROOT'
#HistogramPersistencySvc().OutputFile = 'TMuonMatchingHistos.root'
#HistogramPersistencySvc().OutputFile = "ModuleAlignment.root"
HistogramPersistencySvc().OutputFile = 'alignhistos.root'

#from Gaudi.Configuration import NTupleSvc
#NTupleSvc().Output = [ "FILE1 DATAFILE='TMuonMatching.root' TYPE='ROOT' OPT='NEW' " ]

## Need this to read raw data
importOptions('$STDOPTS/DecodeRawEvent.py')

## Open Files; Also initialises Application Manager
## Specify the data
# add some of my OT cosmics data. this will probably mean that we'll
# have most IT tracks double, but okay, we need something to fix the OT
data = [
   'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34120.dst'
   ]

#Matt's data
data += ['PFN:/afs/cern.ch/lhcb/group/tracking/vol4/mneedham/cosmics_19_20_sept.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_20_21_sept.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30636.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30639.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30640.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30641.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30645.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30648.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30653.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30654.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30659.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30660.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30661.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30662.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30664.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30666.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run30669.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31300.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31528.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31529.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31530.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31531.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31533.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31537.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31540.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31548.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31549.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31557.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31714.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31715.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31716.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31719.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31720.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31723.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31725.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31726.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31727.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31731.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31732.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31734.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31736.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31737.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31739.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31744.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31747.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31760.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31761.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31762.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31789.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31791.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31792.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31793.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31794.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31795.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31797.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31817.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run31856.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run32381.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run32385.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run32406.raw',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run32432.dst',
        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_run34116.raw']

#data += [ 'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34117.dst' ]


#data = ['PFN:/afs/cern.ch/lhcb/group/tracking/vol4/mneedham/cosmics_19_20_sept.raw',
#        'PFN:/afs/cern.ch/lhcb/group/tracking/vol5/mneedham/cosmics_20_21_sept.raw']

#        'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34120.dst']

# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment

import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.PrintOff()

from AlConfigurable import *

alignment = AlConfigurable()
alignment.DataType             = '2008'
alignment.Pat                  = True
alignment.OutputLevel          = INFO
alignment.ElementsToAlign      = list(elements)
alignment.NumIterations        = nIter
alignment.AlignInputTrackCont  = 'Rec/Track/SelectedSeed'
alignment.Constraints          = constraints
alignment.MinNumberOfHits      = minNumHits
alignment.SolvTool             = "DiagSolvTool"
alignment.EigenValueThreshold  = eigenvaluecut
alignment.WriteCondSubDetList  = [ "OT","IT" ]
alignment.UseLocalFrame        = False
alignment.Chi2Outlier          = 10000

if preloadalignment:
   from Configurables import ( CondDBAccessSvc,CondDB )
   from Configurables import ( CondDBAccessSvc,CondDB )
   OTAlignmentCondition = CondDBAccessSvc("OTAlignmentCondition")
   OTAlignmentCondition.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/OTCFramesTxTyRz.db/LHCBCOND"
   #CondDB().addLayer(OTAlignmentCondition)
   ITAlignmentCondition = CondDBAccessSvc("ITAlignmentCondition")
   #ITAlignmentCondition.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/AlignedMattNewTxTyRzITLayerSlice.db/LHCBCOND"
   #ITAlignmentCondition.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/AlignedMattNewTxTyRzITLayerTxTyWRTOTCFramesSlice.db/LHCBCOND"
   ITAlignmentCondition.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/MattsITLayerSlice.db/LHCBCOND"
   CondDB().addLayer(ITAlignmentCondition)
   
if loadnewgeometry:
   from Configurables import ( CondDB, CondDBAccessSvc )
   otGeom = CondDBAccessSvc( 'OTGeom' )
   otGeom.ConnectionString = 'sqlite_file:/afs/cern.ch/user/j/janos/dbase/DDDB-FixModules.db/DDDB'
   CondDB().addLayer( otGeom )


## create a reconstruction sequence
patseq = GaudiSequencer("PatSeq")
patseq.Members = []

from ITSpillMerge import ITSpillMergeSequence
#patseq.Members.append(ITSpillMergeSequence( ClustersLocation = '', Spills = ['', 'Prev1', 'Next1' ]) )
patseq.Members.append(ITSpillMergeSequence( ClustersLocation = '', Spills = ['','Prev1/','Next1/','Prev2/','Next2/']) )

from Configurables import STClusterMonitor
patseq.Members.append(STClusterMonitor(InputData='/Event/Raw/IT/MergedClusters',
                                       DetType='IT',
                                       FullDetail=True))

from Configurables import (PatSeeding, PatSeedingTool, TrackRemoveOddOTClusters,TrackMonitor)
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
PatSeeding.PatSeedingTool.TolCollectIT           = 10

patseq.Members += [PatSeeding,
                   TrackMonitor(name = 'PatMonitor',
                                TracksInContainer = 'Rec/Track/Seed',FullDetail = True)]

# fit the track with straight line fit
from TrackFitter.ConfiguredFitters import (ConfiguredStraightLineFit)
from Configurables import (TrackKalmanFilter, TrajOTCosmicsProjector,TrajOTProjector,TrackProjectorSelector,
                           MeasurementProvider)

defaultOTNoDriftTimeProjector = TrajOTProjector( 'OTNoDrifttimesProjector' )
defaultOTNoDriftTimeProjector.UseDrift = False

trackprefit = ConfiguredStraightLineFit('SeedPreFit', TracksInContainer = 'Rec/Track/Seed')

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
trackprefit.Fitter.MaxNumberOutliers = 2
trackprefit.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
trackprefit.Fitter.MeasProvider.IgnoreIT = ignoreIT
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
   seedselectoralg.Selector.MaxChi2Cut = 100
else:
   trackfit = trackprefit.clone("AfterPrefit")
   trackfit.Fitter.MaxNumberOutliers = 0
   patseq.Members.append( trackfit )

from Configurables import TrackMonitor

patseq.Members += [ seedselectoralg ]

## introduce some monitoring for first and last iteration
from Configurables import AlRobustAlignAlg
preMonitorSeq = GaudiSequencer("PreMonitorSeq") ;
postMonitorSeq = GaudiSequencer("PostMonitorSeq") ;
patseq.Members += [preMonitorSeq,postMonitorSeq]

preMonitorSeq.Members.append(TrackMonitor(name = "SeedPreMonitor",
                                          TracksInContainer = 'Rec/Track/Seed',
                                          FullDetail = True))

preMonitorSeq.Members.append(TrackMonitor(name = "SelectedSeedPreMonitor",
                                          TracksInContainer = 'Rec/Track/SelectedSeed',
                                          FullDetail = True))
                          
postMonitorSeq.Members.append(TrackMonitor(name = "SeedPostMonitor",
                                           TracksInContainer = 'Rec/Track/Seed',
                                           FullDetail = True))

postMonitorSeq.Members.append(AlRobustAlignAlg(TrackLocation='Rec/Track/Seed'))
                              
## Now lets run it
from GaudiPython import *
from GaudiPython import gbl

EventPersistencySvc().CnvServices.append( "LHCb::RawDataCnvSvc" )

# Hack to overide default EvtSel open
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



