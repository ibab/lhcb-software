from Gaudi.Configuration import *

nEvents            = 100000
nIter              = 3
datasample = 'TED2'
condname = 'Detectors'
conddepths = [0,1,2,3]

from Alignables import *
elements = Alignables()
constraints = []
elements.TTLayers("Tx")
elements.IT("Tx")
   
print list(elements)
from TrackSys.Configuration import *
TrackSys().fieldOff = True
TrackSys().expertTracking.append('simplifiedGeometry')
from Configurables import MagneticFieldSvc
MagneticFieldSvc().UseConstantField = True
MagneticFieldSvc().UseConditions    = False

# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment
#ApplicationMgr( AppName         = "VeloAlignment", AppVersion      = "v1",
#                AuditTools      = True       , AuditServices   = True  , AuditAlgorithms = True )

#from Configurables import LbAppInit
#ApplicationMgr().TopAlg.append( LbAppInit( ApplicationMgr().AppName ) )

import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.printing_level += 1

from AlConfigurable import *

## Patttern Recognition?
AlConfigurable().Pat                          = True
## Set output level
AlConfigurable().OutputLevel                  = INFO
AlConfigurable().ElementsToAlign              = list(elements)
AlConfigurable().NumIterations                = nIter
AlConfigurable().AlignInputTrackCont           = "Alignment/AlignmentTracks"
#AlConfigurable().AlignInputTackCont           = "Rec/Track/Best"
AlConfigurable().UseCorrelations              = True
AlConfigurable().Constraints                  = constraints
AlConfigurable().UseWeightedAverageConstraint = False
AlConfigurable().MinNumberOfHits              = 10
AlConfigurable().UsePreconditioning           = True
AlConfigurable().SolvTool                     = "gslSolver"
AlConfigurable().SolvTool                     = "DiagSolvTool"
AlConfigurable().WriteCondToXML               = True
AlConfigurable().CondFileName                 = condname
AlConfigurable().CondDepths                   = conddepths
AlConfigurable().Precision                    = 8
AlConfigurable().SimplifiedGeom               = True
AlConfigurable().WriteCondSubDetList          = [ "TT" ]
#AlConfigurable().VertexLocation = "Rec/Vertex/Primary"
AlConfigurable().Chi2Outlier = 10000
#AlConfigurable().RegularizationFactor = 1

## Call after all options are set
AlConfigurable().applyConf()

importOptions("$DDDBROOT/options/LHCb-2008.py")

#EventSelector().Input = ["DATA='castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/30933/030933_0000077704.raw' SVC='LHCb::MDFSelector'"]

# Load an alignment for the VELO
MisAlCh1COND = CondDBAccessSvc("MisAlCh1COND")
MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/TEDVeloModulesSliceManyIter.db/LHCBCOND"
addCondDBLayer(MisAlCh1COND)

# Load an alignment for IT
ITAlignCond = CondDBAccessSvc("ITAlignCond")
ITAlignCond.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/MattsITLayerSlice.db/LHCBCOND"
addCondDBLayer(ITAlignCond)

# load an alignment for TT
#TTAlignCond = CondDBAccessSvc("TTAlignCond")
#TTAlignCond.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/TTSlice.db/LHCBCOND"
#addCondDBLayer(TTAlignCond)

# configure VELO reconstruction
veloreco = GaudiSequencer("PatSeq")
veloreco.Members = []
from Configurables import (Tf__PatVeloGeneric,Tf__PatVeloGeneralTracking)
patVeloGeneric = Tf__PatVeloGeneric()
veloreco.Members = [ patVeloGeneric ]
patVeloGeneric.FullAlignment        = True
patVeloGeneric.ForwardProp          = True
patVeloGeneric.PrivateBest          = True
patVeloGeneric.CheckReadOut         = True

# fit the velo tracks
from Configurables import ( PatVeloTT,PatForward, PatForwardTool, 
			    PatFwdTool, TrackEventFitter)
