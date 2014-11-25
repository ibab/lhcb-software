# $Id$
'''
Fully and partially reconstructed stripping lines for the following decay modes:

  B-    -> Xi_c(2815)+  anti-Sigma_c(2455)--
  B0bar -> Xi_c(2815)0  anti-Sigma_c(2455)0

where

  Xi_c(2815)+ -> Xi_c(2645)0 pi+
  Xi_c(2815)0 -> Xi_c(2645)+ pi-
  Xi_c(2645)+ -> Xi_c0 pi+
  Xi_c(2645)0 -> Xi_c+ pi-
  Sigma_c(2455)++ -> Lambda_c+ pi+
  Sigma_c(2455)0  -> Lambda_c+ pi-

and where the weak decay modes used are

  Xi_c0 -> Xi- pi+, Omega- K+, p K- K- pi+
  Xi_c+ -> Xi- pi+ pi+
  Lambda_c+ -> p K- pi+

The following partially reconstructed lines are included:

  B-    -> Xi_c(2815)+  pi-                (missing Lc-)
  B0bar -> Xi_c(2815)0  pi+                (missing Lc-)
  B-    -> pi+ pi-  anti-Sigma_c(2455)--   (missing Xic+)
  B0bar -> pi+ pi-  anti-Sigma_c(2455)0    (missing Xic0)

To represent the partial reconstruction, we use Delta states, i.e.

  B-    -> Delta0 Lc-,        Delta0 -> Xi_c(2815)+  pi-
  B0bar -> Delta+ Lc-,        Delta+ -> Xi_c(2815)0  pi+
  B-    -> anti-Delta-- Xic+, anti-Delta-- -> pi+ pi-  anti-Sigma_c(2455)--
  B0bar -> anti-Delta0  Xic0, anti-Delta0  -> pi+ pi-  anti-Sigma_c(2455)0

These are designed for studies of absolute branching fractions.

Useful short names from particle table:
  Lambda_c+, Lambda_c~-
  Sigma_c0, Sigma_c~0
  Sigma_c++, Sigma_c~--
  Xi_c0, Xi_c~0
  Xi_c+, Xi_c~-
  Xi_c*0, Xi_c*~0
  Xi_c*+, Xi_c*~-
  Xi_c(2815)+, Xi_c(2815)~-
  Xi_c(2815)0, Xi_c(2815)~0
  B0, B~0, B+, B-
  Delta-, Delta0, Delta+, Delta++
  Delta~+, Delta~0, Delta~-, Delta~--
'''

__author__ = ['Mat Charles']
__date__ = '22/Mar/2012'
__version__ = '$Revision: 1.0$'

