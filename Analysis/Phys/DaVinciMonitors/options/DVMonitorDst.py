##############################################################################
# $Id: DVMonitorDst.py,v 1.10 2009-02-15 17:37:38 jonrob Exp $
#
# syntax: gaudirun.py $DAVINCIMONITORSROOT/options/DVMonitorDst.py
#
# Author: Patrick Koppenburg <patrick.koppenburg@cern.ch>
#
##############################################################################
from DaVinci.Configuration import *
from Gaudi.Configuration import *
##############################################################################
#
# Make a sequence and take only events from given Hlt selections
#
from Configurables import GaudiSequencer
JpsiSeq = GaudiSequencer("JpsiMonitorSeq")
##############################################################################
#
# Get right selection @todo does not work yet 
#
# from Configurables import LoKi__HDRFilter   as HltDecReportsFilter
# HltDecReportsFilter  ( 'myname', Code = "HLT_PASS('somedecisionname')" )
#
##############################################################################
#
# Make a J/psi with only one side mu-IDed
#
from Configurables import CombineParticles, PhysDesktop
Jpsi2MuPi = CombineParticles("Jpsi2MuPi")
Jpsi2MuPi.addTool(PhysDesktop())
Jpsi2MuPi.PhysDesktop.InputLocations = [ "StdLooseMuons", "StdNoPIDsPions" ]
Jpsi2MuPi.DecayDescriptor = "[J/psi(1S) -> mu+ pi-]cc"
# a good muon and any pion
Jpsi2MuPi.DaughtersCuts = { "pi+" : "(P>1*GeV)",
                            "mu+" : "(P>10*GeV) & (PT>1*GeV)" }
Jpsi2MuPi.CombinationCut = "(ADAMASS('J/psi(1S)')<100*MeV)"
Jpsi2MuPi.MotherCut = "(ALL)"
JpsiSeq.Members += [ Jpsi2MuPi ]
##############################################################################
#
# Plot quantities
#
from Configurables import ParticleMonitor
plotter = ParticleMonitor("Jpsi2MuPiMoni")
plotter.addTool(PhysDesktop())
plotter.PhysDesktop.InputLocations = [ "Jpsi2MuPi" ]
plotter.PeakCut = "(ADMASS('J/psi(1S)')<5*MeV)"
plotter.SideBandCut = "(ADMASS('J/psi(1S)')>20*MeV)"
plotter.PlotTools = [ "MassPlotTool","MomentumPlotTool",
                      "CombinedPidPlotTool",
                      "RichPlotTool","CaloPlotTool","MuonPlotTool" ]
JpsiSeq.Members += [ plotter ]
##############################################################################
#
# RICH PID Monitoring from data
#
from Configurables import RichPIDQCConf
richSeq = GaudiSequencer("RichPIDMoniSeq")
RichPIDQCConf().setProp("CalibSequencer",richSeq)
RichPIDQCConf().PIDCalibrations = [ "DsPhiPi" ]  # The PID Calibration selections to run
#RichPIDQCConf().MCChecks = True                  # Enable MC checking as well
##############################################################################
#
# Add Sequences to Monitors
#
DaVinci().MoniSequence += [ JpsiSeq ] # J/psi sequence
DaVinci().MoniSequence += [ richSeq ] # RICH sequence
##############################################################################
#
# Histograms
#
DaVinci().HistogramFile = "DVMonitors.root"
##############################################################################
#
# Most of this will be configured from Dirac
#
##############################################################################
DaVinci().EvtMax = 1000
#DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Simulation = True
