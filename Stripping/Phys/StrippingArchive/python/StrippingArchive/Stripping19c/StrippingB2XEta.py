"""
Module for construction of Bd->KS eta' and Lb ->Lambda eta' stripping Selections and StrippingLines.
Will be extended in future to include same decays to eta
Provides functions to build KS->DD, KS->LL, lambda->LL, lambda->DD, eta', rho, Bd, Lambda_b0  selections.
Provides class B2XEtaConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
"""

__author__ = ['Jimmy McCarthy']
__date__ = '08/06/2012'
__version__ = 'Stripping19'
__all__ = 'B2XEtaConf'

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as Units
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence, MergedSelection
from Configurables import FilterDesktop, CombineParticles
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdLoosePions as Pions
from StandardParticles import StdLooseAllPhotons as Photons
from StandardParticles import StdLooseResolvedEta as Eta

_KsLL=DataOnDemand("Phys/StdLooseKsLL/Particles")

_KsDD=DataOnDemand("Phys/StdLooseKsDD/Particles")
_lambdaLL=DataOnDemand(Location='Phys/StdLooseLambdaLL/Particles')
_lambdaDD=DataOnDemand("Phys/StdLooseLambdaDD/Particles")

default_config = {'Trk_Chi2'                : 3.0,
                  'Trk_PT'                  : 600.0,
                  'Trk_IPChi2'              : 30.0,
                  'KS_DD_MassWindow'        : 25.0,
                  'KS_DD_VtxChi2'           : 10.0,
                  'KS_DD_FDChi2'            : 400.0,
                  'KS_DD_PTmin'             : 2500.0,
                  'KS_DD_Dira'              : 0.9998,            
                  'KS_LL_MassWindow'        : 15.0,
                  'KS_LL_VtxChi2'           : 10.0,
                  'KS_LL_FDChi2'            : 20.0,
                  'KS_LL_Dira'              : 0.9998,
                  'KS_LL_PTmin'             : 1500.0,
                  'L_DD_MassWindow'         : 20.0,
                  'L_DD_VtxChi2'            : 10.0,
                  'L_DD_FDChi2'             : 100.0,
                  'L_DD_PTmin'              : 2500.0,
                  'L_DD_Dira'               : 0.9998,
                  'L_LL_MassWindow'         : 15.0,
                  'L_LL_VtxChi2'            : 10.0,
                  'L_LL_FDChi2'             : 15.0,
                  'L_LL_Dira'               : 0.9998,
                  'L_LL_PTmin'              : 1500.0,  
                  'rho_MassWindow'          : 200.0,
                  'rho_PT'                  : 600.0,
                  'rho_DOCA'                : 10.0,
                  'rho_deltaZ'              : 0.0,
                  'rho_IPChi2'              : 50.0,
                  'rho_vtxChi2'             : 10.0,
                  'eta_prime_MassWindow'    : 45.0,
                  'eta_prime_PT'            : 1500.0,
                  'eta_prime_vtxChi2'       : 10.0,
                  'eta_prime_DOCA'          : 8.0,
                  'B_MassWindow'            : 500.0,
                  'B_PTmin'                 : 600.0,
                  'BDaug_DD_maxDocaChi2'    : 20.0,
                  'BDaug_LL_maxDocaChi2'    : 20.0,
                  'B_VtxChi2'               : 12.0,
                  'B_Dira'                  : 0.999,
                  'B_DD_IPCHI2wrtPV'        : 20.0,
                  'B_LL_IPCHI2wrtPV'        : 20.0,
                  'B_DD_FDChi2'             : 20.0,
                  'B_LL_FDChi2'             : 20.0,
                  'Lb_MassWindow'           : 500.0,
                  'Lb_Pmin'                 : 5000.0,
                  'LbDaug_DD_maxDocaChi2'   : 15.0,
                  'LbDaug_LL_maxDocaChi2'   : 15.0,
                  'Lb_VtxChi2'              : 12.0,
                  'Lb_Dira'                 : 0.999,
                  'Lb_DD_IPCHI2wrtPV'       : 20.0,
                  'Lb_LL_IPCHI2wrtPV'       : 20.0,
                  'GEC_MaxTracks'           : 250,
                  'Prescale'                : 1.0,
                  'Postscale'               : 1.0
                  }


class B2XEtaConf(LineBuilder) :
    """
    Builder of B->XEta stripping Selection and StrippingLine.
    Constructs B -> KS eta' and Lb -> Lambda eta' Selections and StrippingLines from a configuration dictionary.
    
    Exports as instance data members:
    selKS2DD               : KS -> Down Down Selection object
    selKS2LL               : KS -> Long Long Selection object
    selL2LL                : Lambda -> Long Long Selection Object
    selL2DD                : Lambda -> Down Down Selection Object
    selRho                 : rho -> pi+ pi- Selection Object
    selEtap                : eta' Selection Object
    selB2KSDDEtap          : B -> KS(DD) eta' Selection object
    selB2KSLLEtap          : B -> KS(LL) eta' Selection object
    selLb2LDDEtap          : Lb -> Lambda(DD) eta' Selection Object
    selLb2LLLEtap          : Lb -> Lambda(LL) eta' Selection Object
    B2etap_DD_line         : StrippingLine made from selB2KSDDhh
    B2etap_LL_line         : StrippingLine made from selB2KSLLhh
    Lb2etap_DD_line        : StrippingLine made from selLb2LLDDEtap
    Lb2etap_LL_line        : StrippingLine made from selLb2DDEtap
    lines                  : List of lines, [dd_line, ll_line]

    Exports as class data member:
    B2KShhConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Trk_Chi2',
                              'Trk_PT',
                              'Trk_IPChi2',
                              'KS_DD_MassWindow',
                              'KS_DD_VtxChi2',   
                              'KS_DD_FDChi2',    
                              'KS_DD_PTmin',     
                              'KS_DD_Dira',      
                              'KS_LL_MassWindow',
                              'KS_LL_VtxChi2',   
                              'KS_LL_FDChi2',    
                              'KS_LL_Dira',
                              'KS_LL_PTmin',
                              'L_DD_MassWindow', 
                              'L_DD_VtxChi2',    
                              'L_DD_FDChi2',     
                              'L_DD_PTmin',      
                              'L_DD_Dira',       
                              'L_LL_MassWindow', 
                              'L_LL_VtxChi2',    
                              'L_LL_FDChi2',     
                              'L_LL_Dira',
                              'L_LL_PTmin',
                              'rho_MassWindow',
                              'rho_DOCA',
                              'rho_PT',           
                              'rho_deltaZ',      
                              'rho_IPChi2',
                              'rho_vtxChi2',
                              'eta_prime_MassWindow',
                              'eta_prime_PT',
                              'eta_prime_vtxChi2',
                              'eta_prime_DOCA',
                              'B_MassWindow',              
                              'B_PTmin',             
                              'BDaug_DD_maxDocaChi2',
                              'BDaug_LL_maxDocaChi2',
                              'B_VtxChi2',           
                              'B_Dira',              
                              'B_DD_IPCHI2wrtPV',    
                              'B_LL_IPCHI2wrtPV',   
                              'B_DD_FDChi2',         
                              'B_LL_FDChi2',         
                              'Lb_MassWindow',             
                              'Lb_Pmin',             
                              'LbDaug_DD_maxDocaChi2',
                              'LbDaug_LL_maxDocaChi2',
                              'Lb_VtxChi2',           
                              'Lb_Dira',              
                              'Lb_DD_IPCHI2wrtPV',    
                              'Lb_LL_IPCHI2wrtPV',    
                              'GEC_MaxTracks',        
                              'Prescale',             
                              'Postscale'
                              )

    def __init__(self, name, config):
        
        LineBuilder.__init__(self,name,config)

        B2etap_LL_name = name+'B2etapKSLL'
        B2etap_DD_name = name+'B2etapKSDD'
        Lb2etap_LL_name = name+'Lb2etapLLL'
        Lb2etap_DD_name = name+'Lb2etapLDD'

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions = Pions
        self.photons = Photons
        self.eta = Eta

        self.daughters = MergedSelection("DaughtersFor" + name, RequiredSelections = [ self.pions, self.photons, self.eta ] )

        self.makeKS2LL( 'KSfor'+B2etap_LL_name, config )
        self.makeKS2DD( 'KSfor'+B2etap_DD_name, config )        
        self.makeL2LL( 'Lambdafor'+Lb2etap_LL_name,config)
        self.makeL2DD('Lambdafor'+Lb2etap_DD_name,config)

        self.makeRho('Rhofor'+name,config)
        self.makeEtap('Etapfor'+name,config)

        self.makeB2KSLLetap(B2etap_LL_name, config)
        self.makeB2KSDDetap(B2etap_DD_name,config)
        self.makeLb2LLLetap(Lb2etap_LL_name,config)
        self.makeLb2LDDetap(Lb2etap_DD_name,config)

        self.B2etap_LL_line = StrippingLine(B2etap_LL_name+'Line',
                                            prescale = config['Prescale'],
                                            postscale = config['Postscale'],
                                            selection = self.selB2KSLLetap,
                                            FILTER = GECCode
                                            )
        self.B2etap_DD_line = StrippingLine(B2etap_DD_name+'Line',
                                            prescale = config['Prescale'],
                                            postscale = config['Postscale'],
                                            selection = self.selB2KSDDetap,
                                            FILTER = GECCode
                                            )
        self.Lb2etap_LL_line = StrippingLine(Lb2etap_LL_name+'Line',
                                            prescale = config['Prescale'],
                                            postscale = config['Postscale'],
                                            selection = self.selLb2LLLetap,
                                            FILTER = GECCode
                                            )
        self.Lb2etap_DD_line = StrippingLine(Lb2etap_DD_name+'Line',
                                            prescale = config['Prescale'],
                                            postscale = config['Postscale'],
                                            selection = self.selLb2LDDetap,
                                            FILTER = GECCode
                                            )

        self.registerLine(self.B2etap_LL_line)
        self.registerLine(self.B2etap_DD_line)
        self.registerLine(self.Lb2etap_LL_line)
        self.registerLine(self.Lb2etap_DD_line)

    def makeKS2LL( self, name, config):

        _massCut = "(ADMASS('KS0')<%s*MeV)"            % config['KS_LL_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2/VDOF)<%s)"           % config['KS_LL_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"                    % config['KS_LL_FDChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['KS_LL_Dira']
        _momCut = "(PT>%s*MeV)"                        % config['KS_LL_PTmin']
        _trackChi2_1 = "(CHILDCUT((TRCHI2DOF<%s),1))"  % config['Trk_Chi2']
        _trackChi2_2 = "(CHILDCUT((TRCHI2DOF<%s),2))"  % config['Trk_Chi2']
                
        _allCuts =_momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_diraCut+'&'+_trackChi2_1+'&'+_trackChi2_2

        _KSLLFilter=FilterDesktop("_KSLLFilter", Code=_allCuts)
        self.selKS2LL=Selection( name, Algorithm=_KSLLFilter, RequiredSelections=[_KsLL])

    def makeKS2DD( self, name, config):
        
        _massCut = "(ADMASS('KS0')<%s*MeV)"            % config['KS_DD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2/VDOF)<%s)"           % config['KS_DD_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"                    % config['KS_DD_FDChi2']
        _momCut  = "(PT>%s*MeV)"                       % config['KS_DD_PTmin']
        _diraCut = "(BPVDIRA>%s)"                      % config['KS_DD_Dira']
        
        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_diraCut

        _KSDDFilter=FilterDesktop("_KSDDFilter", Code=_allCuts)
        self.selKS2DD=Selection( name, Algorithm=_KSDDFilter, RequiredSelections=[_KsDD])

    def makeL2LL( self, name, config):
    
        _massCut = "(ADMASS('Lambda0')<%s*MeV)"        % config['L_LL_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2/VDOF)<%s)"           % config['L_LL_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"                    % config['L_LL_FDChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['L_LL_Dira']
        _momCut = "(PT>%s*MeV)"                        % config['L_LL_PTmin']
        _trackChi2_1 = "(CHILDCUT((TRCHI2DOF<%s),1))"  % config['Trk_Chi2']
        _trackChi2_2 = "(CHILDCUT((TRCHI2DOF<%s),2))"  % config['Trk_Chi2']      
        
        _allCuts =_momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_diraCut+'&'+_trackChi2_1+'&'+_trackChi2_2      
        
        _LLLFilter=FilterDesktop("_LLLFilter", Code=_allCuts)
        self.selL2LL=Selection( name, Algorithm=_LLLFilter, RequiredSelections=[_lambdaLL])
        
    def makeL2DD( self, name, config):
        
        _massCut = "(ADMASS('Lambda0')<%s*MeV)"        % config['L_DD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2/VDOF)<%s)"           % config['L_DD_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"                    % config['L_DD_FDChi2']
        _momCut  = "(PT>%s*MeV)"                       % config['L_DD_PTmin']
        _diraCut = "(BPVDIRA>%s)"                      % config['L_DD_Dira']
        
        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_diraCut         
        
        _LDDFilter=FilterDesktop("_LDDFilter", Code=_allCuts)
        self.selL2DD=Selection( name, Algorithm=_LDDFilter, RequiredSelections=[_lambdaDD])
        
    def makeRho( self, name, config):

        _massCut="(ADAMASS('rho(770)0')<%s*MeV)"       % config['rho_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['rho_PT']
        _deltaZCut = "(BPVVDZ>%s)"                     % config['rho_deltaZ']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['rho_DOCA']
        _IPCut = "(BPVVDCHI2>%s)"                      % config['rho_IPChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['rho_vtxChi2']
        _trackChi2_1 = "(CHILDCUT((TRCHI2DOF<%s),1))"  % config['Trk_Chi2']
        _trackChi2_2 = "(CHILDCUT((TRCHI2DOF<%s),2))"  % config['Trk_Chi2']
        _track_PT_1 = "(CHILDCUT((PT>%s*MeV),1))"      % config['Trk_PT']
        _track_PT_2 = "(CHILDCUT((PT>%s*MeV),2))"      % config['Trk_PT']
        _track_IPCut_1 = "(CHILDCUT((BPVIPCHI2()>%s),1))" % config['Trk_IPChi2']
        _track_IPCut_2 = "(CHILDCUT((BPVIPCHI2()>%s),2))" % config['Trk_IPChi2']   
        
        _allCuts=_PTCut+'&'+_deltaZCut+'&'+_IPCut+'&'+_trackChi2_1+'&'+_trackChi2_2+'&'+_track_PT_1+'&'+_track_PT_2+'&'+_track_IPCut_1+'&'+_track_IPCut_2
        _combCuts=_massCut+'&'+_docaCut
        
        _rho2pipi=CombineParticles("rho2pipi",
                                   DecayDescriptor="[rho(770)0 -> pi+ pi-]cc",
                                   CombinationCut=_combCuts,
                                   MotherCut=_allCuts)
        self.selRho=Selection( name, Algorithm=_rho2pipi, RequiredSelections=[self.pions])

    def makeEtap( self, name, config):
      
        _massCut = "(ADAMASS('eta_prime')<%s*MeV)"     % config['eta_prime_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['eta_prime_PT']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['eta_prime_vtxChi2']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['eta_prime_DOCA']
        _track_PT="(PT>%s*MeV)"                        % config['Trk_PT']
        _track_Chi2="(TRCHI2DOF<%s)"                   % config['Trk_Chi2']
        _track_IPCut = "(BPVIPCHI2()>%s)"              % config['Trk_IPChi2']
     

        _allCuts = _PTCut+'&'+_vtxCut
        _trackCuts = _track_PT+'&'+_track_Chi2+'&'+_track_IPCut
        _combCuts=_massCut+'&'+_docaCut

        _etap=CombineParticles("etap")
        _etap.DecayDescriptors = ["[eta_prime -> rho(770)0 gamma]cc", "[eta_prime -> eta pi+ pi-]cc"]
        _etap.CombinationCut=_combCuts
        _etap.MotherCut=_allCuts
        _etap.DaughtersCuts = { "pi+" : _trackCuts, "pi-" : _trackCuts}

        self.selEtap = Selection(name, Algorithm=_etap, RequiredSelections=[self.selRho, self.daughters])
        
    def makeB2KSLLetap( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['BDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_LL_IPCHI2wrtPV']
        _fdChi2Cut = "(BPVVDCHI2>%s)"                  % config['B_LL_FDChi2']

        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut#+'&'+_fdChi2Cut
                
        _b2etapksLL = CombineParticles("B2etapKSLL",
                               DecayDescriptor = "[B0 -> KS0 eta_prime]cc",
                               CombinationCut = _combCuts,
                               MotherCut = _allCuts)
        self.selB2KSLLetap = Selection( name, Algorithm=_b2etapksLL, RequiredSelections=[self.selKS2LL, self.selEtap])

    def makeB2KSDDetap( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['BDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_DD_IPCHI2wrtPV']
        _fdChi2Cut = "(BPVVDCHI2>%s)"                  % config['B_DD_FDChi2']

#fdChi2 cut removed to improve signal efficiency. May be put back in for next round of stripping
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut#+'&'+_fdChi2Cut
                
        _b2etapksDD = CombineParticles("B2etapKSDD",
                               DecayDescriptor = "[B0 -> KS0 eta_prime]cc",
                               CombinationCut = _combCuts,
                               MotherCut = _allCuts)
        self.selB2KSDDetap = Selection( name, Algorithm=_b2etapksDD, RequiredSelections=[self.selKS2DD, self.selEtap])
        
    def makeLb2LLLetap( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PCut = "(P>%s*MeV)"                           % config['Lb_Pmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['LbDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_LL_IPCHI2wrtPV']

        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
                
        _Lb2etapLLL = CombineParticles("Lb2etapLLL",
                               DecayDescriptor = "[Lambda_b0 -> Lambda0 eta_prime]cc",
                               CombinationCut = _combCuts,
                               MotherCut = _allCuts)
        self.selLb2LLLetap = Selection( name, Algorithm=_Lb2etapLLL, RequiredSelections=[self.selL2LL, self.selEtap])

    def makeLb2LDDetap( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PCut = "(P>%s*MeV)"                           % config['Lb_Pmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['LbDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_DD_IPCHI2wrtPV']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
                
        _Lb2etapLDD = CombineParticles("Lb2etapLDD",
                               DecayDescriptor = "[Lambda_b0 -> Lambda0 eta_prime]cc",
                               CombinationCut = _combCuts,
                               MotherCut = _allCuts)
        self.selLb2LDDetap = Selection( name, Algorithm=_Lb2etapLDD, RequiredSelections=[self.selL2DD, self.selEtap])




