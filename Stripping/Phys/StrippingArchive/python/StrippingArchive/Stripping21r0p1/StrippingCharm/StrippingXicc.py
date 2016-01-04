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

__author__ = ['Patrick Spradlin', 'Mat Charles']
__date__ = '2015 May 13'
__version__ = '$Revision: 1.1 $'

__all__ = ( 'XiccBuilder'               ## LineBuilder class specialization
            , 'default_name'            ## Default name for XiccBuilder object
            , 'default_config'          ## Default config for XiccBuilder object
            , 'makeLc'                  ## Lambda_c+ selection
            , 'makeXi'                  ## Xi selection
            , 'makeXicZero'             ## Xi_c^0 selection
            , 'makeXicPlusToXiPiPi'     ## Xi_c^+ selection
            , 'makeXicPlusToPKPi'       ## Xi_c^+ selection
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


default_config = {
    'NAME'  : 'Xicc'
    , 'WGs'   : [ 'Charm' ]
    , 'BUILDERTYPE' : 'XiccBuilder'
    , 'CONFIG' : { 'LongTrackGEC' : 150
                   , 'controlPrescaleLc'       : 0.05
                   , 'controlPrescaleXic'      : 0.05
                   , 'controlPrescaleDp'       : 0.01
                   , 'controlPrescaleD0'       : 0.01
                   , 'controlPrescaleDsp'      : 0.01
                   , 'signalPrescaleViaLc'     : 1.0
                   , 'signalPrescaleViaLcWC'   : 1.0
                   , 'signalPrescaleViaLcDCS'  : 1.0
                   , 'signalPrescaleViaXic'    : 1.0
                   , 'signalPrescaleViaXicWC'  : 1.0
                   , 'signalPrescaleViaDp'     : 1.0
                   , 'signalPrescaleViaDpWC'   : 1.0
                   , 'signalPrescaleViaD0'     : 1.0
                   , 'signalPrescaleViaD0WC'   : 1.0
                   , 'signalPrescaleViaD0DCS'  : 1.0
                   , 'LcHlt2TisTosSpec' : { 'Hlt2CharmHadLcpToPpKmPip.*Decision%TOS' : 0, 'Hlt2Global%TIS' : 0 }
                   , 'DzHlt2TisTosSpec' : { 'Hlt2CharmHad.*Decision%TOS' : 0, 'Hlt2Global.*%TIS' : 0 }
                   , 'DpHlt2TisTosSpec' : { 'Hlt2CharmHadDpToKmPipPip.*Decision%TOS' : 0, 'Hlt2Global%TIS' : 0 }
                   , 'XiHlt2TisTosSpec' : { 'Hlt2CharmHadXim2LamPim.*Decision%TOS' : 0, 'Hlt2Global%TIS' : 0 }
                 }
    , 'STREAMS' : [ 'Charm' ]
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
                               , 'LcHlt2TisTosSpec'
                               , 'DzHlt2TisTosSpec'
                               , 'DpHlt2TisTosSpec'
                               , 'XiHlt2TisTosSpec' )


    ## Possible parameters and default values copied from the definition
    ##   of StrippingLine
    def _strippingLine ( self,
                          name             ,   # the base name for the Line
                          prescale  = 1.0  ,   # prescale factor
                          ODIN      = None ,   # ODIN predicate
                          L0DU      = None ,   # L0DU predicate
                          HLT1      = None ,   # HltDecReports predicate
                          HLT2      = None ,   # HltDecReports predicate
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
                                  HLT1            = HLT1,
                                  HLT2            = HLT2,
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

        # Optionally, apply additional ghost-prob filtering:
        self.dauPi_GP = filterGhosts(name+'FilteredPionsGP', self.dauPi)
        self.dauK_GP = filterGhosts(name+'FilteredKaonsGP', self.dauK)
        self.dauP_GP = filterGhosts(name+'FilteredProtonsGP', self.dauP)

        # Pick up standard Lambdac -> p K- pi+ then filter it to reduce rate:
        self.filterLc = makeLc(name+'FilterLc')
        self.filterLcTisTos = makeTisTos(name+"filterLcTisTos", selection = self.filterLc, hltTisTosSpec = config['LcHlt2TisTosSpec'])
        self.filterLcForControl = filterLcForControl(name+'FilterLcForControl', self.filterLcTisTos)

        # Pick up standard D+ -> K- pi+ pi+ then filter it to reduce rate:
        self.dplus  = filterDplus(name+'FilteredDplus2KPiPi')
        self.dplusTisTos = makeTisTos(name+"dplusTisTos", selection = self.dplus, hltTisTosSpec = config['DpHlt2TisTosSpec'])

        # Pick up standard D_s+ -> K- K+ pi+ then filter it to reduce rate:
        self.dsplus = filterDsplus(name+'FilteredDsplus2KKPi')
        self.dsplusTisTos = makeTisTos(name+"dsplusTisTos", selection = self.dsplus, hltTisTosSpec = config['DpHlt2TisTosSpec'])

        # Pick up standard D0 -> K- pi+ then filter it to reduce rate:
        self.dzero = filterD0(name+'FilteredD02KPi')
        self.dzeroTisTos = makeTisTos(name+"dzeroTisTos", selection = self.dzero, hltTisTosSpec = config['DzHlt2TisTosSpec'])

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
        # Apply TISTOS filter
        self.combineXiTisTos = makeTisTos(name+"CombineXiTisTos", selection = self.combineXi, hltTisTosSpec = config['XiHlt2TisTosSpec'])

        # Combine Xi- with pion(s) to make Xic0, Xic+
        self.combineXicZero = makeXicZero(name+"CombineXicZero", [ self.combineXiTisTos, self.dauPi ])
        self.combineXicPlusToXiPiPi = makeXicPlusToXiPiPi(name+"CombineXicPlusToXiPiPi", [ self.combineXiTisTos, self.dauPi ])
        #self.combineXicPlusToPKPi   = makeXicPlusToPKPi(name+"CombineXicPlusToPKPi", [ self.dauP_GP, self.dauK_GP, self.dauPi_GP ])
        #self.combineXicPlus = mergeLists(name+'CombineXicPlus', [ self.combineXicPlusToPKPi, self.combineXicPlusToXiPiPi ])
        self.combineXicPlus = self.combineXicPlusToXiPiPi

        # Combine Lc+ with a K and a pi to make a Xicc+ or Xicc++:
        self.combineXicc1 = makeXicc(name+'CombineXicc1', [ self.filterLcTisTos, self.dauPi, self.dauK ], '[Xi_cc+ -> Lambda_c+ K- pi+]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2 = makeXicc(name+'CombineXicc2', [ self.filterLcTisTos, self.dauPi, self.dauK ], '[Xi_cc++ -> Lambda_c+ K- pi+ pi+]cc', _strCutComb, _strCutMoth4)
        ## Construct charge violating combinations.
        self.combineXicc1WC = makeXicc(name+'CombineXicc1WC', [ self.filterLcTisTos, self.dauPi, self.dauK ], '[Xi_cc+ -> Lambda_c+ K- pi-]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2WC = makeXicc(name+'CombineXicc2WC', [ self.filterLcTisTos, self.dauPi, self.dauK ], '[Xi_cc++ -> Lambda_c+ K- pi+ pi-]cc', _strCutComb, _strCutMoth4)

        ## Construct DCS combinations (background modelling check)
        self.combineXicc1DCS = makeXicc(name+'CombineXicc1DCS', [ self.filterLcTisTos, self.dauPi, self.dauK ], '[Xi_cc+ -> Lambda_c+ K+ pi-]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2DCS = makeXicc(name+'CombineXicc2DCS', [ self.filterLcTisTos, self.dauPi, self.dauK ], '[Xi_cc++ -> Lambda_c+ K+ pi- pi+]cc', _strCutComb, _strCutMoth4)


        # Combine D+ with a K and a p to make a Xicc+ or Xicc++:
        self.combineXicc1a = makeXicc(name+'CombineXicc1a', [ self.dplusTisTos, self.dauP, self.dauK ], '[Xi_cc+ -> D+ p+ K-]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2a = makeXicc(name+'CombineXicc2a', [ self.dplusTisTos, self.dauP, self.dauK, self.dauPi ], '[Xi_cc++ -> D+ p+ K- pi+]cc', _strCutComb, _strCutMoth4)

        ## Construct charge violating combinations.
        self.combineXicc1aWC = makeXicc(name+'CombineXicc1aWC', [ self.dplusTisTos, self.dauP, self.dauK ], '[Xi_cc+ -> D+ p+ K+]cc', _strCutComb, _strCutMoth3)
        self.combineXicc2aWC = makeXicc(name+'CombineXicc2aWC', [ self.dplusTisTos, self.dauP, self.dauK, self.dauPi ], '[Xi_cc++ -> D+ p+ K+ pi+]cc', _strCutComb, _strCutMoth4)
        self.combineXicc2aWC0 = makeXicc(name+'CombineXicc2aWC0', [ self.dplusTisTos, self.dauP, self.dauK, self.dauPi ], '[Xi_cc++ -> D+ p+ K- pi-]cc', _strCutComb, _strCutMoth4)


        # Combine D0 with a K, a p and a pi to make a Xicc+ or Xicc++:
        # NB 5-body decays (#8) have the ghost prob cut applied
        self.combineXicc7 = makeXicc(name+'CombineXicc7', [ self.dzeroTisTos, self.dauP, self.dauK, self.dauPi ], '[Xi_cc+ -> D0 p+ K- pi+]cc', _strCutComb, _strCutMoth4)
        self.combineXicc8 = makeXicc(name+'CombineXicc8', [ self.dzeroTisTos, self.dauP_GP, self.dauK_GP, self.dauPi_GP ], '[Xi_cc++ -> D0 p+ K- pi+ pi+]cc', _strCutComb, _strCutMoth4)

        ## Construct charge violating combinations.
        ## NB 5-body decays (#8) have the ghost prob cut applied
        self.combineXicc7WC = makeXicc(name+'CombineXicc7WC', [ self.dzeroTisTos, self.dauP, self.dauK, self.dauPi ], '[Xi_cc+ -> D0 p+ K- pi-]cc', _strCutComb, _strCutMoth4)
        self.combineXicc8WC = makeXicc(name+'CombineXicc8WC', [ self.dzeroTisTos, self.dauP_GP, self.dauK_GP, self.dauPi_GP ], '[Xi_cc++ -> D0 p+ K- pi+ pi-]cc', _strCutComb, _strCutMoth4)

        ## Construct DCS combinations (background modelling check)
        ## NB 5-body decays (#8) have the ghost prob cut applied
        self.combineXicc7DCS = makeXicc(name+'CombineXicc7DCS', [ self.dzeroTisTos, self.dauP, self.dauK, self.dauPi ], '[Xi_cc+ -> D0 p+ K+ pi-]cc', _strCutComb, _strCutMoth4)
        self.combineXicc8DCS = makeXicc(name+'CombineXicc8DCS', [ self.dzeroTisTos, self.dauP_GP, self.dauK_GP, self.dauPi_GP ], '[Xi_cc++ -> D0 p+ K+ pi+ pi-]cc', _strCutComb, _strCutMoth4)


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
        self.lineControl1 = self._strippingLine(name = name+'ControlLc',
                                          prescale = config['controlPrescaleLc'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.filterLcForControl)
        self.lineControl2 = self._strippingLine(name = name+'ControlXicZero',
                                          prescale = config['controlPrescaleXic'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.combineXicZero)
        self.lineControl3 = self._strippingLine(name = name+'ControlXicPlus',
                                          prescale = config['controlPrescaleXic'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.combineXicPlus)
        self.lineControl4 = self._strippingLine(name = name+'ControlDp',
                                          prescale = config['controlPrescaleDp'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.dplusTisTos)
        self.lineControl5 = self._strippingLine(name = name+'ControlD0',
                                          prescale = config['controlPrescaleD0'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.dzeroTisTos)
        self.lineControl6 = self._strippingLine(name = name+'ControlDsp',
                                          prescale = config['controlPrescaleDsp'],
                                          postscale = 1.0,
                                          FILTER = _globalEventCuts,
                                          selection = self.dsplusTisTos)

        # Physics lines
        self.lineXicc1 = self._strippingLine(name = name+'XiccPlusToLcKPi',
                                      prescale = config['signalPrescaleViaLc'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc1)
        self.lineXicc2 = self._strippingLine(name = name+'XiccPlusPlusToLcKPiPi',
                                      prescale = config['signalPrescaleViaLc'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc2)
        self.lineXicc1WC = self._strippingLine(name = name+'XiccPlusToLcKPiWC',
                                      prescale = config['signalPrescaleViaLcWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc1WC)
        self.lineXicc2WC = self._strippingLine(name = name+'XiccPlusPlusToLcKPiPiWC',
                                      prescale = config['signalPrescaleViaLcWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc2WC)
        self.lineXicc1DCS = self._strippingLine(name = name+'XiccPlusToLcKPiDCS',
                                      prescale = config['signalPrescaleViaLcDCS'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc1DCS)
        self.lineXicc2DCS = self._strippingLine(name = name+'XiccPlusPlusToLcKPiPiDCS',
                                      prescale = config['signalPrescaleViaLcDCS'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc2DCS)
        self.lineXicc1a = self._strippingLine(name = name+'XiccPlusToDpPK',
                                      prescale = config['signalPrescaleViaDp'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc1a)
        self.lineXicc2a = self._strippingLine(name = name+'XiccPlusPlusToDpPKPi',
                                      prescale = config['signalPrescaleViaDp'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc2a)
        self.lineXicc1aWC = self._strippingLine(name = name+'XiccPlusToDpPKWC',
                                      prescale = config['signalPrescaleViaDpWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc1aWC)
        self.lineXicc2aWC = self._strippingLine(name = name+'XiccPlusPlusToDpPKPiWC',
                                      prescale = config['signalPrescaleViaDpWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc2aWC)
        self.lineXicc2aWC0 = self._strippingLine(name = name+'XiccPlusPlusToDpPKPiWC0',
                                      prescale = config['signalPrescaleViaDpWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc2aWC0)
        self.lineXicc3 = self._strippingLine(name = name+'XiccPlusToXicZeroPi',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc3)
        self.lineXicc4 = self._strippingLine(name = name+'XiccPlusPlusToXicZeroPiPi',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc4)
        self.lineXicc5 = self._strippingLine(name = name+'XiccPlusToXicPlusPiPi',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc5)
        self.lineXicc6 = self._strippingLine(name = name+'XiccPlusPlusToXicPlusPi',
                                      prescale = config['signalPrescaleViaXic'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc6)
        self.lineXicc3WC = self._strippingLine(name = name+'XiccPlusToXicZeroPiWC',
                                      prescale = config['signalPrescaleViaXicWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc3WC)
        self.lineXicc4WC = self._strippingLine(name = name+'XiccPlusPlusToXicZeroPiPiWC',
                                      prescale = config['signalPrescaleViaXicWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc4WC)
        self.lineXicc5WC = self._strippingLine(name = name+'XiccPlusToXicPlusPiPiWC',
                                      prescale = config['signalPrescaleViaXicWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc5WC)
        self.lineXicc6WC = self._strippingLine(name = name+'XiccPlusPlusToXicPlusPiWC',
                                      prescale = config['signalPrescaleViaXicWC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc6WC)
        self.lineXicc7 = self._strippingLine(name = name+'XiccPlusToD0PKPi',
                                      prescale = config['signalPrescaleViaD0'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc7)
        self.lineXicc8 = self._strippingLine(name = name+'XiccPlusPlusToD0PKPiPi',
                                      prescale = config['signalPrescaleViaD0'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc8)
        self.lineXicc7WC = self._strippingLine(name = name+'XiccPlusToD0PKPiWC',
                                      prescale = config['signalPrescaleViaD0WC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc7WC)
        self.lineXicc8WC = self._strippingLine(name = name+'XiccPlusPlusToD0PKPiPiWC',
                                      prescale = config['signalPrescaleViaD0WC'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc8WC)
        self.lineXicc7DCS = self._strippingLine(name = name+'XiccPlusToD0PKPiDCS',
                                      prescale = config['signalPrescaleViaD0DCS'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc7DCS)
        self.lineXicc8DCS = self._strippingLine(name = name+'XiccPlusPlusToD0PKPiPiDCS',
                                      prescale = config['signalPrescaleViaD0DCS'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self.combineXicc8DCS)


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

def filterGhosts(localName, inputSelection) :
    _strCutGhost = '( TRGHOSTPROB < 0.5 )'
    _filter = FilterDesktop(Code = _strCutGhost)
    return Selection(localName,
                     Algorithm = _filter,
                     RequiredSelections = [ inputSelection ] )

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
                         "& (BPVVDCHI2 > %(Xic0_BPVVDCHI2_Min)s )" \
                         "& (BPVDIRA > %(Xic0_BPVDIRA_Min)s)" % configDict

    _combineXicZero = CombineParticles( DecayDescriptor = '[Xi_c0 -> Xi- pi+]cc',
                                        CombinationCut = _strCutCombXicZero,
                                        MotherCut = _strCutMothXicZero )
    return Selection( localName,
                      Algorithm = _combineXicZero,
                      RequiredSelections = inputSelections )


def makeXicPlusToXiPiPi(localName, inputSelections, configDict = _my_immutable_config) :
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

## NEW for Stripping 21
## This is Xic -> pKpi -- uses very similar cuts to Lc -> pKpi
def makeXicPlusToPKPi(localName, inputSelections, configDict = _my_immutable_config) :
    _strCutComb = "( ( ADAMASS('Xi_c+') < %(Xicplus_ADAMASS_HalfWin)s )" \
                  "& (AMAXCHILD(MIPCHI2DV(PRIMARY))>%(Lc_Daug_1of3_MIPCHI2DV_Min)s)" \
                  "& (ADOCAMAX('')<%(Lc_ADOCAMAX_Max)s)" \
                  "& (APT>%(Lc_APT_Min)s) )" % configDict

    _strCutMoth = "( (VFASPF(VCHI2) < %(Lc_VCHI2_Max)s)" \
                  "& ( ADMASS('Xi_c+') < %(Xicplus_ADMASS_HalfWin)s )" \
                  "& (BPVVDCHI2>%(Xicplus_BPVVDCHI2_Min)s)" \
                  "& (BPVDIRA>%(Xicplus_BPVDIRA_Min)s) )" % configDict

    _combineXicPlus = CombineParticles( DecayDescriptor = '[Xi_c+ -> p+ K- pi+]cc',
                                        CombinationCut = _strCutComb,
                                        MotherCut = _strCutMoth )

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

