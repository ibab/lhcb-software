import Gaudi
import GaudiKernel.ProcessJobOptions
from Gaudi.Configuration  import *
from Configurables import TAlignment
from TAlignment import *
from TAlignment import Alignables



nEvents            = 100
nIter              = 3
#alignlevel         = 'sensors'
alignlevel         = 'halves'
#alignlevel = 'all'
computeVertexCorrelations = False

#elements = Alignables("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo(Right|Left)/Module..", "TxTyTz")
elements = Alignables.Alignables()

if alignlevel == 'sensors':
   elements = elements.VeloRSensors("TxTyRxRy")
   elements = elements.VeloPhiSensors("TxTyRxRyRz")
#   constraints = ["Tx","Ty","Tz","Szz","Szx","Szy"]
#   elements.VeloRSensors("Tz")
#   elements.VeloPhiSensors("Tz")
   constraints = ["Tz","Ty","Tx"]
   conddepths = []
   condname = "Detectors.xml"
elif alignlevel == 'halves':
   print "Going for VELO HALVES!"
#   elements.VeloLeft("TxTyRxRy")
#   elements.VeloRight("TxTyRxRy")
#   constraints = ["Tx","Ty","Rx","Ry"]
   elements.VeloLeft("Tx")
   elements.VeloRight("Tx")
   constraints = ["Tx"]
#   constraints = ["Tx","Ty","Tz","Szz","Szx","Szy"]
#   elements.VeloLeft("TxTyTz")
#   elements.VeloRight("TxTyTz")
   conddepths = [0,1]
   condname   = "Global.xml"
else :
   elements = ali.Velo("TxTyRxRy")
#   elements.TT("Tz")
#   elements.IT("Tz")
#   elements.OT("Tz")
   constraints = []#["TxTyRxRy"]
   conddepths = [0,1]
   condname   = "Global.xml"


#elements.VeloPileUpSensors("TxTy")

#elements.extend()

print "Aligning: ", elements
## Set output level
TAlignment().OutputLevel                  = INFO
TAlignment().setProp("ElementsToAlign", elements.__getslice__(1,elements.__len__()) )
TAlignment().NumIterations                = nIter
TAlignment().TrackContainer               = "Alignment/AlignmentTracks"
TAlignment().UseCorrelations              = True
TAlignment().Constraints                  = constraints
TAlignment().UseWeightedAverageConstraint = False
TAlignment().MinNumberOfHits              = 1
TAlignment().UsePreconditioning           = True
TAlignment().SolvTool                     = "gslSVDsolver"
TAlignment().WriteCondToXML               = True
TAlignment().CondFileName                 = condname
TAlignment().WriteCondSubDetList          = ["Velo"]
TAlignment().CondDepths                   = conddepths
TAlignment().Precision                    = 8
TAlignment().SimplifiedGeom               = True
TAlignment().Level			  = alignlevel
#TAlignment().VertexLocation = "Rec/Vertex/Primary"
TAlignment().Chi2Outlier = 10000
#TAlignment().RegularizationFactor = 1

## Call after all options are set

## Here we configure the track fitting/selection and what else?
## importOptions( trackingOpts )
from Configurables import (GetElementsToBeAligned, TrackContainerCopy, TrackSelector, Al__VertexResidualTool, Al__AlignConstraintTool)
#trackselectortool = TrackSelector( MinPCut = "3000", MaxChi2Cut="20", TrackTypes = [ "Long","Upstream","Velo" ] ) 
#trackselectortool = TrackSelector( MaxChi2Cut="20", TrackTypes = [ "Long","Upstream","Velo" ] )

elements = TAlignment().getProp( "ElementsToAlign" )
print "Elements = ", elements

trackselectortool = TrackSelector( "AlignTrackSelectorTool",
                                   MaxChi2Cut="20",
                                   MinNDoF=2,
                                   MinNVeloPhiHits=5,
                                   MinNVeloRHits=5,
                                   #TrackTypes = [ "Long","Velo","Upstream" ]
                                   TrackTypes = [ "Velo", "Long" ] ) 

trackselectoralg = TrackContainerCopy("AlignTrackSelector",
                                      inputLocation = "Rec/Track/Best",
                                      outputLocation = "Alignment/AlignmentTracks" )
trackselectoralg.addTool( GetElementsToBeAligned( OutputLevel     = TAlignment().getProp("OutputLevel")
                                                , Elements      = elements )
                        , name = "GetElementsToBeAligned" )

trackselectoralg.addTool( trackselectortool, "Selector" )

trkselector = TrackSelector("VertexTrackSelector",TrackTypes = [ "Long" ] )
vertexresidualtool = Al__VertexResidualTool("Al::VertexResidualTool")
#vertexresidualtool.addTool(TrackSelector("TrackSelector",TrackTypes = [ "Long" ] ) )
vertexresidualtool.addTool(trkselector,name = "MyTrackSelector")
vertexresidualtool.MyTrackSelector.TrackTypes = [ "Long" ]
#vertexresidualtool.MyTrackSelectorHandle = trkselector 
#vertexresidualtool.UseCorrelations = computeVertexCorrelations

TAlignment().filterSeq().Members.append( trackselectoralg  )

## run some monitoring code
from Configurables import ( TrackMonitor,TrackVertexMonitor )
trackmonitor = TrackMonitor("AlignTrackMonitor", TracksInContainer = "Alignment/AlignmentTracks" )
TAlignment().filterSeq().Members.append( trackmonitor )
TAlignment().filterSeq().Members.append(TrackVertexMonitor())

## To load parts of the database from a particular xml location
#DetectorDataSvc().DetDbLocation = "/data/bfys/janos/LHCbDB/lhcb.xml"

#constrainttool = Al__AlignConstraintTool("Al::AlignConstraintTool")
#constrainttool.ConstrainToNominal = True
#constrainttool.SigmaNominalTx = 0.0001 ;
#constrainttool.SigmaNominalTy = 0.0001 ;
#constrainttool.SigmaNominalTz = 0.0001 ;
#constrainttool.SigmaNominalRx = 1e-6 ;
#constrainttool.SigmaNominalRy = 1e-6 ;
#constrainttool.SigmaNominalRz = 1e-6 ;
