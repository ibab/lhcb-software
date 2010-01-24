## #####################################################################
# A stripping selection for Z0 -> e+ e- decays
#
# @authors J.Keaveney
# @date 2010-1-24
# 
## #####################################################################

name = "Z02ee"

__all__ = ('name', 'Z0', 'sequence')

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

# Create Z0 -> ee candidates out of std loose electrons
## ############################################################
_electrons =  DataOnDemand('stdLooseElectrons', Location = 'Phys/StdLooseElectrons')

ecut = '(PT>10*GeV) '

_Z0 = CombineParticles(name,
                         DecayDescriptor = 'Z0 -> e+ e-',
                         DaughtersCuts = { 'e+' : ecut , 
                                           'e-' : ecut },
                         MotherCut = "(MM>40*GeV) ",
                         WriteP2PVRelations = False
                         )
                         
Z0 = Selection( "Sel"+name,
                  Algorithm = _Z0,
                  RequiredSelections = [_electrons]
                  )

# build the SelectionSequence
sequence = SelectionSequence("Seq"+name,
                             TopSelection = Z0
                             )
# Define the line
## ############################################################
line = StrippingLine('Z02ee'
                           , prescale = 1.
                           , algos = [ sequence ]
                           )

