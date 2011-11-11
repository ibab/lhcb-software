
__author__ = 'Paul Schaack'
__date__ = '12/02/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ( 'B2XMuMuConf' )

"""
Stripping selection for B_{s,d} channels
"""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import  CombineParticles, FilterDesktop

from PhysSelPython.Wrappers import Selection, AutomaticData, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from LHCbKernel.Configuration import *  #check if needed


#################
#
#  Define Cuts here
#
#################


defaultConfig = {
      'BVXCHI2NDOF'        : 8         # dimensionless
    , 'BIPCHI2'            : 9.0           # dimensionless
    , 'BDIRA'              : 0.999968      # dimensionless
    , 'BFDCHI2'            : 100.0         # dimensionless
    , 'KpiMINIPCHI2'       : 9.0           # dimensionless
    , 'KpiTRACKCHI2'       : 4.0           # dimensionless    
    , 'KpiVXCHI2NDOF'      : 9.0           # dimensionless
    , 'MuonMINIPCHI2'      : 16.0           # dimensionless
    , 'MuonTRACKCHI2'      : 4.0           # dimensionless
    , 'MuonPID'            : 0.0           # dimensionless
    , 'DimuonVXCHI2NDOF'   : 9.0           # dimensionless
    , 'DimuonUPPERMASS'    : 5050.0        # MeV
    , 'Pi0MINPT'           : 800.0         # MeV
    , 'DplusLOWERMASS'     : 1600.0        # MeV
    , 'DplusUPPERMASS'     : 2300.0        # MeV      
    , 'KstarplusWINDOW'    : 300.0         # MeV      
    , 'KsWINDOW'           : 30.0          # MeV     
    , 'LambdaWINDOW'       : 30.0          # MeV    
    , 'LongLivedPT'        : 500.0          # MeV  
    , 'LongLivedTau'        : 2          # ps 
}


#################
#
#  Make line here
#
#################

defaultName = "B2XMuMu"


