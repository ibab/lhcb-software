"""
Stripping selection for nearly all electroweak penguin analyses. Includes same-sign combinations.
"""

__author__ = 'Paul Schaack, M. Kreps, T. Blake'
__date__ = '12/02/2011'
__version__ = '$Revision: 1.2 $'

__all__ = ( 'B2XMuMuConf', 'default_config' )


from Gaudi.Configuration import *
from LHCbKernel.Configuration import *  #check if needed


from GaudiConfUtils.ConfigurableGenerators import  CombineParticles, FilterDesktop

from PhysSelPython.Wrappers import Selection, AutomaticData, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from Configurables import SubstitutePID,  SubPIDMMFilter


#################
#
#  Define Cuts here
#
#################

default_config = {
    'NAME'       : 'B2XMuMu',
    'BUILDERTYPE' : 'B2XMuMuConf',
    'CONFIG'     :
    {
    'RelatedInfoTools'      : [
        {"Type" : "RelInfoConeVariables"
      , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM','CONEPT','CONEP','CONEPASYM','CONEDELTAETA','CONEDELTAPHI']
      , 'Location':'ConeIsoInfo'} 
      , {'Type' : 'RelInfoVertexIsolation'
      , "Variables" : ['VTXISONUMVTX', 'VTXISODCHI2ONETRACK', 'VTXISODCHI2MASSONETRACK','VTXISODCHI2TWOTRACK','VTXISODCHI2MASSTWOTRACK']
      , 'Location':'VtxIsoInfo'} 
      , {'Type': 'RelInfoVertexIsolationBDT'
      , 'Location':'VtxIsoBDTInfo' }
      #,  {"Type" : "RelInfoBs2MuMuIsolations"
      #, "Variables" : ['BSMUMUCDFISO', 'BSMUMUOTHERBMAG', 'BSMUMUOTHERBANGLE', 'BSMUMUOTHERBBOOSTMAG', 'BSMUMUOTHERBBOOSTANGLE', 'BSMUMUTRACKPLUSISO', 'BSMUMUTRACKMINUSISO', 'BSMUMUOTHERBTRACKS']
      #, "Location"  : "BSMUMUVARIABLES"
      #  }
    ]
    , 'KpiVXCHI2NDOF'      : 9.0           # dimensionless
    , 'MuonPID'            : -3.0          # dimensionless
    , 'DimuonUPPERMASS'    : 7100.0        # MeV
    , 'Pi0MINPT'           : 800.0         # MeV
    , 'Pi0ForOmegaMINPT'   : 500.0         # MeV
    , 'DplusLOWERMASS'     : 1600.0        # MeV
    , 'DplusUPPERMASS'     : 2300.0        # MeV      
    , 'KstarplusWINDOW'    : 300.0         # MeV      
    , 'KsWINDOW'           : 30.0          # MeV     
    , 'LambdaWINDOW'       : 30.0          # MeV    
    , 'LongLivedPT'        : 0.0           # MeV , used to be 500.0 MeV 
    , 'LongLivedTau'        : 2            # ps
    
    # A1 cuts
    , 'A1_Comb_MassLow'  :    0.0
    , 'A1_Comb_MassHigh' : 5550.0
    , 'A1_MassLow'       :    0.0
    , 'A1_MassHigh'      : 5500.0
    , 'A1_MinIPCHI2'     :    4.0
    , 'A1_FlightChi2'    :   25.0
    , 'A1_VtxChi2'       :   10.0
    , 'A1_Dau_MaxIPCHI2' :    9.0
    # From Bd2KstarMuMu line 
    ,'UseNoPIDsHadrons'          : True,
    
    # B cuts
    'B_Comb_MassLow'      : 4800.0,
    'B_Comb_MassHigh'     : 7100.0,
    'B_MassLow'           : 4900.0,
    'B_MassHigh'          : 7000.0,
    'B_VertexCHI2'        :    8.0,
    'B_IPCHI2'            :   16.0,
    'B_DIRA'              : 0.9999,
    'B_FlightCHI2'        :  121.0,
    'B_Dau_MaxIPCHI2'     :    9.0,
    
    # Daughter cuts
    'Dau_VertexCHI2'      :   12.0,
    'Dau_DIRA'            :   -0.9,
    
    # Kstar cuts
    'Kstar_Comb_MassLow'  :    0.0,
    'Kstar_Comb_MassHigh' : 6200.0,
    'Kstar_MassLow'       :    0.0,
    'Kstar_MassHigh'      : 6200.0,
    'Kstar_MinIPCHI2'     :    0.0,
    'Kstar_FlightChi2'    :    9.0,
    'Kstar_Dau_MaxIPCHI2' :    9.0,

    # Omega cuts         
    'Omega_MassWin'       :   100, #MeV
    'Omega_CombMassWin'   :   200, #MeV
    'OmegaChi2Prob'       : 0.00001,  # dimensionless               

    #K1->OmegaK cuts
    'K12OmegaK_MassLow'   :   300, #MeV
    'K12OmegaK_MassHigh'  :  2100, #MeV
    'K12OmegaK_CombMassLow'   :   400, #MeV
    'K12OmegaK_CombMassHigh'  :  2000, #MeV
    'K12OmegaK_VtxChi2'   : 10 ,
    
    # JPsi (dimu) cuts
    'Dimu_FlightChi2'     :   9.0,
    'Dimu_Dau_MaxIPCHI2'  :   9.0,

    # Track cuts
    'Track_GhostProb'     :    0.5,  
      
    # Hadron cuts
    'Hadron_MinIPCHI2'    :    6.0,

    # Muon cuts
    'Muon_MinIPCHI2'      :    9.0,
    'Muon_IsMuon'         :    True,
    'MuonNoPIDs_PIDmu'    :    0.0,

    # Wrong sign combinations
    'DimuonWS'            :   True,
    'HadronWS'            :   True,

    # GEC
    'SpdMult'             :  600,
    
    'HLT2_FILTER' : None ,
    'HLT1_FILTER' : None ,
    'L0DU_FILTER' : None , 

    'DECAYS'              :  ["B0 -> J/psi(1S) phi(1020)",                  
                              "[B0 -> J/psi(1S) K*(892)0]cc",               
                              "B0 -> J/psi(1S) rho(770)0",
                              "[B+ -> J/psi(1S) rho(770)+]cc",
                              "B0 -> J/psi(1S) f_2(1950)",                  
                              "B0 -> J/psi(1S) KS0",                        
                              "[B0 -> J/psi(1S) D~0]cc",                    
                              "[B+ -> J/psi(1S) K+]cc",                     
                              "[B+ -> J/psi(1S) pi+]cc",                    
                              "[B+ -> J/psi(1S) K*(892)+]cc",               
                              "[B+ -> J/psi(1S) D+]cc",                     
                              "[B+ -> J/psi(1S) D*(2010)+]cc",              
                              "[Lambda_b0 -> J/psi(1S) Lambda0]cc",         
                              "[Lambda_b0 -> J/psi(1S) Lambda(1520)0]cc",   
                              "B0 -> J/psi(1S) pi0",                        
                              "[B+ -> J/psi(1S) a_1(1260)+]cc",             
                              "[B+ -> J/psi(1S) K_1(1270)+]cc",             
                              "[B+ -> J/psi(1S) K_2(1770)+]cc",
                              "B0 -> J/psi(1S) K_1(1270)0",
                              "[B+ -> J/psi(1S) K_1(1400)+]cc", 
                              "B0 -> J/psi(1S) K_1(1400)0"
                             ]

    },
    'WGs'     : [ 'RD' ],
    'STREAMS' : ['Leptonic'] 
    }

