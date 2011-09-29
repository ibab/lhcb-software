# standalone.py
#
# a minimal example on how to get the ZooWriter up and running
#
# a more full-fleged example with lots of comments can be found in
# standalone-complicated.py
#
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, DaVinci, ZooWriter, CombineParticles
importOptions("$STDOPTS/PreloadUnits.opts")
        
# some dumb selection to see some signal
preselSeq = GaudiSequencer("MyZooSeq")
mybs = CombineParticles("MyBs");
mybs.Inputs = [ "Phys/StdLooseDsplus2KKPi", "Phys/StdTightKaons" ]
mybs.DecayDescriptor = "[B_s0 -> D_s- K+]cc"
mybs.DaughtersCuts = { "D_s-" : "MIPDV(PRIMARY)>0.1", "K+": "MIPDV(PRIMARY)>0.1" }
mybs.CombinationCut = "((AMAXDOCA('') < 1*mm) & (ADAMASS('B_s0') < 1000 * MeV))"
mybs.MotherCut = "((VFASPF(VCHI2/VDOF) < 25) & (ADMASS('B_s0') < 300 * MeV))"

# tell the job about the Zoo libaries
ApplicationMgr().Dlls += [ "Zoo" ]
zoowriter = ZooWriter("ZooWriter")
preselSeq.Members += [ mybs, zoowriter ]

# set up ST decoding (needed because the ZooWriter sometimes looks at clusters)
from STTools import STOfflineConf
STOfflineConf.DefaultConf().configureTools()

# for a full explanation, see standalone-complicated.py
zoowriter.WriteMC = True # write MC truth
zoowriter.WriteMCGenEventInfo = True # write generator level info? (generated event type)

# lists of PID codes always take the positive number to refer to both
# particle and antiparticle, -1 means write everything...
zoowriter.MCList = [ 321, 431, 531 ] # (abs.) PID of MC particles to write
zoowriter.BackgroundList = [ 321, 431, 531 ] # background category for PID
zoowriter.TriggerList = [ 321, 431, 531 ] # TIS/TOS/TOB info for PID

zoowriter.WriteDLL = True # write PID variables (DLL)

# particle "collections" to write out
zoowriter.InputCollections = [ "Phys/MyBs/Particles",
        "Phys/StdLooseDsplus2KKPi/Particles",
        "Phys/StdTightKaons/Particles" ]
# name of the corresponding branches in tuple
zoowriter.DecayName = [ "MyBs", "MyDs", "MyKaons" ]

zoowriter.Filename = "MC.root" # Output filename

# set up DaVinci: data type, db tags, nr of events, ...
DaVinci().InputType = "DST"
DaVinci().UserAlgorithms += [ preselSeq ]

# make sure this fits the data you are running over
DaVinci().DataType = "2010"
DaVinci().Simulation = True
DaVinci().EvtMax = 25000
DaVinci().DDDBtag = "head-20101206"
DaVinci().CondDBtag = "sim-20101210-vc-md100"