class B2XMuMuConf(LineBuilder) :

    __configuration_keys__ = (
        'BVXCHI2NDOF'
        , 'BIPCHI2'
        , 'BDIRA'
        , 'BFDCHI2'
        , 'KpiMINIPCHI2'
        , 'KpiTRACKCHI2'
        , 'KpiVXCHI2NDOF'
        , 'MuonMINIPCHI2'
        , 'MuonTRACKCHI2'
        , 'MuonPID'
        , 'DimuonVXCHI2NDOF'
        , 'DimuonUPPERMASS'
        , 'Pi0MINPT'
        , 'DplusLOWERMASS'
        , 'DplusUPPERMASS'
        , 'KstarplusWINDOW'
        , 'KsWINDOW'
        , 'LambdaWINDOW'
        , 'LongLivedPT'
        , 'LongLivedTau'
    )

    def __init__(self, name, config) :


        LineBuilder.__init__(self, name, config)

        self.name = name
        self.Muons = self.__Muons__(config)
        self.Dimuon = self.__Dimuon__(self.Muons, config)        
        self.PIDProtons = self.__Protons__(config)
        self.PIDKaons = self.__Kaons__(config)
        self.PIDPions = self.__Pions__(config)
        self.Protons = self.__NoPIDProtons__(config)
        self.Kaons = self.__NoPIDKaons__(config)
        self.Pions = self.__NoPIDPions__(config)       
        self.Kshort = self.__Kshort__(config)
        self.Dplus = self.__Dplus__(config)
        self.Lambda = self.__Lambda__(config)
        self.Pi0 = self.__Pi0__(config)
        self.Phi = self.__Phi__(self.Kaons, config)
        self.Rho = self.__Rho__(self.Pions, config)
        self.Kstar = self.__Kstar__(self.Kaons, self.Pions, config)
        self.Lambdastar = self.__Lambdastar__(self.Protons, self.Kaons, config)
        self.Kstar2KsPi = self.__Kstar2KsPi__(self.Kshort, self.Pions, config)
        self.Kstar2KPi0 = self.__Kstar2KPi0__(self.Kaons, self.Pi0, config)

        self.Bs = self.__Bs__(self.Dimuon, self.Protons, self.Kaons, self.Pions,
                              self.Kshort, self.Lambda, self.Phi, self.Rho, self.Dplus,
                              self.Kstar, self.Lambdastar, self.Kstar2KsPi,
                              self.Kstar2KPi0, config)

        self.line = StrippingLine(self.name+"_Line",
                                  prescale = 1,
                                  algos = [ self.Bs ]
                                  )
        self.registerLine(self.line)



        
    def __DimuonCuts__(self, conf):
        """
        Returns the Dimuon cut string
        """
        _DimuonCuts = """
        (VFASPF(VCHI2/VDOF)< %(DimuonVXCHI2NDOF)s )
        """ % conf
        return _DimuonCuts


    def __MuonCuts__(self, conf):
        """
        Returns the Muon cut string
        """
        _MuonCuts = """
        (PIDmu> %(MuonPID)s ) &
        (TRCHI2DOF < %(MuonTRACKCHI2)s ) &
        (MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s )
        """ % conf
        return _MuonCuts


    def __Muons__(self, conf):
        """
        Filter muons from StdAllLooseMuons
        """  
        _muons = AutomaticData(Location = 'Phys/StdAllLooseMuons/Particles')
        _filter = FilterDesktop( Code = self.__MuonCuts__(conf) ) 
        _sel = Selection("Selection_"+self.name+"_Muons",
                         RequiredSelections = [ _muons ] ,
                         Algorithm = _filter)
        return _sel


    def __Dimuon__(self, Muons, conf):
        """
        Make and return a Dimuon
        """      
        _dimuon2mumu = CombineParticles()
        _dimuon2mumu.DecayDescriptors = ["J/psi(1S) -> mu+ mu-", " J/psi(1S) -> mu+ mu+", " J/psi(1S) -> mu- mu-"]
        _dimuon2mumu.CombinationCut = "(AM < %(DimuonUPPERMASS)s *MeV)" % conf
        _dimuon2mumu.MotherCut = self.__DimuonCuts__(conf)
        
        _selDIMUON2MUMU = Selection( "Selection_"+self.name+"_Dimuon",
                                    Algorithm = _dimuon2mumu,
                                    RequiredSelections = [ Muons ] )
        return _selDIMUON2MUMU
    


    def __TrackCuts__(self, conf):
        """
        Returns the KaonPion cut string
        """
        _TrackCuts = """
        (TRCHI2DOF < %(KpiTRACKCHI2)s ) &
        (MIPCHI2DV(PRIMARY) > %(KpiMINIPCHI2)s )
        """ % conf
        return _TrackCuts


    def __Kaons__(self, conf):
        """
        Filter kaons from StdAllLooseKaons
        """  
        _kaons = AutomaticData(Location = 'Phys/StdAllLooseKaons/Particles')
        _filter = FilterDesktop(Code = self.__TrackCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_StdAllLooseKaons",
                         RequiredSelections = [ _kaons ] ,
                         Algorithm = _filter)
        return _sel

    def __NoPIDKaons__(self, conf):
        """
        Filter kaons from StdAllNoPIDsKaons
        """  
        _kaons = AutomaticData(Location = 'Phys/StdAllNoPIDsKaons/Particles')
        _filter = FilterDesktop(Code = self.__TrackCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_StdAllNoPIDsKaons",
                         RequiredSelections = [ _kaons ] ,
                         Algorithm = _filter)
        return _sel

    def __Protons__(self, conf):
        """
        Filter protons from StdAllLooseProtons
        """  
        _protons = AutomaticData(Location = 'Phys/StdAllLooseProtons/Particles')
        _filter = FilterDesktop(Code = self.__TrackCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_StdAllLooseProtons",
                         RequiredSelections = [ _protons ] ,
                         Algorithm = _filter)
        return _sel

    def __NoPIDProtons__(self, conf):
        """
        Filter protons from StdAllLooseProtons
        """  
        _protons = AutomaticData(Location = 'Phys/StdAllNoPIDsProtons/Particles')
        _filter = FilterDesktop(Code = self.__TrackCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_StdAllNoPIDsProtons",
                         RequiredSelections = [ _protons ] ,
                         Algorithm = _filter)
        return _sel



    def __Pions__(self, conf):
        """
        Filter pions from StdAllLoosePions
        """  
        _pions = AutomaticData(Location = 'Phys/StdAllLoosePions/Particles')
        _filter = FilterDesktop(Code = self.__TrackCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_StdAllLoosePions",
                         RequiredSelections = [ _pions ] ,
                         Algorithm = _filter)
        return _sel

    def __NoPIDPions__(self, conf):
        """
        Filter pions from StdAllNoPIDsPions
        """  
        _pions = AutomaticData(Location = 'Phys/StdAllNoPIDsPions/Particles')
        _filter = FilterDesktop(Code = self.__TrackCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_StdAllNoPIDsPions",
                         RequiredSelections = [ _pions ] ,
                         Algorithm = _filter)
        return _sel


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
        Filter Lambda from StdLooseLambda
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
        _filter_pi0resolved = FilterDesktop(Code = self.__Pi0Cuts__(conf))
        

        _sel = Selection("Selection_"+self.name+"_pi0resolved",
                              RequiredSelections = [ _pi0resolved ] ,
                              Algorithm = _filter_pi0resolved)
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

    def __pKCuts__(self, conf):
        """
        Returns the pK cut string
        """
        _pKCuts = """
        (VFASPF(VCHI2PDOF)< %(KpiVXCHI2NDOF)s )
        """ % conf
        return _pKCuts





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
        _kstar2kspi.MotherCut = self.__KpiCuts__(conf)

        _selKSTAR2KSPI = Selection( "Selection_"+self.name+"_Kstar2kspi",
                                     Algorithm = _kstar2kspi,
                                     RequiredSelections = [ Kshort, Pions ] )
        return _selKSTAR2KSPI



    def __Phi__(self, Kaons, conf):
        """
        Make a phi
        """      
        _phi2kk = CombineParticles()
        _phi2kk.DecayDescriptors = [ "phi(1020) -> K+ K-", "phi(1020) -> K+ K+", "phi(1020) -> K- K-" ]
        _phi2kk.MotherCut = self.__KpiCuts__(conf)

        _selPHI2KK = Selection( "Selection_"+self.name+"_Phi",
                                     Algorithm = _phi2kk,
                                     RequiredSelections = [ Kaons ] )
        return _selPHI2KK
    
    def __Rho__(self, Pions, conf):
        """
        Make a rho
        """      
        _rho2pipi = CombineParticles()
        _rho2pipi.DecayDescriptors = [ "rho(770)0 -> pi+ pi-", "rho(770)0 -> pi+ pi+" , "rho(770)0 -> pi- pi-" ]
        _rho2pipi.MotherCut = self.__KpiCuts__(conf)

        _selRHO2PIPI = Selection( "Selection_"+self.name+"_Rho",
                                     Algorithm = _rho2pipi,
                                     RequiredSelections = [ Pions ] )
        return _selRHO2PIPI

    def __Kstar__(self, Kaons, Pions, conf):
        """
        Make a kstar
        """      
        _kstar2kpi = CombineParticles()
        _kstar2kpi.DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc", "[K*(892)0 -> K+ pi+]cc" ]
        _kstar2kpi.MotherCut = self.__KpiCuts__(conf)

        _selKSTAR2KPI = Selection( "Selection_"+self.name+"_Kstar",
                                     Algorithm = _kstar2kpi,
                                     RequiredSelections = [ Kaons, Pions ] )
        return _selKSTAR2KPI

    def __Lambdastar__(self, Protons, Kaons, conf):
        """
        Make a Lambda* 
        """      
        _lambdastar2pk = CombineParticles()
        _lambdastar2pk.DecayDescriptors = [ "[Lambda(1520)0 -> p+ K-]cc", "[Lambda(1520)0 -> p+ K+]cc" ]
        _lambdastar2pk.MotherCut = self.__pKCuts__(conf)

        _selLAMBDASTAR2PK = Selection( "Selection_"+self.name+"_Lambdastar",
                                     Algorithm = _lambdastar2pk,
                                     RequiredSelections = [ Protons, Kaons ] )
        return _selLAMBDASTAR2PK

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



    def __BsCuts__(self, conf):
        """
        Returns the Bs cut string
        """
        _BsCuts = """
        (VFASPF(VCHI2/VDOF)< %(BVXCHI2NDOF)s ) &
        (BPVVDCHI2 > %(BFDCHI2)s ) &
        (BPVDIRA > %(BDIRA)s ) &
        (BPVIPCHI2()< %(BIPCHI2)s )
        """ % conf
        return _BsCuts






    def __Bs__(self, Dimuon, Protons, Kaons, Pions, Kshort, Lambda, Phi, Rho, Dplus, Kstar, Lambdastar, Kstar2KsPi, Kstar2KPi0, conf):
        """
        Make and return a Bs selection
        """      

        _b2xmumu = CombineParticles()
        _b2xmumu.DecayDescriptors = [ "B0 -> J/psi(1S) phi(1020)",
                                      "[B0 -> J/psi(1S) K*(892)0]cc",
                                      "B0 -> J/psi(1S) rho(770)0",
                                      "B0 -> J/psi(1S) KS0",
                                      "[B+ -> J/psi(1S) K+]cc",
                                      "[B+ -> J/psi(1S) pi+]cc",
                                      "[B+ -> J/psi(1S) K*(892)+]cc",
                                      "[B+ -> J/psi(1S) D+]cc",
                                      "[Lambda_b0 -> J/psi(1S) Lambda0]cc",
                                      "[Lambda_b0 -> J/psi(1S) Lambda(1520)0]cc"]
        
        _b2xmumu.CombinationCut = "(AM > 4900.0 *MeV) & (AM < 7000.0 *MeV)"
        _b2xmumu.MotherCut = self.__BsCuts__(conf)
        
        _sel_Daughters = MergedSelection("Selection_"+self.name+"_daughters",
                                         RequiredSelections = [Protons, Kaons, Pions, Kshort, Lambda,
                                                               Phi, Rho, Dplus, Kstar, Lambdastar,
                                                               Kstar2KsPi, Kstar2KPi0])
        sel = Selection( "Selection_"+self.name+"_bs2xmumu",
                         Algorithm = _b2xmumu,
                         RequiredSelections = [ Dimuon, _sel_Daughters ])
        return sel






















