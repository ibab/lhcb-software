from Gaudi.Configuration import *

nEvents            = 100000
nIter              = 3
alignlevel         = 'sensors'
alignlevel = 'modules'
#alignlevel = 'velott'
#alignlevel= 'halves'
runVeloTT = False
datasample = 'TED2'

from Alignables import *

elements = Alignables()
constraints = []
if alignlevel == 'sensors':
   elements.VeloRSensors("TxTy")
   elements.VeloPhiSensors("TxTy")
   constraints += ["VeloRight : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/.*? : Tx Ty Szx Szy Rz SRz"]
   constraints += ["VeloLeft : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/.*? : Tx Ty Szx Szy Rz SRz"]
#   constraints = ["Tx","Ty","Tz","Szz","Szx","Szy"]
#   elements.VeloRSensors("Tz")
#   elements.VeloPhiSensors("Tz")
#   conddepths = [3]
#   condname = "Detectors.xml"
#   WriteCondSubDetList = ['Velo']
elif alignlevel == 'halves':
#   elements.VeloLeft("TxTyRxRy")
#   elements.VeloRight("TxTyRxRy")
#   constraints = ["Tx","Ty","Rx","Ry"]
   elements.VeloLeft("Tx")
   elements.VeloRight("Tx")
#   constraints = ["Tx"]
#   constraints = ["Tx","Ty","Tz","Szz","Szx","Szy"]
#   elements.VeloLeft("TxTyTz")
#   elements.VeloRight("TxTyTz")
   conddepths = [0,1]
   condname   = "Global.xml"
elif alignlevel == 'modules':
   elements.VeloModules("TxTy")
   constraints = []
   constraints += ["VeloRight : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/.*? : Tx Ty Szx Szy"]
   constraints += ["VeloLeft : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/.*? : Tx Ty Szx Szy"]
#   constraints += ["modL00 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module00/.*? : Rz "]
#   constraints += ["modL04 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module04/.*? : Rz "]
#   constraints += ["modL12 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module12/.*? : Rz "]
#   constraints += ["modL14 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module14/.*? : Rz "]
#   constraints += ["modL18 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module18/.*? : Rz "]
#   constraints += ["modL22 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module22/.*? : Rz "]
#   constraints += ["modL38 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module38/.*? : Rz "]
#   constraints += ["modL40 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module40/.*? : Rz "]
#   constraints += ["modR23 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/Module23/.*? : Rz "]
#   constraints += ["modR27 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/Module27/.*? : Rz "]
#   constraints += ["modR29 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/Module29/.*? : Rz "]
#   constraints += ["modR33 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/Module33/.*? : Rz "]
#   constraints += ["modR39 : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/Module39/.*? : Rz "]
   
   #constraints = ["Tx","Ty","Szx","Szy"]
   #elements.VeloLeftModules("Rz")
   #constraints = ["Rz","SRz"]
   #["Tx"]
   conddepths = [2]
   condname   = "Modules.xml"
elif alignlevel == 'velott':
   elements.VeloRight("Tx")
   elements.TTLayers("Tx")
   condname   = "Something.xml"
   conddepths = [0,1,2,3]
else :
   elements.Velo("TxTyRxRy")
#   elements.TT("Tz")
#   elements.IT("Tz")
#   elements.OT("Tz")
   constraints = []#["TxTyRxRy"]
   conddepths = [0,1]
   condname   = "Global.xml"
   
#elements = Alignables()
if not runVeloTT:
   elements.TTLayers("Tx")
elements.IT("Tx")
   
print list(elements)
from TrackSys.Configuration import *
TrackSys().SpecialData += [ 'fieldOff' ]
TrackSys().ExpertTracking += ['noDrifttimes','kalmanSmoother','simplifiedGeometry' ]
from Configurables import MagneticFieldSvc
MagneticFieldSvc().UseConstantField = True
MagneticFieldSvc().UseConditions    = False

EventDataSvc().ForceLeaves        = True
EventDataSvc().EnableFaultHandler = True
EventDataSvc().RootCLID           =    1

#from DDDB.Configuration import *
from Configurables import DDDBConf
DDDBConf().DataType = '2008'
#importOptions("$DDDBROOT/options/LHCb-2008.py")

# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment

import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.PrintOff()

from AlConfigurable import *
alignment = AlConfigurable()
alignment.DataType                     = '2008'
alignment.Pat                          = True
alignment.CondDBTag                    = "head-20081002"
alignment.OutputLevel                  = INFO
alignment.ElementsToAlign              = list(elements)
alignment.NumIterations                = nIter
alignment.AlignInputTrackCont          = 'Rec/Track/VeloSelection'
alignment.Constraints                  = constraints
alignment.MinNumberOfHits              = 20
alignment.SolvTool                     = "DiagSolvTool"
alignment.WriteCondSubDetList          = [ "Velo","TT" ]
#alignment.VertexLocation = "Rec/Vertex/Primary"
alignment.UseLocalFrame                = False
alignment.EigenValueThreshold = 50

# load eventual alignment file

MisAlCh1COND = CondDBAccessSvc("MisAlCh1COND")
#MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/user/m/marcocle/public/Wouter/LHCBCOND_MisAlCh1.db/LHCBCOND"
#MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/VeloSliceMisalignedTest3.db/LHCBCOND"
#MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/VeloSliceAlignedHalvesNoCorr.db/LHCBCOND"
#MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/TEDVeloModulesSlice.db/LHCBCOND"
MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/TEDVeloModulesSliceManyIter.db/LHCBCOND"
#MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/TED1VeloModulesSlice.db/LHCBCOND"
MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/SilviaTED1VeloModulesSliceNov.db/LHCBCOND"
MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/SilviaTED2VeloModulesSliceNov.db/LHCBCOND"
#addCondDBLayer(MisAlCh1COND)

ITAlignCond = CondDBAccessSvc("ITAlignCond")
ITAlignCond.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/MattsITLayerSlice.db/LHCBCOND"
#addCondDBLayer(ITAlignCond)

TTAlignCond = CondDBAccessSvc("TTAlignCond")
TTAlignCond.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/TTSlice.db/LHCBCOND"
#addCondDBLayer(TTAlignCond)

veloreco = GaudiSequencer("PatSeq")
#veloreco = GaudiSequencer("veloreco")
veloreco.Members = []
#veloreco.Members += [createVeloClusters]
#veloreco.Members += [  patVeloRTracking, patVeloSpaceTracking, veloGeneral ]
#veloreco.Members = [  veloGeneral ]
#ApplicationMgr().TopAlg += [ veloreco ]
#veloreco.OutputLevel = DEBUG

from Configurables import (Tf__PatVeloGeneric,Tf__PatVeloGeneralTracking)
patVeloGeneric = Tf__PatVeloGeneric()
veloreco.Members = [ patVeloGeneric ]
#veloreco.Members = [ Tf__PatVeloGeneralTracking("PatVeloGeneralTracking") ]
#GenericTracking.CheckReadOut = true;
#GenericTracking.ForwardProp = true;
#GenericTracking.SigmaTol = 10;

if True:
   patVeloGeneric.FullAlignment        = True
   #patVeloGeneric.MinModules           = 4
   #patVeloGeneric.ClusterCut           = 30
   #patVeloGeneric.KalmanState          = 4
   patVeloGeneric.ForwardProp          = True
   #patVeloGeneric.MaxSkip              = 3
   patVeloGeneric.PrivateBest          = True
   #patVeloGeneric.SigmaTol             = 5
   #patVeloGeneric.RAliTol              = 0.05
   #patVeloGeneric.PAliTol              = 0.005
   patVeloGeneric.CheckReadOut         = True

from Configurables import ( PatVeloTT,PatForward, PatForwardTool, 
			    PatFwdTool, TrackEventFitter,TrackKalmanFilter)
from Configurables import (TrackSelector,TrackMonitor,TrackContainerCopy,TrackListPrinter)


patForward = PatForward("PatForward")
PatFwdTool("PatFwdTool").withoutBField  = True;
PatForwardTool("PatForwardTool").WithoutBField = True;
veloreco.Members += [patForward]
#from Configurables import (PatSeeding, PatSeedingTool)

