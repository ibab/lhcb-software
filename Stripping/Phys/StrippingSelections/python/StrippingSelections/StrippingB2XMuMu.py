
__author__ = 'Paul Schaack'
__date__ = '12/02/2011'
__version__ = '$Revision: 1.2 $'

__all__ = ( 'B2XMuMuConf' )

"""
Stripping selection for nearly all electroweak penguin analyses. Includes same-sign combinations.
"""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import  CombineParticles, FilterDesktop

from PhysSelPython.Wrappers import Selection, AutomaticData, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from LHCbKernel.Configuration import *  #check if needed
from Configurables import SubstitutePID


#################
#
#  Define Cuts here
#
#################


defaultConfig = {
      'KpiVXCHI2NDOF'      : 9.0           # dimensionless
    , 'MuonPID'            : -3.0           # dimensionless
    , 'DimuonUPPERMASS'    : 7100.0        # MeV
    , 'Pi0MINPT'           : 800.0         # MeV
    , 'DplusLOWERMASS'     : 1600.0        # MeV
    , 'DplusUPPERMASS'     : 2300.0        # MeV      
    , 'KstarplusWINDOW'    : 300.0         # MeV      
    , 'KsWINDOW'           : 30.0          # MeV     
    , 'LambdaWINDOW'       : 30.0          # MeV    
    , 'LongLivedPT'        : 0.0          # MeV , used to be 500.0 MeV 
    , 'LongLivedTau'        : 2          # ps
    
    # K1 cuts
    , 'K1_Comb_MassLow'  :  720.0
    , 'K1_Comb_MassHigh' : 2450.0
    , 'K1_MassLow'       :  750.0
    , 'K1_MassHigh'      : 2400.0
    , 'K1_MinIPCHI2'     :    4.0
    , 'K1_FlightChi2'    :   25.0
    , 'K1_Dau_MaxIPCHI2' : 9.0
    # From Bd2KstarMuMu line 
    ,'UseNoPIDsHadrons'          : True,
    
    # B cuts
    'B_Comb_MassLow'      : 4800.0,
    'B_Comb_MassHigh'     : 7100.0,
    'B_MassLow'           : 4900.0,
    'B_MassHigh'          : 7000.0,
    'B_VertexCHI2'        :    8.0,
    'B_IPCHI2'            :   16.0,
    'B_DIRA'              :    0.9999,
    'B_FlightCHI2'        :  121.0,
    'B_Dau_MaxIPCHI2'     : 9.0,
    
    # Daughter cuts
    'Dau_VertexCHI2'      :   12.0,
    'Dau_DIRA'            :   -0.9,
    
    # Kstar cuts
    'Kstar_Comb_MassLow'  :  0.0,
    'Kstar_Comb_MassHigh' : 6200.0,
    'Kstar_MassLow'       :  0.0,
    'Kstar_MassHigh'      : 6200.0,
    'Kstar_MinIPCHI2'     :    0.0,
    'Kstar_FlightChi2'    :    9.0,
    'Kstar_Dau_MaxIPCHI2' : 9.0,
    
    # JPsi (dimu) cuts
    'Dimu_FlightChi2'     :   9.0,
    'Dimu_Dau_MaxIPCHI2'  :   9.0,
    
    # Track cuts
    'Track_CHI2nDOF'      :    5.0,
 
    # Hadron cuts
    'Hadron_MinIPCHI2'    :    9.0,

    # Muon cuts
    'Muon_MinIPCHI2'      :    9.0,
    'Muon_IsMuon'         :    True,
    'MuonNoPIDs_PIDmu'    :    0.0,

    # Wrong sign combinations
    'DimuonWS'            :   True,
    'HadronWS'            :   True,

    # GEC
    'SpdMult'             :  600
    }


#################
#
#  Make line here
#
#################

defaultName = "B2XMuMu"


