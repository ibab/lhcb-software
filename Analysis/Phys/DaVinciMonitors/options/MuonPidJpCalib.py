##############################################################################
# $Id: MuonPidJpCalib.py,v 1.1 2009-02-26 17:11:09 pkoppenb Exp $
#
# Author Alessio Sarti <Alessio.Sarti@lnf.infn.it>
#
##############################################################################
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop, ParticleMonitor, DaVinci
muonPIDJpSeq = GaudiSequencer("MuonPIDJpsiSequence")

JpsiCalib = CombineParticles("JpsiCalib")
JpsiCalib.addTool(PhysDesktop())
JpsiCalib.PhysDesktop.InputLocations  = [ "StdNoPIDsMuons" ]
JpsiCalib.DecayDescriptor = "J/psi(1S) -> mu+ mu-" ;
JpsiCalib.CombinationCut = "(ADAMASS('J/psi(1S)')<300*MeV)"

muonscut = "(TRCHI2DOF<3) & (P>3*GeV) & (PT>500*MeV) & (ISLONG) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<800*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV)"
JpsiCalib.DaughtersCuts = { "mu+" : muonscut,
			    "mu-" : muonscut }

childcuts = " ( ((CHILDCUT(ISMUON,1)) & (CHILDCUT((PT>1*GeV),1)) & (CHILDCUT((P>10*GeV),1)) ) | ((CHILDCUT(ISMUON,2)) & (CHILDCUT((PT>1*GeV),2)) & (CHILDCUT((P>10*GeV),2))) ) "
JpsiCalib.MotherCut = "(VFASPF(VCHI2/VDOF)<3) & " + childcuts
childcuts = " ( ((CHILDCUT(ISMUON,1)) & (CHILDCUT((PT>1*GeV),1)) & (CHILDCUT((P>10*GeV),1)) ) | ((CHILDCUT(ISMUON,2)) & (CHILDCUT((PT>1*GeV),2)) & (CHILDCUT((P>10*GeV),2))) ) "
JpsiCalib.MotherCut = "(VFASPF(VCHI2/VDOF)<3) & " + childcuts

muonPIDJpSeq.Members += [ JpsiCalib ]

plotJPsiPr = ParticleMonitor("JpsiPromptMoni")
plotJPsiPr.addTool(PhysDesktop())
plotJPsiPr.PhysDesktop.InputLocations = [ "JpsiCalib" ]
plotJPsiPr.PeakCut = "(ADMASS('J/psi(1S)')<50*MeV)"
plotJPsiPr.SideBandCut = "(ADMASS('J/psi(1S)')>100*MeV)"
plotJPsiPr.PlotTools = [ "MassPlotTool"]
muonPIDJpSeq.Members += [ plotJPsiPr ]
##############################################################################
#
# Add Sequences to Monitors
#
DaVinci().MoniSequence += [ muonPIDJpSeq ] # Muon PID Jpsi sequence
