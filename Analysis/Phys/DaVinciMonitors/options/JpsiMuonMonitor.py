##############################################################################
# $Id: JpsiMuonMonitor.py,v 1.4 2008-12-10 16:42:16 pkoppenb Exp $
#
# Jpsi muon monitor
#
# Author: Patrick Koppenburg <patrick.koppenburg@cern.ch>
#
##############################################################################
from Gaudi.Configuration import *
##############################################################################
#
# Make a sequence and take only events from given Hlt selections
#
from Configurables import GaudiSequencer
JpsiSeq = GaudiSequencer("JpsiMonitorSeq")
ApplicationMgr().TopAlg += [ JpsiSeq ]
##############################################################################
#
# Get right selection
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
plotter =  ParticleMonitor()
plotter.addTool(PhysDesktop())
plotter.PhysDesktop.InputLocations = [ "Jpsi2MuPi" ]
plotter.PeakCut = "(ADMASS('J/psi(1S)')<5*MeV)"
plotter.SideBandCut = "(ADMASS('J/psi(1S)')>20*MeV)"
plotter.PlotTools = [ "PidPlotTool" ]

JpsiSeq.Members += [ plotter ]
