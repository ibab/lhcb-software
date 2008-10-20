from Gaudi.Configuration import *

nIter               = 5
nEvents             = 1000000
minNumHits          = 10
eigenvalueThreshold = 50
useDriftTime        = False
granularity         = 'halflayers'
preloadalignment    = False
#granularity         = 'modules'

# configure for half-layers
from Alignables import *
elements           = Alignables()
if granularity=='halflayers':
   elements.OTLayersCSide("Tx")
   elements.OTLayersASide("Tx")
   constraints = [
      "T1X1C : /.*?/OT/T1/X1layer/Quarter(0|2)/.*? : Tx",
      "T1UC  : /.*?/OT/T1/Ulayer/Quarter(0|2)/.*?  : Tx",
      "T1X1A : /.*?/OT/T1/X1layer/Quarter(1|3)/.*? : Tx",
      "T1UA  : /.*?/OT/T1/Ulayer/Quarter(1|3)/.*?  : Tx",
      "T3X2C : /.*?/OT/T3/X2layer/Quarter(0|2)/.*? : Tx",
      "T3UC  : /.*?/OT/T3/Vlayer/Quarter(0|2)/.*?  : Tx",
      "T3X2A : /.*?/OT/T3/X2layer/Quarter(1|3)/.*? : Tx",
      "T3UA  : /.*?/OT/T3/Vlayer/Quarter(1|3)/.*?  : Tx",
      ]
else :
   elements.OTModules("Tx")
   constraints = [
      "T1X1C : /.*?/OT/T1/X1layer/Quarter(0|2)/.*? : Tx",
      "T1UC  : /.*?/OT/T1/Ulayer/Quarter(0|2)/.*?  : Tx",
      "T1X1A : /.*?/OT/T1/X1layer/Quarter(1|3)/.*? : Tx",
      "T1UA  : /.*?/OT/T1/Ulayer/Quarter(1|3)/.*?  : Tx",
      "T3X2C : /.*?/OT/T3/X2layer/Quarter(0|2)/.*? : Tx",
      "T3UC  : /.*?/OT/T3/Vlayer/Quarter(0|2)/.*?  : Tx",
      "T3X2A : /.*?/OT/T3/X2layer/Quarter(1|3)/.*? : Tx",
      "T3UA  : /.*?/OT/T3/Vlayer/Quarter(1|3)/.*?  : Tx",
      ]

elements.ITBoxes("TxTy")

# configure some tracking opts
from TrackSys.Configuration import *
TrackSys().fieldOff = True
#TrackSys().expertTracking.append('simplifiedGeometry')
TrackSys().expertTracking += ['noDrifttimes' ]

from Configurables import MagneticFieldSvc
MagneticFieldSvc().UseConstantField = True
MagneticFieldSvc().UseConditions    = False


# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment

import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.printing_level += 1

from AlConfigurable import *

## AlternativeDB
#AlConfigurable().AlternativeCondDB           = "/afs/cern.ch/lhcb/software/releases/DBASE/Det/SQLDDDB/v4r3/db/LHCBCOND.db/LHCBCOND"
#AlConfigurable().AlternativeCondDBTag        = "DC06-20080407"
#AlConfigurable().AlternativeCondDBTag       = "MisA-OTL-1"
#AlConfigurable().AlternativeCondDBOverlays   = [ "/Conditions/IT", "/Conditions/OT", "Conditions/Velo" ]

## Patttern Recognition?
#AlConfigurable().CondDBTag                    = "2008-default"
AlConfigurable().Pat                          = True
AlConfigurable().OutputLevel                  = INFO
AlConfigurable().ElementsToAlign              = list(elements)
AlConfigurable().NumIterations                = nIter
AlConfigurable().AlignInputTrackCont           = "Alignment/AlignmentTracks"
AlConfigurable().UseCorrelations              = True
AlConfigurable().Constraints                  = constraints
AlConfigurable().UseWeightedAverageConstraint = False
AlConfigurable().MinNumberOfHits              = minNumHits
AlConfigurable().UsePreconditioning           = True
AlConfigurable().SolvTool                     = "gslSolver"
AlConfigurable().SolvTool                     = "DiagSolvTool"
AlConfigurable().WriteCondToXML               = True
AlConfigurable().CondFileName                 = "Elements.xml"
AlConfigurable().CondDepths                   = [0,1,2,3,4,5,6]
AlConfigurable().SimplifiedGeom               = True
AlConfigurable().WriteCondSubDetList          = [ "OT","IT" ]
AlConfigurable().Chi2Outlier = 10000

