# Stripping Lines for study of background to Electroweak processes
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani
#
# SingleTrackTIS:    StdAllNoPIDsMuons, pT>20GeV & TTHits & Hlt1xHlt2 TIS (10% PRESCALE)
# SingleTrackTISLow: StdAllNoPIDsMuons, pT>15GeV & TTHits & Hlt1xHlt2 TIS ( 1% PRESCALE)

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsMuons

confdict_SingleTrackTIS = { 'SingleTrackTIS_Prescale'    : 0.10,
                            'SingleTrackTISLow_Prescale' : 0.01,
                            'SingleTrackTIS_Postscale'   : 1.00,
                            'pT'    : 20.,
                            'pTlow' : 15.
                            }

name = 'SingleTrackTIS'

class SingleTrackTISConf( LineBuilder ) :

    __configuration_keys__ = ( 'SingleTrackTIS_Prescale',
                               'SingleTrackTISLow_Prescale',
                               'SingleTrackTIS_Postscale',
                               'pT',
                               'pTlow'
                               )
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts

        _cut    = "(PT>%(pT)s*GeV)    & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        _cutlow = "(PT>%(pTlow)s*GeV) & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        

        # SingleTrackTIS

        self.sel_NoPIDs = makeFilter( self._myname + 'NoPIDs',
                                      StdAllNoPIDsMuons,
                                      "from LoKiTracks.decorators import *",
                                      _cut
                                      )

        self.sel_Hlt1TIS = makeTISTOS( self._myname + 'Hlt1TIS',
                                       self.sel_NoPIDs,
                                       "Hlt1.*Decision%TIS"
                                       )

        self.sel_Hlt2TIS = makeTISTOS( self._myname + 'Hlt2TIS',
                                       self.sel_Hlt1TIS,
                                       "Hlt2.*Decision%TIS"
                                       )

        self.line_SingleTrackTIS = StrippingLine( self._myname + 'Line',
                                                  prescale  = config[ 'SingleTrackTIS_Prescale' ],
                                                  postscale = config[ 'SingleTrackTIS_Postscale' ],
                                                  checkPV   = False,
                                                  selection = self.sel_Hlt2TIS
                                                  )

	self.registerLine( self.line_SingleTrackTIS )


        # SingleTrackTIS

        self.sel_NoPIDsLow = makeFilter( self._myname + 'NoPIDsLow',
                                         StdAllNoPIDsMuons,
                                         "from LoKiTracks.decorators import *",
                                         _cutlow
                                         )

        self.sel_Hlt1TISLow = makeTISTOS( self._myname + 'Hlt1TISLow',
                                          self.sel_NoPIDsLow,
                                          "Hlt1.*Decision%TIS"
                                          )

        self.sel_Hlt2TISLow = makeTISTOS( self._myname + 'Hlt2TISLow',
                                          self.sel_Hlt1TISLow,
                                          "Hlt2.*Decision%TIS"
                                          )

        self.line_SingleTrackTISLow = StrippingLine( self._myname + 'LowLine',
                                                     prescale  = config[ 'SingleTrackTISLow_Prescale' ],
                                                     postscale = config[ 'SingleTrackTIS_Postscale' ],
                                                     checkPV   = False,
                                                     selection = self.sel_Hlt2TISLow
                                                     )

	self.registerLine( self.line_SingleTrackTISLow )


def makeFilter( name, _input, _preambulo, _code ) :

    _filter = FilterDesktop( Preambulo = [ _preambulo ],
                             Code      = _code
                             )

    return Selection ( name,
                       Algorithm          = _filter,
                       RequiredSelections = [ _input ]
                       )


def makeTISTOS( name, _input, _trigger ) :

    from Configurables import TisTosParticleTagger
    _tisTosFilter = TisTosParticleTagger( name + 'Tagger' )
    _tisTosFilter.TisTosSpecs = { _trigger : 0 }
    #_tisTosFilter.ProjectTracksToCalo = False
    #_tisTosFilter.CaloClustForCharged = False
    #_tisTosFilter.CaloClustForNeutral = False
    #_tisTosFilter.TOSFrac = { 4:0.0, 5:0.0 }

    return Selection( name,
                      Algorithm          = _tisTosFilter,
                      RequiredSelections = [ _input ]
                      )
