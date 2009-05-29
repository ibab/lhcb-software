##############################################################################
# $Id: MuID2BodyPlot.py,v 1.1 2009-05-29 14:03:22 asarti Exp $
#
# Author Alessio Sarti <Alessio.Sarti@lnf.infn.it>
#
##############################################################################
from Gaudi.Configuration import *
from Configurables import MuID2BodyPlot, CombineParticles, ParticleMonitor, DaVinci

muonIDm2BSeq_Lam = GaudiSequencer("MuonID2BodyLamSequence")

LambdaMonitor = CombineParticles("LambdaMonitor")
LambdaMonitor.InputLocations  = [ "StdNoPIDsDownPions","StdNoPIDsDownProtons" ]
LambdaMonitor.DecayDescriptor = "[Lambda0 -> p+ pi-]cc" ;
LambdaMonitor.CombinationCut = "(ADAMASS('Lambda0')<100*MeV)"
LambdaMonitor.DaughtersCuts = { "p+"  :  "(P>3000*MeV) & (PT>100*MeV) & (ISDOWN) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))",
			        "pi-" :  "(P>3000*MeV) & (PT>100*MeV) & (ISDOWN) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))" }
LambdaMonitor.MotherCut = " (VFASPF(VCHI2)<5) & (500<BPVVDZ) & (BPVVDZ<2000) & (BPVDIRA>0.9999995) "
muonIDm2BSeq_Lam.Members += [ LambdaMonitor ]

muonIDm2BSeq_Jps = GaudiSequencer("MuonID2BodyJpsiSequence")

JpsiMonitor = CombineParticles("JpsiMonitor")
JpsiMonitor.InputLocations  = [ "StdNoPIDsMuons" ]
JpsiMonitor.DecayDescriptor = "J/psi(1S) -> mu+ mu-" ;
JpsiMonitor.CombinationCut = "(ADAMASS('J/psi(1S)')<300*MeV)"

muonscut = "(TRCHI2DOF<3) & (P>3*GeV) & (PT>500*MeV) & (ISLONG) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<800*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV)"
JpsiMonitor.DaughtersCuts = { "mu+" : muonscut,
			    "mu-" : muonscut }

#childcuts = " (CHILDCUT(ISMUON,1)) & ( ((CHILDCUT((PT>1*GeV),1)) & (CHILDCUT((P>10*GeV),1)) ) | ((CHILDCUT((PT>1*GeV),2)) & (CHILDCUT((P>10*GeV),2))) ) "
childcuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((PT>1*GeV),1)) & (CHILDCUT((P>10*GeV),1)) "
JpsiMonitor.MotherCut = "(VFASPF(VCHI2/VDOF)<3) & " + childcuts

muonIDm2BSeq_Jps.Members += [ JpsiMonitor ]

MuIDLambdaPlot = MuID2BodyPlot("MuIDLambdaPlot")
MuIDLambdaPlot.InputLocations = [ "LambdaMonitor" ]
MuIDLambdaPlot.OutputLevel = 6

MuIDLambdaPlot.MassMean = 1115.68
MuIDLambdaPlot.MassWindow = 10.

MuIDLambdaPlot.JpsiAnalysis = 0
MuIDLambdaPlot.LambdaAnalysis = 1

#// MuonID preselection momentum (MeV/c)
MuIDLambdaPlot.PreSelMomentum = 3000.
#// MuonID momentum cut ranges (MeV/c)
MuIDLambdaPlot.MomentumCuts = [ 6000. , 10000. ]

#// MuonID FOI parameters

MuIDLambdaPlot.FOIfactor = 1.

MuIDLambdaPlot.XFOIParameter1 = [5.5, 4.0, 3.3, 2.8,
                         5.2, 3.6, 2.4, 2.4,
                         5.7, 4.4, 2.8, 2.3,
                         5.1, 3.1, 2.3, 2.1,
                         5.8, 3.4, 2.6, 2.8]

MuIDLambdaPlot.XFOIParameter2 = [11., 3., 1., 1.,
                         31., 28., 21., 17.,
                         30., 31., 27., 22.,
                         28., 33., 35., 47.,
                         31., 39., 56., 151.]

MuIDLambdaPlot.XFOIParameter3 = [0.2, 0.08, 0.03, 0.1,
                         0.06, 0.08, 0.10, 0.15,
                         0.04, 0.06, 0.09, 0.12,
                         0.08, 0.15, 0.23, 0.36,
                         0.07, 0.14, 0.24, 0.49]

MuIDLambdaPlot.YFOIParameter1 = [2.8, 1.7, -153., 1.9,
                         3.3, 2.1, 1.7, 1.6,
                         3.6, 2.8, 1.9, 1.8,
                         4.4, 3.3, 2.2, 2.2,
                         4.8, 3.9, 2.6, 2.3]