from Configurables import (TrackSelector,TrackMonitor,TrackContainerCopy,TrackListPrinter)

# fit the velo tracks
from TrackFitter.ConfiguredFitters import *
veloFitter = ConfiguredFitVelo()
veloFitter.TracksInContainer = 'Rec/Track/Velo'
veloFitter.Fitter.ApplyMaterialCorrections = False
veloFitter.Fitter.Extrapolator.ApplyMultScattCorr = False
veloFitter.Fitter.NodeFitter.BiDirectionalFit = False
veloFitter.Fitter.ZPositions += [ 770 ]
veloFitter.Fitter.MaxNumberOutliers = 2
veloFitter.Fitter.ErrorX2 = 1
veloFitter.Fitter.ErrorY2 = 1
veloFitter.Fitter.ErrorTx2 = 0.001 
veloFitter.Fitter.ErrorTy2 = 0.001
veloreco.Members += [ veloFitter ]

# copy the velo tracks, selecting he good ones. this is for monitoring
trackselector = TrackSelector("MyTrackSelector")
trackselector.MinNVeloPhiHits = 5
trackselector.MinNVeloRHits = 5
trackselector.MaxChi2Cut = 10
velocopy = TrackContainerCopy("VeloSelectionAlg")
velocopy.inputLocation =   'Rec/Track/Velo'
velocopy.outputLocation =   'Rec/Track/VeloSelection'
velocopy.addTool( trackselector, name = "Selector") ;
veloreco.Members += [ velocopy ]

# configure Velo-TT reconstruction
patVeloTT = PatVeloTT("PatVeloTT")
importOptions ("$PATVELOTTROOT/options/PatVeloTT.py")
patVeloTT.removeUsedTracks = False
patVeloTT.InputUsedTracksNames = []
patVeloTT.PatVeloTTTool.minMomentum = 5000.;
patVeloTT.PatVeloTTTool.maxPseudoChi2 = 256 ;
patVeloTT.maxChi2 = 256 ;
patVeloTT.PatVeloTTTool.DxGroupFactor = 0.0;
patVeloTT.fitTracks = False;

velottFitter = ConfiguredFitVeloTT()
velottFitter.TracksInContainer = 'Rec/Track/VeloTT'
velottFitter.Fitter.MakeNodes = True
velottFitter.Fitter.ApplyMaterialCorrections = False
velottFitter.Fitter.Extrapolator.ApplyMultScattCorr = False
velottFitter.Fitter.NodeFitter.BiDirectionalFit = False
velottFitter.Fitter.MaxNumberOutliers = 2
velottFitter.Fitter.ErrorX2 = 1
velottFitter.Fitter.ErrorY2 = 1
velottFitter.Fitter.ErrorTx2 = 0.001 
velottFitter.Fitter.ErrorTy2 = 0.001

veloreco.Members += [ patVeloTT, velottFitter ]

# make a subselection for alignment
velottselection = TrackContainerCopy("VeloTTSelectionAlg")
velottselection.inputLocation  =   'Rec/Track/VeloTT'
velottselection.outputLocation =   'Rec/Track/VeloTTSelection'
velottselection.addTool( trackselector, name = "Selector") ;
veloreco.Members += [ velottselection ]

# add the alignment. this is configured somewhere else. we just make a few changes.
from Configurables import (AlignAlgorithm, GetElementsToBeAligned,Al__AlignConstraintTool,
                          Al__AlignUpdateTool,gslSVDsolver,DiagSolvTool,AlRobustAlignAlg )

alignAlg = AlignAlgorithm("Alignment")
alignAlg.TracksLocation  = 'Rec/Track/VeloTTSelection'
alignAlg.UseCorrelations = True
updatetool = Al__AlignUpdateTool("Al::AlignUpdateTool")
#DiagSolvTool().EigenValueThreshold = 50
#DiagSolvTool().WriteMonNTuple = True
elementtool = GetElementsToBeAligned( "GetElementsToBeAligned" )
elementtool.UseLocalFrame = False

