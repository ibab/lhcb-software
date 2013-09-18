'''
D0/D*+ cross-section lines

Adapted to current stripping framework by P. Spradlin.
'''

__author__ = [ 'Alexandr Kozlinskiy', 'Patrick Spradlin' ]
__date__ = '22/05/2010'
__version__ = '$Revision: 1.9 $'


__all__ = ( 'StrippingDstar2D0Pi_D02KPiForXSecConf',
            'makeD02HH',
            'makeDstar2D0Pi',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV, picosecond
from LHCbKernel.Configuration import *
#from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
import StandardParticles
if hasattr(StandardParticles, "StdAllLoosePions"):
  from StandardParticles import StdAllLoosePions as StdAllLoosePions
else:
  from StandardParticles import StdLoosePions as StdAllLoosePions
#from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons, StdAllLoosePions
from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons
class StrippingDstar2D0Pi_D02KPiForXSecConf(LineBuilder): # {

    __configuration_keys__ = (   'Daug_TRCHI2DOF_MAX'
                               , 'Daug_BPVIPCHI2_MIN'
                               , 'K_PIDK_MIN'
                               , 'Pi_PIDK_MAX'
                               , 'D0_ADAMASS_WIN'
                               , 'D0_ADMASS_WIN'
                               , 'D0_VCHI2VDOF_MAX'
                               , 'D0_BPVLTIME_MIN'
                               , 'D0_BPVLTFITCHI2_MAX'
                               , 'Dstar_AMDiff_MAX'
                               , 'Dstar_VCHI2VDOF_MAX'
                               , 'Dstar_MDiff_MAX'
                               , 'HltFilter'
                               , 'PrescaleD02HH'
                               , 'PrescaleDstar2D0Pi_D02HH'
                               , 'PostscaleD02HH'
                               , 'PostscaleDstar2D0Pi_D02HH'
                             )


    ## Possible parameters and default values copied from the definition
    ##   of StrippingLine
    def _strippingLine ( self,
                          name             ,   # the base name for the Line
                          prescale  = 1.0  ,   # prescale factor
                          ODIN      = None ,   # ODIN predicate
                          L0DU      = None ,   # L0DU predicate
                          HLT       = None ,   # HltDecReports predicate
                          FILTER    = None ,   # 'VOID'-predicate, e.g. Global Event Cut
                          checkPV   = True ,   # Check PV before running algos
                          algos     = None ,   # the list of stripping members
                          selection = None ,
                          postscale = 1.0    ,   # postscale factor
                          MaxCandidates = "Override",   # Maxumum number
                          MaxCombinations = "Override", # Maxumum number
                          HDRLocation = None ) : # other configuration parameters
    # {

        if (prescale > 0) and (postscale > 0) : # {
            line = StrippingLine( name,
                                  prescale        = prescale,
                                  ODIN            = ODIN,
                                  L0DU            = L0DU,
                                  HLT             = HLT,
                                  FILTER          = FILTER,
                                  checkPV         = checkPV,
                                  algos           = algos,
                                  selection       = selection,
                                  postscale       = postscale,
                                  MaxCandidates   = MaxCandidates,
                                  MaxCombinations = MaxCombinations,
                                  HDRLocation     = HDRLocation )

            self.registerLine(line)
            return line
        # }
        else : 
            return False

    # }


    def __init__(self, name, config) : # {

        LineBuilder.__init__(self, name, config)

        d02HH_name = name + 'D02HH'
        dstar_name  = name + 'Dstar2D0Pi_D02HH'

        self.inPions = StdNoPIDsPions
        self.inDstarPions = StdAllLoosePions
        self.inKaons = StdNoPIDsKaons

        self.selD02HH = makeD02HH( d02HH_name
                                   , inputSel = [ self.inPions, self.inKaons ]
                                   , Daug_TRCHI2DOF_MAX  = config['Daug_TRCHI2DOF_MAX']
                                   , Daug_BPVIPCHI2_MIN  = config['Daug_BPVIPCHI2_MIN']
                                   , K_PIDK_MIN          = config['K_PIDK_MIN']
                                   , Pi_PIDK_MAX         = config['Pi_PIDK_MAX']
                                   , D0_ADAMASS_WIN      = config['D0_ADAMASS_WIN']
                                   , D0_ADMASS_WIN       = config['D0_ADMASS_WIN']
                                   , D0_VCHI2VDOF_MAX    = config['D0_VCHI2VDOF_MAX']
                                   , D0_BPVLTIME_MIN     = config['D0_BPVLTIME_MIN']
                                   , D0_BPVLTFITCHI2_MAX = config['D0_BPVLTFITCHI2_MAX']
                                 )


        self.line_D02HH = self._strippingLine( name = d02HH_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleD02HH'],
                                         postscale = config['PostscaleD02HH'],
                                         selection = self.selD02HH
                                       )


        self.selDstar2D0Pi_D02HH = makeDstar2D0Pi( dstar_name
                    , inputSel = [ self.inDstarPions, self.selD02HH ]
                    , Daug_TRCHI2DOF_MAX  = config['Daug_TRCHI2DOF_MAX']
                    , Dstar_AMDiff_MAX    = config['Dstar_AMDiff_MAX']
                    , Dstar_VCHI2VDOF_MAX = config['Dstar_VCHI2VDOF_MAX']
                    , Dstar_MDiff_MAX     = config['Dstar_MDiff_MAX']
        )

        self.line_Dstar2D0Pi_D02HH = self._strippingLine( name = dstar_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale   = config['PrescaleDstar2D0Pi_D02HH'],
                                         postscale = config['PostscaleDstar2D0Pi_D02HH'],
                                         selection = self.selDstar2D0Pi_D02HH
                                        )


    # }

# }



def makeD02HH( name
               , inputSel
               , Daug_TRCHI2DOF_MAX
               , Daug_BPVIPCHI2_MIN
               , K_PIDK_MIN
               , Pi_PIDK_MAX
               , D0_ADAMASS_WIN
               , D0_ADMASS_WIN
               , D0_VCHI2VDOF_MAX
               , D0_BPVLTIME_MIN
               , D0_BPVLTFITCHI2_MAX
               , decDescriptors = [ "D0 -> pi+ pi-",
                                    "D0 -> K- pi+",
                                    "D0 -> K+ pi-",
                                    "D0 -> K+ K-" ]
             ) : # {

    daugCuts = "(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" \
               "& (BPVIPCHI2() > %(Daug_BPVIPCHI2_MIN)s)" % locals()
    kCuts =    "(PIDK - PIDpi > %(K_PIDK_MIN)s)" % locals()
    piCuts =   "(PIDK - PIDpi < %(Pi_PIDK_MAX)s)" % locals()

    combCuts = "(ADAMASS('D0') < %(D0_ADAMASS_WIN)s)" % locals()

    d0Cuts = "(ADMASS('D0') < %(D0_ADMASS_WIN)s)" \
             "& (VFASPF(VCHI2/VDOF) < %(D0_VCHI2VDOF_MAX)s)" \
             "& (BPVLTIME() > %(D0_BPVLTIME_MIN)s)" \
             "& (BPVLTFITCHI2() < %(D0_BPVLTFITCHI2_MAX)s)" % locals()

    _D0 = CombineParticles(
        DecayDescriptors = decDescriptors
        , DaughtersCuts = { "pi+" : daugCuts + '&' + piCuts, 
                            "K+"  : daugCuts + '&' + kCuts }
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

    piCuts = "(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" % locals()

    combCuts = "((AM - AM1) < %(Dstar_AMDiff_MAX)s)" % locals()

    dstarCuts = "(VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2VDOF_MAX)s)" \
                "& ((M - M1) < %(Dstar_MDiff_MAX)s)" % locals()

    _Dstar = CombineParticles(
        DecayDescriptors = decDescriptors
        , DaughtersCuts = { "pi+" : piCuts }
        , CombinationCut = combCuts
        , MotherCut = dstarCuts
    )

    return Selection( name,
                      Algorithm = _Dstar,
                      RequiredSelections = inputSel
                    )

# }




default_config = {  'Daug_TRCHI2DOF_MAX'        :  10.0
                  , 'Daug_BPVIPCHI2_MIN'        :   6.0
                  , 'K_PIDK_MIN'                :   0.0
                  , 'Pi_PIDK_MAX'               :   0.0
                  , 'D0_ADAMASS_WIN'            :  80.0 * MeV
                  , 'D0_ADMASS_WIN'             :  75.0 * MeV
                  , 'D0_VCHI2VDOF_MAX'          :  25.0
                  , 'D0_BPVLTIME_MIN'           :   0.2 * picosecond
                  , 'D0_BPVLTFITCHI2_MAX'       : 100.0
                  , 'Dstar_AMDiff_MAX'          : 160.0 * MeV
                  , 'Dstar_VCHI2VDOF_MAX'       : 100.0
                  , 'Dstar_MDiff_MAX'           : 155.0 * MeV
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
                  #
                  , 'PrescaleD02HH'             :   1.0
                  , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
                  , 'PostscaleD02HH'            :   1.0
                  , 'PostscaleDstar2D0Pi_D02HH' :   1.0
                 }


