## #####################################################################
# A stripping selection for W -> e decays
#
# @authors J.Keaveney
# @date 2010-1-24
# 
## #####################################################################

name = "We"

__all__ = ('name', 'W', 'sequence')

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

# Create W -> e candidates out of std loose electrons
## ############################################################
_electrons =  DataOnDemand('stdLooseElectrons', Location = 'Phys/StdLooseElectrons')

ecut = '(PT>10*GeV)'

_W = FilterDesktop(name
                    , InputLocations = ["StdLooseElectrons"]
                    , Code = ecut
                               )
                         
W = Selection( "Sel"+name,
                  Algorithm = _W,
                  RequiredSelections = [_electrons]
                  )

# build the SelectionSequence
sequence = SelectionSequence("Seq"+name,
                             TopSelection = W
                             )
# Define the line
## ############################################################
line = StrippingLine('W2e'
                           , prescale = 1.
                           , algos = [ sequence ]
                           )

