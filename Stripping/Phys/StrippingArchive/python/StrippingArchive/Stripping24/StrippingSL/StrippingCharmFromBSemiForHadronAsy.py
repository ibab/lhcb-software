"""
Partially reconstructed charm from B,
with muon and slow pion(s) tags.
Includes
- D0 -> K3pi with muon and D* tag
- Lambda_c+ -> pKpi with muon and Lambda_c* tags
"""
__author__ = ['Mika Vesterinen']
__date__ = '10/03/2013'
__version__ = '$Revision: 0.0 $'

from Gaudi.Configuration import *

from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLoosePions, StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons
from Configurables import TisTosParticleTagger

__all__ = ('CharmFromBSemiForHadronAsyAllLinesConf',
           'TOSFilter',
           'confdict')
           
default_config = {
  'CharmFromBSemiForHadronAsy' : {
        'WGs'         : ['Semileptonic'],
	'BUILDERTYPE' : 'CharmFromBSemiForHadronAsyAllLinesConf',
        'CONFIG'      : {
	"prescale_LbRS" : 1.0
	,"prescale_LbWS" : 0.2 
	,"prescale_D0to3piRS" : 1.0
	,"prescale_D0to3piWS" : 0.2
	,"prescale_D0toK2piRS" : 1.0
	,"prescale_D0toK2piWS" : 0.2 
	#### common to all modes
	,"GEC_nLongTrk" : 250. # adimensional
	,"GHOSTPROB_MAX" : 0.35 #adimensional
	,"Mu_PT" : 800. # MeV
	,"H_PT"  : 250. # MeV
	,"Pi_PIDKMax" : 6. # adimensional
	,"K_PIDKMin"  : 6. # adimensional
	,"Slowpi_PIDKMax" : 10. # adimensional
	,"Slowpi_PIDeMax" : 99. # adimensional
	,"Slowpi_PTMin"   : 200. # MeV
	##### specific to D0 modes
	,"MuPi_SUMPT_MIN" : 1300. # MeV
	,"MuPi_DOCACHI2_MAX" : 8.0 # adimensional
	,"MuPi_CHI2NDOF_MAX" : 3.0 # adimensional
	,"MuPi_DIRA_MIN" : -99. # adimensional
	,"MuPi_FDCHI2_MIN" : 20. # adimensional
	,"D0to3H_REQUIRE_TOS" : True # bool
	,"D0to3H_DOCACHI2_MAX" : 10. # adimensional
	,"D0to3H_VCHI2NDF_MAX" : 3.0 # adimensional
	,"D0to3H_SUMPT_MIN" : 1800. # MeV
	,"D0to3H_DZ" : 2.0 # mm
	,"D0to3H_3pi_DeltaMass_MAX" : 350. # MeV
	,"D0to3H_K2pi_DeltaMass_MAX" : 250. # MeV
	,"D0to3H_3pi_MASS_MIN" : 900. # MeV
	,"D0to3H_3pi_MASS_MAX" : 1400. # MeV
	,"D0to3H_K2pi_MASS_MIN" : 1300. # MeV
	,"D0to3H_K2pi_MASS_MAX" : 1800. # MeV
	,"D0to3H_B_MASS_MIN" : 1800. # MeV
	,"D0to3H_B_MASS_MAX" : 4900. # MeV
	,"D0to3H_B_DIRA_MIN" : 0.99 # adimensional
	,"D0to3H_B_VCHI2NDF_MAX" : 15. # adimensional
	,"D0to3H_B_DOCACHI2_MAX" : 50. # adimensional
	#### specific to Lambda_c+ modes
	,"PiPi_MASS_MAX" : 500.  # MeV
	,"PiPi_DOCACHI2_MAX" : 15. # adimensional
	,"PiPi_CHI2NDF" : 3. # adimensional
	,"PiPi_SUMPT_MIN" : 600.  # MeV
	,"MuPiPi_DOCACHI2_MAX" : 15. 
	,"MuPiPi_CHI2NDF": 3.
	,"MuPiPi_FDCHI2_MIN" : 20. # adimensional
	,"Lc2Kpi_REQUIRE_TOS" : True # bool
	,"Lc2Kpi_DOCACHI2_MAX" : 10. # adimensional
	,"Lc2Kpi_VCHI2NDF_MAX" : 3.0 # adimensional
	,"Lc2Kpi_SUMPT_MIN" : 1500.  # MeV
	,"Lc2Kpi_FDCHI2_MIN" : 20. # adimensional
	,"Lc2Kpi_MASS_MIN" : 800.  # MeV
	,"Lc2Kpi_MASS_MAX" : 1350. # MeV
	,"Lc2Kpi_DeltaMass_MAX" : 700. # MeV
	,"Lc2Kpi_DZ" : 1.0 # mm
	,"Lc2Kpi_B_MASS_MIN" : 2200. # MeV
	,"Lc2Kpi_B_MASS_MAX" : 4300. # MeV
	,"Lc2Kpi_B_FDCHI2_MIN" : 20. # adimensional
	,"Lc2Kpi_B_DIRA_MIN" : 0.99 # adimensional
	,"Lc2Kpi_B_DOCACHI2_MAX" : 50. # adimensional
	,"Lc2Kpi_B_VCHI2NDF_MAX" : 15. # adimensional
      },
    'STREAMS' : ['BhadronCompleteEvent']
    }
}   

