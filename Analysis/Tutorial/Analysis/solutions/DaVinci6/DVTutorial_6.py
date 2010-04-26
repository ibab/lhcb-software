########################################################################
#
# Options for exercise 6
#
# @author Patrick Koppenburg
# @date 2008-06-03
#
########################################################################
from Gaudi.Configuration import *
#######################################################################
#
# Load the sequencer from Ex 4 
#
# import the SelectionSequence
from DaVinci4.solutions.Bs2JpsiPhi import SeqBs2JpsiPhi
# get the GaudiSequencer with everything we need
seq = SeqBs2JpsiPhi.sequence()
#######################################################################
#
# DecayTreeTuple
#
from Configurables import DecayTreeTuple, PhysDesktop
tuple = DecayTreeTuple() 
tuple.InputLocations = [ SeqBs2JpsiPhi.outputLocation() ]
tuple.ToolList +=  [
#      "TupleToolTrigger"
     "TupleToolMCTruth"
    , "TupleToolMCBackgroundInfo"
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPropertime"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "TupleToolTrackInfo"
#    , "TupleToolTISTOS"
    , "TupleToolTagging" ]
tuple.Decay = "[B_s0 -> (^J/psi(1S) => ^mu+ ^mu-) (^phi(1020) -> ^K+ ^K-)]cc"
#######################################################################
#
# Event Tuple
#
from Configurables import EventTuple
etuple = EventTuple()
etuple.ToolList = [ "TupleToolEventInfo", "TupleToolGeneration", "TupleToolTrigger"  ]

#######################################################################
#
# Configure the application
#
from Configurables import DaVinci
#DaVinci().TupleFile = "Tutorial6.root"         # Ntuple
DaVinci().TupleFile = "DVNtuples.root"         # Ntuple
DaVinci().HistogramFile='DVHistos.root'
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "MC09"                    # 
DaVinci().Simulation   = True                  # It's MC
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ seq ]             # The selection sequence
DaVinci().MoniSequence = [ tuple, etuple ]
#
# Trigger (Restore when fixed on 2008 data)
#
DaVinci().Hlt = True
DaVinci().HltThresholdSettings = "Physics_320Vis_300L0_10Hlt1_Aug09"   # some settings. See HltConf for more.
DaVinci().ReplaceL0BanksWithEmulated = True  # to get L0 compatible with Hlt
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci6/DVTutorial_6.py options/Bs2JpsiPhi2008.py
#
########################################################################
