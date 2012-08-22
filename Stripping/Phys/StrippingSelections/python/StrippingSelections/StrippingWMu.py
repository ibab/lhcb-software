# Stripping Lines for W->MuNu and studies of their background
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani
#
# WMu signal:           StdAllLooseMuons,  pT>20GeV
# WMu control:          StdAllLooseMuons,  pT>15GeV                    (10% PRESCALE)
# WMu background:       StdAllNoPIDsMuons, pT>5GeV  &  Hlt1MBNoBias Dec (RATE LIMITED, 20% PRESCALE)
# SingleMuon control:   StdAllLooseMuons,  pT>10GeV &  Hlt2SingleMuonHighPT Dec     (1% PRESCALE)
# SingleMuon control:   StdAllLooseMuons,  pT>4.8GeV & Hlt2SingleMuonLowPT Dec      (20% PRESCALE)

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons, StdAllNoPIDsMuons

confdict_WMu = { 'WMu_Prescale'    : 1.0,
                 'WMuLow_Prescale' : 0.1,
                 'STNB_Prescale' : 0.2,
                 'WMu_Postscale'   : 1.0,
                 'SingMuon10_Prescale' : 0.01,
                 'SingMuon48_Prescale'  :0.2,
                 'pT'     : 20.,
                 'pTlow'  : 15.,
                 'pTvlow' :  5.,
                 'SingMuon10_pT': 10.,
                 'SingMuon48_pT': 4.8
                 }

default_name = 'WMu'

class WMuConf( LineBuilder ) :

    __configuration_keys__ = ( 'WMu_Prescale',
                               'WMuLow_Prescale',
                               'STNB_Prescale',
                               'WMu_Postscale',
                               'SingMuon10_Prescale',
                               'SingMuon48_Prescale', 
                               'pT',
                               'pTlow',
                               'pTvlow',
                               'SingMuon10_pT',
                               'SingMuon48_pT'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts

        _pT     = '(PT>%(pT)s*GeV)'%config
        #_pT     = "(PT>%(pT)s*GeV) & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        _pTlow  = '(PT>%(pTlow)s*GeV)'%config
        _pTvlow = '(PT>%(pTvlow)s*GeV)'%config

        _pTSingMuon10    = '(PT>%(SingMuon10_pT)s*GeV)'%config
        _pTSingMuon48    = '(PT>%(SingMuon48_pT)s*GeV)'%config


        #Single Muon Control Lines
        self.sel_Mu10 = makeFilter( self._myname + 'Mu10',
                                   StdAllLooseMuons,
                                   "from LoKiTracks.decorators import *",
                                   _pTSingMuon10
                                   )

        self.line_Mu10 = StrippingLine( self._myname + 'Control10Line',
                                       prescale  = config[ 'SingMuon10_Prescale' ],
                                       postscale = config[ 'WMu_Postscale' ],
                                       checkPV   = False,
                                       HLT = "HLT_PASS('Hlt2SingleMuonHighPTDecision')",
                                       selection = self.sel_Mu10
                                       )

        self.registerLine( self.line_Mu10 )
        

        self.sel_Mu48 = makeFilter( self._myname + 'Mu48',
                                   StdAllLooseMuons,
                                   "from LoKiTracks.decorators import *",
                                   _pTSingMuon48
                                   )

        self.line_Mu48 = StrippingLine( self._myname + 'Control4800Line',
                                       prescale  = config[ 'SingMuon48_Prescale' ],
                                       postscale = config[ 'WMu_Postscale' ],
                                       checkPV   = False,
                                       HLT = "HLT_PASS('Hlt2SingleMuonLowPTDecision')",
                                       selection = self.sel_Mu48
                                       )

        self.registerLine( self.line_Mu48 )


        # WMu signal

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


        # WMu control

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


        # WMu background

        self.sel_SingleTrackNoBias = makeFilter( self._myname + 'SingleTrackNoBias',
                                                 StdAllNoPIDsMuons,
                                                 "from LoKiTracks.decorators import *",
                                                 _pTvlow
                                                 )  

        self.line_SingleTrackNoBias = StrippingLine( self._myname + 'SingleTrackNoBiasLine',
                                                     prescale  = config[ 'STNB_Prescale' ],
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
