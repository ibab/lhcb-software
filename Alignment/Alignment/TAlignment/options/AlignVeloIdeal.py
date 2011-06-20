raise DeprecationWarning, "This file needs to be migrated to IOHelper for persistency"

from Gaudi.Configuration import *

nEvents            = 200
nIter              = 1
alignlevel         = 'sensors'
alignlevel         = 'modules'
#alignlevel = 'halves'
#alignlevel = 'all'
maxHitsPerTrackForVertexCorrelations = 999
usecorrelations = True
usevertexing = True
constraints = []
uselocalframe = True

from OTAlElements import *

from Alignables import *

#elements = Alignables("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo(Right|Left)/Module..", "TxTyTz")
elements = Alignables()

if alignlevel == 'sensors':
   elements.VeloRSensors("TxTyRxRy")
   elements.VeloPhiSensors("TxTyRxRyRz")
#   constraints = ["Tx","Ty","Tz","Szz","Szx","Szy"]
#   elements.VeloRSensors("Tz")
#   elements.VeloPhiSensors("Tz")
   constraints = ["Tz","Ty","Tx"]
   conddepths = []
   condname = "Detectors.xml"
if alignlevel == 'modules':
   elements.VeloModules("TxTy")
   #constraints = ["Tx","Ty","Tz"]
   #   constraints = ["Tx","Ty","Tz","Szz","Szx","Szy"]
   constraints = ["Tx","Ty"]
   constraints += ["VeloRight: .*?/VeloRight/.*? : Szx Szy"]
   constraints += ["VeloLeft : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/.*? : Szx Szy"]
   #constraints += ["VeloRight : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/.*? : Szx Szy"]
   #constraints += ["VeloLeft : /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/.*? : Tx Ty Szx Szy Rz SRz"]
   
#   elements.VeloRSensors("Tz")
   #   elements.VeloPhiSensors("Tz")
   #constraints = ["Tz","Ty","Tx","Szz","Szx","Szy"]
   #constraints = []
   conddepths = []
   condname = "Detectors.xml"
elif alignlevel == 'halves':
   elements.VeloLeft("TxTyTzRxRyRz")
   elements.VeloRight("TxTyTzRxRyRz")
#   constraints = ["Tx","Ty","Tz","Szz","Szx","Szy"]
#   elements.VeloLeft("TxTyTz")
#   elements.VeloRight("TxTyTz")
#   constraints = ["Tz","Tx","Ty","Rx"]
   constraints = []
   conddepths = [0,1]
   condname   = "Global.xml"
else :
   elements.Velo("Tz")
#   elements.TT("Tz")
#   elements.IT("Tz")
#   elements.OT("Tz")
   constraints = []#["Tz"]
   conddepths = [0,1]
   condname   = "Global.xml"
   
data = [ "PFN:rfio:/afs/cern.ch/lhcb/group/tracking/vol4/mneedham/boole.digi" ]
#FileCatalog().Catalogs = [ "xmlcatalog_file:../MattsCatalog.xml" ]
data = [ "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DIGI/0000/00001820_00000001_4.digi" ]
#data = [ "PFN:00001820_00000001_4.digi" ]
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#elements.VeloPileUpSensors("TxTy")

#/castor/cern.ch/user/s/sblusk/7TeV_Boff_Beam12_C/MisAlCh1/Boole/digi/Boole_7TeV_Boff_Beam12_C_MisAlCh1_1000_0.digi
#/castor/cern.ch/user/s/sblusk/7TeV_Boff_Beam12_C/MisAlCh1/Boole/digi/Boole_7TeV_Boff_Beam12_C_MisAlCh1_1000_1.digi
#/castor/cern.ch/user/s/sblusk/7TeV_Boff_Beam12_C/MisAlCh1/Boole/digi/Boole_7TeV_Boff_Beam12_C_MisAlCh1_1000_2.digi
#/castor/cern.ch/user/s/sblusk/7TeV_Boff_Beam12_C/MisAlCh1/Boole/digi/Boole_7TeV_Boff_Beam12_C_MisAlCh1_1000_3.digi

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

## Patttern Recognition?
alignment.Pat                          = False

