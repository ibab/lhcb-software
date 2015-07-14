"""
Study K0 regeneration with phi->KsKs and J/psi ->KsKs.
Idea from Thomas Ruf and originally implemented by Mika Vesterinen.
Updated by Jon Harrison to include D0->KsKs.
"""
__author__ = ['Jon Harrison']
__date__ = '14/05/2015'
__version__ = '$Revision: 0.2 $'

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseKaons, StdAllLooseMuons
from Configurables import FitDecayTrees
from PhysSelPython.Wrappers import AutomaticData, MergedSelection

__all__ = ('PhiToKSKSAllLinesConf',
           'TOSFilter',
           'confdict')

confdict = {
    "prescale_PhiToKK" : 0.001,
    "prescale_PhiToKsKs" : 1.0,
    "prescale_PhiToMuMu" : 0.01,
    "prescale_JPsiToKK" : 0.01,
    "prescale_JPsiToKsKs" : 1.0,
    "prescale_D0ToKsKs" : 1.0,
    "prescale_EtaCToLL" : 1.0,
    
    "GEC_nLongTrk" : 250,
    "DoDTF" : True,
    "GHOSTPROB_MAX" : 0.35, #adimensional
    
    "K_PTMIN" : 200, #MeV
    "K_PIDK_MIN" : 7, #adimensional
    "K_IPCHI2_MAX" : 9, #adimensional
    
    "Mu_PTMIN" : 200, #MeV
    "Mu_PIDmu_MIN" : 0, #adimensional
    "Mu_IPCHI2_MAX" : 9, #adimensional
    
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
    
    "Lambda_PTMIN" : 200, #MeV
    "Lambda_MASS_WINDOW" : 50, #MeV
    "Lambda_FD_MIN" : 10.0, #mm
    "Lambda_FDCHI2_MIN" : 100, #adimensional
    "Lambda_VCHI2NDOF_MAX" : 4,#adimensional
    "Lambda_DIRA_MIN" : 0.999, #adimensional
    
    "Phi_MASS_MAX" : 1100, # MeV
    "Phi_PT_MIN" : 400, # MeV
    "Phi_DOCACHI2_MAX" : 20, #adimensional
    "Phi_VCHI2NDOF_MAX" : 6, #adimensional
    "Phi_IPCHI2_MAX" : 9, #adimensional

    "D0_MASS_WIN" : 80, # MeV
    "D0_PT_MIN" : 400, # MeV
    "D0_DOCACHI2_MAX" : 20, #adimensional
    "D0_VCHI2NDOF_MAX" : 6, #adimensional
    "D0_IPCHI2_MAX" : 9, #adimensional

    "JPsi_MASS_WIN" : 120, # MeV
    "JPsi_PT_MIN" : 500, # MeV
    "JPsi_DOCACHI2_MAX" : 20, #adimensional
    "JPsi_VCHI2NDOF_MAX" : 6, #adimensional
    "JPsi_IPCHI2_MAX" : 9, #adimensional
    
    "etaC_MASS_WIN" : 200, # MeV
    "etaC_PT_MIN" : 400, # MeV
    "etaC_DOCACHI2_MAX" : 20, #adimensional
    "etaC_VCHI2NDOF_MAX" : 6, #adimensional
    "etaC_IPCHI2_MAX" : 9 #adimensional
    }   

default_config = {
    'NAME'              : 'PhiToKSKS',
    'WGs'               : ['RD'],
    'BUILDERTYPE'       : 'PhiToKSKSAllLinesConf',
    'CONFIG'    : confdict,
    'STREAMS' : ['Charm'] }

class PhiToKSKSAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "prescale_PhiToKK" ,
        "prescale_PhiToKsKs" ,
        "prescale_PhiToMuMu" ,
        "prescale_JPsiToKK" ,
        "prescale_JPsiToKsKs" ,
        "prescale_D0ToKsKs" ,
        "prescale_EtaCToLL" ,
        "GEC_nLongTrk",
        "DoDTF",
        "GHOSTPROB_MAX",
        "K_PTMIN",
        "K_PIDK_MIN",
        "K_IPCHI2_MAX",
        "Mu_PTMIN",
        "Mu_PIDmu_MIN",
        "Mu_IPCHI2_MAX",
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
        "Lambda_PTMIN",
        "Lambda_MASS_WINDOW",
        "Lambda_FD_MIN",
        "Lambda_FDCHI2_MIN",
        "Lambda_VCHI2NDOF_MAX",
        "Lambda_DIRA_MIN",
        "Phi_MASS_MAX",
        "Phi_PT_MIN",
        "Phi_DOCACHI2_MAX",
        "Phi_VCHI2NDOF_MAX",
        "Phi_IPCHI2_MAX",
        "D0_MASS_WIN",
        "D0_PT_MIN",
        "D0_DOCACHI2_MAX",
        "D0_VCHI2NDOF_MAX",
        "D0_IPCHI2_MAX",
        "JPsi_MASS_WIN",
        "JPsi_PT_MIN",
        "JPsi_DOCACHI2_MAX",
        "JPsi_VCHI2NDOF_MAX",
        "JPsi_IPCHI2_MAX",
        "etaC_MASS_WIN",
        "etaC_PT_MIN",
        "etaC_DOCACHI2_MAX",
        "etaC_VCHI2NDOF_MAX",
        "etaC_IPCHI2_MAX"
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
        
        self.MuonCuts = " (PT> %(Mu_PTMIN)s *MeV)" \
            " & (PIDmu > %(Mu_PIDmu_MIN)s)"\
            " & (MIPCHI2DV(PRIMARY) < %(Mu_IPCHI2_MAX)s)"\
            " & (TRGHOSTPROB < %(GHOSTPROB_MAX)s)" %config
        
        self.LambdaCuts = " (PT> %(Lambda_PTMIN)s *MeV)" \
            " & (ADMASS('Lambda0') < %(Lambda_MASS_WINDOW)s *MeV)"\
            " & (BPVVD > %(Lambda_FD_MIN)s *mm)" \
            " & (BPVVDCHI2 > %(Lambda_FDCHI2_MIN)s)" \
            " & CHILDCUT((TRGHOSTPROB < %(GHOSTPROB_MAX)s),1)" \
            " & CHILDCUT((TRGHOSTPROB < %(GHOSTPROB_MAX)s),2)" \
            " & (VFASPF(VCHI2PDOF) < %(Lambda_VCHI2NDOF_MAX)s)" \
            " & (BPVDIRA > %(Lambda_DIRA_MIN)s)" %config
        
        self.KsLL = Selection( "KsLLFor" + _name,
                               Algorithm = FilterDesktop(name = "KsLLFilterFor"+_name, Code = self.KsLLCuts ),
                               RequiredSelections = [DataOnDemand(Location = 'Phys/StdLooseKsLL/Particles')])
        
        self.KsDD = Selection( "KsDDFor" + _name,
                               Algorithm = FilterDesktop(name = "KsDDFilterFor"+_name, Code = self.KsDDCuts ),
                               RequiredSelections = [DataOnDemand(Location = 'Phys/StdLooseKsDD/Particles')])
        
        self.Ks = MergedSelection("KsFor"+_name, RequiredSelections = [ self.KsLL, self.KsDD] )
        
        self.Kaons = Selection( "KaonsFor" + _name,
                                Algorithm = FilterDesktop(name = "KaonFilterFor"+_name, Code = self.KaonCuts ),
                                RequiredSelections = [StdAllLooseKaons])
        
        self.Muons = Selection( "MuonsFor" + _name,
                                Algorithm = FilterDesktop(name = "MuonFilterFor"+_name, Code = self.MuonCuts ),
                                RequiredSelections = [StdAllLooseMuons])
        
        self.LambdaDD = Selection( "LambdaDDFor" + _name,
                                Algorithm = FilterDesktop(name = "LambdaDDFilterFor"+_name, Code = self.LambdaCuts ),
                                RequiredSelections = [DataOnDemand(Location = 'Phys/StdLooseLambdaDD/Particles')])
	
        self.LambdaLL = Selection( "LambdaLLFor" + _name,
                                Algorithm = FilterDesktop(name = "LambdaLLFilterFor"+_name, Code = self.LambdaCuts ),
                                RequiredSelections = [DataOnDemand(Location = 'Phys/StdLooseLambdaLL/Particles')])
        
        self.Lambdas = MergedSelection("LambdasFor"+_name, RequiredSelections = [ self.LambdaLL, self.LambdaDD] )
        
        ####### phi(1020) #############

        self.PhiToKK_Line = VMaker(_name+"_PhiToKK",[self.Kaons],"phi(1020) -> K+ K-",GECs,config,config["prescale_PhiToKK"])
        self.registerLine(self.PhiToKK_Line)        
        
        self.PhiToMuMu_Line = VMaker(_name+"_PhiToMuMu",[self.Muons],"phi(1020) -> mu+ mu-",GECs,config,config["prescale_PhiToMuMu"])
        self.registerLine(self.PhiToMuMu_Line)        
        
        self.PhiToKsKs_Line = VMaker(_name+"_PhiToKsKs",[self.Ks],"phi(1020) -> KS0 KS0",GECs,config,config["prescale_PhiToKsKs"])
        self.registerLine(self.PhiToKsKs_Line)        
        
        ########### J/psi(1S)  ################
        
        self.JPsiToKK_Line = VMaker(_name+"_JPsiToKK",[self.Kaons],"J/psi(1S) -> K+ K-",GECs,config,config["prescale_JPsiToKK"])
        self.registerLine(self.JPsiToKK_Line)        
        
        self.JPsiToKsKs_Line = VMaker(_name+"_JPsiToKsKs",[self.Ks],"J/psi(1S) -> KS0 KS0",GECs,config,config["prescale_JPsiToKsKs"])
        self.registerLine(self.JPsiToKsKs_Line)        
        
	########### D0  ################
        
        self.D0ToKsKs_Line = VMaker(_name+"_D0ToKsKs",[self.Ks],"D0 -> KS0 KS0",GECs,config,config["prescale_D0ToKsKs"])
        self.registerLine(self.D0ToKsKs_Line)        
        
	########### eta_c  ################
        
        self.EtaCToLL_Line = VMaker(_name+"_EtaCToLL",[self.Lambdas],"eta_c(1S) -> Lambda0 Lambda~0",GECs,config,config["prescale_EtaCToLL"])
        self.registerLine(self.EtaCToLL_Line)        

