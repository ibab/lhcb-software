"""
Stripping selection for B->X tau mu:
    - B -> K tau(mu) mu
    - B -> K*0 tau(mu) mu
    - B -> phi tau(mu) mu
    - B -> phi tau(3pi) mu
"""

__author__ = "Paula Alvarez"
__date__ = "27/04/2015"
__version__ = "1.0"

__all__ = ('B2XTauMuConf',
           'default_conf')


from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdAllLooseMuons, StdLooseKaons, StdTightDetachedTau3pi


default_config = {
    'NAME'          : 'B2XTauMu',
    'BUILDERTYPE'   : 'B2XTauMuConf',
    'STREAMS'       : ['Dimuon'],
    'WGs'           : ['RD'],
    'CONFIG'        : { 
        # Common cuts
        "MINIPCHI2"      : 16.0    # adimensiional
        ,"TrackCHI2"     : 3       # adimensional
        ,"TrackGhostProb": 0.5     # adimensional
        ,"KaonPIDK"      : 4.0     # adimensiional
        ,"PionPIDK"      : 0.0     # adimensiional
        ,"PIDmu"         : 2.0     # adimensiional
        ,"B_MIN_MASS"    : 2000    # MeV
        ,"B_MAX_MASS"    : 10000    # MeV

        ####  Bd->K*0 tau(mu) mu
        # Daughters
        ,"MuonPT_kst"        : 300.0   # MeV
        ,"MuonP_kst"         : 2.0     # GeV
        ,"KPiPT_kst"         : 300.0   # MeV
        ,"KaonP_kst"         : 2.0     # GeV
        ,"PionP_kst"         : 2.0     # GeV


        # K*0 cuts
        ,"KstMassWin"    : 150.0   # MeV
        ,"KstAMassWin"   : 180.0   # MeV
        ,"KstVCHI2DOF"   : 15.0    # adimensiional
                        
        # Xmu cuts
        ,"XMuVCHI2DOF_kst" : 15.0    # adimensiional
        
        # B cuts
        ,"BDIRA_kst"         : 0.95    # adimensiional
        ,"BVCHI2DOF_kst"     : 25.0    # adimensiional
        ,"B_FDCHI2_kst"      : 80      # adimensiional
        
        ####  Bd->phi tau(mu) mu
        # Phi cuts
        ,"PhiAMassWin"    : 30.0   # MeV
        ,"PhiMassWin"    : 25.0   # MeV
        ,"PhiVCHI2DOF"   : 20.0    # adimensiional

        # Xmu cuts
        ,"XMuVCHI2DOF_phi" : 20.0    # adimensiional
        
        # B cuts for phitaumu
        ,"BVCHI2DOF_phi"     : 1e10    # adimensiional

        
        
        ####  B+->K+ tau(mu) mu
        # Daughters
        ,"MuonPT_K"        : 1000.0   # MeV
        ,"MuonP_K"         : 5.0     # GeV
        ,"KPiPT_K"         : 500.0   # MeV
        ,"KaonP_K"         : 3.0     # GeV
        ,"MINIPCHI2_K"     : 36.0
        ,"KaonPIDK_K"      : 5    

        # Xmu cuts
        ,"XMuVCHI2DOF_K" : 9.0    # adimensiional
        ,"KMuSumPT_K"      : 2000.0  # MeV
        ,"KMuPT_K"      : 1000.0  # MeV

        # B cuts for Ktaumu
        ,"BDIRA_K"         : 0.99    # adimensiional
        ,"BVCHI2DOF_K"     : 15.0    # adimensiional
        ,"B_FDCHI2_K"      : 400.      # adimensiional
        

        
        # Wrong sign combinations
        ,'MuTauWS'       : True


        ,"Prescale"      : 1
        ,"Prescale_WS"    : 1

    }    
}




