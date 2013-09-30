########################################################################
#
# Options for exercise 6
#
# @author Patrick Koppenburg
# @date 2008-06-03
#
########################################################################
from Gaudi.Configuration import *
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#######################################################################
#
# Load the sequencer from Ex 4 
#
# import the SelectionSequence
from DaVinci4.solutions.Bs2JpsiPhi import SeqBs2JpsiPhi
# get the GaudiSequencer with everything we need
seq = SeqBs2JpsiPhi.sequence()
#######################################################################
# now tell if you want MC or not
simulation = False 
#######################################################################
#
# DecayTreeTuple
#
from DecayTreeTuple.Configuration import *
tuple = DecayTreeTuple() 
tuple.Inputs = [ SeqBs2JpsiPhi.outputLocation() ]
# tuple.addTupleTool( "TupleToolGeometry") // already default
# tuple.addTupleTool( "TupleToolKinematic")// already default
tuple.addTupleTool( "TupleToolPropertime")
tuple.addTupleTool( "TupleToolPrimaries")
# tuple.addTupleTool( "TupleToolEventInfo")// already default
tuple.addTupleTool( "TupleToolTrackInfo")
tuple.addTupleTool( "TupleToolTagging")

if (simulation):
    tuple.addTupleTool( "TupleToolMCTruth")
    tuple.addTupleTool( "TupleToolMCBackgroundInfo")
    
tuple.Decay = "[B_s0 -> ^(J/psi(1S) -> ^mu+ ^mu-) ^(phi(1020) -> ^K+ ^K-)]CC"
#######################################################################
#
# Event Tuple
#
from Configurables import EventTuple
etuple = EventTuple()
# etuple.addTupleTool("TupleToolEventInfo")// already default
if (simulation): etuple.addTupleTool("TupleToolGeneration")
etuple.addTupleTool("TupleToolTrigger")

#######################################################################
#
# Configure the application
#
from Configurables import DaVinci
DaVinci().TupleFile = "Tutorial6.root"         # Ntuple
DaVinci().HistogramFile='DVHistos.root'
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "2012"                    # 
DaVinci().Simulation = True
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ seq ]             # The selection sequence
DaVinci().MoniSequence = [ tuple, etuple ]
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci6/DVTutorial_6.py options/Bs2JpsiPhi_Sim08a.py
#
########################################################################