def VMaker(_name,_RequiredSelections,_DecayDescriptor,_Filter,conf,_prescale):
    
    _CombiCut = ""
    if 'KS0' in _DecayDescriptor:
        _CombiCut = "( (ACHILDCUT(CHILDCUT(ISLONG,1),1)) | (ACHILDCUT(CHILDCUT(ISLONG,1),2)) ) & "
    if 'phi(1020)' in _DecayDescriptor:
        _CombiCut += " (APT > %(Phi_PT_MIN)s *MeV) & (AM < %(Phi_MASS_MAX)s + 30*MeV) & (ACUTDOCACHI2(%(Phi_DOCACHI2_MAX)s,''))" %conf
        _MotherCut = "(M < %(Phi_MASS_MAX)s +20*MeV) & (VFASPF(VCHI2/VDOF) < %(Phi_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(Phi_IPCHI2_MAX)s)" %conf
        _MassFilter = FilterDesktop(name = "MassFilter_"+_name,Code = "(M < %(Phi_MASS_MAX)s *MeV)" %conf)
        _MassFilterDTF = FilterDesktop(name = "MassFilterDTF_"+_name,Code = "(DTF_FUN(M,True,'phi(1020)') < %(Phi_MASS_MAX)s *MeV)" %conf)
    elif 'J/psi(1S)' in _DecayDescriptor:
        _CombiCut += " (ADAMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s +30*MeV) & (APT > %(JPsi_PT_MIN)s*MeV)"\
            "& (ACUTDOCACHI2(%(JPsi_DOCACHI2_MAX)s,''))" %conf
        _MotherCut = "(DMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s +20*MeV)"\
            "& (VFASPF(VCHI2/VDOF) < %(JPsi_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(JPsi_IPCHI2_MAX)s)" %conf
        _MassFilter = FilterDesktop(name = "MassFilter_"+_name,Code = "(DMASS('J/psi(1S)') < %(JPsi_MASS_WIN)s *MeV)" %conf)
        _MassFilterDTF = FilterDesktop(name = "MassFilterDTF_"+_name,Code = "(abs(DTF_FUN(M,True,'J/psi(1S)') - 3097) < %(JPsi_MASS_WIN)s *MeV)" %conf)
    elif 'D0' in _DecayDescriptor:
        _CombiCut += " (APT > %(D0_PT_MIN)s *MeV) & (ADAMASS('D0') < %(D0_MASS_WIN)s + 30*MeV) & (ACUTDOCACHI2(%(D0_DOCACHI2_MAX)s,''))" %conf
        _MotherCut = "(DMASS('D0') < %(D0_MASS_WIN)s + 20*MeV) & (VFASPF(VCHI2/VDOF) < %(D0_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(D0_IPCHI2_MAX)s)" %conf
        _MassFilter = FilterDesktop(name = "MassFilter_"+_name,Code = "(DMASS('D0') < %(D0_MASS_WIN)s *MeV)" %conf)
        _MassFilterDTF = FilterDesktop(name = "MassFilterDTF_"+_name,Code = "(abs(DTF_FUN(M,True,'D0') - 1865) < %(D0_MASS_WIN)s *MeV)" %conf)
    elif 'eta_c(1S)' in _DecayDescriptor:
        _CombiCut += " (APT > %(etaC_PT_MIN)s *MeV) & (ADAMASS('eta_c(1S)') < %(etaC_MASS_WIN)s + 30*MeV) & (ACUTDOCACHI2(%(etaC_DOCACHI2_MAX)s,''))" %conf
        _MotherCut = "(DMASS('eta_c(1S)') < %(etaC_MASS_WIN)s + 20*MeV) & (VFASPF(VCHI2/VDOF) < %(etaC_VCHI2NDOF_MAX)s) & (MIPCHI2DV(PRIMARY) < %(etaC_IPCHI2_MAX)s)" %conf
        _MassFilter = FilterDesktop(name = "MassFilter_"+_name,Code = "(DMASS('eta_c(1S)') < %(etaC_MASS_WIN)s *MeV)" %conf)
        _MassFilterDTF = FilterDesktop(name = "MassFilterDTF_"+_name,Code = "(abs(DTF_FUN(M,True,'eta_c(1S)') - 2978) < %(etaC_MASS_WIN)s *MeV)" %conf)
    
        
    Comb = CombineParticles( name = "Comb_"+_name,
                             DecayDescriptor = _DecayDescriptor,
                             CombinationCut = _CombiCut,
                             MotherCut = _MotherCut)

    Sel = Selection(name = "Sel_"+_name,
                    Algorithm = Comb,
                    RequiredSelections = _RequiredSelections)
        
    if conf['DoDTF'] == True:
        MASS_FILTER = Selection("MASS_FILTER_"+_name,Algorithm = _MassFilterDTF,RequiredSelections = [Sel])
    else:
        MASS_FILTER = Selection("MASS_FILTER_"+_name,Algorithm = _MassFilter,RequiredSelections = [Sel])
        
    Line = StrippingLine(_name+'Line',
                         prescale = _prescale,
                         FILTER=_Filter,
                         selection = MASS_FILTER)
        
    return Line
