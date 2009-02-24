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
# Load the sequencer from Ex 4 and catch it
#
importOptions("$ANALYSISROOT/solutions/DaVinci4/TutorialSeq.py")
tutorialseq = GaudiSequencer("TutorialSeq")
#######################################################################
#
# DecayTreeTuple
#
from Configurables import DecayTreeTuple, PhysDesktop
tuple = DecayTreeTuple() 
tuple.addTool( PhysDesktop )
tuple.PhysDesktop.InputLocations = [ "Bs2JpsiPhi" ]
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
DaVinci().TupleFile = "Tutorial6.root"         # Ntuple
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation   = True                  # It's MC
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ tutorialseq, tuple, etuple ]
DaVinci().MainOptions  = ""                    # None
#
# Trigger (Restore when fixed on 2008 data)
#
DaVinci().HltType = 'Hlt1+Hlt2'                ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci6/DVTutorial_6.py options/Bs2JpsiPhi2008.py
#
########################################################################
