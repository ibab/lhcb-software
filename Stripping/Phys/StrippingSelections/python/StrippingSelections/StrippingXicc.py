# $Id$
'''
Speculative code for doubly-charmed baryon searches.
The following six final states are used:
  Xicc+  -> Lambdac+ K- pi+
  Xicc++ -> Lambdac+ K- pi+ pi+
  Xicc+  -> Xic0 pi+
  Xicc++ -> Xic0 pi+ pi+
  Xicc+  -> Xic+ pi+ pi-
  Xicc++ -> Xic+ pi+
where the daughter charmed baryon is reconstructed via:
  Lambdac+ -> p K- pi+ (from StdLooseLambdac2PKPi)
  Xic0 -> Xi- pi+
  Xic+ -> Xi- pi+ pi+
and where the Xi- is reconstructed via:
  Xi- -> Lambda pi-, Lambda -> p pi- (from StdLooseLambdaDD and StdLooseLambdaLL)
In addition to the six Xicc signal lines, three control lines are written out
for the Lambdac+, Xic0, and Xic+. It will certainly be necessary to prescale
the Lambdac+ control line, and perhaps the others too.
'''

__author__ = ['Mat Charles', 'Patrick Spradlin', 'Stephen Ogilvy']
__date__ = '20/Jan/2012'
__version__ = '$Revision: 1.1 $'

__all__ = ( 'XiccBuilder'               ## LineBuilder class specialization
            , 'default_name'            ## Default name for XiccBuilder object
            , 'default_config'          ## Default config for XiccBuilder object
            , 'makeLc'                  ## Lambda_c+ selection
            , 'makeXi'                  ## Xi selection
            , 'makeXicZero'             ## Xi_c^0 selection
            , 'makeXicPlus'             ## Xi_c^+ selection
            , 'makeXicc'                ## Xi_cc^{+(+)} selection
            , 'makeTisTos'              ## Apply TisTos filters
            , 'filterKaons'
            , 'filterPions'
            , 'filterProtons'
            , 'filterDplus'
            , 'filterDsplus'
            , 'filterD0' 
            , 'filterLcForControl'
          )



from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLoosePions, StdAllLooseKaons, StdAllLooseProtons
from StandardParticles import StdLoosePions
from CommonParticles.StdLooseLambda import StdLooseLambdaLL, StdLooseLambdaDD
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from Configurables import TisTosParticleTagger


default_name = "Xicc"

default_config = {
      'LongTrackGEC' : 150
    , 'controlPrescaleLc' : 0.05
    , 'controlPrescaleXic' : 1.0
    , 'controlPrescaleDp' : 0.01
    , 'controlPrescaleD0' : 0.01
    , 'controlPrescaleDsp' : 0.01
    , 'signalPrescaleViaLc' : 1.0
    , 'signalPrescaleViaLcWC' : 1.0
    , 'signalPrescaleViaLcDCS' : 1.0
    , 'signalPrescaleViaXic' : 1.0
    , 'signalPrescaleViaXicWC' : 1.0
    , 'signalPrescaleViaDp'  : 1.0
    , 'signalPrescaleViaDpWC': 1.0
    , 'signalPrescaleViaD0'  : 1.0
    , 'signalPrescaleViaD0WC'  : 1.0
    , 'signalPrescaleViaD0DCS'  : 1.0
    , 'Hlt2TisTosSpec' : { 'Hlt2CharmHad.*Decision%TOS' : 0, 'Hlt2Global%TIS' : 0 }
}


## These configuration parameters should not be changed outside of this file.
_my_immutable_config = {
        'Lc_Pi_InputList'               : 'Phys/StdLoosePions/Particles'
        , 'Lc_K_InputList'              : 'Phys/StdLooseKaons/Particles'
        , 'Lc_P_InputList'              : 'Phys/StdLooseProtons/Particles'
        , 'Xi_Pi_InputList'             : 'Phys/StdLoosePions/Particles'
        , 'Xicc_Pi_InputList'           : 'Phys/StdAllLoosePions/Particles'
        , 'Xicc_K_InputList'            : 'Phys/StdAllLooseKaons/Particles'
        , 'Xicc_P_InputList'            : 'Phys/StdAllLooseProtons/Particles'
        , 'Xicc_Dp_InputList'           : 'Phys/StdLooseDplus2KPiPi/Particles'
        , 'Xicc_Dsp_InputList'          : 'Phys/StdLooseDsplus2KKPi/Particles'
        , 'Xicc_D0_InputList'           : 'Phys/StdLooseD02KPi/Particles'
        , 'Lc_Daug_TRCHI2DOF_Max'       :   5.0
        , 'Lc_Daug_PT_Min'              : 200.0*MeV
        , 'Lc_Daug_P_Min'               :   2.0*GeV
        , 'Lc_Daug_1of3_MIPCHI2DV_Min'  :   4.0
        , 'Lc_K_PIDKPIDpi_Min'          :   5.0
        , 'Lc_Pi_PIDpiPIDK_Min'         :   0.0
        , 'Lc_P_PIDpPIDpi_Min'          :   5.0
        , 'Lc_P_PIDpPIDK_Min'           :   0.0
        , 'Lc_ADMASS_HalfWin'           :  75.0*MeV
        , 'Lc_APT_Min'                  :   1.0*GeV
        , 'Lc_ADOCAMAX_Max'             :   0.5*mm
        , 'Lc_VCHI2_Max'                :  30.0
        , 'Lc_BPVVDCHI2_Min'            :  16.0
        , 'Lc_BPVDIRA_Min'              :   0.99
        , 'LcControl_BPVDIRA_Min'       :   0.999
        , 'Dp_ADMASS_HalfWin'           :  75.0*MeV
        , 'Dp_BPVVDCHI2_Min'            : 100.0
        , 'Dsp_ADMASS_HalfWin'          :  75.0*MeV
        , 'Dsp_BPVVDCHI2_Min'           :  64.0
        , 'D0_ADMASS_HalfWin'           :  75.0*MeV
        , 'D0_BPVVDCHI2_Min'            :  64.0
        , 'Xi_Pi_P_Min'                 :   2.0*GeV
        , 'Xi_Pi_TRCHI2DOF_Max'         :   4.0
        , 'Xi_Pi_PT_Min'                : 250.0*MeV
        , 'Xi_Pi_MIPCHI2DV_Min'         :  25.0
        , 'Xi_VCHI2_Max'                :  20.0
        , 'Xi_LL_ADAMASS_HalfWin'       :  50.0*MeV
        , 'Xi_LL_ADMASS_HalfWin'        :  35.0*MeV
        , 'Xi_DD_ADAMASS_HalfWin'       :  80.0*MeV
        , 'Xi_DD_ADMASS_HalfWin'        :  50.0*MeV
        , 'Xic0_ADAMASS_HalfWin'        : 170.0*MeV
        , 'Xic0_ADMASS_HalfWin'         : 120.0*MeV
        , 'Xic0_BPVDIRA_Min'            :   0.9
        , 'Xic0_BPVVDCHI2_Min'          :  25.0
        , 'Xic0_VCHI2_Max'              :  30.0
        , 'Xicplus_ADAMASS_HalfWin'     : 170.0*MeV
        , 'Xicplus_ADMASS_HalfWin'      : 120.0*MeV
        , 'Xicplus_BPVDIRA_Min'         :   0.9
        , 'Xicplus_BPVVDCHI2_Min'       :  25.0
        , 'Xicplus_VCHI2_Max'           :  60.0
        , 'Xicplus_1of2_MIPCHI2DV_Min'  :  10.0
        , 'Xicc_Daug_TRCHI2DOF_Max'     :   5.0
        , 'Xicc_Daug_P_Min'             :   2.0*GeV
        , 'Xicc_Daug_PT_Min'            : 250.0*MeV
        , 'Xicc_Daug_MIPCHI2DV_Min'     :  -1.0
        , 'Xicc_Pi_PIDpiPIDK_Min'       :   0.0
        , 'Xicc_K_PIDKPIDpi_Min'        :   5.0
        , 'Xicc_P_PIDpPIDpi_Min'        :   5.0
        , 'Xicc_P_PIDpPIDK_Min'         :   0.0
        , 'Xicc_APT_Min'                :   2.0*GeV # Mat changes from 1.0 to 2.0 GeV for S20
        , 'Xicc_ADOCAMAX_Max'           :   0.5*mm
        , 'Xicc_BPVDIRA_Min'            :   0.0
        , 'Xicc_BPVVDCHI2_Min'          :  -1.0
        , 'Xicc_Lc_delta_VZ_Min'        :   0.01*mm
        , 'Xicc_2Dau_VCHI2_Max'         :  20.0
        , 'Xicc_3Dau_VCHI2_Max'         :  30.0
        , 'Xicc_4Dau_VCHI2_Max'         :  60.0
        , 'Xicc_AM_Max'                 :   4.0*GeV
        }




