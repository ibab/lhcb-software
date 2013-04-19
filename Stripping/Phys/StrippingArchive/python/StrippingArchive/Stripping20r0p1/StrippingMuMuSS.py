# Stripping Lines for Study of SameSign Background to DY->MuMu
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# J.Anderson
#
# MuMuSS1 (3.2-5GeV): StdAllLooseMuons & P>10GeV & pT>1.5GeV (10% PRESCALE)
# MuMuSS2 (5-10GeV):  StdAllLooseMuons & P>10GeV & pT>3GeV
# MuMuSS3 (10-20GeV): StdAllLooseMuons & P>10GeV & pT>3GeV
# MuMuSS4 (>20GeV):   StdAllLooseMuons & P>10GeV & pT>3GeV

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons

confdict_MuMuSS = { 'MuMuSSLine1Prescale' :  .1,
                    'MuMuSSLine2Prescale' : 1.0,
                    'MuMuSSLine3Prescale' : 1.0,
                    'MuMuSSLine4Prescale' : 1.0,
                    'MuMuSSLinePostscale' : 1.0,
                    'MuMuSSLine1MinMass' :  3.2,
                    'MuMuSSLine1MaxMass' :  5.,
                    'MuMuSSLine2MinMass' :  5.,
                    'MuMuSSLine2MaxMass' : 10.,
                    'MuMuSSLine3MinMass' : 10.,
                    'MuMuSSLine3MaxMass' : 20.,
                    'MuMuSSLine4MinMass' : 20.,
                    'p'   : 10.,
                    'pT1' :  1.5,
                    'pT2' :  3.
                    }

name = 'MuMuSS'

class MuMuSSConf( LineBuilder ) :

    __configuration_keys__ = ( 'MuMuSSLine1Prescale',
                               'MuMuSSLine2Prescale',
                               'MuMuSSLine3Prescale',
                               'MuMuSSLine4Prescale',
                               'MuMuSSLinePostscale',
                               'MuMuSSLine1MinMass',
                               'MuMuSSLine2MinMass',
                               'MuMuSSLine3MinMass',
                               'MuMuSSLine4MinMass',
                               'MuMuSSLine1MaxMass',
                               'MuMuSSLine2MaxMass',
                               'MuMuSSLine3MaxMass',
                               'p',
                               'pT1',
                               'pT2'
                               )
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
        

        # Define the cuts

        _cut1 = '(P>%(p)s*GeV) & (PT>%(pT1)s*GeV)'%config
        _cut2 = '(P>%(p)s*GeV) & (PT>%(pT2)s*GeV)'%config

        _MuMuSS1MassCut = '(MM>%(MuMuSSLine1MinMass)s*GeV) & (MM<%(MuMuSSLine1MaxMass)s*GeV)'%config
        _MuMuSS2MassCut = '(MM>%(MuMuSSLine2MinMass)s*GeV) & (MM<%(MuMuSSLine2MaxMass)s*GeV)'%config
        _MuMuSS3MassCut = '(MM>%(MuMuSSLine3MinMass)s*GeV) & (MM<%(MuMuSSLine3MaxMass)s*GeV)'%config
        _MuMuSS4MassCut = '(MM>%(MuMuSSLine4MinMass)s*GeV)'%config


        # MuMuSS1

        self.sel_MuMuSS1 = makeSSCombination( self._myname + 'MuMuSS1',
                                              StdAllLooseMuons,
                                              _cut1,
                                              _MuMuSS1MassCut
                                              )
     
        self.line_MuMuSS1 = StrippingLine( self._myname + 'Line1',
                                           prescale  = config[ 'MuMuSSLine1Prescale' ],
                                           postscale = config[ 'MuMuSSLinePostscale' ],
                                           selection = self.sel_MuMuSS1
                                           )

        self.registerLine( self.line_MuMuSS1 )


        # MuMuSS2

        self.sel_MuMuSS2 = makeSSCombination( self._myname + 'MuMuSS2', 
                                              StdAllLooseMuons,
                                              _cut2,
                                              _MuMuSS2MassCut
                                              )
     
        self.line_MuMuSS2 = StrippingLine( self._myname + 'Line2',
                                           prescale  = config[ 'MuMuSSLine2Prescale' ],
                                           postscale = config[ 'MuMuSSLinePostscale' ],
                                           selection = self.sel_MuMuSS2
                                           )

        self.registerLine( self.line_MuMuSS2 )


        # MuMuSS3

        self.sel_MuMuSS3 = makeSSCombination( self._myname + 'MuMuSS3', 
                                              StdAllLooseMuons,
                                              _cut2,
                                              _MuMuSS3MassCut
                                              )
     
        self.line_MuMuSS3 = StrippingLine( self._myname + 'Line3',
                                           prescale  = config[ 'MuMuSSLine3Prescale' ],
                                           postscale = config[ 'MuMuSSLinePostscale' ],
                                           selection = self.sel_MuMuSS3
                                           )

        self.registerLine( self.line_MuMuSS3 )


        # MuMuSS4

        self.sel_MuMuSS4 = makeSSCombination( self._myname + 'MuMuSS4', 
                                              StdAllLooseMuons,
                                              _cut2,
                                              _MuMuSS4MassCut
                                              )
     
        self.line_MuMuSS4 = StrippingLine( self._myname + 'Line4',
                                           prescale  = config[ 'MuMuSSLine4Prescale' ],
                                           postscale = config[ 'MuMuSSLinePostscale' ],
                                           selection = self.sel_MuMuSS4
                                           )

        self.registerLine( self.line_MuMuSS4 )
      

def makeSSCombination( name, _input, _daughters, _mother ) :

    _combination = CombineParticles( DecayDescriptor    = '[Z0 -> mu- mu-]cc',
                                     DaughtersCuts      = { 'mu-' : _daughters },
                                     MotherCut          = _mother,
                                     WriteP2PVRelations = False
                                     )

    return Selection ( name,
                       Algorithm          = _combination,
                       RequiredSelections = [ _input ]
                       )