# fit the velo tracks
from TrackFitter.ConfiguredFitters import *
veloFitter = ConfiguredFitVelo('MyFitVelo')
veloFitter.TracksInContainer = 'Rec/Track/Velo'
veloFitter.Fitter.ApplyMaterialCorrections = False
veloFitter.Fitter.Extrapolator.ApplyMultScattCorr = False
veloFitter.Fitter.addTool( TrackKalmanFilter(), name="NodeFitter")
veloFitter.Fitter.NodeFitter.BiDirectionalFit = False
veloFitter.Fitter.ZPositions += [ 770 ]
veloFitter.Fitter.MaxNumberOutliers = 2
veloFitter.Fitter.ErrorX2 = 1
veloFitter.Fitter.ErrorY2 = 1
veloFitter.Fitter.ErrorTx2 = 0.001 
veloFitter.Fitter.ErrorTy2 = 0.001
#veloFitter.OutputLevel = DEBUG

##veloFitter.Fitter.NodeFitter.OutputLevel = DEBUG
veloreco.Members += [ veloFitter,
                      TrackContainerCopy("VeloCopy", inputLocation='Rec/Track/Velo',
                                         outputLocation = 'Rec/Track/VeloCopy' ) ]
##TrackSelector('VeloCopy.Selector').OutputLevel = 2

# do some monitoring
monitorseq = GaudiSequencer("PreMonitorSeq")
monitorseq.Members += [ TrackMonitor(name = "VeloMonitor",
                                     TracksInContainer = 'Rec/Track/VeloCopy',
                                     FullDetail = True) ]
postmonitorseq = GaudiSequencer("PostMonitorSeq")
postmonitorseq.Members += [ TrackMonitor(name = "VeloPostMonitor",
                                         TracksInContainer = 'Rec/Track/VeloCopy',
                                         FullDetail = True) ]

## Velo-TT pattern
if runVeloTT:
   patVeloTT = PatVeloTT("PatVeloTT")
   importOptions ("$PATVELOTTROOT/options/PatVeloTT.py")
   patVeloTT.removeUsedTracks = False
   patVeloTT.InputUsedTracksNames = []
   patVeloTT.PatVeloTTTool.minMomentum = 5000.;
   patVeloTT.PatVeloTTTool.maxPseudoChi2 = 256 ;
   patVeloTT.maxChi2 = 256 ;
   patVeloTT.PatVeloTTTool.DxGroupFactor = 0.0;
   patVeloTT.fitTracks = False;
   velottFitter = veloFitter.clone('FitVeloTT')
   velottFitter.TracksInContainer = 'Rec/Track/VeloTT'
   velottFitter.Fitter.MakeNodes = True
   veloreco.Members += [ patVeloTT, velottFitter ]
   monitorseq.Members += [ TrackMonitor(name = "VeloTTMonitor",
                                        TracksInContainer = 'Rec/Track/VeloTT',
                                        FullDetail = True)]
   postmonitorseq.Members += [ TrackMonitor(name = "VeloTTPostMonitor",
                                        TracksInContainer = 'Rec/Track/VeloTT',
                                        FullDetail = True)]

#trackmonitor.addTool( trackselector, name = "TrackSelector" )

trackselector = TrackSelector("MyTrackSelector")
trackselector.MinNVeloPhiHits = 5
trackselector.MinNVeloRHits = 5
trackselector.MaxChi2Cut = 10
trackcontainercopy = TrackContainerCopy("VeloSelectionAlg")
trackcontainercopy.inputLocation =   'Rec/Track/Velo'
trackcontainercopy.outputLocation =   'Rec/Track/VeloSelection'
trackcontainercopy.addTool( trackselector, name = "Selector") ;
#trackcontainercopy.OutputLevel = DEBUG
veloreco.Members += [ trackcontainercopy ]
# also add the velott tracks
if runVeloTT:
   VeloTTSelectionAlg = TrackContainerCopy("VeloTTSelectionAlg")
   VeloTTSelectionAlg.inputLocation =   'Rec/Track/VeloTT'
   VeloTTSelectionAlg.outputLocation =   'Rec/Track/VeloSelection'
   VeloTTSelectionAlg.addTool( trackselector, name = "Selector") ;
   veloreco.Members += [ VeloTTSelectionAlg ]

