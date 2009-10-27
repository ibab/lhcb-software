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
mucut = '(PT>500*MeV) & (TRCHI2DOF<3) & (ISLONG) '
JpsiInclusive = StrippingMember( CombineParticles
                                 , 'Combine'
                                 , InputLocations = [ 'StdLooseMuons' ]
                                 , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                                 , DaughtersCuts = { 'mu+' : mucut , 
                                                     'mu-' : mucut }
                                 , CombinationCut = " (AM>2900*MeV) "
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<20) "
                                 )

# Define the line
## ############################################################
line = StrippingLine('JpsiInclusive'
                           , prescale = 1.
                           , algos = [ JpsiInclusive ]
                           )

