##############################################################################
# $Id: MuonPidLaCalib.py,v 1.2 2009-05-08 19:04:35 jonrob Exp $
#
# Author Alessio Sarti <Alessio.Sarti@lnf.infn.it>
#
##############################################################################
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop, ParticleMonitor, DaVinci

muonPIDLaSeq = GaudiSequencer("MuonPIDLambdaSequence")

LambdaCalibDDName = "MuonLambdaCalibDD"
LambdaCalibDD = CombineParticles(LambdaCalibDDName)
LambdaCalibDD.addTool(PhysDesktop())
LambdaCalibDD.PhysDesktop.InputLocations  = [ "StdNoPIDsDownPions","StdNoPIDsDownProtons" ]
LambdaCalibDD.DecayDescriptor = "[Lambda0 -> p+ pi-]cc" ;
LambdaCalibDD.CombinationCut = "(ADAMASS('Lambda0')<100*MeV)"
LambdaCalibDD.DaughtersCuts = { "p+"  :  "(P>3000*MeV) & (PT>100*MeV) & (ISDOWN) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))",
			        "pi-" :  "(P>3000*MeV) & (PT>100*MeV) & (ISDOWN) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))" }
LambdaCalibDD.MotherCut = " (VFASPF(VCHI2)<5) & (500<BPVVDZ) & (BPVVDZ<2000) & (BPVDIRA>0.9999995) "
muonPIDLaSeq.Members += [ LambdaCalibDD ]

plotLamDD = ParticleMonitor("MuonLamdaDDMoni")
plotLamDD.addTool(PhysDesktop())
plotLamDD.PhysDesktop.InputLocations = [ LambdaCalibDDName ]
plotLamDD.PeakCut = "(ADMASS('Lambda0')<10*MeV)"
plotLamDD.SideBandCut = "(ADMASS('Lambda0')>20*MeV)"
plotLamDD.PlotTools = [ "MassPlotTool"]
muonPIDLaSeq.Members += [ plotLamDD ]
##############################################################################
#
# Add Sequences to Monitors
#
DaVinci().MoniSequence += [ muonPIDLaSeq ] # Muon PID Lambda sequence
