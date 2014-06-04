__author__ = ['Mika Vesterinen','Liming Zhang, Alessandra Borgia']
__date__ = '23/07/2010'
__version__ = '$Revision: 4.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdNoPIDsUpPions
from Configurables import ConjugateNeutralPID

__all__ = ('B2DMuNuXAllLinesConf',
           'makeb2DMuX',
           'makeb2DsMuX',
           'makeb2DX',
           'makeDstar',
           'TOSFilter',
           'confdict')

confdict = {
    ##### global settings
    "prescales" : {"b2D0MuX":0.5}
    ,"GEC_nLongTrk" : 250 # adimensional
    ,"TTSpecs"      : {} #'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0,'Hlt2.*IncPhi.*Decision%TOS':0,"Hlt2Global%TIS":0} 
    ,"HLT_FILTER" : {} #"HLT_PASS_RE('Hlt2.*SingleMuon.*Decision') | HLT_PASS_RE('Hlt2Topo(2|3|4)Body.*Decision') | HLT_PASS_RE('Hlt2.*IncPhi.*Decision')"
    ##### daughter particles
    ,"TRGHOSTPROB"   : 0.5  # adimensional
    ,"TRCHI2"        : 4    # adimensional
    ,"MuonGHOSTPROB" : 0.5  # adimensional
    ,"MINIPCHI2"     : 9.0  # adimensiional
    ,"ProtonPIDp"    : 4.0  # adimensiional 
    ,"KaonPIDK"      : -5.0 # adimensiional ## loose
    ,"PionPIDK"      : 20.0 # adimensiional
    ,"MuonPIDmu"     : 0.0  # adimensiional
    ,"MuonIPCHI2"    : 4.00 # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"HadronPT"      : 250.0  # MeV
    ,"FakeMuonPT"    : 1500.0  # MeV
    ,"MuonP"         : 6.0  # GeV
    ,"HadronP"       : 2.0  # GeV
    ,"ProtonP"       : 2.0  # GeV
    ,"FakeMuonP"     : 6.0    # GeV
    ###### charm combination
    ,"D_DIRA"        : 0.99   # adimensiional
    ,"D_FDCHI2"      : 100.0  # adimensiional
    ,"D_MassWin"     : 80.0   # MeV
    ,"D_AMassWin"    : 100.0  # MeV
    ,"D_MINIP"          : 7.4    #mm
    ,"D_VCHI2DOF"    : 6.0    # adimensiional
    ,"D_DocaChi2Max"  : 20     #adimensiional
    ###### D-mu combination
    ,"B_DIRA"         : 0.999  #adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"B_D_DZ"            : 0  #mm
    ,"BMassMin" : 2.5 #GeV
    ,"BMassMax" : 6.0 #GeV
    ###### For D* decays
    ,"Dstar_Chi2"       :  8.0 ## unitless
    ,"Dstar_SoftPion_PIDe" : 2. ## unitless
    ,"Dstar_SoftPion_PT" : 180. ## MeV ###
    ,"Dstar_wideDMCutLower" : 0. ## MeV
    ,"Dstar_wideDMCutUpper" : 170. ## MeV,
    }

class B2DMuNuXAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "prescales"
        ,"GEC_nLongTrk"
        ,"HLT_FILTER"
        ,"TRGHOSTPROB"
        ,"TRCHI2"
        ,"MuonGHOSTPROB"
        ,"MINIPCHI2"
        ,"ProtonPIDp"
        ,"KaonPIDK"
        ,"PionPIDK"
        ,"MuonIPCHI2"    
        ,"MuonPT"        
        ,"HadronPT"
        ,"FakeMuonPT" 
        ,"D_DIRA"        
        ,"D_FDCHI2"      
        ,"D_MassWin"     
        ,"D_AMassWin"    
        ,"D_MINIP"          
        ,"D_VCHI2DOF"    
        ,"MuonPIDmu"         
        ,"B_DIRA"         
        ,"BVCHI2DOF"     
        ,"B_D_DZ"
        ,"D_DocaChi2Max"
        ,"Dstar_Chi2"       
        ,"Dstar_SoftPion_PIDe" 
        ,"Dstar_SoftPion_PT" 
        ,"Dstar_wideDMCutLower" 
        ,"Dstar_wideDMCutUpper" 
        ,"FakeMuonP"
        ,"MuonP"
        ,"HadronP"
        ,"ProtonP"
        ,"BMassMin"
        ,"BMassMax"
        ,"TTSpecs"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config

        ####################### BASIC FINAL STATE PARTICLE SELECTIONS ##########################
        
        self.selmuon = Selection("Mufor"+name,
                                 Algorithm=FilterDesktop(Code="(PT > %(MuonPT)s *MeV) & (P> %(MuonP)s*GeV)"\
                                                             "& (TRCHI2DOF < %(TRCHI2)s)"\
                                                             "&(TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
                                                             "& (MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s) & (PIDmu > %(MuonPIDmu)s)" % config),
                                 RequiredSelections = [StdLooseMuons])
        
        self.selKaon = Selection( "Kfor" + name,
                                  Algorithm = FilterDesktop(Code="(P>%(HadronP)s*GeV) & (PT > %(HadronPT)s *MeV)"\
                                                                "& (TRCHI2DOF < %(TRCHI2)s)"\
                                                                "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                                                                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK> %(KaonPIDK)s)" % config),
                                  RequiredSelections = [StdLooseKaons])
        
        self.selPion = Selection( "Pifor" + name,
                                  Algorithm = FilterDesktop(Code="(P>%(HadronP)s*GeV) & (PT > %(HadronPT)s *MeV)"\
                                                                "& (TRCHI2DOF < %(TRCHI2)s)"\
                                                                "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                                                                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDK)s)" % config),                            
                                  RequiredSelections = [StdLoosePions])

        self.selProton = Selection( "ProtonsFor" + name,
                                    Algorithm = FilterDesktop(Code="(P>%(ProtonP)s*GeV) & (PT > %(HadronPT)s *MeV)"\
                                                                  "& (TRCHI2DOF < %(TRCHI2)s)"\
                                                                  "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                                                                  "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)"\
                                                                  "& (PIDp> %(ProtonPIDp)s) & (PIDp-PIDK>1.0e-10)" % config),
                                    RequiredSelections = [StdLooseProtons])
        
        self.selPion_fakes = Selection( "Pion_fakes_for" + name,
                                        Algorithm = FilterDesktop(Code = "(P>%(FakeMuonP)s*GeV) & (PT > %(FakeMuonPT)s*MeV)"\
                                                                      "& (TRCHI2DOF < %(TRCHI2)s)"\
                                                                      "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                                                                      "& (INMUON) & (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (PIDmu < %(MuonPIDmu)s)" % config),
                                        RequiredSelections = [StdLoosePions])

        ####################### D0 -> HH LINES (inc D*->D0pi) ###############################
        
        # comments
        # try to copy the same requirements as the previous stripping
        D02HH_CONFIG = config.copy()
        #D02HH_CONFIG["TTSpecs"] = {} 
        D02HH_CONFIG["ExtraMuonCuts"] = "(PT>1.2*GeV) & (MIPCHI2DV(PRIMARY)> 9.0)" % config 
        D02HH_CONFIG["CharmComboCuts"] = "(ADAMASS('D0') < %(D_AMassWin)s *MeV)"\
            "& (ACHILD(PT,1)+ACHILD(PT,2) > 1400.*MeV) & (ADOCACHI2CUT( %(D_DocaChi2Max)s, ''))" % config
        D02HH_CONFIG["CharmMotherCuts"] = "(SUMTREE( PT,  ISBASIC )>1400.*MeV)"\
            "& (ADMASS('D0') < %(D_MassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(D_VCHI2DOF)s) " \
            "& (BPVVDCHI2 > %(D_FDCHI2)s) &  (BPVDIRA> %(D_DIRA)s)"  % config
        D02HH_CONFIG["B_D_DZ"] = -9999
        D02HH_CONFIG["CharmDaugCuts"] = {"K+":"(PIDK > 4) & (PT > 300*MeV)",
                                         "pi+":"(PIDK < 4) & (PT > 300*MeV)"}
        
        self.b2D0MuXLine = makeb2DMuXNEW(name,
                                         'b2D0MuX',
                                         ['[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                         ['[D0 -> K- pi+]cc'],
                                         D02HH_CONFIG,
                                         [self.selKaon, self.selPion],self.selmuon)
        
        self.b2DstarMuXLine = makeb2DMuXNEW(name,
                                            'b2DstarMuX',
                                            ['[B~0 -> D*(2010)+ mu-]cc','[B~0 -> D*(2010)- mu+]cc'],
                                            ['[D0 -> K- pi+]cc'],
                                            D02HH_CONFIG,
                                            [self.selKaon, self.selPion],self.selmuon)
        
        self.b2D0MuXDCSLine = makeb2DMuXNEW(name,
                                            'b2D0MuXDCS',
                                            ['[B+ -> D0 mu+]cc'],
                                            ['[D0 -> K- pi+]cc'],
                                            D02HH_CONFIG,
                                            [self.selKaon, self.selPion],self.selmuon)
        
        self.b2D0MuXKKLine = makeb2DMuXNEW(name,
                                           'b2D0MuXKK',
                                           ['[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                           ['D0 -> K- K+'],
                                           D02HH_CONFIG,
                                           [self.selKaon],self.selmuon)
        
        self.b2D0MuXpipiLine = makeb2DMuXNEW(name,
                                             'b2D0MuXpipi',
                                             ['[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                             ['D0 -> pi- pi+'],
                                             D02HH_CONFIG,
                                             [self.selPion],self.selmuon)
        
        self.registerLine(self.b2D0MuXLine)        
        self.registerLine(self.b2D0MuXDCSLine)        
        self.registerLine(self.b2D0MuXKKLine)        
        self.registerLine(self.b2D0MuXpipiLine)        
        
        ####################### D0 -> HHHH LINES (inc D*->D0pi) ###############################
        
        # comments: try to improve the efficiency of this line a lot
        D02HHHH_CONFIG = config.copy()
        D02HHHH_CONFIG["B_D_DZ"] = 0.0
        D02HHHH_CONFIG["D_MassWin"] = 50.
        D02HHHH_CONFIG["D_AMassWin"] = 55.
        D02HHHH_CONFIG["CharmDaugCuts"] = {"K+":"(PIDK > 4) & (PT > 300*MeV)",
                                           "pi+":"(PIDK < 4) & (PT > 300*MeV)"}
        D02HHHH_CONFIG["ExtraMuonCuts"] = "(PT>1.5*GeV) & (MIPCHI2DV(PRIMARY)> 9.0)" % D02HHHH_CONFIG 
        D02HHHH_CONFIG["CharmComboCuts"] =  "(ADAMASS('D0') < %(D_AMassWin)s *MeV)"\
            "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > 2000 *MeV)"\
            "& (ADOCACHI2CUT( %(D_DocaChi2Max)s, ''))" % D02HHHH_CONFIG
        D02HHHH_CONFIG["CharmMotherCuts"] = "(ADMASS('D0') < %(D_MassWin)s *MeV)"\
            " & (VFASPF(VCHI2/VDOF) < %(D_VCHI2DOF)s) " \
            " & (SUMTREE( PT, ISBASIC )> 2000 *MeV)"\
            " & (BPVVDCHI2 > %(D_FDCHI2)s) &  (BPVDIRA> %(D_DIRA)s)"  % D02HHHH_CONFIG
        self.b2D0MuXK3PiLine = makeb2DMuXNEW(name,
                                             'b2D0MuXK3Pi',
                                             ['[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                             [ '[D0 -> K- pi+ pi- pi+]cc' ],
                                             D02HHHH_CONFIG,
                                             [self.selKaon, self.selPion],self.selmuon)

        self.b2D0MuX2K2PiLine = makeb2DMuXNEW(name,
                                              'b2D0MuX2K2Pi',
                                              ['[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                              [ '[D0 -> K- K+ pi- pi+]cc' ],
                                              D02HHHH_CONFIG,
                                              [self.selKaon, self.selPion],self.selmuon)
        
        self.b2D0MuX3KPiLine = makeb2DMuXNEW(name,
                                              'b2D0MuX3KPi',
                                              ['[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                              [ '[D0 -> K- K+ K- pi+]cc' ],
                                              D02HHHH_CONFIG,
                                             [self.selKaon, self.selPion],self.selmuon)
        
        self.registerLine(self.b2D0MuXK3PiLine)        
        self.registerLine(self.b2D0MuX2K2PiLine)        
        self.registerLine(self.b2D0MuX3KPiLine)        
        
        ####################### D+ -> HHH LINES ###############################
        D2HHH_CONFIG = config.copy()
        D2HHH_CONFIG["B_D_DZ"] = -0.2
        D2HHH_CONFIG["PhiMassMin"] = 979.455 #MeV
        D2HHH_CONFIG["PhiMassMax"] = 1059.455 #MeV
        D2HHH_CONFIG["KStarMassMin"] = 805.94 #MeV
        D2HHH_CONFIG["KStarMassMax"] = 985.94 #MeV
        D2HHH_CONFIG["TTSpecs"] = {'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0,'Hlt2.*IncPhi.*Decision%TOS':0,"Hlt2Global%TIS":0}           
        D2HHH_CONFIG["HLT_FILTER"] = "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision') | HLT_PASS_RE('Hlt2Topo(2|3|4)Body.*Decision') | HLT_PASS_RE('Hlt2.*IncPhi.*Decision')"  
        D2HHH_CONFIG["CharmComboCuts"] =  "(ADOCACHI2CUT( %(D_DocaChi2Max)s, ''))" % config
        D2HHH_CONFIG["CharmMotherCuts"] = "(VFASPF(VCHI2/VDOF) < %(D_VCHI2DOF)s)"\
            " & (BPVVDCHI2 > %(D_FDCHI2)s) &  (BPVDIRA> %(D_DIRA)s)"  % config
        D2HHH_CONFIG["CharmComboCuts"] += "& (DAMASS('D_s+') < %(D_AMassWin)s *MeV) & (DAMASS('D+')> -%(D_AMassWin)s *MeV)"\
            "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(D_DocaChi2Max)s, ''))" % config
        D2HHH_CONFIG["CharmMotherCuts"] += "&(SUMTREE( PT,  ISBASIC )>1800.*MeV)"\
            "& (DMASS('D_s+') < %(D_MassWin)s *MeV) & (DMASS('D+')> -%(D_AMassWin)s *MeV)"\
            "& (VFASPF(VCHI2/VDOF) < %(D_VCHI2DOF)s) & (BPVVDCHI2 > %(D_FDCHI2)s)" % config
        
        D2HHH_CONFIG_K2Pi = D2HHH_CONFIG.copy()
        D2HHH_CONFIG_K2Pi["ExtraMuonCuts"] = "(MIPCHI2DV(PRIMARY)> 9) & (PT > 800*MeV)"
        D2HHH_CONFIG_K2Pi["CharmDaugCuts"] = {"pi+":"(PIDK < 10) & (PT > 300*MeV)",
                                              "K+" :"(PIDK > 4) & (PT > 300*MeV)"}
        D2HHH_CONFIG_K2Pi["B_D_DZ"] = 0
        self.b2DpMuXLine = makeb2DMuXNEW(name,
                                         'b2DpMuX',
                                         [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                         [ '[D+ -> K- pi+ pi+]cc' ],
                                         D2HHH_CONFIG_K2Pi,
                                         [self.selKaon, self.selPion],self.selmuon)
        
        D2HHH_CONFIG_KKPi = D2HHH_CONFIG.copy()
        D2HHH_CONFIG_KKPi["BMassMin"] = 0
        D2HHH_CONFIG_KKPi["BMassMax"] = 9999
        self.b2DsMuXLine = makeb2DMuXNEW(name,
                                         'b2DsMuX',
                                         [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                         [ '[D+ -> K+ K- pi+]cc' ],
                                         D2HHH_CONFIG,
                                         [self.selKaon, self.selPion],self.selmuon)
        
        D2HHH_CONFIG_3Pi = D2HHH_CONFIG.copy()
        D2HHH_CONFIG_3Pi["CharmDaugCuts"] = {"pi+":"(PIDK < 4) & (PT > 380*MeV)"}
        D2HHH_CONFIG_3Pi["BMassMin"] = 0
        D2HHH_CONFIG_3Pi["BMassMax"] = 9999
        self.b2Ds3PiMuXLine = makeb2DMuXNEW(name,
                                            'b2Ds3PiMuX',
                                            [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                            [ '[D+ -> pi+ pi- pi+]cc' ],
                                            D2HHH_CONFIG,
                                            [self.selPion],self.selmuon)
        
        D2HHH_CONFIG_PhiPi = D2HHH_CONFIG.copy()
        D2HHH_CONFIG_PhiPi["CharmExtraComboCuts"]  = "& (AM12 > (%(PhiMassMin)s - 10) *MeV) & (AM12 < (%(PhiMassMax)s + 10) *MeV)" % D2HHH_CONFIG
        D2HHH_CONFIG_PhiPi["CharmExtraMotherCuts"] = "& (M12 > %(PhiMassMin)s *MeV) & (M12 < %(PhiMassMax)s *MeV)" % D2HHH_CONFIG
        D2HHH_CONFIG_PhiPi["BMassMin"] = 0
        D2HHH_CONFIG_PhiPi["BMassMax"] = 9999
        self.b2DsPhiPiMuXLine = makeb2DMuXNEW(name,
                                              'b2DsPhiPiMuX',
                                              [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                              [ '[D+ -> K+ K- pi+]cc' ],
                                              D2HHH_CONFIG_PhiPi,
                                              [self.selKaon, self.selPion],self.selmuon)
        
        D2HHH_CONFIG_KStarK = D2HHH_CONFIG.copy()
        D2HHH_CONFIG_KStarK["CharmExtraComboCuts"]  = "& (AM23 > (%(KStarMassMin)s - 10) *MeV) & (AM23 < (%(KStarMassMax)s + 10) *MeV)" % D2HHH_CONFIG
        D2HHH_CONFIG_KStarK["CharmExtraMotherCuts"] = "& (M23 > %(KStarMassMin)s *MeV) & (M23 < %(KStarMassMax)s *MeV)" % D2HHH_CONFIG
        D2HHH_CONFIG_KStarK["BMassMin"] = 0
        D2HHH_CONFIG_KStarK["BMassMax"] = 9999
        self.b2DsKStarKMuXLine = makeb2DMuXNEW(name,
                                               'b2DsKStarKMuX',
                                               [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                               [ '[D+ -> K+ K- pi+]cc' ],
                                               D2HHH_CONFIG_KStarK,
                                               [self.selKaon, self.selPion],self.selmuon)
        
        D2HHH_CONFIG_KKK = D2HHH_CONFIG.copy()
        D2HHH_CONFIG_KKK["B_D_DZ"] = -9999 # since for mass measurements
        D2HHH_CONFIG_KKK["CharmDaugCuts"] = {"K+":"(PIDK > 8) & (PT > 300*MeV)"}
        self.b2DsMuXKKKLine = makeb2DMuXNEW(name,
                                            'b2DsMuXKKK',
                                            [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                            [ '[D+ -> K+ K- K+]cc' ],
                                            D2HHH_CONFIG,
                                            [self.selKaon,],self.selmuon)
        
        D2HHH_CONFIG_PhiPiFakes = D2HHH_CONFIG_PhiPi.copy()
        self.b2DsPhiPiMuXFakesLine = makeb2DMuXNEW(name,
                                                   'b2DsPi_PhiPi_fakes'
                                                   [ '[B0 -> D- pi+]cc', '[B0 -> D- pi-]cc' ],
                                                   [ '[D+ -> K+ K- pi+]cc' ],
                                                   D2HHH_CONFIG_PhiPiFakes,
                                                   [self.selKaon, self.selPion],self.selPionFakes)
        
        self.registerLine(self.b2DpMuXLine)        
        self.registerLine(self.b2DsMuXLine)        
        self.registerLine(self.b2DsKStarKMuXLine)        
        self.registerLine(self.b2DsPhiPiMuXLine)   
        self.registerLine(self.b2DsPhiPiMuXFakesLine)   
        self.registerLine(self.b2Ds3PiMuXLine)        
        self.registerLine(self.b2DsMuXKKKLine)        
        
        ####################### D+ -> KS0H LINES ###############################
        
        #@@@@@@@@@@@@@@@@@@@ KS selections @@@@@@@@@@@@@@@@@@@@@@@@
        self.combKsLL = CombineParticles ( 
                                           DecayDescriptor = "KS0 -> pi+ pi-",
                                           DaughtersCuts = { "pi+" : "(P > 2.*GeV) & (MIPCHI2DV(PRIMARY) > 9.)" } ,
                                           CombinationCut = "(ADAMASS('KS0') < 25.*MeV) & (ADOCACHI2CUT(25, '')) ",
                                           MotherCut = "(ADMASS('KS0') < 20.*MeV) & (BPVVDCHI2 > 100) & (VFASPF(VCHI2) < 25.)")
        self.selKsLL = Selection("SelKsLLFor"+name,
                                 Algorithm = self.combKsLL,
                                 RequiredSelections = [ StdAllNoPIDsPions ])
        
        self.combKsLU_PosLong = CombineParticles(
                                                 DecayDescriptor = "KS0 -> pi+ pi-",
                                                 DaughtersCuts = { "pi+" : "(P > 2.*GeV) & (MIPCHI2DV(PRIMARY) > 9.) & (ISLONG)",
                                                                   "pi-" : "(P > 2.*GeV) & (MIPCHI2DV(PRIMARY) > 9.) & (ISUP)" } ,
                                                 CombinationCut = "(ADAMASS('KS0') < 25.*MeV) & (ADOCACHI2CUT(25, '')) ",
                                                 MotherCut = "(ADMASS('KS0') < 20.*MeV) & (BPVVDCHI2 > 100) & (VFASPF(VCHI2) < 25.)")
        self.combKsLU_NegLong = CombineParticles(
                                                 DecayDescriptor = "KS0 -> pi+ pi-",
                                                 DaughtersCuts = { "pi-" : "(P > 2.*GeV) & (MIPCHI2DV(PRIMARY) > 9.) & (ISLONG)",
                                                                   "pi+" : "(P > 2.*GeV) & (MIPCHI2DV(PRIMARY) > 9.) & (ISUP)" } ,
                                                 CombinationCut = "(ADAMASS('KS0') < 25.*MeV) & (ADOCACHI2CUT(25, '')) ",
                                                 MotherCut = "(ADMASS('KS0') < 20.*MeV) & (BPVVDCHI2 > 100) & (VFASPF(VCHI2) < 25.)")
        self.selKsLU_PosLong = Selection("SelKsLU_PosLongFor"+name,
                                         Algorithm = self.combKsLU_PosLong,
                                         RequiredSelections = [ StdAllNoPIDsPions,StdNoPIDsUpPions])

        self.selKsLU_NegLong = Selection("SelKsLU_NegLongFor"+name,
                                         Algorithm = self.combKsLU_NegLong,
                                         RequiredSelections = [ StdAllNoPIDsPions,StdNoPIDsUpPions])

        self.selKsLL = MergedSelection("SelMergedKsLLFor"+name, RequiredSelections = [ self.selKsLU_PosLong,self.selKsLU_NegLong,self.selKsLL])
        
        
        self.selKsDD = Selection("SelKsDDfor"+name,
                                 Algorithm = FilterDesktop(Code = "(ADMASS('KS0') < 20*MeV) & (PT > 500*MeV)"),
                                 RequiredSelections = [DataOnDemand("Phys/StdLooseKsDD/Particles")])
        #@@@@@@@@@@@@@@@@@@@ KS selections @@@@@@@@@@@@@@@@@@@@@@@@
        
        D2KSH_CONFIG = config.copy()
        D2KSH_CONFIG["B_D_DZ"] = -0.2
        D2KSH_CONFIG["TTSpecs"] = {'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0,'Hlt2.*IncPhi.*Decision%TOS':0,"Hlt2Global%TIS":0}           
        D2KSH_CONFIG["HLT_FILTER"] = "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision') | HLT_PASS_RE('Hlt2Topo(2|3|4)Body.*Decision')"  
        D2KSH_CONFIG["CharmComboCuts"] =  "(DAMASS('D_s+') < %(D_AMassWin)s *MeV) & (DAMASS('D+')> -%(D_AMassWin)s *MeV)"\
            "& (ADOCACHI2CUT( %(D_DocaChi2Max)s, ''))" % config
        D2KSH_CONFIG["CharmMotherCuts"] = "(DMASS('D_s+') < %(D_MassWin)s *MeV) & (DMASS('D+')> -%(D_AMassWin)s *MeV)"\
            "& (MINTREE(((ABSID=='KS0')) , VFASPF(VZ))-VFASPF(VZ) > 2.0 *mm )"\
            " & (BPVVDCHI2 > %(D_FDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(D_VCHI2DOF)s) & (BPVDIRA> %(D_DIRA)s)"  % config
        self.b2DpMuXKsLLPiLine = makeb2DMuXNEW(name,
                                               'b2DpMuXKsLLPi',
                                               [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                               [ '[D+ -> KS0 pi+]cc' ],
                                               D2KSH_CONFIG,
                                               [self.selKsLL, self.selPion],self.selmuon)
        self.registerLine(self.b2DpMuXKsLLPiLine)        
        
        self.b2DpMuXKsDDPiLine = makeb2DMuXNEW(name,
                                               'b2DpMuXKsDDPi',
                                               [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                               [ '[D+ -> KS0 pi+]cc' ],
                                               D2KSH_CONFIG,
                                               [self.selKsDD, self.selPion],self.selmuon)
        self.registerLine(self.b2DpMuXKsDDPiLine)        
        
        ####################### Lc+ -> pHH LINES ###############################
        LC2PHH_CONFIG = config.copy()
        LC2PHH_CONFIG["B_D_DZ"] = 0.0
        LC2PHH_CONFIG["D_DocaChi2Max"] = 9
        LC2PHH_CONFIG["CharmComboCuts"] = "(ADAMASS('Lambda_c+') < %(D_AMassWin)s *MeV)"\
            "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(D_DocaChi2Max)s, ''))" % LC2PHH_CONFIG
        LC2PHH_CONFIG["CharmMotherCuts"] = "(ADMASS('Lambda_c+') < %(D_MassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(D_VCHI2DOF)s) " \
            "& (BPVVDCHI2 > %(D_FDCHI2)s) & (SUMTREE( PT,  ISBASIC )>1800.*MeV) & (BPVDIRA> %(D_DIRA)s)"  % LC2PHH_CONFIG
        self.lb2LcMuXLine = makeb2DMuXNEW(name,
                                          "b2LcMuX",
                                          [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                          [ '[Lambda_c+ -> K- p+ pi+]cc' ],
                                          LC2PHH_CONFIG,
                                          [self.selProton,self.selKaon,self.selPion],self.selmuon)
        self.registerLine(self.lb2LcMuXLine)
            

###################### FUNCTION TO MAKE AN ENTIRE B->D MU X DECAY CANDIDATE  ########################
def makeb2DMuXNEW(module_name,name,BDecays,DDecays,CONFIG,CHARM_DAUGHTERS,MUON):
    
    DEFAULT_GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" %CONFIG,
                     "Preambulo": ["from LoKiTracks.decorators import *"]}
    DEFAULT_HLT = CONFIG["HLT_FILTER"]
    
    if "CharmDaugCuts" in CONFIG.keys():
        CHARM_DaugCuts = CONFIG["CharmDaugCuts"]
    else:
        CHARM_DaugCuts = {}
    CHARM_ComboCuts = CONFIG["CharmComboCuts"]
    CHARM_MotherCuts = CONFIG["CharmMotherCuts"]
    if "CharmExtraComboCuts" in CONFIG.keys():
        CHARM_ComboCuts += CONFIG["CharmExtraComboCuts"]
    if "CharmExtraMotherCuts" in CONFIG.keys():
        CHARM_MotherCuts += CONFIG["CharmExtraMotherCuts"]
    
    CHARM = Selection("CharmSelFor"+name,
                      Algorithm=CombineParticles(DecayDescriptors = DDecays,
                                                 DaughtersCuts = CHARM_DaugCuts,
                                                 CombinationCut = CHARM_ComboCuts,
                                                 MotherCut = CHARM_MotherCuts),
                      RequiredSelections = CHARM_DAUGHTERS)

    USED_CHARM = CHARM
    if "D*" in BDecays:
        DST = makeDstar("CharmSelDstFor"+name, CHARM,CONFIG)
        USED_CHARM = DST

    B_combinationCut = "(AM<6.2*GeV)"
    B_motherCut = " (MM>%(BMassMin)s*GeV) & (MM<%(BMassMax)s*GeV) &  (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(B_DIRA)s)  " \
        "& (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > %(B_D_DZ)s *mm ) " %CONFIG
    if "ExtraComboCuts" in CONFIG.keys():
        B_combinationCut += CONFIG["ExtraComboCuts"]
    if "ExtraMotherCuts" in CONFIG.keys():
        B_motherCut += CONFIG["ExtraMotherCuts"]
        
    B_DaugCuts = {}
    if "ExtraMuonCuts" in CONFIG.keys():
        B_DaugCuts = {"mu+":CONFIG["ExtraMuonCuts"]}
    _B = CombineParticles(DecayDescriptors = BDecays,
                          DaughtersCuts = B_DaugCuts,
                          CombinationCut = B_combinationCut,
                          MotherCut = B_motherCut)
                     
    BSel = Selection ("BSelFor"+name,
                      Algorithm = _B,
                      RequiredSelections = [USED_CHARM, MUON])
    
    BSelTOS = TOSFilter( "BSelFor"+name+"TOS"
                         ,BSel
                         ,CONFIG["TTSpecs"])
    
    _prescale = 1.0
    if name in CONFIG["prescales"].keys():
        _prescale = CONFIG["prescales"][name]
    
    print ''
    print name
    print CHARM_DaugCuts
    print CHARM_ComboCuts
    print CHARM_MotherCuts
    print B_DaugCuts
    print B_combinationCut
    print B_motherCut
    
    return StrippingLine(name + module_name + 'Line', 
                         HLT = DEFAULT_HLT,
                         FILTER=DEFAULT_GECs,
                         prescale = _prescale,
                         selection = BSelTOS)

########### HELP WITH MAKING A DSTAR ########################
def makeDstar(_name, inputD0,CONFIG) : 
    _softPi = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
    _inputD0_conj = Selection("SelConjugateD0For"+_name,
                             Algorithm = ConjugateNeutralPID('ConjugateD0For'+_name),
                             RequiredSelections = [inputD0])
    _cutsSoftPi = '( PT > %(Dstar_SoftPion_PT)s *MeV )' % CONFIG
    _cutsDstarComb = '(AM - ACHILD(M,1) + 5 > %(Dstar_wideDMCutLower)s *MeV) & (AM - ACHILD(M,1) - 5 < %(Dstar_wideDMCutUpper)s *MeV)' % CONFIG
    _cutsDstarMoth_base = '(VFASPF(VCHI2/VDOF) < %(Dstar_Chi2)s )' % CONFIG
    _cutsDstarMoth_DM = '(M - CHILD(M,1) > %(Dstar_wideDMCutLower)s *MeV) & (M - CHILD(M,1) < %(Dstar_wideDMCutUpper)s *MeV)' % CONFIG
    _cutsDstarMoth = '(' + _cutsDstarMoth_base + ' & ' + _cutsDstarMoth_DM + ')'
    _Dstar = CombineParticles( DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc",
                               DaughtersCuts = { "pi+" : _cutsSoftPi },
                               CombinationCut = _cutsDstarComb,
                               MotherCut = _cutsDstarMoth)
    return Selection (name = "Sel"+_name,Algorithm = _Dstar,RequiredSelections = [inputD0,_inputD0_conj] + [_softPi])

########## TISTOS FILTERING ##################################
def TOSFilter( name = None, sel = None, trigger = None ):
    if len(trigger) == 0:
        return sel
    from Configurables import TisTosParticleTagger
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = trigger
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel



