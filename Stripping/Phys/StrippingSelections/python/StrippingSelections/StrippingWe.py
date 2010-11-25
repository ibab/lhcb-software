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
_electrons =  DataOnDemand(Location = 'Phys/StdLooseElectrons')

ecut = '(PT>20*GeV)&(TRPCHI2>0.001)&(PIDe>1)&(TRGHOSTPROB<0.6)'

_W = FilterDesktop(name
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
                           , algos = [ W ]
                           )


#pre-scaled line with looser Pt cut, and no ghost prob cut
ecut_1 = '(PT>15*GeV)&(TRPCHI2>0.001)&(PIDe>1)'



_W_ps = FilterDesktop(name+"_ps"
                    , Code = ecut_1
                               )
                         
W_ps = Selection( "Sel"+name+"_ps",
                  Algorithm = _W_ps,
                  RequiredSelections = [_electrons]
                  )

# build the SelectionSequence
sequence_ps = SelectionSequence("Seq"+name+"_ps",
                             TopSelection = W_ps
                             )
# Define the line
## ############################################################
line_ps = StrippingLine('W2e_ps'
                           , prescale = .1
                           , algos = [ W_ps ]
                           )




