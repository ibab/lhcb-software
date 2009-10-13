"""
Basem Khanji + Olivier Leroy , 12/07/2009
Bs->JpsiPhi Biased stripping selection 
"""

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units

##################
# Jpsi Selection :
##################
JpsiFilter = FilterDesktop('JpsiFilter')
JpsiFilter.InputLocations = ['StdLTUnbiasedJpsi2MuMu']
# cuts 
JpsiFilter.Code = " (MINTREE('mu+'==ABSID, PIDmu) > -10.0)" \
                  " &(MINTREE('mu+'==ABSID,TRCHI2DOF)< 10.0)"\
                  " & (ADMASS('J/psi(1S)') < 50.*MeV)"\
                  " & (VFASPF(VCHI2/VDOF) < 15.0)"
# released the PID cut w.r.t offline
################
# Phi Selection:
################

Phi2KK = FilterDesktop('Phi2KK')
Phi2KK.InputLocations = ["StdLoosePhi2KK"]
Phi2KK.Code = "  (MINTREE('K+'==ABSID, PIDK) > -5.0)" \
              "& (MINTREE('K+'==ABSID, PT) > 200.0)"\
              "& (MINTREE('K+'==ABSID,TRCHI2DOF)< 10.0)"\
              "& (ADMASS('phi(1020)') < 20.*MeV)" \
              "& (VFASPF(VCHI2/VDOF) < 15.0)"
# released the pT kaons cut and the PID cut w.r.t offline 
######
# Bs :
######
BiasedBs2JpsiPhi = CombineParticles( "BiasedBs2JpsiPhi")
BiasedBs2JpsiPhi.DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
BiasedBs2JpsiPhi.InputLocations = ["JpsiFilter","Phi2KK"]
BiasedBs2JpsiPhi.addTool( OfflineVertexFitter() )
BiasedBs2JpsiPhi.VertexFitters.update( { "" : "OfflineVertexFitter"} )
BiasedBs2JpsiPhi.ReFitPVs= True
# cuts
BiasedBs2JpsiPhi.CombinationCut = "(ADAMASS('B_s0') < 300.*MeV)"
BiasedBs2JpsiPhi.MotherCut = "(VFASPF(VCHI2/VDOF) < 15) & (BPVDIRA>0.9995) & (BPVVDCHI2 > 4)"
# releasing a cut on Bsvertex Chi2 
# Stripping 
line = StrippingLine('Bs2JpsiPhiBiasedLine'
                                     , prescale = 1
                                     , algos = [JpsiFilter,Phi2KK ,BiasedBs2JpsiPhi ]
                                     )

