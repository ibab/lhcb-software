'''
D0/D*+ cross-section lines

Adapted to current stripping framework by P. Spradlin.
'''

__author__ = ['Marco Gersabeck', 'Alexandr Kozlinskiy', 'Patrick Spradlin']
__date__ = '03/09/2010'
__version__ = '$Revision: 1.4 $'

__all__ = ( 'StrippingD02HHForXSecConf',
            'makeD02HH',
            'makeDstar2D0Pi',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV, mm, mrad
from LHCbKernel.Configuration import *
#from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
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

class StrippingD02HHForXSecConf(LineBuilder): # {

    __configuration_keys__ = (   'Daug_PT_MIN'
                               , 'Daug_BPVIPCHI2_MIN'
                               , 'K_PIDK_MIN'
                               , 'Pi_PIDK_MAX'
                               , 'D0_ADAMASS_WIN'
                               , 'D0_VCHI2VDOF_MAX'
                               , 'D0_acosBPVDIRA_MAX'
                               , 'D0_PVDispCut'
                               , 'Dstar_AMDiff_MAX'
                               , 'Dstar_VCHI2VDOF_MAX'
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
                          prescale  = 1  ,   # prescale factor
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

        self.inPions = StdAllNoPIDsPions
        self.inDstarPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons

        self.selD02HH = makeD02HH( d02HH_name
                                   , inputSel = [ self.inPions, self.inKaons ]
                                   , Daug_PT_MIN        = config['Daug_PT_MIN']
                                   , Daug_BPVIPCHI2_MIN = config['Daug_BPVIPCHI2_MIN']
                                   , K_PIDK_MIN         = config['K_PIDK_MIN']
                                   , Pi_PIDK_MAX        = config['Pi_PIDK_MAX']
                                   , D0_ADAMASS_WIN     = config['D0_ADAMASS_WIN']
                                   , D0_VCHI2VDOF_MAX   = config['D0_VCHI2VDOF_MAX']
                                   , D0_acosBPVDIRA_MAX = config['D0_acosBPVDIRA_MAX']
                                   , D0_PVDispCut       = config['D0_PVDispCut']
                                 )


        self.line_D02HH = self._strippingLine( name = d02HH_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleD02HH'],
                                         postscale = config['PostscaleD02HH'],
                                         selection = self.selD02HH
                                       )


        self.selDstar2D0Pi_D02HH = makeDstar2D0Pi( dstar_name
                    , inputSel = [ self.inDstarPions, self.selD02HH ]
                    , Dstar_AMDiff_MAX    = config['Dstar_AMDiff_MAX']
                    , Dstar_VCHI2VDOF_MAX = config['Dstar_VCHI2VDOF_MAX']
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
               , Daug_PT_MIN
               , Daug_BPVIPCHI2_MIN
               , K_PIDK_MIN
               , Pi_PIDK_MAX
               , D0_ADAMASS_WIN
               , D0_VCHI2VDOF_MAX
               , D0_acosBPVDIRA_MAX
               , D0_PVDispCut
               , decDescriptors = [ "D0 -> pi+ pi-",
                                    "D0 -> K- pi+",
                                    "D0 -> K+ pi-",
                                    "D0 -> K+ K-" ]
             ) : # {

    ## Construct a preambulo to simplify some calculations.
    lclPreambulo = [
      "from math import cos"
      , "bpvdirathresh = cos(%(D0_acosBPVDIRA_MAX)s)" % locals()
      , "pidFiducialPMin = 3.0 * GeV"
      , "pidFiducialPMax = 100.0 * GeV"
    ]

    daugCuts = "(PT > %(Daug_PT_MIN)s)" \
               "& (BPVIPCHI2() > %(Daug_BPVIPCHI2_MIN)s)" % locals()

    pidFiducialCuts = "(HASRICH)" \
                      "& (in_range(pidFiducialPMin, P, pidFiducialPMax))" \
                      "& (in_range(2.0, ETA, 5.0))"

    kaonPIDCut = pidFiducialCuts + "& (PIDK-PIDpi > %(K_PIDK_MIN)s)" % locals()
    pionPIDCut = pidFiducialCuts + "& (PIDK-PIDpi < %(Pi_PIDK_MAX)s)" % locals()

    combCuts = "(ADAMASS('D0') < %(D0_ADAMASS_WIN)s)" % locals()

    d0Cuts = "(VFASPF(VCHI2/VDOF) < %(D0_VCHI2VDOF_MAX)s)" \
             "& (%(D0_PVDispCut)s)" \
             "& (BPVDIRA > bpvdirathresh)" % locals()


    _D0 = CombineParticles(
        DecayDescriptors = decDescriptors
        , Preambulo = lclPreambulo
        , DaughtersCuts = { "pi+" : daugCuts + '&' + pionPIDCut, 
                            "K+"  : daugCuts + '&' + kaonPIDCut }
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
                    , Dstar_AMDiff_MAX
                    , Dstar_VCHI2VDOF_MAX
                    , decDescriptors= [ "D*(2010)+ -> D0 pi+",
                                        "D*(2010)- -> D0 pi-" ]
                  ) : # {

    daugCuts = "(ALL)" % locals()

    combCuts = "((AM - AM1) < %(Dstar_AMDiff_MAX)s)" % locals()

    dstarCuts = "(VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2VDOF_MAX)s)" % locals()

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




default_config = {
                    'Daug_PT_MIN'               : 250.0 * MeV
                  , 'Daug_BPVIPCHI2_MIN'        :   4.0
                  , 'K_PIDK_MIN'                :   0.0
                  , 'Pi_PIDK_MAX'               :   3.0
                  , 'D0_ADAMASS_WIN'            :  80.0*MeV
                  , 'D0_VCHI2VDOF_MAX'          :  25.0
                  , 'D0_acosBPVDIRA_MAX'        :  35.0 * mrad
                  , 'D0_PVDispCut'              : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'Dstar_AMDiff_MAX'          : 160.0*MeV
                  , 'Dstar_VCHI2VDOF_MAX'       : 100.0
                  #
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
                  #, 'HltFilter'          : none
                  #
                  , 'PrescaleD02HH'             :   0.1
                  , 'PrescaleDstar2D0Pi_D02HH'  :   0.1
                  , 'PostscaleD02HH'            :   1.0
                  , 'PostscaleDstar2D0Pi_D02HH' :   1.0
                 }


