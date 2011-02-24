'''
D+ cross-section lines.

Adapted to current stripping framework by P. Spradlin.
'''

__author__ = ['Hamish Gordon', 'Patrick Spradlin']
__date__ = '03/09/2010'
__version__ = '$Revision: 1.4 $'

__all__ = ( 'StrippingD2HHHForXSecConf',
            'makeD2HHH',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV
from LHCbKernel.Configuration import *
#from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions, StdLooseKaons

class StrippingD2HHHForXSecConf(LineBuilder): # {

    __configuration_keys__ = (  'Daug_All_IPCHI2_MIN'
                              , 'Daug_2of3_IPCHI2_MIN'
                              , 'Daug_1of3_IPCHI2_MIN'
                              , 'Daug_All_PT_MIN'
                              , 'Daug_2of3_PT_MIN'
                              , 'Daug_P_MIN'
                              , 'Daug_P_MAX'
                              , 'Daug_TRCHI2DOF_MAX'
                              , 'K_PIDK_MIN'
                              , 'Pi_PIDK_MAX'
                              , 'Comb_AM_MIN'
                              , 'Comb_AM_MAX'
                              , 'D_M_MIN'
                              , 'D_M_MAX'
                              , 'D_BPVDIRA_MIN'
                              , 'D_BPVVDCHI2_MIN'
                              , 'HltFilter'
                              , 'PrescaleD2KPP'
                              , 'PrescaleD2KKP'
                              , 'PrescaleD2KKK'
                              , 'PrescaleD2PPP'
                              , 'PrescaleD2KPPDCS'
                              , 'PostscaleD2KPP'
                              , 'PostscaleD2KKP'
                              , 'PostscaleD2KKK'
                              , 'PostscaleD2PPP'
                              , 'PostscaleD2KPPDCS'
                             )



    def __init__(self, name, config) : # {

        ## Base class constructor
        LineBuilder.__init__(self, name, config)

        ## Line names
        d2KPP_name = name + 'D2KPP'
        d2KKP_name = name + 'D2KKP'
        d2KKK_name = name + 'D2KKK'
        d2PPP_name = name + 'D2PPP'
        d2PPK_name = name + 'D2KPPDCS'

        self.inNoPIDsPions = StdNoPIDsPions
        self.inLooseKaons  = StdLooseKaons


        ## The (K- pi+ pi+) final state
        ## No PID lines.  It looks like the Loose PID lines have the
        self.selD2KPP = makeD2HHH( name = d2KPP_name
               , inputSel = [ self.inNoPIDsPions, self.inLooseKaons ]
               , Daug_All_IPCHI2_MIN = config['Daug_All_IPCHI2_MIN']
               , Daug_2of3_IPCHI2_MIN = config['Daug_2of3_IPCHI2_MIN']
               , Daug_1of3_IPCHI2_MIN = config['Daug_1of3_IPCHI2_MIN']
               , Daug_All_PT_MIN = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN = config['Daug_2of3_PT_MIN']
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_P_MAX = config['Daug_P_MAX']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , K_PIDK_MIN = config['K_PIDK_MIN']
               , Pi_PIDK_MAX = config['Pi_PIDK_MAX']
               , Comb_AM_MIN = config['Comb_AM_MIN']
               , Comb_AM_MAX = config['Comb_AM_MAX']
               , D_M_MIN = config['D_M_MIN']
               , D_M_MAX = config['D_M_MAX']
               , D_BPVDIRA_MIN = config['D_BPVDIRA_MIN']
               , D_BPVVDCHI2_MIN = config['D_BPVVDCHI2_MIN']
               , decDescriptors = ['[D+ -> K- pi+ pi+]cc' ]
             )


        self.line_D2KPP = StrippingLine( d2KPP_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2KPP'],
                                postscale = config['PostscaleD2KPP'],
                                algos = [ self.selD2KPP ])
        self.registerLine(self.line_D2KPP)


        ## The (K- K+ pi+) final state
        ## No PID lines.
        self.selD2KKP = makeD2HHH( name = d2KKP_name 
               , inputSel = [ self.inNoPIDsPions, self.inLooseKaons ]
               , Daug_All_IPCHI2_MIN = config['Daug_All_IPCHI2_MIN']
               , Daug_2of3_IPCHI2_MIN = config['Daug_2of3_IPCHI2_MIN']
               , Daug_1of3_IPCHI2_MIN = config['Daug_1of3_IPCHI2_MIN']
               , Daug_All_PT_MIN = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN = config['Daug_2of3_PT_MIN']
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_P_MAX = config['Daug_P_MAX']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , K_PIDK_MIN = config['K_PIDK_MIN']
               , Pi_PIDK_MAX = config['Pi_PIDK_MAX']
               , Comb_AM_MIN = config['Comb_AM_MIN']
               , Comb_AM_MAX = config['Comb_AM_MAX']
               , D_M_MIN = config['D_M_MIN']
               , D_M_MAX = config['D_M_MAX']
               , D_BPVDIRA_MIN = config['D_BPVDIRA_MIN']
               , D_BPVVDCHI2_MIN = config['D_BPVVDCHI2_MIN']
               , decDescriptors = ['[D+ -> K- K+ pi+]cc' ]
             )


        self.line_D2KKP = StrippingLine( d2KKP_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2KKP'],
                                postscale = config['PostscaleD2KKP'],
                                algos = [ self.selD2KKP ])
        self.registerLine(self.line_D2KKP)



        ## The (K- K+ K+) final state
        self.selD2KKK = makeD2HHH( name = d2KKK_name 
               , inputSel = [ self.inLooseKaons ]
               , Daug_All_IPCHI2_MIN = config['Daug_All_IPCHI2_MIN']
               , Daug_2of3_IPCHI2_MIN = config['Daug_2of3_IPCHI2_MIN']
               , Daug_1of3_IPCHI2_MIN = config['Daug_1of3_IPCHI2_MIN']
               , Daug_All_PT_MIN = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN = config['Daug_2of3_PT_MIN']
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_P_MAX = config['Daug_P_MAX']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , K_PIDK_MIN = config['K_PIDK_MIN']
               , Pi_PIDK_MAX = config['Pi_PIDK_MAX']
               , Comb_AM_MIN = config['Comb_AM_MIN']
               , Comb_AM_MAX = config['Comb_AM_MAX']
               , D_M_MIN = config['D_M_MIN']
               , D_M_MAX = config['D_M_MAX']
               , D_BPVDIRA_MIN = config['D_BPVDIRA_MIN']
               , D_BPVVDCHI2_MIN = config['D_BPVVDCHI2_MIN']
               , decDescriptors = ['[D+ -> K- K+ K+]cc' ]
             )


        self.line_D2KKK = StrippingLine( d2KKK_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2KKK'],
                                postscale = config['PostscaleD2KKK'],
                                algos = [ self.selD2KKK ])
        #self.registerLine(self.line_D2KKK)


        ## The (pi- pi+ pi+) final state
        ## No PID lines. 
        self.selD2PPP = makeD2HHH( name = d2PPP_name
               , inputSel = [ self.inNoPIDsPions, self.inLooseKaons ]
               , Daug_All_IPCHI2_MIN = config['Daug_All_IPCHI2_MIN']
               , Daug_2of3_IPCHI2_MIN = config['Daug_2of3_IPCHI2_MIN']
               , Daug_1of3_IPCHI2_MIN = config['Daug_1of3_IPCHI2_MIN']
               , Daug_All_PT_MIN = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN = config['Daug_2of3_PT_MIN']
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_P_MAX = config['Daug_P_MAX']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , K_PIDK_MIN = config['K_PIDK_MIN']
               , Pi_PIDK_MAX = config['Pi_PIDK_MAX']
               , Comb_AM_MIN = config['Comb_AM_MIN']
               , Comb_AM_MAX = config['Comb_AM_MAX']
               , D_M_MIN = config['D_M_MIN']
               , D_M_MAX = config['D_M_MAX']
               , D_BPVDIRA_MIN = config['D_BPVDIRA_MIN']
               , D_BPVVDCHI2_MIN = config['D_BPVVDCHI2_MIN']
               , decDescriptors = ['[D+ -> pi- pi+ pi+]cc' ]
             )


        self.line_D2PPP = StrippingLine( d2PPP_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2PPP'],
                                postscale = config['PostscaleD2PPP'],
                                algos = [ self.selD2PPP])
        #self.registerLine(self.line_D2PPP)


        ## The (K+ pi- pi+) final state
        ## No PID lines.  It looks like the Loose PID lines have the
        self.selD2PPK = makeD2HHH( name = d2PPK_name
               , inputSel = [ self.inNoPIDsPions, self.inLooseKaons ]
               , Daug_All_IPCHI2_MIN = config['Daug_All_IPCHI2_MIN']
               , Daug_2of3_IPCHI2_MIN = config['Daug_2of3_IPCHI2_MIN']
               , Daug_1of3_IPCHI2_MIN = config['Daug_1of3_IPCHI2_MIN']
               , Daug_All_PT_MIN = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN = config['Daug_2of3_PT_MIN']
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_P_MAX = config['Daug_P_MAX']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , K_PIDK_MIN = config['K_PIDK_MIN']
               , Pi_PIDK_MAX = config['Pi_PIDK_MAX']
               , Comb_AM_MIN = config['Comb_AM_MIN']
               , Comb_AM_MAX = config['Comb_AM_MAX']
               , D_M_MIN = config['D_M_MIN']
               , D_M_MAX = config['D_M_MAX']
               , D_BPVDIRA_MIN = config['D_BPVDIRA_MIN']
               , D_BPVVDCHI2_MIN = config['D_BPVVDCHI2_MIN']
               , decDescriptors = ['[D+ -> K+ pi- pi+]cc' ]
             )


        self.line_D2PPK = StrippingLine( d2PPK_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2KPPDCS'],
                                postscale = config['PostscaleD2KPPDCS'],
                                algos = [ self.selD2PPK ])
        #self.registerLine(self.line_D2PPK)

    # }

# }




def makeD2HHH( name
               , inputSel
               , Daug_All_IPCHI2_MIN
               , Daug_2of3_IPCHI2_MIN
               , Daug_1of3_IPCHI2_MIN
               , Daug_All_PT_MIN
               , Daug_2of3_PT_MIN
               , Daug_P_MIN
               , Daug_P_MAX
               , Daug_TRCHI2DOF_MAX
               , K_PIDK_MIN
               , Pi_PIDK_MAX
               , Comb_AM_MIN
               , Comb_AM_MAX
               , D_M_MIN
               , D_M_MAX
               , D_BPVDIRA_MIN
               , D_BPVVDCHI2_MIN
               , decDescriptors
             ) : # {

    daugCuts = "(PT > %(Daug_All_PT_MIN)s)" \
               "& (P > %(Daug_P_MIN)s)" \
               "& (P < %(Daug_P_MAX)s)" \
               "& (TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" \
               "& (MIPCHI2DV(PRIMARY) > %(Daug_All_IPCHI2_MIN)s)" % locals()

    kaonPIDCut = "(PIDK-PIDpi > %(K_PIDK_MIN)s)" % locals()
    pionPIDCut = "(PIDK-PIDpi < %(Pi_PIDK_MAX)s)" % locals()

    combCuts = "(AM > %(Comb_AM_MIN)s) & (AM < %(Comb_AM_MAX)s)" \
               "& (ANUM(PT > %(Daug_2of3_PT_MIN)s) >= 2)" \
               "& (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Daug_1of3_IPCHI2_MIN)s))"\
               "& (ANUM(MIPCHI2DV(PRIMARY) > %(Daug_2of3_IPCHI2_MIN)s) >= 2)" % locals()

    dCuts = "in_range(%(D_M_MIN)s, M, %(D_M_MAX)s)" \
            "& (BPVDIRA > %(D_BPVDIRA_MIN)s)" \
            "& (BPVVDCHI2 > %(D_BPVVDCHI2_MIN)s)" % locals()

    _dplus = CombineParticles( DecayDescriptors = decDescriptors
                               , DaughtersCuts = { "pi+" : daugCuts + '&' + pionPIDCut, 
                                                   "K+" : daugCuts + '&' + kaonPIDCut }
                               , CombinationCut = combCuts
                               , MotherCut = dCuts
                             )
    return Selection( name,
                      Algorithm = _dplus,
                      RequiredSelections = inputSel
                    )

# }




default_config = {  'Daug_All_IPCHI2_MIN' : 2.0
                  , 'Daug_2of3_IPCHI2_MIN' : 8.0
                  , 'Daug_1of3_IPCHI2_MIN' : 30.0
                  , 'Daug_All_PT_MIN' : 200.0*MeV
                  , 'Daug_2of3_PT_MIN' : 400.0*MeV
                  , 'Daug_P_MIN' : 3200.0*MeV
                  , 'Daug_P_MAX' : 1.0e5 * MeV
                  , 'Daug_TRCHI2DOF_MAX' : 10.0
                  , 'K_PIDK_MIN' : 3.0
                  , 'Pi_PIDK_MAX' : 12.0
                  , 'Comb_AM_MIN' : 1580.0 *MeV
                  , 'Comb_AM_MAX' : 2260.0 *MeV
                  , 'D_M_MIN' : 1780.0 *MeV
                  , 'D_M_MAX' : 2060.0 *MeV
                  , 'D_BPVDIRA_MIN' : 0.9999
                  , 'D_BPVVDCHI2_MIN' : 60.0
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
                  , 'PrescaleD2KPP' : 1.0
                  , 'PrescaleD2KKP' : 1.0
                  , 'PrescaleD2KKK' : 1.0
                  , 'PrescaleD2PPP' : 1.0
                  , 'PrescaleD2KPPDCS' : 1.0
                  , 'PostscaleD2KPP' : 1.0
                  , 'PostscaleD2KKP' : 1.0
                  , 'PostscaleD2KKK' : 1.0
                  , 'PostscaleD2PPP' : 1.0
                  , 'PostscaleD2KPPDCS' : 1.0
                 }

