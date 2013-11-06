#######################################################################
# DIMUON RESONANCES
#
# Example DaVinci script demonstrating how to analyze data with MassBassMonitor
# This script should be used when running over the DiMuon stream
# It selects and analyzes J/psi, psi(2S), Ups(1S), and Z0 candidates
# It also selects Ups(1S) - Ups(3S) simultaneously, but MassBiasesFitter doesn't have support for these yet
#
# @author Jack Wimberley
# @date 2013-06-06
########################################################################

from Gaudi.Configuration import *
from Configurables import DaVinci
from GaudiKernel.SystemOfUnits import *
from MassMonitor.ExampleSelections import *
from MassMonitor.ExampleMonitors import *


# -------
# OPTIONS
# -------

MessageSvc().OutputLevel = INFO
DaVinci().EvtMax = 100
DaVinci().PrintFreq = 1000
DaVinci().HistogramFile = "DiMuonMonitor_histos.root"
DaVinci().TupleFile = "DiMuonMonitor.root"

doing2011 = True
from Configurables import CondDB
if (doing2011):
    DaVinci().DataType = "2011"
    CondDB().UseLatestTags = ["2011"]
else:
    DaVinci().DataType = "2012"
    CondDB().UseLatestTags = ["2012"]

# -------------
# DIMUON FILTER
# -------------

# The individual selections include filters, but this pre-event filter should save a small bit of time

from PhysConf.Filters import LoKi_Filters
Filt = LoKi_Filters(
    L0DU_Code = "L0_CHANNEL_RE('.*Muon')",
    HLT_Code  = "HLT_PASS_RE( 'Hlt1DiMuonHighMassDecision' ) & HLT_PASS_RE( 'Hlt2DiMuon.*Decision' )",
    STRIP_Code = "HLT_PASS_RE( 'StrippingFullDST.*Decision|StrippingZ02MuMuLineDecision' )"
    )
DaVinci().EventPreFilters = Filt.filters("Filters")

# -----
# JPSI
# -----

JpsiSeq, JpsiLoc = exampleJpsiSelection('JpsiSeq')
JpsiPreMon = exampleJpsiMonitor('JPSIPREMON',JpsiLoc)
JpsiPostMon = exampleJpsiMonitor('JPSIPOSTMON',JpsiLoc)

# -----
# PSI2S
# -----

Psi2SSeq, Psi2SLoc = examplePsi2SSelection('Psi2SSeq')
Psi2SPreMon = examplePsi2SMonitor('PSI2SPREMON',Psi2SLoc)
Psi2SPostMon = examplePsi2SMonitor('PSI2SPOSTMON',Psi2SLoc)

# -------
# UPS(1S)
# -------

Ups1SSeq, Ups1SLoc = exampleUps1SSelection('Ups1SSeq')
Ups1SPreMon = exampleUps1SMonitor('UPS1SPREMON',Ups1SLoc)
Ups1SPostMon = exampleUps1SMonitor('UPS1SPOSTMON',Ups1SLoc)

# -------
# UPS(#S)
# -------

UpsAllSSeq, UpsAllSLoc = exampleUpsAllSSelection('UpsAllSSeq')
UpsAllSPreMon = exampleUpsAllSMonitor('UPSALLSPREMON',UpsAllSLoc)
UpsAllSPostMon = exampleUpsAllSMonitor('UPSALLSPOSTMON',UpsAllSLoc)

# --
# Z0
# --

Z0Seq, Z0Loc = exampleZ0Selection('Z0Seq')
Z0PreMon = exampleZ0Monitor('Z0PREMON',Z0Loc)
Z0PostMon = exampleZ0Monitor('Z0POSTMON',Z0Loc)


# --------
# REFITTER
# --------

from DaVinciTrackRefitting.ParticleRefitter import ParticleRefitterSeq
Updater = ParticleRefitterSeq(inputs = [ JpsiLoc,
                                         Psi2SLoc,
                                         Ups1SLoc,
                                         UpsAllSLoc,
                                         Z0Loc ], scale = True)

# ---------
# SEQUENCES
# ---------

from Configurables import GaudiSequencer
JpsiGS = GaudiSequencer("JpsiSequence")
JpsiGS.Members = [ JpsiSeq, JpsiPreMon, Updater, JpsiPostMon ]
Psi2SGS = GaudiSequencer("Psi2SSequence")
Psi2SGS.Members = [ Psi2SSeq, Psi2SPreMon, Updater, Psi2SPostMon ]
Ups1SGS = GaudiSequencer("Ups1SSequence")
Ups1SGS.Members = [ Ups1SSeq, Ups1SPreMon, Updater, Ups1SPostMon ]
UpsAllSGS = GaudiSequencer("UpsAllSSequence")
UpsAllSGS.Members = [ UpsAllSSeq, UpsAllSPreMon, Updater, UpsAllSPostMon ]
Z0GS = GaudiSequencer("Z0Sequence")
Z0GS.Members = [ Z0Seq, Z0PreMon, Updater, Z0PostMon ]

# -------------------------------
# APPEND ALL SEQUENCES TO DAVINCI
# -------------------------------

DaVinci().appendToMainSequence( [ JpsiGS, Psi2SGS, Ups1SGS, UpsAllSGS, Z0GS ] )