## Call after all options are set
AlConfigurable().applyConf()

importOptions("$DDDBROOT/options/LHCb-2008.py")

if preloadalignment:
   from Configurables import ( CondDBAccessSvc )
   AlignmentCondition = CondDBAccessSvc("AlignmentCondition")
   AlignmentCondition.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/OTHalfLayers.db/LHCBCOND"
   addCondDBLayer(AlignmentCondition)

## create a reconstruction sequence
patseq = GaudiSequencer("PatSeq")
patseq.Members = []

from Configurables import (PatSeeding, PatSeedingTool,TrackMonitor)
PatSeeding = PatSeeding("PatSeeding")
if TrackSys().getProp("fieldOff"):
   PatSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
   PatSeeding.PatSeedingTool.zMagnet = 0.
   PatSeeding.PatSeedingTool.FieldOff = True
importOptions('$PATALGORITHMSROOT/options/PatSeedingTool-Cosmics.opts')
PatSeeding.PatSeedingTool.MaxOTOccupancy = 0.01
PatSeeding.PatSeedingTool.MaxITOccupancy = 0.01
PatSeeding.PatSeedingTool.CommonXFraction        = 0.
PatSeeding.PatSeedingTool.MaxUsedFractPerRegion  = 0.
PatSeeding.PatSeedingTool.MaxUsedFractITOT       = 0.
PatSeeding.PatSeedingTool.MaxUsedFractLowQual    = 0.

patseq.Members += [PatSeeding,
                   TrackMonitor(name = "PatMonitor",
                                TracksInContainer = 'Rec/Track/Seed',FullDetail = True)]

# fit the track with straight line fit
from TrackFitter.ConfiguredFitters import (ConfiguredStraightLineFit)
from Configurables import (TrajOTCosmicsProjector,TrajOTProjector,TrackProjectorSelector,
                           MeasurementProvider)
trackprefit = ConfiguredStraightLineFit('SeedPreFit',
                                        TracksInContainer = 'Rec/Track/Seed')

defaultOTNoDriftTimeProjector = TrajOTProjector("OTNoDrifttimesProjector")
defaultOTNoDriftTimeProjector.UseDrift = False
trackprefit.Fitter.NodeFitter.addTool( TrackProjectorSelector(), "Projector" )
trackprefit.Fitter.NodeFitter.Projector.OT = "TrajOTProjector/" + defaultOTNoDriftTimeProjector.name()
trackprefit.Fitter.NodeFitter.BiDirectionalFit = False
trackprefit.Fitter.ErrorX2 = 10000
trackprefit.Fitter.ErrorY2 = 10000
trackprefit.Fitter.ErrorTx2 = 0.01
trackprefit.Fitter.ErrorTy2 = 0.01
trackprefit.Fitter.NumberFitIterations = 4
trackprefit.Fitter.MaxNumberOutliers = 2
trackprefit.Fitter.addTool( MeasurementProvider(), name = 'MeasProvider')
trackprefit.Fitter.MeasProvider.IgnoreIT = True ;
patseq.Members.append( trackprefit )

# make a selection based on the chisquare
from Configurables import (TrackSelector,TrackContainerCopy)
seedselectoralg  = TrackContainerCopy("SeedSelection")
seedselectoralg.addTool( TrackSelector("SeedSelector"), name = "Selector") ;
seedselectoralg.inputLocation =   'Rec/Track/Seed'
seedselectoralg.outputLocation =   'Rec/Track/SelectedSeed'
seedselectoralg.Selector.MaxChi2Cut = 5
seedselectoralg.Selector.MinNOTHits = 12

