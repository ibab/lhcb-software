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
from GaudiKernel.SystemOfUnits import MeV, mrad
from LHCbKernel.Configuration import *
#from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
import StandardParticles
if hasattr(StandardParticles, "StdAllNoPIDsPions"):
  from StandardParticles import StdAllNoPIDsPions
else:
  from StandardParticles import StdNoPIDsPions as StdAllNoPIDsPions
if hasattr(StandardParticles, "StdAllNoPIDsKaons"):
  from StandardParticles import StdAllNoPIDsKaons
else:
  from StandardParticles import StdNoPIDsKaons as StdAllNoPIDsKaons
        
class StrippingD2PhiPiForXSecConf(LineBuilder): # {

    __configuration_keys__ = ( 'K_BPVIPCHI2_MIN'
                               , 'Pi_BPVIPCHI2_MIN'
                               , 'K_PIDK_MIN'
                               , 'Phi_M_MIN'
                               , 'Phi_M_MAX'
                               , 'D_AM_MIN'
                               , 'D_AM_MAX'
                               , 'D_VCHI2VDOF_MAX'
                               , 'D_acosBPVDIRA_MAX'
                               , 'D_PVDispCut'
                               , 'HltFilter'
                               , 'PrescaleD2PhiPi'
                               , 'PostscaleD2PhiPi'
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

        phi2KK_name = name + 'Phi2KK'
        d2PhiPi_name = name + 'D2PhiPi'

        self.inPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons

        self.selPhi2KK =  makePhi2KK( name = phi2KK_name
                            , inputSel = [ self.inPions, self.inKaons ]
                            , Daug_BPVIPCHI2_MIN = config['K_BPVIPCHI2_MIN']
                            , K_PIDK_MIN = config['K_PIDK_MIN']
                            , Comb_M_MIN = config['Phi_M_MIN']
                            , Comb_M_MAX = config['Phi_M_MAX']
                          )

        self.selD2PhiPi = makeD2PhiPi( name = d2PhiPi_name 
                                , inputSel = [ self.selPhi2KK, self.inPions ]
                                , Pi_BPVIPCHI2_MIN = config['Pi_BPVIPCHI2_MIN']
                                , Comb_AM_MIN = config['D_AM_MIN']
                                , Comb_AM_MAX = config['D_AM_MAX']
                                , D_acosBPVDIRA_MAX = config['D_acosBPVDIRA_MAX']
                                , D_PVDispCut = config['D_PVDispCut']
                                , D_VCHI2VDOF_MAX = config['D_VCHI2VDOF_MAX']
                          )


        self.line_D2PhiPi = self._strippingLine( name = d2PhiPi_name + 'Line',
                                           HLT = config['HltFilter'],
                                           prescale   = config['PrescaleD2PhiPi'],
                                           postscale = config['PostscaleD2PhiPi'],
                                           selection = self.selD2PhiPi
                                         )
    # }

# }


def makePhi2KK( name
                , inputSel
                , Daug_BPVIPCHI2_MIN
                , K_PIDK_MIN
                , Comb_M_MIN
                , Comb_M_MAX
                , decDescriptors = [ "phi(1020) -> K+ K-" ]
               ) : # {

    ## Construct a preambulo to simplify some calculations.
    lclPreambulo = [
      "pidFiducialPMin = 3.0 * GeV"
      , "pidFiducialPMax = 100.0 * GeV"
    ]

    daugCuts = "(HASRICH)" \
               "& (in_range(pidFiducialPMin, P, pidFiducialPMax))" \
               "& (in_range(2.0, ETA, 5.0))" \
               "& ((PIDK-PIDpi) > %(K_PIDK_MIN)s)" \
               "& (BPVIPCHI2() > %(Daug_BPVIPCHI2_MIN)s)" % locals()

    combCuts =  "(AALL)"

    phiCuts =   "(in_range(%(Comb_M_MIN)s, M, %(Comb_M_MAX)s))" % locals()


    _phi = CombineParticles(
        DecayDescriptors = decDescriptors
        , Preambulo = lclPreambulo
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
                 , Pi_BPVIPCHI2_MIN
                 , Comb_AM_MIN
                 , Comb_AM_MAX
                 , D_acosBPVDIRA_MAX
                 , D_PVDispCut
                 , D_VCHI2VDOF_MAX
                 , decDescriptors = [ "[D_s+ -> pi+ phi(1020)]cc" ]
                ): # {

    ## Construct a preambulo to simplify some calculations.
    lclPreambulo = [
      "from math import cos"
      , "bpvdirathresh = cos(%(D_acosBPVDIRA_MAX)s)" % locals()
    ]

    phiCuts =  "(ALL)"
    piCuts =   "(BPVIPCHI2() > %(Pi_BPVIPCHI2_MIN)s)" % locals()

    combCuts = "(in_range(%(Comb_AM_MIN)s, AM, %(Comb_AM_MAX)s))" % locals()

    dCuts =    "(VFASPF(VCHI2/VDOF) < %(D_VCHI2VDOF_MAX)s)" \
               "& (%(D_PVDispCut)s)" \
               "& (BPVDIRA > bpvdirathresh) " % locals()

    _dplus = CombineParticles(
        DecayDescriptors = decDescriptors
        , Preambulo = lclPreambulo
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
                    'K_BPVIPCHI2_MIN'           :    1.0
                  , 'Pi_BPVIPCHI2_MIN'          :    1.0
                  , 'K_PIDK_MIN'                :    0.0
                  , 'Phi_M_MIN'                 : 1000.0 * MeV
                  , 'Phi_M_MAX'                 : 1040.0 * MeV
                  , 'D_AM_MIN'                  : 1770.0 * MeV
                  , 'D_AM_MAX'                  : 2070.0 * MeV
                  , 'D_VCHI2VDOF_MAX'           :   25.0
                  , 'D_acosBPVDIRA_MAX'         :   35.0 * mrad
                  , 'D_PVDispCut'               : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'HltFilter'                 : None
                  #
                  , 'PrescaleD2PhiPi'           :    1.0
                  , 'PostscaleD2PhiPi'          :    1.0
                 }


