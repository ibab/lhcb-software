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
from Configurables import FitDecayTrees
from PhysSelPython.Wrappers import AutomaticData

__all__ = ('PhiToKSKSAllLinesConf',
           'TOSFilter',
           'confdict')

confdict = {
    "prescale_PhiToKK" : 1.0,
    "prescale_PhiToKsKs" : 1.0,
    "prescale_JPsiToKK" : 1.0,
    "prescale_JPsiToKsKs" : 1.0,
    
    "GEC_nLongTrk" : 250,
    "DoDTF" : True,
    "GHOSTPROB_MAX" : 0.35, #adimensional
    
    "K_PTMIN" : 200, #MeV
    "K_PIDK_MIN" : 7, #adimensional
    "K_IPCHI2_MAX" : 9, #adimensional
    
    "KS_LL_PTMIN" : 200, #MeV
    "KS_LL_MASS_WINDOW" : 50, #MeV
    "KS_LL_FD_MIN" : 10.0, #mm
    "KS_LL_FDCHI2_MIN" : 100, #adimensional
    "KS_LL_VCHI2NDOF_MAX" : 4,#adimensional
    "KS_LL_DIRA_MIN" : 0.9999, #adimensional
    
    "KS_DD_PTMIN" : 200, #MeV
    "KS_DD_MASS_WINDOW" : 50, #MeV
    "KS_DD_FD_MIN" : 10.0, #mm
    "KS_DD_FDCHI2_MIN" : 100, #adimensional
    "KS_DD_VCHI2NDOF_MAX" : 4,#adimensional
    "KS_DD_DIRA_MIN" : 0.999, #adimensional
    
    "Phi_MASS_MAX" : 1100, # MeV
    "Phi_PT_MIN" : 400, # MeV
    "Phi_DOCACHI2_MAX" : 20, #adimensional
    "Phi_VCHI2NDOF_MAX" : 6, #adimensional
    "Phi_IPCHI2_MAX" : 9, #adimensional

    "JPsi_MASS_WIN" : 120, # MeV
    "JPsi_PT_MIN" : 500, # MeV
    "JPsi_DOCACHI2_MAX" : 20, #adimensional
    "JPsi_VCHI2NDOF_MAX" : 6, #adimensional
    "JPsi_IPCHI2_MAX" : 9 #adimensional
    
    }   

class PhiToKSKSAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "prescale_PhiToKK" ,
        "prescale_PhiToKsKs" ,
        "prescale_JPsiToKK" ,
        "prescale_JPsiToKsKs" ,
        "GEC_nLongTrk",
        "DoDTF",
        "GHOSTPROB_MAX",
        "K_PTMIN",
        "K_PIDK_MIN",
        "K_IPCHI2_MAX",
        "KS_LL_PTMIN",
        "KS_LL_MASS_WINDOW",
        "KS_LL_FD_MIN",
        "KS_LL_FDCHI2_MIN",
        "KS_LL_VCHI2NDOF_MAX",
        "KS_LL_DIRA_MIN",
        "KS_DD_PTMIN",
        "KS_DD_MASS_WINDOW",
        "KS_DD_FD_MIN",
        "KS_DD_FDCHI2_MIN",
        "KS_DD_VCHI2NDOF_MAX",
        "KS_DD_DIRA_MIN",
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
        
        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" %config ,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        self.KsLLCuts = " (PT> %(KS_LL_PTMIN)s *MeV)" \
            " & (ADMASS('KS0') < %(KS_LL_MASS_WINDOW)s *MeV)"\
            " & (BPVVD > %(KS_LL_FD_MIN)s *mm)" \
            " & (BPVVDCHI2 > %(KS_LL_FDCHI2_MIN)s)" \
            " & CHILDCUT((TRGHOSTPROB < %(GHOSTPROB_MAX)s),1)" \
            " & CHILDCUT((TRGHOSTPROB < %(GHOSTPROB_MAX)s),2)" \
            " & (VFASPF(VCHI2PDOF) < %(KS_LL_VCHI2NDOF_MAX)s)" \
            " & (BPVDIRA > %(KS_LL_DIRA_MIN)s)" %config
        
        self.KsDDCuts = " (PT> %(KS_DD_PTMIN)s *MeV)" \
            " & (ADMASS('KS0') < %(KS_DD_MASS_WINDOW)s *MeV)"\
            " & (BPVVD > %(KS_DD_FD_MIN)s *mm)" \
            " & (BPVVDCHI2 > %(KS_DD_FDCHI2_MIN)s)" \
            " & (VFASPF(VCHI2PDOF) < %(KS_DD_VCHI2NDOF_MAX)s)" \
            " & (BPVDIRA > %(KS_DD_DIRA_MIN)s)" %config

        self.KaonCuts = " (PT> %(K_PTMIN)s *MeV)" \
            " & (PIDK > %(K_PIDK_MIN)s)"\
            " & (MIPCHI2DV(PRIMARY) < %(K_IPCHI2_MAX)s)"\
            " & (TRGHOSTPROB < %(GHOSTPROB_MAX)s)" %config
        
        
        self.KsLL = Selection( "KsLLFor" + _name,
                               Algorithm = FilterDesktop(name = "KsLLFilterFor"+_name, Code = self.KsLLCuts ),
                               RequiredSelections = [DataOnDemand(Location = 'Phys/StdLooseKsLL/Particles')])
        
        self.KsDD = Selection( "KsDDFor" + _name,
                               Algorithm = FilterDesktop(name = "KsDDFilterFor"+_name, Code = self.KsDDCuts ),
                               RequiredSelections = [DataOnDemand(Location = 'Phys/StdLooseKsDD/Particles')])
        
        self.Kaons = Selection( "KaonsFor" + _name,
                                Algorithm = FilterDesktop(name = "KaonFilterFor"+_name, Code = self.KaonCuts ),
                                RequiredSelections = [StdLooseKaons])
        
        ####### phi(1020) #############

        self.PhiToKK_Line = VMaker(_name+"_PhiToKK",[self.Kaons],"phi(1020) -> K+ K-",GECs,config,config["prescale_PhiToKK"])
        self.registerLine(self.PhiToKK_Line)        
        
        self.PhiToKsKs_Line = VMaker(_name+"_PhiToKsKs",[self.KsLL,self.KsDD],"phi(1020) -> KS0 KS0",GECs,config,config["prescale_PhiToKsKs"])
        self.registerLine(self.PhiToKsKs_Line)        
        
        ########### J/psi(1S)  ################
        
        self.JPsiToKK_Line = VMaker(_name+"_JPsiToKK",[self.Kaons],"J/psi(1S) -> K+ K-",GECs,config,config["prescale_JPsiToKK"])
        self.registerLine(self.JPsiToKK_Line)        
        
        self.JPsiToKsKs_Line = VMaker(_name+"_JPsiToKsKs",[self.KsLL,self.KsDD],"J/psi(1S) -> KS0 KS0",GECs,config,config["prescale_JPsiToKsKs"])
        self.registerLine(self.JPsiToKsKs_Line)        
        

