# Stripping Line for High pT gammas, aiming for H-> gammaZ
# QEE Group
#
# Xabier Cid Vidal
#
# Signal:  StdLoosePhotons, pT > 20GeV/c


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePhotons

config_default = { 'Prescale'  : 1.0,
                   'Postscale' : 1.0,
                   'pT'        : 15.,
                   'photonIso' : 25,

                   'PrescaleLoose'  : 0.05,
                   'PostscaleLoose' : 1.0,
                   'pTLoose'        : 7.5,
                   'photonIsoLoose' : 5
                   }



default_name = 'HighPtGamma'

class HighPtGammaConf( LineBuilder ) :

    __configuration_keys__ = ( 'Prescale',
                               'Prescale',
                               'pT',
                               'photonIso',
                               'PrescaleLoose',
                               'PostscaleLoose',
                               'pTLoose',
                               'photonIsoLoose'
                               )
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts
        _cut    = "(PT>%(pT)s*GeV) & (PPINFO(LHCb.ProtoParticle.CaloTrMatch,-1)>%(photonIso)s)" %config

        _cutLoose    = "(PT>%(pTLoose)s*GeV) & (PPINFO(LHCb.ProtoParticle.CaloTrMatch,-1)>%(photonIsoLoose)s)" %config


        # Signal
        self.sel = makeFilter( self._myname + 'HighPtGamma',
                                  StdLoosePhotons,
                                  "from LoKiTracks.decorators import *",
                                  _cut
                                  )

        # Loose line to control effects
        self.selLoose = makeFilter( self._myname + 'HighPtGammaLoose',
                                    StdLoosePhotons,
                                    "from LoKiTracks.decorators import *",
                                    _cutLoose
                                    )



        self.line = StrippingLine( self._myname + 'Line',
                                      prescale  = config[ 'Prescale' ],
                                      postscale = config[ 'Postscale' ],
                                      selection = self.sel
                                      )

        self.lineLoose = StrippingLine( self._myname + 'LooseLine',
                                        prescale  = config[ 'PrescaleLoose' ],
                                        postscale = config[ 'PostscaleLoose' ],
                                        selection = self.selLoose
                                        )

        self.registerLine( self.line)
        self.registerLine( self.lineLoose)





def makeFilter( name, _input, _preambulo, _code ) :

    _filter = FilterDesktop( Preambulo = [ _preambulo ],
                             Code      = _code
                             )

    return Selection ( name,
                       Algorithm          = _filter,
                       RequiredSelections = [ _input ]
                       )
