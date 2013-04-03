
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
DaVinci().Simulation = False

DaVinci().DataType  = "2012"

NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='ProtoPIDANN.Data.tuples.root' TYP='ROOT' OPT='NEW'"]

from Configurables import LHCbApp
LHCbApp().TimeStamp = True