__all__ = ( 'B2XicScBuilder'               ## LineBuilder class specialization
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


default_name = "B2XicScBuilder"

default_config = {
      'LongTrackGEC' : 250
    , 'signalPrescalePartial'  : 1.0
    , 'signalPrescaleFull'  : 1.0
}


## These configuration parameters should not be changed outside of this file.
_my_immutable_config = {
        'BasePions_InputList'           : 'Phys/StdLoosePions/Particles'
        , 'BaseKaons_InputList'         : 'Phys/StdLooseKaons/Particles'
        , 'BaseProtons_InputList'       : 'Phys/StdLooseProtons/Particles'
        , 'SoftPions_InputList'         : 'Phys/StdAllNoPIDsPions/Particles'
        , 'DownPions_InputList'         : 'Phys/StdNoPIDsDownPions/Particles'
        , 'DownKaons_InputList'         : 'Phys/StdLooseDownKaons/Particles'
        , 'BaseTrack_TRCHI2DOF_Max'     :   5.0
        , 'SoftTrack_TRCHI2DOF_Max'     :   5.0
        , 'DownTrack_TRCHI2DOF_Max'     :   5.0
        , 'BaseTrack_PT_Min'            : 200.0*MeV
        , 'SoftTrack_PT_Min'            : 100.0*MeV
        , 'DownTrack_PT_Min'            : 100.0*MeV
        , 'BaseTrack_P_Min'             :   2.0*GeV
        , 'SoftTrack_P_Min'             :   2.0*GeV
        , 'DownTrack_P_Min'             :   2.0*GeV
        , 'BaseTrack_MIPCHI2DV_Min'     :   4.0
        , 'SoftTrack_MIPCHI2DV_Min'     :   4.0
        , 'DownTrack_MIPCHI2DV_Min'     :  -1.0
        , 'BasePion_PIDpiPIDK_Min'      :   0.0
        , 'BaseKaon_PIDKPIDpi_Min'      :   5.0
        , 'BaseProton_PIDpPIDpi_Min'    :   5.0
        , 'BaseProton_PIDpPIDK_Min'     :   0.0
        , 'Lc_Daug_P_Min'               :   2.0*GeV
        , 'Lc_Daug_1of3_MIPCHI2DV_Min'  :   4.0
        , 'Lc_ADMASS_HalfWin'           :  75.0*MeV
        , 'Lc_APT_Min'                  :   1.0*GeV
        , 'Lc_ADOCAMAX_Max'             :   0.5*mm
        , 'Lc_BPVVDCHI2_Min'            :  16.0
        , 'Lc_BPVDIRA_Min'              :   0.9
        , 'Xi_LL_ADAMASS_HalfWin'       :  50.0*MeV
        , 'Xi_LL_ADMASS_HalfWin'        :  35.0*MeV
        , 'Xi_DD_ADAMASS_HalfWin'       :  80.0*MeV
        , 'Xi_DD_ADMASS_HalfWin'        :  50.0*MeV
        , 'Xic_ADAMASS_HalfWin'         : 170.0*MeV
        , 'Xic_ADMASS_HalfWin'          : 120.0*MeV
        , 'Xic_BPVDIRA_Min'             :   0.9
        , 'Xic_BPVVDCHI2_Min'           :  25.0
        , 'ChisqCutTwoBody'             :  15.0
        , 'ChisqCutThreeBody'           :  30.0
        , 'ChisqCutFourBody'            :  60.0
        , 'ExcitedBaryon_dm_Min'        : -20.0
        , 'ExcitedBaryon_dm_Max'        :  60.0
        , 'PartialB_BPVVDCHI2_Min'      :  25.0
        , 'PartialB_M_Max'              :   4.0*GeV
        }




class B2XicScBuilder(LineBuilder) :
    """
    Search for Xicc
    """

    __configuration_keys__ = ( 'LongTrackGEC'
                               , 'signalPrescalePartial'
                               , 'signalPrescaleFull'
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


    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCutsCode = "(recSummary (LHCb.RecSummary.nLongTracks, 'Rec/Track/Long') < %(LongTrackGEC)s )" % config
        _globalEventCuts = { 'Code' : _globalEventCutsCode, 'Preambulo' : ["from LoKiTracks.decorators import *"]}

        # Pick up standard kaons, pions. Filter with PID as appropriate.
        self.dauPi_init = filterPions(name+'FilteredPions')
        self.dauK_init  = filterKaons(name+'FilteredKaons')
        self.dauP_init  = filterProtons(name+'FilteredProtons')
        # Filter further with base kinematic cuts:
        self.dauPi_base = filterBase(name+'FilteredPionsBase',   self.dauPi_init)
        self.dauK_base  = filterBase(name+'FilteredKaonsBase',   self.dauK_init)
        self.dauP_base  = filterBase(name+'FilteredProtonsBase', self.dauP_init)
        # Apply additional filtering (ghost-prob):
        self.dauPi = filterGhosts(name+'FilteredPionsGP', self.dauPi_base)
        self.dauK  = filterGhosts(name+'FilteredKaonsGP', self.dauK_base)
        self.dauP  = filterGhosts(name+'FilteredProtonsGP', self.dauP_base)

        # Soft pions get special handling:
        self.dauSoftPi_init = DataOnDemand(Location = _my_immutable_config['SoftPions_InputList'])
        self.dauSoftPi_base = filterSoft(name+'FilteredSoftPionsBase', self.dauSoftPi_init)
        self.dauSoftPi = filterGhosts(name+'FilteredSoftPionsGP', self.dauSoftPi_base)

        # Downstream tracks get special handling and no ghost cuts:
        self.dauDownPi_init = DataOnDemand(Location = _my_immutable_config['DownPions_InputList'])
        self.dauDownK_init  = DataOnDemand(Location = _my_immutable_config['DownKaons_InputList'])        
        self.dauDownPi = filterDownstream(name+"FilteredDownstreamPions", self.dauDownPi_init)
        self.dauDownK  = filterDownstream(name+"FilteredDownstreamKaons", self.dauDownK_init)

        # Make Lambdac -> p K- pi+
        self.filterLc = makeLc(name+'FilterLc', [self.dauPi, self.dauK, self.dauP])

        # Combine Lambda with pion to make Xi-, or with kaon to make Omega-
        self.stdLambdaLL = DataOnDemand(Location = 'Phys/StdLooseLambdaLL/Particles')
        self.stdLambdaDD = DataOnDemand(Location = 'Phys/StdLooseLambdaDD/Particles')
        self.combineXiLLL = makeXi(name+'CombineXiLLL', self.stdLambdaLL, self.dauPi,     _my_immutable_config['Xi_LL_ADAMASS_HalfWin'], _my_immutable_config['Xi_LL_ADMASS_HalfWin'])
        self.combineXiDDL = makeXi(name+'CombineXiDDL', self.stdLambdaDD, self.dauPi,     _my_immutable_config['Xi_DD_ADAMASS_HalfWin'], _my_immutable_config['Xi_DD_ADMASS_HalfWin'])
        self.combineXiDDD = makeXi(name+'CombineXiDDD', self.stdLambdaDD, self.dauDownPi, _my_immutable_config['Xi_DD_ADAMASS_HalfWin'], _my_immutable_config['Xi_DD_ADMASS_HalfWin'])
        self.combineXi    = mergeLists(name+'CombineXi', [ self.combineXiLLL, self.combineXiDDL, self.combineXiDDD ] )
        self.combineOmegaLLL = makeOmega(name+'CombineOmegaLLL', self.stdLambdaLL, self.dauK,     _my_immutable_config['Xi_LL_ADAMASS_HalfWin'], _my_immutable_config['Xi_LL_ADMASS_HalfWin'])
        self.combineOmegaDDL = makeOmega(name+'CombineOmegaDDL', self.stdLambdaDD, self.dauK,     _my_immutable_config['Xi_DD_ADAMASS_HalfWin'], _my_immutable_config['Xi_DD_ADMASS_HalfWin'])
        self.combineOmegaDDD = makeOmega(name+'CombineOmegaDDD', self.stdLambdaDD, self.dauDownK, _my_immutable_config['Xi_DD_ADAMASS_HalfWin'], _my_immutable_config['Xi_DD_ADMASS_HalfWin'])
        self.combineOmega    = mergeLists(name+'CombineOmega', [ self.combineOmegaLLL, self.combineOmegaDDL, self.combineOmegaDDD ] )

        # FIXME: Don't hard-code chi2 cuts
        # Build Xic+ (only one decay mode):
        self.combineXicPlus = makeXic(name+"CombineXicPlus", [ self.combineXi, self.dauPi ], 30.0, '[Xi_c+ -> Xi- pi+ pi+]cc')
        # Build Xic0 (few different modes):
        self.combineXicZeroToXiPi   = makeXic(name+"CombineXicZeroToXiPi",   [ self.combineXi,    self.dauPi    ], 10.0, '[Xi_c0 -> Xi- pi+]cc')
        self.combineXicZeroToOmegaK = makeXic(name+"CombineXicZeroToOmegaK", [ self.combineOmega, self.dauK     ], 10.0, '[Xi_c0 -> Omega- K+]cc')
        self.combineXicZeroToPKKpi  = makeXic(name+"CombineXicZeroToPKKpi",  [ self.dauP, self.dauK, self.dauPi ], 60.0, '[Xi_c0 -> p+ K- K- pi+]cc')
        self.combineXicZero = mergeLists(name+"CombineXicZero", [ self.combineXicZeroToXiPi, self.combineXicZeroToOmegaK, self.combineXicZeroToPKKpi ] )

        # Annoyingly, DaVinci doesn't recognize the Xi_c(2815)0 and Xi_c(2815)+
        # As a temporary test, change them:
        #     Xi_c(2815)0   ->    Xi'_c0
        #     Xi_c(2815)+   ->    Xi'_c+

        # Combine Lambdac with a pion to make Sigmac
        self.combineScZero     = makeExcitedBaryon(name+"CombineScZero",     [ self.filterLc, self.dauPi ], "[Sigma_c0 -> Lambda_c+ pi-]cc")
        self.combineScPlusPlus = makeExcitedBaryon(name+"CombineScPlusPlus", [ self.filterLc, self.dauPi ], "[Sigma_c++ -> Lambda_c+ pi+]cc")
        # Combine Xic with a pion to make Xic(2645)
        self.combineXic2645Zero = makeExcitedBaryon(name+"CombineXic2645Zero", [ self.combineXicPlus, self.dauPi], "[Xi_c*0 -> Xi_c+ pi-]cc")
        self.combineXic2645Plus = makeExcitedBaryon(name+"CombineXic2645Plus", [ self.combineXicZero, self.dauPi], "[Xi_c*+ -> Xi_c0 pi+]cc")
        # Combine Xic(2645) with a pion to make Xic(2815)
        self.combineXic2815Zero = makeExcitedBaryon(name+"CombineXic2815Zero", [ self.combineXic2645Plus, self.dauPi], "[Xi'_c0 -> Xi_c*+ pi-]cc")
        self.combineXic2815Plus = makeExcitedBaryon(name+"CombineXic2815Plus", [ self.combineXic2645Zero, self.dauPi], "[Xi'_c+ -> Xi_c*0 pi+]cc")

        # Make partial B with missing Lc, representing it as a FAKE Delta
        # FIXME: Don't hard-code chisq cut
        self.combinePartialBMinusMissingLc  = makePartialB(name+"CombinePartialBMinusMissingLc",  [ self.combineXic2815Plus, self.dauPi ], 10.0, "[Delta0 -> Xi'_c+ pi-]cc")
        self.combinePartialBZeroMissingLc   = makePartialB(name+"CombinePartialBZeroMissingLc",   [ self.combineXic2815Zero, self.dauPi ], 10.0, "[Delta+ -> Xi'_c0 pi+]cc")
        self.combinePartialBMinusMissingXic = makePartialB(name+"CombinePartialBMinusMissingXic", [ self.combineScPlusPlus,  self.dauPi ], 30.0, "[Delta++ -> Sigma_c++ pi+ pi-]cc")
        self.combinePartialBZeroMissingXic  = makePartialB(name+"CombinePartialBZeroMissingXic",  [ self.combineScZero,      self.dauPi ], 30.0, "[Delta0 -> Sigma_c0 pi+ pi-]cc")

        # Reassemble complete B with the missing piece. Note that even though we end up at the same
        # final state, we do this from both directions (i.e. B-missing-Lc + Lc, and then B-missing-Xic + Xic).
        # The reason is that we must be sure the partial-complete reconstruction efficiency factorizes.
        self.combineFullBMinusMissingLc  = makeFullB(name+"CombineFullBMinusMissingLc",  [ self.combinePartialBMinusMissingLc,  self.filterLc ],       "[B-  -> Delta0 Lambda_c~-]cc")
        self.combineFullBZeroMissingLc   = makeFullB(name+"CombineFullBZeroMissingLc",   [ self.combinePartialBZeroMissingLc,   self.filterLc ],       "[B~0 -> Delta+ Lambda_c~-]cc")
        self.combineFullBMinusMissingXic = makeFullB(name+"CombineFullBMinusMissingXic", [ self.combinePartialBMinusMissingXic, self.combineXicPlus ], "[B-  -> Xi_c+  Delta~--  ]cc")
        self.combineFullBZeroMissingXic  = makeFullB(name+"CombineFullBZeroMissingXic",  [ self.combinePartialBZeroMissingXic,  self.combineXicZero ], "[B~0 -> Xi_c0  Delta~0   ]cc")


        # Make stripping lines:
        self.line1 = self._strippingLine(name = name+'LineFullBMinusMissingLc',
                                         prescale = config['signalPrescalePartial'],
                                         postscale = 1.0,
                                         FILTER = _globalEventCuts,
                                         selection = self.combineFullBMinusMissingLc)
        self.line2 = self._strippingLine(name = name+'LineFullBZeroMissingLc',
                                         prescale = config['signalPrescalePartial'],
                                         postscale = 1.0,
                                         FILTER = _globalEventCuts,
                                         selection = self.combineFullBZeroMissingLc)
        self.line3 = self._strippingLine(name = name+'LineFullBMinusMissingXic',
                                         prescale = config['signalPrescalePartial'],
                                         postscale = 1.0,
                                         FILTER = _globalEventCuts,
                                         selection = self.combineFullBMinusMissingXic)
        self.line4 = self._strippingLine(name = name+'LineFullBZeroMissingXic',
                                         prescale = config['signalPrescalePartial'],
                                         postscale = 1.0,
                                         FILTER = _globalEventCuts,
                                         selection = self.combineFullBZeroMissingXic)
        self.line5 = self._strippingLine(name = name+'LinePartialBMinusMissingLc',
                                         prescale = config['signalPrescalePartial'],
                                         postscale = 1.0,
                                         FILTER = _globalEventCuts,
                                         selection = self.combinePartialBMinusMissingLc)
        self.line6 = self._strippingLine(name = name+'LinePartialBZeroMissingLc',
                                         prescale = config['signalPrescalePartial'],
                                         postscale = 1.0,
                                         FILTER = _globalEventCuts,
                                         selection = self.combinePartialBZeroMissingLc)
        self.line7 = self._strippingLine(name = name+'LinePartialBMinusMissingXic',
                                         prescale = config['signalPrescalePartial'],
                                         postscale = 1.0,
                                         FILTER = _globalEventCuts,
                                         selection = self.combinePartialBMinusMissingXic)
        self.line8 = self._strippingLine(name = name+'LinePartialBZeroMissingXic',
                                         prescale = config['signalPrescalePartial'],
                                         postscale = 1.0,
                                         FILTER = _globalEventCuts,
                                         selection = self.combinePartialBZeroMissingXic)

        
 

def mergeLists(localName, inputSelections) :
    return MergedSelection ( localName,
                             RequiredSelections = inputSelections )

def makeLc(localName, inputSelections, configDict = _my_immutable_config) :
    ## Combination cuts
    _strCutComb = "(ADAMASS('Lambda_c+')<1.1*%(Lc_ADMASS_HalfWin)s)" \
                  "& (AMAXCHILD(MIPCHI2DV(PRIMARY))>%(Lc_Daug_1of3_MIPCHI2DV_Min)s)" \
                  "& (ADOCAMAX('')<%(Lc_ADOCAMAX_Max)s)" \
                  "& (APT>%(Lc_APT_Min)s)" % configDict

    _strCutMoth = "(VFASPF(VCHI2) < %(ChisqCutThreeBody)s)" \
                  "& (ADMASS('Lambda_c+')<%(Lc_ADMASS_HalfWin)s)" \
                  "& (BPVVDCHI2>%(Lc_BPVVDCHI2_Min)s)" \
                  "& (BPVDIRA>%(Lc_BPVDIRA_Min)s)" % configDict

    _combineLambdac2PKPi = CombineParticles(
        DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc",
        CombinationCut = _strCutComb,
        MotherCut = _strCutMoth
    )

    return Selection ( localName,
                       Algorithm = _combineLambdac2PKPi,
                       RequiredSelections = inputSelections )

# Generic filters for final-state input particles:

def filterBase(localName, inputSelection, configDict = _my_immutable_config) :
    _strCut = "(TRCHI2DOF<%(BaseTrack_TRCHI2DOF_Max)s)" \
              "& (P>%(BaseTrack_P_Min)s)" \
              "& (PT>%(BaseTrack_PT_Min)s)" \
              "& (MIPCHI2DV(PRIMARY)>%(BaseTrack_MIPCHI2DV_Min)s)" % configDict
    _filter = FilterDesktop(Code = _strCut)
    return Selection(localName,
                     Algorithm = _filter,
                     RequiredSelections = [ inputSelection ] )

def filterSoft(localName, inputSelection, configDict = _my_immutable_config) :
    # Filter:
    _strCut = "(TRCHI2DOF<%(SoftTrack_TRCHI2DOF_Max)s)" \
              "& (P>%(SoftTrack_P_Min)s)" \
              "& (PT>%(SoftTrack_PT_Min)s)" \
              "& (MIPCHI2DV(PRIMARY)>%(SoftTrack_MIPCHI2DV_Min)s)" % configDict
    _filter = FilterDesktop(Code = _strCut)
    return Selection ( localName,
                       Algorithm = _filter,
                       RequiredSelections = [ inputSelection ] )

def filterDownstream(localName, inputSelection, configDict = _my_immutable_config) :
    # Filter:
    _strCut = "(TRCHI2DOF<%(DownTrack_TRCHI2DOF_Max)s)" \
              "& (P>%(DownTrack_P_Min)s)" \
              "& (PT>%(DownTrack_PT_Min)s)" \
              "& (MIPCHI2DV(PRIMARY)>%(DownTrack_MIPCHI2DV_Min)s)" % configDict
    _filter = FilterDesktop(Code = _strCut)
    return Selection ( localName,
                       Algorithm = _filter,
                       RequiredSelections = [ inputSelection ] )

def filterGhosts(localName, inputSelection) :
    _strCutGhost = '( TRGHOSTPROB < 0.5 )'
    _filter = FilterDesktop(Code = _strCutGhost)
    return Selection(localName,
                     Algorithm = _filter,
                     RequiredSelections = [ inputSelection ] )

# PID filters for specific particle types:

def filterKaons(localName, configDict = _my_immutable_config) :
    # Pick up standard input list
    _stdK = DataOnDemand(Location = configDict['BaseKaons_InputList'])
    # Filter:
    _strCutDauK = "(HASRICH)&(PIDK-PIDpi>%(BaseKaon_PIDKPIDpi_Min)s)" % configDict
    _filterK = FilterDesktop(Code = _strCutDauK)
    return Selection ( localName,
                       Algorithm = _filterK,
                       RequiredSelections = [ _stdK ] )

def filterPions(localName, configDict = _my_immutable_config) :
    # Pick up standard input list
    _stdPi = DataOnDemand(Location = configDict['BasePions_InputList'])
    # Filter:
    _strCutDauPi = "(HASRICH)&(PIDpi-PIDK>%(BasePion_PIDpiPIDK_Min)s)" % configDict
    _filterPi = FilterDesktop(Code = _strCutDauPi)
    return Selection ( localName,
                       Algorithm = _filterPi,
                       RequiredSelections = [ _stdPi ] )

def filterProtons(localName, configDict = _my_immutable_config) :
    # Pick up standard input list
    _stdP = DataOnDemand(Location = configDict['BaseProtons_InputList'])
    # Filter:
    _strCutDauP  = "(HASRICH)&(PIDp-PIDpi>%(BaseProton_PIDpPIDpi_Min)s)" \
                   "& (PIDp-PIDK>%(BaseProton_PIDpPIDK_Min)s)" % configDict
    _filterP  = FilterDesktop(Code = _strCutDauP)
    return Selection ( localName,
                       Algorithm = _filterP,
                       RequiredSelections = [ _stdP ] )

# Reconstruct composites:

def makeXi(localName, inputListLambda, inputListTrack, cutWide, cutTight, configDict = _my_immutable_config) :
    _strCutComb = "( ADAMASS('Xi-') < %(cutWide)s * MeV )" % locals()
    _strCutMothMass = "( ADMASS('Xi-') < %(cutTight)s * MeV )" % locals()
    _strCutMothChi2 = "(VFASPF(VCHI2)<%(ChisqCutTwoBody)s)" % configDict
    _strCutMoth = _strCutMothMass + '&' + _strCutMothChi2
    _combine = CombineParticles( DecayDescriptor = '[Xi- -> Lambda0 pi-]cc',
                                 CombinationCut = _strCutComb,
                                 MotherCut = _strCutMoth )
    return Selection ( localName,
                       Algorithm = _combine,
                       RequiredSelections = [ inputListLambda, inputListTrack ] )

def makeOmega(localName, inputListLambda, inputListTrack, cutWide, cutTight, configDict = _my_immutable_config) :
    _strCutComb = "( ADAMASS('Omega-') < %(cutWide)s * MeV )" % locals()
    _strCutMothMass = "( ADMASS('Omega-') < %(cutTight)s * MeV )" % locals()
    _strCutMothChi2 = "(VFASPF(VCHI2)<%(ChisqCutTwoBody)s)" % configDict
    _strCutMoth = _strCutMothMass + '&' + _strCutMothChi2
    _combine = CombineParticles( DecayDescriptor = '[Omega- -> Lambda0 K-]cc',
                                 CombinationCut = _strCutComb,
                                 MotherCut = _strCutMoth )
    return Selection ( localName,
                       Algorithm = _combine,
                       RequiredSelections = [ inputListLambda, inputListTrack ] )

# For simplicity, use windows around the nominal Xic0 mass for both Xic0 and Xic+.
def makeXic(localName, inputSelections, inputChisqCut, inputDecay, configDict = _my_immutable_config) :
    _strCutComb = "( ADAMASS('Xi_c0') < %(Xic_ADAMASS_HalfWin)s )" % configDict
    _strCutMothChisq = "(VFASPF(VCHI2)<%(inputChisqCut)s)" % locals()
    _strCutMothRest  = "(ADMASS('Xi_c0') < %(Xic_ADMASS_HalfWin)s)" \
                       "& (BPVDIRA > %(Xic_BPVDIRA_Min)s)" % configDict
    _strCutMoth = _strCutMothChisq + '&' + _strCutMothRest
    _combine = CombineParticles( DecayDescriptor = inputDecay,
                                 CombinationCut = _strCutComb,
                                 MotherCut = _strCutMoth )
    return Selection( localName,
                      Algorithm = _combine,
                      RequiredSelections = inputSelections )

def makeExcitedBaryon(localName, inputSelections, inputDecay, configDict = _my_immutable_config) :
    _strCutMothChisq = "(VFASPF(VCHI2)<%(ChisqCutTwoBody)s)" % configDict
    _strCutMothDM = "(M - CHILD(M,1) - CHILD(M,2) > %(ExcitedBaryon_dm_Min)s) & (M - CHILD(M,1) - CHILD(M,2) < %(ExcitedBaryon_dm_Max)s)" % configDict
    _strCutMoth = _strCutMothChisq + '&' + _strCutMothDM
    _combine = CombineParticles( DecayDescriptor = inputDecay,
                                 MotherCut = _strCutMoth )
    return Selection( localName,
                      Algorithm = _combine,
                      RequiredSelections = inputSelections )

def makePartialB(localName, inputSelections, inputChisqCut, inputDecay, configDict = _my_immutable_config) :
    _strCutMothChisq = "(VFASPF(VCHI2)<%(inputChisqCut)s)" % locals()
    _strCutVD = "(BPVVDCHI2>%(PartialB_BPVVDCHI2_Min)s)" % configDict
    _strCutMothMass = "(M<%(PartialB_M_Max)s)" % configDict
    _strCutMoth = _strCutMothChisq + '&' + _strCutVD + '&' + _strCutMothMass
    _combine = CombineParticles( DecayDescriptor = inputDecay,
                                 MotherCut = _strCutMoth )
    return Selection( localName,
                      Algorithm = _combine,
                      RequiredSelections = inputSelections )

def makeFullB(localName, inputSelections, inputDecay, configDict = _my_immutable_config) :
    _strCutMothChisq = "(VFASPF(VCHI2)<%(ChisqCutTwoBody)s)" % configDict
    _strCutMoth = _strCutMothChisq
    _combine = CombineParticles( DecayDescriptor = inputDecay,
                                 MotherCut = _strCutMoth )
    return Selection( localName,
                      Algorithm = _combine,
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