def VMaker(_name,_RequiredSelections,_DecayDescriptor,_Filter,conf,_prescale):
    
    _CombiCut = ""
    if 'KS0' in _DecayDescriptor:
        _CombiCut = "( (ACHILDCUT(CHILDCUT(ISLONG,1),1)) | (ACHILDCUT(CHILDCUT(ISLONG,1),2)) ) & "
    if 'phi(1020)' in _DecayDescriptor:
        _CombiCut += " (APT > %(Phi_PT_MIN)s *MeV) & (AM < %(Phi_MASS_MAX)s + 30*MeV) & (ACUTDOCACHI2(%(Phi_DOCACHI2_MAX)s,''))" %conf
        _MotherCut = "(M < %(Phi_MASS_MAX)s +20*MeV) & (VFASPF(VCHI2/VDOF) < %(Phi_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(Phi_IPCHI2_MAX)s)" %conf
        _MassFilter = FilterDesktop(name = "MassFilter_"+_name,Code = "(M < %(Phi_MASS_MAX)s *MeV)" %conf)
    elif 'J/psi(1S)' in _DecayDescriptor:
        _CombiCut += " (ADAMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s +30*MeV) & (APT > %(JPsi_PT_MIN)s*MeV)"\
            "& (ACUTDOCACHI2(%(JPsi_DOCACHI2_MAX)s,''))" %conf
        _MotherCut = "(DMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s +20*MeV)"\
            "& (VFASPF(VCHI2/VDOF) < %(JPsi_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(JPsi_IPCHI2_MAX)s)" %conf
        _MassFilter = FilterDesktop(name = "MassFilter_"+_name,Code = "(DMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s *MeV)" %conf)
        
    
        
    Comb = CombineParticles( name = "Comb_"+_name,
                             DecayDescriptor = _DecayDescriptor,
                             CombinationCut = _CombiCut,
                             MotherCut = _MotherCut)

    Sel = Selection(name = "Sel_"+_name,
                    Algorithm = Comb,
                    RequiredSelections = _RequiredSelections)
        
    if conf['DoDTF'] == True:
        _MassConstraints = []
        if 'KS0' in _DecayDescriptor:
            _MassConstraints = ['KS0']
        FITTER = FitDecayTrees (
            name = "FITTER_"+_name,
            Code = "DECTREE('" + _DecayDescriptor +"')",
            MaxChi2PerDoF = 10,
            MassConstraints = _MassConstraints,
            UsePVConstraint = True)
        FITTER_SEL = Selection("FITTER_SEL_"+_name,
                               Algorithm = FITTER,
                               RequiredSelections = [Sel])
        MASS_FILTER = Selection("MASS_FILTER_"+_name,Algorithm = _MassFilter,RequiredSelections = [FITTER_SEL])
    else:
        MASS_FILTER = Selection("MASS_FILTER_"+_name,Algorithm = _MassFilter,RequiredSelections = [Sel])
        
    Line = StrippingLine(_name+'Line',
                         prescale = _prescale,
                         FILTER=_Filter,
                         selection = MASS_FILTER)
        
    return Line
