#Stripping Lines for Study of Background to Electroweak Processes
#Electroweak Group (Convener: Tara Shears)
#Line designed by Simone Bifani and William Barter

# Accepts events with at least one noPID track which is TIS wrt Hlt1 and Hlt2 lines
# Low and high Pt lines implemented with different prescales

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsMuons

confdict_SingleTrackTIS = { 'LowPtPrescale'     : .001
                            , 'LowPtPostscale'  : 1.0
                            , 'HighPtPrescale'  : .1
                            , 'HighPtPostscale' : 1.0
                            , 'LowPtCut'  : 5.
                            , 'HighPtCut' : 15.
                            }

name = "SingleTrackTIS"

class SingleTrackTISConf(LineBuilder) :

    __configuration_keys__ = ( 'LowPtPrescale'
                               , 'LowPtPostscale'
                               , 'HighPtPrescale'
                               , 'HighPtPostscale'
                               , 'LowPtCut'
                               , 'HighPtCut'
                               )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        # Requiring at least one TT hits removes ghost-tracks
        _LowPtCutTight  = "(PT>%(LowPtCut)s*GeV) & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        _HighPtCutTight = "(PT>%(HighPtCut)s*GeV) & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        
        self._myname = name


        # Low Pt line
        self.selLowPtNoPIDs = makeFilter( self._myname + "LowPtNoPIDs"
                                          , StdAllNoPIDsMuons
                                          , "from LoKiTracks.decorators import *"
                                          , _LowPtCutTight
                                          )
        self.selLowPtHlt1TIS = makeTISTOS( self._myname + "LowPtHlt1TIS"
                                           , self.selLowPtNoPIDs
                                           , "Hlt1.*Decision%TIS"
                                           )
        self.selLowPtHlt2TIS = makeTISTOS( self._myname + "LowPtHlt2TIS"
                                           , self.selLowPtHlt1TIS
                                           , "Hlt2.*Decision%TIS"
                                           )
        self.lineSingleTrackTISLowPt = StrippingLine( self._myname + "LowPt"
                                                      , prescale = config[ "LowPtPrescale" ]
                                                      , postscale = config[ "LowPtPostscale" ]
                                                      , checkPV = False
                                                      , selection = self.selLowPtHlt2TIS
                                                      )
	self.registerLine(self.lineSingleTrackTISLowPt)	


        # High Pt line
        self.selHighPtNoPIDs = makeFilter( self._myname + "HighPtNoPIDs"
                                           , StdAllNoPIDsMuons
                                           , "from LoKiTracks.decorators import *"
                                           , _HighPtCutTight
                                           )
        self.selHighPtHlt1TIS = makeTISTOS( self._myname + "HighPtHlt1TIS"
                                            , self.selHighPtNoPIDs
                                            , "Hlt1.*Decision%TIS"
                                            )
        self.selHighPtHlt2TIS = makeTISTOS( self._myname + "HighPtHlt2TIS"
                                            , self.selHighPtHlt1TIS
                                            , "Hlt2.*Decision%TIS"
                                            )
        self.lineSingleTrackTISHighPt = StrippingLine( self._myname + "HighPt"
                                                       , prescale = config[ "HighPtPrescale" ]
                                                       , postscale = config[ "HighPtPostscale" ]
                                                       , checkPV = False
                                                       , selection = self.selHighPtHlt2TIS
                                                       )
	self.registerLine(self.lineSingleTrackTISHighPt)


def makeFilter( name, _input, _preambulo, _code ) :
    _filter = FilterDesktop( Preambulo = [ _preambulo ]
                             , Code = _code
                             )
    return Selection ( name
                       , Algorithm = _filter
                       , RequiredSelections = [ _input ]
                       )

def makeTISTOS( name, _input, _trigger ) :
    from Configurables import TisTosParticleTagger
    _tisTosFilter = TisTosParticleTagger( name + "Tagger" )
    _tisTosFilter.TisTosSpecs = { _trigger : 0 }
    #_tisTosFilter.ProjectTracksToCalo = False
    #_tisTosFilter.CaloClustForCharged = False
    #_tisTosFilter.CaloClustForNeutral = False
    #_tisTosFilter.TOSFrac = { 4:0.0, 5:0.0 }
    return Selection( name
                      , Algorithm = _tisTosFilter
                      , RequiredSelections = [ _input ]
                      )
