# Stripping Lines for study of background to Electroweak processes
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani
#
# SingleTrackTIS:    StdAllNoPIDsMuons, pT>20GeV & TTHits & Hlt1xHlt2 TIS (10% PRESCALE)
# SingleTrackTISLow: StdAllNoPIDsMuons, pT>15GeV & TTHits & Hlt1xHlt2 TIS ( 1% PRESCALE)

__all__ = (
  'SingleTrackTISConf',
  'default_config',
)

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StandardParticles import StdAllNoPIDsMuons
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import GeV


default_config = {
    'NAME'        : 'SingleTrackTIS',
    'BUILDERTYPE' : 'SingleTrackTISConf',
    'WGs'         : [ 'QEE'],
    'STREAMS'     : [ 'EW' ],
    'CONFIG'      : { 
        'SingleTrackTIS_Prescale'    : 0.1,
        'SingleTrackTISLow_Prescale' : 0.01,
        'SingleTrackTIS_Postscale'   : 1.00,
        'pT'    : 20. * GeV,
        'pTlow' : 15. * GeV,
    },
}

class SingleTrackTISConf( LineBuilder ) :

    __configuration_keys__ = default_config['CONFIG'].keys()
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )


        # Define the cuts

        _cut    = "(PT>%(pT)s)    & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        _cutlow = "(PT>%(pTlow)s) & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        

        # SingleTrackTIS

        sel_NoPIDs = makeFilter( name + 'NoPIDs',
                                      StdAllNoPIDsMuons,
                                      "from LoKiTracks.decorators import *",
                                      _cut
                                      )

        sel_Hlt1TIS = makeTISTOS( name + 'Hlt1TIS',
                                       sel_NoPIDs,
                                       "Hlt1.*Decision%TIS"
                                       )

        sel_Hlt2TIS = makeTISTOS( name + 'Hlt2TIS',
                                       sel_Hlt1TIS,
                                       "Hlt2.*Decision%TIS"
                                       )

        line_SingleTrackTIS = StrippingLine( name + 'Line',
                                                  prescale  = config[ 'SingleTrackTIS_Prescale' ],
                                                  postscale = config[ 'SingleTrackTIS_Postscale' ],
                                                  RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                                  checkPV   = False,
                                                  selection = sel_Hlt2TIS
                                                  )

        self.registerLine( line_SingleTrackTIS )


        # SingleTrackTIS

        sel_NoPIDsLow = makeFilter( name + 'NoPIDsLow',
                                         StdAllNoPIDsMuons,
                                         "from LoKiTracks.decorators import *",
                                         _cutlow
                                         )

        sel_Hlt1TISLow = makeTISTOS( name + 'Hlt1TISLow',
                                          sel_NoPIDsLow,
                                          "Hlt1.*Decision%TIS"
                                          )

        sel_Hlt2TISLow = makeTISTOS( name + 'Hlt2TISLow',
                                          sel_Hlt1TISLow,
                                          "Hlt2.*Decision%TIS"
                                          )

        line_SingleTrackTISLow = StrippingLine( name + 'LowLine',
                                                     prescale  = config[ 'SingleTrackTISLow_Prescale' ],
                                                     postscale = config[ 'SingleTrackTIS_Postscale' ],
                                                     RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                                     checkPV   = False,
                                                     selection = sel_Hlt2TISLow
                                                     )

        self.registerLine( line_SingleTrackTISLow )


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
