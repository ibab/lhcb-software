'''
D0/D*+ cross-section lines.  Geometric variable.

Adapted to current stripping framework by P. Spradlin.
'''

__author__ = ['Markward Britsch', 'Patrick Spradlin']
__date__ = '21/09/2010'
__version__ = '$Revision: 1.0 $'

__all__ = ( 'StrippingD02KPiGeoForXSecConf',
            'makeD02HH',
            'makeDstar2D0Pi',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV, mm
from LHCbKernel.Configuration import *
#from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
#from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons
import StandardParticles
if hasattr(StandardParticles, "StdAllLoosePions"):
  from StandardParticles import StdAllLoosePions as StdAllLoosePions
else:
  from StandardParticles import StdNoPIDsPions as StdAllLoosePions
#from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons, StdAllLoosePions
from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons

class StrippingD02KPiGeoForXSecConf(LineBuilder): # {

    __configuration_keys__ = (   'D0_ADAMASS_WIN'
                               , 'D0_AMAXDOCA_MAX'
                               , 'D0_ADMASS_WIN'
                               , 'D0_NU_2PT_MIN'
                               , 'D0_BPVVDSIGN_MIN'
                               , 'SPi_TRCHI2DOF_MAX'
                               , 'Dstar_AMDiff_MAX'
                               , 'Dstar_VCHI2VDOF_MAX'
                               , 'Dstar_MDiff_MAX'
                               , 'HltFilter'
                               , 'PrescaleD02HH'
                               , 'PrescaleDstar2D0Pi_D02HH'
                               , 'PostscaleD02HH'
                               , 'PostscaleDstar2D0Pi_D02HH'
                             )



    def __init__(self, name, config) : # {

        LineBuilder.__init__(self, name, config)

        d02HH_name = name + 'D02HH'
        dstar_name  = name + 'Dstar2D0Pi_D02HH'

        self.inPions = StdNoPIDsPions
        self.inDstarPions = StdAllLoosePions
        self.inKaons = StdNoPIDsKaons

        self.selD02HH = makeD02HH( d02HH_name
                                   , inputSel = [ self.inPions, self.inKaons ]
                                   , Comb_ADAMASS_WIN = config['D0_ADAMASS_WIN']
                                   , Comb_AMAXDOCA_MAX = config['D0_AMAXDOCA_MAX']
                                   , D0_ADMASS_WIN = config['D0_ADMASS_WIN']
                                   , D0_NU_2PT_MIN = config['D0_NU_2PT_MIN']
                                   , D0_BPVVDSIGN_MIN = config['D0_BPVVDSIGN_MIN']
                                 )


        self.line_D02HH = StrippingLine( d02HH_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleD02HH'],
                                         postscale = config['PostscaleD02HH'],
                                         algos = [ self.selD02HH ]
                                       )
        self.registerLine(self.line_D02HH)


        ## Original lines did not include D*+ reconstruction from the
        ##   D0 candidates.  This is included for completeness.
        self.selDstar2D0Pi_D02HH = makeDstar2D0Pi( dstar_name
                    , inputSel = [ self.inDstarPions, self.selD02HH ]
                    , Daug_TRCHI2DOF_MAX  = config['SPi_TRCHI2DOF_MAX']
                    , Dstar_AMDiff_MAX    = config['Dstar_AMDiff_MAX']
                    , Dstar_VCHI2VDOF_MAX = config['Dstar_VCHI2VDOF_MAX']
                    , Dstar_MDiff_MAX     = config['Dstar_MDiff_MAX']
        )

        self.line_Dstar2D0Pi_D02HH = StrippingLine( dstar_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale   = config['PrescaleDstar2D0Pi_D02HH'],
                                         postscale = config['PostscaleDstar2D0Pi_D02HH'],
                                         algos = [ self.selDstar2D0Pi_D02HH ]
                                        )
        self.registerLine(self.line_Dstar2D0Pi_D02HH)

    # }

# }


## Remainder stuff

def makeD02HH( name
               , inputSel
               , Comb_ADAMASS_WIN
               , Comb_AMAXDOCA_MAX
               , D0_ADMASS_WIN
               , D0_NU_2PT_MIN
               , D0_BPVVDSIGN_MIN
               , decDescriptors = [ "D0 -> pi+ pi-",
                                    "D0 -> K- pi+",
                                    "D0 -> K+ pi-",
                                    "D0 -> K+ K-" ]
             ) : # {

    ## Preambulo to define the geometric variables.
    ## The MIPDV of the reconstructed parents prevents the geometric variable
    ##   from being applied in the CombinationCut.
    preambuloNuDef = [ "PFUNA = LoKi.Particles.PFunA",
                       "NU_2 = log( CHILD(MIPDV(PRIMARY), 1) " \
                         "* CHILD(MIPDV(PRIMARY), 2) " \
                         "/ (MIPDV(PRIMARY)**2 + PFUNA(AMAXDOCA(''))**2 ) )",
                       "NU_2PT = log(NU_2 * CHILD(PT, 1) * CHILD(PT, 2) / MeV**2)"
    ]

    daugCuts = "(ISLONG)"

    combCuts = "(ADAMASS('D0') < %(Comb_ADAMASS_WIN)s)" \
               "& (AMAXDOCA('') < %(Comb_AMAXDOCA_MAX)s)" % locals()

    d0Cuts = "(ADMASS('D0') < %(D0_ADMASS_WIN)s)" \
             "& (NU_2 > 0) & (NU_2PT > %(D0_NU_2PT_MIN)s)" \
             "& (BPVVDSIGN > %(D0_BPVVDSIGN_MIN)s)" % locals()

    _D0 = CombineParticles(
        DecayDescriptors = decDescriptors
        , Preambulo      = preambuloNuDef
        , DaughtersCuts = { "pi+" : daugCuts, "K+" : daugCuts }
        , CombinationCut = combCuts
        , MotherCut = d0Cuts
    )


    return Selection( name,
                      Algorithm = _D0,
                      RequiredSelections = inputSel
                    )

# }



def makeDstar2D0Pi( name
                    , inputSel
                    , Daug_TRCHI2DOF_MAX
                    , Dstar_AMDiff_MAX
                    , Dstar_VCHI2VDOF_MAX
                    , Dstar_MDiff_MAX
                    , decDescriptors= [ "D*(2010)+ -> D0 pi+",
                                        "D*(2010)- -> D0 pi-" ]
                  ) : # {

    daugCuts = "(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" % locals()

    combCuts = "((AM - AM1) < %(Dstar_AMDiff_MAX)s)" % locals()

    dstarCuts = "(VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2VDOF_MAX)s)" \
                "& ((M - M1) < %(Dstar_MDiff_MAX)s)" % locals()

    _Dstar = CombineParticles(
        DecayDescriptors = decDescriptors
        , DaughtersCuts = { "pi+" : daugCuts }
        , CombinationCut = combCuts
        , MotherCut = dstarCuts
    )

    return Selection( name,
                      Algorithm = _Dstar,
                      RequiredSelections = inputSel
                    )

# }



default_config = {  'D0_ADAMASS_WIN'      : 250.0 * MeV
                  , 'D0_AMAXDOCA_MAX'     :  10.0 * mm
                  , 'D0_ADMASS_WIN'       : 125.0 * MeV
                  , 'D0_NU_2PT_MIN'       :  14.0
                  , 'D0_BPVVDSIGN_MIN'    :   1.0 * mm
                  , 'SPi_TRCHI2DOF_MAX'   :   9.0
                  , 'Dstar_AMDiff_MAX'    : 160.0 * MeV
                  , 'Dstar_VCHI2VDOF_MAX' : 100.0
                  , 'Dstar_MDiff_MAX'     : 155.0 * MeV
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
                  #
                  , 'PrescaleD02HH'             :   1.0
                  , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
                  , 'PostscaleD02HH'            :   1.0
                  , 'PostscaleDstar2D0Pi_D02HH' :   1.0
                 }