class XiccBuilder(LineBuilder) :
    """
    Search for Xicc
    """

    __configuration_keys__ = ( 'LongTrackGEC'
                               , 'controlPrescaleLc'
                               , 'controlPrescaleXic'
                               , 'controlPrescaleDp'
                               , 'controlPrescaleD0'
                               , 'controlPrescaleDsp'
                               , 'signalPrescaleViaLc'
                               , 'signalPrescaleViaLcWC'
                               , 'signalPrescaleViaLcDCS'
                               , 'signalPrescaleViaXic'
                               , 'signalPrescaleViaXicWC'
                               , 'signalPrescaleViaDp'
                               , 'signalPrescaleViaDpWC'
                               , 'signalPrescaleViaD0'
                               , 'signalPrescaleViaD0WC'
                               , 'signalPrescaleViaD0DCS'
                               , 'Hlt2TisTosSpec')

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCutsCode = "(recSummary (LHCb.RecSummary.nLongTracks, 'Rec/Track/Long') < %(LongTrackGEC)s )" % config
        _globalEventCuts = { 'Code' : _globalEventCutsCode, 'Preambulo' : ["from LoKiTracks.decorators import *"]}

        # Pick up standard kaons, pions
        # Filter them for use as daughter particles:
        self.dauPi = filterPions(name+'FilteredPions')
        self.dauK  = filterKaons(name+'FilteredKaons')
        self.dauP  = filterProtons(name+'FilteredProtons')

        # Pick up standard Lambdac -> p K- pi+ then filter it to reduce rate:
        self.filterLc = makeLc(name+'FilterLc')
        self.filterLcForControl = filterLcForControl(name+'FilterLcForControl', self.filterLc)

        # Pick up standard D+ -> K- pi+ pi+ then filter it to reduce rate:
        self.dplus  = filterDplus(name+'FilteredDplus2KPiPi')

        # Pick up standard D_s+ -> K- K+ pi+ then filter it to reduce rate:
        self.dsplus = filterDsplus(name+'FilteredDsplus2KKPi')

        # Pick up standard D0 -> K- pi+ then filter it to reduce rate:
        self.dzero = filterD0(name+'FilteredD02KPi')


        ## Some generic cuts for Xicc.
        ## Vertex chi2 cut depends on number of daughters:
        ##      (2 dau => 1 NDF; 3 dau => 3 NDF; 4 dau => 5 NDF)
        _strCutComb  = "(AM<%(Xicc_AM_Max)s)" \
                       "& (APT>%(Xicc_APT_Min)s)" \
                       "& (ADOCAMAX('')<%(Xicc_ADOCAMAX_Max)s)" % _my_immutable_config
        _strCutMoth  = "(CHILD(VFASPF(VZ),1) - VFASPF(VZ) > %(Xicc_Lc_delta_VZ_Min)s)" \
                       "& (BPVVDCHI2 > %(Xicc_BPVVDCHI2_Min)s)" \
                       "& (BPVDIRA > %(Xicc_BPVDIRA_Min)s)" % _my_immutable_config

        _strChi2Moth2 = "(VFASPF(VCHI2)<%(Xicc_2Dau_VCHI2_Max)s)" % _my_immutable_config
        _strChi2Moth3 = "(VFASPF(VCHI2)<%(Xicc_3Dau_VCHI2_Max)s)" % _my_immutable_config
        _strChi2Moth4 = "(VFASPF(VCHI2)<%(Xicc_4Dau_VCHI2_Max)s)" % _my_immutable_config

        _strCutMoth2 = _strChi2Moth2 + '&' + _strCutMoth
        _strCutMoth3 = _strChi2Moth3 + '&' + _strCutMoth
        _strCutMoth4 = _strChi2Moth4 + '&' + _strCutMoth


        # Combine Lambda with pion to make Xi-
        self.stdLambdaLL = DataOnDemand(Location = 'Phys/StdLooseLambdaLL/Particles')
        self.stdLambdaDD = DataOnDemand(Location = 'Phys/StdLooseLambdaDD/Particles')
        self.combineXiLL = makeXi(name+'CombineXiLL', self.stdLambdaLL, _my_immutable_config['Xi_LL_ADAMASS_HalfWin'], _my_immutable_config['Xi_LL_ADMASS_HalfWin'])
        self.combineXiDD = makeXi(name+'CombineXiDD', self.stdLambdaDD, _my_immutable_config['Xi_DD_ADAMASS_HalfWin'], _my_immutable_config['Xi_DD_ADMASS_HalfWin'])
        self.combineXi   = mergeLists(name+'CombineXi', [ self.combineXiLL, self.combineXiDD ] )

        # Combine Xi- with pion(s) to make Xic0, Xic+
        self.combineXicZero = makeXicZero(name+"CombineXicZero", [ self.combineXi, self.dauPi ])
        self.combineXicPlus = makeXicPlus(name+"CombineXicPlus", [ self.combineXi, self.dauPi ])


        # Combine Lc+ with a K and a pi to make a Xicc+ or Xicc++:
        self.combineXicc1 = makeXicc(name+'CombineXicc1', [ self.filterLc, self.dauPi, self.dauK ], '[Xi_cc+ -> Lambda_c+ K- pi+]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2 = makeXicc(name+'CombineXicc2', [ self.filterLc, self.dauPi, self.dauK ], '[Xi_cc++ -> Lambda_c+ K- pi+ pi+]cc', _strCutComb, _strCutMoth4)
        ## Construct charge violating combinations.
        self.combineXicc1WC = makeXicc(name+'CombineXicc1WC', [ self.filterLc, self.dauPi, self.dauK ], '[Xi_cc+ -> Lambda_c+ K- pi-]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2WC = makeXicc(name+'CombineXicc2WC', [ self.filterLc, self.dauPi, self.dauK ], '[Xi_cc++ -> Lambda_c+ K- pi+ pi-]cc', _strCutComb, _strCutMoth4)

        ## Construct DCS combinations (background modelling check)
        self.combineXicc1DCS = makeXicc(name+'CombineXicc1DCS', [ self.filterLc, self.dauPi, self.dauK ], '[Xi_cc+ -> Lambda_c+ K+ pi-]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2DCS = makeXicc(name+'CombineXicc2DCS', [ self.filterLc, self.dauPi, self.dauK ], '[Xi_cc++ -> Lambda_c+ K+ pi- pi+]cc', _strCutComb, _strCutMoth4)


        # Combine D+ with a K and a p to make a Xicc+ or Xicc++:
        self.combineXicc1a = makeXicc(name+'CombineXicc1a', [ self.dplus, self.dauP, self.dauK ], '[Xi_cc+ -> D+ p+ K-]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2a = makeXicc(name+'CombineXicc2a', [ self.dplus, self.dauP, self.dauK, self.dauPi ], '[Xi_cc++ -> D+ p+ K- pi+]cc', _strCutComb, _strCutMoth4)

        ## Construct charge violating combinations.
        self.combineXicc1aWC = makeXicc(name+'CombineXicc1aWC', [ self.dplus, self.dauP, self.dauK ], '[Xi_cc+ -> D+ p+ K+]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2aWC = makeXicc(name+'CombineXicc2aWC', [ self.dplus, self.dauP, self.dauK, self.dauPi ], '[Xi_cc++ -> D+ p+ K+ pi+]cc', _strCutComb, _strCutMoth4)
        self.combineXicc2aWC0 = makeXicc(name+'CombineXicc2aWC0', [ self.dplus, self.dauP, self.dauK, self.dauPi ], '[Xi_cc++ -> D+ p+ K- pi-]cc', _strCutComb, _strCutMoth4)


        # Combine D0 with a K, a p and a pi to make a Xicc+ or Xicc++:
        self.combineXicc7 = makeXicc(name+'CombineXicc7', [ self.dzero, self.dauP, self.dauK, self.dauPi ], '[Xi_cc+ -> D0 p+ K- pi+]cc', _strCutComb, _strCutMoth4)
        self.combineXicc8 = makeXicc(name+'CombineXicc8', [ self.dzero, self.dauP, self.dauK, self.dauPi ], '[Xi_cc++ -> D0 p+ K- pi+ pi+]cc', _strCutComb, _strCutMoth4)

        ## Construct charge violating combinations.
        self.combineXicc7WC = makeXicc(name+'CombineXicc7WC', [ self.dzero, self.dauP, self.dauK, self.dauPi ], '[Xi_cc+ -> D0 p+ K- pi-]cc', _strCutComb, _strCutMoth4)
        self.combineXicc8WC = makeXicc(name+'CombineXicc8WC', [ self.dzero, self.dauP, self.dauK, self.dauPi ], '[Xi_cc++ -> D0 p+ K- pi+ pi-]cc', _strCutComb, _strCutMoth4)

        ## Construct DCS combinations (background modelling check)
        self.combineXicc7DCS = makeXicc(name+'CombineXicc7DCS', [ self.dzero, self.dauP, self.dauK, self.dauPi ], '[Xi_cc+ -> D0 p+ K+ pi-]cc', _strCutComb, _strCutMoth4)
        self.combineXicc8DCS = makeXicc(name+'CombineXicc8DCS', [ self.dzero, self.dauP, self.dauK, self.dauPi ], '[Xi_cc++ -> D0 p+ K+ pi+ pi-]cc', _strCutComb, _strCutMoth4)


        # Combine Xic0/+ with pion(s) to make Xicc+, Xicc++
        self.combineXicc3 = makeXicc(name+'CombineXicc3', [ self.combineXicZero, self.dauPi ], '[Xi_cc+ -> Xi_c0 pi+]cc', _strCutComb, _strCutMoth2)
        self.combineXicc4 = makeXicc(name+'CombineXicc4', [ self.combineXicZero, self.dauPi ], '[Xi_cc++ -> Xi_c0 pi+ pi+]cc', _strCutComb, _strCutMoth3)
        self.combineXicc5 = makeXicc(name+'CombineXicc5', [ self.combineXicPlus, self.dauPi ], '[Xi_cc+ -> Xi_c+ pi+ pi-]cc', _strCutComb, _strCutMoth3)
        self.combineXicc6 = makeXicc(name+'CombineXicc6', [ self.combineXicPlus, self.dauPi ], '[Xi_cc++ -> Xi_c+ pi+]cc', _strCutComb, _strCutMoth2)

        ## Construct charge violating combinations.
        self.combineXicc3WC = makeXicc(name+'CombineXicc3WC', [ self.combineXicZero, self.dauPi ], '[Xi_cc+ -> Xi_c0 pi-]cc', _strCutComb, _strCutMoth2)
        self.combineXicc4WC = makeXicc(name+'CombineXicc4WC', [ self.combineXicZero, self.dauPi ], '[Xi_cc++ -> Xi_c0 pi+ pi-]cc', _strCutComb, _strCutMoth3)
        self.combineXicc5WC = makeXicc(name+'CombineXicc5WC', [ self.combineXicPlus, self.dauPi ], '[Xi_cc+ -> Xi_c+ pi+ pi-]cc', _strCutComb, _strCutMoth3)
        self.combineXicc6WC = makeXicc(name+'CombineXicc6WC', [ self.combineXicPlus, self.dauPi ], '[Xi_cc++ -> Xi_c+ pi-]cc', _strCutComb, _strCutMoth2)



        # Control lines (to be prescaled!)
        #self.lineControl1 = StrippingLine(name+'ControlLc',
                                          #selection = self.filterLcForControl)
        #self.lineControl2 = StrippingLine(name+'ControlXicZero',
                                          #selection = self.combineXicZero)
        #self.lineControl3 = StrippingLine(name+'ControlXicPlus',
                                          #selection = self.combineXicPlus)

        self.selXicc1 = makeTisTos(name+'SelXiccPlusToLcKPi'
                                   , selection = self.combineXicc1
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc2 = makeTisTos(name+'SelXiccPlusPlusToLcKPiPi'
                                   , selection = self.combineXicc2
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc1WC = makeTisTos(name+'SelXiccPlusToLcKPiWC'
                                   , selection = self.combineXicc1WC
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc2WC = makeTisTos(name+'SelXiccPlusPlusToLcKPiPiWC'
                                   , selection = self.combineXicc2WC
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc1DCS = makeTisTos(name+'SelXiccPlusToLcKPiDCS'
                                   , selection = self.combineXicc1DCS
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc2DCS = makeTisTos(name+'SelXiccPlusPlusToLcKPiPiDCS'
                                   , selection = self.combineXicc2DCS
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc1a = makeTisTos(name+'SelXiccPlusToDpPK'
                                    , selection = self.combineXicc1a
                                    , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc2a = makeTisTos(name+'SelXiccPlusPlusToDpPKPi'
                                    , selection = self.combineXicc2a
                                    , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc1aWC = makeTisTos(name+'SelXiccPlusToDpPKWC'
                                    , selection = self.combineXicc1aWC
                                    , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc2aWC = makeTisTos(name+'SelXiccPlusPlusToDpPKPiWC'
                                    , selection = self.combineXicc2aWC
                                    , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc2aWC0 = makeTisTos(name+'SelXiccPlusPlusToDpPKPiWC0'
                                    , selection = self.combineXicc2aWC0
                                    , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc3 = makeTisTos(name+'SelXiccPlusToXicZeroPi'
                                   , selection = self.combineXicc3
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc4 = makeTisTos(name+'SelXiccPlusPlusToXicZeroPiPi'
                                   , selection = self.combineXicc4
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc5 = makeTisTos(name+'SelXiccPlusToXicPlusPiPi'
                                   , selection = self.combineXicc5
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc6 = makeTisTos(name+'SelXiccPlusPlusToXicPlusPi'
                                   , selection = self.combineXicc6
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc3WC = makeTisTos(name+'SelXiccPlusToXicZeroPiWC'
                                   , selection = self.combineXicc3WC
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc4WC = makeTisTos(name+'SelXiccPlusPlusToXicZeroPiPiWC'
                                   , selection = self.combineXicc4WC
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc5WC = makeTisTos(name+'SelXiccPlusToXicPlusPiPiWC'
                                   , selection = self.combineXicc5WC
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc6WC = makeTisTos(name+'SelXiccPlusPlusToXicPlusPiWC'
                                   , selection = self.combineXicc6WC
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc7 = makeTisTos(name+'SelXiccPlusToD0PKPi'
                                   , selection = self.combineXicc7
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc8 = makeTisTos(name+'SelXiccPlusPlusToD0PKPiPi'
                                   , selection = self.combineXicc8
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc7WC = makeTisTos(name+'SelXiccPlusToD0PKPiWC'
                                   , selection = self.combineXicc7WC
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc8WC = makeTisTos(name+'SelXiccPlusPlusToD0PKPiPiWC'
                                   , selection = self.combineXicc8WC
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc7DCS = makeTisTos(name+'SelXiccPlusToD0PKPiDCS'
                                   , selection = self.combineXicc7DCS
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])
        self.selXicc8DCS = makeTisTos(name+'SelXiccPlusPlusToD0PKPiPiDCS'
                                   , selection = self.combineXicc8DCS
                                   , hltTisTosSpec = config['Hlt2TisTosSpec'])





        # Control lines (to be prescaled!)
        self.lineControl1 = StrippingLine(name+'ControlLc',
                                          prescale = config['controlPrescaleLc'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.filterLcForControl)
        self.lineControl2 = StrippingLine(name+'ControlXicZero',
                                          prescale = config['controlPrescaleXic'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.combineXicZero)
        self.lineControl3 = StrippingLine(name+'ControlXicPlus',
                                          prescale = config['controlPrescaleXic'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.combineXicPlus)
        self.lineControl4 = StrippingLine(name+'ControlDp',
                                          prescale = config['controlPrescaleDp'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.dplus)
        self.lineControl5 = StrippingLine(name+'ControlD0',
                                          prescale = config['controlPrescaleD0'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.dzero)
        self.lineControl6 = StrippingLine(name+'ControlDsp',
                                          prescale = config['controlPrescaleDsp'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.dsplus)

        # Physics lines
        self.lineXicc1 = StrippingLine(name+'XiccPlusToLcKPi',
                                      prescale = config['signalPrescaleViaLc'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc1)
        self.lineXicc2 = StrippingLine(name+'XiccPlusPlusToLcKPiPi',
                                      prescale = config['signalPrescaleViaLc'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc2)
        self.lineXicc1WC = StrippingLine(name+'XiccPlusToLcKPiWC',
                                      prescale = config['signalPrescaleViaLcWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc1WC)
        self.lineXicc2WC = StrippingLine(name+'XiccPlusPlusToLcKPiPiWC',
                                      prescale = config['signalPrescaleViaLcWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc2WC)
        self.lineXicc1DCS = StrippingLine(name+'XiccPlusToLcKPiDCS',
                                      prescale = config['signalPrescaleViaLcDCS'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc1DCS)
        self.lineXicc2DCS = StrippingLine(name+'XiccPlusPlusToLcKPiPiDCS',
                                      prescale = config['signalPrescaleViaLcDCS'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc2DCS)
        self.lineXicc1a = StrippingLine(name+'XiccPlusToDpPK',
                                      prescale = config['signalPrescaleViaDp'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc1a)
        self.lineXicc2a = StrippingLine(name+'XiccPlusPlusToDpPKPi',
                                      prescale = config['signalPrescaleViaDp'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc2a)
        self.lineXicc1aWC = StrippingLine(name+'XiccPlusToDpPKWC',
                                      prescale = config['signalPrescaleViaDpWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc1aWC)
        self.lineXicc2aWC = StrippingLine(name+'XiccPlusPlusToDpPKPiWC',
                                      prescale = config['signalPrescaleViaDpWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc2aWC)
        self.lineXicc2aWC0 = StrippingLine(name+'XiccPlusPlusToDpPKPiWC0',
                                      prescale = config['signalPrescaleViaDpWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc2aWC0)
        self.lineXicc3 = StrippingLine(name+'XiccPlusToXicZeroPi',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc3)
        self.lineXicc4 = StrippingLine(name+'XiccPlusPlusToXicZeroPiPi',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc4)
        self.lineXicc5 = StrippingLine(name+'XiccPlusToXicPlusPiPi',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc5)
        self.lineXicc6 = StrippingLine(name+'XiccPlusPlusToXicPlusPi',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc6)
        self.lineXicc3WC = StrippingLine(name+'XiccPlusToXicZeroPiWC',
                                      prescale = config['signalPrescaleViaXicWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc3WC)
        self.lineXicc4WC = StrippingLine(name+'XiccPlusPlusToXicZeroPiPiWC',
                                      prescale = config['signalPrescaleViaXicWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc4WC)
        self.lineXicc5WC = StrippingLine(name+'XiccPlusToXicPlusPiPiWC',
                                      prescale = config['signalPrescaleViaXicWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc5WC)
        self.lineXicc6WC = StrippingLine(name+'XiccPlusPlusToXicPlusPiWC',
                                      prescale = config['signalPrescaleViaXicWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc6WC)
        self.lineXicc7 = StrippingLine(name+'XiccPlusToD0PKPi',
                                      prescale = config['signalPrescaleViaD0'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc7)
        self.lineXicc8 = StrippingLine(name+'XiccPlusPlusToD0PKPiPi',
                                      prescale = config['signalPrescaleViaD0'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc8)
        self.lineXicc7WC = StrippingLine(name+'XiccPlusToD0PKPiWC',
                                      prescale = config['signalPrescaleViaD0WC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc7WC)
        self.lineXicc8WC = StrippingLine(name+'XiccPlusPlusToD0PKPiPiWC',
                                      prescale = config['signalPrescaleViaD0WC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc8WC)
        self.lineXicc7DCS = StrippingLine(name+'XiccPlusToD0PKPiDCS',
                                      prescale = config['signalPrescaleViaD0DCS'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc7DCS)
        self.lineXicc8DCS = StrippingLine(name+'XiccPlusPlusToD0PKPiPiDCS',
                                      prescale = config['signalPrescaleViaD0DCS'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.selXicc8DCS)

        self.registerLine(self.lineControl1)
        self.registerLine(self.lineControl2)
        self.registerLine(self.lineControl3)
        self.registerLine(self.lineControl4)
        self.registerLine(self.lineControl5)
        self.registerLine(self.lineControl6)
        self.registerLine(self.lineXicc1)
        self.registerLine(self.lineXicc2)
        self.registerLine(self.lineXicc1WC)
        self.registerLine(self.lineXicc2WC)
        self.registerLine(self.lineXicc1DCS)
        self.registerLine(self.lineXicc2DCS)
        self.registerLine(self.lineXicc1a)
        self.registerLine(self.lineXicc2a)
        self.registerLine(self.lineXicc1aWC)
        self.registerLine(self.lineXicc2aWC)
        self.registerLine(self.lineXicc2aWC0)
        self.registerLine(self.lineXicc3)
        self.registerLine(self.lineXicc4)
        self.registerLine(self.lineXicc5)
        self.registerLine(self.lineXicc6)
        self.registerLine(self.lineXicc3WC)
        self.registerLine(self.lineXicc4WC)
        self.registerLine(self.lineXicc5WC)
        self.registerLine(self.lineXicc6WC)
        self.registerLine(self.lineXicc7)
        self.registerLine(self.lineXicc8)
        self.registerLine(self.lineXicc7WC)
        self.registerLine(self.lineXicc8WC)
        self.registerLine(self.lineXicc7DCS)
        self.registerLine(self.lineXicc8DCS)

def mergeLists(localName, inputSelections) :
    return MergedSelection ( localName,
                             RequiredSelections = inputSelections )

def makeLc(localName, configDict = _my_immutable_config) :
    ## Pick up standard Lambdac -> p K- pi+
    _LcKaons = DataOnDemand(Location = configDict['Lc_K_InputList'])
    _LcPions = DataOnDemand(Location = configDict['Lc_Pi_InputList'])
    _LcProtons = DataOnDemand(Location = configDict['Lc_P_InputList'])

    ## Daughter cuts
    _strCutAllDaug = "(TRCHI2DOF<%(Lc_Daug_TRCHI2DOF_Max)s)" \
                     "& (PT>%(Lc_Daug_PT_Min)s)" \
                     "& (P>%(Lc_Daug_P_Min)s)" % configDict
    _strCutK  = _strCutAllDaug + '&' + "(HASRICH)&((PIDK - PIDpi)>%(Lc_K_PIDKPIDpi_Min)s)" % configDict
    _strCutpi = _strCutAllDaug + '&' + "(HASRICH)&((PIDpi - PIDK)>%(Lc_Pi_PIDpiPIDK_Min)s)" % configDict
    _strCutp  = _strCutAllDaug + '&' + "(HASRICH)&((PIDp - PIDpi)>%(Lc_P_PIDpPIDpi_Min)s)&((PIDp-PIDK)>%(Lc_P_PIDpPIDK_Min)s)" % configDict

    _daughterCuts = { 'pi+' : _strCutpi,
                      'K-'  : _strCutK,
                      'p+'  : _strCutp }

    ## Combination cuts
    _strCutComb = "(ADAMASS('Lambda_c+')<1.1*%(Lc_ADMASS_HalfWin)s)" \
                  "& (AMAXCHILD(MIPCHI2DV(PRIMARY))>%(Lc_Daug_1of3_MIPCHI2DV_Min)s)" \
                  "& (ADOCAMAX('')<%(Lc_ADOCAMAX_Max)s)" \
                  "& (APT>%(Lc_APT_Min)s)" % configDict

    _strCutMoth = "(VFASPF(VCHI2) < %(Lc_VCHI2_Max)s)" \
                  "& (ADMASS('Lambda_c+')<%(Lc_ADMASS_HalfWin)s)" \
                  "& (BPVVDCHI2>%(Lc_BPVVDCHI2_Min)s)" \
                  "& (BPVDIRA>%(Lc_BPVDIRA_Min)s)" % configDict

    _combineLambdac2PKPi = CombineParticles(
        DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc",
        DaughtersCuts = _daughterCuts,
        CombinationCut = _strCutComb,
        MotherCut = _strCutMoth
    )

    return Selection ( localName,
                       Algorithm = _combineLambdac2PKPi,
                       RequiredSelections = [ _LcKaons, _LcPions, _LcProtons ] )


def filterLcForControl(localName, inputSel, configDict = _my_immutable_config) :
    # Apply additional cuts for prompt Lc:
    _strCutDIRA = "( BPVDIRA > %(LcControl_BPVDIRA_Min)s)" % configDict
    _filterLc = FilterDesktop(Code = _strCutDIRA)
    return Selection ( localName,
                       Algorithm = _filterLc,
                       RequiredSelections = [ inputSel ] )


def filterKaons(localName, configDict = _my_immutable_config) :
    # Pick up standard input list
    _stdK = DataOnDemand(Location = configDict['Xicc_K_InputList'])
    # Filter:
    _strCutDauK = "(TRCHI2DOF<%(Xicc_Daug_TRCHI2DOF_Max)s)" \
                   "& (P>%(Xicc_Daug_P_Min)s)" \
                   "& (HASRICH)&(PIDK-PIDpi>%(Xicc_K_PIDKPIDpi_Min)s)" \
                   "& (PT>%(Xicc_Daug_PT_Min)s)" \
                   "& (MIPCHI2DV(PRIMARY)>%(Xicc_Daug_MIPCHI2DV_Min)s)" % configDict

    _filterK = FilterDesktop(Code = _strCutDauK)
    return Selection ( localName,
                       Algorithm = _filterK,
                       RequiredSelections = [ _stdK ] )


def filterPions(localName, configDict = _my_immutable_config) :
    # Pick up standard input list
    _stdPi = DataOnDemand(Location = configDict['Xicc_Pi_InputList'])
    # Filter:
    _strCutDauPi = "(TRCHI2DOF<%(Xicc_Daug_TRCHI2DOF_Max)s)" \
                   "& (P>%(Xicc_Daug_P_Min)s)" \
                   "& (HASRICH)&(PIDpi-PIDK>%(Xicc_Pi_PIDpiPIDK_Min)s)" \
                   "& (PT>%(Xicc_Daug_PT_Min)s)" \
                   "& (MIPCHI2DV(PRIMARY)>%(Xicc_Daug_MIPCHI2DV_Min)s)" % configDict

    _filterPi = FilterDesktop(Code = _strCutDauPi)
    return Selection ( localName,
                       Algorithm = _filterPi,
                       RequiredSelections = [ _stdPi ] )


def filterProtons(localName, configDict = _my_immutable_config) :
    # Pick up standard input list
    _stdP = DataOnDemand(Location = configDict['Xicc_P_InputList'])
    # Filter:
    _strCutDauP  = "(TRCHI2DOF<%(Xicc_Daug_TRCHI2DOF_Max)s)" \
                   "& (P>%(Xicc_Daug_P_Min)s)" \
                   "& (HASRICH)&(PIDp-PIDpi>%(Xicc_P_PIDpPIDpi_Min)s)" \
                   "& (HASRICH)&(PIDp-PIDK>%(Xicc_P_PIDpPIDK_Min)s)" \
                   "& (PT>%(Xicc_Daug_PT_Min)s)" \
                   "& (MIPCHI2DV(PRIMARY)>%(Xicc_Daug_MIPCHI2DV_Min)s)" % configDict

    _filterP  = FilterDesktop(Code = _strCutDauP)
    return Selection ( localName,
                       Algorithm = _filterP,
                       RequiredSelections = [ _stdP ] )


def filterDplus(localName, configDict = _my_immutable_config) :
    # Pick up standard input list
    _stdDplus = DataOnDemand(Location = configDict['Xicc_Dp_InputList'])
    # Filter:
    _strCutDplus = "(ADMASS('D+')<%(Dp_ADMASS_HalfWin)s)" \
                   "& (BPVVDCHI2>%(Dp_BPVVDCHI2_Min)s)" % configDict

    _filterDplus = FilterDesktop(Code = _strCutDplus)
    return Selection ( localName,
                       Algorithm = _filterDplus,
                       RequiredSelections = [ _stdDplus ] )


def filterDsplus(localName, configDict = _my_immutable_config) :
    # Pick up standard input list
    _stdDsplus = DataOnDemand(Location = configDict['Xicc_Dsp_InputList'])
    # Filter:
    _strCutDsplus = "(ADMASS('D_s+')<%(Dsp_ADMASS_HalfWin)s)" \
                    "& (BPVVDCHI2>%(Dsp_BPVVDCHI2_Min)s)" % configDict

    _filterDsplus = FilterDesktop(Code = _strCutDsplus)
    return Selection ( localName,
                       Algorithm = _filterDsplus,
                       RequiredSelections = [ _stdDsplus ] )


def filterD0(localName, configDict = _my_immutable_config) :
    # Pick up standard input list
    _stdD0 = DataOnDemand(Location = configDict['Xicc_D0_InputList'])
    # Filter:
    _strCutD0 = "(ADMASS('D0')<%(D0_ADMASS_HalfWin)s)" \
                "& (BPVVDCHI2>%(D0_BPVVDCHI2_Min)s)" % configDict

    _filterD0 = FilterDesktop(Code = _strCutD0)
    return Selection ( localName,
                       Algorithm = _filterD0,
                       RequiredSelections = [ _stdD0 ] )


def makeXi(localName, inputList, cutWide, cutTight, configDict = _my_immutable_config) :
    _stdPi = DataOnDemand(Location = configDict['Xi_Pi_InputList'])

    _strCutPiForXi = "(P>%(Xi_Pi_P_Min)s)" \
                     "& (TRCHI2DOF<%(Xi_Pi_TRCHI2DOF_Max)s)" \
                     "& (PT>%(Xi_Pi_PT_Min)s)" \
                     "& (MIPCHI2DV(PRIMARY)>%(Xi_Pi_MIPCHI2DV_Min)s)" % configDict
    _strCutCombXi = "( ADAMASS('Xi-') < %(cutWide)s * MeV )" % locals()
    _strCutMothMass = "( ADMASS('Xi-') < %(cutTight)s * MeV )" % locals()
    _strCutMothChi2 = "(VFASPF(VCHI2)<%(Xi_VCHI2_Max)s)" % configDict
    _strCutMothXi = _strCutMothMass + '&' + _strCutMothChi2
    _combineXi = CombineParticles( DecayDescriptor = '[Xi- -> Lambda0 pi-]cc',
                                   DaughtersCuts = { "pi-": _strCutPiForXi },
                                   CombinationCut = _strCutCombXi,
                                   MotherCut = _strCutMothXi )

    return Selection ( localName,
                       Algorithm = _combineXi,
                       RequiredSelections = [ inputList, _stdPi ] )


def makeXicc(localName, inputSelections, decay, cutComb, cutMoth) :
    _combineXicc = CombineParticles( DecayDescriptor = decay,
                                     CombinationCut = cutComb,
       	       	       	       	     MotherCut = cutMoth )
    return Selection ( localName,
                       Algorithm = _combineXicc,
                       RequiredSelections = inputSelections)

def makeXicZero(localName, inputSelections, configDict = _my_immutable_config) :
    _strCutCombXicZero = "( ADAMASS('Xi_c0') < %(Xic0_ADAMASS_HalfWin)s )" % configDict
    _strCutMothXicZero = "(ADMASS('Xi_c0') < %(Xic0_ADMASS_HalfWin)s)" \
                         "& (VFASPF(VCHI2)<%(Xic0_VCHI2_Max)s)" \
                         "& (BPVVDCHI2 > %(Xic0_BPVDIRA_Min)s )" \
                         "& (BPVDIRA > %(Xic0_BPVDIRA_Min)s)" % configDict

    _combineXicZero = CombineParticles( DecayDescriptor = '[Xi_c0 -> Xi- pi+]cc',
                                        CombinationCut = _strCutCombXicZero,
                                        MotherCut = _strCutMothXicZero )
    return Selection( localName,
                      Algorithm = _combineXicZero,
                      RequiredSelections = inputSelections )


def makeXicPlus(localName, inputSelections, configDict = _my_immutable_config) :
    _strCutCombXicPlus = "(( ADAMASS('Xi_c+') < %(Xicplus_ADAMASS_HalfWin)s )" \
                         "& ( AHASCHILD( (ABSID == 'pi+') & (MIPCHI2DV(PRIMARY) > %(Xicplus_1of2_MIPCHI2DV_Min)s) ) ) )" % configDict

    _strCutMothXicPlus = "( ADMASS('Xi_c+') < %(Xicplus_ADMASS_HalfWin)s )" \
                         "& (VFASPF(VCHI2)<%(Xicplus_VCHI2_Max)s)" \
                         "& (BPVVDCHI2 > %(Xicplus_BPVVDCHI2_Min)s)" \
                         "& (BPVDIRA > %(Xicplus_BPVDIRA_Min)s)" % configDict

    _combineXicPlus = CombineParticles( DecayDescriptor = '[Xi_c+ -> Xi- pi+ pi+]cc',
                                        CombinationCut = _strCutCombXicPlus,
                                        MotherCut = _strCutMothXicPlus )

    return Selection(localName,
                      Algorithm = _combineXicPlus,
                      RequiredSelections = inputSelections )


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

