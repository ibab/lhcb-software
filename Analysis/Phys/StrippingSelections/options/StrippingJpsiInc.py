## #####################################################################
# A stripping selection for inclusive J/psi(1S) -> mu+ mu- decays
#
# @authors G.Lanfranchi, A.Sarti 
# @date 2009-Sept-1
# 
## #####################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember


# Create Jpsi -> mumu candidates out of std loose muons
## ############################################################
#mucut = '(PT>500*MeV) & (TRCHI2DOF<3) & (ISLONG) & (ISMUONLOOSE) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<3000*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<7000*MeV) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>-10*MeV)'
mucut = '(PT>500*MeV) & (TRCHI2DOF<3) & (ISLONG)'
JpsiInclusive = StrippingMember( CombineParticles
                                 , 'Combine'
                                 , InputLocations = [ 'StdLooseMuons' ]
                                 , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                                 , DaughtersCuts = { 'mu+' : mucut , 
                                                     'mu-' : mucut }
                                 , CombinationCut = "(ADAMASS('J/psi(1S)')<200*MeV) | (ADAMASS('psi(2S)')<200*MeV) | (ADAMASS('Upsilon(1S)')<200*MeV) | (ADAMASS('Upsilon(2S)')<200*MeV) | (ADAMASS('Upsilon(3S)')<200*MeV) | (ADAMASS('B_s0')<200*MeV)"
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<20) "
                                 )

# Define the line
## ############################################################
jpinc_line = StrippingLine('JpsiInclusive'
                           , prescale = 1.
                           , algos = [ JpsiInclusive ]
                           , stream = 'Jpsi'
                           )

