#
# Example options to run Gauss in a pure reading mode with generator
# and MCtruth.
# Unpacking of all containers that one wants to access have to be done
# here only MCParticles and MCVertices are done, hence it works for .gen
# or .xgen files
# 
from Gauss.Configuration import *
from Configurables import UnpackMCParticle, UnpackMCVertex
from Configurables import MuonMultipleScatteringChecker

#-- Phases to be executed: initialization + monitor
#
def readOnly():

    ApplicationMgr().TopAlg = ["LbAppInit/ReadFile",
                               "GaudiSequencer/Unpack",
                               "GaudiSequencer/GenMonitor",
                               ]   

    unpack = GaudiSequencer( "Unpack" )
    unpack.Members = [ 'UnpackMCParticle/UnpackMCParticles','UnpackMCVertex/UnpackMCVertices'
                       ]
    #-- Must read events
    #   Overwrite property of no events input set for "normal" Gauss jobs
    #
    #   ...is set to "NONE" - now where in v37r5?...
    #
    ApplicationMgr().EvtSel = ""

    moni = GaudiSequencer("SimMonitor")

    moni.Members = moni.Members[0:-1]

##     if 'GaudiSequencer/DataUnpackTest' in moni.Members:
##         moni.Members.remove('GaudiSequencer/DataUnpackTest')

    alg = GaudiSequencer("DataUnpackTest")
    if alg in moni.Members:
        moni.Members.remove(alg)
    
    print "before", moni.Members
 
#-- Do the readOnly as a postconfig action for Gauss
#
appendPostConfigAction ( readOnly )

#--General Gauss setting and sequence
#
Gauss().Phases = []
Gauss().Output = 'NONE'

#-- Unpack all data
DataOnDemandSvc().AlgMap =  {'/Event/MC/Particles': 'UnpackMCParticle/UnpackMCParticle',
                             'pSim/MCParticles': 'UnpackMCParticle/UnpackMCParticle'
}

#------------------------------------------------------------------------
# Detailed Monitor
#------------------------------------------------------------------------
#Algorithm filling detailed info in an ntuple from HepMC
GaudiSequencer("Monitor").Members += [ "GaudiSequencer/GenMonitor" ]
GaudiSequencer("Monitor").Members += [ "GaudiSequencer/SimMonitor" ]

EventSelector().Input   += [
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000001_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000002_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000003_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000004_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000005_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000006_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000007_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000008_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000009_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000010_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000011_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000012_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000013_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000014_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000015_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000016_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000017_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000018_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000019_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000020_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000021_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000022_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000023_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000024_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00010475/0000/00010475_00000025_1.allstreams.dst?svcClass=lhcbdisk' TYP='POOL_ROOTTREE' OPT='READ'"
]

#-- Do not need them here, can use #importOptions($APPCONFIGOPTS/Conditions...)
LHCbApp( DataType = "2010", Simulation = True )
LHCbApp().DDDBtag = "head-20100504"
LHCbApp().CondDBtag = "sim-20100510-vc-md100"

importOptions("$APPCONFIGOPTS/Gauss/Beam3500GeV-md100-nu1.py")
LHCbApp().EvtMax = 100
