__author__ = ['Phillip Urquijo']
__date__ = '21/05/2010'
__version__ = '$Revision: 1.2 $'

name = "MuMuSS"

__all__ = ('name', 'MuMuSS', 'sequence')

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

# Create DY -> mumu candidates out of std loose muons
## ############################################################
_muons =  DataOnDemand(Location = 'Phys/StdLooseMuons')

mucut = '(PT>1*GeV)'

_MuMuSS1 = CombineParticles(name,
                         DecayDescriptor = '[Z0 -> mu- mu-]cc',
                         DaughtersCuts = { 'mu-' : mucut },
                         MotherCut = "(MM>2.5*GeV) & (MM<5*GeV )",
                         WriteP2PVRelations = False
                         )

_MuMuSS2 = CombineParticles(name,
                         DecayDescriptor = '[Z0 -> mu- mu-]cc',
                         DaughtersCuts = { 'mu+' : mucut , 
                                           'mu-' : mucut },
                         MotherCut = "(MM>5*GeV) &(MM<10*GeV)",
                         WriteP2PVRelations = False
                         )

_MuMuSS3 = CombineParticles(name,
                         DecayDescriptor = '[Z0 -> mu- mu-]cc',
                         DaughtersCuts = { 'mu+' : mucut , 
                                           'mu-' : mucut },
                         MotherCut = "(MM>10*GeV) &(MM<20*GeV ) ",
                         WriteP2PVRelations = False
                         )

_MuMuSS4 = CombineParticles(name,
                         DecayDescriptor = '[Z0 -> mu- mu-]cc',
                         DaughtersCuts = { 'mu+' : mucut , 
                                           'mu-' : mucut },
                         MotherCut = "(MM>20*GeV) ",
                         WriteP2PVRelations = False
                         )




                         
MuMuSS1 = Selection( "Sel"+name+"1",
                  Algorithm = _MuMuSS1,
                  RequiredSelections = [_muons]
                  )

MuMuSS2 = Selection( "Sel"+name+"2",
                  Algorithm = _MuMuSS2,
                  RequiredSelections = [_muons]
                  )
MuMuSS3 = Selection( "Sel"+name+"3",
                  Algorithm = _MuMuSS3,
                  RequiredSelections = [_muons]
                  )
MuMuSS4 = Selection( "Sel"+name+"4",
                  Algorithm = _MuMuSS4,
                  RequiredSelections = [_muons]
                  )




# build the SelectionSequence
sequence1 = SelectionSequence("Seq"+name+"1",
                             TopSelection = MuMuSS1
                             )
sequence2 = SelectionSequence("Seq"+name+"2",
                             TopSelection = MuMuSS2
                             )
sequence3 = SelectionSequence("Seq"+name+"3",
                             TopSelection = MuMuSS3
                             )
sequence4 = SelectionSequence("Seq"+name+"4",
                             TopSelection = MuMuSS4
                             )

# Define the lines
## ############################################################
line1 = StrippingLine('MuMuSS1'
                           , prescale = 1.
                           , algos = [ sequence1 ]
                           )

line2 = StrippingLine('MuMuSS2'
                           , prescale = 1.
                           , algos = [ sequence2 ]
                           )

line3 = StrippingLine('MuMuSS3'
                           , prescale = 1.
                           , algos = [ sequence3 ]
                           )

line4 = StrippingLine('MuMuSS4'
                           , prescale = 1.
                           , algos = [ sequence4 ]
                           )
