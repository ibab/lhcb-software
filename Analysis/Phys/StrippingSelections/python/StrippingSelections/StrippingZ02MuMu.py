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



# Create Z0 -> mumu candidates out of std very loose muons
## ############################################################
_loosemuons =  DataOnDemand('stdVeryLooseMuons', Location = 'Phys/StdVeryLooseMuons')


_Z0loose = CombineParticles(name+"loose",
                         DecayDescriptor = 'Z0 -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut , 
                                           'mu-' : mucut },
                         MotherCut = "(MM>40*GeV) ",
                         WriteP2PVRelations = False
                         )
                         
Z0loose = Selection( "Sel"+name+"loose",
                  Algorithm = _Z0loose,
                  RequiredSelections = [_loosemuons]
                  )

# build the SelectionSequence
sequenceloose = SelectionSequence("Seq"+name+"loose",
                             TopSelection = Z0loose
                             )
# Define the line
## ############################################################
lineloose = StrippingLine('Z02MuMuloose'
                           , prescale = 1.
                           , algos = [ sequenceloose ]

                           )



# Create Z0 -> mumu candidates out of NoPIDs  muons
## ############################################################
_NoPIDsmuons =  DataOnDemand('stdNoPIDsLooseMuons', Location = 'Phys/StdNoPIDsMuons')


_Z0NoPIDs = CombineParticles(name+"NoPIDs",
                         DecayDescriptor = 'Z0 -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut , 
                                           'mu-' : mucut },
                         MotherCut = "(MM>40*GeV) ",
                         WriteP2PVRelations = False
                         )
                         
Z0NoPIDs = Selection( "Sel"+name+"NoPIDs",
                  Algorithm = _Z0NoPIDs,
                  RequiredSelections = [_NoPIDsmuons]
                  )

# build the SelectionSequence
sequenceNoPIDs = SelectionSequence("Seq"+name+"NoPIDs",
                             TopSelection = Z0NoPIDs
                             )
# Define the line
## ############################################################
lineNoPIDs = StrippingLine('Z02MuMuNoPIDs'
                           , prescale = 1.
                           , algos = [ sequenceNoPIDs ]

                           )