class CharmFromBSemiForHadronAsyAllLinesConf(LineBuilder) :
    """
    """
    
    __configuration_keys__ = (
        "prescale_LbRS"
        ,"prescale_LbWS"
        ,"prescale_D0to3piRS" 
        ,"prescale_D0to3piWS" 
        ,"prescale_D0toK2piRS"
        ,"prescale_D0toK2piWS"
        #### common to all modes
        ,"GEC_nLongTrk" 
        ,"GHOSTPROB_MAX"
        ,"Mu_PT"
        ,"H_PT" 
        ,"Pi_PIDKMax"
        ,"K_PIDKMin"
        ,"Slowpi_PIDKMax"
        ,"Slowpi_PIDeMax"
        ,"Slowpi_PTMin" 
        ##### specific to D0 modes
        ,"MuPi_SUMPT_MIN"
        ,"MuPi_DOCACHI2_MAX"
        ,"MuPi_CHI2NDOF_MAX"
        ,"MuPi_DIRA_MIN" 
        ,"MuPi_FDCHI2_MIN"
        ,"D0to3H_REQUIRE_TOS"
        ,"D0to3H_DOCACHI2_MAX" 
        ,"D0to3H_VCHI2NDF_MAX"
        ,"D0to3H_SUMPT_MIN"
        ,"D0to3H_DZ" 
        ,"D0to3H_3pi_DeltaMass_MAX" 
        ,"D0to3H_K2pi_DeltaMass_MAX" 
        ,"D0to3H_3pi_MASS_MIN"
        ,"D0to3H_3pi_MASS_MAX" 
        ,"D0to3H_K2pi_MASS_MIN"
        ,"D0to3H_K2pi_MASS_MAX"
        ,"D0to3H_B_MASS_MIN" 
        ,"D0to3H_B_MASS_MAX" 
        ,"D0to3H_B_DIRA_MIN" 
        ,"D0to3H_B_VCHI2NDF_MAX" 
        ,"D0to3H_B_DOCACHI2_MAX" 
        #### specific to Lambda_c+ modes
        ,"PiPi_MASS_MAX" 
        ,"PiPi_DOCACHI2_MAX" 
        ,"PiPi_CHI2NDF" 
        ,"PiPi_SUMPT_MIN" 
        ,"MuPiPi_DOCACHI2_MAX"
        ,"MuPiPi_CHI2NDF"
        ,"MuPiPi_FDCHI2_MIN"
        ,"Lc2Kpi_REQUIRE_TOS"
        ,"Lc2Kpi_DOCACHI2_MAX" 
        ,"Lc2Kpi_VCHI2NDF_MAX" 
        ,"Lc2Kpi_SUMPT_MIN" 
        ,"Lc2Kpi_FDCHI2_MIN"
        ,"Lc2Kpi_MASS_MIN" 
        ,"Lc2Kpi_MASS_MAX"
        ,"Lc2Kpi_DeltaMass_MAX" 
        ,"Lc2Kpi_DZ" 
        ,"Lc2Kpi_B_MASS_MIN" 
        ,"Lc2Kpi_B_MASS_MAX" 
        ,"Lc2Kpi_B_FDCHI2_MIN" 
        ,"Lc2Kpi_B_DIRA_MIN" 
        ,"Lc2Kpi_B_DOCACHI2_MAX" 
        ,"Lc2Kpi_B_VCHI2NDF_MAX" 
        )
    __confdict__={}
    
    def __init__(self, _name, config) :

        LineBuilder.__init__(self, _name, config)
        self.__confdict__=config
        
        ### define the global event cuts
        ### max number of long tracks
        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" %config ,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        ### define the muon cuts
        self.MuonCuts = "(PT > %(Mu_PT)s *MeV) & (TRGHOSTPROB < %(GHOSTPROB_MAX)s) & (PIDmu > 0)" %config
        self.PionCuts = "(PT > %(H_PT)s *MeV) & (TRGHOSTPROB < %(GHOSTPROB_MAX)s) & (PIDK < %(Pi_PIDKMax)s)" %config 
        self.KaonCuts = "(PT > %(H_PT)s *MeV) & (TRGHOSTPROB < %(GHOSTPROB_MAX)s) & (PIDK > %(K_PIDKMin)s)" %config 
        self.SlowpionCuts = "(PT > %(Slowpi_PTMin)s*MeV) & (TRGHOSTPROB < %(GHOSTPROB_MAX)s) & (PIDe < %(Slowpi_PIDeMax)s)"\
                          "& (PIDK < %(Slowpi_PIDKMax)s)" %config
        
        self.Muons = Selection( "Mufor" + _name,
                                Algorithm = FilterDesktop(name = "MuFilterFor"+_name, Code = self.MuonCuts ),
                                RequiredSelections = [StdLooseMuons])
        
        self.Pions = Selection( "Pifor" + _name,
                                Algorithm = FilterDesktop(name = "PiFilterFor"+_name, Code = self.PionCuts),
                                RequiredSelections = [StdLoosePions])

        self.Kaons = Selection( "Kfor" + _name,
                                Algorithm = FilterDesktop(name = "KFilterFor"+_name, Code = self.KaonCuts),
                                RequiredSelections = [StdLooseKaons])

        self.Slowpions = Selection( "Slowpifor" + _name,
                                    Algorithm = FilterDesktop(name = "SlowpiFilterFor"+_name, Code = self.SlowpionCuts),
                                    RequiredSelections = [StdLoosePions])
        #### D0 -> K3pi lines
        
        self.D0to3pi_RS = D0To3HMaker("D0To3piRSFor"+_name
                                      ,config["D0to3H_3pi_MASS_MIN"],config["D0to3H_3pi_MASS_MAX"]
                                      ,config
                                      ,[self.Pions]
                                      ,['[D0 -> pi+ pi- pi+]cc'])

        self.D0to3pi_WS = D0To3HMaker("D0To3piWSFor"+_name
                                      ,config["D0to3H_3pi_MASS_MIN"],config["D0to3H_3pi_MASS_MAX"]
                                      ,config
                                      ,[self.Pions]
                                      ,['[D0 -> pi+ pi- pi+]cc','[D0 -> pi+ pi+ pi+]cc'])
        
        self.D0toK2pi_RS = D0To3HMaker("D0ToK2piRSFor"+_name
                                       ,config["D0to3H_K2pi_MASS_MIN"],config["D0to3H_K2pi_MASS_MAX"]
                                       ,config,[self.Kaons,self.Pions]
                                       ,['[D0 -> K- pi+ pi-]cc','[D0 -> K- pi+ pi+]cc'])

        self.D0toK2pi_WS = D0To3HMaker("D0ToK2piWSFor"+_name
                                       ,config["D0to3H_K2pi_MASS_MIN"],config["D0to3H_K2pi_MASS_MAX"]
                                       ,config,[self.Kaons,self.Pions]
                                       ,['[D0 -> K- pi+ pi-]cc',
                                         '[D0 -> K- pi+ pi+]cc',
                                         '[D0 -> K- pi- pi-]cc',
                                         '[D0 -> K+ pi+ pi-]cc',
                                         '[D0 -> K+ pi+ pi+]cc',
                                         '[D0 -> K+ pi- pi-]cc'])

        self.MuPi_RS = MuPiMaker("MuPiRSFor"+_name
                                 ,config
                                 ,[self.Muons,self.Slowpions]
                                 ,['[K*(892)+ -> mu+ pi-]cc'])

        self.MuPi_WS = MuPiMaker("MuPiWSFor"+_name
                                 ,config
                                 ,[self.Muons,self.Slowpions]
                                 ,['[K*(892)+ -> mu+ pi-]cc',
                                   '[K*(892)+ -> mu- pi+]cc',
                                   '[K*(892)+ -> mu+ pi+]cc',
                                   '[K*(892)+ -> mu- pi-]cc'])

        self.BtoDstarMuD0to3pi_RS = BtoDstarMuMaker("BtoDstarMuD0to3pi_RSFor"+_name
                                                    ,config["D0to3H_3pi_DeltaMass_MAX"]
                                                    ,config
                                                    ,[self.MuPi_RS,self.D0to3pi_RS]
                                                    ,['[B0 -> D~0 K*(892)+]cc'])

        self.BtoDstarMuD0to3pi_WS = BtoDstarMuMaker("BtoDstarMuD0to3pi_WSFor"+_name
                                                    ,config["D0to3H_3pi_DeltaMass_MAX"]
                                                    ,config
                                                    ,[self.MuPi_WS,self.D0to3pi_WS]
                                                    ,['[B0 -> D~0 K*(892)+]cc'])
        
        self.BtoDstarMuD0toK2pi_RS = BtoDstarMuMaker("BtoDstarMuD0toK2pi_RSFor"+_name
                                                     ,config["D0to3H_K2pi_DeltaMass_MAX"]
                                                     ,config
                                                     ,[self.MuPi_RS,self.D0toK2pi_RS]
                                                     ,['[B0 -> D~0 K*(892)+]cc'])

        self.BtoDstarMuD0toK2pi_WS = BtoDstarMuMaker("BtoDstarMuD0toK2pi_WSFor"+_name
                                                     ,config["D0to3H_K2pi_DeltaMass_MAX"]
                                                     ,config
                                                     ,[self.MuPi_WS,self.D0toK2pi_WS]
                                                     ,['[B0 -> D~0 K*(892)+]cc'])
        
        self.B2DstarMuD0to3piRSLine = StrippingLine(_name+'_B2DstarMuD0to3piRS',
                                                    prescale = config["prescale_D0to3piRS"],
                                                    FILTER=GECs,
                                                    selection = self.BtoDstarMuD0to3pi_RS)

        self.B2DstarMuD0to3piWSLine = StrippingLine(_name+'_B2DstarMuD0to3piWS',
                                                    prescale = config["prescale_D0to3piWS"],
                                                    FILTER=GECs,
                                                    selection = self.BtoDstarMuD0to3pi_WS)
        
        self.B2DstarMuD0toK2piRSLine = StrippingLine(_name+'_B2DstarMuD0toK2piRS',
                                                    prescale = config["prescale_D0toK2piRS"],
                                                    FILTER=GECs,
                                                    selection = self.BtoDstarMuD0toK2pi_RS)

        self.B2DstarMuD0toK2piWSLine = StrippingLine(_name+'_B2DstarMuD0toK2piWS',
                                                    prescale = config["prescale_D0toK2piWS"],
                                                    FILTER=GECs,
                                                    selection = self.BtoDstarMuD0toK2pi_WS)
        
        
        ###################### Lambda_c lines ##########################
        
        self.LcToKpi_RS = LcToKpiMaker("LcToKpiRSFor"+_name,config,[self.Kaons,self.Pions],['[Lambda_c+ -> K- pi+]cc'])
        self.LcToKpi_WS = LcToKpiMaker("LcToKpiWSFor"+_name,config,[self.Kaons,self.Pions],
                                        ['[Lambda_c+ -> K- pi+]cc','[Lambda_c+ -> K+ pi-]cc','[Lambda_c+ -> K+ pi+]cc','[Lambda_c+ -> K- pi-]cc'])
        
        self.PiPi_RS = PiPiMaker("PiPiRSFor"+_name,config,[self.Slowpions],['rho(770)0 -> pi+ pi-'])
        self.PiPi_WS = PiPiMaker("PiPiWSFor"+_name,config,[self.Slowpions],['rho(770)0 -> pi+ pi-','rho(770)0 -> pi+ pi+','rho(770)0 -> pi- pi-'])

        self.MuPiPi_RS = MuPiPiMaker("MuPiPiRSFor"+_name,config,[self.Muons,self.PiPi_RS],['[K*(892)+ -> mu+ rho(770)0]cc'])
        self.MuPiPi_WS = MuPiPiMaker("MuPiPiWSFor"+_name,config,[self.Muons,self.PiPi_WS],['[K*(892)+ -> mu+ rho(770)0]cc',
                                                                                           '[K*(892)+ -> mu- rho(770)0]cc'])

        self.Lb_RS = LbMaker("LbRSFor"+_name,config,[self.MuPiPi_RS,self.LcToKpi_RS],['[Lambda_b0 -> Lambda_c+ K*(892)-]cc'])
        self.Lb_WS = LbMaker("LbWSFor"+_name,config,[self.MuPiPi_WS,self.LcToKpi_WS],['[Lambda_b0 -> Lambda_c+ K*(892)-]cc'])

        self.LbRSLine = StrippingLine(_name+'_LbToLcStarMuLcToKpiRS',
                                      prescale = config["prescale_LbRS"],
                                      FILTER=GECs,
                                      selection = self.Lb_RS)
        
        self.LbWSLine = StrippingLine(_name+'_LbToLcStarMuLcToKpiWS',
                                      prescale = config["prescale_LbWS"],
                                      FILTER=GECs,
                                       selection = self.Lb_WS)

        ######## register lines 
        
        self.registerLine(self.B2DstarMuD0to3piRSLine)        
        self.registerLine(self.B2DstarMuD0to3piWSLine)        
        self.registerLine(self.B2DstarMuD0toK2piRSLine)        
        self.registerLine(self.B2DstarMuD0toK2piWSLine)        
        self.registerLine(self.LbRSLine)        
        self.registerLine(self.LbWSLine)        
        