#################
#
#  Make line here
#
#################



class B2XMuMuConf(LineBuilder) :

    __configuration_keys__ = (
        'RelatedInfoTools'
        , 'KpiVXCHI2NDOF'
        , 'MuonPID'
        , 'DimuonUPPERMASS'
        , 'Pi0MINPT'
        , 'Pi0ForOmegaMINPT'
        , 'DplusLOWERMASS'
        , 'DplusUPPERMASS'
        , 'KstarplusWINDOW'
        , 'KsWINDOW'
        , 'LambdaWINDOW'
        , 'LongLivedPT'
        , 'LongLivedTau'
        
        #, 'UseNoPIDsHadrons',
        
        # A1 cuts
        , 'A1_Comb_MassLow'
        , 'A1_Comb_MassHigh'
        , 'A1_MassLow'
        , 'A1_MassHigh'
        , 'A1_MinIPCHI2'
        , 'A1_FlightChi2'
        , 'A1_VtxChi2'  
        , 'A1_Dau_MaxIPCHI2',
          
        # Keys taken over from Bd2KstarMuMu line
        # Need to make sure there is no overlap with already existing ones
        'UseNoPIDsHadrons',
        
        # B cuts
        'B_Comb_MassLow',
        'B_Comb_MassHigh',
        'B_MassLow',
        'B_MassHigh',
        'B_VertexCHI2',
        'B_IPCHI2',
        'B_DIRA',
        'B_FlightCHI2',
        'B_Dau_MaxIPCHI2',
        
        # Daughter cuts
        'Dau_VertexCHI2',
        'Dau_DIRA',
        
        # Kstar cuts
        'Kstar_Comb_MassLow',
        'Kstar_Comb_MassHigh',
        'Kstar_MassLow',
        'Kstar_MassHigh',
        'Kstar_MinIPCHI2',
        'Kstar_FlightChi2',
        'Kstar_Dau_MaxIPCHI2',

        # Omega cuts         
        'Omega_MassWin',
        'Omega_CombMassWin',
        'OmegaChi2Prob',

        #K1->OmegaK cuts
        'K12OmegaK_MassLow',
        'K12OmegaK_MassHigh',
        'K12OmegaK_CombMassLow',
        'K12OmegaK_CombMassHigh',
        'K12OmegaK_VtxChi2',

        # JPsi (dimu) cuts
        'Dimu_FlightChi2',
        'Dimu_Dau_MaxIPCHI2',
        
        # Track cuts
        'Track_GhostProb',
        
        # Hadron cuts
        'Hadron_MinIPCHI2',
        
        # Muon cuts
        'Muon_MinIPCHI2',
        'Muon_IsMuon',
        'MuonNoPIDs_PIDmu',
        
        # Choose WS combinations
        'DimuonWS',
        'HadronWS',
        
        #GEC
        'SpdMult',
        'HLT1_FILTER',
        'HLT2_FILTER',
        'L0DU_FILTER' ,
        
        'DECAYS' 
        )
    
    def __init__(self, name, config) :


        LineBuilder.__init__(self, name, config)

        self.name = name
        
        # Bd2KstartMuMu cuts definitions
        self.BdCombCut = "(AM > %(B_Comb_MassLow)s * MeV) & (AM < %(B_Comb_MassHigh)s * MeV)" %config

        self.BdCut = "(abs(SUMQ) < 3) & " \
                     "(M > %(B_MassLow)s * MeV) & " \
                     "(M < %(B_MassHigh)s * MeV) & " \
                     "(VFASPF(VCHI2/VDOF) < %(B_VertexCHI2)s) & " \
                     "(BPVIPCHI2() < %(B_IPCHI2)s) & " \
                     "(BPVDIRA> %(B_DIRA)s) & " \
                     "(BPVVDCHI2 > %(B_FlightCHI2)s) & " \
                     "(MAXTREE(ISBASIC,MIPCHI2DV(PRIMARY))> %(B_Dau_MaxIPCHI2)s )" %config

        DaughterCuts = "(VFASPF(VCHI2/VDOF) < %(Dau_VertexCHI2)s) & " \
                       "(BPVDIRA> %(Dau_DIRA)s)" %config
        
        self.KstarCombCut = "(AM > %(Kstar_Comb_MassLow)s * MeV) & " \
                            "(AM < %(Kstar_Comb_MassHigh)s * MeV) & " \
                            "(ADOCACHI2CUT(20.,''))" %config
        self.KstarCut = DaughterCuts + " & (M > %(Kstar_MassLow)s * MeV) & " \
                        "(M < %(Kstar_MassHigh)s * MeV) & " \
                        "(BPVVDCHI2 > %(Kstar_FlightChi2)s) & " \
                        "(MIPCHI2DV(PRIMARY) > %(Kstar_MinIPCHI2)s) & " \
                        "(MAXTREE(ISBASIC,MIPCHI2DV(PRIMARY))> %(Kstar_Dau_MaxIPCHI2)s )" %config
        
        self.DiMuonCombCut = "(AM < %(DimuonUPPERMASS)s *MeV)" %config 
        self.DiMuonCut = DaughterCuts + " & (BPVVDCHI2 > %(Dimu_FlightChi2)s) & " \
                         "(MAXTREE(ISBASIC,MIPCHI2DV(PRIMARY))> %(Dimu_Dau_MaxIPCHI2)s )" %config
        

        self.TrackCuts = "(TRGHP < %(Track_GhostProb)s)" %config
        
        self.HadronCuts = "(MIPCHI2DV(PRIMARY) > %(Hadron_MinIPCHI2)s) & (HASRICH)" %config
        
        self.KaonCut = self.TrackCuts + " & " + self.HadronCuts
        self.PionCut = self.TrackCuts + " & " + self.HadronCuts
        
        self.MuonCut = self.TrackCuts + " & (MIPCHI2DV(PRIMARY) > %(Muon_MinIPCHI2)s) & (PIDmu> %(MuonPID)s)" %config

        self.Dimuon = self.__Dimuon__(config)

        self.Protons = self.__Protons__(config)
        self.Kaons = self.__Kaons__(config)
        self.Pions = self.__Pions__(config)       
        self.Kshort = self.__Kshort__(config)
        self.Dplus = self.__Dplus__(config)
        self.Dzero = self.__Dzero__(config)
        self.Lambda = self.__Lambda__(config)
        self.Pi0 = self.__Pi0__(config)
        self.Pi0ForOmega = self.__Pi0ForOmega__(config)
        self.Rho = self.__Rho__(self.Pions, config)
        self.F2 = self.__F2__(self.Rho, config)
        self.Phi = self.__Phi__(self.Rho, config)
        self.Kstar = self.__Kstar__(self.Rho, config)        
        self.Lambdastar = self.__Lambdastar__(self.Rho, config)
        self.Kstar2KsPi = self.__Kstar2KsPi__(self.Kshort, self.Pions, config)
        self.Kstar2KPi0 = self.__Kstar2KPi0__(self.Kaons, self.Pi0, config)
        self.Rho2PiPi0 = self.__Rho2PiPi0__(self.Pions, self.Pi0, config)
        self.Omega2PiPiPi0 = self.__Omega2PiPiPi0__(self.Pions, self.Pi0ForOmega, config)
        self.K12OmegaK = self.__K12OmegaK__(self.Kaons, self.Omega2PiPiPi0, config)
        self.K12OmegaKS = self.__K12OmegaKS__(self.Kshort, self.Omega2PiPiPi0, config)
        self.Dstar = self.__Dstar__(config)
        self.A1 = self.__A1__(self.Pions, config)
        self.K1 = self.__K1__(self.A1, config)
        self.K2 = self.__K2__(self.A1, config)
        self.K10 = self.__K10__(self.Kshort,self.Pions,config)

        self.AvailableDaughters = {
            'rho(770)0'     : [ self.Rho   ] ,
            'K*(892)0'      : [ self.Kstar ] ,
            'phi(1020)'     : [ self.Phi   ] ,
            'f_2(1950)'     : [ self.F2    ] ,
            'KS0'           : [ self.Kshort] ,
            'D~0'           : [ self.Dzero ] ,
            'K+'            : [ self.Kaons ] ,
            'pi+'           : [ self.Pions ] ,
            'K*(892)+'      : [ self.Kstar2KsPi, self.Kstar2KPi0 ],
            'rho(770)+'     : [ self.Rho2PiPi0 ],
            'omega(782)'    : [ self.Omega2PiPiPi0 ],
            'D+'            : [ self.Dplus ] ,
            'D*(2010)+'     : [ self.Dstar ] ,
            'Lambda0'       : [ self.Lambda ] ,
            'Lambda(1520)0' : [ self.Lambdastar ] ,
            'pi0'           : [ self.Pi0 ] ,
            'a_1(1260)+'    : [ self.A1 ],
            'K_1(1270)+'    : [ self.K1 ],
            'K_2(1770)+'    : [ self.K2 ],
            'K_1(1270)0'    : [ self.K10 ],
            'K_1(1400)+'    : [ self.K12OmegaK ],
            'K_1(1400)0'    : [ self.K12OmegaKS ]
            }

        self.DeclaredDaughters = [] 
        
        for d in config['DECAYS']:
            for k in self.AvailableDaughters.keys():
                if k in d: self.DeclaredDaughters += self.AvailableDaughters.pop(k) 
                
        
        self.Bs = self.__Bs__( self.Dimuon,
                               daughters = self.DeclaredDaughters,  
                               conf = config)

        self.FilterSPD = {
                'Code' : " ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < %(SpdMult)s )" %config ,
                'Preambulo' : [
                "from LoKiNumbers.decorators import *", "from LoKiCore.basic import LHCb"
                ]
                }
        
        if config['DECAYS']:
            # standard lines
            self.line = StrippingLine(
                self.name+"_Line",
                prescale = 1,
                RelatedInfoTools = config['RelatedInfoTools'],
                HLT2 = config['HLT2_FILTER'],
                HLT1 = config['HLT1_FILTER'],
                L0DU = config['L0DU_FILTER'],
                FILTER = self.FilterSPD,
                algos=[self.Bs],
                MDSTFlag=True
                )
        
            self.registerLine(self.line)


        
    def __Dimuon__(self, conf):
        '''
        Create a new dimuon from scratch
        '''
        wsCombinations=conf['DimuonWS']
        from  GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineDiMuon = CombineParticles()
        if wsCombinations == True:
            CombineDiMuon.DecayDescriptors = ["J/psi(1S) -> mu- mu+", "J/psi(1S) -> mu+ mu+", " J/psi(1S) -> mu- mu-"]
        else:
            CombineDiMuon.DecayDescriptors = ["J/psi(1S) -> mu- mu+"]
        CombineDiMuon.DaughtersCuts = { "mu+" : self.MuonCut, "mu-" : self.MuonCut }
        CombineDiMuon.CombinationCut = self.DiMuonCombCut 
        CombineDiMuon.MotherCut     = self.DiMuonCut

        IsMuonFlag = conf['Muon_IsMuon'] 
        from StandardParticles import StdAllLooseMuons, StdAllVeryLooseMuons
        Muons = StdAllLooseMuons if IsMuonFlag else StdAllVeryLooseMuons
         
        from PhysSelPython.Wrappers import Selection
        SelDiMuon = Selection("Sel_" + self.name + "_DiMuon", Algorithm = CombineDiMuon, RequiredSelections = [ Muons ] )
        return SelDiMuon


    
    def __Kaons__(self, conf):
        """
        Filter kaons from StdAllNoPIDsKaons or StdAllLooseKaons
        """  

        _kaons = None
        UseNoPIDsHadrons=conf['UseNoPIDsHadrons']
        if (UseNoPIDsHadrons == True):
            _kaons = AutomaticData(Location = 'Phys/StdAllNoPIDsKaons/Particles')
        else:
            _kaons = AutomaticData(Location = 'Phys/StdAllLooseKaons/Particles')

        _filter = FilterDesktop(Code = self.TrackCuts+" & "+self.HadronCuts)
        _sel = Selection("Selection_"+self.name+"_StdAllNoPIDsKaons",
                         RequiredSelections = [ _kaons ] ,
                         Algorithm = _filter)
        return _sel

    def __Protons__(self, conf):
        """
        Filter protons from StdAllLooseProtons or StdAllLooseProtons
        """  

        _protons = None
        UseNoPIDsHadrons=conf['UseNoPIDsHadrons']
        if (UseNoPIDsHadrons == True):
            _protons = AutomaticData(Location = 'Phys/StdAllNoPIDsProtons/Particles')
        else:
            _protons = AutomaticData(Location = 'Phys/StdAllLooseProtons/Particles')
        _filter = FilterDesktop(Code = self.TrackCuts+" & "+self.HadronCuts)
        _sel = Selection("Selection_"+self.name+"_StdAllNoPIDsProtons",
                         RequiredSelections = [ _protons ] ,
                         Algorithm = _filter)
        return _sel



    def __Pions__(self, conf):
        """
        Filter pions from StdAllNoPIDsPions or StdAllLoosePions
        """  

        _pions = None
        UseNoPIDsHadrons=conf['UseNoPIDsHadrons']
        if (UseNoPIDsHadrons == True):
            _pions = AutomaticData(Location = 'Phys/StdAllNoPIDsPions/Particles')
        else:
            _pions = AutomaticData(Location = 'Phys/StdAllLoosePions/Particles')
        _filter = FilterDesktop(Code = self.TrackCuts+" & "+self.HadronCuts)
        _sel = Selection("Selection_"+self.name+"_StdAllNoPIDsPions",
                         RequiredSelections = [ _pions ] ,
                         Algorithm = _filter)
        return _sel
 
    def __A1CombCut__(self, conf):
        """
        Returns the A1 cut string
        """
        _A1CombCut = "(AM > %(A1_Comb_MassLow)s * MeV) &"\
        "(AM < %(A1_Comb_MassHigh)s * MeV) & "\
        "(ADOCACHI2CUT(20.,'')) &" \
        "(AHASCHILD(MIPCHI2DV(PRIMARY) > %(A1_Dau_MaxIPCHI2)s ))" %conf
        return _A1CombCut
        
    def __A1Cut__(self, conf):
        """
        Returns the A1 cut string
        """
        _A1Cut = "(M > %(A1_MassLow)s * MeV) &"\
        "(M < %(A1_MassHigh)s * MeV) & "\
        "(VFASPF(VCHI2PDOF) < %(A1_VtxChi2)s) & "\
        "(BPVVDCHI2 > %(A1_FlightChi2)s) & "\
        "(MIPCHI2DV(PRIMARY) > %(A1_MinIPCHI2)s) " %conf 
        return _A1Cut
        

    def __KsCuts__(self, conf):
        """
        Returns the KaonPion cut string
        """
        _KsCuts = """
        (ADMASS('KS0') < %(KsWINDOW)s* MeV) &
        (PT > %(LongLivedPT)s *MeV) &
        (BPVLTIME() > %(LongLivedTau)s *ps)
        """ % conf
        return _KsCuts


    def __Kshort__(self, conf):
        """
        Filter kshort from StdLooseKshort
        """  
        _ksdd = AutomaticData(Location = 'Phys/StdLooseKsDD/Particles')
        _ksll = AutomaticData(Location = 'Phys/StdLooseKsLL/Particles')
        _filter_ksdd = FilterDesktop(Code = self.__KsCuts__(conf))
        _filter_ksll = FilterDesktop(Code = self.__KsCuts__(conf))        
        _selksdd = Selection("Selection_"+self.name+"_Ksdd",
                             RequiredSelections = [ _ksdd ] ,
                             Algorithm = _filter_ksdd)
        _selksll = Selection("Selection_"+self.name+"_Ksll",
                             RequiredSelections = [ _ksll ] ,
                             Algorithm = _filter_ksll)

        _sel = MergedSelection("Selection_"+self.name+"_Kshort",
                               RequiredSelections = [ _selksdd, _selksll ])
        return _sel

    def __LambdaCuts__(self, conf):
        """
        Returns the Lambda cut string
        """
        _LambdaCuts = """
        (ADMASS('Lambda0') < %(LambdaWINDOW)s *MeV) & 
        (PT > %(LongLivedPT)s *MeV) &
        (BPVLTIME() > %(LongLivedTau)s *ps)
        """ % conf
        return _LambdaCuts

    def __Lambda__(self, conf):
        """
        Filter Lambda from StdVeryLooseLambdaLL and StdLooseLambdaDD
        """  
        _lambdadd = AutomaticData(Location = 'Phys/StdLooseLambdaDD/Particles')
        _lambdall = AutomaticData(Location = 'Phys/StdVeryLooseLambdaLL/Particles')
        _filter_lambdadd = FilterDesktop(Code = self.__LambdaCuts__(conf))
        _filter_lambdall = FilterDesktop(Code = self.__LambdaCuts__(conf))        
        _sellambdadd = Selection("Selection_"+self.name+"_Lambdadd",
                             RequiredSelections = [ _lambdadd ] ,
                             Algorithm = _filter_lambdadd)
        _sellambdall = Selection("Selection_"+self.name+"_Lambdall",
                             RequiredSelections = [ _lambdall ] ,
                             Algorithm = _filter_lambdall)
        _sel = MergedSelection("Selection_"+self.name+"_Lambda",
                               RequiredSelections = [ _sellambdadd, _sellambdall ])
        return _sel

    def __Pi0Cuts__(self, conf):
        """
        Returns the KaonPion cut string
        """
        _Pi0Cuts = """
        (PT > %(Pi0MINPT)s )
        """ % conf
        return _Pi0Cuts



    def __Pi0__(self, conf):
        """
        Filter Pi0 from Std Pi0
        """  
        _pi0resolved = AutomaticData(Location = 'Phys/StdLooseResolvedPi0/Particles')
        _pi0merged = AutomaticData(Location = 'Phys/StdLooseMergedPi0/Particles')
        _filter_pi0resolved = FilterDesktop(Code = self.__Pi0Cuts__(conf) )
        #+ " & ( 250*MeV < MINTREE('gamma' == ID, PT))"  )
        _filter_pi0merged = FilterDesktop(Code = self.__Pi0Cuts__(conf)  )        
        _selpi0resolved = Selection("Selection_"+self.name+"_pi0resolved",
                             RequiredSelections = [ _pi0resolved ] ,
                             Algorithm = _filter_pi0resolved)
        _selpi0merged = Selection("Selection_"+self.name+"_pi0merged",
                             RequiredSelections = [ _pi0merged ] ,
                             Algorithm = _filter_pi0merged)
        _sel = MergedSelection("Selection_"+self.name+"_pi0",
                              RequiredSelections = [ _selpi0resolved,_selpi0merged ])
        return _sel


    def __Pi0ForOmegaCuts__(self, conf):
        """
        Returns the KaonPion cut string
        """
        
        _Pi0Cuts = """
        (PT > %(Pi0ForOmegaMINPT)s )
        """ % conf
        return _Pi0Cuts

    
    def __Pi0ForOmega__(self, conf):
        """
        Filter Pi0 from Std Pi0
        """
        _pi0resolved = AutomaticData(Location = 'Phys/StdLooseResolvedPi0/Particles')
        _pi0merged = AutomaticData(Location = 'Phys/StdLooseMergedPi0/Particles')
        _filter_pi0resolved = FilterDesktop(Code = self.__Pi0ForOmegaCuts__(conf) )
        _filter_pi0merged = FilterDesktop(Code = self.__Pi0ForOmegaCuts__(conf)  )
        _selpi0resolved = Selection("Selection_"+self.name+"_pi0foromegaresolved",
                                    RequiredSelections = [ _pi0resolved ] ,
                                    Algorithm = _filter_pi0resolved)
        _selpi0merged = Selection("Selection_"+self.name+"_pi0foromegamerged",
                                  RequiredSelections = [ _pi0merged ] ,
                                  Algorithm = _filter_pi0merged)
        _sel = MergedSelection("Selection_"+self.name+"_pi0foromega",
                               RequiredSelections = [ _selpi0resolved,_selpi0merged ])
        return _sel

    
    def __Kstar2KPi0__( self, Kaons, Pi0, conf):
        """
        Make K*(892)+ -> K+ pi0 
        """
        _kstar2kpizero = CombineParticles()
        _kstar2kpizero.DecayDescriptor = "[K*(892)+ -> K+ pi0]cc"
        _kstar2kpizero.MotherCut = "(ADMASS('K*(892)+') < %(KstarplusWINDOW)s *MeV)" % conf

        _kstarConf = _kstar2kpizero.configurable("Combine_"+self.name+"_KPi0")
        _kstarConf.ParticleCombiners.update ( { '' : 'LoKi::VertexFitter' } )
                                                 
        _selKSTAR2KPIZERO = Selection( "Selection_"+self.name+"_Kstar2kpizero",
                                       Algorithm = _kstarConf,
                                       RequiredSelections = [ Kaons, Pi0 ] )
        return _selKSTAR2KPIZERO
        


    def __Rho2PiPi0__( self, Pions, Pi0, conf):
        """
        Make rho(770)+ -> pi+ pi0 
        """
        _rho2pipizero = CombineParticles()
        _rho2pipizero.DecayDescriptor = "[rho(770)+ -> pi+ pi0]cc"
        _rho2pipizero.MotherCut = "(ADMASS('rho(770)+') < %(KstarplusWINDOW)s *MeV)" % conf

        _rhoConf = _rho2pipizero.configurable("Combine_"+self.name+"_PiPi0")
        _rhoConf.ParticleCombiners.update ( { '' : 'LoKi::VertexFitter' } )
                                                 
        _selRHO2PIPIZERO = Selection( "Selection_"+self.name+"_rho2pipizero",
                                       Algorithm = _rhoConf,
                                       RequiredSelections = [ Pions, Pi0 ] )
        return _selRHO2PIPIZERO


    def __Omega2PiPiPi0__( self, Pions, Pi0, conf):
        """
        Make omega -> pi+ pi- pi0 
        """
        _omega2pipipizero = CombineParticles()
        _omega2pipipizero.DecayDescriptor = "omega(782) -> pi+ pi- pi0"
        _omega2pipipizero.CombinationCut = "(ADAMASS('omega(782)') < %(Omega_CombMassWin)s * MeV) " %conf
                         #"(ADOCACHI2CUT(20.,''))" 
                                                
        _omega2pipipizero.MotherCut = "(ADMASS('omega(782)') < %(Omega_MassWin)s *MeV) & " \
                                      "(VFASPF(VPCHI2)> %(OmegaChi2Prob)s )"  %conf
                         
                                 

        _omegaConf = _omega2pipipizero.configurable("Combine_"+self.name+"_PiPiPi0")
        _omegaConf.ParticleCombiners.update ( { '' : 'OfflineVertexFitter:PUBLIC' } )
                                                 
        _selOMEGA2PIPIPIZERO = Selection( "Selection_"+self.name+"_omega2pipipizero",
                                       Algorithm = _omegaConf,
                                       RequiredSelections = [ Pions, Pi0 ] )
        return _selOMEGA2PIPIPIZERO


    def __K12OmegaK__(self, K, Omega, conf):
        """
        Make a K1-> Omega(782) K+
        """      
        _k12omegak = CombineParticles()
        _k12omegak.DecayDescriptor = "[K_1(1400)+ -> K+ omega(782)]cc"
        _k12omegak.CombinationCut = "(AM > %(K12OmegaK_MassLow)s * MeV) & (AM < %(K12OmegaK_MassHigh)s * MeV)" %conf
        _k12omegak.MotherCut = "(M > %(K12OmegaK_CombMassLow)s * MeV) & (M < %(K12OmegaK_CombMassHigh)s * MeV) & (VFASPF(VCHI2PDOF) < %(K12OmegaK_VtxChi2)s)" %conf
        
        #_k12omegakConf = _k12omegak.configurable("Combine_"+self.name+"_OmegaK")
        #_k12omegakConf.ParticleCombiners.update ( { '' : 'OfflineVertexFitter:PUBLIC' } )                   
        
        _selK12OMEGAK = Selection( "Selection_"+self.name+"_k12omegak",
                                     Algorithm = _k12omegak,
                                     RequiredSelections = [ K, Omega ] )
        return _selK12OMEGAK


    def __K12OmegaKS__(self, Kshort, Omega, conf):
        """
        Make a K1-> Omega(782) K+
        """      
        _k12omegaks = CombineParticles()
        _k12omegaks.DecayDescriptor = "K_1(1400)0 -> KS0 omega(782)"
        _k12omegaks.CombinationCut = "(AM > %(K12OmegaK_MassLow)s * MeV) & (AM < %(K12OmegaK_MassHigh)s * MeV)" %conf
        _k12omegaks.MotherCut = "(M > %(K12OmegaK_CombMassLow)s * MeV) & " \
                                "(M < %(K12OmegaK_CombMassHigh)s * MeV) & " \
                                "(VFASPF(VCHI2PDOF) < %(K12OmegaK_VtxChi2)s)" %conf

        #_k12omegaksConf = _k12omegaks.configurable("Combine_"+self.name+"_OmegaKs")
        #_k12omegaksConf.ParticleCombiners.update ( { '' : 'OfflineVertexFitter:PUBLIC' } )
                
        _selK12OMEGAKS = Selection( "Selection_"+self.name+"_k12omegaks",
                                     Algorithm = _k12omegaks,
                                     RequiredSelections = [ Kshort, Omega ] )
        return _selK12OMEGAKS


    def __KpiCuts__(self, conf):
        """
        Returns the Kpi cut string
        """
        _KpiCuts = """
        (VFASPF(VCHI2PDOF)< %(KpiVXCHI2NDOF)s )
        """ % conf
        return _KpiCuts


    def __DplusCuts__(self, conf):
        """
        Returns the Kpi cut string
        """
        _DplusCuts = """
        (M > %(DplusLOWERMASS)s *MeV) &
        (M < %(DplusUPPERMASS)s *MeV)
        """ % conf
        return _DplusCuts


    def __Kstar2KsPi__(self, Kshort, Pions, conf):
        """
        Make a kstarplus
        """      
        _kstar2kspi = CombineParticles()
        _kstar2kspi.DecayDescriptor = "[K*(892)+ -> KS0 pi+]cc"
        _kstar2kspi.CombinationCut = self.KstarCombCut
        _kstar2kspi.MotherCut = self.KstarCut

        _selKSTAR2KSPI = Selection( "Selection_"+self.name+"_Kstar2kspi",
                                     Algorithm = _kstar2kspi,
                                     RequiredSelections = [ Kshort, Pions ] )
        return _selKSTAR2KSPI
    

    def __Phi__(self, Rho, conf):
        """
        Make a phi through substitution 
        """      

        phiSubMMZAlg = SubPIDMMFilter(self.name+"_PhiSubMMZ_Alg", Code= "DECTREE('rho(770)0 -> pi+ pi-')", MinMM=0, MaxMM=6050, PIDs = [ [ 'K+', 'K-'] ]  )

        phiSubMMPAlg = SubPIDMMFilter(self.name+"_PhiSubMMP_Alg", Code= "DECTREE('rho(770)0 -> pi+ pi+')", MinMM=0, MaxMM=6050, PIDs = [ [ 'K+', 'K+'] ]  )     

        phiSubMMMAlg = SubPIDMMFilter(self.name+"_PhiSubMMM_Alg", Code= "DECTREE('rho(770)0 -> pi- pi-')", MinMM=0, MaxMM=6050, PIDs = [ [ 'K-', 'K-'] ]  )   

        phiSubMMZSel = Selection( self.name+"_PhiSubMMZ_Sel", Algorithm = phiSubMMZAlg, RequiredSelections = [ Rho ] )

        phiSubMMPSel = Selection( self.name+"_PhiSubMMP_Sel", Algorithm = phiSubMMPAlg, RequiredSelections = [ Rho ] )
        
        phiSubMMMSel = Selection( self.name+"_PhiSubMMM_Sel", Algorithm = phiSubMMMAlg, RequiredSelections = [ Rho ] )

        phiMerge = MergedSelection( self.name+"_PhiMerge", RequiredSelections = [ phiSubMMZSel, phiSubMMPSel, phiSubMMMSel ] ) 
        
        phiSubAlg = SubstitutePID( self.name+"_PhiSub_Alg", Code = "ALL",  MaxChi2PerDoF = -666 )

        #(DECTREE('rho(770)0 -> K+ K-')) | (DECTREE('rho(770)0 -> K+ K+')) | (DECTREE('rho(770)0 -> K- K-'))",
        
        phiSubAlg.Substitutions = {
            'rho(770)0 -> K+ K-' : 'phi(1020)' ,
            'rho(770)0 -> K+ K+' : 'phi(1020)' ,
            'rho(770)0 -> K- K-' : 'phi(1020)'
            }
        
        phiSubSel =  Selection( self.name+"_PhiSub_Sel", Algorithm = phiSubAlg, RequiredSelections = [ phiMerge ] )

        phiFilterAlg = FilterDesktop( Code = "(ABSID=='phi(1020)')" )

        phiFilterSel = Selection( self.name + "_PhiFilter", Algorithm = phiFilterAlg, RequiredSelections = [ phiSubSel ] )
        
        return phiFilterSel

    
    def __F2__(self, Rho, conf):
        """
        Make a f_2(1950) -> p pbar
        """
        f2SubMMZAlg = SubPIDMMFilter(self.name+"_F2SubMMZ_Alg", Code= "DECTREE('rho(770)0 -> pi+ pi-')", MinMM=0, MaxMM=6050, PIDs = [ [ 'p+', 'p~-'] ]  )

        f2SubMMPAlg = SubPIDMMFilter(self.name+"_F2SubMMP_Alg", Code= "DECTREE('rho(770)0 -> pi+ pi+')", MinMM=0, MaxMM=6050, PIDs = [ [ 'p+', 'p+'] ]  )     

        f2SubMMMAlg = SubPIDMMFilter(self.name+"_F2SubMMM_Alg", Code= "DECTREE('rho(770)0 -> pi- pi-')", MinMM=0, MaxMM=6050, PIDs = [ [ 'p~-', 'p~-'] ]  )   

        f2SubMMZSel = Selection( self.name+"_F2SubMMZ_Sel", Algorithm = f2SubMMZAlg, RequiredSelections = [ Rho ] )

        f2SubMMPSel = Selection( self.name+"_F2SubMMP_Sel", Algorithm = f2SubMMPAlg, RequiredSelections = [ Rho ] )
        
        f2SubMMMSel = Selection( self.name+"_F2SubMMM_Sel", Algorithm = f2SubMMMAlg, RequiredSelections = [ Rho ] )
        
        f2Merge    = MergedSelection( self.name+"_F2Merge", RequiredSelections = [ f2SubMMZSel, f2SubMMPSel, f2SubMMMSel ] ) 

        f2SubAlg = SubstitutePID( self.name+"_F2Sub_Alg", 
                                   Code = "ALL" , #(DECTREE('rho(770)0 -> p+ p~-')) | (DECTREE('rho(770)0 -> p+ p+')) | (DECTREE('rho(770)0 -> p~- p~-'))",
                                   MaxChi2PerDoF = -666 )
        
        f2SubAlg.Substitutions = {
            'rho(770)0 -> p+ p~-' : 'f_2(1950)' ,
            'rho(770)0 -> p+ p+'   : 'f_2(1950)' ,
            'rho(770)0 -> p~- p~-' : 'f_2(1950)'
            }

        f2SubSel =  Selection( self.name+"_F2Sub_Sel", Algorithm = f2SubAlg, RequiredSelections = [ f2Merge ] )

        f2FilterAlg = FilterDesktop( Code = "(ABSID=='f_2(1950)')" )
        
        f2FilterSel = Selection( self.name + "_F2Filter", Algorithm = f2FilterAlg, RequiredSelections = [ f2SubSel ] )
        
        return f2FilterSel
    
    
    def __Rho__(self, Pions, conf):
        """
        Make a rho
        """      
        wsCombinations=conf['HadronWS']
        _rho2pipi = CombineParticles()
        if wsCombinations == True:
            _rho2pipi.DecayDescriptors = [ "rho(770)0 -> pi+ pi-", "rho(770)0 -> pi+ pi+" , "rho(770)0 -> pi- pi-" ]
        else:
            _rho2pipi.DecayDescriptors = [ "rho(770)0 -> pi+ pi-"]
        _rho2pipi.CombinationCut = self.KstarCombCut
        _rho2pipi.MotherCut = self.KstarCut

        _selRHO2PIPI = Selection( "Selection_"+self.name+"_Rho",
                                     Algorithm = _rho2pipi,
                                     RequiredSelections = [ Pions ] )
        return _selRHO2PIPI


    def __Kstar__(self, Rho, conf):
        """
        Make a kstar
        """
                
        kstarSubMMZAlg = SubPIDMMFilter(self.name+"_KstarSubMMZ_Alg", Code= "DECTREE('rho(770)0 -> pi+ pi-')", MinMM=0, MaxMM=6050, PIDs = [ [ 'K+', 'pi-'], ['pi+', 'K-' ] ])
        
        kstarSubMMPAlg = SubPIDMMFilter(self.name+"_KstarSubMMP_Alg", Code= "DECTREE('rho(770)0 -> pi+ pi+')", MinMM=0, MaxMM=6050, PIDs = [ [ 'K+', 'pi+'], ['pi+', 'K+' ] ])     

        kstarSubMMMAlg = SubPIDMMFilter(self.name+"_KstarSubMMM_Alg", Code= "DECTREE('rho(770)0 -> pi- pi-')", MinMM=0, MaxMM=6050, PIDs = [ [ 'K-', 'pi-'], ['pi-', 'K-' ] ]) 

        kstarSubMMZSel = Selection( self.name+"_KstarSubMMZ_Sel", Algorithm = kstarSubMMZAlg, RequiredSelections = [ Rho ] )

        kstarSubMMPSel = Selection( self.name+"_KstarSubMMP_Sel", Algorithm = kstarSubMMPAlg, RequiredSelections = [ Rho ] )
        
        kstarSubMMMSel = Selection( self.name+"_KstarSubMMM_Sel", Algorithm = kstarSubMMMAlg, RequiredSelections = [ Rho ] )
        
        kstarMerge     = MergedSelection( self.name+"_KstarMerge", RequiredSelections = [ kstarSubMMZSel, kstarSubMMPSel, kstarSubMMMSel ] )

        kstarSubAlg    = SubstitutePID( self.name+"_KstarSub_Alg", Code = "ALL", MaxChi2PerDoF = -666 )

        #kstarSubAlg.Code = "ALL" #(DECTREE('rho(770)0 -> K+ pi-')) | (DECTREE('rho(770)0 -> K- pi+')) | (DECTREE('rho(770)0 -> K+ pi+')) | (DECTREE('rho(770)0 -> K- pi-'))" 
        
        kstarSubAlg.Substitutions = {
            'rho(770)0 -> K+ pi-' : 'K*(892)0',
            'rho(770)0 -> K+ pi+' : 'K*(892)0',
            'rho(770)0 -> pi+ K-' : 'K*(892)~0',
            'rho(770)0 -> K- pi-' : 'K*(892)~0'
            }
        
        
        kstarSubSel =  Selection( self.name+"_KstarSub_Sel", Algorithm = kstarSubAlg, RequiredSelections = [ kstarMerge ] )

        kstarFilterAlg = FilterDesktop( Code = "(ABSID=='K*(892)0')" )
        
        kstarFilterSel = Selection( self.name + "_KstarFilter", Algorithm = kstarFilterAlg, RequiredSelections = [ kstarSubSel ] )
        
        return kstarFilterSel
       

    def __A1__(self, Pions, conf): 
        """
        Make A1 objects:
            a_1 - > pi pi pi 
        """      
        # First make an a_1(1260)+ selection
        _a12pipipi = CombineParticles()
        _a12pipipi.DecayDescriptors = [ "[a_1(1260)+ -> pi+ pi+ pi-]cc" ]
        _a12pipipi.CombinationCut = self.__A1CombCut__(conf)
        _a12pipipi.MotherCut = self.__A1Cut__(conf)

        selA1 = Selection( "Selection_"+self.name+"_a1",
                           Algorithm=_a12pipipi,
                           RequiredSelections=[Pions] )
        
        return selA1 

    def __K1__(self, A1, conf): 
        """
        SubPID:
            K_1 -> K pi pi
        """      
        #replace head with K_1(1270)
        _a1k1_SubPID = SubstitutePID(self.name+"_a1k1_SubPID", Code="(DECTREE('a_1(1260)+ -> pi+ pi+ pi-')) | (DECTREE('a_1(1260)- -> pi- pi- pi+'))",
                                     Substitutions={'a_1(1260)+ -> pi+ pi+ pi-' : 'K_1(1270)+', 'a_1(1260)- -> pi- pi- pi+' : 'K_1(1270)-'}, MaxChi2PerDoF=-666)
        _sel_k1 =  Selection(self.name+"_a1k1_SubPIDAlg",
                             Algorithm = _a1k1_SubPID, RequiredSelections = [A1])
        #replace daughters for K_1(1270)+, order is important for this tool
        _a1k1_SubPIDp = SubPIDMMFilter(self.name+"_a1k1_SubPIDp", Code="DECTREE('K_1(1270)+ -> pi+ pi+ pi-')",
                                       MinMM=0, MaxMM=conf['A1_Comb_MassHigh'], PIDs = [['K+','pi+','pi-'], ['pi+','K+','pi-']] ) 
        _sel_k1p =  Selection(self.name+"_a1k1_SubPIDAlgp",
                              Algorithm = _a1k1_SubPIDp, RequiredSelections = [_sel_k1])
        #replace daughters for K_1(1270)-
        _a1k1_SubPIDm = SubPIDMMFilter(self.name+"_a1k1_SubPIDm", Code="DECTREE('K_1(1270)- -> pi- pi- pi+')",
                                       MinMM=0, MaxMM=conf['A1_Comb_MassHigh'], PIDs = [['K-','pi-','pi+'], ['pi-','K-','pi+']] ) 
        _sel_k1m =  Selection(self.name+"_a1k1_SubPIDAlgm",
                              Algorithm = _a1k1_SubPIDm, RequiredSelections = [_sel_k1])

        pick = Selection(self.name+"_K1_PickDecay",
                     Algorithm = FilterDesktop( Code = "(DECTREE('K_1(1270)+ -> K+ pi+ pi-')) | (DECTREE('K_1(1270)- -> pi+ K- pi-'))" ),
                     RequiredSelections = [_sel_k1p, _sel_k1m])

        return pick

    def __K2__(self, A1, conf): 
        """
        SubPID:
            K_2 -> K K K
        """      
        #replace head with K_2(1770)
        _a1k2_SubPID = SubstitutePID(self.name+"_a1k2_SubPID", Code="(DECTREE('a_1(1260)+ -> pi+ pi+ pi-')) | (DECTREE('a_1(1260)- -> pi- pi- pi+'))",
                                      Substitutions={'a_1(1260)+ -> pi+ pi+ pi-' : 'K_2(1770)+', 'a_1(1260)- -> pi- pi- pi+' : 'K_2(1770)-'}, MaxChi2PerDoF=-666)
        _sel_k2 =  Selection(self.name+"_a1k2_SubPIDAlg",
                             Algorithm = _a1k2_SubPID, RequiredSelections = [A1])
        #replace daughters for K_2(1770)+, order is important for this tool
        _a1k2_SubPIDp = SubPIDMMFilter(self.name+"_a1k2_SubPIDp", Code="DECTREE('K_2(1770)+ -> pi+ pi+ pi-')",
                                       MinMM=0, MaxMM=conf['A1_Comb_MassHigh'], PIDs = [['K+','K+','K-']] )        
        _sel_k2p =  Selection(self.name+"_a1k2_SubPIDAlgp",
                              Algorithm = _a1k2_SubPIDp, RequiredSelections = [_sel_k2])
        #replace daughters for K_2(1770)-
        _a1k2_SubPIDm = SubPIDMMFilter(self.name+"_a1k2_SubPIDm", Code="DECTREE('K_2(1770)- -> pi- pi- pi+')",
                                       MinMM=0, MaxMM=conf['A1_Comb_MassHigh'], PIDs = [['K-','K-','K+']] )        
        _sel_k2m =  Selection(self.name+"_a1k2_SubPIDAlgm",
                              Algorithm = _a1k2_SubPIDm, RequiredSelections = [_sel_k2])

        pick = Selection(self.name+"_K2_PickDecay",
                     Algorithm = FilterDesktop( Code = "(DECTREE('K_2(1770)+ -> K- K+ K+')) | (DECTREE('K_2(1770)- -> K+ K- K-'))" ),
                     RequiredSelections = [_sel_k2p, _sel_k2m])

        return pick

    def __K10__(self,Kshort,Pions,conf):
        """
        K_1 0 -> Ks pi+ pi-
        """

        _k102kspipi = CombineParticles()
        _k102kspipi.DecayDescriptors = [ "K_1(1270)0 -> KS0 pi+ pi-" ]
        _k102kspipi.CombinationCut = self.__A1CombCut__(conf)
        _k102kspipi.MotherCut = self.__A1Cut__(conf)

        _sel_k10 = Selection( "Selection_"+self.name+"_k10",
                              Algorithm=_k102kspipi,
                              RequiredSelections=[Kshort,Pions] )
        
        pick = Selection(self.name+"_K10_PickDecay",
                     Algorithm = FilterDesktop( Code = "(DECTREE('K_1(1270)0 -> KS0 pi+ pi-'))" ),
                     RequiredSelections = [_sel_k10])

        return pick

        


    def __Lambdastar__(self, Rho, conf):
        """
        Make a Lambdastar
        """
        lambdastarSubMMZAlg = SubPIDMMFilter(self.name+"_LambdastarSubMMZ_Alg", Code= "DECTREE('rho(770)0 -> pi+ pi-')", MinMM=0, MaxMM=6050, PIDs = [ [ 'p+', 'K-'], ['K+', 'p~-' ] ])
        
        lambdastarSubMMPAlg = SubPIDMMFilter(self.name+"_LambdastarSubMMP_Alg", Code= "DECTREE('rho(770)0 -> pi+ pi+')", MinMM=0, MaxMM=6050, PIDs = [ [ 'p+', 'K+'], ['K+', 'p+' ] ])     

        lambdastarSubMMMAlg = SubPIDMMFilter(self.name+"_LambdastarSubMMM_Alg", Code= "DECTREE('rho(770)0 -> pi- pi-')", MinMM=0, MaxMM=6050, PIDs = [ [ 'p~-', 'K-'], ['K-', 'p~-' ] ]) 

        lambdastarSubMMZSel = Selection( self.name+"_LambdastarSubMMZ_Sel", Algorithm = lambdastarSubMMZAlg, RequiredSelections = [ Rho ] )

        lambdastarSubMMPSel = Selection( self.name+"_LambdastarSubMMP_Sel", Algorithm = lambdastarSubMMPAlg, RequiredSelections = [ Rho ] )
        
        lambdastarSubMMMSel = Selection( self.name+"_LambdastarSubMMM_Sel", Algorithm = lambdastarSubMMMAlg, RequiredSelections = [ Rho ] )
        
        lambdastarMerge     = MergedSelection( self.name+"_LambdastarMerge", RequiredSelections = [ lambdastarSubMMZSel, lambdastarSubMMPSel, lambdastarSubMMMSel ] )

        lambdastarSubAlg    = SubstitutePID( self.name+"_LambdastarSub_Alg", Code = "ALL", MaxChi2PerDoF = -666 )

        #lambdastarSubAlg.Code = "ALL"
        
        #(DECTREE('rho(770)0 -> p+ K-')) | (DECTREE('rho(770)0 -> K+ p~-')) | (DECTREE('rho(770)0 -> p+ K+')) | (DECTREE('rho(770)0 -> p~- K-'))" 
        
        lambdastarSubAlg.Substitutions = {
            'rho(770)0 -> p+ K-' : 'Lambda(1520)0',
            'rho(770)0 -> p+ K+' : 'Lambda(1520)0',
            'rho(770)0 -> p~- K-' : 'Lambda(1520)~0',
            'rho(770)0 -> K+ p~-' : 'Lambda(1520)~0'
            }
        
        
        lambdastarSubSel =  Selection( self.name+"_LambdastarSub_Sel", Algorithm = lambdastarSubAlg, RequiredSelections = [ lambdastarMerge ] )

        lambdastarFilterAlg = FilterDesktop( Code = "(ABSID=='Lambda(1520)0')" )
          
        lambdastarFilterSel = Selection( self.name + "_LambdastarFilter", Algorithm = lambdastarFilterAlg, RequiredSelections = [ lambdastarSubSel ] )
        
        return lambdastarFilterSel    
    

    def __Dplus__(self, conf):
        """
        Make a Dplus, the D->Kpipi should cover all the Ds we need (no PID requirement)
        """
        _dplus = AutomaticData(Location = 'Phys/StdLooseDplus2hhh/Particles')
        _filter_dplus = FilterDesktop(Code = self.__KpiCuts__(conf) +" & "+ self.__DplusCuts__(conf))
        _seldplus = Selection("Selection_"+self.name+"_dplus",
                             RequiredSelections = [ _dplus ] ,
                             Algorithm = _filter_dplus)
        return _seldplus

    def __Dzero__(self, conf):
        """
        Make a D0, the D0->Kpi should cover all the D we need (no PID requirement)
        """
        _dzero = AutomaticData(Location = 'Phys/StdLooseD02KPi/Particles') 
        _filter_dzero = FilterDesktop(Code = self.KstarCut +" & "+ self.__DplusCuts__(conf))      
        _seldzero = Selection("Selection_"+self.name+"_dzero",
                             RequiredSelections = [ _dzero] ,
                             Algorithm = _filter_dzero)
        return _seldzero

    def __Dstar__(self, conf):
        """
        Get D*+
        """
        _dstar = AutomaticData(Location = 'Phys/StdLooseDstarWithD02KPi/Particles')

        return _dstar

    
    def __Bs__(self, Dimuon, daughters, conf):
        """
        Make and return a Bs selection
        """      

        _b2xmumu = CombineParticles()
        _b2xmumu.DecayDescriptors = conf['DECAYS']


        
        _b2xmumu.CombinationCut = self.BdCombCut
        _b2xmumu.MotherCut = self.BdCut
        
        _sel_Daughters = MergedSelection("Selection_"+self.name+"_daughters",
                                         RequiredSelections = daughters )
        
        sel = Selection( "Selection_"+self.name+"_B2XMuMu",
                         Algorithm = _b2xmumu,
                         RequiredSelections = [ Dimuon, _sel_Daughters ])
        return sel


##   _b2xmumu.DecayDescriptors = [       "B0 -> J/psi(1S) phi(1020)",
##                                       "[B0 -> J/psi(1S) K*(892)0]cc",
##                                       "B0 -> J/psi(1S) rho(770)0",
##                                       "B0 -> J/psi(1S) f_2(1950)",
##                                       "B0 -> J/psi(1S) KS0",
##                                       "[B0 -> J/psi(1S) D~0]cc",
##                                       "[B+ -> J/psi(1S) K+]cc",
##                                       "[B+ -> J/psi(1S) pi+]cc",
##                                       "[B+ -> J/psi(1S) K*(892)+]cc",
##                                       "[B+ -> J/psi(1S) D+]cc",
##                                       "[B+ -> J/psi(1S) D*(2010)+]cc",
##                                       "[Lambda_b0 -> J/psi(1S) Lambda0]cc",
##                                       "[Lambda_b0 -> J/psi(1S) Lambda(1520)0]cc",
##                                       "B0 -> J/psi(1S) pi0",
##                                       "[B+ -> J/psi(1S) a_1(1260)+]cc",
##                                       "[B+ -> J/psi(1S) K_1(1270)+]cc",
##                                       "[B+ -> J/psi(1S) K_2(1770)+]cc"]             
