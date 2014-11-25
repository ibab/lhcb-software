"""
Module for construction of Bd->KS eta' and Lb ->Lambda eta' stripping Selections and StrippingLines.
Will be extended in future to include same decays to eta
Provides functions to build KS->DD, KS->LL, lambda->LL, lambda->DD, eta', eta, Bd, Lambda_b0  selections.
Provides class B2XEtaConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
"""

__author__ = ['Jimmy McCarthy']
__date__ = '31/08/2014'
__version__ = 'Stripping20r1'
__all__ = 'B2XEtaConf'

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as Units
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence, MergedSelection
from Configurables import FilterDesktop, CombineParticles
from Configurables import DaVinci__N3BodyDecays
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from MVADictHelpers import *
#from Configurables import LoKi__ODINFilter as ODINFilter
#from Configurables import LoKi__Hybrid__DictValue as DictValue
#from Configurables import LoKi__Hybrid__DictTransform_TMVATransform_ as TMVAClassifier
#from Configurables import LoKi__Hybrid__DictOfFunctors

from StandardParticles import StdLoosePions as Pions
from StandardParticles import StdLooseKaons as Kaons
from StandardParticles import StdLooseProtons as Protons
from StandardParticles import StdLooseAllPhotons as Photons
from StandardParticles import StdLooseResolvedEta as Eta
from StandardParticles import StdLooseResolvedPi0 as PiZero

_KsLL=DataOnDemand("Phys/StdLooseKsLL/Particles")
_KsDD=DataOnDemand("Phys/StdLooseKsDD/Particles")
_lambdaLL=DataOnDemand(Location='Phys/StdLooseLambdaLL/Particles')
_lambdaDD=DataOnDemand("Phys/StdLooseLambdaDD/Particles")
_kstar=DataOnDemand("Phys/StdLooseKstar2Kpi/Particles")

default_config = {
    'NAME'         : 'B2XEta',
    'WGs'          : ['Charmless'],
    'BUILDERTYPE'  : 'B2XEtaConf',
    'CONFIG'       : {
                  'Trk_Chi2'                : 3.0,
                  'Trk_PT'                  : 300.0,
                  'Trk_GP'                  : 0.5,
                  'pK_PT'                   : 500., #1000.
                  'pK_IPCHI2'               : 20.,
                  'ProbNNCut'               : 0.1,
                  'kstar_daug_PT'           : 500.,
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
                  'Kstar_PTmin'             : 1200.0,
                  'Kstar_massWdw'           : 100.0,
                  'Kstar_vtxChi2'           : 9.0,
                  'Kstar_ipChi2'            : 5.0,
                  'eta_PT'                  : 2000,
                  'eta_MassWindow'          : 150,
                  'eta_vtxChi2'             : 10.,
                  'eta_DOCA'                : 10.0, #20
                  'gamma_PT'                : 500, #photons from eta
                  'eta_prime_MassWindow'    : 100.0,
                  'eta_prime_PT'            : 2000.0,
                  'eta_prime_vtxChi2'       : 10.0,
                  'eta_prime_DOCA'          : 10.0, #15
                  'B_MassWindow'            : 750.0,
                  'B_PTmin'                 : 1500.0,
                  'BDaug_DD_maxDocaChi2'    : 15.0, #20 
                  'BDaug_LL_maxDocaChi2'    : 15.0,  #20
                  'B_VtxChi2'               : 15.0,
                  'B_Dira'                  : 0.9995,
                  'B_IPCHI2'                : 20.0,
                  'B_eta_IPCHI2'            : 6.0, 
                  'Lb_MassWindow'           : 750.0,
                  'Lb_PTmin'                : 1000.0,
                  'LbDaug_DD_maxDocaChi2'   : 15.0, #20
                  'LbDaug_LL_maxDocaChi2'   : 15.0, #20
                  'Lb_VtxChi2'              : 15.0,
                  'Lb_Dira'                 : 0.9995,
                  'Lb_IPCHI2'               : 20.0,
                  'Lb_eta_IPCHI2'           : 6.0,
                  'GEC_MaxTracks'           : 250,
                  'Prescale'                : 1.0,
                  'Postscale'               : 1.0,
                  'etaGG_Prescale'          : 0.0
                  #'TCKs'                : ('0x00470032','0x00790038')
                  #'TCKs'                : ('0x00470032','0x00790038','0x007E003A','0x0097003D','0x407E003A','0x4097003D','0x00990042','0x00AC0046','0x40990042','0x40AC0046')
                  },
    'STREAMS'     : ['Bhadron']
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
                              'pK_PT',
                              'pK_IPCHI2',
                              'kstar_daug_PT',
                              'ProbNNCut',
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
                              'Kstar_PTmin',
                              'Kstar_massWdw',
                              'Kstar_vtxChi2',
                              'Kstar_ipChi2',
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
                              #'TCKs'
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

        B2etap_Kst_name = name+'B2etapKstar'
        B2etaGG_Kst_name = name+'B2etaGGKstar'
        B2eta3Pi_Kst_name = name+'B2eta3piKstar'
        Lb2etap_pK_name = name+'Lb2pKetap'
        Lb2etaGG_pK_name = name+'Lb2pKetaGG'
        Lb2eta3Pi_pK_name = name+'Lb2pKeta3pi'
 
        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        #TCKFilters  = {'Code' : "(in_range( %s, ODIN_TCK, %s )) | (in_range( %s, ODIN_TCK, %s )) | (in_range( %s, ODIN_TCK, %s )) | (in_range( %s, ODIN_TCK, %s )) | (in_range( %s, ODIN_TCK, %s ))" % config['TCKs'],
        #               'Preambulo' : ["from LoKiCore.functions import *"]}
        #TCKFilters  = {'Code' : "(in_range( %s, ODIN_TCK, %s ))" % config['TCKs'],
        #               'Preambulo' : ["from LoKiCore.functions import *"]}
        
        Hlt1Filter = "(HLT_PASS_RE('Hlt1TrackAllL0Decision'))"
        Hlt2Filter = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))"

        _tagging=True
        self.refitPVs=True
        
        self.pions = Pions
        self.kaons = Kaons
        self.protons = Protons
        self.photons = Photons
        self.eta = Eta
        self.pizero = PiZero

        self.daughters = MergedSelection("DaughtersFor" + name, RequiredSelections = [ self.pions, self.photons, self.eta, self.pizero] )

        self.makeKS2LL( 'KSfor'+B2etap_LL_name, config )
        self.makeKS2DD( 'KSfor'+B2etap_DD_name, config )        
        self.makeL2LL( 'Lambdafor'+Lb2etap_LL_name,config)
        self.makeL2DD('Lambdafor'+Lb2etap_DD_name,config)

        self.makeFilterKaons( 'KaonsFor'+Lb2etap_pK_name, config )
        self.makeFilterProtons( 'ProtonsFor'+Lb2etap_pK_name, config )
        self.makeFilterKstar( 'KstarFor'+B2etap_Kst_name, config )

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

        self.makeB2Kstetap(B2etap_Kst_name, config)
        self.makeB2KstetaGG(B2etaGG_Kst_name, config)
        self.makeB2Ksteta3Pi(B2eta3Pi_Kst_name, config)
        self.makeLb2pKetap(Lb2etap_pK_name, config)
        self.makeLb2pKetaGG(Lb2etaGG_pK_name, config)
        self.makeLb2pKeta3Pi(Lb2eta3Pi_pK_name, config)

        self.B2etap_LL_line = StrippingLine(B2etap_LL_name+'Line',
                                            prescale = config['Prescale'],
                                            postscale = config['Postscale'],
                                            selection = self.selB2KSLLetap,
                                            HLT = Hlt1Filter,
                                            FILTER = GECCode,
                                            EnableFlavourTagging=_tagging
                                            )
        self.B2etap_DD_line = StrippingLine(B2etap_DD_name+'Line',
                                            prescale = config['Prescale'],
                                            postscale = config['Postscale'],
                                            selection = self.selB2KSDDetap,
                                            HLT = Hlt1Filter,
                                            FILTER = GECCode,
                                            EnableFlavourTagging=_tagging
                                            )
        self.B2etaGG_LL_line = StrippingLine(B2etaGG_LL_name+'Line',
                                             prescale = config['etaGG_Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selB2KSLLetaGG,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=_tagging
                                             )
        self.B2etaGG_DD_line = StrippingLine(B2etaGG_DD_name+'Line',
                                             prescale = config['etaGG_Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selB2KSDDetaGG,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=_tagging
                                             )
        self.B2eta3Pi_LL_line = StrippingLine(B2eta3Pi_LL_name+'Line',
                                              prescale = config['Prescale'],
                                              postscale = config['Postscale'],
                                              selection = self.selB2KSLLeta3Pi,
                                              HLT = Hlt1Filter,
                                              FILTER = GECCode,
                                              EnableFlavourTagging=_tagging
                                              )
        self.B2eta3Pi_DD_line = StrippingLine(B2eta3Pi_DD_name+'Line',
                                              prescale = config['Prescale'],
                                              postscale = config['Postscale'],
                                              selection = self.selB2KSDDeta3Pi,
                                              HLT = Hlt1Filter,
                                              FILTER = GECCode,
                                              EnableFlavourTagging=_tagging
                                              )        
        self.Lb2etap_LL_line = StrippingLine(Lb2etap_LL_name+'Line',
                                             prescale = config['Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selLb2LLLetap,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=_tagging
                                             )
        self.Lb2etap_DD_line = StrippingLine(Lb2etap_DD_name+'Line',
                                             prescale = config['Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selLb2LDDetap,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=_tagging
                                             )
        self.Lb2etaGG_LL_line = StrippingLine(Lb2etaGG_LL_name+'Line',
                                              prescale = config['etaGG_Prescale'],
                                              postscale = config['Postscale'],
                                              selection = self.selLb2LLLetaGG,
                                              FILTER = GECCode,
                                              EnableFlavourTagging=_tagging
                                              )
        self.Lb2etaGG_DD_line = StrippingLine(Lb2etaGG_DD_name+'Line',
                                              prescale = config['etaGG_Prescale'],
                                              postscale = config['Postscale'],
                                              selection = self.selLb2LDDetaGG,
                                              FILTER = GECCode,
                                              EnableFlavourTagging=_tagging
                                              )        
        self.Lb2eta3Pi_LL_line = StrippingLine(Lb2eta3Pi_LL_name+'Line',
                                               prescale = config['Prescale'],
                                               postscale = config['Postscale'],
                                               selection = self.selLb2LLLeta3Pi,
                                               FILTER = GECCode,
                                               EnableFlavourTagging=_tagging
                                               )
        self.Lb2eta3Pi_DD_line = StrippingLine(Lb2eta3Pi_DD_name+'Line',
                                               prescale = config['Prescale'],
                                               postscale = config['Postscale'],
                                               selection = self.selLb2LDDeta3Pi,
                                               FILTER = GECCode,
                                               EnableFlavourTagging=_tagging
                                               )

        self.B2etap_Kst_line = StrippingLine(B2etap_Kst_name+'Line',
                                             prescale = config['Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selB2etapKst,
                                             HLT = Hlt2Filter,
                                             FILTER = GECCode,
                                             #ODIN = TCKFilters,
                                             EnableFlavourTagging=_tagging
                                             )
        self.B2etaGG_Kst_line = StrippingLine(B2etaGG_Kst_name+'Line',
                                             prescale = config['etaGG_Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selB2etaGGKst,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=_tagging
                                             )
        self.B2eta3Pi_Kst_line = StrippingLine(B2eta3Pi_Kst_name+'Line',
                                               prescale = config['Prescale'],
                                               postscale = config['Postscale'],
                                               selection = self.selB2eta3PiKst,
                                               HLT = Hlt1Filter,
                                               FILTER = GECCode,
                                               EnableFlavourTagging=_tagging
                                               )
        self.Lb2etap_pK_line = StrippingLine(Lb2etap_pK_name+'Line',
                                             prescale = config['Prescale'],
                                             postscale = config['Postscale'],
                                             selection = self.selLb2pKetap,
                                             HLT = Hlt2Filter,
                                             FILTER = GECCode,
                                             EnableFlavourTagging=_tagging
                                             )
        self.Lb2etaGG_pK_line = StrippingLine(Lb2etaGG_pK_name+'Line',
                                              prescale = config['etaGG_Prescale'],
                                              postscale = config['Postscale'],
                                              selection = self.selLb2pKetaGG,
                                              FILTER = GECCode,
                                              EnableFlavourTagging=_tagging
                                              )
        self.Lb2eta3Pi_pK_line = StrippingLine(Lb2eta3Pi_pK_name+'Line',
                                               prescale = config['Prescale'],
                                               postscale = config['Postscale'],
                                               selection = self.selLb2pKeta3Pi,
                                               HLT = Hlt1Filter,
                                               FILTER = GECCode,
                                               EnableFlavourTagging=_tagging
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

        self.registerLine(self.B2etap_Kst_line)
        self.registerLine(self.B2etaGG_Kst_line)
        self.registerLine(self.B2eta3Pi_Kst_line)
        self.registerLine(self.Lb2etap_pK_line)
        self.registerLine(self.Lb2etaGG_pK_line)
        self.registerLine(self.Lb2eta3Pi_pK_line)

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
        _track_PID_1 = "(CHILDCUT((PROBNNpi>%s),1))"      % config['ProbNNCut']
        _track_PID_2 = "(CHILDCUT((PROBNNpi>%s),2))"      % config['ProbNNCut']
        
        _allCuts =_momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_trackChi2_1+'&'+_trackChi2_2+'&'+_track_PT_1+'&'+_track_PT_2+'&'+_track_GP_1+'&'+_track_GP_2+'&'+_track_PID_1+'&'+_track_PID_2

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
        _track_PID_1 = "(CHILDCUT((PROBNNpi>%s),1))"      % config['ProbNNCut']
        _track_PID_2 = "(CHILDCUT((PROBNNpi>%s),2))"      % config['ProbNNCut']
        
        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_trackChi2_1+'&'+_trackChi2_2+'&'+_track_PT_1+'&'+_track_PT_2+'&'+_track_GP_1+'&'+_track_GP_2+'&'+_track_PID_1+'&'+_track_PID_2

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
        #_track_PID_1 = "(CHILDCUT((PROBNNp>%s),1))"      % config['ProbNNCut']
        #_track_PID_2 = "(CHILDCUT((PROBNNpi>%s),2))"      % config['ProbNNCut']
        
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
        #_track_PID_1 = "(CHILDCUT((PROBNNp>%s),1))"      % config['ProbNNCut']
        #_track_PID_2 = "(CHILDCUT((PROBNNpi>%s),2))"      % config['ProbNNCut']
        
        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_trackChi2_1+'&'+_trackChi2_2+'&'+_track_PT_1+'&'+_track_PT_2+'&'+_track_GP_1+'&'+_track_GP_2
        
        _LDDFilter=FilterDesktop("_LDDFilter", Code=_allCuts)
        self.selL2DD=Selection( name, Algorithm=_LDDFilter, RequiredSelections=[_lambdaDD])

    def makeFilterKaons( self, name, config):

        _momCut = "(PT>%s*MeV)"           % config['pK_PT']
        _GPCut = "(TRGHOSTPROB<%s)"       % config['Trk_GP']
        _PIDCut = "(PROBNNk>%s)"          % config['ProbNNCut']
        _IPChi2_Cut = "(BPVIPCHI2()>%s)"  % config['pK_IPCHI2']

        _allCuts = _momCut+'&'+_GPCut+'&'+_PIDCut+'&'+_IPChi2_Cut

        _kaonFilter=FilterDesktop("_kaonFilter", Code=_allCuts)
        self.selKaons = Selection( name, Algorithm=_kaonFilter, RequiredSelections=[self.kaons] )

    def makeFilterProtons( self, name, config):

        _momCut = "(PT>%s*MeV)"           % config['pK_PT']
        _GPCut = "(TRGHOSTPROB<%s)"       % config['Trk_GP']
        _PIDCut = "(PROBNNp>%s)"          % config['ProbNNCut']
        _IPChi2_Cut = "(BPVIPCHI2()>%s)"  % config['pK_IPCHI2']
        
        _allCuts = _momCut+'&'+_GPCut+'&'+_PIDCut+'&'+_IPChi2_Cut

        _protonFilter=FilterDesktop("_protonFilter", Code=_allCuts)
        self.selProtons = Selection( name, Algorithm=_protonFilter, RequiredSelections=[self.protons] )

    def makeFilterKstar( self, name, config):
        _massCut = "(ADMASS('K*(892)0')<%s*MeV)"                      % config['Kstar_massWdw']
        _momCut = "(PT>%s*MeV)"                                       % config['Kstar_PTmin']
        _vtxCut  = "(VFASPF(VCHI2/VDOF)<%s)"                          % config['Kstar_vtxChi2']
        _IPChi2Cut = "(BPVIPCHI2()>%s)"                               % config['Kstar_ipChi2']
              
        _trackChi2_1 = "(CHILDCUT((TRCHI2DOF<%s),1))"                 % config['Trk_Chi2']
        _trackChi2_2 = "(CHILDCUT((TRCHI2DOF<%s),2))"                 % config['Trk_Chi2']        
        _track_PT_1 = "(CHILDCUT((PT>%s*MeV),1))"                     % config['kstar_daug_PT']
        _track_PT_2 = "(CHILDCUT((PT>%s*MeV),2))"                     % config['kstar_daug_PT']
        _track_GP_1 = "(CHILDCUT((TRGHOSTPROB<%s),1))"                % config['Trk_GP']
        _track_GP_2 = "(CHILDCUT((TRGHOSTPROB<%s),2))"                % config['Trk_GP']
        _track_PID_1 = "(INTREE((ABSID=='pi-') & (PROBNNpi>%s)))"     % config['ProbNNCut']
        _track_PID_2 = "(INTREE((ABSID=='K+') & (PROBNNk>%s)))"       % config['ProbNNCut']
        
        _allCuts = _massCut+'&'+_vtxCut+'&'+_momCut+'&'+_IPChi2Cut+'&'+_trackChi2_1+'&'+_trackChi2_2+'&'+_track_PT_1+'&'+_track_PT_2+'&'+_track_GP_1+'&'+_track_GP_2+'&'+_track_PID_1+'&'+_track_PID_2

        _kstarFilter=FilterDesktop("_kstarFilter", Code=_allCuts)
        self.selKstar = Selection( name, Algorithm=_kstarFilter, RequiredSelections=[_kstar] )

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
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['eta_DOCA']

        _track_PT="(PT>%s*MeV)"                        % config['Trk_PT']
        _track_Chi2="(TRCHI2DOF<%s)"                   % config['Trk_Chi2']
        _track_GPCut ="(TRGHOSTPROB<%s)"               % config['Trk_GP']
        _track_PIDCut = "(PROBNNpi>%s)"                % config['ProbNNCut']
        
        _allCuts = _PTCut+'&'+_vtxCut
        _trackCuts = _track_PT+'&'+_track_Chi2+'&'+_track_GPCut+'&'+_track_PIDCut
        _combCuts=_massCut+'&'+_docaCut
        _combCut12Doca="ACHI2DOCA(1,2)<%s"                     % config['eta_DOCA']
        _combCut12Vtx="(VFASPF(VCHI2/VDOF)<%s)"            % config['eta_vtxChi2']
        _combCut12=_combCut12Doca
        
        #_eta3Pi=CombineParticles("eta3Pi",
        _eta3Pi=DaVinci__N3BodyDecays("eta3Pi",
                                      DecayDescriptor = "eta -> pi+ pi- pi0",
                                      Combination12Cut = _combCut12,
                                      CombinationCut=_combCuts,
                                      MotherCut=_allCuts,
                                      DaughtersCuts = { "pi+" : _trackCuts, "pi-" : _trackCuts})
        
        self.selEta3Pi = Selection(name, Algorithm=_eta3Pi, RequiredSelections=[self.daughters])
        
    def makeEtap( self, name, config):
      
        _massCut = "(ADAMASS('eta_prime')<%s*MeV)"     % config['eta_prime_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['eta_prime_PT']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['eta_prime_vtxChi2']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['eta_prime_DOCA']
        _track_PT="(PT>%s*MeV)"                        % config['Trk_PT']
        _track_Chi2="(TRCHI2DOF<%s)"                   % config['Trk_Chi2']
        _track_GPCut ="(TRGHOSTPROB<%s)"               % config['Trk_GP']
        _track_PIDCut = "(PROBNNpi>%s)"                % config['ProbNNCut']

        _allCuts = _PTCut+'&'+_vtxCut
        _trackCuts = _track_PT+'&'+_track_Chi2+'&'+_track_GPCut+'&'+_track_PIDCut
        _combCuts=_massCut+'&'+_docaCut
        _combCut12Doca="ACHI2DOCA(1,2)<%s"                     % config['eta_prime_DOCA']
        _combCut12Vtx="(VFASPF(VCHI2/VDOF)<%s)"            % config['eta_prime_vtxChi2']
        _combCut12=_combCut12Doca
        
        #_etap=CombineParticles("etap",
        _etap=DaVinci__N3BodyDecays("etap",
                              DecayDescriptors = ["eta_prime -> pi+ pi- gamma", "eta_prime -> pi+ pi- eta"],
                              Combination12Cut = _combCut12,
                              CombinationCut=_combCuts,
                              MotherCut=_allCuts,
                              DaughtersCuts = { "pi+" : _trackCuts, "pi-" : _trackCuts})
        
        self.selEtap = Selection(name, Algorithm=_etap, RequiredSelections=[self.daughters])
        
    def makeB2KSLLetap( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['BDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
        
        _b2etapksLL = CombineParticles("B2etapKSLL",
                                       DecayDescriptor = "B0 -> KS0 eta_prime",
                                       CombinationCut = _combCuts,
                                       MotherCut = _allCuts,
                                       ReFitPVs = self.refitPVs )
        self.selB2KSLLetap = Selection( name, Algorithm=_b2etapksLL, RequiredSelections=[self.selKS2LL, self.selEtap])

    def makeB2KSDDetap( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['BDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
        
        _b2etapksDD = CombineParticles("B2etapKSDD",
                                       DecayDescriptor = "B0 -> KS0 eta_prime",
                                       CombinationCut = _combCuts,
                                       MotherCut = _allCuts,
                                       ReFitPVs = self.refitPVs )
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
                                        ReFitPVs = self.refitPVs )
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
                                        ReFitPVs = self.refitPVs )
        self.selB2KSDDetaGG = Selection( name, Algorithm=_b2etaGGksDD, RequiredSelections=[self.selKS2DD, self.selEtaGG])
        
    def makeB2KSLLeta3Pi( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['BDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
                
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut+'&'+_diraCut
        
        _b2etaksLL = CombineParticles("B2etaKSLL",
                                      DecayDescriptor = "B0 -> KS0 eta",
                                      CombinationCut = _combCuts,
                                      MotherCut = _allCuts,
                                      ReFitPVs = self.refitPVs )
        self.selB2KSLLeta3Pi = Selection( name, Algorithm=_b2etaksLL, RequiredSelections=[self.selKS2LL, self.selEta3Pi])
        
    def makeB2KSDDeta3Pi( self, name, config):

        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['BDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
                
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut+'&'+_diraCut
        
        _b2eta3PiksDD = CombineParticles("B2eta3PiKSDD",
                                         DecayDescriptor = "B0 -> KS0 eta",
                                         CombinationCut = _combCuts,
                                         MotherCut = _allCuts,
                                         ReFitPVs = self.refitPVs )
        self.selB2KSDDeta3Pi = Selection( name, Algorithm=_b2eta3PiksDD, RequiredSelections=[self.selKS2DD, self.selEta3Pi])       
        
    
    def makeLb2LLLetap( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['LbDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
                
        _Lb2etapLLL = CombineParticles("Lb2etapLLL",
                                       DecayDescriptor = "[Lambda_b0 -> Lambda0 eta_prime]cc",
                                       CombinationCut = _combCuts,
                                       MotherCut = _allCuts,
                                       ReFitPVs = self.refitPVs )
        self.selLb2LLLetap = Selection( name, Algorithm=_Lb2etapLLL, RequiredSelections=[self.selL2LL, self.selEtap])
        
    def makeLb2LDDetap( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['LbDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
        
        _Lb2etapLDD = CombineParticles("Lb2etapLDD",
                                       DecayDescriptor = "[Lambda_b0 -> Lambda0 eta_prime]cc",
                                       CombinationCut = _combCuts,
                                       MotherCut = _allCuts )
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
                                        ReFitPVs = self.refitPVs )
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
                                        ReFitPVs = self.refitPVs )
        self.selLb2LDDetaGG = Selection( name, Algorithm=_Lb2etaGGLDD, RequiredSelections=[self.selL2DD, self.selEtaGG])
        
    def makeLb2LLLeta3Pi( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['LbDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut+'&'+_diraCut
        
        _Lb2eta3PiLLL = CombineParticles("Lb2eta3PiLLL",
                                         DecayDescriptor = "[Lambda_b0 -> Lambda0 eta]cc",
                                         CombinationCut = _combCuts,
                                         MotherCut = _allCuts,
                                         ReFitPVs = self.refitPVs )
        self.selLb2LLLeta3Pi = Selection( name, Algorithm=_Lb2eta3PiLLL, RequiredSelections=[self.selL2LL, self.selEta3Pi])
        
    def makeLb2LDDeta3Pi( self, name, config):

        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['LbDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut+'&'+_diraCut
        
        _Lb2eta3PiLDD = CombineParticles("Lb2eta3PiLDD",
                                         DecayDescriptor = "[Lambda_b0 -> Lambda0 eta]cc",
                                         CombinationCut = _combCuts,
                                         MotherCut = _allCuts,
                                         ReFitPVs = self.refitPVs )
        self.selLb2LDDeta3Pi = Selection( name, Algorithm=_Lb2eta3PiLDD, RequiredSelections=[self.selL2DD, self.selEta3Pi])


    def makeB2Kstetap(self, name, config):
        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['BDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
                
        _b2etapKst = CombineParticles("B2etapKst",
                                      DecayDescriptor = "[B0 ->  K*(892)0 eta_prime]cc",
                                      CombinationCut = _combCuts,
                                      MotherCut = _allCuts,
                                      ReFitPVs = self.refitPVs )
        self.selB2etapKst = Selection( name, Algorithm=_b2etapKst, RequiredSelections=[self.selKstar, self.selEtap])
        
    def makeB2KstetaGG(self, name, config):
        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_eta_IPCHI2']
        
        _combCuts = _massCut
        _allCuts = _PTCut+'&'+_IPChi2Cut
        
        _b2etaGGKst = CombineParticles("B2etaGGKst",
                                       DecayDescriptor = "[B0 -> K*(892)0 eta]cc",
                                       CombinationCut = _combCuts,
                                       MotherCut = _allCuts,
                                       ReFitPVs = self.refitPVs )
        self.selB2etaGGKst = Selection( name, Algorithm=_b2etaGGKst, RequiredSelections=[self.selKstar, self.selEtaGG])
        
    def makeB2Ksteta3Pi(self, name, config):
        _massCut = "(ADAMASS('B0')<%s*MeV)"            % config['B_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['B_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['BDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['B_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['B_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['B_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut+'&'+_diraCut
                
        _b2eta3PiKst = CombineParticles("B2eta3PiKst",
                                        DecayDescriptor = "[B0 -> K*(892)0 eta]cc",
                                        CombinationCut = _combCuts,
                                        MotherCut = _allCuts,
                                        ReFitPVs = self.refitPVs )
        self.selB2eta3PiKst = Selection( name, Algorithm=_b2eta3PiKst, RequiredSelections=[self.selKstar, self.selEta3Pi])
        
    def makeLb2pKetap(self, name, config):
        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['LbDaug_LL_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_vtxCut+'&'+_diraCut+'&'+_IPChi2Cut
        _combCut12Doca="ACHI2DOCA(1,2)<%s"                     % config['LbDaug_LL_maxDocaChi2']
        _combCut12Vtx="(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _combCut12=_combCut12Doca
        
        _Lb2pKetap = DaVinci__N3BodyDecays("Lb2pKetap",
                                           DecayDescriptor = "[Lambda_b0 -> p+ K- eta_prime]cc",
                                           Combination12Cut = _combCut12,
                                           CombinationCut = _combCuts,
                                           MotherCut = _allCuts,
                                           ReFitPVs = self.refitPVs )
        self.selLb2pKetap = Selection( name, Algorithm=_Lb2pKetap, RequiredSelections=[self.selProtons, self.selKaons, self.selEtap])
        
    def makeLb2pKetaGG(self, name, config):
        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_eta_IPCHI2']
        
        _combCuts = _massCut
        _allCuts = _PTCut+'&'+_IPChi2Cut
        _combCut12="ACHI2DOCA(1,2)<%s"                     % config['LbDaug_LL_maxDocaChi2']
               
        _Lb2pKetaGG = DaVinci__N3BodyDecays("Lb2pKetaGG",
                                            DecayDescriptor = "[Lambda_b0 -> p+ K- eta]cc",
                                            Combination12Cut = _combCut12,
                                            CombinationCut = _combCuts,
                                            MotherCut = _allCuts,
                                            ReFitPVs = self.refitPVs )
        self.selLb2pKetaGG = Selection( name, Algorithm=_Lb2pKetaGG, RequiredSelections=[self.selProtons, self.selKaons, self.selEtaGG])
        
    def makeLb2pKeta3Pi(self, name, config):
        _massCut = "(ADAMASS('Lambda_b0')<%s*MeV)"     % config['Lb_MassWindow']
        _PTCut = "(PT>%s*MeV)"                         % config['Lb_PTmin']
        _docaCut = "(ACUTDOCACHI2(%s,''))"                 % config['LbDaug_DD_maxDocaChi2']
        _vtxCut = "(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _diraCut = "(BPVDIRA>%s)"                      % config['Lb_Dira']
        _IPChi2Cut = "(BPVIPCHI2()<%s)"                % config['Lb_IPCHI2']
        
        _combCuts = _massCut+'&'+_docaCut
        _allCuts = _PTCut+'&'+_IPChi2Cut+'&'+_vtxCut+'&'+_diraCut
        _combCut12Doca="ACHI2DOCA(1,2)<%s"                     % config['LbDaug_LL_maxDocaChi2']
        _combCut12Vtx="(VFASPF(VCHI2/VDOF)<%s)"            % config['Lb_VtxChi2']
        _combCut12=_combCut12Doca
        
        _Lb2pKeta3Pi = DaVinci__N3BodyDecays("Lb2pKeta3Pi",
                                             DecayDescriptor = "[Lambda_b0 -> p+ K- eta]cc",
                                             Combination12Cut = _combCut12,
                                             CombinationCut = _combCuts,
                                             MotherCut = _allCuts,
                                             ReFitPVs = self.refitPVs )
        self.selLb2pKeta3Pi = Selection( name, Algorithm=_Lb2pKeta3Pi, RequiredSelections=[self.selProtons, self.selKaons, self.selEta3Pi])
