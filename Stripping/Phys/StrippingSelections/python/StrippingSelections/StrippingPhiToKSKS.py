"""
Study K0 regeneration with phi->KsKs and J/psi ->KsKs.
Idea from Thomas Ruf.
"""
__author__ = ['Mika Vesterinen','Thomas Ruf']
__date__ = '30/07/2013'
__version__ = '$Revision: 0.0 $'

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseKaons

__all__ = ('PhiToKSKSAllLinesConf',
           'TOSFilter',
           'confdict')

confdict = {
    "GEC_nLongTrk" : 250,
    "prescale_PhiToKK" : 1.0,
    "prescale_PhiToKsKs" : 1.0,
    "prescale_JPsiToKsKs" : 1.0,
    "prescale_JPsiToKK" : 1.0,
    "GHOSTPROB_MAX" : 0.35, #adimensional
    "K_PTMIN" : 200, #MeV
    "K_PIDK_MIN" : 4, #adimensional
    "K_IPCHI2_MAX" : 9, #adimensional
    "KS_PTMIN" : 200, #MeV
    "KS_MASS_WINDOW" : 50, #MeV
    "KS_FD_MIN" : 10.0, #mm
    "KS_FDCHI2_MIN" : 100, #adimensional
    "KS_VCHI2NDOF_MAX" : 4,#adimensional
    "KS_DIRA_MIN" : 0.9999, #adimensional
    "Phi_MASS_MAX" : 1100, # MeV
    "Phi_PT_MIN" : 400, # MeV
    "Phi_DOCACHI2_MAX" : 10, #adimensional
    "Phi_VCHI2NDOF_MAX" : 6, #adimensional
    "Phi_IPCHI2_MAX" : 9, #adimensional
    "JPsi_MASS_WIN" : 120, # MeV
    "JPsi_PT_MIN" : 500, # MeV
    "JPsi_DOCACHI2_MAX" : 10, #adimensional
    "JPsi_VCHI2NDOF_MAX" : 6, #adimensional
    "JPsi_IPCHI2_MAX" : 9 #adimensional
    }   

class PhiToKSKSAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "GEC_nLongTrk",
        "prescale_PhiToKK",
        "prescale_PhiToKsKs",
        "prescale_JPsiToKsKs",
        "prescale_JPsiToKK",
        "GHOSTPROB_MAX",
        "K_PTMIN",
        "K_PIDK_MIN",
        "K_IPCHI2_MAX",
        "KS_PTMIN",
        "KS_MASS_WINDOW",
        "KS_FD_MIN",
        "KS_FDCHI2_MIN",
        "KS_VCHI2NDOF_MAX",
        "KS_DIRA_MIN",
        "Phi_MASS_MAX",
        "Phi_PT_MIN",
        "Phi_DOCACHI2_MAX",
        "Phi_VCHI2NDOF_MAX",
        "Phi_IPCHI2_MAX",
        "JPsi_MASS_WIN",
        "JPsi_PT_MIN",
        "JPsi_DOCACHI2_MAX",
        "JPsi_VCHI2NDOF_MAX",
        "JPsi_IPCHI2_MAX"
        )
    
    __confdict__={}
    
    def __init__(self, _name, config) :

        LineBuilder.__init__(self, _name, config)
        self.__confdict__=config
        
        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" %config ,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}

        self.KsLLCuts = " (PT> %(K_PTMIN)s *MeV)" \
            " & (ADMASS('KS0') < %(KS_MASS_WINDOW)s *MeV)"\
            " & (BPVVD > %(KS_FD_MIN)s *mm)" \
            " & (BPVVDCHI2 > %(KS_FDCHI2_MIN)s)" \
            " & CHILDCUT((TRGHOSTPROB < %(GHOSTPROB_MAX)s),1)" \
            " & CHILDCUT((TRGHOSTPROB < %(GHOSTPROB_MAX)s),2)" \
            " & (VFASPF(VCHI2PDOF) < %(KS_VCHI2NDOF_MAX)s)" \
            " & (BPVDIRA > %(KS_DIRA_MIN)s)" %self.__confdict__

        self.KaonCuts = " (PT> %(K_PTMIN)s *MeV)" \
            " & (PIDK > %(K_PIDK_MIN)s)"\
            " & (MIPCHI2DV(PRIMARY) < %(K_IPCHI2_MAX)s)"\
            " & (TRGHOSTPROB < %(GHOSTPROB_MAX)s)" %self.__confdict__
        
        
        self.KsLL = Selection( "KsLLFor" + _name,
                               Algorithm = FilterDesktop(name = "KsLLFilterFor"+_name, Code = self.KsLLCuts ),
                               RequiredSelections = [DataOnDemand(Location = 'Phys/StdLooseKsLL/Particles')])
        
        self.Kaons = Selection( "KaonsFor" + _name,
                                Algorithm = FilterDesktop(name = "KaonFilterFor"+_name, Code = self.KaonCuts ),
                                RequiredSelections = [StdLooseKaons])
        
        ########### phi(1020) -> KsKs
        self.PhiToKsKs_CombiCut = "(APT > %(Phi_PT_MIN)s *MeV) & (AM < %(Phi_MASS_MAX)s + 20*MeV) & (ACUTDOCACHI2(%(Phi_DOCACHI2_MAX)s,''))" %self.__confdict__
        self.PhiToKsKs_MotherCut = "(M < %(Phi_MASS_MAX)s *MeV) & (VFASPF(VCHI2/VDOF) < %(Phi_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(Phi_IPCHI2_MAX)s)" %self.__confdict__
        
        self.PhiToKsKs_Comb = CombineParticles( name = "CombPhiToKsKs_"+_name,
                                      DecayDescriptors = ["phi(1020) -> KS0 KS0"],
                                      CombinationCut = self.PhiToKsKs_CombiCut,
                                      MotherCut = self.PhiToKsKs_MotherCut)

        self.PhiToKsKs_Sel = Selection(name = "SelPhiToKsKs_"+_name,
                                  Algorithm = self.PhiToKsKs_Comb,
                                  RequiredSelections = [self.KsLL])
        
        self.PhiToKsKs_Line = StrippingLine(_name+'_PhiToKsKs',
                                            prescale = config["prescale_PhiToKsKs"],
                                            FILTER=GECs,
                                            selection = self.PhiToKsKs_Sel)
        
        self.registerLine(self.PhiToKsKs_Line)        

        ########### phi(1020) -> KK
        self.PhiToKK_CombiCut = "(APT > %(Phi_PT_MIN)s *MeV) & (AM < %(Phi_MASS_MAX)s + 20*MeV) & (ACUTDOCACHI2(%(Phi_DOCACHI2_MAX)s,''))" %self.__confdict__
        self.PhiToKK_MotherCut = "(M < %(Phi_MASS_MAX)s *MeV) & (VFASPF(VCHI2/VDOF) < %(Phi_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(Phi_IPCHI2_MAX)s)" %self.__confdict__
        
        self.PhiToKK_Comb = CombineParticles( name = "CombPhiToKK_"+_name,
                                      DecayDescriptors = ["phi(1020) -> K+ K-"],
                                      CombinationCut = self.PhiToKK_CombiCut,
                                      MotherCut = self.PhiToKK_MotherCut)

        self.PhiToKK_Sel = Selection(name = "SelPhiToKK_"+_name,
                                  Algorithm = self.PhiToKK_Comb,
                                  RequiredSelections = [self.Kaons])
        
        self.PhiToKK_Line = StrippingLine(_name+'_PhiToKK',
                                          prescale = config["prescale_PhiToKK"],
                                          FILTER=GECs,
                                          selection = self.PhiToKK_Sel)
        
        self.registerLine(self.PhiToKK_Line)        


        
        ########### J/psi(1S) -> KsKs
        self.JPsiToKsKs_CombiCut = "(ADAMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s +10*MeV) & (APT > %(JPsi_PT_MIN)s*MeV)"\
            "& (ACUTDOCACHI2(%(JPsi_DOCACHI2_MAX)s,''))" %self.__confdict__
        self.JPsiToKsKs_MotherCut = "(DMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s *MeV)"\
            "& (VFASPF(VCHI2/VDOF) < %(JPsi_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(JPsi_IPCHI2_MAX)s)" %self.__confdict__
        
        self.JPsiToKsKs_Comb = CombineParticles( name = "CombJPsiToKsKs_"+_name,
                                                 DecayDescriptors = ["J/psi(1S) -> KS0 KS0"],
                                                 CombinationCut = self.JPsiToKsKs_CombiCut,
                                                 MotherCut = self.JPsiToKsKs_MotherCut)
        
        self.JPsiToKsKs_Sel = Selection(name = "SelJPsiToKsKs_"+_name,
                                        Algorithm = self.JPsiToKsKs_Comb,
                                        RequiredSelections = [self.KsLL])
        
        self.JPsiToKsKs_Line = StrippingLine(_name+'_JPsiToKsKs',
                                             prescale = config["prescale_JPsiToKsKs"],
                                             FILTER=GECs,
                                             selection = self.JPsiToKsKs_Sel)
        
        self.registerLine(self.JPsiToKsKs_Line)        
        
        ########### J/psi(1S) -> KK
        self.JPsiToKK_CombiCut = "(ADAMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s +10*MeV) & (APT > %(JPsi_PT_MIN)s*MeV)"\
            "& (ACUTDOCACHI2(%(JPsi_DOCACHI2_MAX)s,''))" %self.__confdict__
        self.JPsiToKK_MotherCut = "(DMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s *MeV)"\
            "& (VFASPF(VCHI2/VDOF) < %(JPsi_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(JPsi_IPCHI2_MAX)s)" %self.__confdict__
        
        self.JPsiToKK_Comb = CombineParticles( name = "CombJPsiToKK_"+_name,
                                                 DecayDescriptors = ["J/psi(1S) -> K+ K-"],
                                                 CombinationCut = self.JPsiToKK_CombiCut,
                                                 MotherCut = self.JPsiToKK_MotherCut)
        
        self.JPsiToKK_Sel = Selection(name = "SelJPsiToKK_"+_name,
                                        Algorithm = self.JPsiToKK_Comb,
                                        RequiredSelections = [self.Kaons])
        
        self.JPsiToKK_Line = StrippingLine(_name+'_JPsiToKK',
                                             prescale = config["prescale_JPsiToKK"],
                                             FILTER=GECs,
                                             selection = self.JPsiToKK_Sel)
        
        self.registerLine(self.JPsiToKK_Line)        
