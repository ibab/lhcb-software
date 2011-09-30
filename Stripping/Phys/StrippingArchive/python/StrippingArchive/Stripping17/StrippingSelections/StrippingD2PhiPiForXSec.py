'''
D -> phi pi cross-section lines

Adapted to current stripping framework by P. Spradlin.
'''

__author__ = ['Conor Fitzpatrick', 'Patrick Spradlin']
__date__ = '03/09/2010'
__version__ = '$Revision: 1.4 $'

__all__ = ( 'StrippingD2PhiPiForXSecConf',
            'makePhi2KK',
            'makeD2PhiPi',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV
from LHCbKernel.Configuration import *
#from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons
        
class StrippingD2PhiPiForXSecConf(LineBuilder): # {

    __configuration_keys__ = (   'Daug_TRCHI2DOF_MAX'
                               , 'K_MIPCHI2DV_MIN'
                               , 'Pi_MIPCHI2DV_MIN'
                               , 'K_PIDK_MIN'
                               , 'Phi_AM_MIN'
                               , 'Phi_AM_MAX'
                               , 'D_AM_MIN'
                               , 'D_AM_MAX'
                               , 'D_BPVDIRA_MIN'
                               , 'D_BPVVDCHI2_MIN'
                               , 'D_VCHI2VDOF_MAX'
                               , 'HltFilter'
                               , 'PrescaleD2PhiPi'
                               , 'PostscaleD2PhiPi'
                             )


    def __init__(self, name, config) : # {

        LineBuilder.__init__(self, name, config)

        phi2KK_name = name + 'Phi2KK'
        d2PhiPi_name = name + 'D2PhiPi'

        self.inPions = StdNoPIDsPions
        self.inKaons = StdNoPIDsKaons

        self.selPhi2KK =  makePhi2KK( name = phi2KK_name
                            , inputSel = [ self.inPions, self.inKaons ]
                            , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
                            , Daug_MIPCHI2DV_MIN = config['K_MIPCHI2DV_MIN']
                            , K_PIDK_MIN = config['K_PIDK_MIN']
                            , Comb_AM_MIN = config['Phi_AM_MIN']
                            , Comb_AM_MAX = config['Phi_AM_MAX']
                          )

        self.selD2PhiPi = makeD2PhiPi( name = d2PhiPi_name 
                                , inputSel = [ self.selPhi2KK, self.inPions ]
                                , Pi_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
                                , Pi_MIPCHI2DV_MIN = config['Pi_MIPCHI2DV_MIN']
                                , Comb_AM_MIN = config['D_AM_MIN']
                                , Comb_AM_MAX = config['D_AM_MAX']
                                , D_BPVDIRA_MIN = config['D_BPVDIRA_MIN']
                                , D_BPVVDCHI2_MIN = config['D_BPVVDCHI2_MIN']
                                , D_VCHI2VDOF_MAX = config['D_VCHI2VDOF_MAX']
                          )


        self.line_D2PhiPi = StrippingLine( d2PhiPi_name + 'Line',
                                           HLT = config['HltFilter'],
                                           prescale   = config['PrescaleD2PhiPi'],
                                           postscale = config['PostscaleD2PhiPi'],
                                           algos = [ self.selD2PhiPi ]
                                         )
        self.registerLine(self.line_D2PhiPi)

    # }

# }


def makePhi2KK( name
                , inputSel
                , Daug_TRCHI2DOF_MAX
                , Daug_MIPCHI2DV_MIN
                , K_PIDK_MIN
                , Comb_AM_MIN
                , Comb_AM_MAX
                , decDescriptors = [ "phi(1020) -> K+ K-" ]
               ) : # {

    daugCuts =  "(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" \
                "& ((PIDK-PIDpi) > %(K_PIDK_MIN)s)" \
                "& (MIPCHI2DV(PRIMARY) > %(Daug_MIPCHI2DV_MIN)s)" % locals()

    combCuts =  "(AM < %(Comb_AM_MAX)s)" \
                "& (AM > %(Comb_AM_MIN)s)" % locals()

    phiCuts =   "(ALL)"


    _phi = CombineParticles(
        DecayDescriptors = decDescriptors
        , DaughtersCuts = { "K+" : daugCuts }
        , CombinationCut = combCuts
        , MotherCut = phiCuts 
    )

    return Selection( name,
                      Algorithm = _phi,
                      RequiredSelections = inputSel
                    )

# }


def makeD2PhiPi( name
                 , inputSel
                 , Pi_TRCHI2DOF_MAX
                 , Pi_MIPCHI2DV_MIN
                 , Comb_AM_MIN
                 , Comb_AM_MAX
                 , D_BPVDIRA_MIN
                 , D_BPVVDCHI2_MIN
                 , D_VCHI2VDOF_MAX
                 , decDescriptors = [ "[D_s+ -> pi+ phi(1020)]cc" ]
                ): # {

    phiCuts =  "(ALL)"
    piCuts =   "(TRCHI2DOF < %(Pi_TRCHI2DOF_MAX)s)" \
               "& (MIPCHI2DV(PRIMARY) > %(Pi_MIPCHI2DV_MIN)s)" % locals()

    combCuts = "(AM < %(Comb_AM_MAX)s)" \
               "& (AM > %(Comb_AM_MIN)s)" % locals()

    dCuts =    "(VFASPF(VCHI2/VDOF) < %(D_VCHI2VDOF_MAX)s)" \
               "& (BPVVDCHI2 > %(D_BPVVDCHI2_MIN)s)" \
               "& (BPVDIRA > %(D_BPVDIRA_MIN)s) " % locals()
        

    _dplus = CombineParticles(
        DecayDescriptors = decDescriptors
        , DaughtersCuts = { "pi+" : piCuts, "phi(1020)" :  phiCuts }
        , CombinationCut = combCuts
        , MotherCut = dCuts
    )

    return Selection( name,
                      Algorithm = _dplus,
                      RequiredSelections = inputSel
                    )

# }


default_config = {
                    'Daug_TRCHI2DOF_MAX'        :   10.0
                  , 'K_MIPCHI2DV_MIN'           :    1.0
                  , 'Pi_MIPCHI2DV_MIN'          :    1.0
                  , 'K_PIDK_MIN'                :    5.0
                  , 'Phi_AM_MIN'                : 1000.0 * MeV
                  , 'Phi_AM_MAX'                : 1040.0 * MeV
                  , 'D_AM_MIN'                  : 1770.0 * MeV
                  , 'D_AM_MAX'                  : 2070.0 * MeV
                  , 'D_BPVDIRA_MIN'             :    0.9999
                  , 'D_BPVVDCHI2_MIN'           :   30.0
                  , 'D_VCHI2VDOF_MAX'           :   10.0
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
                  #
                  , 'PrescaleD2PhiPi'           :    1.0
                  , 'PostscaleD2PhiPi'          :    1.0
                 }


