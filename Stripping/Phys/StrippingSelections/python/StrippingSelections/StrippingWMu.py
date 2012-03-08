# Stripping Lines for W->MuNu and studies of their background
# Electroweak Group (Convenor: S.Bifani, J.Anderson)
#
# Adaptation of lines originally written by J.Keaveney and W.Barter
#
# (2012-02-28) S.Bifani
# WMu signal:     StdAllLooseMuons,  pT>20GeV
# WMu control:    StdAllLooseMuons,  pT>15GeV                    (10% PRESCALE)
# WMu background: StdAllNoPIDsMuons, pT>5GeV  & Hlt1MBNoBias Dec (RATE LIMITED)

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons, StdAllNoPIDsMuons

confdict_WMu = { 'WMu_Prescale'    : 1.0,
                 'WMuLow_Prescale' : 0.1,
                 'WMu_Postscale'   : 1.0,
                 'pT'     : 20.,
                 'pTlow'  : 15.,
                 'pTvlow' :  5.
                 }

default_name = 'WMu'

class WMuConf( LineBuilder ) :

    __configuration_keys__ = ( 'WMu_Prescale',
                               'WMuLow_Prescale',
                               'WMu_Postscale',
                               'pT',
                               'pTlow',
                               'pTvlow'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts

        _pT     = "(PT>%(pT)s*GeV)"%config
        #_pT     = "(PT>%(pT)s*GeV) & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        _pTlow  = "(PT>%(pTlow)s*GeV)"%config
        _pTvlow = "(PT>%(pTvlow)s*GeV)"%config


        # WMu signal: StdAllLooseMuons, pT>20GeV

        self.sel_WMu = makeFilter( self._myname + 'WMu',
                                   StdAllLooseMuons,
                                   "from LoKiTracks.decorators import *",
                                   _pT
                                   )

        self.line_WMu = StrippingLine( self._myname + 'Line',
                                       prescale  = config[ 'WMu_Prescale' ],
                                       postscale = config[ 'WMu_Postscale' ],
                                       checkPV   = False,
                                       selection = self.sel_WMu
                                       )

        self.registerLine( self.line_WMu )


        # WMu control: StdAllLooseMuons, pT>15GeV (10% PRESCALE)

        self.sel_WMuLow = makeFilter( self._myname + 'WMuLow',
                                      StdAllLooseMuons,
                                      "from LoKiTracks.decorators import *",
                                      _pTlow
                                      )

        self.line_WMuLow = StrippingLine( self._myname + 'LowLine',
                                          prescale  = config[ 'WMuLow_Prescale' ],
                                          postscale = config[ 'WMu_Postscale' ],
                                          checkPV   = False,
                                          selection = self.sel_WMuLow
                                          )

        self.registerLine( self.line_WMuLow )


        # WMu background: StdAllNoPIDsMuons, pT>5GeV & Hlt1MBNoBias Dec (RATE LIMITED)

        self.sel_SingleTrackNoBias = makeFilter( self._myname + 'SingleTrackNoBias',
                                                 StdAllNoPIDsMuons,
                                                 "from LoKiTracks.decorators import *",
                                                 _pTvlow
                                                 )  

        self.line_SingleTrackNoBias = StrippingLine( self._myname + 'SingleTrackNoBiasLine',
                                                     prescale  = config[ 'WMu_Prescale' ],
                                                     postscale = config[ 'WMu_Postscale' ],
                                                     checkPV   = False,
                                                     HLT       = "HLT_PASS( 'Hlt1MBNoBiasDecision' )",
                                                     selection = self.sel_SingleTrackNoBias
                                                     )

        self.registerLine( self.line_SingleTrackNoBias )


def makeFilter( name, _input, _preambulo, _code ) :

    _filter = FilterDesktop( Preambulo = [ _preambulo ],
                             Code      = _code
                             )

    return Selection ( name,
                       Algorithm          = _filter,
                       RequiredSelections = [ _input ]
                       )
