__author__ = ['Kevin Maguire','Mika Vesterinen']
__date__ = '23/11/2015'
__version__ = '$Revision: 0.2 $'

"""
Module for D* tagged D0 -> hhpi0.
Supersedes StrippingDstarD2hhpi0
"""

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLoosePions, StdLoosePions, StdLooseKaons, StdLooseResolvedPi0, StdLooseMergedPi0
from Configurables import TisTosParticleTagger

__all__ = ('DstarD0ToHHPi0AllLinesBuilder',
           'DstarMaker',
           'TOSFilter',
           'default_config')

default_config = {
        'NAME': 'DstarD0ToHHPi0',
        'WGs'         : ['Charm'],
        'STREAMS':["Charm"],
        'BUILDERTYPE' : 'DstarD0ToHHPi0AllLinesBuilder',
        'CONFIG'      : {
            "prescale_Pi0R_WIDEMASS" : 1.0 #adimensional
            ,"Pi0R_PT" : 500  # MeV
            ,"Pi0R_DMASS" : 15  # MeV
            ,"Pi0M_PT" : 1000 #MeV
            ,"Pi0_MassConstraint" : False # adimensional
            ,"D0_M_WINDOW" : 150 #MeV
            ,"DELTA_MASS_MAX" : 180 #MeV
            ,"Pion_PIDK" : 0 #adimensional 
            ,"Kaon_PIDK" : 7 #adimensional 
            ,"Pion_PT" : 500 #MeV 
            ,"Kaon_PT" : 500 #MeV
            ,"Slowpion_PT" : 300 #MeV 
            ,"Pion_TRGHOSTPROB" : 0.35 #adimensional
            ,"Kaon_TRGHOSTPROB" : 0.35 #adimensional
            ,"Slowpion_TRGHOSTPROB" : 0.35 #adimensional
            ,"Slowpion_PIDe" : 5 #adimensional
            ,"D0_APT" : 1400  #MeV
            ,"useTOS" : True  #adimensional
            ,"useHLT2" : False  #adimensional
            ,"TOSFilter" : { 'Hlt2CharmHadInclDst2PiD02HHXBDTDecision%TOS' : 0 }  #adimensional
            ,"Hlt2Filter" : ""  #adimensional
            }
        }

class DstarD0ToHHPi0AllLinesBuilder(LineBuilder) :
    """
    """
    
    __configuration_keys__ = (
        "prescale_Pi0R_WIDEMASS" 
        ,"Pi0R_PT"
        ,"Pi0R_DMASS"
        ,"Pi0M_PT"
        ,"Pi0_MassConstraint"
        ,"D0_M_WINDOW"
        ,"DELTA_MASS_MAX"
        ,"Pion_PIDK"
        ,"Kaon_PIDK"
        ,"Pion_PT"
        ,"Kaon_PT"
        ,"Slowpion_PT"
        ,"Pion_TRGHOSTPROB"
        ,"Kaon_TRGHOSTPROB"
        ,"Slowpion_TRGHOSTPROB"
        ,"Slowpion_PIDe"
        ,"D0_APT"
        ,"useHLT2" 
        ,"useTOS"
        ,"TOSFilter"
        ,"Hlt2Filter"
        )
    
    __confdict__={}
    
    
    def __init__(self, _name, config) :

        LineBuilder.__init__(self, _name, config)
        self.__confdict__=config
        
        ### define the global event cuts
        ### max number of long tracks
        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 180)", 
                 "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        ### define the muon cuts                                                                                                       
        
        self.PionCuts = "(PT > %(Pion_PT)s *MeV) & (TRGHOSTPROB < %(Pion_TRGHOSTPROB)s) & (PIDK < %(Pion_PIDK)s)" %self.__confdict__ 

        self.KaonCuts = "(PT > %(Kaon_PT)s *MeV) & (TRGHOSTPROB < %(Kaon_TRGHOSTPROB)s) & (PIDK > %(Kaon_PIDK)s)" %self.__confdict__
        
        self.SlowpionCuts = "(PT > %(Slowpion_PT)s *MeV) & (TRGHOSTPROB < %(Slowpion_TRGHOSTPROB)s) & (PIDe < %(Slowpion_PIDe)s) & (MIPCHI2DV(PRIMARY)< 9.0)" %self.__confdict__
        
        self.Pi0RCuts = "(PT > %(Pi0R_PT)s *MeV) & (M > 135 - %(Pi0R_DMASS)s *MeV) & (M < 135 + %(Pi0R_DMASS)s *MeV)" %self.__confdict__                
        self.Pi0RWIDEMASSCuts = "(PT > %(Pi0R_PT)s *MeV)" %self.__confdict__ 
        
        self.Pi0MCuts = "(PT > %(Pi0R_PT)s *MeV)" %self.__confdict__

        self.Pions = Selection( "PionsFor" + _name, 
                                Algorithm = FilterDesktop(name = "PionFilterFor"+_name, Code = self.PionCuts ),
                                RequiredSelections = [StdLoosePions])
        self.Kaons = Selection( "KaonsFor" + _name,
                                Algorithm = FilterDesktop(name = "KaonFilterFor"+_name, Code = self.KaonCuts ),
                                RequiredSelections = [StdLooseKaons])
        self.Slowpions = Selection( "SlowpionsFor" + _name,
                                    Algorithm = FilterDesktop(name = "SlowpionFilterFor"+_name, Code = self.SlowpionCuts ),
                                    RequiredSelections = [StdAllLoosePions])
        
        self.Pi0R_WIDEMASS = Selection( "Pi0R_WIDEMASS_For" + _name,
                               Algorithm = FilterDesktop(name = "Pi0RFilterFor"+_name, Code = self.Pi0RWIDEMASSCuts ),
                               RequiredSelections = [StdLooseResolvedPi0])
        
        self.Pi0R = Selection( "Pi0RFor" + _name,
                               Algorithm = FilterDesktop(name = "Pi0RFilterFor"+_name, Code = self.Pi0RCuts ),
                               RequiredSelections = [StdLooseResolvedPi0])

        self.Pi0M = Selection( "Pi0MFor" + _name,
                               Algorithm = FilterDesktop(name = "Pi0MFilterFor"+_name, Code = self.Pi0MCuts ),
                               RequiredSelections = [StdLooseMergedPi0])
                
        self.Kpipi0_R_Line = DstarMaker(_name+"_Kpipi0_R_",
                                        ['[K*(892)0 -> K- pi+]cc','[K*(892)0 -> K+ pi-]cc'],                                                  
                                        ['[D0 -> K*(892)0 pi0]cc'],    
                                        ['[D*(2010)+ -> D0 pi+]cc'],
                                        [self.Pions,self.Kaons],
                                        self.Pi0R,
                                        self.Slowpions,
                                        GECs,
                                        self.__confdict__,
                                        1.0
                                        )

        self.Kpipi0_R_WIDEMASS_Line = DstarMaker(_name+"_Kpipi0_R_WIDEMASS_",
                                                 ['[K*(892)0 -> K- pi+]cc'],                                                  
                                                 ['[D0 -> K*(892)0 pi0]cc'],    
                                                 ['[D*(2010)+ -> D0 pi+]cc'],
                                                 [self.Pions,self.Kaons],
                                                 self.Pi0R_WIDEMASS,
                                                 self.Slowpions,
                                                 GECs,
                                                 self.__confdict__,
                                                 self.__confdict__['prescale_Pi0R_WIDEMASS']
                                                 )

        self.pipipi0_R_Line = DstarMaker(_name+"_pipipi0_R_",
                                         ['[K*(892)0 -> pi- pi+]cc'],                                                  
                                         ['[D0 -> K*(892)0 pi0]cc'],    
                                         ['[D*(2010)+ -> D0 pi+]cc'],
                                         [self.Pions],
                                         self.Pi0R,
                                         self.Slowpions,
                                         GECs,
                                         self.__confdict__,
                                         1.0
                                         )
        
        self.KKpi0_R_Line = DstarMaker(_name+"_KKpi0_R_",
                                       ['[K*(892)0 -> K- K+]cc'],                                                  
                                       ['[D0 -> K*(892)0 pi0]cc'],    
                                       ['[D*(2010)+ -> D0 pi+]cc'],
                                       [self.Kaons],
                                       self.Pi0R,
                                       self.Slowpions,
                                       GECs,
                                       self.__confdict__,
                                       1.0
                                       )
        
        self.Kpipi0_M_Line = DstarMaker(_name+"_Kpipi0_M_",
                                        ['[K*(892)0 -> K- pi+]cc','[K*(892)0 -> K+ pi-]cc'],                                                  
                                        ['[D0 -> K*(892)0 pi0]cc'],    
                                        ['[D*(2010)+ -> D0 pi+]cc'],
                                        [self.Pions,self.Kaons],
                                        self.Pi0M,
                                        self.Slowpions,
                                        GECs,
                                        self.__confdict__ ,
                                        1.0
                                        )
        
        self.pipipi0_M_Line = DstarMaker(_name+"_pipipi0_M_",
                                         ['[K*(892)0 -> pi- pi+]cc'],                                                  
                                         ['[D0 -> K*(892)0 pi0]cc'],    
                                         ['[D*(2010)+ -> D0 pi+]cc'],
                                         [self.Pions],
                                         self.Pi0M,
                                         self.Slowpions,
                                         GECs,
                                         self.__confdict__ ,
                                         1.0
                                         )

        self.KKpi0_M_Line = DstarMaker(_name+"_KKpi0_M_",
                                       ['[K*(892)0 -> K- K+]cc'],                                                  
                                       ['[D0 -> K*(892)0 pi0]cc'],    
                                       ['[D*(2010)+ -> D0 pi+]cc'],
                                       [self.Kaons],
                                       self.Pi0M,
                                       self.Slowpions,
                                       GECs,
                                       self.__confdict__ ,
                                       1.0
                                       )
        
        
        self.registerLine(self.Kpipi0_R_Line)
        self.registerLine(self.pipipi0_R_Line)
        self.registerLine(self.KKpi0_R_Line)
        self.registerLine(self.Kpipi0_M_Line)
        self.registerLine(self.pipipi0_M_Line)
        self.registerLine(self.KKpi0_M_Line)
        self.registerLine(self.Kpipi0_R_WIDEMASS_Line)
        