class B2XMuMuConf(LineBuilder) :

    __configuration_keys__ = (
          'KpiVXCHI2NDOF'
        , 'MuonPID'
        , 'DimuonUPPERMASS'
        , 'Pi0MINPT'
        , 'DplusLOWERMASS'
        , 'DplusUPPERMASS'
        , 'KstarplusWINDOW'
        , 'KsWINDOW'
        , 'LambdaWINDOW'
        , 'LongLivedPT'
        , 'LongLivedTau'

        #, 'UseNoPIDsHadrons',
        
        # K1 cuts
        , 'K1_Comb_MassLow'
        , 'K1_Comb_MassHigh'
        , 'K1_MassLow'
        , 'K1_MassHigh'
        , 'K1_MinIPCHI2'
        , 'K1_FlightChi2'
        , 'K1_Dau_MaxIPCHI2',
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

        # JPsi (dimu) cuts
        'Dimu_FlightChi2',
        'Dimu_Dau_MaxIPCHI2',
        
        # Track cuts
        'Track_CHI2nDOF',
        
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
        'SpdMult'
       )
    
    def __init__(self, name, config) :


        LineBuilder.__init__(self, name, config)

        self.name = name
        
        # Bd2KstartMuMu cuts definitions
        self.BdCombCut = "(AM > %(B_Comb_MassLow)s * MeV) & (AM < %(B_Comb_MassHigh)s * MeV)" %config

        self.BdCut = "(M > %(B_MassLow)s * MeV) & " \
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

        self.TrackCuts = "(TRCHI2DOF < %(Track_CHI2nDOF)s)" %config

        self.HadronCuts = "(MIPCHI2DV(PRIMARY) > %(Hadron_MinIPCHI2)s) & (HASRICH) & (~ISMUON)" %config
        
        self.KaonCut = self.TrackCuts + " & " + self.HadronCuts
        self.PionCut = self.TrackCuts + " & " + self.HadronCuts
        
        self.MuonCut = self.TrackCuts + " & (MIPCHI2DV(PRIMARY) > %(Muon_MinIPCHI2)s) & (PIDmu> %(MuonPID)s)" %config

        self.KstarFilterCut  = self.KstarCut + " & (INTREE(ABSID=='K+') & " + self.KaonCut + ") & (INTREE(ABSID=='pi+') & " + self.PionCut + ")"

        self.Dimuon = self.__Dimuon__(config)        
        self.Protons = self.__Protons__(config)
        self.Kaons = self.__Kaons__(config)
        self.Pions = self.__Pions__(config)       
        self.Kshort = self.__Kshort__(config)
        self.Dplus = self.__Dplus__(config)
        self.Dzero = self.__Dzero__(config)
        self.Lambda = self.__Lambda__(config)
        self.Pi0 = self.__Pi0__(config)
        self.Rho = self.__Rho__(self.Pions, config)
        self.Phi = self.__Phi__(self.Rho, config)
        self.Kstar = self.__Kstar__(self.Rho, config)
        self.K1 = self.__K1__(self.Kaons, self.Pions, config)
        self.Lambdastar = self.__Lambdastar__(self.Rho, config)
        self.Kstar2KsPi = self.__Kstar2KsPi__(self.Kshort, self.Pions, config)
        self.Kstar2KPi0 = self.__Kstar2KPi0__(self.Kaons, self.Pi0, config)
        self.Dstar = self.__Dstar__(config)

        self.Bs = self.__Bs__(self.Dimuon, self.Protons, self.Kaons, self.Pions, self.Pi0,
                              self.Kshort, self.Lambda, self.Phi, self.Rho, self.Dplus,
                              self.Kstar, self.K1, self.Lambdastar, self.Kstar2KsPi,
                              self.Kstar2KPi0, config)

#        self.line = None

#        if ( config['SpdMult'] > 0 ):
        self.line = StrippingLine(self.name+"_Line",
                                  prescale = 1,
                                   FILTER = {
                                   'Code' : " ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < %(SpdMult)s )" %config ,
                                   'Preambulo' : [ "from LoKiNumbers.decorators import *", "from LoKiCore.basic import LHCb" ]
                                   },
                                  algos=[self.Bs]
                                  )
