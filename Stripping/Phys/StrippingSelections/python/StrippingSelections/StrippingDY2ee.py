## #####################################################################
# A stripping selection for Drell-Yan ( gamma* -> e+ e- ) decays
#
# @authors J.Keaveney
# @date 2010-1-24
# 
## #####################################################################

name = "dy2ee"

__all__ = ('name', 'DY', 'sequence')

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

# Create DY -> ee candidates out of std loose electrons
## ############################################################
_electrons =  DataOnDemand(Location = 'Phys/StdTightElectrons')

ecut_1 = '(PT>1*GeV)&(TRPCHI2>0.001)&(PIDe >1)&(MIPDV(PRIMARY)/ MIPCHI2DV(PRIMARY)< 5)'
ecut_2 = '(PT>2*GeV)&(TRPCHI2>0.001)&(PIDe >1)&(MIPDV(PRIMARY)/ MIPCHI2DV(PRIMARY)< 5)'


_DY1 = CombineParticles(name+"1",
                         DecayDescriptor = 'Z0 -> e+ e-',
                         DaughtersCuts = { 'e+' : ecut_1 , 
                                           'e-' : ecut_1 },
                         MotherCut = "(MM>2.5*GeV) & (MM<5*GeV ) ",
                         WriteP2PVRelations = False
                         )

_DY2 = CombineParticles(name+"2",
                         DecayDescriptor = 'Z0 -> e+ e-',
                        DaughtersCuts = {  'e+'  : ecut_2 , 
                                           'e-'  : ecut_2 },
                         MotherCut = "(MM>5*GeV) & (MM<10*GeV ) ",
                         WriteP2PVRelations = False
                         )

_DY3 = CombineParticles(name+"3",
                         DecayDescriptor = 'Z0 -> e+ e-',
                         DaughtersCuts = { 'e+' : ecut_2 , 
                                           'e-' : ecut_2 },
                         MotherCut = "(MM>10*GeV) & (MM<20*GeV ) ",
                         WriteP2PVRelations = False
                         )

_DY4 = CombineParticles(name+"4",
                         DecayDescriptor = 'Z0 -> e+ e-',
                         DaughtersCuts = { 'e+' : ecut_2 , 
                                           'e-' : ecut_2 },
                         MotherCut = "(MM>20*GeV) &(MM<40*GeV ) ",
                         WriteP2PVRelations = False
                         )


                         
DY1 = Selection( "Sel"+name+"1",
                  Algorithm = _DY1,
                  RequiredSelections = [_electrons]
                  )

DY2 = Selection( "Sel"+name+"2",
                  Algorithm = _DY2,
                  RequiredSelections = [_electrons]
                  )
DY3 = Selection( "Sel"+name+"3",
                  Algorithm = _DY3,
                  RequiredSelections = [_electrons]
                  )
DY4 = Selection( "Sel"+name+"4",
                  Algorithm = _DY4,
                  RequiredSelections = [_electrons]
                  )




# build the SelectionSequence
sequence1 = SelectionSequence("Seq"+name+"1",
                             TopSelection = DY1
                             )
sequence2 = SelectionSequence("Seq"+name+"2",
                             TopSelection = DY2
                             )
sequence3 = SelectionSequence("Seq"+name+"3",
                             TopSelection = DY3
                             )
sequence4 = SelectionSequence("Seq"+name+"4",
                             TopSelection = DY4
                             )

# Define the lines
## ############################################################
line1 = StrippingLine('DY2ee1'
                           , prescale = .1
                           , algos = [ DY1 ]
                           )

line2 = StrippingLine('DY2ee2'
                           , prescale = 1.
                           , algos = [ DY2 ]
                           )

line3 = StrippingLine('DY2ee3'
                           , prescale = 1.
                           , algos = [ DY3 ]
                           )

line4 = StrippingLine('DY2ee4'
                           , prescale = 1.
                           , algos = [ DY4 ]
                           )
