## #####################################################################
# A stripping selection for Drell-Yan ( gamma* -> mu+ mu- ) decays
#
# @authors J.Keaveney
# @date 2010-1-24
# 
## #####################################################################

name = "dy2MuMu"

__all__ = ('name', 'DY', 'sequence')

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

# Create DY -> mumu candidates out of std loose muons
## ############################################################
_muons =  DataOnDemand(Location = 'Phys/StdLooseMuons')


mucut_1_ps = '(PT>800*MeV)&(TRPCHI2>0.001)&(MIPDV(PRIMARY)/ MIPCHI2DV(PRIMARY)< 5)&(( PIDmu-PIDpi)>-3.0)'
mucut_2 = '(PT>1*GeV)&(TRPCHI2>0.001)&(MIPDV(PRIMARY)/ MIPCHI2DV(PRIMARY)< 5)'



#pre-scaled line for lowest mass bin
_DY1_ps = CombineParticles(name+"1_ps",
                         DecayDescriptor = 'Z0 -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut_1_ps, 
                                           'mu-' : mucut_1_ps },
                         MotherCut = "(MM>2.5*GeV) & (MM<5*GeV )",
                         WriteP2PVRelations = False
                         )


#lines with HLT2 requirements
_DY1_HLT2 = CombineParticles(name+"1_HLT2",
                         DecayDescriptor = 'Z0 -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut_1_ps, 
                                           'mu-' : mucut_1_ps },
                         MotherCut = "(MM>2.5*GeV)",
                         WriteP2PVRelations = False
                         )

_DY2_HLT2 = CombineParticles(name+"2_HLT2",
                         DecayDescriptor = 'Z0 -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut_1_ps, 
                                           'mu-' : mucut_1_ps },
                         MotherCut = "(MM>5*GeV)",
                         WriteP2PVRelations = False
                         )



_DY2 = CombineParticles(name+"2",
                         DecayDescriptor = 'Z0 -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut_2 , 
                                           'mu-' : mucut_2 },
                         MotherCut = "(MM>5*GeV) &(MM<10*GeV)",
                         WriteP2PVRelations = False
                         )

_DY3 = CombineParticles(name+"3",
                         DecayDescriptor = 'Z0 -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut_2 , 
                                           'mu-' : mucut_2 },
                         MotherCut = "(MM>10*GeV) &(MM<20*GeV ) ",
                         WriteP2PVRelations = False
                         )

_DY4 = CombineParticles(name+"4",
                         DecayDescriptor = 'Z0 -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut_2 , 
                                           'mu-' : mucut_2 },
                         MotherCut = "(MM>20*GeV) &(MM<40*GeV ) ",
                         WriteP2PVRelations = False
                         )




DY1_ps = Selection( "Sel"+name+"1_ps",
                  Algorithm = _DY1_ps,
                  RequiredSelections = [_muons]
                  )


DY1_HLT2 = Selection( "Sel"+name+"1_HLT2",
                  Algorithm = _DY1_HLT2,
                  RequiredSelections = [_muons]
                  )

DY2_HLT2 = Selection( "Sel"+name+"2_HLT2",
                  Algorithm = _DY2_HLT2,
                  RequiredSelections = [_muons]
                  )


DY2 = Selection( "Sel"+name+"2",
                  Algorithm = _DY2,
                  RequiredSelections = [_muons]
                  )
DY3 = Selection( "Sel"+name+"3",
                  Algorithm = _DY3,
                  RequiredSelections = [_muons]
                  )
DY4 = Selection( "Sel"+name+"4",
                  Algorithm = _DY4,
                  RequiredSelections = [_muons]
                  )



# build the SelectionSequence

sequence1_ps = SelectionSequence("Seq"+name+"1_ps",
                             TopSelection = DY1_ps
                             )

sequence1_HLT2 = SelectionSequence("Seq"+name+"1_HLT2",
                             TopSelection = DY1_HLT2
                             )

sequence2_HLT2 = SelectionSequence("Seq"+name+"2_HLT2",
                             TopSelection = DY2_HLT2
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

line1_ps = StrippingLine('DY2MuMu1_ps'
                           , prescale = .02
                           , algos = [ DY1_ps ]
                           )

line1_HLT2 = StrippingLine('DY2MuMu1_HLT2'
                           , prescale = .1
                           , algos = [ DY1_HLT2 ]
                           , HLT = "HLT_PASS_RE('Hlt2DiMuonDY.*Decision')"
                           )

line2_HLT2 = StrippingLine('DY2MuMu2_HLT2'
                           , prescale = .2
                           , algos = [ DY2_HLT2 ]
                           , HLT = "HLT_PASS_RE('Hlt2DiMuonDY.*Decision')"
                           )


line1_HLT2_a = StrippingLine('DY2MuMu1_HLT2_a'
                          , prescale = .1
                          , algos = [ DY1_HLT2 ]
                          , HLT = "HLT_PASS_RE('Hlt2(MuonFromHLT1|PassThrough).*Decision')"
                          )

line2_HLT2_a = StrippingLine('DY2MuMu2_HLT2_a'
                          , prescale = .2
                          , algos = [ DY2_HLT2 ]
                          , HLT = "HLT_PASS_RE('Hlt2(MuonFromHLT1|PassThrough).*Decision')"
                          )


line2 = StrippingLine('DY2MuMu2'
                           , prescale = 1.
                           , algos = [ DY2 ]
                           )

line3 = StrippingLine('DY2MuMu3'
                           , prescale = 1.
                           , algos = [ DY3 ]
                           )

line4 = StrippingLine('DY2MuMu4'
                           , prescale = 1.
                           , algos = [ DY4 ]
                           )
