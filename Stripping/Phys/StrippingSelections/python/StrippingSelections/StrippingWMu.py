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



#Pre-scaled lines

# Create W -> mu candidates out of std loose muons
## ############################################################
_muons =  DataOnDemand(Location = 'Phys/StdLooseMuons')


mucut =  '(PT>15*GeV)'
mucut_tight =  '(PT>20*GeV)'


_W_ps = FilterDesktop(name+"_ps"
                    , Code = mucut
                               )
                         
W_ps = Selection( "Sel"+name+"_ps",
                  Algorithm = _W_ps,
                  RequiredSelections = [_muons]
                  )

# build the SelectionSequence
sequence_ps = SelectionSequence("Seq"+name+"_ps",
                             TopSelection = W_ps
                             )
# Define the line
## ############################################################
line_ps = StrippingLine('W2Mu_ps'
                           , prescale = .1
                           , algos = [ W_ps ]
                           )



# Create W -> mu candidates out of std very loose muons
## ############################################################
_loosemuons =  DataOnDemand(Location = 'Phys/StdVeryLooseMuons')


_Wloose_ps = FilterDesktop(name+"loose_ps"
                    , Code = mucut
                               )
                         
Wloose_ps = Selection( "Sel"+name+"loose_ps",
                  Algorithm = _Wloose_ps,
                  RequiredSelections = [_loosemuons]
                  )

# build the SelectionSequence
sequenceloose_ps = SelectionSequence("Seq"+name+"loose_ps",
                             TopSelection = Wloose_ps
                             )


# Define the line
## ############################################################
lineloose_ps = StrippingLine('W2Muloose_ps'
                           , prescale = .1
                           , algos = [ Wloose_ps ]
                           )


# Create W -> mu candidates out of std No PIDs muons
## ############################################################
_NoPIDsmuons =  DataOnDemand(Location = 'Phys/StdNoPIDsMuons')

_WNoPIDs_ps = FilterDesktop(name+"NoPIDs_ps"
                    , Code = mucut
                               )
                         
WNoPIDs_ps = Selection( "Sel"+name+"NoPIDs_ps",
                  Algorithm = _WNoPIDs_ps,
                  RequiredSelections = [_NoPIDsmuons]
                  )

# build the SelectionSequence
sequenceNoPIDs_ps = SelectionSequence("Seq"+name+"NoPIDs_ps",
                             TopSelection = WNoPIDs_ps
                             )
# Define the line
## ############################################################
lineNoPIDs_ps = StrippingLine('W2MuNoPIDs_ps'
                           , prescale = .1
                           , algos = [ WNoPIDs_ps ]
                           )




#Non Pre-scaled lines

# Candidates from StdLooseMuons
## ############################################################

_W = FilterDesktop(name
                    , Code = mucut_tight
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
                           , algos = [ W ]
                           )

#Candidates from StdVeryLooseMuons
_Wloose = FilterDesktop(name+"loose"
                    , Code = mucut_tight
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
                           , algos = [ Wloose ]
                           )