if useDriftTime:
   # load a calibration
   from Configurables import (OTCalibrationIO)
   patseq.Members += [ OTCalibrationIO() ]
   
    # initialize event t0
   from Configurables import (TrackSeedT0Alg)
   patseq.Members += [ TrackSeedT0Alg('TrackSeedT0',TrackLocation = 'Rec/Track/Seed') ]
   # refit the tracks
   trackfit = ConfiguredStraightLineFit('SeedFit',
                                        TracksInContainer = 'Rec/Track/Seed')
   trackfit.Fitter.MaxNumberOutliers = 0
   cosmicsOTProjector = TrajOTCosmicsProjector('OTCosmicsProjector')
   cosmicsOTProjector.UseDrift = True
   cosmicsOTProjector.FitEventT0 = True
   cosmicsOTProjector.UseConstantDriftVelocity = True
   trackfit.Fitter.NodeFitter.addTool( TrackProjectorSelector(), "Projector" )
   trackfit.Fitter.NodeFitter.Projector.OT = "TrajOTCosmicsProjector/" + cosmicsOTProjector.name()
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

from Configurables import TrackMonitor

patseq.Members += [ seedselectoralg
                    ]


# add the alignment. this is a bit tricky, but for now I just want the histograms
from Configurables import (AlignAlgorithm, GetElementsToBeAligned,Al__AlignConstraintTool,
                          Al__AlignUpdateTool,gslSVDsolver,DiagSolvTool,AlRobustAlignAlg )

alignAlg = AlignAlgorithm("Alignment")
alignAlg.TracksLocation  = 'Rec/Track/SelectedSeed'
alignAlg.UseCorrelations = True
#alignAlg.UpdateInFinalize = True

updatetool = Al__AlignUpdateTool("Al::AlignUpdateTool")
#solver = gslSVDsolver("MatrixSolverTool")
#solver.EigenValueThreshold = 100
#updatetool.addTool( gslSVDsolver(), name = "MatrixSolverTool" )
DiagSolvTool().EigenValueThreshold = eigenvalueThreshold
#DiagSolvTool().WriteMonNTuple = True
#updatetool.addTool( solver, name = "MatrixSolverTool" )
#updatetool.MatrixSolverTool.EigenValueThreshold = 100
#updatetool.MatrixSolverTool.WriteMonNTuple = True
elementtool = GetElementsToBeAligned( "GetElementsToBeAligned" )
elementtool.UseLocalFrame = False

## introduce some monitoring for first and last iteration
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

## Now lets run it
from GaudiPython import *
from GaudiPython import gbl

## Need this to read raw data
importOptions('$STDOPTS/DecodeRawEvent.opts')

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
   else:
      self.__open_orig__(stream,typ,opt,sel,fun,collection)
iEventSelector.open = _my_open_

def update(algorithm, appMgr) :
   # get pointer to incident service
   incSvc = appMgr.service( 'IncidentSvc', 'IIncidentSvc' )
   # incident to trigger update of constants
   updateConstants = gbl.Incident( algorithm, 'UpdateConstants' )
   incSvc.fireIncident( updateConstants )

HistogramPersistencySvc().OutputFile = "alignhistos.root"
#from Gaudi.Configuration import NTupleSvc
#NTupleSvc().Output = [ "FILE1 DATAFILE='aligntuples.root' TYPE='ROOT' OPT='NEW' " ]

## Instantiate application manager
appMgr = AppMgr()
mainSeq = appMgr.algorithm( 'AlignmentMainSeq' )

## Print flow of application
AlConfigurable().printFlow(appMgr)

evtSel           = appMgr.evtSel()
evtSel.printfreq = 100
##evtSel.FirstEvent = 604

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
   #'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34083.dst'
   'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34117.dst',
   'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34120.dst'
   ]
   
#evtSel.open( data, typ = "MDF")
evtSel.open( data )

print evtSel.Input


for i in range( nIter ) :
    mainSeq.Enable = False
    evtSel.rewind()
    mainSeq.Enable = True
    appMgr.algorithm('PreMonitorSeq').Enable = ( i == 0)
    appMgr.algorithm('PostMonitorSeq').Enable = ( i ==nIter-1)
    appMgr.run( nEvents )
    update( "Alignment", appMgr )

appMgr.finalize()


