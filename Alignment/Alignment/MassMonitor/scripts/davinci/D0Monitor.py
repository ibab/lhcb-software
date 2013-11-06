#######################################################################
# D0 -> K pi
#
# Example DaVinci script demonstrating how to analyze data with MassBassMonitor
# This script should be used when running over the CharmCompleteEvent stream
# It selects and analyzes D0 -> K- pi+ (where D0 comes from D*) candidates
#
# @author Jack Wimberley
# @date 2013-06-06
########################################################################

from Gaudi.Configuration import *
from Configurables import DaVinci
from GaudiKernel.SystemOfUnits import *
from MassMonitor.ExampleSelections import *
from MassMonitor.ExampleMonitors import *

# OPTIONS
MessageSvc().OutputLevel = INFO
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1000
DaVinci().HistogramFile = "D02KPi_histos.root"
DaVinci().TupleFile = "D02KPi.root"

doing2011 = True
from Configurables import CondDB
if (doing2011):
    DaVinci().DataType = "2011"
    CondDB().UseLatestTags = ["2011"]
else:
    DaVinci().DataType = "2012"
    CondDB().UseLatestTags = ["2012"]

# --
# D0
# --

D0Seq, D0Loc = exampleD0FromDstarSelection()
D0PreMon = exampleD0Monitor("D0PREMON",D0Loc)
D0PostMon = exampleD0Monitor("D0POSTMON",D0Loc)

# --------
# REFITTER
# --------

from DaVinciTrackRefitting.ParticleRefitter import ParticleRefitterSeq
Updater = ParticleRefitterSeq(inputs=[ D0Loc ], scale = True)

# ---------
# SEQUENCES
# ---------

from Configurables import GaudiSequencer
D0GS = GaudiSequencer("D0Sequence")
D0GS.Members = [ D0Seq, D0PreMon, Updater, D0PostMon ]

# -------------------------------
# APPEND ALL SEQUENCES TO DAVINCI
# -------------------------------

DaVinci().appendToMainSequence( [ D0GS ] )