# now do some monitoring. the 'pre' sequence runs only in the first iteration.
premonitorseq = GaudiSequencer("PreMonitorSeq")
premonitorseq.Members += [ TrackMonitor(name = "VeloTTMonitor",
                                     TracksInContainer = 'Rec/Track/VeloTT'),
                           TrackMonitor(name = "AlignTracksMonitor",
                                        TracksInContainer = 'Rec/Track/VeloTTSelection') ]
# the 'post' sequence only in the last
postmonitorseq = GaudiSequencer("PostMonitorSeq")
postmonitorseq.Members += [ TrackMonitor(name = "VeloTTPostMonitor",
                                         TracksInContainer = 'Rec/Track/VeloTT')]

# also add the 'robust' algorithm --> that gives the histograms
premonitorseq.Members += [ AlRobustAlignAlg("RobustAlignAlg", TrackLocation = 'Rec/Track/VeloSelection') ]
postmonitorseq.Members += [ AlRobustAlignAlg("PostRobustAlignAlg", TrackLocation = 'Rec/Track/VeloSelection') ]
veloreco.Members += [ premonitorseq,postmonitorseq ]

## Now lets run it
from GaudiPython import *
from GaudiPython import gbl

## Need this to read raw data
importOptions('$STDOPTS/DecodeRawEvent.opts')
importOptions('Velo-CommissioningConditions.py')

## Specify the data
data = []
if datasample == 'TED1':
   data = ['PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/30933/030933_0000077704.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/30930/030930_0000077697.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/30931/030931_0000077700.raw']

if datasample == 'TED2':
   data = ['PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32477/032477_0000081644.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32479/032479_0000081647.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32481/032481_0000081648.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32484/032484_0000081651.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32493/032493_0000081660.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32498/032498_0000081699.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32501/032501_0000081702.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32581/032581_0000081823.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32583/032583_0000081825.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32586/032586_0000081828.raw'
           ]
   
   # these last data need some modifications because velo timing is off
   from Configurables import (DecodeVeloRawBuffer,RawBankToSTClusterAlg,RawBankToSTLiteClusterAlg,UpdateManagerSvc)
   DecodeVeloRawBuffer('createVeloClusters').RawEventLocation='Prev2/DAQ/RawEvent'
   DecodeVeloRawBuffer('createVeloLiteClusters').RawEventLocation='Prev2/DAQ/RawEvent'
   RawBankToSTClusterAlg('createITClusters').rawEventLocation = '/Event/Prev2/DAQ/RawEvent'
   RawBankToSTClusterAlg('createTTClusters').rawEventLocation = '/Event/Prev2/DAQ/RawEvent'
   RawBankToSTLiteClusterAlg('createITLiteClusters').rawEventLocation = '/Event/Prev2/DAQ/RawEvent'
   RawBankToSTLiteClusterAlg('createTTLiteClusters').rawEventLocation = '/Event/Prev2/DAQ/RawEvent'
   UpdateManagerSvc().ConditionsOverride += [
      "Conditions/Online/Velo/MotionSystem := double ResolPosLA =28.5; double ResolPosRC =-28.5;"
      ]

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
from Gaudi.Configuration import NTupleSvc
NTupleSvc().Output = [ "FILE1 DATAFILE='aligntuples.root' TYPE='ROOT' OPT='NEW' " ]

## Instantiate application manager
appMgr = AppMgr()
mainSeq = appMgr.algorithm( 'AlignmentMainSeq' )
monitorSeq = appMgr.algorithm( 'MonitorSeq' )

## Print flow of application
AlConfigurable().printFlow(appMgr)

evtSel           = appMgr.evtSel()
evtSel.printfreq = 10
##evtSel.FirstEvent = 604

## Open Files; Also initialises Application Manager
evtSel.open( data, typ = "MDF")
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


