## #####################################################################
# A stripping selection for inclusive J/psi(1S) -> mu+ mu- decays
#
# @authors G.Lanfranchi, A.Sarti 
# @date 2009-Sept-1
# 
## #####################################################################

name = "DiMuonInc"

__all__ = ('name', 'Jpsi', 'sequence')

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

# Create Jpsi -> mumu candidates out of std loose muons
## ############################################################
_muons =  DataOnDemand('stdLooseMuons', Location = 'Phys/StdLooseMuons')

mucut = '(PT>500*MeV) & (TRCHI2DOF<3) & (ISLONG) '

_Jpsi = CombineParticles(name,
                         DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut , 
                                           'mu-' : mucut },
                         CombinationCut = " (AM>2700*MeV) ",
                         MotherCut = "(VFASPF(VCHI2/VDOF)<20) ",
                         WriteP2PVRelations = False
                         )
                         
Jpsi = Selection( "Sel"+name,
                  Algorithm = _Jpsi,
                  RequiredSelections = [_muons]
                  )

# build the SelectionSequence
sequence = SelectionSequence("Seq"+name,
                             TopSelection = Jpsi
                             )
# Define the line
## ############################################################
line = StrippingLine('DiMuonInclusive'
                           , prescale = 1.
                           , algos = [ sequence ]
                           )

