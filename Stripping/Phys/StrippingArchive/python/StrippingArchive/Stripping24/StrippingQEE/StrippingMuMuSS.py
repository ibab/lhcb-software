# Stripping Lines for Study of SameSign Background to DY->MuMu
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# J.Anderson
#
# MuMuSS1 (3.2-5GeV): StdAllLooseMuons & P>10GeV & pT>1.5GeV (10% PRESCALE)
# MuMuSS2 (5-10GeV):  StdAllLooseMuons & P>10GeV & pT>3GeV
# MuMuSS3 (10-20GeV): StdAllLooseMuons & P>10GeV & pT>3GeV
# MuMuSS4 (>20GeV):   StdAllLooseMuons & P>10GeV & pT>3GeV

__all__ = (
  'MuMuSSConf',
  'default_config',
)

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons
from GaudiKernel.SystemOfUnits import GeV


default_config = {
    'NAME'        : 'MuMuSS',
    'BUILDERTYPE' : 'MuMuSSConf',
    'WGs'         : [ 'QEE'],
    'STREAMS'     : [ 'EW' ],
    'CONFIG'      : { 
      'MuMuSSLine1Prescale' : 0.1,
      'MuMuSSLine2Prescale' : 1.0,
      'MuMuSSLine3Prescale' : 1.0,
      'MuMuSSLine4Prescale' : 1.0,
      'MuMuSSLinePostscale' : 1.0,  
      'MuMuSSLine1MinMass'  : 3.2  * GeV,
      'MuMuSSLine1MaxMass'  :  5.  * GeV,
      'MuMuSSLine2MinMass'  :  5.  * GeV,
      'MuMuSSLine2MaxMass'  : 10.  * GeV,
      'MuMuSSLine3MinMass'  : 10.  * GeV,
      'MuMuSSLine3MaxMass'  : 20.  * GeV,
      'MuMuSSLine4MinMass'  : 20.  * GeV,
      'p'     : 10. * GeV,
      'pT1'   : 1.5 * GeV,
      'pT2'   :  3. * GeV
    },
}


class MuMuSSConf( LineBuilder ) :

    __configuration_keys__ = default_config['CONFIG'].keys()
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )


        # Define the cuts

        _cut1 = '(P>%(p)s) & (PT>%(pT1)s)'%config
        _cut2 = '(P>%(p)s) & (PT>%(pT2)s)'%config

        _MuMuSS1MassCut = '(MM>%(MuMuSSLine1MinMass)s) & (MM<%(MuMuSSLine1MaxMass)s)'%config
        _MuMuSS2MassCut = '(MM>%(MuMuSSLine2MinMass)s) & (MM<%(MuMuSSLine2MaxMass)s)'%config
        _MuMuSS3MassCut = '(MM>%(MuMuSSLine3MinMass)s) & (MM<%(MuMuSSLine3MaxMass)s)'%config
        _MuMuSS4MassCut = '(MM>%(MuMuSSLine4MinMass)s)'%config


        # MuMuSS1

        self.sel_MuMuSS1 = makeSSCombination( name + 'MuMuSS1',
                                              StdAllLooseMuons,
                                              _cut1,
                                              _MuMuSS1MassCut
                                              )
     
        self.line_MuMuSS1 = StrippingLine( name + 'Line1',
                                           prescale  = config[ 'MuMuSSLine1Prescale' ],
                                           postscale = config[ 'MuMuSSLinePostscale' ],
                                           MDSTFlag = True,
                                           selection = self.sel_MuMuSS1
                                           )

        self.registerLine( self.line_MuMuSS1 )


        # MuMuSS2

        self.sel_MuMuSS2 = makeSSCombination( name + 'MuMuSS2', 
                                              StdAllLooseMuons,
                                              _cut2,
                                              _MuMuSS2MassCut
                                              )
     
        self.line_MuMuSS2 = StrippingLine( name + 'Line2',
                                           prescale  = config[ 'MuMuSSLine2Prescale' ],
                                           postscale = config[ 'MuMuSSLinePostscale' ],
                                           MDSTFlag = True,
                                           selection = self.sel_MuMuSS2
                                           )

        self.registerLine( self.line_MuMuSS2 )


        # MuMuSS3

        self.sel_MuMuSS3 = makeSSCombination( name + 'MuMuSS3', 
                                              StdAllLooseMuons,
                                              _cut2,
                                              _MuMuSS3MassCut
                                              )
     
        self.line_MuMuSS3 = StrippingLine( name + 'Line3',
                                           prescale  = config[ 'MuMuSSLine3Prescale' ],
                                           postscale = config[ 'MuMuSSLinePostscale' ],
                                           MDSTFlag = True,
                                           selection = self.sel_MuMuSS3
                                           )

        self.registerLine( self.line_MuMuSS3 )


        # MuMuSS4

        self.sel_MuMuSS4 = makeSSCombination( name + 'MuMuSS4', 
                                              StdAllLooseMuons,
                                              _cut2,
                                              _MuMuSS4MassCut
                                              )
     
        self.line_MuMuSS4 = StrippingLine( name + 'Line4',
                                           prescale  = config[ 'MuMuSSLine4Prescale' ],
                                           postscale = config[ 'MuMuSSLinePostscale' ],
                                           MDSTFlag = True,
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
