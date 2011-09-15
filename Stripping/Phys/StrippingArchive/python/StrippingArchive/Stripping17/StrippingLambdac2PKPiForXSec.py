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
from GaudiKernel.SystemOfUnits import MeV, mm, ns
from LHCbKernel.Configuration import *
#from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons, StdNoPIDsProtons

class StrippingLambdac2PKPiForXSecConf(LineBuilder): # {

    __configuration_keys__ = (   'Daug_All_PT_MIN'
                               , 'Daug_1of3_PT_MIN'
                               , 'Daug_P_MIN'
                               , 'Daug_TRCHI2DOF_MAX'
                               , 'Daug_BPVIPCHI2_MIN'
                               , 'Proton_PIDp_MIN'
                               , 'Pi_PIDK_MAX'
                               , 'K_PIDK_MIN'
                               , 'Comb_ADAMASS_WIN'
                               , 'Comb_ADOCAMAX_MAX'
                               , 'Lambdac_PT_MIN'
                               , 'Lambdac_VCHI2VDOF_MAX'
                               , 'Lambdac_BPVVDCHI2_MIN'
                               , 'Lambdac_BPVDIRA_MIN'
                               , 'Lambdac_BPVLTIME_MAX'
                               , 'Lambdac_BPVLTIME_MIN'
                               , 'HltFilter'
                               , 'PrescaleLambdac2PKPi'
                               , 'PostscaleLambdac2PKPi'
                             )



    def __init__(self, name, config) : # {

        LineBuilder.__init__(self, name, config)


        lambdac_name = name + 'Lambdac2PKPi'

        self.inPions   = StdNoPIDsPions
        self.inKaons   = StdNoPIDsKaons
        self.inProtons = StdNoPIDsProtons


        self.selLambdac2PKPi = makeLambdac2PKPi( name = lambdac_name 
               , inputSel = [ self.inPions, self.inKaons, self.inProtons ]
               , Daug_All_PT_MIN = config['Daug_All_PT_MIN']
               , Daug_1of3_PT_MIN  = config['Daug_1of3_PT_MIN']
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , Daug_BPVIPCHI2_MIN = config['Daug_BPVIPCHI2_MIN']
               , Proton_PIDp_MIN = config['Proton_PIDp_MIN']
               , Pi_PIDK_MAX = config['Pi_PIDK_MAX']
               , K_PIDK_MIN = config['K_PIDK_MIN']
               , Comb_ADAMASS_WIN  = config['Comb_ADAMASS_WIN']
               , Comb_ADOCAMAX_MAX = config['Comb_ADOCAMAX_MAX']
               , Lambdac_PT_MIN = config['Lambdac_PT_MIN']
               , Lambdac_VCHI2VDOF_MAX = config['Lambdac_VCHI2VDOF_MAX']
               , Lambdac_BPVVDCHI2_MIN = config['Lambdac_BPVVDCHI2_MIN']
               , Lambdac_BPVDIRA_MIN = config['Lambdac_BPVDIRA_MIN']
               , Lambdac_BPVLTIME_MAX = config['Lambdac_BPVLTIME_MAX']
               , Lambdac_BPVLTIME_MIN = config['Lambdac_BPVLTIME_MIN']
             )


        self.line_Lambdac2PKPi = StrippingLine( lambdac_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleLambdac2PKPi'],
                                         postscale = config['PostscaleLambdac2PKPi'],
                                         algos = [ self.selLambdac2PKPi ]
                                       )

        self.registerLine(self.line_Lambdac2PKPi)

    # }

# }


def makeLambdac2PKPi( name
               , inputSel
               , Daug_All_PT_MIN
               , Daug_1of3_PT_MIN 
               , Daug_P_MIN
               , Daug_TRCHI2DOF_MAX
               , Daug_BPVIPCHI2_MIN
               , Proton_PIDp_MIN
               , Pi_PIDK_MAX
               , K_PIDK_MIN
               , Comb_ADAMASS_WIN 
               , Comb_ADOCAMAX_MAX
               , Lambdac_PT_MIN
               , Lambdac_VCHI2VDOF_MAX
               , Lambdac_BPVVDCHI2_MIN
               , Lambdac_BPVDIRA_MIN
               , Lambdac_BPVLTIME_MAX
               , Lambdac_BPVLTIME_MIN
               , decDescriptors = [ "[Lambda_c+ -> p+ K- pi+]cc" ]
             ) : # {

    daugCuts = "(PT > %(Daug_All_PT_MIN)s)" \
               "& (P > %(Daug_P_MIN)s)" \
               "& (TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" \
               "& (BPVIPCHI2() > %(Daug_BPVIPCHI2_MIN)s)" % locals()

    pCuts =  "((PIDp-PIDpi) > %(Proton_PIDp_MIN)s)" % locals()
    piCuts = "((PIDK-PIDpi) < %(Pi_PIDK_MAX)s)" % locals()
    kCuts =  "((PIDK-PIDpi) > %(K_PIDK_MIN)s)" % locals()


    combCuts = "(ADAMASS('Lambda_c+') < %(Comb_ADAMASS_WIN)s)" \
               "& (AMAXCHILD(PT) > %(Daug_1of3_PT_MIN)s)" \
               "& (ADOCAMAX('') < %(Comb_ADOCAMAX_MAX)s)" % locals()

    lambdacCuts = "(PT > %(Lambdac_PT_MIN)s)" \
                  "& (VFASPF(VCHI2/VDOF) < %(Lambdac_VCHI2VDOF_MAX)s)" \
                  "& (BPVVDCHI2 > %(Lambdac_BPVVDCHI2_MIN)s)" \
                  "& (BPVDIRA > %(Lambdac_BPVDIRA_MIN)s)" \
                  "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(Lambdac_BPVLTIME_MIN)s)" \
                  "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') < %(Lambdac_BPVLTIME_MAX)s)" % locals()


    _Lambdac = CombineParticles(
        DecayDescriptors = decDescriptors
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


default_config = {  'Daug_All_PT_MIN'       :  400.0 * MeV
                  , 'Daug_1of3_PT_MIN'      : 1200.0 * MeV
                  , 'Daug_P_MIN'            : 3200.0 * MeV
                  , 'Daug_TRCHI2DOF_MAX'    :   10.0
                  , 'Daug_BPVIPCHI2_MIN'    :    0.5
                  , 'Proton_PIDp_MIN'       :   10.0
                  , 'Pi_PIDK_MAX'           :    0.0
                  , 'K_PIDK_MIN'            :   10.0
                  , 'Comb_ADAMASS_WIN'      :   90.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'     :    0.1 * mm
                  , 'Lambdac_PT_MIN'        :    0.0 * MeV
                  , 'Lambdac_VCHI2VDOF_MAX' :   20.0
                  , 'Lambdac_BPVVDCHI2_MIN' :    8.0
                  , 'Lambdac_BPVDIRA_MIN'   :    0.9999
                  , 'Lambdac_BPVLTIME_MAX'  :    0.0012 * ns
                  , 'Lambdac_BPVLTIME_MIN'  :    0.0 * ns
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
                  , 'PrescaleLambdac2PKPi'  :    1.0
                  , 'PostscaleLambdac2PKPi' :    1.0
                 }


