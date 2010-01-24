## #####################################################################
# A stripping selection for Z0 -> mu+ mu- decays
#
# @author J.Keaveney
# @date 2010-1-24
# 
## #####################################################################

name = "Z02MuMu"

__all__ = ('name', 'Z0', 'sequence')

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

# Create Z0 -> mumu candidates out of std loose muons
## ############################################################
_muons =  DataOnDemand('stdLooseMuons', Location = 'Phys/StdLooseMuons')

mucut = '(PT>10*GeV) '

_Z0 = CombineParticles(name,
                         DecayDescriptor = 'Z0 -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut , 
                                           'mu-' : mucut },
                         MotherCut = "(MM>40*GeV) ",
                         WriteP2PVRelations = False
                         )
                         
Z0 = Selection( "Sel"+name,
                  Algorithm = _Z0,
                  RequiredSelections = [_muons]
                  )

# build the SelectionSequence
sequence = SelectionSequence("Seq"+name,
                             TopSelection = Z0
                             )
# Define the line
## ############################################################
line = StrippingLine('Z02MuMu'
                           , prescale = 1.
                           , algos = [ sequence ]
                           )

