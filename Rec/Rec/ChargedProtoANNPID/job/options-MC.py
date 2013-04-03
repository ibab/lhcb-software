from Gaudi.Configuration import *
from Configurables import ( DaVinci, GaudiSequencer )
from Configurables import ( ANNGlobalPID__ChargedProtoANNPIDTrainingTuple,
                            ANNGlobalPID__ChargedProtoANNPIDTupleTool )

pidtuple = ANNGlobalPID__ChargedProtoANNPIDTrainingTuple("ANNPID")
pidtuple.addTool( ANNGlobalPID__ChargedProtoANNPIDTupleTool, name = "Tuple" )
pidtuple.Tuple.NTupleLUN = "ANNPIDTUPLE"
   
DaVinci().UserAlgorithms += [ pidtuple ]

DaVinci().EvtMax     = -1
DaVinci().PrintFreq  = 1000
#DaVinci().SkipEvents = 100000

DaVinci().InputType = 'DST'
DaVinci().Simulation = True

DaVinci().DataType  = "2012"

NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='ProtoPIDANN.MC.tuples.root' TYP='ROOT' OPT='NEW'"]

from Configurables import LHCbApp
LHCbApp().TimeStamp = True

# Mag Down
#DaVinci().DDDBtag   = "head-20120413"
#DaVinci().CondDBtag = "sim-20120727-vc-md100"
#importOptions( "/usera/jonesc/MC-MagDown.py" )
#EventSelector().Input = ["DATAFILE='PFN:/r02/lhcb/jonesc/data/MC/DEV/00019571_00000002_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
#EventSelector().Input = ["DATAFILE='PFN:root://clhcbdlf.ads.rl.ac.uk//castor/ads.rl.ac.uk/prod/lhcb/validation/Collision12/BHADRONCOMPLETEEVENT.DST/00019408/0000/00019408_00000017_1.bhadroncompleteevent.dst?svcClass=lhcbDst' TYP='POOL_ROOTTREE' OPT='READ'"]

# Mag Up
#DaVinci().DDDBtag   = "head-20120413"
#DaVinci().CondDBtag = "sim-20120727-vc-mu100"
#importOptions( "/usera/jonesc/MC-MagUp.py" )
