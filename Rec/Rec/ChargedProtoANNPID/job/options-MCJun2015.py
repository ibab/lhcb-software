from Gaudi.Configuration import *
from Configurables import ( DaVinci, GaudiSequencer )
from DecayTreeTuple.Configuration import *

#from Configurables import ( ANNGlobalPID__ChargedProtoANNPIDTrainingTuple )
#pidtuple = ANNGlobalPID__ChargedProtoANNPIDTrainingTuple("ANNPID")
#pidtuple.NTupleLUN = "ANNPIDTUPLE" 
#DaVinci().UserAlgorithms += [ pidtuple ]

from Configurables import ( DecayTreeTuple, LoKi__Hybrid__TupleTool )
tuple = DecayTreeTuple("ANNPID")
tuple.Decay    = "[pi+]cc"
tuple.NTupleLUN = "ANNPIDTUPLE"
tuple.Inputs = [ 'Phys/StdAllNoPIDsPions/Particles',
                 'Phys/StdNoPIDsUpPions/Particles',
                 'Phys/StdNoPIDsDownPions/Particles' ]
tuple.ToolList = [ "TupleToolANNPIDTraining", "TupleToolGeometry" ]
#lokiT = tuple.addTupleTool( LoKi__Hybrid__TupleTool, name = "LokiTool" )
#lokiT.Variables = { "MIPCHI2_PRIMARY" : "MIPCHI2DV(PRIMARY)" }
DaVinci().UserAlgorithms += [ tuple ]

DaVinci().EvtMax     = -1
DaVinci().PrintFreq  = 1000

#DaVinci().EvtMax     = 1000
#DaVinci().PrintFreq  = 100

#DaVinci().SkipEvents = 100000

DaVinci().InputType = 'DST'
DaVinci().Simulation = True

DaVinci().DataType  = "2015"

NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='ProtoPIDANN.MC.tuples.root' TYP='ROOT' OPT='NEW'"]

from Configurables import LHCbApp
LHCbApp().TimeStamp = True

# Set the compression level for the ROOT tuple file
from GaudiKernel.Configurable import ConfigurableGeneric as RFileCnv
RFileCnv('RFileCnv').GlobalCompression = "LZMA:6"

# Mag Down
#DaVinci().DDDBtag   = "head-20120413"
#DaVinci().CondDBtag = "sim-20120727-vc-md100"

#importOptions( "/usera/jonesc/MC-MagDown.py" )
#EventSelector().Input = ["DATAFILE='PFN:/r03/lhcb/jonesc/data/MC/MC12/DST/10000000/00024859_00000040_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
#EventSelector().Input = ["DATAFILE='PFN:root://clhcbdlf.ads.rl.ac.uk//castor/ads.rl.ac.uk/prod/lhcb/validation/Collision12/BHADRONCOMPLETEEVENT.DST/00019408/0000/00019408_00000017_1.bhadroncompleteevent.dst?svcClass=lhcbDst' TYP='POOL_ROOTTREE' OPT='READ'"]

# Mag Up
#DaVinci().DDDBtag   = "head-20120413"
#DaVinci().CondDBtag = "sim-20120727-vc-mu100"
#importOptions( "/usera/jonesc/MC-MagUp.py" )