#   TrackMonitor(name = "ForwardMonitor",TracksInContainer = 'Rec/Track/Forward'),
monitorseq.Members += [ TrackMonitor(name = "AlignTracksMonitor",
                                     TracksInContainer = 'Rec/Track/VeloSelection',
                                     FullDetail = True) ]
postmonitorseq.Members += [ TrackMonitor(name = "AlignTracksPostMonitor",
                                         TracksInContainer = 'Rec/Track/VeloSelection',
                                         FullDetail = True) ]
veloreco.Members += [ monitorseq,postmonitorseq ]

# add the alignment. this is a bit tricky, but for now I just want the histograms
from Configurables import (AlRobustAlignAlg )
#from Gaudi.Configuration import NTupleSvc
#NTupleSvc().Output = [ "FILE1 DATAFILE='aligntuples.root' TYPE='ROOT' OPT='NEW' " ]

monitorseq.Members += [ AlRobustAlignAlg("RobustAlignAlg", TrackLocation = 'Rec/Track/VeloCopy') ]
monitorseq.Members += [ AlRobustAlignAlg("RobustAlignAlg2", TrackLocation = 'Rec/Track/VeloSelection') ]
postmonitorseq.Members += [ AlRobustAlignAlg("PostRobustAlignAlg", TrackLocation = 'Rec/Track/VeloCopy') ]

ApplicationMgr().HistogramPersistency = 'ROOT'
HistogramPersistencySvc().OutputFile = "alignhistos.root"

## Need this to read raw data
importOptions('$STDOPTS/DecodeRawEvent.py')
from Configurables import DataOnDemandSvc
DataOnDemandSvc().OutputLevel = 2
ApplicationMgr().ExtSvc += [ 'DataOnDemandSvc' ]

from Configurables import (DecodeVeloRawBuffer,RawBankToSTClusterAlg,RawBankToSTLiteClusterAlg,UpdateManagerSvc)
#importOptions('Velo-CommissioningConditions.py')
DecodeVeloRawBuffer('createVeloClusters').ForceBankVersion=3
DecodeVeloRawBuffer('createVeloClusters').RawEventLocation='Prev1/DAQ/RawEvent'
DecodeVeloRawBuffer('createVeloLiteClusters').ForceBankVersion=3
DecodeVeloRawBuffer('createVeloLiteClusters').RawEventLocation='Prev1/DAQ/RawEvent'
RawBankToSTClusterAlg('createITClusters').rawEventLocation = '/Event/Prev2/DAQ/RawEvent'
RawBankToSTClusterAlg('createTTClusters').rawEventLocation = '/Event/Prev2/DAQ/RawEvent'
RawBankToSTLiteClusterAlg('createITLiteClusters').rawEventLocation = '/Event/Prev2/DAQ/RawEvent'
RawBankToSTLiteClusterAlg('createTTLiteClusters').rawEventLocation = '/Event/Prev2/DAQ/RawEvent'


UpdateManagerSvc().ConditionsOverride += [
   "Conditions/Online/Velo/MotionSystem := double ResolPosLA =28.5; double ResolPosRC =-28.5;"
   ]

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
   data = ['PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32474/032474_0000081642.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32476/032476_0000081643.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32477/032477_0000081644.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32479/032479_0000081647.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32481/032481_0000081648.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32484/032484_0000081651.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32493/032493_0000081660.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32498/032498_0000081699.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32500/032500_0000081701.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32501/032501_0000081702.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32576/032576_0000081818.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32578/032578_0000081820.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32580/032580_0000081822.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32581/032581_0000081823.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32583/032583_0000081825.raw',
           'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/32586/032586_0000081828.raw'
           ]

   # these last data need some modifications because velo timing is off
   DecodeVeloRawBuffer('createVeloClusters').RawEventLocation='Prev2/DAQ/RawEvent'
   DecodeVeloRawBuffer('createVeloLiteClusters').RawEventLocation='Prev2/DAQ/RawEvent'

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
monitorSeq = appMgr.algorithm( 'MonitorSeq' )

## Print flow of application
alignment.printFlow(appMgr)

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

#appMgr.finalize()
appMgr.algorithm('PreMonitorSeq').Enable = True