class B2XTauMuConf(LineBuilder) :
    """
    Class defining B->X tau mu stripping line
    """


    __configuration_keys__ = (
        "MINIPCHI2"
        ,"TrackCHI2"
        ,"TrackGhostProb"    
        ,"KaonPIDK"     
        ,"PionPIDK"      
        ,"PIDmu"         
        ,"B_MIN_MASS"
        ,"B_MAX_MASS"

        ,"MuonPT_kst"
        ,"MuonP_kst"
        ,"KPiPT_kst"
        ,"KaonP_kst"
        ,"PionP_kst"
        ,"KstMassWin"     
        ,"KstAMassWin"
        ,"KstVCHI2DOF"
        ,"XMuVCHI2DOF_kst"
        ,"BDIRA_kst"         
        ,"BVCHI2DOF_kst"
        ,"B_FDCHI2_kst"

        ,"PhiMassWin"
        ,"PhiAMassWin"
        ,"PhiVCHI2DOF"
        ,"XMuVCHI2DOF_phi"
        ,"BVCHI2DOF_phi"


        ,"MuonPT_K"
        ,"MuonP_K"
        ,"KPiPT_K"
        ,"KaonP_K"
        ,"KMuSumPT_K"
        ,"KMuPT_K"
        ,"MINIPCHI2_K"
        ,"KaonPIDK_K"
        ,"XMuVCHI2DOF_K"
        ,"BDIRA_K"         
        ,"BVCHI2DOF_K"
        ,"B_FDCHI2_K"

        ,"MuTauWS"

        ,"Prescale"
        ,"Prescale_WS"
        )
    
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config


        name_kst = name+"_Kst"
        name_phi = name+"_Phi"
        name_k = name+"_K"
        name_phi_3pi = name+"_Phi_3pi"

        name_kst_WS = name+"_Kst_WS"
        name_phi_WS = name+"_Phi_WS"
        name_k_WS = name+"_K_WS"
        name_phi_3pi_WS = name+"_Phi_3pi_WS"

        ############### MUON SELECTIONS ###################
        self.selmuon = Selection( "Mufor" + name,
                                  Algorithm = self._muonFilter(),
                                  RequiredSelections = [StdAllLooseMuons])
                                  

        ############### KAON AND PION SELECTIONS ################
        
        self.selKaon = Selection( "Kfor" + name,
                                  Algorithm = self._kaonFilter(),
                                  RequiredSelections = [StdLooseKaons])
        

        self.selPion = Selection( "Pifor" + name,
                                  Algorithm = self._pionFilter(),
                                  RequiredSelections = [StdLoosePions])
                                  
                                                             

        ################ phi -> KK SELECTION ##########################
        
        self.selphi2kk = Selection( "Phi2KKfor" + name,
                                    Algorithm = self._Phi2KKFilter(),
                                    RequiredSelections = [self.selKaon] )


        ################ Kst0 -> Kpi SELECTION ##########################
        
        self.selkst02kpi = Selection( "Kst02KPifor" + name,
                                    Algorithm = self._Kst02KPiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion] )


        ################ Kst(1430)0 -> K+ Mu- (OS) SELECTION ##########################
        
        self.selkmuOS = Selection(name = "KMuOSFor"+name,
                                  Algorithm          = self._KMuOSFilter(),
                                  RequiredSelections = [self.selKaon,self.selmuon])


        ################ Delta++ -> K+ Mu+  (SS) SELECTION ##########################
        
        self.selkmuSS = Selection(name = "KMuSSFor"+name,
                                  Algorithm          = self._KMuSSFilter(),
                                  RequiredSelections = [self.selKaon,self.selmuon])


        ################ Kst(1430)+ -> phi Mu  SELECTION ##########################
        
        self.selkstarmu = Selection(name = "Kst0MuFor"+name,
                                    Algorithm          = self._Kst0MuFilter(),
                                    RequiredSelections = [self.selkst02kpi,self.selmuon])


        ################ K_2(1770)+ -> Kst0 Mu  SELECTION ##########################
        
        self.selphimu = Selection(name = "PhiMuFor"+name,
                                    Algorithm          = self._PhiMuFilter(),
                                    RequiredSelections = [self.selphi2kk,self.selmuon])



        ################ B0 -> X Tau Mu  SELECTION ##########################

        # _B_dec_descriptors = config['DECAYS']
        # if config["MuTauWS"]: _B_dec_descriptors +=  config['DECAYS_WS']


        self.selb2Kst0TauMu = self._B2XTauMu(name_kst,
                                             DecayDescriptors = ["[B0 ->  K*(1410)+  mu-]cc"],
                                             MuSel = self.selmuon,
                                             XMuSel = [self.selkstarmu],
                                             MuonP = config['MuonP_kst'],
                                             MuonPT = config['MuonPT_kst'],
                                             PIDmu = config['PIDmu'],
                                             MuonIPCHI2 = config['MINIPCHI2'],
                                             BVCHI2DOF = config['BVCHI2DOF_kst'],
                                             BDIRA = config['BDIRA_kst'],
                                             B_FDCHI2 = config["B_FDCHI2_kst"],
                                             B_MIN_MASS = config['B_MIN_MASS'], 
                                             B_MAX_MASS = config['B_MAX_MASS'])


        self.selb2PhiTauMu = self._B2XTauMu(name_phi,
                                            DecayDescriptors = ["[B0 ->  K_2(1770)+  mu-]cc"],
                                            MuSel = self.selmuon,
                                            XMuSel =  [self.selphimu],
                                            MuonP = 0,
                                            MuonPT = 0,
                                            PIDmu = config['PIDmu'],
                                            MuonIPCHI2 = config['MINIPCHI2'],
                                            BVCHI2DOF = config['BVCHI2DOF_phi'],
                                            BDIRA = 0,
                                            B_FDCHI2 = 0,
                                            B_MIN_MASS = config['B_MIN_MASS'], B_MAX_MASS = config['B_MAX_MASS'])


        self.selb2KTauMu = self._B2XTauMu(name_k,
                                          DecayDescriptors = ["[B+ ->  K*(1410)0  mu+]cc",     # K*(1410)0 -> K+ mu-
                                                              "[B+ ->  Delta(1600)++  mu-]cc"], # Delta(1600)++ -> K+ mu+,
                                          MuSel = self.selmuon,
                                          XMuSel = [self.selkmuOS,
                                                    self.selkmuSS],
                                          MuonP = config['MuonP_K'],
                                          MuonPT = config['MuonPT_K'],
                                          PIDmu = config['PIDmu'],
                                          MuonIPCHI2 = config['MINIPCHI2_K'],
                                          BVCHI2DOF = config['BVCHI2DOF_K'],
                                          BDIRA = config['BDIRA_K'],
                                          B_FDCHI2 = config["B_FDCHI2_K"],
                                          B_MIN_MASS = config['B_MIN_MASS'], B_MAX_MASS = config['B_MAX_MASS'])



        ### B -> Phi Tau(3pi) Mu
        self.selb2PhiTauMu_3pi = self._B2XTauMu_3pi(name_phi_3pi,
                                                    DecayDescriptors = ["[B0 ->  phi(1020)  tau+ mu-]cc",  
                                                                        "[B0 ->  phi(1020)  tau- mu+]cc"], 
                                                    
                                                    TauSel = StdTightDetachedTau3pi,
                                                    MuSel = self.selmuon,
                                                    XSel = [self.selphi2kk],
                                                    BVCHI2DOF = config['BVCHI2DOF_phi'],
                                                    BDIRA = 0,
                                                    B_FDCHI2 = 0,
                                                    B_MIN_MASS = config['B_MIN_MASS'], B_MAX_MASS = config['B_MAX_MASS'])
        


        ################ B0 -> X Tau Mu WS  SELECTION ##########################



        self.selb2Kst0TauMu_WS = self._B2XTauMu(name_kst_WS,
                                                DecayDescriptors = ["[B0 ->  K*(1410)+  mu+]cc"],
                                                MuSel = self.selmuon,
                                                XMuSel = [self.selkstarmu],
                                                MuonP = config['MuonP_kst'],
                                                MuonPT = config['MuonPT_kst'],
                                                PIDmu = config['PIDmu'],
                                                MuonIPCHI2 = config['MINIPCHI2'],
                                                BVCHI2DOF = config['BVCHI2DOF_kst'],
                                                BDIRA = config['BDIRA_kst'],
                                                B_FDCHI2 = config["B_FDCHI2_kst"],
                                                B_MIN_MASS = config['B_MIN_MASS'], B_MAX_MASS = config['B_MAX_MASS'])


        self.selb2PhiTauMu_WS = self._B2XTauMu(name_phi_WS,
                                               DecayDescriptors = ["[B0 ->  K_2(1770)+  mu+]cc"],
                                               MuSel = self.selmuon,
                                               XMuSel =  [self.selphimu],
                                               MuonP = 0,
                                               MuonPT = 0,
                                               PIDmu = config['PIDmu'],
                                               MuonIPCHI2 = config['MINIPCHI2'],
                                               BVCHI2DOF = config['BVCHI2DOF_phi'],
                                               BDIRA = 0,
                                               B_FDCHI2 = 0,
                                               B_MIN_MASS = config['B_MIN_MASS'], B_MAX_MASS = config['B_MAX_MASS'])


        self.selb2KTauMu_WS = self._B2XTauMu(name_k_WS,
                                             DecayDescriptors = ["[B+ ->  K*(1410)0  mu-]cc",     # K*(1410)0 -> K+ mu-
                                                                 "[B+ ->  Delta(1600)++  mu+]cc"], # Delta(1600)++ -> K+ mu+
                                             MuSel = self.selmuon,
                                             XMuSel = [self.selkmuOS,
                                                       self.selkmuSS],
                                             MuonP = config['MuonP_K'],
                                             MuonPT = config['MuonPT_K'],
                                             PIDmu = config['PIDmu'],
                                             MuonIPCHI2 = config['MINIPCHI2_K'],
                                             BVCHI2DOF = config['BVCHI2DOF_K'],
                                             BDIRA = config['BDIRA_K'],
                                             B_FDCHI2 = config["B_FDCHI2_K"],
                                            B_MIN_MASS = config['B_MIN_MASS'], B_MAX_MASS = config['B_MAX_MASS'])


        ### B -> Phi Tau(3pi) Mu
        self.selb2PhiTauMu_3pi_WS = self._B2XTauMu_3pi(name_phi_3pi_WS,
                                                       DecayDescriptors = ["[B0 ->  phi(1020)  tau+ mu+]cc"], 
                                                       TauSel = StdTightDetachedTau3pi,
                                                       MuSel = self.selmuon,
                                                       XSel = [self.selphi2kk],
                                                       BVCHI2DOF = config['BVCHI2DOF_phi'],
                                                       BDIRA = 0,
                                                       B_FDCHI2 = 0,
                                                       B_MIN_MASS = config['B_MIN_MASS'], B_MAX_MASS = config['B_MAX_MASS'])



        ################# DECLARE THE STRIPPING LINES #################################
        
        self.B2KstTauMu = StrippingLine(name_kst+ 'Line', 
                                        prescale = config["Prescale"], 
                                        selection = self.selb2Kst0TauMu)

        self.B2PhiTauMu = StrippingLine(name_phi+ 'Line', 
                                        prescale = config["Prescale"], 
                                        selection = self.selb2PhiTauMu)

        self.B2KTauMu = StrippingLine(name_k+ 'Line', 
                                        prescale = config["Prescale"], 
                                        selection = self.selb2KTauMu)

        self.B2PhiTauMu_3pi = StrippingLine(name_phi_3pi+ 'Line', 
                                            prescale = config["Prescale"], 
                                            selection = self.selb2PhiTauMu_3pi)



        self.B2KstTauMu_WS = StrippingLine(name_kst_WS+ 'Line', 
                                        prescale = config["Prescale_WS"], 
                                        selection = self.selb2Kst0TauMu_WS)

        self.B2PhiTauMu_WS = StrippingLine(name_phi_WS+ 'Line', 
                                        prescale = config["Prescale_WS"], 
                                        selection = self.selb2PhiTauMu_WS)

        self.B2KTauMu_WS = StrippingLine(name_k_WS+ 'Line', 
                                        prescale = config["Prescale_WS"], 
                                        selection = self.selb2KTauMu_WS)

        self.B2PhiTauMu_3pi_WS = StrippingLine(name_phi_3pi_WS+ 'Line', 
                                               prescale = config["Prescale_WS"], 
                                               selection = self.selb2PhiTauMu_3pi_WS)


        self.registerLine(self.B2KstTauMu)        
        self.registerLine(self.B2PhiTauMu)        
        self.registerLine(self.B2KTauMu)        
        self.registerLine(self.B2PhiTauMu_3pi)        

        if config["MuTauWS"]: 
            self.registerLine(self.B2KstTauMu_WS)        
            self.registerLine(self.B2PhiTauMu_WS)        
            self.registerLine(self.B2KTauMu_WS)        
            self.registerLine(self.B2PhiTauMu_3pi_WS)        




    def _muonFilter( self ):

        _code = "(TRCHI2DOF< %(TrackCHI2)s)" % self.__confdict__
        _code += " & (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" % self.__confdict__
        _code += " & (TRGHOSTPROB < %(TrackGhostProb)s)" % self.__confdict__ 
        _code += " & (PIDmu > %(PIDmu)s)" % self.__confdict__ 
        
        _mu = FilterDesktop( Code = _code )
        return _mu 
        

    def _pionFilter( self ):
        
        _code = "(TRCHI2DOF< %(TrackCHI2)s)" % self.__confdict__
        _code += " & (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" % self.__confdict__
        _code += " & (TRGHOSTPROB < %(TrackGhostProb)s)" % self.__confdict__ 
        _code += " & (PIDK < %(PionPIDK)s)" % self.__confdict__ 

        _pi = FilterDesktop( Code = _code )
        return _pi
        

    def _kaonFilter( self ):
        
        _code = "(TRCHI2DOF< %(TrackCHI2)s)" % self.__confdict__
        _code += " & (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" % self.__confdict__
        _code += " & (TRGHOSTPROB < %(TrackGhostProb)s)" % self.__confdict__ 
        _code += " & (PIDK > %(KaonPIDK)s)" % self.__confdict__ 

        _ka = FilterDesktop( Code = _code )
        return _ka     



   
    def _Phi2KKFilter( self ):

        _decayDescriptors = [ 'phi(1020) -> K+ K-' ]

        _combinationCut = "(ADAMASS('phi(1020)') < %(PhiAMassWin)s *MeV)" % self.__confdict__

        _motherCut = "(ADMASS('phi(1020)') < %(PhiMassWin)s *MeV)"\
                     " & (VFASPF(VCHI2/VDOF) < %(PhiVCHI2DOF)s)" % self.__confdict__


        _phi2kpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                      CombinationCut = _combinationCut,
                                      MotherCut = _motherCut)                            
        return _phi2kpi


    def _Kst02KPiFilter( self ):

        _decayDescriptors = [ '[K*(892)0 -> K+ pi-]cc' ]

        _daughtersCuts = {  "K+"  : "(P > %(KaonP_kst)s *GeV) & (PT > %(KPiPT_kst)s *MeV)" % self.__confdict__
                           ,"pi-" : "(P > %(PionP_kst)s *GeV) & (PT > %(KPiPT_kst)s *MeV)" % self.__confdict__ }

        _combinationCut = "(ADAMASS('K*(892)0') < %(KstAMassWin)s *MeV)" % self.__confdict__

        _motherCut = "(ADMASS('K*(892)0') < %(KstMassWin)s *MeV)"\
                     " & (VFASPF(VCHI2/VDOF) < %(KstVCHI2DOF)s)" % self.__confdict__


        _kst02kpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                      DaughtersCuts = _daughtersCuts,
                                      CombinationCut = _combinationCut,
                                      MotherCut = _motherCut)                            
        return _kst02kpi



    def _PhiMuFilter( self ):

        _decayDescriptors = ["[K_2(1770)+ -> phi(1020) mu+]cc"]

        _motherCut = "(VFASPF(VCHI2/VDOF)  < %(XMuVCHI2DOF_phi)s) " % self.__confdict__


        _phiMu = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    MotherCut = _motherCut)                            
        return _phiMu


    def _Kst0MuFilter( self ):

        _decayDescriptors = ["[K*(1410)+ -> K*(892)0 mu+]cc","[K*(1410)- -> K*(892)0 mu-]cc"]

        _daughtersCuts = {  "mu+"  : "(P > %(MuonP_kst)s *GeV) & (PT > %(MuonPT_kst)s *MeV)" % self.__confdict__
                            ,"mu-"  : "(P > %(MuonP_kst)s *GeV) & (PT > %(MuonPT_kst)s *MeV)" % self.__confdict__ }


        _motherCut = "(VFASPF(VCHI2/VDOF)  < %(XMuVCHI2DOF_kst)s) " % self.__confdict__


        _kst0Mu = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    DaughtersCuts = _daughtersCuts,
                                    MotherCut = _motherCut)                            
        return _kst0Mu


    def _KMuOSFilter( self ):
        _decayDescriptors = ["[K*(1410)0 -> K+ mu-]cc"]

        _daughtersCuts = {  "K+"  : "(P > %(KaonP_K)s *GeV) & (PIDK> %(KaonPIDK_K)s) & (PT > %(KPiPT_K)s *MeV) & (MIPCHI2DV(PRIMARY) > %(MINIPCHI2_K)s )" % self.__confdict__
                            ,"mu+" : " (MIPCHI2DV(PRIMARY) > %(MINIPCHI2_K)s )" % self.__confdict__ }

        _combinationCut = "((ACHILD(PT,1)+ACHILD(PT,2)) > %(KMuSumPT_K)s)" % self.__confdict__ 

        _motherCut = "(VFASPF(VCHI2/VDOF)  < %(XMuVCHI2DOF_K)s) "\
                     "& (PT >%(KMuPT_K)s)"% self.__confdict__


        _kMu = CombineParticles( DecayDescriptors = _decayDescriptors,
                                 DaughtersCuts = _daughtersCuts,
                                 CombinationCut = _combinationCut,
                                 MotherCut = _motherCut)                            
        return _kMu


    def _KMuSSFilter( self ):

        _decayDescriptors = ["[Delta(1600)++ -> K+ mu+]cc"]

        _daughtersCuts = {  "K+"  : "(P > %(KaonP_K)s *GeV) & (PIDK> %(KaonPIDK_K)s) & (PT > %(KPiPT_K)s *MeV) & (MIPCHI2DV(PRIMARY) > %(MINIPCHI2_K)s )" % self.__confdict__
                            ,"mu+" : " (MIPCHI2DV(PRIMARY) > %(MINIPCHI2_K)s )" % self.__confdict__}

        _combinationCut = "((ACHILD(PT,1)+ACHILD(PT,2)) > %(KMuSumPT_K)s)" % self.__confdict__ 

        _motherCut = "(VFASPF(VCHI2/VDOF)  < %(XMuVCHI2DOF_K)s) "\
                     "& (PT > %(KMuPT_K)s)"% self.__confdict__


        _kMu = CombineParticles( DecayDescriptors = _decayDescriptors,
                                 DaughtersCuts = _daughtersCuts,
                                 CombinationCut = _combinationCut,
                                 MotherCut = _motherCut)                            
        return _kMu


    def _B2XTauMu(self,
                  name,
                  DecayDescriptors,
                  MuSel,
                  XMuSel,
                  MuonP,
                  MuonPT,
                  PIDmu,
                  MuonIPCHI2,
                  BVCHI2DOF,
                  BDIRA,
                  B_FDCHI2,
                  B_MIN_MASS,
                  B_MAX_MASS):


        _daughtersCuts = {  "mu+"  : "(P > %(MuonP)s *GeV) & (PT > %(MuonPT)s *MeV) & (PIDmu > %(PIDmu)s) & (MIPCHI2DV(PRIMARY) > %(MuonIPCHI2)s )" % locals()
                            ,"mu-"  : "(P > %(MuonP)s *GeV) & (PT > %(MuonPT)s *MeV) & (PIDmu > %(PIDmu)s) & (MIPCHI2DV(PRIMARY) > %(MuonIPCHI2)s )" % locals() }

        _combinationCut = "(AM<( %(B_MAX_MASS)s + 200) *MeV)"  % locals()
        _motherCut = "  (MM < %(B_MAX_MASS)s *MeV)"\
                     " & (MM> %(B_MIN_MASS)s *MeV)"\
                     " & (VFASPF(VCHI2/VDOF) < %(BVCHI2DOF)s)"\
                     " & (BPVDIRA > %(BDIRA)s)"\
                     " & (BPVVDCHI2 > %(B_FDCHI2)s)"  % locals()
        

        _B = CombineParticles(DecayDescriptors = DecayDescriptors,
                              DaughtersCuts = _daughtersCuts,
                              CombinationCut = _combinationCut,
                              MotherCut = _motherCut)
                          
        _sel_Daughters = MergedSelection(name+"_daughters",
                                         RequiredSelections = XMuSel )
        
        sel = Selection( name,
                         Algorithm = _B,
                         RequiredSelections = [ MuSel, _sel_Daughters ])
        return sel





    def _B2XTauMu_3pi(self,
                      name,
                      DecayDescriptors,
                      TauSel,
                      MuSel,
                      XSel,
                      BVCHI2DOF,
                      BDIRA,
                      B_FDCHI2,
                      B_MIN_MASS,
                      B_MAX_MASS):

        _combinationCut = "(AM< ( %(B_MAX_MASS)s + 200) *MeV)" % locals()
        _motherCut = "  (MM < %(B_MAX_MASS)s *MeV)"\
                     " & (MM> %(B_MIN_MASS)s *MeV)"\
                     " & (VFASPF(VCHI2/VDOF) < %(BVCHI2DOF)s)"\
                     " & (BPVDIRA > %(BDIRA)s)"\
                     " & (BPVVDCHI2 > %(B_FDCHI2)s)" % locals()


        _B = CombineParticles(DecayDescriptors = DecayDescriptors,
                              CombinationCut = _combinationCut,
                              MotherCut = _motherCut)
                          
        _sel_Daughters = MergedSelection(name+"_daughters",
                                         RequiredSelections = XSel )
        
        sel = Selection( name,
                         Algorithm = _B,
                         RequiredSelections = [ TauSel, MuSel, _sel_Daughters ])
        return sel





    
