# $Id$
'''
Comment goes here.
'''

__author__ = ['Mat Charles']
__date__ = '01/Jan/2014'
__version__ = '$Revision: 1.1 $'

# Stuff below needs updating
__all__ = ( 'XibStarBuilder'               ## LineBuilder class specialization
            , 'default_name'            ## Default name
            , 'default_config'          ## Default config
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


default_name = "XibStarToXibZero"

# Should match __configuration_keys__
default_config = {
      'LongTrackGEC' : 300
      , 'prescale'  : 1.0
      , 'prescaleControlHadronic' : 1.0
      , 'prescaleControlMuonic' : 1.0
}


## These configuration parameters should not be changed outside of this file.
_my_immutable_config = {
        'DisplacedKaonInputList'         : 'Phys/StdLooseANNKaons/Particles'
        ,'DisplacedPionInputList'        : 'Phys/StdLooseANNPions/Particles'
        ,'DisplacedProtonInputList'      : 'Phys/StdLooseANNProtons/Particles'
        ,'DisplacedMuonInputList'        : 'Phys/StdLooseMuons/Particles'
        ,'SoftPions_InputList'           : 'Phys/StdAllNoPIDsPions/Particles'
        ,'DisplacedJpsiInputList'        : 'Phys/StdLooseJpsi2MuMu/Particles'
        ,'DownPions_InputList'           : 'Phys/StdNoPIDsDownPions/Particles'
        ,'DownKaons_InputList'           : 'Phys/StdLooseDownKaons/Particles'
        ,'DisplacedD0InputList'          : 'Phys/StdLooseD02KPi/Particles'
        ,'DisplacedDpInputList'          : 'Phys/StdLooseDplus2KPiPi/Particles'
        ,'DisplacedLcInputList'          : 'Phys/StdLooseLambdac2PKPi/Particles'
        ,'DisplacedTrack_P_Min'          : 2.0*GeV
        ,'DisplacedTrack_PT_Min'         : 250.0*MeV
        ,'DisplacedTrack_TRCHI2DOF_Max'  : 5.0
        ,'DisplacedTrack_MIPCHI2DV_Min'  : -1.0
        ,'DisplacedTrack_MIPCHI2DV_Tight_Min'  : 4.0
        ,'DisplacedTrack_GhostProb_Max'  : 0.7
        ,'Proton_ProbNNp_Min'            : 0.02
        ,'Proton_ProbNNp_Tight_Min'      : 0.05
        ,'Proton_ProbNNp_VeryTight_Min'      : 0.1
        ,'Kaon_ProbNNk_Min'              : 0.02
        ,'Kaon_ProbNNk_Tight_Min'        : 0.04
        ,'Kaon_ProbNNk_VeryTight_Min'        : 0.1
        ,'Pion_ProbNNpi_Min'             : 0.01
        ,'Pion_ProbNNpi_Tight_Min'       : 0.04
        ,'Muon_ProbNNmu_Min'             : 0.01
        ,'Xi_LLL_ADAMASS_HalfWin'        :  50.0*MeV
        ,'Xi_LLL_ADMASS_HalfWin'         :  35.0*MeV
        ,'Xi_DDL_ADAMASS_HalfWin'        :  80.0*MeV
        ,'Xi_DDL_ADMASS_HalfWin'         :  50.0*MeV
        ,'Xi_DDD_ADAMASS_HalfWin'        :  80.0*MeV
        ,'Xi_DDD_ADMASS_HalfWin'         :  50.0*MeV
        ,'Xi_LLL_VtxChisq_Max'           : 15.0
        ,'Xi_DDL_VtxChisq_Max'           : 15.0
        ,'Xi_DDD_VtxChisq_Max'           : 15.0
        ,'SoftTrack_TRCHI2DOF_Max'     :   5.0
        ,'SoftTrack_PT_Min'            :   0.0*MeV
        ,'SoftTrack_P_Min'             :   1.0*GeV
        ,'SoftTrack_ProbNN_e_Max'      : 0.2
        ,'SoftTrack_MIPCHI2DV_Min'     :   -999.0
        ,'DownTrack_TRCHI2DOF_Max'     :   5.0
        ,'DownTrack_PT_Min'            : 100.0*MeV
        ,'DownTrack_P_Min'             :   2.0*GeV
        ,'DownTrack_MIPCHI2DV_Min'     :  -1.0
        ,'XibStar_PT_Min'              :2500.0*MeV
        ,'XibStar_VtxChisq_Max'        :  20.0
        ,'XibStar_DMCutLower'          : -25.0*MeV
        ,'XibStar_DMCutUpper'          :  50.0*MeV
        ,'XibStar_DMCutLower_SL'       :-250.0*MeV
        ,'XibStar_DMCutUpper_SL'       : 250.0*MeV
        ,'Xic_ADAMASS_HalfWin'         :  80.0*MeV
        ,'Xic_ADMASS_HalfWin'          :  50.0*MeV
        ,'Xic_BPVDIRA_Min'             :   0.9
        ,'Xic_BPVVDCHI2_Min'           :  25.0
        ,'Xib_BPVVDCHI2_Min'           :  36.0
        ,'Xib_VtxChisqPerDOF_Max'      :  10.0
        ,'Xib_VtxChisqPerDOF_Tight_Max':   7.0
        ,'Xib_BPVDIRA_Min'             :   0.98
        ,'Xib_BPVDIRA_Tight_Min'       :   0.995
        ,'Xib_CombCutPT_Min'           :2000.0*MeV
        }


class XibStarBuilder(LineBuilder) :
    """
    Look for Xib*- -> Xib0 pi-
    """
    
    # Should match default_config
    __configuration_keys__ = ( 'LongTrackGEC'
                               , 'prescale'
                               , 'prescaleControlHadronic'
                               , 'prescaleControlMuonic')

    ## Possible parameters and default values copied from the definition
    ##   of StrippingLine. The point of this little wrapper is that only
    ##   lines with non-zero prescale and postscale will get instantiated,
    ##   saving overhead from lines that will never select anything.
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

        if (prescale > 0) and (postscale > 0) :
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
        else : 
            return False

    ## Top-level routine to make the stripping lines
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCutsCode = "(recSummary (LHCb.RecSummary.nLongTracks, 'Rec/Track/Long') < %(LongTrackGEC)s )" % config
        _globalEventCuts = { 'Code' : _globalEventCutsCode, 'Preambulo' : ["from LoKiTracks.decorators import *"]}

        # Pick up standard kaons, pions
        self.stdPi = DataOnDemand(Location = _my_immutable_config['DisplacedPionInputList'])
        self.stdK  = DataOnDemand(Location = _my_immutable_config['DisplacedKaonInputList'])
        self.stdP  = DataOnDemand(Location = _my_immutable_config['DisplacedProtonInputList'])
        self.stdMu = DataOnDemand(Location = _my_immutable_config['DisplacedMuonInputList'])
        # Apply some generic filtering
        self.baseFiltPi = filterDisplacedTracks(name+'BaseFilteredPions',   self.stdPi)
        self.baseFiltK  = filterDisplacedTracks(name+'BaseFilteredKaons',   self.stdK)
        self.baseFiltP  = filterDisplacedTracks(name+'BaseFilteredProtons', self.stdP)
        self.baseFiltMu = filterDisplacedTracks(name+'BaseFilteredMuons',   self.stdMu)
        # Remove ghosts
        self.ghostFiltPi = filterGhosts(name+'GhostFilteredPions',   self.baseFiltPi)
        self.ghostFiltK  = filterGhosts(name+'GhostFilteredKaons',   self.baseFiltK)
        self.ghostFiltP  = filterGhosts(name+'GhostFilteredProtons', self.baseFiltP)
        self.ghostFiltMu = filterGhosts(name+'GhostFilteredMuons',   self.baseFiltMu)
        # Filter them for use as daughter particles from a displaced vertex:
        self.dauPi = filterPions(name+'FilteredPions', self.ghostFiltPi)
        self.dauK  = filterKaons(name+'FilteredKaons', self.ghostFiltK)
        self.dauP  = filterProtons(name+'FilteredProtons', self.ghostFiltP)
        self.dauMu = filterMuons(name+'FilteredMuons', self.ghostFiltMu)
        # Tighter cuts, when needed
        self.dauTightPi = filterPionsTight(name+'TightFilteredPions', self.dauPi)
        self.dauTightK  = filterKaonsTight(name+'TightFilteredKaons', self.dauK)
        self.dauTightP  = filterProtonsTight(name+'TightFilteredProtons', self.dauP)
        self.dauPiForSL = filterForSL(name+'SLFilteredPions', self.dauPi)
        # Very tight cuts
        self.dauVeryTightP = filterProtonsVeryTight(name+'VeryTightFilteredProtons', self.dauTightP)
        self.dauVeryTightK = filterKaonsVeryTight(name+'VeryTightFilteredKaons', self.dauTightK)

        # Soft pions get special handling:
        self.dauSoftPi_init = DataOnDemand(Location = _my_immutable_config['SoftPions_InputList'])
        self.dauSoftPi_base = filterSoft(name+'FilteredSoftPionsBase', self.dauSoftPi_init)
        self.dauSoftPi = filterGhosts(name+'FilteredSoftPionsGP', self.dauSoftPi_base)

        # Downstream tracks get special handling and no ghost cuts:
        self.dauDownPi_init = DataOnDemand(Location = _my_immutable_config['DownPions_InputList'])
        self.dauDownK_init  = DataOnDemand(Location = _my_immutable_config['DownKaons_InputList'])
        self.dauDownPi_filt = filterDownstream(name+"PartFilteredDownstreamPions", self.dauDownPi_init)
        self.dauDownK_filt  = filterDownstream(name+"PartFilteredDownstreamKaons", self.dauDownK_init)
        self.dauDownPi      = filterPions(name+"FilteredDownstreamPions", self.dauDownPi_filt)
        self.dauDownK       = filterKaons(name+"FilteredDownstreamKaons", self.dauDownK_filt)
        
        # Make some useful composite daughters:
        self.stdJpsi = DataOnDemand(Location = _my_immutable_config['DisplacedJpsiInputList'])
        self.stdD0   = DataOnDemand(Location = _my_immutable_config['DisplacedD0InputList'])
        self.stdDp   = DataOnDemand(Location = _my_immutable_config['DisplacedDpInputList'])
        self.stdLc   = DataOnDemand(Location = _my_immutable_config['DisplacedLcInputList'])

        # Filter the Lc to be tighter:
        _LcFiltCut = "( MINTREE('pi+'==ABSID, PROBNNpi) > 0.01 ) & ( MINTREE('K+'==ABSID, PROBNNk) > 0.02 ) & ( MINTREE('p+'==ABSID, PROBNNp) > 0.02 ) & ( PT > 2.0*GeV ) & (ADMASS('Lambda_c+')<50*MeV)"
        self.Lc = filterGeneric(name+"DisplacedLcFilt", self.stdLc, _LcFiltCut)
        # Even tighter:
        _LcTightFiltCut = "( MINTREE('K+'==ABSID, PROBNNk) > 0.05 ) & ( MINTREE('p+'==ABSID, PROBNNp) > 0.1 )"
        self.TightLc = filterGeneric(name+"DisplacedLcTightFilt", self.Lc, _LcTightFiltCut)


        # Filter the D0 and D+ to be tighter...
        _DFiltCutGeneric = "( MINTREE('pi+'==ABSID, PROBNNpi) > 0.01 ) & ( MINTREE('K+'==ABSID, PROBNNk) > 0.02 ) & ( PT > 2.0*GeV )"
        _DpFiltCut = _DFiltCutGeneric + " & (ADMASS('D+')<50*MeV)"
        _D0FiltCut = _DFiltCutGeneric + " & (ADMASS('D0')<50*MeV)"
        self.D0 = filterGeneric(name+"DisplacedD0Filt", self.stdD0, _D0FiltCut)
        self.Dp = filterGeneric(name+"DisplacedDpFilt", self.stdDp, _DpFiltCut)
        
        # Combine Lambda with pion to make Xi-
        self.stdLambdaLL = DataOnDemand(Location = 'Phys/StdLooseLambdaLL/Particles')
        self.stdLambdaDD = DataOnDemand(Location = 'Phys/StdLooseLambdaDD/Particles')
        self.combineXiLLL = makeXi(name+'CombineXiLLL', self.stdLambdaLL, self.dauPi,     _my_immutable_config['Xi_LLL_ADAMASS_HalfWin'], _my_immutable_config['Xi_LLL_ADMASS_HalfWin'], _my_immutable_config['Xi_LLL_VtxChisq_Max'])
        self.combineXiDDL = makeXi(name+'CombineXiDDL', self.stdLambdaDD, self.dauPi,     _my_immutable_config['Xi_DDL_ADAMASS_HalfWin'], _my_immutable_config['Xi_DDL_ADMASS_HalfWin'], _my_immutable_config['Xi_DDL_VtxChisq_Max'])
        self.combineXiDDD = makeXi(name+'CombineXiDDD', self.stdLambdaDD, self.dauDownPi, _my_immutable_config['Xi_DDD_ADAMASS_HalfWin'], _my_immutable_config['Xi_DDD_ADMASS_HalfWin'], _my_immutable_config['Xi_DDD_VtxChisq_Max'])
        self.combineOmegaLLL = makeOmega(name+'CombineOmegaLLL', self.stdLambdaLL, self.dauK,     _my_immutable_config['Xi_LLL_ADAMASS_HalfWin'], _my_immutable_config['Xi_LLL_ADMASS_HalfWin'], _my_immutable_config['Xi_LLL_VtxChisq_Max'])
        self.combineOmegaDDL = makeOmega(name+'CombineOmegaDDL', self.stdLambdaDD, self.dauK,     _my_immutable_config['Xi_DDL_ADAMASS_HalfWin'], _my_immutable_config['Xi_DDL_ADMASS_HalfWin'], _my_immutable_config['Xi_DDL_VtxChisq_Max'])
        self.combineOmegaDDD = makeOmega(name+'CombineOmegaDDD', self.stdLambdaDD, self.dauDownK, _my_immutable_config['Xi_DDD_ADAMASS_HalfWin'], _my_immutable_config['Xi_DDD_ADMASS_HalfWin'], _my_immutable_config['Xi_DDD_VtxChisq_Max'])
        # Make lists merging (long/downstream) combinations for Lambda and Xi:
        self.combineLambda = mergeLists(name+'CombineLambda', [ self.stdLambdaLL, self.stdLambdaDD ] )
        self.combineXi     = mergeLists(name+'CombineXi', [ self.combineXiLLL, self.combineXiDDL, self.combineXiDDD ] )
        self.combineOmega    = mergeLists(name+'CombineOmega', [ self.combineOmegaLLL, self.combineOmegaDDL, self.combineOmegaDDD ] )

        # Build Xic+ (only one decay mode):
        self.combineXicPlusToXiPiPi = makeXic(name+"CombineXicPlusToXiPiPi", [ self.combineXi, self.dauTightPi ], 30.0, '[Xi_c+ -> Xi- pi+ pi+]cc')
        self.combineXicPlusToPKPi = makeXic(name+"CombineXicPlusToPKpi",  [ self.dauVeryTightP, self.dauVeryTightK, self.dauTightPi ], 30.0,  '[Xi_c+ -> p+ K- pi+]cc')
        self.Xicp = mergeLists(name+"CombineXicPlus", [ self.combineXicPlusToXiPiPi, self.combineXicPlusToPKPi ] )
        # Build Xic0 (few different modes):
        self.combineXicZeroToXiPi   = makeXic(name+"CombineXicZeroToXiPi",   [ self.combineXi,    self.dauTightPi    ], 10.0, '[Xi_c0 -> Xi- pi+]cc')
        self.combineXicZeroToOmegaK = makeXic(name+"CombineXicZeroToOmegaK", [ self.combineOmega, self.dauTightK     ], 10.0, '[Xi_c0 -> Omega- K+]cc')
        self.combineXicZeroToPKKpi  = makeXic(name+"CombineXicZeroToPKKpi",  [ self.dauTightP, self.dauTightK, self.dauTightPi ], 60.0,  '[Xi_c0 -> p+ K- K- pi+]cc')
        self.Xic0 = mergeLists(name+"CombineXicZero", [ self.combineXicZeroToXiPi, self.combineXicZeroToOmegaK, self.combineXicZeroToPKKpi ] )
        
        # Some very generic cuts for making Xib0:
        # Nominal mass: 5788 MeV
        _strCutXib0Comb = "( APT > %(Xib_CombCutPT_Min)s ) & ( AMAXDOCA('')<0.5*mm ) & ( AM>3800.0*MeV ) & ( AM<7200.0*MeV )" % _my_immutable_config
        _strCutXib0MothLoose = "( BPVVDCHI2 > %(Xib_BPVVDCHI2_Min)s ) & ( VFASPF(VCHI2PDOF)<%(Xib_VtxChisqPerDOF_Max)s )" % _my_immutable_config
        _strCutXib0MothSL = _strCutXib0MothLoose + " & ( MM>4000.0*MeV ) & ( MM<6100.0*MeV )"
        _strCutXib0MothReg = _strCutXib0MothLoose + " & ( MM>5200.0*MeV ) & ( MM<6400.0*MeV ) & ( BPVDIRA > %(Xib_BPVDIRA_Min)s )" % _my_immutable_config
        _strCutXib0MothTight = "( BPVVDCHI2 > %(Xib_BPVVDCHI2_Min)s ) & ( VFASPF(VCHI2PDOF)<%(Xib_VtxChisqPerDOF_Tight_Max)s ) & ( MM>5500.0*MeV ) & ( MM<6100.0*MeV ) & ( BPVDIRA > %(Xib_BPVDIRA_Tight_Min)s )" % _my_immutable_config

        # Build the Xib0 decay to a range of different final states:

        # Line 1 uses: Xib0 -> J/psi Xi- pi+
        _name1 = 'XibToJpsiXiPi'
        self.XibMode1  = makeGeneric(name+'Combine'+_name1, [ self.stdJpsi, self.combineXi, self.dauPi ], '[ Xi_b0 -> J/psi(1S) Xi- pi+ ]cc', _strCutXib0Comb, _strCutXib0MothReg)
        # Line 2 uses: Xib0 -> J/psi Lambda K- pi+
        _name2 = 'XibToJpsiLambdaKPi'
        self.XibMode2  = makeGeneric(name+'Combine'+_name2, [ self.stdJpsi, self.combineLambda, self.dauK, self.dauPi ], '[ Xi_b0 -> J/psi(1S) Lambda0 K- pi+ ]cc',  _strCutXib0Comb, _strCutXib0MothReg)
        # Line 3 uses: Xib0 -> J/psi p+ K- K- pi+
        _name3 = 'XibToJpsiPKKPi'
        self.XibMode3  = makeGeneric(name+'Combine'+_name3, [ self.stdJpsi, self.dauP, self.dauK, self.dauPi ], '[ Xi_b0 -> J/psi(1S) p+ K- K- pi+ ]cc',  _strCutXib0Comb, _strCutXib0MothReg)
        # Line 4 uses: Xib0 -> J/psi p+ K-
        _name4 = 'XibToJpsiPK'
        self.XibMode4  = makeGeneric(name+'Combine'+_name4, [ self.stdJpsi, self.dauP, self.dauK ], '[ Xi_b0 -> J/psi(1S) p+ K- ]cc', _strCutXib0Comb, _strCutXib0MothReg)
        # Line 5 uses: Xib0 -> D0 p+ K-
        _name5 = 'XibToD0PK'
        self.XibMode5  = makeGeneric(name+'Combine'+_name5, [ self.D0, self.dauTightP, self.dauTightK ], '[ Xi_b0 ->  D0 p+ K- ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 6 uses: Xib0 -> D+ p+ K- pi- (relatively high rate)
        _name6 = 'XibToDpPKPi'
        self.XibMode6  = makeGeneric(name+'Combine'+_name6, [ self.Dp, self.dauTightP, self.dauTightK, self.dauPi ], '[ Xi_b0 -> D+ p+ K- pi- ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 7 uses: Xib0 -> Lc+ K-
        _name7 = 'XibToLcK'
        self.XibMode7  = makeGeneric(name+'Combine'+_name7, [ self.Lc, self.dauTightK ], '[ Xi_b0 -> Lambda_c+ K- ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 8 uses: Xib0 -> Xic+ pi-
        _name8 = 'XibToXicpPi'
        self.XibMode8  = makeGeneric(name+'Combine'+_name8, [ self.Xicp, self.dauPi ], '[ Xi_b0 -> Xi_c+ pi- ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 9 uses: Xib0 -> Xib0 pi- pi+
        _name9 = 'XibToXic0PiPi'
        self.XibMode9  = makeGeneric(name+'Combine'+_name9, [ self.Xic0, self.dauPi ], '[ Xi_b0 -> Xi_c0 pi- pi+ ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 10 uses: Xib0 -> p+ K-
        _name10 = 'XibToPK'
        self.XibMode10 = makeGeneric(name+'Combine'+_name10, [ self.dauTightP, self.dauTightK ], '[ Xi_b0 -> p+ K- ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 11 uses: Xib0 -> Xic+ mu- [and missing neutrino etc]
        _name11 = 'XibToXicpMu'
        self.XibMode11 = makeGeneric(name+'Combine'+_name11, [ self.Xicp, self.dauMu ], '[ Xi_b0 -> Xi_c+ mu- ]cc', _strCutXib0Comb, _strCutXib0MothSL)
        # Line 12 uses: Xib0 -> Xic0 pi+ mu- [and missing neutrino etc]
        _name12 = 'XibToXic0PiMu'
        self.XibMode12 = makeGeneric(name+'Combine'+_name12, [ self.Xic0, self.dauPiForSL, self.dauMu ], '[ Xi_b0 -> Xi_c0 pi+ mu- ]cc', _strCutXib0Comb, _strCutXib0MothSL)
        # Line 13 uses: Xib0 -> Lc+ K- pi+ pi-
        _name13 = 'XibToLcKPiPi'
        self.XibMode13  = makeGeneric(name+'Combine'+_name13, [ self.TightLc, self.dauVeryTightK, self.dauPi ], '[ Xi_b0 -> Lambda_c+ K- pi+ pi- ]cc', _strCutXib0Comb, _strCutXib0MothTight)

        # Build Xib* -> Xib0 pi+
        self.XibStarMode1RS  = makeXibStarRS(  name+'CombineXibStarRS'+_name1,  [ self.XibMode1,  self.dauSoftPi ])
        self.XibStarMode2RS  = makeXibStarRS(  name+'CombineXibStarRS'+_name2,  [ self.XibMode2,  self.dauSoftPi ])
        self.XibStarMode3RS  = makeXibStarRS(  name+'CombineXibStarRS'+_name3,  [ self.XibMode3,  self.dauSoftPi ])
        self.XibStarMode4RS  = makeXibStarRS(  name+'CombineXibStarRS'+_name4,  [ self.XibMode4,  self.dauSoftPi ])
        self.XibStarMode5RS  = makeXibStarRS(  name+'CombineXibStarRS'+_name5,  [ self.XibMode5,  self.dauSoftPi ])
        self.XibStarMode6RS  = makeXibStarRS(  name+'CombineXibStarRS'+_name6,  [ self.XibMode6,  self.dauSoftPi ])
        self.XibStarMode7RS  = makeXibStarRS(  name+'CombineXibStarRS'+_name7,  [ self.XibMode7,  self.dauSoftPi ])
        self.XibStarMode8RS  = makeXibStarRS(  name+'CombineXibStarRS'+_name8,  [ self.XibMode8,  self.dauSoftPi ])
        self.XibStarMode9RS  = makeXibStarRS(  name+'CombineXibStarRS'+_name9,  [ self.XibMode9,  self.dauSoftPi ])
        self.XibStarMode10RS = makeXibStarRS(  name+'CombineXibStarRS'+_name10, [ self.XibMode10, self.dauSoftPi ])
        self.XibStarMode11RS = makeXibStarRSSL(name+'CombineXibStarRS'+_name11, [ self.XibMode11, self.dauSoftPi ])
        self.XibStarMode12RS = makeXibStarRSSL(name+'CombineXibStarRS'+_name12, [ self.XibMode12, self.dauSoftPi ])
        self.XibStarMode13RS = makeXibStarRSSL(name+'CombineXibStarRS'+_name13, [ self.XibMode13, self.dauSoftPi ])
        self.XibStarMode1WS  = makeXibStarWS(  name+'CombineXibStarWS'+_name1,  [ self.XibMode1,  self.dauSoftPi ])
        self.XibStarMode2WS  = makeXibStarWS(  name+'CombineXibStarWS'+_name2,  [ self.XibMode2,  self.dauSoftPi ])
        self.XibStarMode3WS  = makeXibStarWS(  name+'CombineXibStarWS'+_name3,  [ self.XibMode3,  self.dauSoftPi ])
        self.XibStarMode4WS  = makeXibStarWS(  name+'CombineXibStarWS'+_name4,  [ self.XibMode4,  self.dauSoftPi ])
        self.XibStarMode5WS  = makeXibStarWS(  name+'CombineXibStarWS'+_name5,  [ self.XibMode5,  self.dauSoftPi ])
        self.XibStarMode6WS  = makeXibStarWS(  name+'CombineXibStarWS'+_name6,  [ self.XibMode6,  self.dauSoftPi ])
        self.XibStarMode7WS  = makeXibStarWS(  name+'CombineXibStarWS'+_name7,  [ self.XibMode7,  self.dauSoftPi ])
        self.XibStarMode8WS  = makeXibStarWS(  name+'CombineXibStarWS'+_name8,  [ self.XibMode8,  self.dauSoftPi ])
        self.XibStarMode9WS  = makeXibStarWS(  name+'CombineXibStarWS'+_name9,  [ self.XibMode9,  self.dauSoftPi ])
        self.XibStarMode10WS = makeXibStarWS(  name+'CombineXibStarWS'+_name10, [ self.XibMode10, self.dauSoftPi ])
        self.XibStarMode11WS = makeXibStarWSSL(name+'CombineXibStarWS'+_name11, [ self.XibMode11, self.dauSoftPi ])
        self.XibStarMode12WS = makeXibStarWSSL(name+'CombineXibStarWS'+_name12, [ self.XibMode12, self.dauSoftPi ])
        self.XibStarMode13WS = makeXibStarWSSL(name+'CombineXibStarWS'+_name13, [ self.XibMode13, self.dauSoftPi ])

        # Build the stripping lines for Xib*:
        self.line1RS  = self._strippingLine(name = name+_name1,        prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode1RS)
        self.line1WS  = self._strippingLine(name = name+_name1+'_WS',  prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode1WS)
        self.line2RS  = self._strippingLine(name = name+_name2,        prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode2RS)
        self.line2WS  = self._strippingLine(name = name+_name2+'_WS',  prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode2WS)
        self.line3RS  = self._strippingLine(name = name+_name3,        prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode3RS)
        self.line3WS  = self._strippingLine(name = name+_name3+'_WS',  prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode3WS)
        self.line4RS  = self._strippingLine(name = name+_name4,        prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode4RS)
        self.line4WS  = self._strippingLine(name = name+_name4+'_WS',  prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode4WS)
        self.line5RS  = self._strippingLine(name = name+_name5,        prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode5RS)
        self.line5WS  = self._strippingLine(name = name+_name5+'_WS',  prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode5WS)
        self.line6RS  = self._strippingLine(name = name+_name6,        prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode6RS)
        self.line6WS  = self._strippingLine(name = name+_name6+'_WS',  prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode6WS)
        self.line7RS  = self._strippingLine(name = name+_name7,        prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode7RS)
        self.line7WS  = self._strippingLine(name = name+_name7+'_WS',  prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode7WS)
        self.line8RS  = self._strippingLine(name = name+_name8,        prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode8RS)
        self.line8WS  = self._strippingLine(name = name+_name8+'_WS',  prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode8WS)
        self.line9RS  = self._strippingLine(name = name+_name9,        prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode9RS)
        self.line9WS  = self._strippingLine(name = name+_name9+'_WS',  prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode9WS)
        self.line10RS = self._strippingLine(name = name+_name10,       prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode10RS)
        self.line10WS = self._strippingLine(name = name+_name10+'_WS', prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode10WS)
        self.line11RS = self._strippingLine(name = name+_name11,       prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode11RS)
        self.line11WS = self._strippingLine(name = name+_name11+'_WS', prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode11WS)
        self.line12RS = self._strippingLine(name = name+_name12,       prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode12RS)
        self.line12WS = self._strippingLine(name = name+_name12+'_WS', prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode12WS)
        self.line13RS = self._strippingLine(name = name+_name13,       prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode13RS)
        self.line13WS = self._strippingLine(name = name+_name13+'_WS', prescale = config['prescale'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibStarMode13WS)
                                         
        # Build the control lines for Xib0:
        self.control1  = self._strippingLine(name = name+"ControlXib_"+_name1,  prescale = config['prescaleControlMuonic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode1)
        self.control2  = self._strippingLine(name = name+"ControlXib_"+_name2,  prescale = config['prescaleControlMuonic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode2)
        self.control3  = self._strippingLine(name = name+"ControlXib_"+_name3,  prescale = config['prescaleControlMuonic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode3)
        self.control4  = self._strippingLine(name = name+"ControlXib_"+_name4,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode4)
        self.control5  = self._strippingLine(name = name+"ControlXib_"+_name5,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode5)
        self.control6  = self._strippingLine(name = name+"ControlXib_"+_name6,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode6)
        self.control7  = self._strippingLine(name = name+"ControlXib_"+_name7,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode7)
        self.control8  = self._strippingLine(name = name+"ControlXib_"+_name8,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode8)
        self.control9  = self._strippingLine(name = name+"ControlXib_"+_name9,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode9)
        self.control10 = self._strippingLine(name = name+"ControlXib_"+_name10, prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode10)
        self.control11 = self._strippingLine(name = name+"ControlXib_"+_name11, prescale = config['prescaleControlMuonic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode11)
        self.control12 = self._strippingLine(name = name+"ControlXib_"+_name12, prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode12)
        self.control13 = self._strippingLine(name = name+"ControlXib_"+_name13, prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode13)


def filterDisplacedTracks(localName, inputSelection, configDict = _my_immutable_config) :
    _strCut = "(TRCHI2DOF<%(DisplacedTrack_TRCHI2DOF_Max)s)" \
              "& (P>%(DisplacedTrack_P_Min)s)" \
              "& (PT>%(DisplacedTrack_PT_Min)s)" \
              "& (MIPCHI2DV(PRIMARY)>%(DisplacedTrack_MIPCHI2DV_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCut)

def filterKaons(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(HASRICH)&(PROBNNk>%(Kaon_ProbNNk_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterPions(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(HASRICH)&(PROBNNpi>%(Pion_ProbNNpi_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterProtons(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(HASRICH)&(PROBNNp>%(Proton_ProbNNp_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterMuons(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNmu>%(Muon_ProbNNmu_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterKaonsTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNk>%(Kaon_ProbNNk_Tight_Min)s) & (MIPCHI2DV(PRIMARY)>%(DisplacedTrack_MIPCHI2DV_Tight_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterPionsTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNpi>%(Pion_ProbNNpi_Tight_Min)s) & (MIPCHI2DV(PRIMARY)>%(DisplacedTrack_MIPCHI2DV_Tight_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterProtonsTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNp>%(Proton_ProbNNp_Tight_Min)s) & (MIPCHI2DV(PRIMARY)>%(DisplacedTrack_MIPCHI2DV_Tight_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterKaonsVeryTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNk>%(Kaon_ProbNNk_VeryTight_Min)s) & (PROBNNk>PROBNNpi)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterProtonsVeryTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNp>%(Proton_ProbNNp_VeryTight_Min)s) & (PROBNNp>PROBNNpi)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterSoft(localName, inputSelection, configDict = _my_immutable_config) :
    _strCut = "(TRCHI2DOF<%(SoftTrack_TRCHI2DOF_Max)s)" \
              "& (P>%(SoftTrack_P_Min)s)" \
              "& (PT>%(SoftTrack_PT_Min)s)" \
              "& (PROBNNp<%(SoftTrack_ProbNN_e_Max)s)" \
              "& (MIPCHI2DV(PRIMARY)>%(SoftTrack_MIPCHI2DV_Min)s)" % configDict    
    return filterGeneric(localName, inputSelection, _strCut)

def filterDownstream(localName, inputSelection, configDict = _my_immutable_config) :
    _strCut = "(TRCHI2DOF<%(DownTrack_TRCHI2DOF_Max)s)" \
              "& (P>%(DownTrack_P_Min)s)" \
              "& (PT>%(DownTrack_PT_Min)s)" \
              "& (MIPCHI2DV(PRIMARY)>%(DownTrack_MIPCHI2DV_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCut)

def filterGhosts(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutGhost = '(TRGHOSTPROB<%(DisplacedTrack_GhostProb_Max)s)' % configDict
    return filterGeneric(localName, inputSelection, _strCutGhost)

def filterForSL(localName, inputSelection, configDict = _my_immutable_config) :
    return filterGeneric(localName, inputSelection, '(PT>500*MeV)')

def makeXi(localName, inputListLambda, inputListTrack, cutWide, cutTight, ChisqCutTwoBody) :
    _strCutComb = "( ADAMASS('Xi-') < %(cutWide)s )" % locals()
    _strCutMothMass = "( ADMASS('Xi-') < %(cutTight)s )" % locals()
    _strCutMothChi2 = "(VFASPF(VCHI2)<%(ChisqCutTwoBody)s)" % locals()
    _strCutMothDIRA = "( BPVDIRA > 0.95 )"
    _strCutMoth = _strCutMothMass + '&' + _strCutMothChi2 + '&' + _strCutMothDIRA
    _strDecay = '[ Xi- -> Lambda0 pi- ]cc'
    _output = makeGeneric(localName, [ inputListLambda, inputListTrack ],_strDecay, _strCutComb, _strCutMoth )
    return _output

def makeOmega(localName, inputListLambda, inputListTrack, cutWide, cutTight,  ChisqCutTwoBody) :
    _strCutComb = "( ADAMASS('Omega-') < %(cutWide)s )" % locals()
    _strCutMothMass = "( ADMASS('Omega-') < %(cutTight)s )" % locals()
    _strCutMothChi2 = "(VFASPF(VCHI2)<%(ChisqCutTwoBody)s)" % locals()
    _strCutMoth = _strCutMothMass + '&' + _strCutMothChi2
    _strDecay = '[Omega- -> Lambda0 K-]cc'
    _output = makeGeneric(localName, [ inputListLambda, inputListTrack ],_strDecay, _strCutComb, _strCutMoth )
    return _output

def makeXic(localName, inputSelections, inputChisqCut, inputDecay, configDict = _my_immutable_config) :
    _strCutCombRest = "( ADAMASS('Xi_c0') < %(Xic_ADAMASS_HalfWin)s ) & ( APT > 1.0*GeV )" % configDict
    _strCutComb = _strCutCombRest
    _strCutMothChisq = "(VFASPF(VCHI2)<%(inputChisqCut)s)" % locals()
    _strCutMothRest  = "(ADMASS('Xi_c0') < %(Xic_ADMASS_HalfWin)s)" \
                       "& (BPVDIRA > %(Xic_BPVDIRA_Min)s)" % configDict
    _strCutMoth = _strCutMothChisq + '&' + _strCutMothRest
    _output = makeGeneric(localName, inputSelections, inputDecay, _strCutComb, _strCutMoth)
    return _output

# CDF measures: m(Xib*) - m(Xib) - m(pi) = 14.84 +- 0.74 +- 0.28 MeV
def makeXibStarBase(localName, inputSelections, decay, cutMothDM, configDict = _my_immutable_config) :
    _cutComb = '(AM > 5000.0*MeV) & (AM < 8000.0*MeV)'
    _cutMothBase = '(PT > %(XibStar_PT_Min)s) & (VFASPF(VCHI2/VDOF) < %(XibStar_VtxChisq_Max)s )' % configDict
    _cutMoth = '(' + _cutMothBase + ' & ' + cutMothDM + ')'
    _output = makeGeneric(localName, inputSelections, decay, _cutComb, _cutMoth)
    return _output

def makeXibStar(localName, inputSelections, decay, configDict = _my_immutable_config) :
    _cutMothDM = '(MM - CHILD(MM,1) - CHILD(MM,2) > %(XibStar_DMCutLower)s) & (MM - CHILD(MM,1) - CHILD(MM,2) < %(XibStar_DMCutUpper)s)' % configDict
    _output = makeXibStarBase(localName, inputSelections, decay, _cutMothDM, configDict)
    return _output

# For SL decays, we expect some missing particles so DM resolution will be degraded.
def makeXibStarSL(localName, inputSelections, decay, configDict = _my_immutable_config) :
    _cutMothDM = '(MM - CHILD(MM,1) - CHILD(MM,2) > %(XibStar_DMCutLower_SL)s) & (MM - CHILD(MM,1) - CHILD(MM,2) < %(XibStar_DMCutUpper_SL)s)' % configDict
    _output = makeXibStarBase(localName, inputSelections, decay, _cutMothDM, configDict)
    return _output

def makeXibStarRS(localName, inputSelections, configDict = _my_immutable_config) :
    _decay = '[ Sigma_b- -> Xi_b0 pi- ]cc'
    _output = makeXibStar(localName, inputSelections, _decay, configDict)
    return _output

def makeXibStarWS(localName, inputSelections, configDict = _my_immutable_config) :
    _decay = '[ Sigma_b+ -> Xi_b0 pi+ ]cc'
    _output = makeXibStar(localName, inputSelections, _decay, configDict)
    return _output

def makeXibStarRSSL(localName, inputSelections, configDict = _my_immutable_config) :
    _decay = '[ Sigma_b- -> Xi_b0 pi- ]cc'
    _output = makeXibStarSL(localName, inputSelections, _decay, configDict)
    return _output

def makeXibStarWSSL(localName, inputSelections, configDict = _my_immutable_config) :
    _decay = '[ Sigma_b+ -> Xi_b0 pi+ ]cc'
    _output = makeXibStarSL(localName, inputSelections, _decay, configDict)
    return _output

def filterGeneric(localName, inputSelection, filterString) :
    _filter = FilterDesktop(Code = filterString)
    return Selection(localName,
                     Algorithm = _filter,
                     RequiredSelections = [ inputSelection ] )


def makeGeneric(localName, inputSelections, decay, cutComb, cutMoth) :
    _combine = CombineParticles( DecayDescriptor = decay,
                                 CombinationCut = cutComb,
                                 MotherCut = cutMoth )
    return Selection ( localName,
                       Algorithm = _combine,
                       RequiredSelections = inputSelections)

def mergeLists(localName, inputSelections) :
    return MergedSelection ( localName,
                             RequiredSelections = inputSelections )
