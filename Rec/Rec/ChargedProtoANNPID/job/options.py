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

DaVinci().DataType = "2011"
DaVinci().DDDBtag    = "MC11-20111020"
DaVinci().CondDBtag  = "MC11-20111020-vc-md100"

# MC11a sample 1
## NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='ProtoPIDANN.tuples.1.root' TYP='ROOT' OPT='NEW'"]
## EventSelector().Input = [
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000007_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000005_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000009_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000001_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000011_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000009_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000002_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000010_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000005_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000009_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000003_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000005_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000006_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000007_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000004_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000008_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
## "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000007_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##     ]

# MC11a sample 2
NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='ProtoPIDANN.tuples.2.root' TYP='ROOT' OPT='NEW'"]
EventSelector().Input = [
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000006_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000010_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000004_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000008_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000004_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000012_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000011_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000010_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000002_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000002_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013256_00000013_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000008_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000003_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000003_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000001_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013201_00000006_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC11a/DST/Inc-B/00013171_00000001_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
     ]

# MC10
## DaVinci().DDDBtag    = "head-20101206"
## DaVinci().CondDBtag  = "sim-20101210-vc-md100"

# MC10 sample 1
## NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='ProtoPIDANN.tuples.1.root' TYP='ROOT' OPT='NEW'"]
## EventSelector().Input = [
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000001_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000001_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000003_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000003_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000005_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000005_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000007_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000007_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000009_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000009_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000011_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000011_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
## ]

# MC10 sample 2
## NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='ProtoPIDANN.tuples.2.root' TYP='ROOT' OPT='NEW'"]
## EventSelector().Input = [
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000002_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000002_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000004_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000004_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000006_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000006_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000008_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000008_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000010_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000010_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##     "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00008590_00000012_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##    "DATAFILE='PFN:/usera/jonesc/NFS/data/MC/MC10/DST/Inc-B/00010110_00000012_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##]