## Set output level
alignment.DataType = 'DC06'
alignment.Simulation = True
alignment.OutputLevel                  = INFO
alignment.ElementsToAlign              = list(elements)
alignment.NumIterations                = nIter
alignment.AlignInputTrackCont           = "Alignment/AlignmentTracks"
#alignment.AlignInputTackCont           = "Rec/Track/Best"
alignment.UseCorrelations              = usecorrelations
alignment.Constraints                  = constraints
alignment.UseWeightedAverageConstraint = False
alignment.MinNumberOfHits              = 100
alignment.UsePreconditioning           = True
alignment.SolvTool                     = "gslSolver"
alignment.SolvTool                     = "DiagSolvTool"
alignment.WriteCondToXML               = True
alignment.CondFileName                 = condname
alignment.CondDepths                   = conddepths
#alignment.TopLevelElement              = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
alignment.Precision                    = 8
alignment.SimplifiedGeom               = True
if usevertexing :
   alignment.VertexLocation = "Rec/Vertex/Primary"
alignment.Chi2Outlier = 10000
#alignment.RegularizationFactor = 1

MisAlCh1COND = CondDBAccessSvc("MisAlCh1COND")
#MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/user/m/marcocle/public/Wouter/LHCBCOND_MisAlCh1.db/LHCBCOND"
MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/VeloSliceMisalignedTest2.db/LHCBCOND"
#MisAlCh1COND.ConnectionString = "sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/VeloSliceAlignedHalvesNoCorr.db/LHCBCOND"
#addCondDBLayer(MisAlCh1COND)

## Here we configure the track fitting/selection and what else?
## importOptions( trackingOpts )

ApplicationMgr().ExtSvc += [ 'DataOnDemandSvc' ]
# this sets up the linker from particles to hits
importOptions("$ASSOCIATORSROOT/options/Brunel.opts")
# this loads the hits
importOptions('$STDOPTS/DecodeRawEvent.py')
IODataManager().AgeLimit = 2

from Configurables import ( UnpackMCParticle, UnpackMCVertex )
unpackSequence = alignment.decodingSeq()
unpackSequence.Members = [UnpackMCParticle(), UnpackMCVertex() ]

MessageSvc().Format = "% F%40W%S%7W%R%T %0W%M"

from Configurables import (IdealTracksCreator,IdealStateCreator,MCReconstructible)

# this should just become a default value (though quite irrelevant for alignment:-)

# make an empty decoding sequence. (without this line AlConfigurable will create a non-empty one)
decseq = GaudiSequencer('DecodingSeq')

# create our real sequence
idealTrackSeq = alignment.filterSeq()
idealTrackSeq.Members = []
#idealTrackSeq.Members.append( BuildMCTrackInfo() )

## reconstruct ideal tracks
IdealStateCreator("IdealStateCreator").CorrectSlopes = True
idealtrackscreator = IdealTracksCreator("VeloIdealTracksCreator",
                                        MinNHits = 8,
                                        TrueStatesAtMCHits = True,
                                        AddTTClusters= False,
                                        AddITClusters = False,
                                        AddOTTimes = False )
idealtrackscreator.addTool( MCReconstructible(), name="TrackSelectorTool") 
#idealtrackscreator.TrackSelectorTool.OutputLevel = 1
#idealtrackscreator.OutputLevel = 1
idealTrackSeq.Members.append( idealtrackscreator )

from Configurables import (TrackListPrinter)
idealdumper = TrackListPrinter("IdealDumper",inputLocation = "MC/Track/Ideal" )
#idealTrackSeq.Members.append( idealdumper )


## fit the ideal tracks
from Configurables import (TrackEventFitter,TrackMasterFitter,TrackKalmanFilter)
idealtracksfittertool = TrackMasterFitter() ;
idealtracksfittertool.addTool( TrackKalmanFilter, "NodeFitter") ;
idealtracksfitter = TrackEventFitter("IdealTracksFitter",
                                     TracksInContainer = "MC/Track/Ideal" )
idealtracksfitter.addTool( idealtracksfittertool, "Fitter" )
idealtracksfitter.Fitter.ErrorP = [0,1e-9]
idealtracksfitter.Fitter.ErrorX2 = 400
idealtracksfitter.Fitter.ErrorY2 = 400
idealtracksfitter.Fitter.ErrorTx2 = 0.01 
idealtracksfitter.Fitter.ErrorTy2 = 0.01

idealtracksfitter.Fitter.ZPositions = []
idealtracksfitter.Fitter.MaxNumberOutliers = 0
idealtracksfitter.Fitter.NodeFitter.BiDirectionalFit = False
idealTrackSeq.Members.append( idealtracksfitter )


