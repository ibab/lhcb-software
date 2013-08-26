"""
Module for construction of Bd->KS eta' and Lb ->Lambda eta' stripping Selections and StrippingLines.
Will be extended in future to include same decays to eta
Provides functions to build KS->DD, KS->LL, lambda->LL, lambda->DD, eta', eta, Bd, Lambda_b0  selections.
Provides class B2XEtaConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
"""

__author__ = ['Jimmy McCarthy']
__date__ = '02/08/2013'
__version__ = 'Stripping20'
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
from StandardParticles import StdLooseResolvedPi0 as PiZero

_KsLL=DataOnDemand("Phys/StdLooseKsLL/Particles")
_KsDD=DataOnDemand("Phys/StdLooseKsDD/Particles")
_lambdaLL=DataOnDemand(Location='Phys/StdLooseLambdaLL/Particles')
_lambdaDD=DataOnDemand("Phys/StdLooseLambdaDD/Particles")

default_config = {'Trk_Chi2'                : 3.0,
                  'Trk_PT'                  : 300.0,
                  'Trk_GP'                  : 0.5,
                  'KS_DD_MassWindow'        : 23.0,
                  'KS_DD_VtxChi2'           : 15.0, 
                  'KS_DD_FDChi2'            : 20.0,
                  'KS_DD_PTmin'             : 1200.0,
                  'KS_LL_MassWindow'        : 14.0,
                  'KS_LL_VtxChi2'           : 15.0,  
                  'KS_LL_FDChi2'            : 50.0,
                  'KS_LL_PTmin'             : 1200.0,
                  'L_DD_MassWindow'         : 20.0,
                  'L_DD_VtxChi2'            : 15.0,
                  'L_DD_PTmin'              : 1000.0,
                  'L_LL_MassWindow'         : 15.0,
                  'L_LL_VtxChi2'            : 15.0,
                  'L_LL_PTmin'              : 1000.0,  
                  'eta_PT'                  : 2000,
                  'eta_MassWindow'          : 150,
                  'eta_vtxChi2'             : 15,
                  'eta_DOCA'                : 20,
                  'gamma_PT'                : 500, #photons from eta
                  'eta_prime_MassWindow'    : 150.0,
                  'eta_prime_PT'            : 2000.0,
                  'eta_prime_vtxChi2'       : 15.0,
                  'eta_prime_DOCA'          : 20.0,
                  'B_MassWindow'            : 750.0,
                  'B_PTmin'                 : 1500.0,
                  'BDaug_DD_maxDocaChi2'    : 20.0, 
                  'BDaug_LL_maxDocaChi2'    : 20.0, 
                  'B_VtxChi2'               : 15.0,
                  'B_Dira'                  : 0.9995,
                  'B_IPCHI2'                : 20.0,
                  'B_eta_IPCHI2'            : 6.0, 
                  'Lb_MassWindow'           : 750.0,
                  'Lb_PTmin'                : 1000.0,
                  'LbDaug_DD_maxDocaChi2'   : 20.0,
                  'LbDaug_LL_maxDocaChi2'   : 20.0,
                  'Lb_VtxChi2'              : 20.0,
                  'Lb_Dira'                 : 0.9995,
                  'Lb_IPCHI2'               : 20.0,
                  'Lb_eta_IPCHI2'           : 6.0,
                  'GEC_MaxTracks'           : 250,
                  'Prescale'                : 1.0,
                  'Postscale'               : 1.0,
                  'etaGG_Prescale'          : 0.0
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
                              'Trk_GP',
                              'KS_DD_MassWindow',
                              'KS_DD_VtxChi2',   
                              'KS_DD_FDChi2',    
                              'KS_DD_PTmin',     
                              'KS_LL_MassWindow',
                              'KS_LL_VtxChi2',   
                              'KS_LL_FDChi2',    
                              'KS_LL_PTmin',
                              'L_DD_MassWindow', 
                              'L_DD_VtxChi2',    
                              'L_DD_PTmin',      
                              'L_LL_MassWindow', 
                              'L_LL_VtxChi2',    
                              'L_LL_PTmin',
                              'eta_PT',
                              'eta_MassWindow',
                              'eta_vtxChi2',
                              'eta_DOCA',
                              'gamma_PT',
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
                              'B_IPCHI2',    
                              'B_eta_IPCHI2',   
                              'Lb_MassWindow',             
                              'Lb_PTmin',             
                              'LbDaug_DD_maxDocaChi2',
                              'LbDaug_LL_maxDocaChi2',
                              'Lb_VtxChi2',
                              'Lb_Dira',
                              'Lb_IPCHI2',    
                              'Lb_eta_IPCHI2',    
                              'GEC_MaxTracks',        
                              'Prescale',             
                              'Postscale',
                              'etaGG_Prescale'
                              )

    def __init__(self, name, config):
        
        LineBuilder.__init__(self,name,config)

        B2etap_LL_name = name+'B2etapKSLL'
        B2etap_DD_name = name+'B2etapKSDD'
        B2etaGG_LL_name = name+'B2etaGGKSLL'
        B2etaGG_DD_name = name+'B2etaGGKSDD'
        B2eta3Pi_LL_name = name+'B2eta3piKSLL'
        B2eta3Pi_DD_name = name+'B2eta3piKSDD'
        Lb2etap_LL_name = name+'Lb2etapLLL'
        Lb2etap_DD_name = name+'Lb2etapLDD'
        Lb2etaGG_LL_name = name+'Lb2etaGGLLL'
        Lb2etaGG_DD_name = name+'Lb2etaGGLDD'
        Lb2eta3Pi_LL_name = name+'Lb2eta3piLLL'
        Lb2eta3Pi_DD_name = name+'Lb2eta3piLDD'       

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions = Pions
        self.photons = Photons
        self.eta = Eta
        self.pizero = PiZero

        self.daughters = MergedSelection("DaughtersFor" + name, RequiredSelections = [ self.pions, self.photons, self.eta, self.pizero] )

        self.makeKS2LL( 'KSfor'+B2etap_LL_name, config )
        self.makeKS2DD( 'KSfor'+B2etap_DD_name, config )        
        self.makeL2LL( 'Lambdafor'+Lb2etap_LL_name,config)
        self.makeL2DD('Lambdafor'+Lb2etap_DD_name,config)
        self.makeEtaGG('EtaGGfor'+name,config)
        self.makeEta3Pi('Eta3Pifor'+name,config)
        self.makeEtap('Etapfor'+name,config)

        self.makeB2KSLLetap(B2etap_LL_name, config)
        self.makeB2KSDDetap(B2etap_DD_name,config)
        self.makeB2KSLLetaGG(B2etaGG_LL_name, config)
        self.makeB2KSDDetaGG(B2etaGG_DD_name, config)
        self.makeB2KSLLeta3Pi(B2eta3Pi_LL_name, config)
        self.makeB2KSDDeta3Pi(B2eta3Pi_DD_name, config) 
        self.makeLb2LLLetap(Lb2etap_LL_name,config)
        self.makeLb2LDDetap(Lb2etap_DD_name,config)
        self.makeLb2LLLetaGG(Lb2etaGG_LL_name, config)
        self.makeLb2LDDetaGG(Lb2etaGG_DD_name, config)    
        self.makeLb2LLLeta3Pi(Lb2eta3Pi_LL_name, config)
        self.makeLb2LDDeta3Pi(Lb2eta3Pi_DD_name, config)

        self.B2etap_LL_line = StrippingLine(B2etap_LL_name+'Line',
                                            prescale = config['Prescale'],
                                            postscale = config['Postscale'],
                                            selection = self.selB2KSLLetap,
                                            FILTER = GECCode,
                                            EnableFlavourTagging=True
                                            )
        self.B2etap_DD_line = StrippingLine(B2etap_DD_name+'Line',
                                            prescale = config['Prescale'],
                                            postscale = config['Postscale'],
                                            selection = self.selB2KSDDetap,
                                            FILTER = GECCode,
                                            EnableFlavourTagging=True
                                            )
        self.B2etaGG_LL_line = StrippingLine(B2etaGG_LL_name+'Line',
                                             prescale = config['etaGG_Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selB2KSLLetaGG,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=True
                                             )
        self.B2etaGG_DD_line = StrippingLine(B2etaGG_DD_name+'Line',
                                             prescale = config['etaGG_Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selB2KSDDetaGG,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=True
                                             )
        self.B2eta3Pi_LL_line = StrippingLine(B2eta3Pi_LL_name+'Line',
                                              prescale = config['Prescale'],
                                              postscale = config['Postscale'],
                                              selection = self.selB2KSLLeta3Pi,
                                              FILTER = GECCode,
                                              EnableFlavourTagging=True
                                              )
        self.B2eta3Pi_DD_line = StrippingLine(B2eta3Pi_DD_name+'Line',
                                              prescale = config['Prescale'],
                                              postscale = config['Postscale'],
                                              selection = self.selB2KSDDeta3Pi,
                                              FILTER = GECCode,
                                              EnableFlavourTagging=True
                                              )        
        self.Lb2etap_LL_line = StrippingLine(Lb2etap_LL_name+'Line',
                                             prescale = config['Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selLb2LLLetap,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=True
                                             )
        self.Lb2etap_DD_line = StrippingLine(Lb2etap_DD_name+'Line',
                                             prescale = config['Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selLb2LDDetap,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=True
                                             )
        self.Lb2etaGG_LL_line = StrippingLine(Lb2etaGG_LL_name+'Line',
                                              prescale = config['etaGG_Prescale'],
                                              postscale = config['Postscale'],
                                              selection = self.selLb2LLLetaGG,
                                              FILTER = GECCode,
                                              EnableFlavourTagging=True
                                              )
        self.Lb2etaGG_DD_line = StrippingLine(Lb2etaGG_DD_name+'Line',
                                              prescale = config['etaGG_Prescale'],
                                              postscale = config['Postscale'],
                                              selection = self.selLb2LDDetaGG,
                                              FILTER = GECCode,
                                              EnableFlavourTagging=True
                                              )        
        self.Lb2eta3Pi_LL_line = StrippingLine(Lb2eta3Pi_LL_name+'Line',
                                               prescale = config['Prescale'],
                                               postscale = config['Postscale'],
                                               selection = self.selLb2LLLeta3Pi,
                                               FILTER = GECCode,
                                               EnableFlavourTagging=True
                                               )
        self.Lb2eta3Pi_DD_line = StrippingLine(Lb2eta3Pi_DD_name+'Line',
                                               prescale = config['Prescale'],
                                               postscale = config['Postscale'],
                                               selection = self.selLb2LDDeta3Pi,
                                               FILTER = GECCode,
                                               EnableFlavourTagging=True
                                               )
        
        self.registerLine(self.B2etap_LL_line)
        self.registerLine(self.B2etap_DD_line)
        self.registerLine(self.B2etaGG_LL_line)
        self.registerLine(self.B2etaGG_DD_line)
        self.registerLine(self.B2eta3Pi_LL_line)
        self.registerLine(self.B2eta3Pi_DD_line)        
        self.registerLine(self.Lb2etap_LL_line)
        self.registerLine(self.Lb2etap_DD_line)
        self.registerLine(self.Lb2etaGG_LL_line)
        self.registerLine(self.Lb2etaGG_DD_line)
        self.registerLine(self.Lb2eta3Pi_LL_line)
        self.registerLine(self.Lb2eta3Pi_DD_line)       

    def makeKS2LL( self, name, config):

        _massCut = "(ADMASS('KS0')<%s*MeV)"            % config['KS_LL_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2/VDOF)<%s)"           % config['KS_LL_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"                    % config['KS_LL_FDChi2']
        _momCut = "(PT>%s*MeV)"                        % config['KS_LL_PTmin']
        _trackChi2_1 = "(CHILDCUT((TRCHI2DOF<%s),1))"  % config['Trk_Chi2']
        _trackChi2_2 = "(CHILDCUT((TRCHI2DOF<%s),2))"  % config['Trk_Chi2']
        _track_PT_1 = "(CHILDCUT((PT>%s*MeV),1))"      % config['Trk_PT']
        _track_PT_2 = "(CHILDCUT((PT>%s*MeV),2))"      % config['Trk_PT']
        _track_GP_1 = "(CHILDCUT((TRGHOSTPROB<%s),1))"    % config['Trk_GP']
        _track_GP_2 = "(CHILDCUT((TRGHOSTPROB<%s),2))"    % config['Trk_GP'] 
        
        _allCuts =_momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_trackChi2_1+'&'+_trackChi2_2+'&'+_track_PT_1+'&'+_track_PT_2+'&'+_track_GP_1+'&'+_track_GP_2

        _KSLLFilter=FilterDesktop("_KSLLFilter", Code=_allCuts)
        self.selKS2LL=Selection( name, Algorithm=_KSLLFilter, RequiredSelections=[_KsLL])

    def makeKS2DD( self, name, config):
        
        _massCut = "(ADMASS('KS0')<%s*MeV)"            % config['KS_DD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2/VDOF)<%s)"           % config['KS_DD_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"                    % config['KS_DD_FDChi2']
        _momCut  = "(PT>%s*MeV)"                       % config['KS_DD_PTmin']
        _trackChi2_1 = "(CHILDCUT((TRCHI2DOF<%s),1))"  % config['Trk_Chi2']
        _trackChi2_2 = "(CHILDCUT((TRCHI2DOF<%s),2))"  % config['Trk_Chi2']
        _track_PT_1 = "(CHILDCUT((PT>%s*MeV),1))"      % config['Trk_PT']
        _track_PT_2 = "(CHILDCUT((PT>%s*MeV),2))"      % config['Trk_PT']
        _track_GP_1 = "(CHILDCUT((TRGHOSTPROB<%s),1))"    % config['Trk_GP']
        _track_GP_2 = "(CHILDCUT((TRGHOSTPROB<%s),2))"    % config['Trk_GP']
        
        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_trackChi2_1+'&'+_trackChi2_2+'&'+_track_PT_1+'&'+_track_PT_2+'&'+_track_GP_1+'&'+_track_GP_2

        _KSDDFilter=FilterDesktop("_KSDDFilter", Code=_allCuts)
        self.selKS2DD=Selection( name, Algorithm=_KSDDFilter, RequiredSelections=[_KsDD])

    def makeL2LL( self, name, config):
    
        _massCut = "(ADMASS('Lambda0')<%s*MeV)"        % config['L_LL_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2/VDOF)<%s)"           % config['L_LL_VtxChi2']
        _momCut = "(PT>%s*MeV)"                        % config['L_LL_PTmin']
        _trackChi2_1 = "(CHILDCUT((TRCHI2DOF<%s),1))"  % config['Trk_Chi2']
        _trackChi2_2 = "(CHILDCUT((TRCHI2DOF<%s),2))"  % config['Trk_Chi2']
        _track_PT_1 = "(CHILDCUT((PT>%s*MeV),1))"      % config['Trk_PT']
        _track_PT_2 = "(CHILDCUT((PT>%s*MeV),2))"      % config['Trk_PT']
        _track_GP_1 = "(CHILDCUT((TRGHOSTPROB<%s),1))"    % config['Trk_GP']
        _track_GP_2 = "(CHILDCUT((TRGHOSTPROB<%s),2))"    % config['Trk_GP']
        
        _allCuts =_momCut+'&'+_massCut+'&'+_vtxCut+'&'+_trackChi2_1+'&'+_trackChi2_2+'&'+_track_PT_1+'&'+_track_PT_2+'&'+_track_GP_1+'&'+_track_GP_2
        
        _LLLFilter=FilterDesktop("_LLLFilter", Code=_allCuts)
        self.selL2LL=Selection( name, Algorithm=_LLLFilter, RequiredSelections=[_lambdaLL])
        
    def makeL2DD( self, name, config):
        
        _massCut = "(ADMASS('Lambda0')<%s*MeV)"        % config['L_DD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2/VDOF)<%s)"           % config['L_DD_VtxChi2']
        _momCut  = "(PT>%s*MeV)"                       % config['L_DD_PTmin']
        _trackChi2_1 = "(CHILDCUT((TRCHI2DOF<%s),1))"  % config['Trk_Chi2']
        _trackChi2_2 = "(CHILDCUT((TRCHI2DOF<%s),2))"  % config['Trk_Chi2']        
        _track_PT_1 = "(CHILDCUT((PT>%s*MeV),1))"      % config['Trk_PT']
        _track_PT_2 = "(CHILDCUT((PT>%s*MeV),2))"      % config['Trk_PT']
        _track_GP_1 = "(CHILDCUT((TRGHOSTPROB<%s),1))"    % config['Trk_GP']
        _track_GP_2 = "(CHILDCUT((TRGHOSTPROB<%s),2))"    % config['Trk_GP']
        
        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_trackChi2_1+'&'+_trackChi2_2+'&'+_track_PT_1+'&'+_track_PT_2+'&'+_track_GP_1+'&'+_track_GP_2
        
        _LDDFilter=FilterDesktop("_LDDFilter", Code=_allCuts)
        self.selL2DD=Selection( name, Algorithm=_LDDFilter, RequiredSelections=[_lambdaDD])

    def makeEtaGG( self, name, config):
        
        _etaPTCut = "(PT>%s*MeV)"                      % config['eta_PT']
        _gammaPT1 = "(CHILDCUT((PT>%s*MeV),1))"        % config['gamma_PT']
        _gammaPT2 = "(CHILDCUT((PT>%s*MeV),2))"        % config['gamma_PT']
        
        _allCuts = _etaPTCut+'&'+_gammaPT1+'&'+_gammaPT2
        
        _etaFilter = FilterDesktop("_etaFilter", Code=_allCuts)
        self.selEtaGG=Selection(name, Algorithm=_etaFilter, RequiredSelections=[self.eta])
        
    def makeEta3Pi(self,name,config):

        _PTCut = "(PT>%s*MeV)"                         % config['eta_PT']
        _massCut = "(ADAMASS('eta')<%s*MeV)"           % config['eta_MassWindow']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['eta_vtxChi2']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['eta_DOCA']
        _track_PT="(PT>%s*MeV)"                        % config['Trk_PT']
        _track_Chi2="(TRCHI2DOF<%s)"                   % config['Trk_Chi2']
        _track_GPCut ="(TRGHOSTPROB<%s)"               % config['Trk_GP']
        
        _allCuts = _PTCut+'&'+_vtxCut
        _trackCuts = _track_PT+'&'+_track_Chi2+'&'+_track_GPCut
        _combCuts=_massCut+'&'+_docaCut
        
        _eta3Pi=CombineParticles("eta3Pi",
                                 DecayDescriptor = "eta -> pi0 pi+ pi-",
                                 CombinationCut=_combCuts,
                                 MotherCut=_allCuts,
                                 DaughtersCuts = { "pi+" : _trackCuts, "pi-" : _trackCuts},
                                 ParticleCombiners = {'' : "LoKi::VertexFitter" })
        
        self.selEta3Pi = Selection(name, Algorithm=_eta3Pi, RequiredSelections=[self.daughters])
        
    def makeEtap( self, name, config):
      
        _massCut = "(ADAMASS('eta_prime')<%s*MeV)"     % config['eta_prime_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['eta_prime_PT']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['eta_prime_vtxChi2']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['eta_prime_DOCA']
        _track_PT="(PT>%s*MeV)"                        % config['Trk_PT']
        _track_Chi2="(TRCHI2DOF<%s)"                   % config['Trk_Chi2']
        _track_GPCut ="(TRGHOSTPROB<%s)"               % config['Trk_GP']

        _allCuts = _PTCut+'&'+_vtxCut
        _trackCuts = _track_PT+'&'+_track_Chi2+'&'+_track_GPCut
        _combCuts=_massCut+'&'+_docaCut

        _etap=CombineParticles("etap",
                               DecayDescriptors = ["eta_prime -> gamma pi+ pi-", "eta_prime -> eta pi+ pi-"],
                               CombinationCut=_combCuts,
                               MotherCut=_allCuts,
                               DaughtersCuts = { "pi+" : _trackCuts, "pi-" : _trackCuts},
                               ParticleCombiners = {'' : "LoKi::VertexFitter" })

        self.selEtap = Selection(name, Algorithm=_etap, RequiredSelections=[self.daughters])
        
    def makeB2KSLLetap( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['BDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
        
        _b2etapksLL = CombineParticles("B2etapKSLL",
                                       DecayDescriptor = "B0 -> KS0 eta_prime",
                                       CombinationCut = _combCuts,
                                       MotherCut = _allCuts,
                                       ParticleCombiners = {'' : "LoKi::VertexFitter" }     )
        self.selB2KSLLetap = Selection( name, Algorithm=_b2etapksLL, RequiredSelections=[self.selKS2LL, self.selEtap])

    def makeB2KSDDetap( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['BDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
        
        _b2etapksDD = CombineParticles("B2etapKSDD",
                                       DecayDescriptor = "B0 -> KS0 eta_prime",
                                       CombinationCut = _combCuts,
                                       MotherCut = _allCuts,
                                       ParticleCombiners = {'' : "LoKi::VertexFitter" })
        self.selB2KSDDetap = Selection( name, Algorithm=_b2etapksDD, RequiredSelections=[self.selKS2DD, self.selEtap])


    def makeB2KSLLetaGG( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_eta_IPCHI2']
                
        _combCuts = _massCut
        _allCuts = _PTCut+'&'+_IPChi2Cut
        
        _b2etaGGksLL = CombineParticles("B2etaGGKSLL",
                                        DecayDescriptor = "B0 -> KS0 eta",
                                        CombinationCut = _combCuts,
                                        MotherCut = _allCuts,
                                        ParticleCombiners = {'' : "LoKi::VertexFitter" } )
        self.selB2KSLLetaGG = Selection( name, Algorithm=_b2etaGGksLL, RequiredSelections=[self.selKS2LL, self.selEtaGG])
        
    def makeB2KSDDetaGG( self, name, config):
        
        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_eta_IPCHI2']
                
        _combCuts = _massCut
        _allCuts = _PTCut+'&'+_IPChi2Cut
        
        _b2etaGGksDD = CombineParticles("B2etaGGKSDD",
                                        DecayDescriptor = "B0 -> KS0 eta",
                                        CombinationCut = _combCuts,
                                        MotherCut = _allCuts,
                                        ParticleCombiners = {'' : "LoKi::VertexFitter" })
        self.selB2KSDDetaGG = Selection( name, Algorithm=_b2etaGGksDD, RequiredSelections=[self.selKS2DD, self.selEtaGG])
        
    def makeB2KSLLeta3Pi( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['BDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
                
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut
        
        _b2etaksLL = CombineParticles("B2etaKSLL",
                                      DecayDescriptor = "B0 -> KS0 eta",
                                      CombinationCut = _combCuts,
                                      MotherCut = _allCuts,
                                      ParticleCombiners = {'' : "LoKi::VertexFitter" } )
        self.selB2KSLLeta3Pi = Selection( name, Algorithm=_b2etaksLL, RequiredSelections=[self.selKS2LL, self.selEta3Pi])
        
    def makeB2KSDDeta3Pi( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['BDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
                
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut
        
        _b2eta3PiksDD = CombineParticles("B2eta3PiKSDD",
                                         DecayDescriptor = "B0 -> KS0 eta",
                                         CombinationCut = _combCuts,
                                         MotherCut = _allCuts,
                                         ParticleCombiners = {'' : "LoKi::VertexFitter" })
        self.selB2KSDDeta3Pi = Selection( name, Algorithm=_b2eta3PiksDD, RequiredSelections=[self.selKS2DD, self.selEta3Pi])       
        
    
    def makeLb2LLLetap( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['LbDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
                
        _Lb2etapLLL = CombineParticles("Lb2etapLLL",
                                       DecayDescriptor = "[Lambda_b0 -> Lambda0 eta_prime]cc",
                                       CombinationCut = _combCuts,
                                       MotherCut = _allCuts,
                                       ParticleCombiners = {'' : "LoKi::VertexFitter" })
        self.selLb2LLLetap = Selection( name, Algorithm=_Lb2etapLLL, RequiredSelections=[self.selL2LL, self.selEtap])
        
    def makeLb2LDDetap( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['LbDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
        
        _Lb2etapLDD = CombineParticles("Lb2etapLDD",
                                       DecayDescriptor = "[Lambda_b0 -> Lambda0 eta_prime]cc",
                                       CombinationCut = _combCuts,
                                       MotherCut = _allCuts,
                                       ParticleCombiners = {'' : "LoKi::VertexFitter" } )
        self.selLb2LDDetap = Selection( name, Algorithm=_Lb2etapLDD, RequiredSelections=[self.selL2DD, self.selEtap])

    def makeLb2LLLetaGG( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_eta_IPCHI2']
        
        _combCuts = _massCut
        _allCuts = _PTCut+'&'+_IPChi2Cut
        
        _Lb2etaGGLLL = CombineParticles("Lb2etaGGLLL",
                                        DecayDescriptor = "[Lambda_b0 -> Lambda0 eta]cc",
                                        CombinationCut = _combCuts,
                                        MotherCut = _allCuts,
                                        ParticleCombiners = {'' : "LoKi::VertexFitter" } )
        self.selLb2LLLetaGG = Selection( name, Algorithm=_Lb2etaGGLLL, RequiredSelections=[self.selL2LL, self.selEtaGG])
        
    def makeLb2LDDetaGG( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_eta_IPCHI2']
        
        _combCuts = _massCut
        _allCuts = _PTCut+'&'+_IPChi2Cut
        
        _Lb2etaGGLDD = CombineParticles("Lb2etaGGLDD",
                                        DecayDescriptor = "[Lambda_b0 -> Lambda0 eta]cc",
                                        CombinationCut = _combCuts,
                                        MotherCut = _allCuts,
                                        ParticleCombiners = {'' : "LoKi::VertexFitter" })
        self.selLb2LDDetaGG = Selection( name, Algorithm=_Lb2etaGGLDD, RequiredSelections=[self.selL2DD, self.selEtaGG])
        
    def makeLb2LLLeta3Pi( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['LbDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut
        
        _Lb2eta3PiLLL = CombineParticles("Lb2eta3PiLLL",
                                         DecayDescriptor = "[Lambda_b0 -> Lambda0 eta]cc",
                                         CombinationCut = _combCuts,
                                         MotherCut = _allCuts,
                                         ParticleCombiners = {'' : "LoKi::VertexFitter" } )
        self.selLb2LLLeta3Pi = Selection( name, Algorithm=_Lb2eta3PiLLL, RequiredSelections=[self.selL2LL, self.selEta3Pi])
        
    def makeLb2LDDeta3Pi( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCA(%s,''))"                 % config['LbDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut
        
        _Lb2eta3PiLDD = CombineParticles("Lb2eta3PiLDD",
                                         DecayDescriptor = "[Lambda_b0 -> Lambda0 eta]cc",
                                         CombinationCut = _combCuts,
                                         MotherCut = _allCuts,
                                         ParticleCombiners = {'' : "LoKi::VertexFitter" })
        self.selLb2LDDeta3Pi = Selection( name, Algorithm=_Lb2eta3PiLDD, RequiredSelections=[self.selL2DD, self.selEta3Pi])
