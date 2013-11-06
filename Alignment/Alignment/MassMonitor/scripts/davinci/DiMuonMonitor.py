#######################################################################
# DIMUON RESONANCES
#
# Example DaVinci script demonstrating how to analyze data with MassBassMonitor
# This script should be used when running over the DIMUON stream
# It selects and analyzes J/psi, psi(2S), Ups(1S), and Z0 candidates
# It also selects Ups(1S) - Ups(3S) simultaneously, but MassBiasesFitter doesn't have support for these yet
#
# @author Jack Wimberley
# @date 2013-06-06
########################################################################

from Gaudi.Configuration import *
from Configurables import DaVinci

# -------
# OPTIONS
# -------

MessageSvc().OutputLevel = INFO
DaVinci().EvtMax = -1
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

from MassMonitor.ExampleSelections import *
from MassMonitor.ExampleMonitors import *

# -----
# JPSI
# -----

JpsiSeq = exampleJpsiSelection('JpsiSeq')
JpsiPreMon = exampleJpsiMonitor('JPSIPREMON',JpsiSeq)
JpsiPostMon = exampleJpsiMonitor('JPSIPOSTMON',JpsiSeq)

# -----
# PSI2S
# -----

Psi2SSeq = examplePsi2SSelection('Psi2SSeq')
Psi2SPreMon = examplePsi2SMonitor('PSI2SPREMON',Psi2SSeq)
Psi2SPostMon = examplePsi2SMonitor('PSI2SPOSTMON',Psi2SSeq)

# -------
# UPS(1S)
# -------

Ups1SSeq = exampleUps1SSelection('Ups1SSeq')
Ups1SPreMon = exampleUps1SMonitor('UPS1SPREMON',Ups1SSeq)
Ups1SPostMon = exampleUps1SMonitor('UPS1SPOSTMON',Ups1SSeq)

# -------
# UPS(#S)
# -------

UpsAllSSeq = exampleUpsAllSSelection('UpsAllSSeq')
UpsAllSPreMon = MassBiasesMonitor('UPSALLSPREMON',UpsAllSSeq)
aUpsAllSPostMon = MassBiasesMonitor('UPSALLSPOSTMON',UpsAllSSeq)

# --
# Z0
# --

Z0Seq = exampleZ0Selection('Z0Seq')
Z0PreMon = exampleZ0Monitor('Z0PREMON',Z0Seq)
Z0PostMon = exampleZ0Monitor('Z0POSTMON',Z0Seq)


# --------
# REFITTER
# --------

from DaVinciTrackRefitting.ParticleRefitter import ParticleRefitterSeq
Updater = ParticleRefitterSeq(inputs = [ JpsiSeq.outputLocation(),
                                         Psi2SSeq.outputLocation(),
                                         Ups1SSeq.outputLocation(),
                                         UpsAllSSeq.outputLocation(),
                                         Z0Seq.outputLocation() ], scale = True)

# ---------
# SEQUENCES
# ---------

from Configurables import GaudiSequencer
JpsiGS = GaudiSequencer("JpsiSequence")
JpsiGS.Members = [ JpsiFilt.sequence("JpsiFiltSeq"), JpsiSeq.sequence(), JpsiPreMon, Updater, JpsiPostMon ]
Psi2SGS = GaudiSequencer("Psi2SSequence")
Psi2SGS.Members = [ Psi2SFilt.sequence("Psi2SFiltSeq"), Psi2SSeq.sequence(), Psi2SPreMon, Updater, Psi2SPostMon ]
Ups1SGS = GaudiSequencer("Ups1SSequence")
Ups1SGS.Members = [ Ups1SFilt.sequence("Ups1SFiltSeq"), Ups1SSeq.sequence(), Ups1SPreMon, Updater, Ups1SPostMon ]
UpsAllSGS = GaudiSequencer("UpsAllSSequence")
UpsAllSGS.Members = [ UpsAllSFilt.sequence("UpsAllSFiltSeq"), UpsAllSSeq.sequence(), UpsAllSPreMon, Updater, UpsAllSPostMon ]
Z0GS = GaudiSequencer("Z0Sequence")
Z0GS.Members = [ Z0Filt.sequence("Z0FiltSeq"), Z0Seq.sequence(), Z0PreMon, Updater, Z0PostMon ]

# -------------------------------
# APPEND ALL SEQUENCES TO DAVINCI
# -------------------------------

DaVinci().appendToMainSequence( [ JpsiGS, Psi2SGS, Ups1SGS, UpsAllSGS, Z0GS ] )