## copy the tracks, make some chisquare cut
from Configurables import (TrackContainerCopy, TrackSelector)
#trackselectortool = TrackSelector( MinPCut = "3000", MaxChi2Cut="20", TrackTypes = [ "Long","Upstream","Velo" ] ) 
#trackselectortool = TrackSelector( MaxChi2Cut="20", TrackTypes = [ "Long","Upstream","Velo" ] )
trackselectortool = TrackSelector( "AlignTrackSelectorTool",
                                   MaxChi2Cut="20",
                                   #MinNDoF=2,
                                   #MinNVeloPhiHits=4,
                                   #MinNVeloRHits=4
                                   )
trackselectoralg = TrackContainerCopy("AlignTrackSelector",
                                      inputLocation = "MC/Track/Ideal",
                                      outputLocation = "Alignment/AlignmentTracks" )
trackselectoralg.addTool( trackselectortool, "Selector" )

idealTrackSeq.Members.append( trackselectoralg )

## run the primary vertexing for these tracks
from Configurables import (PatPVOffline,PVOfflineTool)
pvfinder = PatPVOffline("PatPVOffline")
pvfinder.addTool( PVOfflineTool("PVOfflineTool", InputTracks=["Alignment/AlignmentTracks"]), name = "PVOfflineTool")
idealTrackSeq.Members.append( pvfinder )

## run some monitoring code
from Configurables import ( TrackMonitor,TrackVertexMonitor )
trackmonitor = TrackMonitor("IdealTrackMonitor",
                            TracksInContainer = "Alignment/AlignmentTracks" )
idealTrackSeq.Members.append( trackmonitor )
idealTrackSeq.Members.append(TrackVertexMonitor())

## configure some stuff for the alignment

from Configurables import (Al__VertexResidualTool, Al__AlignConstraintTool, Al__TrackResidualTool)
trkselector = TrackSelector("VertexTrackSelector",TrackTypes = [ "Long" ] )
vertexresidualtool = Al__VertexResidualTool("Al::VertexResidualTool")
#vertexresidualtool.addTool(TrackSelector("TrackSelector",TrackTypes = [ "Long" ] ) )
vertexresidualtool.addTool(trkselector,name = "MyTrackSelector")
vertexresidualtool.MyTrackSelector.TrackTypes = [ "Long" ]
vertexresidualtool.MaxHitsPerTrackForCorrelations = maxHitsPerTrackForVertexCorrelations

## To load parts of the database from a particular xml location
#DetectorDataSvc().DetDbLocation = "/data/bfys/janos/LHCbDB/lhcb.xml"
from Configurables import (AlignAlgorithm,Al__AlignUpdateTool,DiagSolvTool,GetElementsToBeAligned)
updatetool = Al__AlignUpdateTool("Al::AlignUpdateTool")
DiagSolvTool().WriteMonNTuple = False
DiagSolvTool().ApplyScaling = False
elementtool = GetElementsToBeAligned( "GetElementsToBeAligned" )
elementtool.UseLocalFrame = uselocalframe

#AlignAlgorithm("Alignment").OutputLevel = 1
#solvertool = DiagSolvTool("Alignment.MatrixSolverTool")
#solvertool.WriteMonNTuple = True
#solvertool.ApplyScaling = False

## Now lets run it
from GaudiPython import *
from GaudiPython import gbl

# POOL Persistency
importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

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

HistogramPersistencySvc().OutputFile = "alignhistos.root"

#from Gaudi.Configuration import NTupleSvc
#NTupleSvc().Output = [ "FILE1 DATAFILE='aligntuples.root' TYPE='ROOT' OPT='NEW' " ]


EventSelector().OutputLevel = 1

## Load the data on demand service
#importOptions('$STDOPTS/DecodeRawEvent.py')
#ApplicationMgr().ExtSvc += ['DataOnDemandSvc']

## Instantiate application manager
appMgr = AppMgr()
mainSeq = appMgr.algorithm( 'AlignmentMainSeq' )

## Print flow of application
alignment.printFlow(appMgr)

evtSel           = appMgr.evtSel()
evtSel.printfreq = 1
evtSel.open( data, typ = 'POOL_ROOT')
evtSel.OutputLevel = 1
appMgr.OutputLevel = 1

print "evtSel.input:"
print evtSel.Input

for i in range( nIter ) :
   print 'Begin iteration ', nIter
   mainSeq.Enable = False
   evtSel.rewind()
   mainSeq.Enable = True
   appMgr.run( nEvents )
   update( "Alignment", appMgr )
   print 'End iteration ', nIter

#appMgr.finalize()


