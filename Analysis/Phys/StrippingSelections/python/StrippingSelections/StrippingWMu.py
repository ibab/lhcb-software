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


# Create W -> mu candidates out of std very loose muons
## ############################################################
_loosemuons =  DataOnDemand('stdVeryLooseMuons', Location = 'Phys/StdVeryLooseMuons')


_Wloose = FilterDesktop(name+"loose"
                    , Code = mucut
                               )
                         
Wloose = Selection( "Sel"+name+"loose",
                  Algorithm = _W,
                  RequiredSelections = [_loosemuons]
                  )

# build the SelectionSequence
sequenceloose = SelectionSequence("Seq"+name+"loose",
                             TopSelection = Wloose
                             )
# Define the line
## ############################################################
lineloose = StrippingLine('W2Muloose'
                           , prescale = 1.
                           , algos = [ sequenceloose ]
                           )





# Create W -> mu candidates out of std No PIDs muons
## ############################################################
_NoPIDsmuons =  DataOnDemand('stdNoPIDsMuons', Location = 'Phys/StdNoPIDsMuons')


_WNoPIDs = FilterDesktop(name+"NoPIDs"
                    , Code = mucut
                               )
                         
WNoPIDs = Selection( "Sel"+name+"NoPIDs",
                  Algorithm = _WNoPIDs,
                  RequiredSelections = [_NoPIDsmuons]
                  )

# build the SelectionSequence
sequenceNoPIDs = SelectionSequence("Seq"+name+"NoPIDs",
                             TopSelection = WNoPIDs
                             )
# Define the line
## ############################################################
lineNoPIDs = StrippingLine('W2MuNoPIDs'
                           , prescale = 1.
                           , algos = [ sequenceNoPIDs ]
                           )
