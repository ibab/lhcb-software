##############################################################################
#$Id: JpsiMuonMonitor.py,v 1.1.1.1 2008-12-05 16:41:05 pkoppenb Exp $
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
jspiseq = GaudiSequencer("JpsiMonitorSeq")
ApplicationMgr().TopAlg += [ jpsiseq ]
# @todo : check for appropriate selection
##############################################################################
#
# Make a J/psi with only one side mu-IDed
#
from Configurables import CombineParticles, PhysDesktop
jpsi = CombineParticles("Jpsi2MuPi")
Jpsi2MuPi.addTool(PhysDesktop())
Jpsi2MuPi.PhysDesktop.InputLocations = [ "StdLooseMuons", "StdNoPIDsPions" ]
Jpsi2MuPi.DecayDescriptor = "[J/psi(1S) -> mu+ pi-]cc"
# a good muon and any pion
Jpsi2MuPi.DaughtersCuts = { "pi+" : "(P>1*GeV)",
                            "mu+" : "(P>10*GeV) & (PT>1*GeV)" }
Jpsi2MuPi.CombinationCut = "(ADAMASS('J/psi(1S)')<50*MeV)"
Jpsi2MuPi.MotherCut = "(ALL)"
jpsiSeq.Members += [ Jpsi2MuPi ]
##############################################################################
#
# Plot quantities
#