#        else:
#        self.line = StrippingLine(self.name+"_Line",
#                                  prescale = 1,
#                                  algos=[self.Bs]
#                                  )
        
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
 
    def __K1CombCut__(self, conf):
        """
        Returns the K1 cut string
        """
        _K1CombCut = "(AM > %(K1_Comb_MassLow)s * MeV) &"\
        "(AM < %(K1_Comb_MassHigh)s * MeV) & "\
        "(ADOCACHI2CUT(20.,'')) &" \
        "(AHASCHILD(MIPCHI2DV(PRIMARY) > %(K1_Dau_MaxIPCHI2)s ))" %conf
        return _K1CombCut
        
    def __K1Cut__(self, conf):
        """
        Returns the K1 cut string
        """
        _K1Cut = "(M > %(K1_MassLow)s * MeV) &"\
        "(M < %(K1_MassHigh)s * MeV) & "\
        "(BPVVDCHI2 > %(K1_FlightChi2)s) & "\
        "(MIPCHI2DV(PRIMARY) > %(K1_MinIPCHI2)s) " %conf 
        return _K1Cut
        

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
        Filter Lambda from StdLooseLambda and StdLooseLambda
        """  
        _lambdadd = AutomaticData(Location = 'Phys/StdLooseLambdaDD/Particles')
        _lambdall = AutomaticData(Location = 'Phys/StdLooseLambdaLL/Particles')
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
        _filter_pi0resolved = FilterDesktop(Code = self.__Pi0Cuts__(conf))
        _filter_pi0merged = FilterDesktop(Code = self.__Pi0Cuts__(conf))        
        _selpi0resolved = Selection("Selection_"+self.name+"_pi0resolved",
                             RequiredSelections = [ _pi0resolved ] ,
                             Algorithm = _filter_pi0resolved)
        _selpi0merged = Selection("Selection_"+self.name+"_pi0merged",
                             RequiredSelections = [ _pi0merged ] ,
                             Algorithm = _filter_pi0merged)
        _sel = MergedSelection("Selection_"+self.name+"_pi0",
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
        _kstarConf.ParticleCombiners.update ( { '' : 'MomentumCombiner' } )
                                                 
        _selKSTAR2KPIZERO = Selection( "Selection_"+self.name+"_Kstar2kpizero",
                                       Algorithm = _kstarConf,
                                       RequiredSelections = [ Kaons, Pi0 ] )
        return _selKSTAR2KPIZERO
        


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
        Make a phi
        """      
        phiChangeAlg1 = SubstitutePID( self.name+"phiChangeAlg",
                                     Code = "(DECTREE('X0 -> X+ X-')) | (DECTREE('X0 -> X+ X+')) | (DECTREE('X0 -> X- X-')) ",
                                     Substitutions = { ' X0 -> ^X+ X- ' : 'K+' ,
                                                       ' X0 -> X+ ^X- ' : 'K-' ,
                                                       ' X0 -> X+ X- ' : 'phi(1020)',
                                                       ' X0 -> ^X+ X+ ' : 'K+',
                                                       ' X0 -> X+ ^X+ ' : 'K+',
                                                       ' X0 -> X+ X+ ' : 'phi(1020)',
                                                       ' X0 -> ^X- X- ' : 'K-',
                                                       ' X0 -> X- ^X- ' : 'K-',
                                                       ' X0 -> X- X- ' : 'phi(1020)' }, 
                                     MaxParticles = 20000,
                                     MaxChi2PerDoF = -666 )

        phiDescr1 =  Selection( self.name+"_Phi_SubPIDAlg",
                                Algorithm = phiChangeAlg1,
                                RequiredSelections = [Rho])
    
        pick = Selection(self.name+"_Phi_PickDecay",
                     Algorithm = FilterDesktop( Code = "(DECTREE('phi(1020) -> K+ K-')) | (DECTREE('phi(1020) -> K+ K+')) | (DECTREE('phi(1020) -> K- K-'))" ),
                     RequiredSelections = [phiDescr1])
    
        return pick

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
        kstarChangeAlg1 = SubstitutePID( self.name+"kstarChangeAlg1",
                                     Code = "(DECTREE('X0 -> X+ X-')) | (DECTREE('X0 -> X+ X+')) | (DECTREE('X0 -> X- X-')) ",
                                     Substitutions = { ' X0 -> ^X+ X- ' : 'K+' ,
                                                       ' X0 -> X+ X- ' : 'K*(892)0',
                                                       ' X0 -> ^X+ X+ ' : 'K+',
                                                       ' X0 -> X+ X+ ' : 'K*(892)0',
                                                       ' X0 -> ^X- X- ' : 'K-',
                                                       ' X0 -> X- X- ' : 'K*(892)~0' }, 
                                     MaxParticles = 20000,
                                     MaxChi2PerDoF = -666 )

        kstarChangeAlg2 = SubstitutePID( self.name+"kstarChangeAlg2",
                                     Code = "(DECTREE('X0 -> X+ X-')) | (DECTREE('X0 -> X+ X+')) | (DECTREE('X0 -> X- X-')) ",
                                     Substitutions = { ' X0 -> X+ ^X- ' : 'K-' ,
                                                       ' X0 -> X+ X- ' : 'K*(892)~0',
                                                       ' X0 -> X+ ^X+ ' : 'K+',
                                                       ' X0 -> X+ X+ ' : 'K*(892)0',
                                                       ' X0 -> X- ^X- ' : 'K-',
                                                       ' X0 -> X- X- ' : 'K*(892)~0' }, 
                                     MaxParticles = 20000,
                                     MaxChi2PerDoF = -666 )

        ksDescr1 =  Selection( self.name+"_Kstar_SubPIDAlg1",
                                Algorithm = kstarChangeAlg1,
                                RequiredSelections = [Rho])
    
        ksDescr2 =  Selection( self.name+"_Kstar_SubPIDAlg2",
                                Algorithm = kstarChangeAlg2,
                                RequiredSelections = [Rho])
    
        pick = Selection(self.name+"_Kstar_PickDecay",
                     Algorithm = FilterDesktop( Code = "(DECTREE('K*(892)0 -> K+ pi-')) | (DECTREE('K*(892)~0 -> K- pi+')) | (DECTREE('K*(892)0 -> K+ pi+')) | (DECTREE('K*(892)~0 -> K- pi-'))" ),
                     RequiredSelections = [ksDescr1, ksDescr2])
    
        return pick


    def __K1__(self, Kaons, Pions, conf):
        """
        Make a k1
        """      
        _k12kpipi = CombineParticles()
        _k12kpipi.DecayDescriptors = [ "[K_1(1270)+ -> K+ pi- pi+]cc" ]
        _k12kpipi.CombinationCut = self.__K1CombCut__(conf)
        _k12kpipi.MotherCut = self.__K1Cut__(conf)

        _selK12KPIPI = Selection( "Selection_"+self.name+"_K1",
                                     Algorithm = _k12kpipi,
                                     RequiredSelections = [ Kaons, Pions ] )
        return _selK12KPIPI

    
    def __Lambdastar__(self, Rho, conf):
        """
        Make a Lambdastar
        """      
        lstarChangeAlg1 = SubstitutePID( self.name+"lstarChangeAlg1",
                                     Code = "(DECTREE('X0 -> X+ X-')) | (DECTREE('X0 -> X+ X+')) | (DECTREE('X0 -> X- X-')) ",
                                     Substitutions = { ' X0 -> ^X+ X- ' : 'p+' ,
                                                       ' X0 -> X+ ^X- ' : 'K-' ,
                                                       ' X0 -> X+ X- ' : 'Lambda(1520)0',
                                                       ' X0 -> ^X+ X+ ' : 'p+',
                                                       ' X0 -> X+ ^X+ ' : 'K+',
                                                       ' X0 -> X+ X+ ' : 'Lambda(1520)0',
                                                       ' X0 -> ^X- X- ' : 'p~-',
                                                       ' X0 -> X- ^X- ' : 'K-',
                                                       ' X0 -> X- X- ' : 'Lambda(1520)~0' }, 
                                     MaxParticles = 20000,
                                     MaxChi2PerDoF = -666 )

        lstarChangeAlg2 = SubstitutePID( self.name+"lstarChangeAlg2",
                                     Code = "(DECTREE('X0 -> X+ X-')) | (DECTREE('X0 -> X+ X+')) | (DECTREE('X0 -> X- X-')) ",
                                     Substitutions = { ' X0 -> X+ ^X- ' : 'p~-' ,
                                                       ' X0 -> ^X+ X- ' : 'K+' ,
                                                       ' X0 -> X+ X- ' : 'Lambda(1520)~0',
                                                       ' X0 -> ^X+ X+ ' : 'K+',
                                                       ' X0 -> X+ ^X+ ' : 'p+',
                                                       ' X0 -> X+ X+ ' : 'Lambda(1520)0',
                                                       ' X0 -> ^X- X- ' : 'K-',
                                                       ' X0 -> X- ^X- ' : 'p~-',
                                                       ' X0 -> X- X- ' : 'Lambda(1520)~0' }, 
                                     MaxParticles = 20000,
                                     MaxChi2PerDoF = -666 )

        lsDescr1 =  Selection( self.name+"_Lambdastar_SubPIDAlg1",
                                Algorithm = lstarChangeAlg1,
                                RequiredSelections = [Rho])
    
        lsDescr2 =  Selection( self.name+"_Lambdastar_SubPIDAlg2",
                                Algorithm = lstarChangeAlg2,
                                RequiredSelections = [Rho])
    
        pick = Selection(self.name+"_Lambdastar_PickDecay",
                     Algorithm = FilterDesktop( Code = "(DECTREE('Lambda(1520)0 -> p+ K-')) | (DECTREE('Lambda(1520)~0 -> p~- K+')) | (DECTREE('Lambda(1520)0 -> p+ K+')) | (DECTREE('Lambda(1520)~0 -> p~- K-'))" ),
                     RequiredSelections = [lsDescr1, lsDescr2])
    
        return pick

    def __Dplus__(self, conf):
        """
        Make a Dplus, the D->Kpipi should cover all the Ds we need (no PID requirement)
        """
        _dplus = AutomaticData(Location = 'Phys/StdLooseDplus2KPiPi/Particles') 
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



    
    def __Bs__(self, Dimuon, Protons, Kaons, Pions, Pi0, Kshort, Lambda, Phi, Rho, Dplus, Kstar, K1, Lambdastar, Kstar2KsPi, Kstar2KPi0, conf):
        """
        Make and return a Bs selection
        """      

        _b2xmumu = CombineParticles()
        _b2xmumu.DecayDescriptors = [ "B0 -> J/psi(1S) phi(1020)",
                                      "[B0 -> J/psi(1S) K*(892)0]cc",
                                      "B0 -> J/psi(1S) rho(770)0",
                                      "B0 -> J/psi(1S) KS0",
                                      "[B0 -> J/psi(1S) D~0]cc",
                                      "[B+ -> J/psi(1S) K+]cc",
                                      "[B+ -> J/psi(1S) pi+]cc",
                                      "[B+ -> J/psi(1S) K*(892)+]cc",
                                      "[B+ -> J/psi(1S) D+]cc",
                                      "[B+ -> J/psi(1S) D*(2010)+]cc",
                                      "[Lambda_b0 -> J/psi(1S) Lambda0]cc",
                                      "[Lambda_b0 -> J/psi(1S) Lambda(1520)0]cc",
                                      "B0 -> J/psi(1S) pi0"]
        # "[B+ -> J/psi(1S) K_1(1270)+]cc",
        # removed K1 for timing reasons
        
        _b2xmumu.CombinationCut = self.BdCombCut 
        _b2xmumu.MotherCut = self.BdCut
        
        _sel_Daughters = MergedSelection("Selection_"+self.name+"_daughters",
                                         RequiredSelections = [ Kaons, Pions, Kshort, Lambda, 
                                                               Rho, Phi, Lambdastar, Kstar,
                                                               self.Dzero, Dplus, self.Dstar,
                                                               Kstar2KsPi, Kstar2KPi0, Pi0])
        sel = Selection( "Selection_"+self.name+"_bs2xmumu",
                         Algorithm = _b2xmumu,
                         RequiredSelections = [ Dimuon, _sel_Daughters ])
        return sel