def DstarMaker(_name,_KstDecays,_D0Decays,_DstDecays,_ChargedTracks,_Pi0s,_Slowpions,_Filter,config,prescale):
    
    KstSel = Selection("SelKst"+_name,
                       Algorithm = CombineParticles( name = "CombKst"+_name,
                                                     DecayDescriptors = _KstDecays,
                                                     CombinationCut = "(((ACHILD(PT,1) > 1.7*GeV) & (ACHILD(BPVIPCHI2(),1) > 36)) | ((ACHILD(PT,2) > 1.7*GeV) & (ACHILD(BPVIPCHI2(),2) > 36))) & (AM < 1850*MeV) & (ADOCACHI2CUT(15,''))",
                                                     MotherCut = "(VFASPF(VCHI2/VDOF) < 3) & (BPVVDCHI2 > 100)"),
                       RequiredSelections = _ChargedTracks)  
    
    D0Comb = CombineParticles( name = "CombD0"+_name,
                               DecayDescriptors = _D0Decays,
                               CombinationCut = "(ADAMASS('D0') < %(D0_M_WINDOW)s + 10 *MeV) & (APT > %(D0_APT)s *MeV)" %config,
                               MotherCut = "(VFASPF(VCHI2/VDOF) < 20.0) & (DMASS('D0') < %(D0_M_WINDOW)s *MeV)" %config)
    
    D0Sel = Selection("SelD0"+_name,
                      Algorithm =  D0Comb,
                      RequiredSelections = [KstSel,_Pi0s]) 
    
    DstComb = CombineParticles( name = "CombDst"+_name,
                                DecayDescriptors = _DstDecays,
                                CombinationCut = "(AM - ACHILD(M,1) < %(DELTA_MASS_MAX)s+5 *MeV) & (ADOCACHI2CUT(20,''))" %config,
                                MotherCut = "(M - CHILD(M,1) < %(DELTA_MASS_MAX)s *MeV) & (VFASPF(VCHI2/VDOF) < 9.0)" %config)
    if config["Pi0_MassConstraint"] == True:
        DstComb.addTool( OfflineVertexFitter )                                                                                                  
        DstComb.ParticleCombiners.update( {"" : "OfflineVertexFitter"} )                                                                                    
        DstComb.OfflineVertexFitter.useResonanceVertex = False
        DstComb.OfflineVertexFitter.applyDauMassConstraint = True                                                                                      
        DstComb.ReFitPVs = True   
        
    DstSel = Selection("SelDst"+_name,
                       Algorithm = DstComb,
                       RequiredSelections = [D0Sel,_Slowpions])
    
    _tosFilter = config['TOSFilter']
    DstSelTOS = TOSFilter( "SelDstKPiPi0_Hlt2TOS"+_name
                           ,DstSel
                           ,_tosFilter)
    
    hlt2 = ""
    if config["useHLT2"] == True:
        _hlt2 =  "%(Hlt2Filter)s" %config
        hlt2 = _hlt2
        
    if config["useTOS"] == True:
        Line = StrippingLine(_name+'Line',
                             prescale = prescale,
                             FILTER=_Filter,
                             HLT2 = hlt2,
                             selection = DstSelTOS) 
    else:
        Line = StrippingLine(_name+'Line',
                             prescale = prescale,
                             FILTER=_Filter,
                             HLT2 = hlt2,
                             selection = DstSel) 
    return Line
                                                    
def TOSFilter( name, _input, _specs ) :

    from Configurables import TisTosParticleTagger
    _tisTosFilter = TisTosParticleTagger( name + "Tagger" )
    _tisTosFilter.TisTosSpecs = _specs 
    return Selection( name
                      , Algorithm = _tisTosFilter
                      , RequiredSelections = [ _input ]
                      )
