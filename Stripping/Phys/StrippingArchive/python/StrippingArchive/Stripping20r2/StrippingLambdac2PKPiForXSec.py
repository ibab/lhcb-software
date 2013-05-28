'''
Lambdac cross-section lines

Adapted to current stripping framework by P. Spradlin.
'''

__author__ = ['Francesca Dordei', 'Francesco Dettori', 'Patrick Spradlin']
__date__ = '2010/07/15'
__version__ = '$Revision: 1.3 $'

__all__ = ( 'StrippingLambdac2PKPiForXSecConf',
            'makeLambdac2PKPi',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV, mm, ns, mrad
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
if hasattr(StandardParticles, "StdAllNoPIDsProtons"):
  from StandardParticles import StdAllNoPIDsProtons
else:
  from StandardParticles import StdNoPIDsProtons as StdAllNoPIDsProtons
from Configurables import TisTosParticleTagger

class StrippingLambdac2PKPiForXSecConf(LineBuilder): # {

    __configuration_keys__ = (   'Daug_All_PT_MIN'
                               , 'Daug_2of3_PT_MIN'
                               , 'Daug_1of3_PT_MIN'
                               , 'Daug_All_BPVIPCHI2_MIN'
                               , 'Daug_2of3_BPVIPCHI2_MIN'
                               , 'Daug_1of3_BPVIPCHI2_MIN'
                               , 'Proton_PIDpPIDpi_MIN'
                               , 'Proton_PIDpPIDK_MIN'
                               , 'K_PIDK_MIN'
                               , 'Pi_PIDK_MAX'
                               , 'Comb_ADAMASS_WIN'
                               , 'Comb_ADOCAMAX_MAX'
                               , 'Lambdac_VCHI2VDOF_MAX'
                               , 'Lambdac_acosBPVDIRA_MAX'
                               , 'Lambdac_PVDispCut'
                               , 'HltFilter'
                               , 'PrescaleLambdac2PKPi'
                               , 'PostscaleLambdac2PKPi'
                               , 'PrescaleLambdac2PKK'
                               , 'PostscaleLambdac2PKK'
                               , 'PrescaleLambdac2PPiPi'
                               , 'PostscaleLambdac2PPiPi'
                               , 'PrescaleLambdac2PPiKWS'
                               , 'PostscaleLambdac2PPiKWS'
                             )


    ## I think that the purpose for which this wrapper was written has been
    ##   properly incorporated into the stripping configuration.
    ## At the very least, i need to check that the argument definition for
    ##    StrippingLine is the same.
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


        lambdac_name = name + 'Lambdac2PKPi'
        lambdac_pKK_name = name + 'Lambdac2PKK'
        lambdac_ppipi_name = name + 'Lambdac2PPiPi'
        lambdac_ppiK_name = name + 'Lambdac2PPiKWS'

        self.inPions   = StdAllNoPIDsPions
        self.inKaons   = StdAllNoPIDsKaons
        self.inProtons = StdAllNoPIDsProtons


        self.combLambdac2PKPi = makeLambdac2PKPi( name = lambdac_name 
               , inputSel = [ self.inPions, self.inKaons, self.inProtons ]
               , Daug_All_PT_MIN         = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN        = config['Daug_2of3_PT_MIN']
               , Daug_1of3_PT_MIN        = config['Daug_1of3_PT_MIN']
               , Daug_All_BPVIPCHI2_MIN  = config['Daug_All_BPVIPCHI2_MIN']
               , Daug_2of3_BPVIPCHI2_MIN = config['Daug_2of3_BPVIPCHI2_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , Proton_PIDpPIDpi_MIN    = config['Proton_PIDpPIDpi_MIN']
               , Proton_PIDpPIDK_MIN     = config['Proton_PIDpPIDK_MIN']
               , K_PIDK_MIN              = config['K_PIDK_MIN']
               , Pi_PIDK_MAX             = config['Pi_PIDK_MAX']
               , Comb_ADAMASS_WIN        = config['Comb_ADAMASS_WIN']
               , Comb_ADOCAMAX_MAX       = config['Comb_ADOCAMAX_MAX']
               , Lambdac_VCHI2VDOF_MAX   = config['Lambdac_VCHI2VDOF_MAX']
               , Lambdac_acosBPVDIRA_MAX = config['Lambdac_acosBPVDIRA_MAX']
               , Lambdac_PVDispCut       = config['Lambdac_PVDispCut']
             )

        self.combLambdac2PKK = makeLambdac2PKPi( name = lambdac_pKK_name 
               , inputSel = [ self.inKaons, self.inProtons ]
               , Daug_All_PT_MIN         = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN        = config['Daug_2of3_PT_MIN']
               , Daug_1of3_PT_MIN        = config['Daug_1of3_PT_MIN']
               , Daug_All_BPVIPCHI2_MIN  = config['Daug_All_BPVIPCHI2_MIN']
               , Daug_2of3_BPVIPCHI2_MIN = config['Daug_2of3_BPVIPCHI2_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , Proton_PIDpPIDpi_MIN    = config['Proton_PIDpPIDpi_MIN']
               , Proton_PIDpPIDK_MIN     = config['Proton_PIDpPIDK_MIN']
               , K_PIDK_MIN              = config['K_PIDK_MIN']
               , Pi_PIDK_MAX             = config['Pi_PIDK_MAX']
               , Comb_ADAMASS_WIN        = config['Comb_ADAMASS_WIN']
               , Comb_ADOCAMAX_MAX       = config['Comb_ADOCAMAX_MAX']
               , Lambdac_VCHI2VDOF_MAX   = config['Lambdac_VCHI2VDOF_MAX']
               , Lambdac_acosBPVDIRA_MAX = config['Lambdac_acosBPVDIRA_MAX']
               , Lambdac_PVDispCut       = config['Lambdac_PVDispCut']
               , decDescriptors = [ "[Lambda_c+ -> p+ K- K+]cc" ]
             )

        self.combLambdac2PPiPi = makeLambdac2PKPi( name = lambdac_ppipi_name
               , inputSel = [ self.inPions, self.inProtons ]
               , Daug_All_PT_MIN         = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN        = config['Daug_2of3_PT_MIN']
               , Daug_1of3_PT_MIN        = config['Daug_1of3_PT_MIN']
               , Daug_All_BPVIPCHI2_MIN  = config['Daug_All_BPVIPCHI2_MIN']
               , Daug_2of3_BPVIPCHI2_MIN = config['Daug_2of3_BPVIPCHI2_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , Proton_PIDpPIDpi_MIN    = config['Proton_PIDpPIDpi_MIN']
               , Proton_PIDpPIDK_MIN     = config['Proton_PIDpPIDK_MIN']
               , K_PIDK_MIN              = config['K_PIDK_MIN']
               , Pi_PIDK_MAX             = config['Pi_PIDK_MAX']
               , Comb_ADAMASS_WIN        = config['Comb_ADAMASS_WIN']
               , Comb_ADOCAMAX_MAX       = config['Comb_ADOCAMAX_MAX']
               , Lambdac_VCHI2VDOF_MAX   = config['Lambdac_VCHI2VDOF_MAX']
               , Lambdac_acosBPVDIRA_MAX = config['Lambdac_acosBPVDIRA_MAX']
               , Lambdac_PVDispCut       = config['Lambdac_PVDispCut']
               , decDescriptors = [ "[Lambda_c+ -> p+ pi- pi+]cc" ]
             )

        self.combLambdac2PPiKWS = makeLambdac2PKPi( name = lambdac_ppiK_name 
               , inputSel = [ self.inPions, self.inKaons, self.inProtons ]
               , Daug_All_PT_MIN         = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN        = config['Daug_2of3_PT_MIN']
               , Daug_1of3_PT_MIN        = config['Daug_1of3_PT_MIN']
               , Daug_All_BPVIPCHI2_MIN  = config['Daug_All_BPVIPCHI2_MIN']
               , Daug_2of3_BPVIPCHI2_MIN = config['Daug_2of3_BPVIPCHI2_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , Proton_PIDpPIDpi_MIN    = config['Proton_PIDpPIDpi_MIN']
               , Proton_PIDpPIDK_MIN     = config['Proton_PIDpPIDK_MIN']
               , K_PIDK_MIN              = config['K_PIDK_MIN']
               , Pi_PIDK_MAX             = config['Pi_PIDK_MAX']
               , Comb_ADAMASS_WIN        = config['Comb_ADAMASS_WIN']
               , Comb_ADOCAMAX_MAX       = config['Comb_ADOCAMAX_MAX']
               , Lambdac_VCHI2VDOF_MAX   = config['Lambdac_VCHI2VDOF_MAX']
               , Lambdac_acosBPVDIRA_MAX = config['Lambdac_acosBPVDIRA_MAX']
               , Lambdac_PVDispCut       = config['Lambdac_PVDispCut']
               , decDescriptors = [ "[Lambda_c+ -> p+ pi- K+]cc" ]
             )


        self.selLambdac2PKPi = makeTisTos( name = 'Sel' + lambdac_name
                                           , selection = self.combLambdac2PKPi )

        self.selLambdac2PKK = makeTisTos( name = 'Sel' + lambdac_pKK_name
                                           , selection = self.combLambdac2PKK )

        self.selLambdac2PPiPi = makeTisTos( name = 'Sel' + lambdac_ppipi_name
                                           , selection = self.combLambdac2PPiPi )

        self.selLambdac2PPiKWS = makeTisTos( name = 'Sel' + lambdac_ppiK_name
                                           , selection = self.combLambdac2PPiKWS )


        self.line_Lambdac2PKPi = self._strippingLine( name = lambdac_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleLambdac2PKPi'],
                                         postscale = config['PostscaleLambdac2PKPi'],
                                         selection = self.selLambdac2PKPi
                                       )

        self.line_Lambdac2PKK = self._strippingLine( name = lambdac_pKK_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleLambdac2PKK'],
                                         postscale = config['PostscaleLambdac2PKK'],
                                         selection = self.selLambdac2PKK
                                       )

        self.line_Lambdac2PPiPi = self._strippingLine( name = lambdac_ppipi_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleLambdac2PPiPi'],
                                         postscale = config['PostscaleLambdac2PPiPi'],
                                         selection = self.selLambdac2PPiPi
                                       )

        self.line_Lambdac2PPiKWS = self._strippingLine( name = lambdac_ppiK_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleLambdac2PPiKWS'],
                                         postscale = config['PostscaleLambdac2PPiKWS'],
                                         selection = self.selLambdac2PPiKWS
                                       )

    # }

# }


def makeLambdac2PKPi( name
               , inputSel
               , Daug_All_PT_MIN
               , Daug_2of3_PT_MIN
               , Daug_1of3_PT_MIN
               , Daug_All_BPVIPCHI2_MIN
               , Daug_2of3_BPVIPCHI2_MIN
               , Daug_1of3_BPVIPCHI2_MIN
               , Proton_PIDpPIDpi_MIN
               , Proton_PIDpPIDK_MIN
               , K_PIDK_MIN
               , Pi_PIDK_MAX
               , Comb_ADAMASS_WIN
               , Comb_ADOCAMAX_MAX
               , Lambdac_VCHI2VDOF_MAX
               , Lambdac_acosBPVDIRA_MAX
               , Lambdac_PVDispCut
               , decDescriptors = [ "[Lambda_c+ -> p+ K- pi+]cc" ]
             ) : # {

    ## Construct a preambulo to simplify some calculations.
    lclPreambulo = [
      "from math import cos"
      , "bpvdirathresh = cos(%(Lambdac_acosBPVDIRA_MAX)s)" % locals()
      , "pidFiducialPMin = 3.0 * GeV"
      , "pidFiducialPMax = 100.0 * GeV"
    ]


    daugCuts = "(PT > %(Daug_All_PT_MIN)s)" \
               "& (BPVIPCHI2() > %(Daug_All_BPVIPCHI2_MIN)s)" % locals()

    pidFiducialCuts = "(HASRICH)" \
                      "& (in_range(pidFiducialPMin, P, pidFiducialPMax))" \
                      "& (in_range(2.0, ETA, 5.0))"

    pCuts =  pidFiducialCuts + "& ((PIDp-PIDpi) > %(Proton_PIDpPIDpi_MIN)s)" \
             "& ((PIDp-PIDK) > %(Proton_PIDpPIDK_MIN)s)" % locals()
    kCuts =  pidFiducialCuts + "& ((PIDK-PIDpi) > %(K_PIDK_MIN)s)" % locals()
    piCuts = pidFiducialCuts + "& ((PIDK-PIDpi) < %(Pi_PIDK_MAX)s)" % locals()


    combCuts = "(ADAMASS('Lambda_c+') < %(Comb_ADAMASS_WIN)s)" \
               "& (AMAXCHILD(PT) > %(Daug_1of3_PT_MIN)s)" \
               "& (AMAXCHILD(BPVIPCHI2()) > %(Daug_1of3_BPVIPCHI2_MIN)s)" \
               "& (ANUM(PT > %(Daug_2of3_PT_MIN)s) >= 2)" \
               "& (ANUM(BPVIPCHI2() > %(Daug_2of3_BPVIPCHI2_MIN)s) >= 2)" \
               "& (ADOCAMAX('') < %(Comb_ADOCAMAX_MAX)s)" % locals()

    lambdacCuts = "(VFASPF(VCHI2/VDOF) < %(Lambdac_VCHI2VDOF_MAX)s)" \
                  "& (%(Lambdac_PVDispCut)s)" \
                  "& (BPVDIRA > bpvdirathresh)" % locals()


    _Lambdac = CombineParticles(
        DecayDescriptors = decDescriptors
        , Preambulo = lclPreambulo
        , DaughtersCuts = { "pi+" : daugCuts + '&' + piCuts, 
                            "K+"  : daugCuts + '&' + kCuts, 
                            "p+"  : daugCuts + '&' + pCuts }
        , CombinationCut = combCuts
        , MotherCut = lambdacCuts 
    )


    return Selection( name,
                      Algorithm = _Lambdac,
                      RequiredSelections = inputSel
                    )
 
# }


def makeTisTos( name, selection, hltTisTosSpec = { } ) :  # {
    outSel = selection
    if len(hltTisTosSpec) > 0 : # {
        _tisTosFilter = TisTosParticleTagger( name + 'TisTos'
                                              , TisTosSpecs = hltTisTosSpec )

        outSel = Selection( name
                            , Algorithm = _tisTosFilter
                            , RequiredSelections = [ selection ] )
    # }

    return outSel
# }




default_config = {
                     'Daug_All_PT_MIN'          :  200.0 * MeV
                   , 'Daug_2of3_PT_MIN'         :  400.0 * MeV
                   , 'Daug_1of3_PT_MIN'         : 1000.0 * MeV
                   , 'Daug_All_BPVIPCHI2_MIN'   :    1.0
                   , 'Daug_2of3_BPVIPCHI2_MIN'  :    1.0
                   , 'Daug_1of3_BPVIPCHI2_MIN'  :    4.0
                   , 'Proton_PIDpPIDpi_MIN'     :    0.0
                   , 'Proton_PIDpPIDK_MIN'      :    0.0
                   , 'K_PIDK_MIN'               :    0.0
                   , 'Pi_PIDK_MAX'              :    3.0
                   , 'Comb_ADAMASS_WIN'         :   90.0 * MeV
                   , 'Comb_ADOCAMAX_MAX'        :    0.5 * mm
                   , 'Lambdac_VCHI2VDOF_MAX'    :   25.0
                   , 'Lambdac_acosBPVDIRA_MAX'  :   35.0 * mrad
                   , 'Lambdac_PVDispCut'        : "((BPVVDCHI2 > 4.0)|(BPVLTIME() > 0.075 * picosecond))"
                   , 'HltFilter'                :  None
                   , 'PrescaleLambdac2PKPi'     :    1.0
                   , 'PostscaleLambdac2PKPi'    :    1.0
                   , 'PrescaleLambdac2PKK'      :    1.0
                   , 'PostscaleLambdac2PKK'     :    1.0
                   , 'PrescaleLambdac2PPiPi'    :   -1.0
                   , 'PostscaleLambdac2PPiPi'   :   -1.0
                   , 'PrescaleLambdac2PPiKWS'   :   -1.0
                   , 'PostscaleLambdac2PPiKWS'  :   -1.0
                 }