MuIDLambdaPlot.YFOIParameter2 = [3., 2., 156., 0.,
                         17., 15., 9., 5.,
                         26., 25., 16., 15.,
                         30., 49., 57., 92.,
                         32., 55., 96., 166.]

MuIDLambdaPlot.YFOIParameter3 = [0.03, 0.02, 0.00, 0.09,
                         0.13, 0.19, 0.19, 0.24, 
                         0.11, 0.19, 0.21, 0.32,
                         0.10, 0.22, 0.30, 0.52,
                         0.08, 0.20, 0.34, 0.52]


#//-----------------------------------------------------------------
#// Parameters of the Landau functions
#//-----------------------------------------------------------------
MuIDLambdaPlot.distMuon  = [0.311, 1.349, 0.524, 0.0020, 17., 10.6, 0.04, 4.1, 1.64]

MuIDLambdaPlot.distPion  = [11., -12., 0.2029, -0.026, 0.06, 0.59, 0.008, -29., 41.]
muonIDm2BSeq_Lam.Members += [ MuIDLambdaPlot ]


MuIDJpsiPlot = MuID2BodyPlot("MuIDJpsiPlot")
MuIDJpsiPlot.InputLocations = [ "JpsiMonitor" ]
MuIDJpsiPlot.OutputLevel = 6

#// MuonID Jpsi Mass Window(MeV/c)
MuIDJpsiPlot.MassMean = 3096.91
MuIDJpsiPlot.MassWindow = 50.

MuIDJpsiPlot.JpsiAnalysis = 1
MuIDJpsiPlot.LambdaAnalysis = 0

#// MuonID preselection momentum (MeV/c)
MuIDJpsiPlot.PreSelMomentum = 3000.
#// MuonID momentum cut ranges (MeV/c)
MuIDJpsiPlot.MomentumCuts = [ 6000. , 10000. ]

#// MuonID FOI parameters

MuIDJpsiPlot.FOIfactor = 1.

MuIDJpsiPlot.XFOIParameter1 = [5.5, 4.0, 3.3, 2.8,
                         5.2, 3.6, 2.4, 2.4,
                         5.7, 4.4, 2.8, 2.3,
                         5.1, 3.1, 2.3, 2.1,
                         5.8, 3.4, 2.6, 2.8]

MuIDJpsiPlot.XFOIParameter2 = [11., 3., 1., 1.,
                         31., 28., 21., 17.,
                         30., 31., 27., 22.,
                         28., 33., 35., 47.,
                         31., 39., 56., 151.]

MuIDJpsiPlot.XFOIParameter3 = [0.2, 0.08, 0.03, 0.1,
                         0.06, 0.08, 0.10, 0.15,
                         0.04, 0.06, 0.09, 0.12,
                         0.08, 0.15, 0.23, 0.36,
                         0.07, 0.14, 0.24, 0.49]

MuIDJpsiPlot.YFOIParameter1 = [2.8, 1.7, -153., 1.9,
                         3.3, 2.1, 1.7, 1.6,
                         3.6, 2.8, 1.9, 1.8,
                         4.4, 3.3, 2.2, 2.2,
                         4.8, 3.9, 2.6, 2.3]

MuIDJpsiPlot.YFOIParameter2 = [3., 2., 156., 0.,
                         17., 15., 9., 5.,
                         26., 25., 16., 15.,
                         30., 49., 57., 92.,
                         32., 55., 96., 166.]

MuIDJpsiPlot.YFOIParameter3 = [0.03, 0.02, 0.00, 0.09,
                         0.13, 0.19, 0.19, 0.24, 
                         0.11, 0.19, 0.21, 0.32,
                         0.10, 0.22, 0.30, 0.52,
                         0.08, 0.20, 0.34, 0.52]


#//-----------------------------------------------------------------
#// Parameters of the Landau functions
#//-----------------------------------------------------------------
MuIDJpsiPlot.distMuon  = [0.311, 1.349, 0.524, 0.0020, 17., 10.6, 0.04, 4.1, 1.64]

MuIDJpsiPlot.distPion  = [11., -12., 0.2029, -0.026, 0.06, 0.59, 0.008, -29., 41.]

muonIDm2BSeq_Jps.Members += [ MuIDJpsiPlot ]

##############################################################################
#
# Add Sequences to Monitors
#
DaVinci().MoniSequence += [ muonIDm2BSeq_Lam ] # Muon ID monitoring Lambda sequence
DaVinci().MoniSequence += [ muonIDm2BSeq_Jps ] # Muon ID monitoring Lambda sequence