def MuPiMaker(_combName,config,_RequiredSelections,_decayDescriptors):
    _CombinationCut = "(ACHILD(PT,1)+ACHILD(PT,2) > %(MuPi_SUMPT_MIN)s *MeV)"\
                      "& (ADOCACHI2CUT(%(MuPi_DOCACHI2_MAX)s, ''))" %config
    _MotherCut = "(VFASPF(VCHI2/VDOF) < %(MuPi_CHI2NDOF_MAX)s)"\
                 "& (BPVDIRA> %(MuPi_DIRA_MIN)s)"\
                 "& (BPVVDCHI2 > %(MuPi_FDCHI2_MIN)s)" %config
    _comb = CombineParticles( name = "Comb"+_combName,
                              DecayDescriptors = _decayDescriptors,
                              CombinationCut = _CombinationCut,
                              MotherCut = _MotherCut)
    sel = Selection("Sel"+_combName,
                    Algorithm = _comb,
                    RequiredSelections = _RequiredSelections)
    return sel

def BtoDstarMuMaker(_combName,DeltaMass_MAX,config,_RequiredSelections,_decayDescriptors):
    preambulo = ['PXD0 = ACHILD(PX,1)',
                 'PYD0 = ACHILD(PY,1)',
                 'PZD0 = ACHILD(PZ,1)',
                 'ED0  = ACHILD(E,1)',
                 'PXPI = ACHILD(CHILD(PX,2),2)',
                 'PYPI = ACHILD(CHILD(PY,2),2)',
                 'PZPI = ACHILD(CHILD(PZ,2),2)',
                 'EPI  = ACHILD(CHILD(E,2),2)',
                 'MD0PI = sqrt((EPI+ED0)**2 - (PXPI+PXD0)**2 - (PYPI+PYD0)**2 - (PZPI+PZD0)**2)',
                 'DELTA_MASS = MD0PI - ACHILD(M,1)']
    _CombinationCut = "(DELTA_MASS < %s *MeV)" %DeltaMass_MAX
    _CombinationCut += "& (ADOCACHI2CUT(%(D0to3H_B_DOCACHI2_MAX)s, ''))" %config 
    _MotherCut = "(M > %(D0to3H_B_MASS_MIN)s *MeV) & (M < %(D0to3H_B_MASS_MAX)s *MeV)"\
                 "& (MINTREE((ABSID=='D0'),VFASPF(VZ)) - MINTREE((ABSID=='K*(892)+'),VFASPF(VZ)) > %(D0to3H_DZ)s *mm )"\
                 "& (VFASPF(VCHI2/VDOF) < %(D0to3H_B_VCHI2NDF_MAX)s)"\
                 "& (BPVDIRA> %(D0to3H_B_DIRA_MIN)s)" %config
    _comb = CombineParticles( name = "Comb"+_combName,
                              Preambulo = preambulo,
                              DecayDescriptors = _decayDescriptors,
                              CombinationCut = _CombinationCut,
                              MotherCut = _MotherCut)
    sel = Selection("Sel"+_combName,
                    Algorithm = _comb,
                    RequiredSelections = _RequiredSelections)
    if config["D0to3H_REQUIRE_TOS"] == True:
        sel_Hlt1TOS = TOSFilter( "Sel"+_combName+"_Hlt1TOS"
                                 ,sel
                                 ,"Hlt1.*Decision%TOS")
        sel_Hlt2TOS = TOSFilter( "Sel"+_combName+"_Hlt2TOS"
                                 ,sel_Hlt1TOS
                                 ,"Hlt2.*Decision%TOS")
        return sel_Hlt2TOS
    else:
        return sel

def D0To3HMaker(_combName,MASS_MIN,MASS_MAX,config,_RequiredSelections,_decayDescriptors):
    _CombinationCut = "(AM+10 > %s *MeV) & (AM-10 < %s *MeV)" %(MASS_MIN,MASS_MAX)
    _CombinationCut = _CombinationCut + "& (ADOCACHI2CUT(%(D0to3H_DOCACHI2_MAX)s, ''))"\
                      "& (ACHILD(PT,1) + ACHILD(PT,2) + ACHILD(PT,3) > %(D0to3H_SUMPT_MIN)s)" %config
    ### different mass cut depending on whether we miss a pion or a kaon
    _MotherCut = "(M > %s *MeV) & (M < %s *MeV)" %(MASS_MIN,MASS_MAX)
    _MotherCut = _MotherCut + "& (VFASPF(VCHI2/VDOF)< %(D0to3H_VCHI2NDF_MAX)s)" %config 
    _comb = CombineParticles( name = "Comb"+_combName,
                              DecayDescriptors = _decayDescriptors,
                              CombinationCut = _CombinationCut,
                              MotherCut = _MotherCut)
    sel = Selection("Sel"+_combName,
                    Algorithm = _comb,
                    RequiredSelections = _RequiredSelections)
    return sel



def PiPiMaker(_combName,config,_RequiredSelections,_decayDescriptors):
    comb = CombineParticles(name="Comb"+_combName,
                            DecayDescriptors = _decayDescriptors, 
                            CombinationCut = "(AM < %(PiPi_MASS_MAX)s*MeV)"\
                            "& (ACUTDOCACHI2(%(PiPi_DOCACHI2_MAX)s,''))"\
                            "& (ACHILD(PT,1) + ACHILD(PT,2) > %(PiPi_SUMPT_MIN)s *MeV)" % config, 
                            MotherCut = "(VFASPF(VCHI2/VDOF)< %(PiPi_CHI2NDF)s)" % config)        
    sel = Selection("Sel"+_combName,
                    Algorithm = comb,
                    RequiredSelections = _RequiredSelections)
    return sel

def MuPiPiMaker(_combName,config,_RequiredSelections,_decayDescriptors):
    comb = CombineParticles(name="Comb"+_combName,
                            DecayDescriptors = _decayDescriptors, 
                            CombinationCut = "(ACUTDOCACHI2(%(MuPiPi_DOCACHI2_MAX)s,''))" % config,
                            MotherCut = "(VFASPF(VCHI2/VDOF)< %(MuPiPi_CHI2NDF)s)"\
                            "& (BPVVDCHI2 > %(MuPiPi_FDCHI2_MIN)s)" %config)
    sel = Selection("Sel"+_combName,
                    Algorithm = comb,
                    RequiredSelections = _RequiredSelections)
    return sel

def LcToKpiMaker(_combName,config,_RequiredSelections,_decayDescriptors):
    ### Cuts on the Kpi combination
    ### sumpt, mass and docachi2 cuts
    _CombinationCut = "(ACHILD(PT,1)+ACHILD(PT,2) > %(Lc2Kpi_SUMPT_MIN)s *MeV)"\
                      "& (AM+10 > %(Lc2Kpi_MASS_MIN)s *MeV)"\
                      "& (AM-10 < %(Lc2Kpi_MASS_MAX)s *MeV)"\
                      "& (ADOCACHI2CUT(%(Lc2Kpi_DOCACHI2_MAX)s, ''))" % config
    ### mass, vertex quality and FD cuts
    _MotherCut = "(M > %(Lc2Kpi_MASS_MIN)s *MeV)"\
                 "& (M < %(Lc2Kpi_MASS_MAX)s *MeV)"\
                 "& (VFASPF(VCHI2/VDOF)< %(Lc2Kpi_VCHI2NDF_MAX)s)"\
                 "& (BPVVDCHI2 > %(Lc2Kpi_FDCHI2_MIN)s)" % config
    _comb = CombineParticles( name = "Comb"+_combName,
                              DecayDescriptors = _decayDescriptors,
                              CombinationCut = _CombinationCut,
                              MotherCut = _MotherCut)
    sel = Selection("Sel"+_combName,
                    Algorithm = _comb,
                    RequiredSelections = _RequiredSelections)
    return sel

def LbMaker(_combName,config,_RequiredSelections,_decayDescriptors):
    preambulo = ['PXLC = ACHILD(PX,1)',
                 'PYLC = ACHILD(PY,1)',
                 'PZLC = ACHILD(PZ,1)',
                 'ELC  = ACHILD(E,1)',
                 'PXPIPI = ACHILD(CHILD(PX,2),2)',
                 'PYPIPI = ACHILD(CHILD(PY,2),2)',
                 'PZPIPI = ACHILD(CHILD(PZ,2),2)',
                 'EPIPI = ACHILD(CHILD(E,2),2)',
                 'MLCPIPI = sqrt((EPIPI+ELC)**2 - (PXPIPI+PXLC)**2 - (PYPIPI+PYLC)**2 - (PZPIPI+PZLC)**2)',
                 'DELTA_MASS_LC = MLCPIPI - ACHILD(M,1)']
    comb = CombineParticles(name="Comb"+_combName,
                            DecayDescriptors = _decayDescriptors,
                            Preambulo = preambulo,
                            CombinationCut = "(DELTA_MASS_LC < %(Lc2Kpi_DeltaMass_MAX)s *MeV)"\
                            "& (AM > %(Lc2Kpi_B_MASS_MIN)s *MeV)  & (AM < %(Lc2Kpi_B_MASS_MAX)s *MeV)"\
                            "& (ACUTDOCACHI2(%(Lc2Kpi_B_DOCACHI2_MAX)s,''))" % config,
                            MotherCut = "(M > %(Lc2Kpi_B_MASS_MIN)s  *MeV) & (M < %(Lc2Kpi_B_MASS_MAX)s  *MeV)"\
                            "& (MINTREE((ABSID=='Lambda_c+'),VFASPF(VZ)) - MINTREE((ABSID=='K*(892)-'),VFASPF(VZ)) > %(Lc2Kpi_DZ)s *mm )"\
                            "& (VFASPF(VCHI2/VDOF) < %(Lc2Kpi_B_VCHI2NDF_MAX)s)"
                            "& (BPVDIRA> %(Lc2Kpi_B_DIRA_MIN)s)"\
                            "& (BPVVDCHI2 > %(Lc2Kpi_B_FDCHI2_MIN)s)" %config)
    sel = Selection("Sel"+_combName,
                    Algorithm = comb,
                    RequiredSelections = _RequiredSelections)
    if config["Lc2Kpi_REQUIRE_TOS"] == True:
        sel_Hlt1TOS = TOSFilter( "Sel"+_combName+"_Hlt1TOS"
                                 ,sel
                                 ,"Hlt1.*Decision%TOS")
        sel_Hlt2TOS = TOSFilter( "Sel"+_combName+"_Hlt2TOS"
                                 ,sel_Hlt1TOS
                                 ,"Hlt2.*Decision%TOS")
        return sel_Hlt2TOS
    else:
        return sel
        
def TOSFilter( name, _input, _trigger ) :
    from Configurables import TisTosParticleTagger
    _tisTosFilter = TisTosParticleTagger( name + "Tagger" )
    _tisTosFilter.TisTosSpecs = { _trigger : 0 }
    return Selection( name
                      , Algorithm = _tisTosFilter
                      , RequiredSelections = [ _input ]
                      )

