## #####################################################################
# A stripping selection for W -> mu decays
#
# @authors J.Keaveney
# @date 2010-1-24
# 
## #####################################################################

name = "WMu"

__all__ = ('name', 'W', 'sequence')

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

# Create W -> mu candidates out of std loose muons
## ############################################################
_muons =  DataOnDemand('stdLooseMuons', Location = 'Phys/StdLooseMuons')

mucut = '(PT>10*GeV)'

_W = FilterDesktop(name
                    , InputLocations = ["StdLooseMuons"]
                    , Code = mucut
                               )
                         
W = Selection( "Sel"+name,
                  Algorithm = _W,
                  RequiredSelections = [_muons]
                  )

# build the SelectionSequence
sequence = SelectionSequence("Seq"+name,
                             TopSelection = W
                             )
# Define the line
## ############################################################
line = StrippingLine('W2Mu'
                           , prescale = 1.
                           , algos = [ sequence ]
                           )

