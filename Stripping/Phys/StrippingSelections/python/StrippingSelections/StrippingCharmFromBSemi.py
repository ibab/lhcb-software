"""
Module for selecting charm decays from semileptonic B decays.
Following decays are included:
* D0->KsHH (also Dstar)
* D0->Pi0HH (also Dstar)
* D0->hhhh (also Dstar)
* D0->KsKs (also Dstar)
* D+->KsH
* D+->HMuNu
* Lc+ -> L0 H
* Lc+ -> p HH
Heavily based on StrippingB2DMuNuX by Alessandra Borgia and Liming Zhang
Dstar methods closely copied from StrippingDstarD2KShh.py by Mat Charles.
"""
__author__ = ['Mika Vesterinen']
__date__ = '08/03/2012'
__version__ = '$Revision: 0.7 $'

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdLooseMergedPi0,StdLooseResolvedPi0
from Configurables import ConjugateNeutralPID
from PhysSelPython.Wrappers import MergedSelection

__all__ = ('CharmFromBSemiAllLinesConf',
           'makeb2DMuX',
           'makeb2DX',
           'makeDstar',
           'TOSFilter',
           'confdict')

confdict = {
    "GEC_nLongTrk"   : 250    # adimensional
    ,"TrGhostProbMax": 0.5    # adimensional
    ,"MinBMass"      : 2500   # MeV
    ,"MaxBMass"      : 6000   # MeV
    ,"MinBMassTight" : 2800   # MeV
    ,"MaxBMassTight" : 5100   # MeV
    ,"MINIPCHI2"     : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PionPIDKTight" : 4.0    # adimensiional
    ,"MuonIPCHI2"    : 4.0    # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensiional
    ,"DsFDCHI2"      : 100.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"Dto4h_MassWin" : 40.0   # MeV
    ,"Dto4h_AMassWin": 42.0   # MeV
    ,"DsIP"          : 7.4    # mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"PIDmu"         : -0.0   # adimensiional
    ,"BDIRA"         : 0.999  # adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"DZ"            : 0      #mm
    ,"DDocaChi2Max"  : 20     #adimensiional
    ,"MINIPCHI2Loose": 4.0   #adimensiional
    ,"KaonPIDKloose" : -5     #adimensiional
    ,'KSCutZFDFromD' :   10.0  #mm
    ,'KSLLPMin'         : 2000  ## MeV
    ,'KSLLPTMin'        : 400   ## MeV
    ,'KSDDPMin'         : 3000  ## MeV
    ,'KSDDPTMin'        : 500   ## MeV
    ,'KSLLCutMass'      : 30    ## MeV
    ,'KSDDCutMass'      : 30    ## MeV
    ,'KSLLCutFDChi2'    : 100   ## unitless
    ,'KSDDCutFDChi2'    : 100   ## unitless
    ,'KSDaugTrackChi2'  : 4     ## max chi2/ndf for Ks daughter tracks
    ,'KSVertexChi2'     : 6     ## max chi2/ndf for Ks vertex
    ,'KSCutDIRA'        : 0.99  ## unitless
    ,'LambdaLLPMin'       : 2000 ## MeV
    ,'LambdaLLPTMin'      : 500  ## MeV
    ,'LambdaLLCutMass'    : 30   ## MeV
    ,'LambdaLLCutFDChi2'  : 100  ## unitless
    ,'LambdaDDPMin'       : 3000 ## MeV
    ,'LambdaDDPTMin'      : 800  ## MeV
    ,'LambdaDDCutMass'    : 30   ## MeV
    ,'LambdaDDCutFDChi2'  : 100  ## unitless
    ,'LambdaCutDIRA'      : 0.99 ## unitless
    ,'LambdaDaugTrackChi2': 4    ## unitless
    ,'LambdaVertexChi2'   : 6    ## max chi2/ndf for Lambda0 vertex
    ,"Pi0PtMinLoose"    : 1000    # Minimum Pt of pi0 (MeV)
    ,"Pi0PtMin"         : 1300   # Minimum Pt of pi0 (MeV)
    ,"Pi0PMin"          : 3000   # Minimum P of pi0 (MeV)
    ,"PhotonCL"         : 0.25   # Confidence level for Pi0 photons
    ,"D02HHPi0AMassWin" : 220  # MeV (mass window for combination)
    ,"D02HHPi0MassWin"  : 210  # MeV (mass window after vertex fit)
    ,"D02HHPi0DocaCut"  : 6.0    # mm
    ,"Dstar_Chi2"       :  8.0 ## unitless
    ,"Dstar_SoftPion_PIDe" : 2. ## unitless
    ,"Dstar_SoftPion_PT" : 180. ## MeV ### 
    ,"Dstar_wideDMCutLower" : 0. ## MeV
    ,"Dstar_wideDMCutUpper" : 170. ## MeV
    ,"PTSUMLoose"  : 1400. ## MeV
    ,"PTSUM"       : 1800. ## MeV
    ,"PTSUM_HHPi0" : 1800. ## MeV
    }

class CharmFromBSemiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "GEC_nLongTrk"
        ,"TrGhostProbMax"
        ,"MinBMass"      
        ,"MaxBMass"      
        ,"MinBMassTight" 
        ,"MaxBMassTight" 
        ,"MINIPCHI2"     
        ,"TRCHI2"     
        ,"KaonPIDK"      
        ,"PionPIDK"
        ,"PionPIDKTight"      
        ,"MuonIPCHI2"    
        ,"MuonPT"        
        ,"KPiPT"
        ,"DsDIRA"        
        ,"DsFDCHI2"      
        ,"DsMassWin"     
        ,"DsAMassWin"    
        ,"Dto4h_MassWin"     
        ,"Dto4h_AMassWin"    
        ,"DsIP"          
        ,"DsVCHI2DOF"    
        ,"PIDmu"         
        ,"BDIRA"         
        ,"BVCHI2DOF"     
        ,"DZ"
        ,"DDocaChi2Max"
        ,"MINIPCHI2Loose"
        ,"KaonPIDKloose"
        ,'KSCutZFDFromD'
        ,'KSLLPMin'   
        ,'KSLLPTMin' 
        ,'KSDDPMin'  
        ,'KSDDPTMin'   
        ,'KSLLCutMass'    
        ,'KSDDCutMass'    
        ,'KSLLCutFDChi2'
        ,'KSDDCutFDChi2'   
        ,'KSDaugTrackChi2'
        ,'KSVertexChi2'
        ,'KSCutDIRA'
        ,'LambdaLLPMin'       
        ,'LambdaLLPTMin'      
        ,'LambdaLLCutMass'    
        ,'LambdaLLCutFDChi2'  
        ,'LambdaDDPMin'       
        ,'LambdaDDPTMin'      
        ,'LambdaDDCutMass'    
        ,'LambdaDDCutFDChi2'  
        ,'LambdaCutDIRA'      
        ,'LambdaDaugTrackChi2'
        ,'LambdaVertexChi2'   
        ,"Pi0PtMin"
        ,"Pi0PtMinLoose" 
        ,"Pi0PMin" 
        ,"PhotonCL"
        ,"D02HHPi0AMassWin"
        ,"D02HHPi0MassWin"
        ,"D02HHPi0DocaCut"
        ,"Dstar_Chi2"
        ,"Dstar_SoftPion_PIDe"
        ,"Dstar_SoftPion_PT"
        ,"Dstar_wideDMCutLower"
        ,"Dstar_wideDMCutUpper"
        ,"PTSUMLoose"       
        ,"PTSUM"       
        ,"PTSUM_HHPi0" 
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        Dstar_cuts = {
            "Dstar_Chi2" : config["Dstar_Chi2"],
            "Dstar_SoftPion_PIDe" : config["Dstar_SoftPion_PIDe"],
            "Dstar_SoftPion_PT" : config["Dstar_SoftPion_PT"],
            "Dstar_wideDMCutLower" : config["Dstar_wideDMCutLower"],
            "Dstar_wideDMCutUpper" : config["Dstar_wideDMCutUpper"]
            }
        BCuts = {'BVCHI2DOF': config['BVCHI2DOF'],
                 'BDIRA': config['BDIRA'],
                 'MinBMass': config['MinBMass'],
                 'MaxBMass': config['MaxBMass'],
                 'DZ': config['DZ']
                 }
        BCutsKsKs = {'BVCHI2DOF': config['BVCHI2DOF'],
                     'BDIRA': config['BDIRA'],
                     'MinBMass': config['MinBMass'],
                     'MaxBMass': config['MaxBMass'],
                     'DZ': -9999} ### no DZ cuts for KsKs.
        BCutsTight = {'BVCHI2DOF': config['BVCHI2DOF'],
                      'BDIRA': config['BDIRA'],
                      'MinBMass': config['MinBMassTight'],
                      'MaxBMass': config['MaxBMassTight'],
                      'DZ': config['DZ']
                      }
        
        ############### MUON SELECTIONS ###################
        
        self.selmuon = Selection( "SelMufor" + name,
                                  Algorithm = self._muonFilter("Mufor"+name),
                                  RequiredSelections = [StdLooseMuons])
        
        self.selmuonhighPT = Selection( "SelMuhighPTfor" + name,
                                        Algorithm = FilterDesktop(name = "MuhighPTfor"+name,
                                                                  Code = "(TRCHI2DOF < %(TRCHI2)s)"\
                                                                  "& (PT>1.2*GeV) & (MIPCHI2DV(PRIMARY)> 9.0)" % self.__confdict__ ),
                                        RequiredSelections = [self.selmuon])
        
        self.selmuontight = Selection( "SelMutightfor" + name,
                                       Algorithm = FilterDesktop( name = "Mutightfor"+name,
                                                                  Code = "(MIPCHI2DV(PRIMARY)> 100)" ),
                                       RequiredSelections = [self.selmuonhighPT])
        
        self.selmuonnew = Selection( "SelMunewfor" + name,
                                     Algorithm = FilterDesktop( name = "Munewfor"+name,
                                                                Code = "(MIPCHI2DV(PRIMARY)> 9.0)"\
                                                                "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__ ),
                                     RequiredSelections = [self.selmuon])
        
        self.selmuonTOS = TOSFilter( "SelMuTOS" + name,
                                     self.selmuontight,
                                     "Hlt2SingleMuonDecision")

        self.selmuonL0TOS = TOSFilter( "SelMuL0TOS" + name,
                                       self.selmuon,
                                       "L0MuonDecision")
        
        ############### KAON AND PION SELECTIONS ################
        
        self.selKaon = Selection( "SelKfor" + name,
                                  Algorithm = self._kaonFilter("Kfor"+name),
                                  RequiredSelections = [StdLooseKaons])
        
        self.selPion = Selection( "SelPifor" + name,
                                  Algorithm = self._pionFilter("Pifor"+name),
                                  RequiredSelections = [StdLoosePions])

        self.selProton = Selection( "SelProtonfor" + name,
                                  Algorithm = self._protonFilter("Protonfor"+name),
                                  RequiredSelections = [StdLooseProtons])
        
        self.selPionTight = Selection( "SelPiTightfor" + name,
                                       Algorithm = FilterDesktop( name = "PiTightFor"+name,
                                                                  Code = "(TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                                                                  "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDKTight)s)" % self.__confdict__ ),
                                       RequiredSelections = [StdLoosePions])
        
        self.selPionloose = Selection( "SelPiloosefor" + name,
                                       Algorithm = self._pionlooseFilter("Piloosefor"+name),
                                       RequiredSelections = [StdNoPIDsPions])
        
        self.selKaonloose = Selection( "SelKloosefor" + name,
                                       Algorithm = self._kaonlooseFilter("Kloosefor"+name),
                                       RequiredSelections = [StdLooseKaons])
        
        ############## PI0 SELECTIONS ############################

        self.selPi0ResolvedLoose = Selection( "SelPi0ResolvedLoosefor" + name,
                                         Algorithm = self._Pi0ResolvedLooseFilter("Pi0ResolvedLoosefor"+name),
                                         RequiredSelections = [StdLooseResolvedPi0])

        self.selPi0Resolved = Selection( "SelPi0Resolvedfor" + name,
                                         Algorithm = self._Pi0ResolvedFilter("Pi0Resolvedfor"+name),
                                         RequiredSelections = [StdLooseResolvedPi0])
        
        self.selPi0Merged = Selection( "SelPi0Mergedfor" + name,
                                         Algorithm = self._Pi0MergedFilter("PiMergedfor"+name),
                                         RequiredSelections = [StdLooseMergedPi0])

        ############## SLOW PION FOR D* TAG ########################
        
        self.selSlowPion = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
        self.selSlowPionUp = DataOnDemand(Location = 'Phys/StdLooseANNUpPions/Particles')
        self.selSlowPionAll = MergedSelection("AllSlowPionfor", RequiredSelections = [self.selSlowPion,self.selSlowPionUp])

        ############## KS0 SELECTIONS ##############################
        
        _stdLooseKsLL = DataOnDemand("Phys/StdLooseKsLL/Particles")
        _stdLooseKsDD = DataOnDemand("Phys/StdLooseKsDD/Particles")
        
        self.selKSLL = Selection("SelKsLLfor"+name,
                                 Algorithm = self._KsLLFilter("KsLLfor"+name),
                                 RequiredSelections = [_stdLooseKsLL])

        self.selKSDD = Selection("SelKsDDfor"+name,
                                 Algorithm = self._KsDDFilter("KsDDfor"+name),
                                 RequiredSelections = [_stdLooseKsDD])

        ############# LAMBDA0 SELECTIONS ##########################
        
        _stdLooseLambdaLL = DataOnDemand("Phys/StdLooseLambdaLL/Particles")
        _stdLooseLambdaDD = DataOnDemand("Phys/StdLooseLambdaDD/Particles")

        self.selLambdaLL = Selection("SelLambdaLLfor"+name,
                                 Algorithm = self._LambdaLLFilter("LambdaLLfor"+name),
                                 RequiredSelections = [_stdLooseLambdaLL])

        self.selLambdaDD = Selection("SelLambdaDDfor"+name,
                                 Algorithm = self._LambdaDDFilter("LambdaDDfor"+name),
                                 RequiredSelections = [_stdLooseLambdaDD])

        ################ D0 -> KsKs SELECTION #############################
        
        self.sel_D0_to_KsKs_LLLL = Selection('SelD02KsKsLLLLfor'+name,
                                             Algorithm = self._D02KsKsFilter(['D0 -> KS0 KS0'],'D02KsKsLLLLfor'+name),
                                             RequiredSelections = [self.selKSLL])
        self.sel_Dstar_to_KsKs_LLLL = makeDstar('Dstar_KsKs_LLLLFor'+name,self.sel_D0_to_KsKs_LLLL,self.selSlowPion,Dstar_cuts)
        
        self.sel_D0_to_KsKs_DDDD = Selection('SelD02KsKsDDDDfor'+name,
                                             Algorithm = self._D02KsKsFilter(['D0 -> KS0 KS0'],'D02KsKsDDDDfor'+name),
                                             RequiredSelections = [self.selKSDD])
        self.sel_Dstar_to_KsKs_DDDD = makeDstar('Dstar_KsKs_DDDDFor'+name,self.sel_D0_to_KsKs_DDDD,self.selSlowPion,Dstar_cuts)
        
        self.sel_D0_to_KsKs_DDLL = Selection('SelD02KsKsDDLLfor'+name,
                                             Algorithm = self._D02KsKsFilter(['D0 -> KS0 KS0'],'D02KsKsDDLLfor'+name),
                                             RequiredSelections = [self.selKSLL,self.selKSDD])
        self.sel_Dstar_to_KsKs_DDLL = makeDstar('Dstar_KsKs_DDLLFor'+name,self.sel_D0_to_KsKs_DDLL,self.selSlowPion,Dstar_cuts)

        ################ D0 -> 4H SELECTION ##########################

        self.sel_D0_to_4Pi = Selection( 'Sel_D0_to_4Pi_for' + name,
                                        Algorithm = self._D02HHHHFilter(['D0 -> pi- pi+ pi- pi+'],'D0_to_4Pi_for' + name),
                                        RequiredSelections = [self.selPion] )        
        self.selDstar_to_4Pi = makeDstar('Dstar_4PiFor'+name,self.sel_D0_to_4Pi,self.selSlowPion,Dstar_cuts)


        self.sel_D0_to_K3Pi = Selection( 'Sel_D0_to_K3Pi_for' + name,
                                         Algorithm = self._D02HHHHFilter(['[D0 -> K- pi+ pi- pi+]cc'],'D0_to_K3Pi_for' + name),
                                         RequiredSelections = [self.selPion,self.selKaon] )        
        self.selDstar_to_K3Pi = makeDstar('Dstar_K3PiFor'+name,self.sel_D0_to_K3Pi,self.selSlowPion,Dstar_cuts)


        self.sel_D0_to_2K2Pi = Selection( 'Sel_D0_to_2K2Pi_for' + name,
                                          Algorithm = self._D02HHHHFilter(['D0 -> K- K+ pi- pi+'],'D0_to_2K2Pi_for' + name),
                                          RequiredSelections = [self.selPion,self.selKaon] )        
        self.selDstar_to_2K2Pi = makeDstar('Dstar_2K2PiFor'+name,self.sel_D0_to_2K2Pi,self.selSlowPion,Dstar_cuts)
        
        self.sel_D0_to_3KPi = Selection( 'Sel_D0_to_3KPi_for' + name,
                                         Algorithm = self._D02HHHHFilter(['[D0 -> K+ K- K- pi+]cc'],'D0_to_3KPi_for' + name),
                                         RequiredSelections = [self.selPion,self.selKaon] )        
        self.selDstar_to_3KPi = makeDstar('Dstar_3KPiFor'+name,self.sel_D0_to_3KPi,self.selSlowPion,Dstar_cuts)
        
        ################## D0 -> Ks HH SELECTIONS ##############################

        ### ks Pi Pi 
        self.seld02KsPiPiLL = Selection( 'SelD02KsPiPiLLfor' + name,
                                         Algorithm = self._D02KsHHFilter(['D0 -> KS0 pi+ pi-'],'D02KsPiPiLLfor' + name),
                                         RequiredSelections = [self.selKSLL,self.selPionloose] )           
        self.selDstar_2KsPiPiLL = makeDstar('DstarPiPiLLFor'+name,self.seld02KsPiPiLL,self.selSlowPion,Dstar_cuts)

        self.seld02KsPiPiDD = Selection( 'SelD02KsPiPiDDfor' + name,
                                         Algorithm = self._D02KsHHFilter(['D0 -> KS0 pi+ pi-'],'D02KsPiPiDDfor' + name),
                                         RequiredSelections = [self.selKSDD,self.selPionloose] )           
        self.selDstar_2KsPiPiDD = makeDstar('DstarPiPiDDFor'+name,self.seld02KsPiPiDD,self.selSlowPion,Dstar_cuts)

        ### ks K K 
        self.seld02KsKKLL = Selection( 'SelD02KsKKLLfor' + name,
                                       Algorithm = self._D02KsHHFilter(['D0 -> KS0 K+ K-'],'D02KsKKLLfor' + name),
                                       RequiredSelections = [self.selKSLL,self.selKaonloose] )           
        self.selDstar_2KsKKLL = makeDstar('DstarKKLLFor'+name,self.seld02KsKKLL,self.selSlowPion,Dstar_cuts)

        self.seld02KsKKDD = Selection( 'SelD02KsKKDDfor' + name,
                                       Algorithm = self._D02KsHHFilter(['D0 -> KS0 K+ K-'],'D02KsKKDDfor' + name),
                                       RequiredSelections = [self.selKSDD,self.selKaonloose] )           
        self.selDstar_2KsKKDD = makeDstar('DstarKKDDFor'+name,self.seld02KsKKDD,self.selSlowPion,Dstar_cuts)

        ### ks K Pi 
        self.seld02KsKPiLL = Selection( 'SelD02KsKPiLLfor' + name,
                                        Algorithm = self._D02KsHHFilter(['[D0 -> KS0 K- pi+]cc','[D0 -> KS0 K+ pi-]cc'],'D02KsKPiLLfor' + name),
                                        RequiredSelections = [self.selKSLL,self.selPionloose,self.selKaonloose] )           
        self.selDstar_2KsKPiLL = makeDstar('DstarKPiLLFor'+name,self.seld02KsKPiLL,self.selSlowPion,Dstar_cuts)
        
        self.seld02KsKPiDD = Selection( 'SelD02KsKPiDDfor' + name,
                                        Algorithm = self._D02KsHHFilter(['[D0 -> KS0 K- pi+]cc','[D0 -> KS0 K+ pi-]cc'],'D02KsKPiDDfor' + name),
                                        RequiredSelections = [self.selKSDD,self.selPionloose,self.selKaonloose] )           
        self.selDstar_2KsKPiDD = makeDstar('DstarKPiDDFor'+name,self.seld02KsKPiDD,self.selSlowPion,Dstar_cuts)

        ################## D0 -> HHPi0 WITH MERGED PI0 #######################

        self.seld02KPiPi0Merged = self._D02HHPi0Filter("D02KPiPi0Mergedfor"+name,["[D0 -> K- pi+ pi0]cc"],[self.selKaon,self.selPion,self.selPi0Merged])
        self.selDstar_2KPiPi0Merged = makeDstar('DstarKPiPi0MergedFor'+name,self.seld02KPiPi0Merged,self.selSlowPion,Dstar_cuts)
        self.selDstar_2KPiPi0MergedUp = makeDstar('DstarKPiPi0MergedUpFor'+name,self.seld02KPiPi0Merged,self.selSlowPionAll,Dstar_cuts)

        self.seld02KKPi0Merged = self._D02HHPi0Filter("D02KKPi0Mergedfor"+name,["D0 -> K- K+ pi0"],[self.selKaon,self.selPi0Merged])
        self.selDstar_2KKPi0Merged = makeDstar('DstarKKPi0MergedFor'+name,self.seld02KKPi0Merged,self.selSlowPion,Dstar_cuts)

        self.seld02PiPiPi0Merged = self._D02HHPi0Filter("D02PiPiPi0Mergedfor"+name,["D0 -> pi- pi+ pi0"],[self.selPion,self.selPi0Merged])
        self.selDstar_2PiPiPi0Merged = makeDstar('DstarPiPiPi0MergedFor'+name,self.seld02PiPiPi0Merged,self.selSlowPion,Dstar_cuts)
        
        ################## D0 -> HHPi0 WITH RESOLVED PI0 #######################

        self.seld02KPiPi0Resolved = self._D02HHPi0Filter("D02KPiPi0Resolvedfor"+name,
                                                            ["[D0 -> K- pi+ pi0]cc"],[self.selKaon,self.selPion,self.selPi0Resolved])
        self.seld02KPiPi0ResolvedLoose = self._D02HHPi0Filter("D02KPiPi0ResolvedLoosefor"+name,
                                                              ["[D0 -> K- pi+ pi0]cc"],[self.selKaon,self.selPion,self.selPi0ResolvedLoose])
        self.selDstar_2KPiPi0Resolved = makeDstar('DstarKPiPi0ResolvedFor'+name,
                                                     self.seld02KPiPi0ResolvedLoose,self.selSlowPion,Dstar_cuts)
        self.selDstar_2KPiPi0ResolvedUp = makeDstar('DstarKPiPi0ResolvedUpFor'+name,
                                                     self.seld02KPiPi0ResolvedLoose,self.selSlowPionAll,Dstar_cuts)

        self.seld02KKPi0Resolved = self._D02HHPi0Filter("D02KKPi0Resolvedfor"+name,
                                                            ["D0 -> K- K+ pi0"],[self.selKaon,self.selPi0Resolved])
        self.seld02KKPi0ResolvedLoose = self._D02HHPi0Filter("D02KKPi0ResolvedLoosefor"+name,
                                                                 ["D0 -> K- K+ pi0"],[self.selKaon,self.selPi0ResolvedLoose])
        self.selDstar_2KKPi0Resolved = makeDstar('DstarKKPi0ResolvedFor'+name,
                                                     self.seld02KKPi0ResolvedLoose,self.selSlowPion,Dstar_cuts)

        self.seld02PiPiPi0Resolved = self._D02HHPi0Filter("D02PiPiPi0Resolvedfor"+name,
                                                             ["[D0 -> pi- pi+ pi0]cc"],[self.selPion,self.selPi0Resolved])
        self.seld02PiPiPi0ResolvedLoose = self._D02HHPi0Filter("D02PiPiPi0ResolvedLoosefor"+name,
                                                                  ["D0 -> pi- pi+ pi0"],[self.selPion,self.selPi0ResolvedLoose])
        self.selDstar_2PiPiPi0Resolved = makeDstar('DstarPiPiPi0ResolvedFor'+name,
                                                      self.seld02PiPiPi0ResolvedLoose,self.selSlowPion,Dstar_cuts)
        
        self.seld02KKPi0SSResolved = self._D02HHPi0Filter("D02KKPi0SSResolvedfor"+name,
                                                             ["[D0 -> K+ K+ pi0]cc"],[self.selKaon,self.selPi0Resolved])
        self.seld02KKPi0SSResolvedLoose = self._D02HHPi0Filter("D02KKPi0SSResolvedLoosefor"+name,
                                                                  ["[D0 -> K+ K+ pi0]cc"],[self.selKaon,self.selPi0ResolvedLoose])
        self.selDstar_2KKPi0SSResolved = makeDstar('DstarKKPi0SSResolvedFor'+name,
                                                      self.seld02KKPi0SSResolvedLoose,self.selSlowPion,Dstar_cuts)
        
        self.seld02PiPiPi0SSResolved = self._D02HHPi0Filter("D02PiPiPi0SSResolvedfor"+name,
                                                               ["[D0 -> pi+ pi+ pi0]cc"],[self.selPion,self.selPi0Resolved])
        self.seld02PiPiPi0SSResolvedLoose = self._D02HHPi0Filter("D02PiPiPi0SSResolvedLoosefor"+name,
                                                                    ["[D0 -> pi+ pi+ pi0]cc"],[self.selPion,self.selPi0ResolvedLoose])
        self.selDstar_2PiPiPi0SSResolved = makeDstar('DstarPiPiPi0SSResolvedFor'+name,
                                                     self.seld02PiPiPi0SSResolvedLoose,self.selSlowPion,Dstar_cuts)

        #################### D -> HHH SELECTIONS ##########################

        self.selds2KKPiDCS = self._D2HHHFilter('Ds2KKPiDCSfor'+name,['[D+ -> K+ K+ pi-]cc'],[self.selKaon,self.selPion])
        
        ################### D+/Ds+ -> Ks H SELECTIONS ######################

        self.selds2KsLLK = Selection( 'SelDs2KsLLKfor' + name,
                                      Algorithm = self._Ds2KsHFilter(['[D+ -> KS0 K+]cc'],'Ds2KsLLKfor' + name),
                                      RequiredSelections = [self.selKaon, self.selKSLL] )

        self.selds2KsDDK = Selection( 'SelDs2KsDDKfor' + name,
                                      Algorithm = self._Ds2KsHFilter(['[D+ -> KS0 K+]cc'],'Ds2KsDDKfor' + name),
                                      RequiredSelections = [self.selKaon, self.selKSDD] )

        self.selds2KsLLPi = Selection( 'SelDs2KsLLPifor' + name,
                                       Algorithm = self._Ds2KsHFilter(['[D+ -> KS0 pi+]cc'],'Ds2KsLLPifor' + name),
                                       RequiredSelections = [self.selPion, self.selKSLL] )
        
        self.selds2KsDDPi = Selection( 'SelDs2KsDDPifor' + name,
                                       Algorithm = self._Ds2KsHFilter(['[D+ -> KS0 pi+]cc'],'Ds2KsDDPifor' + name),
                                       RequiredSelections = [self.selPion, self.selKSDD])
        
        ################### D -> Ks KKpi SELECTION ############################
        
        self.selD2KsKKPiDD = self._D2KsHHHFilter("D2KsKKPiDDfor"+name,['[D+ -> KS0 K+ K- pi+]cc'],[self.selKSDD,self.selKaon,self.selPion])
        self.selD2KsKKPiLL = self._D2KsHHHFilter("D2KsKKPiLLfor"+name,['[D+ -> KS0 K+ K- pi+]cc'],[self.selKSLL,self.selKaon,self.selPion])
                                                 
        BDecays = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ]
        MuSel = self.selmuon
        self.selb2DpMuXKsKKPiLL = makeb2DMuX('b2DpMuXKsKKPiLL'+name,BDecays,MuSel,self.selD2KsKKPiLL,BCuts)
        self.selb2DpMuXKsKKPiDD = makeb2DMuX('b2DpMuXKsKKPiDD'+name,BDecays,MuSel,self.selD2KsKKPiDD,BCuts)
        
        ################## D+/Ds+ -> H mu mu SELECTIONS ########################

        self.selds2pimumu = Selection( 'SelDs2PiMuMufor' + name,
                                       Algorithm = self._Ds2HMuMuFilter([ '[D+ -> pi+ mu+ mu-]cc' ],'Ds2PiMuMufor' + name),
                                       RequiredSelections = [self.selPion,self.selmuon] )
        
        self.selds2kmumu = Selection( 'SelDs2KMuMufor' + name,
                                      Algorithm = self._Ds2HMuMuFilter([ '[D+ -> K+ mu+ mu-]cc' ],'Ds2KMuMufor' + name),
                                      RequiredSelections = [self.selKaon,self.selmuon] )

        #################### Lambda_c+ -> X SELECTIONS #########################

        self.selLc2L0Pi_DD = Selection( 'SelLc2LambdaDDPifor' + name,
                                        Algorithm = self._Lc2L0HFilter([ '[Lambda_c+ -> Lambda0 pi+]cc' ],'Lc2LambdaDDPifor' + name),
                                        RequiredSelections = [self.selPionTight, self.selLambdaDD])
        
        self.selLc2L0Pi_LL = Selection( 'SelLc2LambdaLLPifor' + name,
                                        Algorithm = self._Lc2L0HFilter([ '[Lambda_c+ -> Lambda0 pi+]cc' ],'Lc2LambdaLLPifor' + name),
                                        RequiredSelections = [self.selPionTight, self.selLambdaLL])
        
        self.selLc2L0K_DD = Selection( 'SelLc2LambdaDDKfor' + name,
                                       Algorithm = self._Lc2L0HFilter([ '[Lambda_c+ -> Lambda0 K+]cc' ],'Lc2LambdaDDKfor' + name),
                                        RequiredSelections = [self.selKaon, self.selLambdaDD])
        
        self.selLc2L0K_LL = Selection( 'SelLc2LambdaLLKfor' + name,
                                       Algorithm = self._Lc2L0HFilter([ '[Lambda_c+ -> Lambda0 K+]cc' ],'Lc2LambdaLLKfor' + name),
                                        RequiredSelections = [self.selKaon, self.selLambdaLL])

        self.selLc2pKK = Selection( 'SelLc2pKKfor' + name,
                                    Algorithm = self._Lc2pHHFilter([ '[Lambda_c+ -> p+ K- K+]cc' ],'Lc2pKKfor' + name),
                                    RequiredSelections = [self.selKaon, self.selProton])

        self.selLc2pPiPi = Selection( 'SelLc2pPiPifor' + name,
                                      Algorithm = self._Lc2pHHFilter([ '[Lambda_c+ -> p+ pi- pi+]cc' ],'Lc2pPiPifor' + name),
                                      RequiredSelections = [self.selPionTight, self.selProton])## tighter pion PID needed here to reduce retention
        
        self.sellambdac = Selection( 'SelLc2PKPifor' + name,
                                     Algorithm = self._Lc2pHHFilter([ '[Lambda_c+ -> K- p+ pi+]cc' ],'Lc2PKPifor' + name),
                                     RequiredSelections = [self.selKaon, self.selPion, self.selProton ] )
        
        self.sellambdacDCS = Selection( 'SelLc2PKPiDCSfor' + name,
                                        Algorithm = self._Lc2pHHFilter([ '[Lambda_c+ -> K+ p+ pi-]cc' ],'Lc2PKPiDCSfor' + name),
                                        RequiredSelections = [self.selKaon, self.selPion, self.selProton ] )
        
        self.sel_Lc2pKsLL = Selection("selLc2pKsLLfor"+name,
                                      Algorithm =  self._Lc2pKsFilter( ['[Lambda_c+ -> p+ KS0]cc'], 'Lc2pKsLLfor' + name),
                                      RequiredSelections = [self.selProton, self.selKSLL ] )

        self.sel_Lc2pKsDD = Selection("selLc2pKsDDfor"+name,
                                      Algorithm =  self._Lc2pKsFilter( ['[Lambda_c+ -> p+ KS0]cc'], 'Lc2pKsDDfor' + name),
                                      RequiredSelections = [self.selProton, self.selKSDD ] )

        #################### MAKE THE "B" CANDIDATES ##############################
        MuSel = self.selmuon 
        BDecays = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        self.selb2D0MuXK3Pi = makeb2DMuX('b2D0MuXK3Pi' + name,BDecays,MuSel,self.sel_D0_to_K3Pi ,BCuts)
        self.selb2D0MuX4Pi = makeb2DMuX('b2D0MuX4Pi' + name,BDecays,MuSel,self.sel_D0_to_4Pi,BCuts)
        self.selb2D0MuX2K2Pi = makeb2DMuX('b2D0MuX2K2Pi' + name,BDecays,MuSel,self.sel_D0_to_2K2Pi,BCuts)
        self.selb2D0MuX3KPi = makeb2DMuX('b2D0MuX3KPi' + name,BDecays,MuSel,self.sel_D0_to_3KPi,BCuts)        

        BDecays = [ '[B0 -> D*(2010)+ mu-]cc', '[B0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXK3Pi = makeb2DMuX('b2DstarMuXK3Pi' + name,BDecays,MuSel,self.selDstar_to_K3Pi ,BCuts)
        self.selb2DstarMuX4Pi = makeb2DMuX('b2DstarMuX4Pi' + name,BDecays,MuSel,self.selDstar_to_4Pi,BCuts)
        self.selb2DstarMuX2K2Pi = makeb2DMuX('b2DstarMuX2K2Pi' + name,BDecays,MuSel,self.selDstar_to_2K2Pi,BCuts)
        self.selb2DstarMuX3KPi = makeb2DMuX('b2DstarMuX3KPi' + name,BDecays,MuSel,self.selDstar_to_3KPi,BCuts)        

        ############### B+ -> MU X D0 -> Ks HH #########################
        MuSel = self.selmuon
        BDecays = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        self.selb2D0MuXKsPiPiLL = makeb2DMuX('b2D0MuXKsPiPiLL' + name,BDecays,MuSel,self.seld02KsPiPiLL,BCuts)
        self.selb2D0MuXKsPiPiDD = makeb2DMuX('b2D0MuXKsPiPiDD' + name,BDecays,MuSel,self.seld02KsPiPiDD,BCuts)
        self.selb2D0MuXKsKKLL = makeb2DMuX('b2D0MuXKsKKLL' + name,BDecays,MuSel,self.seld02KsKKLL,BCuts)
        self.selb2D0MuXKsKKDD = makeb2DMuX('b2D0MuXKsKKDD' + name,BDecays,MuSel,self.seld02KsKKDD,BCuts)
        BDecays = [ '[B- -> D0 mu-]cc'] # since already have both D0 -> KS0 K-pi+ and KS0 K+pi-
        self.selb2D0MuXKsKPiLL = makeb2DMuX('b2D0MuXKsKPiLL' + name,BDecays,MuSel,self.seld02KsKPiLL,BCuts)
        self.selb2D0MuXKsKPiDD = makeb2DMuX('b2D0MuXKsKPiDD' + name,BDecays,MuSel,self.seld02KsKPiDD,BCuts)

        #### Dstar
        BDecays = [ '[B0 -> D*(2010)+ mu-]cc', '[B0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXKsPiPiLL = makeb2DMuX('b2DstarMuXKsPiPiLL'+name,BDecays,MuSel,self.selDstar_2KsPiPiLL,BCuts)
        self.selb2DstarMuXKsPiPiDD = makeb2DMuX('b2DstarMuXKsPiPiDD'+name,BDecays,MuSel,self.selDstar_2KsPiPiDD,BCuts)
        self.selb2DstarMuXKsKKLL = makeb2DMuX('b2DstarMuXKsKKLL'+name,BDecays,MuSel,self.selDstar_2KsKKLL,BCuts)
        self.selb2DstarMuXKsKKDD = makeb2DMuX('b2DstarMuXKsKKDD'+name,BDecays,MuSel,self.selDstar_2KsKKDD,BCuts)
        self.selb2DstarMuXKsKPiLL = makeb2DMuX('b2DstarMuXKsKPiLL'+name,BDecays,MuSel,self.selDstar_2KsKPiLL,BCuts)
        self.selb2DstarMuXKsKPiDD = makeb2DMuX('b2DstarMuXKsKPiDD'+name,BDecays,MuSel,self.selDstar_2KsKPiDD,BCuts)


        ############# B+ -> MU X D0 -> Ks Ks #####################
        
        MuSel = self.selmuon
        BDecays = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        self.selb2D0MuXKsKs_DDLL = makeb2DMuX('b2D0MuXKsKs_DDLL' + name,BDecays,MuSel,self.sel_D0_to_KsKs_DDLL,BCutsKsKs)
        self.selb2D0MuXKsKs_DDDD = makeb2DMuX('b2D0MuXKsKs_DDDD' + name,BDecays,MuSel,self.sel_D0_to_KsKs_DDDD,BCutsKsKs)
        self.selb2D0MuXKsKs_LLLL = makeb2DMuX('b2D0MuXKsKs_LLLL' + name,BDecays,MuSel,self.sel_D0_to_KsKs_LLLL,BCutsKsKs)

        ## Dstar
        BDecays = [ '[B0 -> D*(2010)+ mu-]cc', '[B0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXKsKs_DDLL = makeb2DMuX('b2DstarMuXKsKs_DDLL' + name,BDecays,MuSel,self.sel_Dstar_to_KsKs_DDLL,BCutsKsKs)
        self.selb2DstarMuXKsKs_DDDD = makeb2DMuX('b2DstarMuXKsKs_DDDD' + name,BDecays,MuSel,self.sel_Dstar_to_KsKs_DDDD,BCutsKsKs)
        self.selb2DstarMuXKsKs_LLLL = makeb2DMuX('b2DstarMuXKsKs_LLLL' + name,BDecays,MuSel,self.sel_Dstar_to_KsKs_LLLL,BCutsKsKs)

        ############### B+ -> MU X D0 -> H H Pi0  #########################

        #MuSel = self.selmuonL0TOS
        MuSel = self.selmuon
        BDecays = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        self.selb2D0MuXKPiPi0Resolved = makeb2DMuX('b2D0MuXKPiPi0Resolved'+name,BDecays,MuSel,self.seld02KPiPi0Resolved,BCutsTight)
        self.selb2D0MuXKKPi0Resolved = makeb2DMuX('b2D0MuXKKPi0Resolved'+name,BDecays,MuSel,self.seld02KKPi0Resolved,BCutsTight)
        self.selb2D0MuXPiPiPi0Resolved = makeb2DMuX('b2D0MuXPiPiPi0Resolved'+name,BDecays,MuSel,self.seld02PiPiPi0Resolved,BCutsTight)
        self.selb2D0MuXKKPi0SSResolved = makeb2DMuX('b2D0MuXKKPi0SSResolved'+name,BDecays,MuSel,self.seld02KKPi0SSResolved,BCutsTight)
        self.selb2D0MuXPiPiPi0SSResolved = makeb2DMuX('b2D0MuXPiPiPi0SSResolved'+name,BDecays,MuSel,self.seld02PiPiPi0SSResolved,BCutsTight)
        self.selb2D0MuXKPiPi0Merged = makeb2DMuX('b2D0MuXKPiPi0Merged'+name,BDecays,MuSel,self.seld02KPiPi0Merged,BCutsTight)
        self.selb2D0MuXKKPi0Merged = makeb2DMuX('b2D0MuXKKPi0Merged'+name,BDecays,MuSel,self.seld02KKPi0Merged,BCutsTight)
        self.selb2D0MuXPiPiPi0Merged = makeb2DMuX('b2D0MuXPiPiPi0Merged'+name,BDecays,MuSel,self.seld02PiPiPi0Merged,BCutsTight)
        
        ## Dstar ###
        BDecays = [ '[B0 -> D*(2010)+ mu-]cc', '[B0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXKPiPi0ResolvedUp = makeb2DMuX('b2DstarMuXKPiPi0ResolvedUp'+name,BDecays,MuSel,self.selDstar_2KPiPi0ResolvedUp,BCutsTight)
        self.selb2DstarMuXKPiPi0Resolved = makeb2DMuX('b2DstarMuXKPiPi0Resolved'+name,BDecays,MuSel,self.selDstar_2KPiPi0Resolved,BCutsTight)
        self.selb2DstarMuXKKPi0Resolved = makeb2DMuX('b2DstarMuXKKPi0Resolved'+name,BDecays,MuSel,self.selDstar_2KKPi0Resolved,BCutsTight)
        self.selb2DstarMuXPiPiPi0Resolved = makeb2DMuX('b2DstarMuXPiPiPi0Resolved'+name,BDecays,MuSel,self.selDstar_2PiPiPi0Resolved,BCutsTight)
        self.selb2DstarMuXKKPi0SSResolved = makeb2DMuX('b2DstarMuXKKPi0SSResolved'+name,BDecays,MuSel,self.selDstar_2KKPi0SSResolved,BCutsTight)
        self.selb2DstarMuXPiPiPi0SSResolved = makeb2DMuX('b2DstarMuXPiPiPi0SSResolved'+name,BDecays,MuSel,self.selDstar_2PiPiPi0SSResolved,BCutsTight)
        self.selb2DstarMuXKPiPi0MergedUp = makeb2DMuX('b2DstarMuXKPiPi0MergedUp'+name,BDecays,MuSel,self.selDstar_2KPiPi0MergedUp,BCutsTight)
        self.selb2DstarMuXKPiPi0Merged = makeb2DMuX('b2DstarMuXKPiPi0Merged'+name,BDecays,MuSel,self.selDstar_2KPiPi0Merged,BCutsTight)
        self.selb2DstarMuXKKPi0Merged = makeb2DMuX('b2DstarMuXKKPi0Merged'+name,BDecays,MuSel,self.selDstar_2KKPi0Merged,BCutsTight)
        self.selb2DstarMuXPiPiPi0Merged = makeb2DMuX('b2DstarMuXPiPiPi0Merged'+name,BDecays,MuSel,self.selDstar_2PiPiPi0Merged,BCutsTight)


        ############## B0 -> MU X D+ -> HHH ###############
        MuSel = self.selmuon
        BDecays = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ]
        self.selb2DsMuXKKPiDCS = makeb2DMuX('b2DsMuXKKPiDCS'+name,BDecays,MuSel,self.selds2KKPiDCS,BCuts)
        
        ############### B0 -> MU X D+ -> Ks H  #########################

        MuSel = self.selmuon
        BDecays = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ]
        self.selb2DsMuXKsLLK = makeb2DMuX('b2DsMuXKsLLK'+name,BDecays,MuSel,self.selds2KsLLK,BCuts)
        self.selb2DsMuXKsDDK = makeb2DMuX('b2DsMuXKsDDK'+name,BDecays,MuSel,self.selds2KsDDK,BCuts)
        self.selb2DsMuXKsLLPi = makeb2DMuX('b2DsMuXKsLLPi'+name,BDecays,MuSel,self.selds2KsLLPi,BCuts)
        self.selb2DsMuXKsDDPi = makeb2DMuX('b2DsMuXKsDDPi'+name,BDecays,MuSel,self.selds2KsDDPi,BCuts)

        ############### B0 -> MU X (D+ -> H mu mu)  #########################

        MuSel = self.selmuon
        BDecays = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ]
        self.selb2DsMuXPiMuMu = makeb2DMuX('b2DsMuXPiMuMu'+name,BDecays,MuSel,self.selds2pimumu,BCuts)
        self.selb2DsMuXKMuMu = makeb2DMuX('b2DsMuXKMuMu'+name,BDecays,MuSel,self.selds2kmumu,BCuts)

        ############## Lambda_b0 -> MU X Lambda_c -> X ############################

        MuSel = self.selmuon
        BDecays = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc']
        self.selb2Lc2L0DDPiMuX = makeb2DMuX('b2Lc2L0DDPiMuX' + name,BDecays,MuSel,self.selLc2L0Pi_DD,BCuts)
        self.selb2Lc2L0LLPiMuX = makeb2DMuX('b2Lc2L0LLPiMuX' + name,BDecays,MuSel,self.selLc2L0Pi_LL,BCuts)
        self.selb2Lc2L0DDKMuX = makeb2DMuX('b2Lc2L0DDKMuX' + name,BDecays,MuSel,self.selLc2L0K_DD,BCuts)
        self.selb2Lc2L0LLKMuX = makeb2DMuX('b2Lc2L0LLKMuX' + name,BDecays,MuSel,self.selLc2L0K_LL,BCuts)

        ####### Lambda_c -> p K pi
        MuSel = self.selmuonnew
        BDecays = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc']
        self.selb2LcMuX = makeb2DMuX('b2LcMuX' + name,BDecays,MuSel, self.sellambdac ,BCuts)
        self.selb2LcDCSMuX = makeb2DMuX('b2LcDCSMuX' + name, BDecays,MuSel,self.sellambdacDCS,BCuts)
        self.selb2Lc2pPiPiMuX = makeb2DMuX('b2Lc2pPiPiMuX' + name, BDecays,MuSel,self.selLc2pPiPi,BCuts)
        self.selb2Lc2pKKMuX = makeb2DMuX('b2Lc2pKKMuX' + name, BDecays,MuSel,self.selLc2pKK,BCuts)

        ####### Lambda_c -> p Ks
        MuSel = self.selmuonnew
        BDecays = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc']
        self.selb2LcMuXpKsLL = makeb2DMuX('b2LcMuXpKsLL' + name,BDecays,MuSel, self.sel_Lc2pKsLL,BCuts)
        self.selb2LcMuXpKsDD = makeb2DMuX('b2LcMuXpKsDD' + name,BDecays,MuSel, self.sel_Lc2pKsDD,BCuts)
        
        ################# DECLARE THE STRIPPING LINES #################################

        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                     "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        self.registerLine( StrippingLine('b2D0MuXKsKs_DDDD' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuXKsKs_DDDD))
        self.registerLine( StrippingLine('b2D0MuXKsKs_LLLL' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuXKsKs_LLLL))
        self.registerLine( StrippingLine('b2D0MuXKsKs_DDLL' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuXKsKs_DDLL))
        ## D*+ versions
        self.registerLine( StrippingLine('b2DstarMuXKsKs_DDDD' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuXKsKs_DDDD))
        self.registerLine( StrippingLine('b2DstarMuXKsKs_LLLL' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuXKsKs_LLLL))
        self.registerLine( StrippingLine('b2DstarMuXKsKs_DDLL' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuXKsKs_DDLL))

        ########### D0 -> HHHHH
        ## all but the 4pi are in the SL full DST stream.
        #self.registerLine( StrippingLine('b2D0MuXK3Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuXK3Pi) )
        self.registerLine( StrippingLine('b2D0MuX4Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuX4Pi) )
        #self.registerLine( StrippingLine('b2D0MuX2K2Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuX2K2Pi) )
        #self.registerLine( StrippingLine('b2D0MuX3KPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuX3KPi) )
        ## D*+ versions
        #self.registerLine( StrippingLine('b2DstarMuXK3Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuXK3Pi) )
        self.registerLine( StrippingLine('b2DstarMuX4Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuX4Pi) )
        #self.registerLine( StrippingLine('b2DstarMuX2K2Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuX2K2Pi) )
        #self.registerLine( StrippingLine('b2DstarMuX3KPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuX3KPi) )
        
        ########### D0 -> Ks HH 
        self.registerLine( StrippingLine('b2DstarMuXKsPiPiLL'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKsPiPiLL) ) 
        self.registerLine( StrippingLine('b2DstarMuXKsPiPiDD'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKsPiPiDD) )
        self.registerLine( StrippingLine('b2DstarMuXKsKPiLL'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKsKPiLL) )
        self.registerLine( StrippingLine('b2DstarMuXKsKPiDD'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKsKPiDD) )
        self.registerLine( StrippingLine('b2DstarMuXKsKKLL'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKsKKLL) )
        self.registerLine( StrippingLine('b2DstarMuXKsKKDD'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKsKKDD) )
        ## D*+ versions
        self.registerLine( StrippingLine('b2D0MuXKsPiPiLL'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKsPiPiLL) )
        self.registerLine( StrippingLine('b2D0MuXKsPiPiDD'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKsPiPiDD) )
        self.registerLine( StrippingLine('b2D0MuXKsKKLL'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKsKKLL) )
        self.registerLine( StrippingLine('b2D0MuXKsKKDD'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKsKKDD) )
        self.registerLine( StrippingLine('b2D0MuXKsKPiLL'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKsKPiLL) )
        self.registerLine( StrippingLine('b2D0MuXKsKPiDD'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKsKPiDD) )
        
        ########### D0 -> HHPi0
        self.registerLine( StrippingLine('b2D0MuXKPiPi0Resolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKPiPi0Resolved) )
        self.registerLine( StrippingLine('b2D0MuXKKPi0Resolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKKPi0Resolved) )
        self.registerLine( StrippingLine('b2D0MuXPiPiPi0Resolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXPiPiPi0Resolved) )
        self.registerLine( StrippingLine('b2D0MuXKKPi0SSResolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKKPi0SSResolved) )
        self.registerLine( StrippingLine('b2D0MuXPiPiPi0SSResolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXPiPiPi0SSResolved) )
        self.registerLine( StrippingLine('b2D0MuXKPiPi0Merged'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKPiPi0Merged) )
        self.registerLine( StrippingLine('b2D0MuXKKPi0Merged'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXKKPi0Merged) )
        self.registerLine( StrippingLine('b2D0MuXPiPiPi0Merged'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2D0MuXPiPiPi0Merged) )
        ## D*+ versions
        self.registerLine( StrippingLine('b2DstarMuXKPiPi0Resolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKPiPi0Resolved) )
        self.registerLine( StrippingLine('b2DstarMuXKPiPi0ResolvedUp'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKPiPi0ResolvedUp) )
        self.registerLine( StrippingLine('b2DstarMuXKKPi0Resolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKKPi0Resolved) )
        self.registerLine( StrippingLine('b2DstarMuXPiPiPi0Resolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXPiPiPi0Resolved) )
        self.registerLine( StrippingLine('b2DstarMuXKKPi0SSResolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKKPi0SSResolved) )
        self.registerLine( StrippingLine('b2DstarMuXPiPiPi0SSResolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXPiPiPi0SSResolved) )
        self.registerLine( StrippingLine('b2DstarMuXKPiPi0Merged'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKPiPi0Merged) )
        self.registerLine( StrippingLine('b2DstarMuXKPiPi0MergedUp'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKPiPi0MergedUp) )
        self.registerLine( StrippingLine('b2DstarMuXKKPi0Merged'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKKPi0Merged) )
        self.registerLine( StrippingLine('b2DstarMuXPiPiPi0Merged'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXPiPiPi0Merged) )


        ########### D+ -> KsH
        self.registerLine( StrippingLine('b2DsMuXKKPiDCS' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKKPiDCS) )
        self.registerLine( StrippingLine('b2DsMuXKsLLK' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKsLLK) )
        self.registerLine( StrippingLine('b2DsMuXKsDDK' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKsDDK) )
        self.registerLine( StrippingLine('b2DsMuXKsLLPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKsLLPi) )
        self.registerLine( StrippingLine('b2DsMuXKsDDPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKsDDPi) )

        ########### D+ -> KsKKPi
        self.registerLine( StrippingLine('b2DpMuXKsKKPiLL' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DpMuXKsKKPiLL) )
        self.registerLine( StrippingLine('b2DpMuXKsKKPiDD' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DpMuXKsKKPiDD) )
        
        ########## D+ -> H mu mu
        self.registerLine( StrippingLine('b2DsMuXPiMuMu' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXPiMuMu) )
        self.registerLine( StrippingLine('b2DsMuXKMuMu' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKMuMu) )
        
        ########## Lambda_c+ -> Lambda H
        self.registerLine( StrippingLine('b2MuXLc2L0LLPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2Lc2L0LLPiMuX) )
        self.registerLine( StrippingLine('b2MuXLc2L0DDPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2Lc2L0DDPiMuX) )
        self.registerLine( StrippingLine('b2MuXLc2L0LLK' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2Lc2L0LLKMuX) )
        self.registerLine( StrippingLine('b2MuXLc2L0DDK' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2Lc2L0DDKMuX) )
        
        ########## Lambda_c+ -> p HH 
        self.registerLine( StrippingLine('b2LcMuX' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2LcMuX) )
        self.registerLine( StrippingLine('b2LcDCSMuX' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2LcDCSMuX) )
        self.registerLine( StrippingLine('b2Lc2pPiPiMuX' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2Lc2pPiPiMuX) ) 
        self.registerLine( StrippingLine('b2Lc2pKKMuX' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2Lc2pKKMuX) )

        ########## Lambda_c+ -> p KS0
        self.registerLine( StrippingLine('b2MuXLc2pKsLL'+name+'Line',prescale = 1, FILTER=GECs, selection = self.selb2LcMuXpKsLL))
        self.registerLine( StrippingLine('b2MuXLc2pKsDD'+name+'Line',prescale = 1, FILTER=GECs, selection = self.selb2LcMuXpKsDD))
        
        
    def _muonFilter( self , _name):
        _code = "(PT > %(MuonPT)s *MeV) & (P> 3.0*GeV)"\
                "& (TRGHOSTPROB< %(TrGhostProbMax)s)"\
                "& (TRCHI2DOF< %(TRCHI2)s) & (MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s)"\
                "& (PIDmu > %(PIDmu)s)" % self.__confdict__
        _mu = FilterDesktop( name = _name, Code = _code )
        return _mu        

    def _pionFilter( self , _name):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "& (TRGHOSTPROB< %(TrGhostProbMax)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDK)s)" % self.__confdict__
        _pi = FilterDesktop( name = _name, Code = _code )
        return _pi

    def _kaonFilter( self , _name ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "& (TRGHOSTPROB< %(TrGhostProbMax)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK> %(KaonPIDK)s)" % self.__confdict__
        _ka = FilterDesktop( name = _name, Code = _code )
        return _ka 

    def _protonFilter( self, _name ):
        _code = "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(KPiPT)s *MeV) & (P>2.0*GeV)"\
                "& (TRGHOSTPROB< %(TrGhostProbMax)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>1.0e-10)" % self.__confdict__ 
        _pr = FilterDesktop( name = _name, Code = _code)
        return _pr
        
    def _kaonlooseFilter( self, _name ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "& (TRGHOSTPROB< %(TrGhostProbMax)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2Loose)s) &  (PIDK> %(KaonPIDKloose)s)" % self.__confdict__
        _kal = FilterDesktop( name = _name, Code = _code )
        return _kal 
    
    def _pionlooseFilter( self , _name):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "& (TRGHOSTPROB< %(TrGhostProbMax)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2Loose)s)" % self.__confdict__
        _pil = FilterDesktop( name = _name, Code = _code )
        return _pil
    
    def _Pi0ResolvedLooseFilter( self, _name):
        _code = "(PT> %(Pi0PtMinLoose)s *MeV) & (P> %(Pi0PMin)s *MeV)"\
                "& (CHILD(CL,1)> %(PhotonCL)s) & (CHILD(CL,2)> %(PhotonCL)s)" % self.__confdict__
        _pil = FilterDesktop( name = _name, Code = _code )
        return _pil

    def _Pi0ResolvedFilter( self, _name):
        _code = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV)"\
                "& (CHILD(CL,1)> %(PhotonCL)s) & (CHILD(CL,2)> %(PhotonCL)s)" % self.__confdict__
        _pil = FilterDesktop( name = _name, Code = _code )
        return _pil

    def _Pi0MergedFilter( self, _name):
        _code = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV)" % self.__confdict__
        _pil = FilterDesktop( name = _name, Code = _code )
        return _pil

    def _KsDDFilter( self, _name):
        _code = " (P> %(KSDDPMin)s *MeV) & (PT> %(KSDDPTMin)s *MeV)" \
                " & (ADMASS('KS0') < %(KSDDCutMass)s *MeV) & (BPVVDCHI2> %(KSDDCutFDChi2)s)" \
                " & CHILDCUT((TRCHI2DOF < %(KSDaugTrackChi2)s),1)" \
                " & CHILDCUT((TRCHI2DOF < %(KSDaugTrackChi2)s),2)" \
                " & (VFASPF(VCHI2PDOF) < %(KSVertexChi2)s)" \
                " & (BPVDIRA > %(KSCutDIRA)s )" % self.__confdict__
        _pil = FilterDesktop( name = _name, Code = _code)
        return _pil
    
    def _KsLLFilter( self, _name):
        _code = " (P> %(KSLLPMin)s *MeV) & (PT> %(KSLLPTMin)s *MeV)" \
                " & (ADMASS('KS0') < %(KSLLCutMass)s *MeV) & (BPVVDCHI2> %(KSLLCutFDChi2)s)" \
                " & CHILDCUT((TRCHI2DOF < %(KSDaugTrackChi2)s),1)" \
                " & CHILDCUT((TRCHI2DOF < %(KSDaugTrackChi2)s),2)" \
                " & CHILDCUT((TRGHOSTPROB< %(TrGhostProbMax)s),1)"\
                " & CHILDCUT((TRGHOSTPROB< %(TrGhostProbMax)s),2)"\
                " & (VFASPF(VCHI2PDOF) < %(KSVertexChi2)s)" \
                " & (BPVDIRA > %(KSCutDIRA)s )" % self.__confdict__
        _pil = FilterDesktop( name = _name, Code = _code)
        return _pil
    
    def _LambdaLLFilter( self, _name):
        _code = " (P> %(LambdaLLPMin)s *MeV) & (PT> %(LambdaLLPTMin)s *MeV)" \
                " & (ADMASS('Lambda0') < %(LambdaLLCutMass)s *MeV) & (BPVVDCHI2> %(LambdaLLCutFDChi2)s)" \
                " & CHILDCUT((TRCHI2DOF < %(LambdaDaugTrackChi2)s),1)" \
                " & CHILDCUT((TRCHI2DOF < %(LambdaDaugTrackChi2)s),2)" \
                " & CHILDCUT((TRGHOSTPROB< %(TrGhostProbMax)s),1)"\
                " & CHILDCUT((TRGHOSTPROB< %(TrGhostProbMax)s),2)"\
                " & (VFASPF(VCHI2PDOF) < %(LambdaVertexChi2)s)" \
                " & (BPVDIRA > %(LambdaCutDIRA)s )" % self.__confdict__
        _pil = FilterDesktop( name = _name, Code = _code)
        return _pil
    
    def _LambdaDDFilter( self , _name):
        _code = " (P> %(LambdaDDPMin)s *MeV) & (PT> %(LambdaDDPTMin)s *MeV)" \
                " & (ADMASS('Lambda0') < %(LambdaDDCutMass)s *MeV) & (BPVVDCHI2> %(LambdaDDCutFDChi2)s)" \
                " & CHILDCUT((TRCHI2DOF < %(LambdaDaugTrackChi2)s),1)" \
                " & CHILDCUT((TRCHI2DOF < %(LambdaDaugTrackChi2)s),2)" \
                " & (VFASPF(VCHI2PDOF) < %(LambdaVertexChi2)s)" \
                " & (BPVDIRA > %(LambdaCutDIRA)s )" % self.__confdict__
        _pil = FilterDesktop( name = _name, Code = _code)
        return _pil
    
    def _D02HHHHFilter( self , _decayDescriptors,_name):
        _combinationCut = "(ADAMASS('D0') < %(Dto4h_AMassWin)s *MeV)"\
                          "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(PTSUM)s *MeV)"\
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = " (ADMASS('D0') < %(Dto4h_MassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)"\
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUM)s*MeV)"\
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02hhhh = CombineParticles( name = _name,
                                     DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02hhhh
    

    def _Ds2HMuMuFilter( self , _decayDescriptors, _name):
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV)"\
                          "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(PTSUM)s *MeV)"\
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = " (DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUM)s*MeV)"\
                     "& (BPVVDCHI2 > %(DsFDCHI2)s)"\
                     "&  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2hmumu = CombineParticles( name = _name,
                                      DecayDescriptors = _decayDescriptors,
                                      CombinationCut = _combinationCut,
                                      MotherCut = _motherCut)                             
        return _ds2hmumu
    
    def _Ds2KsHFilter( self , _decayDescriptors, _name):
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV)"\
                          "& (ACHILD(PT,1)+ACHILD(PT,2) > %(PTSUM)s *MeV)"\
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUM)s*MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2ksh = CombineParticles( name = _name,
                                    DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                             
        return _ds2ksh

    
    def _D02KsHHFilter( self , _decayDescriptors, _name):
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV)" \
                          "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(PTSUM)s *MeV)" \
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUM)s*MeV)" \
                     "& (MINTREE(((ABSID=='KS0')) , VFASPF(VZ))-VFASPF(VZ) > %(KSCutZFDFromD)s *mm )" \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02KsHH = CombineParticles( name = _name,
                                     DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02KsHH

    def _D2KsHHHFilter( self , _name, _decayDescriptors,_requiredSelections):
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV)"\
                          "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(PTSUM)s *MeV)"\
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('D+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUM)s*MeV)"\
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _comb = CombineParticles( name = "Comb"+_name,
                                  DecayDescriptors = _decayDescriptors,
                                  CombinationCut = _combinationCut,
                                  MotherCut = _motherCut)                            
        return Selection( "Sel" + _name,
                          Algorithm = _comb,
                          RequiredSelections = _requiredSelections)        
    

    
    def _D02KsKsFilter( self , _decayDescriptors, _name):
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV)"\
                          "& (ACHILD(PT,1)+ACHILD(PT,2) > %(PTSUMLoose)s *MeV)"\
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUMLoose)s*MeV)"\
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02KsKs = CombineParticles( name = _name,
                                     DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02KsKs
    
    
    def _D02HHPi0Filter( self,_name , _decayDescriptors,_requiredSelections):
        _combinationCut = "(ADAMASS('D0') < %(D02HHPi0AMassWin)s *MeV) " \
                          "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(PTSUM_HHPi0)s *MeV)"\
                          "& (ADOCA(1,2) < %(D02HHPi0DocaCut)s)" % self.__confdict__
        _motherCut = "(ADMASS('D0') < %(D02HHPi0MassWin)s *MeV) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUM_HHPi0)s*MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02HHPi0 = CombineParticles( name = "Comb"+_name,
                                      DecayDescriptors = _decayDescriptors,
                                      CombinationCut = _combinationCut,
                                      MotherCut = _motherCut)
        ### uncomment the following to do a pi0 mass constraint.
        #_d02HHPi0.addTool( OfflineVertexFitter )
        #_d02HHPi0.ParticleCombiners.update( {"" : "OfflineVertexFitter"} )
        #_d02HHPi0.OfflineVertexFitter.useResonanceVertex = False
        #_d02HHPi0.OfflineVertexFitter.applyDauMassConstraint = True
        #_d02HHPi0.ReFitPVs = True
        return Selection( "Sel" + _name,
                          Algorithm = _d02HHPi0,
                          RequiredSelections = _requiredSelections)        

    def _Lc2L0HFilter( self , _decayDescriptors, _name):
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) " \
                          " & (ACHILD(PT,1)+ACHILD(PT,2) > %(PTSUM)s *MeV)"\
                          " & (ADOCACHI2CUT( %(DDocaChi2Max)s, '')) "  % self.__confdict__
        _motherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) " \
                     " & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     " & (SUMTREE( PT,  ISBASIC )> %(PTSUM)s*MeV)"\
                     " & (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _lambdac = CombineParticles( name = _name,
                                     DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                                                         
        return _lambdac

    def  _Lc2pHHFilter( self , _decayDescriptors, _name):
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV)"\
                          " & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(PTSUM)s *MeV)"\
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUM)s*MeV)"\
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) & (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _lambdac = CombineParticles( name = _name,
                                     DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                                                         
        return _lambdac

    def  _Lc2pKsFilter( self , _decayDescriptors, _name):
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV)"\
                          " & (ACHILD(PT,1)+ACHILD(PT,2) > %(PTSUM)s *MeV)"\
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUM)s*MeV)"\
                     "& (MINTREE(((ABSID=='KS0')) , VFASPF(VZ))-VFASPF(VZ) > %(KSCutZFDFromD)s *mm )" \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) & (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _lambdac = CombineParticles( name = _name,
                                     DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                                                         
        return _lambdac

    def _D2HHHFilter( self, _name, _decayDescriptors,_requiredSelections):
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) "\
                          " & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(PTSUM)s *MeV)"\
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>%(PTSUM)s*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _comb = CombineParticles( name = "Comb"+_name,
                                  DecayDescriptors = _decayDescriptors,
                                  CombinationCut = _combinationCut,
                                  MotherCut = _motherCut)                             
        return Selection (name = "Sel"+_name,
                          Algorithm = _comb,
                          RequiredSelections = _requiredSelections)
    
def makeDstar(_name, inputD0,_softPi,Dstar_cuts) : 
    _inputD0_conj = Selection("SelConjugateD0For"+_name,
                             Algorithm = ConjugateNeutralPID('ConjugateD0For'+_name),
                             RequiredSelections = [inputD0])
    _cutsSoftPi = '( PT > %(Dstar_SoftPion_PT)s *MeV )' % Dstar_cuts
    _cutsDstarComb = '(AM - ACHILD(M,1) + 5 > %(Dstar_wideDMCutLower)s *MeV) & (AM - ACHILD(M,1) - 5 < %(Dstar_wideDMCutUpper)s *MeV)' % Dstar_cuts
    _cutsDstarMoth_base = '(VFASPF(VCHI2/VDOF) < %(Dstar_Chi2)s )' % Dstar_cuts
    _cutsDstarMoth_DM = '(M - CHILD(M,1) > %(Dstar_wideDMCutLower)s *MeV) & (M - CHILD(M,1) < %(Dstar_wideDMCutUpper)s *MeV)' % Dstar_cuts
    _cutsDstarMoth = '(' + _cutsDstarMoth_base + ' & ' + _cutsDstarMoth_DM + ')'
    _Dstar = CombineParticles( name = _name,
                               DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc",
                               DaughtersCuts = { "pi+" : _cutsSoftPi },
                               CombinationCut = _cutsDstarComb,
                               MotherCut = _cutsDstarMoth)
    return Selection (name = "Sel"+_name,Algorithm = _Dstar,RequiredSelections = [inputD0,_inputD0_conj] + [_softPi])

def makeb2DMuX(_name,
               DecayDescriptors,
               MuSel,
               DSel,
               BCuts):
    _combinationCut = "(AM<6200*MeV)"
    _motherCut = "  (MM> %(MinBMass)s *MeV) & (MM<%(MaxBMass)s *MeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)  " \
                 "& (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > %(DZ)s *mm ) "  % BCuts
    _B = CombineParticles(name = _name,DecayDescriptors = DecayDescriptors,
                          CombinationCut = _combinationCut,
                          MotherCut = _motherCut)
    return Selection (name = "Sel"+_name,
                      Algorithm = _B,
                      RequiredSelections = [MuSel, DSel])

def TOSFilter( name = None, sel = None, trigger = None ):
    """
    Function to return a selection object, filtering for TOS candidates from input selection
    """
    from Configurables import TisTosParticleTagger
    
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = { trigger+"%TOS" : 0 }
    _filter.NoRegex = True
    
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel



