__author__ = ['Mika Vesterinen','Liming Zhang, Alessandra Borgia']
__date__ = '23/07/2010'
__version__ = '$Revision: 5.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdLooseElectrons
from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons,StdNoPIDsProtons,StdNoPIDsMuons
from StrippingB2DMuNuXUtils import *
from GaudiKernel.SystemOfUnits import MeV, GeV, cm, mm

__all__ = ('B2DMuNuXAllLinesConf',
           'default_config')

default_config = {
    'B2DMuNuX' : {
        'WGs'         : ['Semileptonic'],
        'BUILDERTYPE' : 'B2DMuNuXAllLinesConf',
        'CONFIG'      : {
            "prescaleFakes": 0.02
            ,"prescales":{'StrippingB2DMuNuX_D0_Electron':0.1}
            ,"GEC_nLongTrk" : 250 
            ,"TTSpecs"      : {'Hlt1.*Track.*Decision%TOS':0,'Hlt2Topo.*Decision%TOS':0,'Hlt2SingleMuon.*Decision%TOS':0,"Hlt2Global%TIS":0}
            ,"TTSpecs"      : {}
            ,"HLT1"   : "HLT_PASS_RE('Hlt1.*Decision')"
            ,"HLT2"   : "HLT_PASS_RE('Hlt2(SingleMuon|Topo).*Decision')"
            ,"Monitor"      : False
            ,"UseNoPIDsInputs":False
            ,"TRGHOSTPROB"   : 0.35 
            ,"TRCHI2"        : 3.0  
            ,"MuonPIDmu"     : 0.0  
            ,"MuonPT"        : 1000*MeV
            ,"MuonIPCHI2"    : 9.00
            ,"MuonP"         : 6.0*GeV 
            ,"HadronPT"      : 250.0*MeV
            ,"HadronP"       : 2.0*GeV
            ,"HadronIPCHI2"  : 4.0 
            ,"ProtonPIDp"    : 0.0 
            ,"ProtonPIDpK"   : 0.0
            ,"ProtonP"       : 8.0*GeV
            ,"KaonPIDK"      : -2.0
            ,"KaonP"         : 2.0*GeV
            ,"PionPIDK"      : 20.0 
            ,"ElectronPIDe"  : 0.0  
            ,"ElectronPT"    : 1200*MeV
            ,"D_BPVDIRA"     : 0.99 
            ,"D_FDCHI2"      : 25.0 
            ,"D_AMassWin"    : 90.*MeV ## this should be 10 MeV wider than the widest D_MassWin
            ,"D_MassWin"     : {"default":80*MeV,
                                "Xic0": 60*MeV,
                                "Omegac": 60*MeV}
            ,"D_VCHI2DOF"    : 6.0 
            ,"D_DocaChi2Max" : 20  
            ,"B_DIRA"         : 0.999 
            ,"B_VCHI2DOF"      : 9.0  
            ,"B_D_DZ"         : -0.05*mm
            ,"B_MassMin"       : 2.2*GeV
            ,"B_MassMax"       : 8.0*GeV
            ,"B_DocaChi2Max"  : 10
            },
        'STREAMS'     : {
            'Semileptonic' : [
                'StrippingB2DMuNuX_D0_Electron',
                'StrippingB2DMuNuX_D0',
                'StrippingB2DMuNuX_Dp',
                'StrippingB2DMuNuX_Ds',
                'StrippingB2DMuNuX_Lc',
                'StrippingB2DMuNuX_Omegac',
                'StrippingB2DMuNuX_Xic',
                'StrippingB2DMuNuX_Xic0',
                'StrippingB2DMuNuX_D0_FakeMuon',
                'StrippingB2DMuNuX_Dp_FakeMuon',
                'StrippingB2DMuNuX_Ds_FakeMuon',
                'StrippingB2DMuNuX_Lc_FakeMuon',
                'StrippingB2DMuNuX_Omegac_FakeMuon',
                'StrippingB2DMuNuX_Xic_FakeMuon',
                'StrippingB2DMuNuX_Xic0_FakeMuon'],
            'Charm' : ['StrippingB2DMuNuX_D0_KK',
                       'StrippingB2DMuNuX_D0_PiPi'],
            }
        }
    }
class B2DMuNuXAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "prescaleFakes"
        ,"prescales"
        ,"GEC_nLongTrk"
        ,"TTSpecs"     
        ,"HLT1"   
        ,"HLT2"   
        ,"Monitor" 
        ,"UseNoPIDsInputs"
        ,"TRGHOSTPROB"   
        ,"TRCHI2"        
        ,"MuonPIDmu"     
        ,"MuonPT"        
        ,"MuonIPCHI2"    
        ,"MuonP"         
        ,"HadronPT"      
        ,"HadronP"       
        ,"HadronIPCHI2"  
        ,"ProtonPIDp"    
        ,"ProtonPIDpK"    
        ,"ProtonP"       
        ,"KaonPIDK"      
        ,"KaonP"         
        ,"PionPIDK"      
        ,"ElectronPIDe"  
        ,"ElectronPT"  
        ,"D_BPVDIRA"     
        ,"D_FDCHI2"      
        ,"D_AMassWin" 
        ,"D_MassWin"     
        ,"D_VCHI2DOF"    
        ,"D_DocaChi2Max" 
        ,"B_DIRA"        
        ,"B_VCHI2DOF"     
        ,"B_D_DZ"        
        ,"B_MassMin"      
        ,"B_MassMax"      
        ,"B_DocaChi2Max" 
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config

        ####################### BASIC FINAL STATE PARTICLE SELECTIONS ##########################
        
        self.HadronCuts = "(P>%(HadronP)s) & (PT > %(HadronPT)s )"\
            "& (TRCHI2DOF < %(TRCHI2)s)"\
            "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
            "& (MIPCHI2DV(PRIMARY)> %(HadronIPCHI2)s)" % config
      
        self.MuonTrackCuts = "(PT > %(MuonPT)s ) & (P> %(MuonP)s)"\
            "& (TRCHI2DOF < %(TRCHI2)s)"\
            "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
            "& (MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s)" %config
        
        inputs = {"muons":StdLooseMuons,
                  "pions":StdLoosePions,
                  "kaons":StdLooseKaons,
                  "protons":StdLooseProtons,
                  "electrons":StdLooseElectrons,
                  "fakemuons":StdNoPIDsMuons}
                          
        if config["UseNoPIDsInputs"] == True:
            inputs["muons"] = StdNoPIDsMuons
            inputs["pions"] = StdNoPIDsPions
            inputs["kaons"] = StdNoPIDsKaons
            inputs["protons"] = StdNoPIDsProtons

        self.cuts = {"muons":self.MuonTrackCuts + " & (PIDmu > %(MuonPIDmu)s)" % config,
                     "fakemuons":self.MuonTrackCuts + " & (INMUON) & (PIDmu < %(MuonPIDmu)s)" % config,  
                     "kaons":self.HadronCuts + " & (P>%(KaonP)s) & (PIDK> %(KaonPIDK)s)" % config,
                     "pions":self.HadronCuts + " & (PIDK< %(PionPIDK)s)" % config,
                     "protons":self.HadronCuts + "& (P>%(ProtonP)s)"\
                         "& (PIDp > %(ProtonPIDp)s) & (PIDp-PIDK > %(ProtonPIDpK)s)" % config,
                     "electrons":self.MuonTrackCuts + " & (PT> %(ElectronPT)s) & (PIDe > %(ElectronPIDe)s)" % config
                     }
                
        self.selMuon = Selection("Mufor"+name,
                                 Algorithm=FilterDesktop(Code=self.cuts["muons"]),
                                 RequiredSelections = [inputs["muons"]])
        
        self.selKaon = Selection( "Kfor" + name,
                                  Algorithm = FilterDesktop(Code=self.cuts["kaons"]),
                                  RequiredSelections = [inputs["kaons"]])
        
        self.selPion = Selection( "Pifor" + name,
                                  Algorithm = FilterDesktop(Code=self.cuts["pions"]),
                                  RequiredSelections = [inputs["pions"]])
        
        self.selProton = Selection( "ProtonsFor" + name,
                                    Algorithm = FilterDesktop(Code=self.cuts["protons"]),
                                    RequiredSelections = [inputs["protons"]])
        
        self.selElectron = Selection("efor"+name,
                                     Algorithm=FilterDesktop(Code=self.cuts["electrons"]),
                                     RequiredSelections = [inputs["electrons"]])
        
                                     
        self.selMuonFakes = Selection( "FakeMuonsFor" + name,
                                       Algorithm = FilterDesktop(Code = self.cuts["fakemuons"]),
                                       RequiredSelections = [inputs["fakemuons"]])
        
        ### Dictionary containing special cuts for each charm mode
        ### at this stage they are just the mass windows.
        ### All other cuts are the same for all charm species
        ### and will be specified later in the line building function
        CharmCuts = {}
        for mode in ["D0","Dp","Ds","Lc","Xic","Xic0","Omegac"]:
            CharmCuts[mode] = config.copy()
        
        # the Dp (->Kpipi) and Ds (->KKpi) are special
        # in that they each cover both the Ds and D+ mass peaks with the default sideband on either side.   
        CharmCuts["Dp"]["CharmComboCuts"] = "(DAMASS('D_s+') < %s )"\
            "& (DAMASS('D+')> -%s )" %(config["D_AMassWin"],config["D_AMassWin"])
        CharmCuts["Dp"]["CharmMotherCuts"] = "(DMASS('D_s+') < %s )"\
            "& (DMASS('D+')> -%s )" %(config["D_MassWin"]["default"],config["D_MassWin"]["default"])
        # and Ds and Dp are the same in this regard
        CharmCuts["Ds"] = CharmCuts["Dp"].copy()
        
        CharmCuts['D0']["CharmComboCuts"]  = "(ADAMASS('D0') < %s)" % config["D_AMassWin"]
        CharmCuts['D0']["CharmMotherCuts"] = "(ADMASS('D0')  < %s)" % config["D_MassWin"]["default"]
        
        CharmCuts["Lc"]["CharmComboCuts"] = "(ADAMASS('Lambda_c+') < %s )" %config["D_AMassWin"]
        CharmCuts["Lc"]["CharmMotherCuts"] = "(ADMASS('Lambda_c+') < %s )" %config["D_MassWin"]["default"]
        
        CharmCuts["Xic"]["CharmComboCuts"] = "(ADAMASS('Xi_c+') < %s )"  %config["D_AMassWin"]
        CharmCuts["Xic"]["CharmMotherCuts"] = "(ADMASS('Xi_c+') < %s )"  %config["D_MassWin"]["default"]
        
        CharmCuts["Xic0"]["CharmComboCuts"] = "(ADAMASS('Xi_c0') < %s )" %config["D_AMassWin"]
        CharmCuts["Xic0"]["CharmMotherCuts"] = "(ADMASS('Xi_c0') < %s )" %config["D_MassWin"]["Xic0"]
        
        CharmCuts["Omegac"]["CharmComboCuts"] = "(ADAMASS('Omega_c0') < %s )" %config["D_AMassWin"]
        CharmCuts["Omegac"]["CharmMotherCuts"] = "(ADMASS('Omega_c0') < %s )" %config["D_MassWin"]["Omegac"]

        self.b2D0MuXLine = BtoDlnuLine(name,
                                       'D0',
                                       ['[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                       ['[D0 -> K- pi+]cc'],
                                       CharmCuts["D0"],
                                       [self.selKaon, self.selPion],self.selMuon,self.selMuonFakes)
        
        self.b2D0eXLine = BtoDlnuLine(name,
                                      'D0_Electron',
                                      ['[B- -> D0 e-]cc'],
                                      ['[D0 -> K- pi+]cc'],
                                      CharmCuts["D0"],
                                      [self.selKaon, self.selPion],self.selElectron)
        
        self.b2D0MuXKKLine = BtoDlnuLine(name,
                                         'D0_KK',
                                         ['[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                         ['D0 -> K- K+'],
                                         CharmCuts["D0"],
                                         [self.selKaon],self.selMuon)
        
        self.b2D0MuXPiPiLine = BtoDlnuLine(name,
                                           'D0_PiPi',
                                           ['[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                           ['D0 -> pi- pi+'],
                                           CharmCuts["D0"],
                                           [self.selPion],self.selMuon)
        
        self.b2DpMuXLine = BtoDlnuLine(name,
                                       'Dp',
                                       [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                       [ '[D+ -> K- pi+ pi+]cc' ],
                                       CharmCuts["Dp"],
                                       [self.selKaon, self.selPion],self.selMuon,self.selMuonFakes)
        
        self.b2DsMuXLine = BtoDlnuLine(name,
                                       'Ds',
                                       [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                       [ '[D+ -> K+ K- pi+]cc' ],
                                       CharmCuts["Ds"],
                                       [self.selKaon, self.selPion],self.selMuon,self.selMuonFakes)
        
        
        self.lb2LcMuXLine = BtoDlnuLine(name,
                                        "Lc",
                                        [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                        [ '[Lambda_c+ -> K- p+ pi+]cc' ],
                                        CharmCuts["Lc"],
                                        [self.selProton,self.selKaon,self.selPion],self.selMuon,self.selMuonFakes)
        self.Xic_Line = BtoDlnuLine(name,
                                    "Xic",
                                    [ '[Xi_b0 -> Xi_c+ mu-]cc', '[Xi_b0 -> Xi_c+ mu+]cc'],
                                    [ '[Xi_c+ -> K- p+ pi+]cc' ],
                                    CharmCuts["Xic"],
                                    [self.selProton,self.selKaon,self.selPion],self.selMuon,self.selMuonFakes)

        self.Xic0_Line = BtoDlnuLine(name,
                                     "Xic0",
                                     [ '[Xi_b- -> Xi_c0 mu-]cc', '[Xi_b- -> Xi_c0 mu+]cc'],
                                     [ '[Xi_c0 -> p+ K- K- pi+]cc'],
                                     CharmCuts["Xic0"],
                                     [self.selProton,self.selKaon,self.selPion],self.selMuon,self.selMuonFakes)

        self.Omegac_Line = BtoDlnuLine(name,
                                       "Omegac",
                                       [ '[Omega_b- -> Omega_c0 mu-]cc', '[Omega_b- -> Omega_c0 mu+]cc'],
                                       [ '[Omega_c0 -> p+ K- K- pi+]cc' ],
                                       CharmCuts["Omegac"],
                                       [self.selProton,self.selKaon,self.selPion],self.selMuon,self.selMuonFakes)

        ##### line registration
        self.registerLine(self.b2D0eXLine)        
        self.registerLine(self.b2D0MuXKKLine)        
        self.registerLine(self.b2D0MuXPiPiLine)        
        for Mu in ["RealMuon","FakeMuon"]:
            self.registerLine(self.b2D0MuXLine[Mu])        
            self.registerLine(self.b2DpMuXLine[Mu])        
            self.registerLine(self.b2DsMuXLine[Mu])        
            self.registerLine(self.lb2LcMuXLine[Mu])        
            self.registerLine(self.Omegac_Line[Mu])        
            self.registerLine(self.Xic_Line[Mu])        
            self.registerLine(self.Xic0_Line[Mu])        
        

