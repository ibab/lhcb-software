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
__version__ = '$Revision: 0.4 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdLooseMergedPi0,StdLooseResolvedPi0
from Configurables import ConjugateNeutralPID

__all__ = ('CharmFromBSemiAllLinesConf',
           'makeb2DMuX',
           'makeb2DX',
           'makeDstar',
           'TOSFilter',
           'confdict')

confdict = {
    "GEC_nLongTrk" : 250    # adimensional
    ,"MINIPCHI2"      : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"TRCHI2Loose"   : 5.0    # adimensiional    
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PionPIDKTight" : 4.0    # adimensiional
    ,"MuonIPCHI2"    : 4.00   # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensiional
    ,"DsFDCHI2"      : 100.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"Dto4h_MassWin" : 40.0   # MeV
    ,"Dto4h_AMassWin": 42.0  # MeV
    ,"DsIP"          : 7.4    #mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"PIDmu"         : -0.0   # adimensiional
    ,"BDIRA"         : 0.999  #adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"DZ"            : 0   #mm
    ,"DDocaChi2Max"  : 20     #adimensiional
    ,"MINIPCHI2Loose": 4.0   #adimensiional
    ,"KaonPIDKloose" : -5     #adimensiional
    ,"PhiVCHI2"      :25.0    #adimensiional
    ,"PhiMassWin"    :50      #adimensiional
    ,'KSLLPMin'         : 2000  ## MeV
    ,'KSLLPTMin'        : 500   ## MeV
    ,'KSDDPMin'         : 3000  ## MeV
    ,'KSDDPTMin'        : 800   ## MeV
    ,'KSLLCutMass'      : 20    ## MeV
    ,'KSDDCutMass'      : 20    ## MeV
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
    ,"Pi0PtMin"     : 1200         # Minimum Pt of pi0 (MeV)
    ,"Pi0PMin"      : 3000         # Minimum P of pi0 (MeV)
    ,"PhotonCL"     : 0.25         # Confidence level for Pi0 photons
    ,"D02HHPi0AMassWin" : 220  # MeV (mass window for combination)
    ,"D02HHPi0MassWin"  : 200  # MeV (mass window after vertex fit)
    ,"D02HHPi0DocaCut"  : 6    # mm
    ,"D02HHPi0PtCut"    : 2000 # MeV
    ,"Dstar_preFitMassCut" : 100 # MeV
    ,"Dstar_preFitMassCut_HHPi0" : 200 # MeV
    ,"Dstar_Chi2" :  10.0 ## unitless
    ,"Dstar_SoftPion_PIDe" : 5. ## unitless
    ,"Dstar_SoftPion_PT" : 200. ## MeV
    ,"Dstar_wideDMCutLower" : -2. ## MeV
    ,"Dstar_wideDMCutUpper" : 15. ## MeV

    }

class CharmFromBSemiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "GEC_nLongTrk"
        ,"MINIPCHI2"     
        ,"TRCHI2"     
        ,"TRCHI2Loose"   
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
        ,"PhiVCHI2"
        ,"PhiMassWin"
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
        ,"Pi0PMin" 
        ,"PhotonCL"
        ,"D02HHPi0AMassWin"
        ,"D02HHPi0MassWin"
        ,"D02HHPi0PtCut"
        ,"D02HHPi0DocaCut"
        ,"Dstar_preFitMassCut"
        ,"Dstar_preFitMassCut_HHPi0"
        ,"Dstar_Chi2"
        ,"Dstar_SoftPion_PIDe"
        ,"Dstar_SoftPion_PT"
        ,"Dstar_wideDMCutLower"
        ,"Dstar_wideDMCutUpper"

        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        ############### MUON SELECTIONS ###################
        
        self.selmuon = Selection( "Mufor" + name,
                                  Algorithm = self._muonFilter(),
                                  RequiredSelections = [StdLooseMuons])
        
        self.selmuonhighPT = Selection( "MuhighPTfor" + name,
                                        Algorithm = FilterDesktop( Code = "(TRCHI2DOF < %(TRCHI2)s) & (PT>1.2*GeV) & (MIPCHI2DV(PRIMARY)> 9.0)" % self.__confdict__ ),
                                        RequiredSelections = [self.selmuon])
        
        self.selmuontight = Selection( "Mutightfor" + name,
                                       Algorithm = FilterDesktop( Code = "(MIPCHI2DV(PRIMARY)> 100)" ),
                                       RequiredSelections = [self.selmuonhighPT])
        
        self.selmuonnew = Selection( "Munewfor" + name,
                                     Algorithm = FilterDesktop( Code = "(MIPCHI2DV(PRIMARY)> 9.0) & (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__ ),
                                     RequiredSelections = [self.selmuon])
        
        self.selmuonTOS = TOSFilter( "MuTOS" + name,
                                     self.selmuontight,
                                     "Hlt2SingleMuonDecision")
        
        ############### KAON AND PION SELECTIONS ################
        
        self.selKaon = Selection( "Kfor" + name,
                                  Algorithm = self._kaonFilter(),
                                  RequiredSelections = [StdLooseKaons])
        
        self.selPion = Selection( "Pifor" + name,
                                  Algorithm = self._pionFilter(),
                                  RequiredSelections = [StdLoosePions])
        
        self.selPionTight = Selection( "PiTightfor" + name,
                                       Algorithm = FilterDesktop( Code = "(TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                                                                  "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDKTight)s)" % self.__confdict__ ),
                                       RequiredSelections = [StdLoosePions])
        
        self.selPionloose = Selection( "Piloosefor" + name,
                                       Algorithm = self._pionlooseFilter(),
                                       RequiredSelections = [StdNoPIDsPions])
        
        self.selKaonloose = Selection( "Kloosefor" + name,
                                       Algorithm = self._kaonlooseFilter(),
                                       RequiredSelections = [StdLooseKaons])
        
        ############## PI0 SELECTIONS ############################
        
        self.selPi0Resolved = Selection( "Pi0Resolvedfor" + name,
                                         Algorithm = self._Pi0ResolvedFilter(),
                                         RequiredSelections = [StdLooseResolvedPi0])
        
        self.selPi0Merged = Selection( "Pi0Mergedfor" + name,
                                         Algorithm = self._Pi0MergedFilter(),
                                         RequiredSelections = [StdLooseMergedPi0])

        ############## KS0 SELECTIONS ##############################
        
        _stdLooseKsLL = DataOnDemand("Phys/StdLooseKsLL/Particles")
        _stdLooseKsDD = DataOnDemand("Phys/StdLooseKsDD/Particles")
        
        self.selKSLL = Selection("KsLLfor"+name,
                                 Algorithm = self._KsLLFilter(),
                                 RequiredSelections = [_stdLooseKsLL])

        self.selKSDD = Selection("KsDDfor"+name,
                                 Algorithm = self._KsDDFilter(),
                                 RequiredSelections = [_stdLooseKsDD])

        ############# LAMBDA0 SELECTIONS ##########################
        
        _stdLooseLambdaLL = DataOnDemand("Phys/StdLooseLambdaLL/Particles")
        _stdLooseLambdaDD = DataOnDemand("Phys/StdLooseLambdaDD/Particles")

        self.selLambdaLL = Selection("LambdaLLfor"+name,
                                 Algorithm = self._LambdaLLFilter(),
                                 RequiredSelections = [_stdLooseLambdaLL])

        self.selLambdaDD = Selection("LambdaDDfor"+name,
                                 Algorithm = self._LambdaDDFilter(),
                                 RequiredSelections = [_stdLooseLambdaDD])

        ################ D0 -> KsKs SELECTION #############################

        Dstar_cuts = {
            "Dstar_preFitMassCut" : config["Dstar_preFitMassCut"],
            "Dstar_Chi2" : config["Dstar_Chi2"],
            "Dstar_SoftPion_PIDe" : config["Dstar_SoftPion_PIDe"],
            "Dstar_SoftPion_PT" : config["Dstar_SoftPion_PT"],
            "Dstar_wideDMCutLower" : config["Dstar_wideDMCutLower"],
            "Dstar_wideDMCutUpper" : config["Dstar_wideDMCutUpper"],
            }

        self.sel_D0_to_KsKs_LLLL = Selection("D02KsKsLLLLfor"+name,Algorithm = self._D02KsKsFilter(),RequiredSelections = [self.selKSLL])
        self.selD0Conj_KsKs_LLLL = Selection('SelConjugate_KsKs_LLLLFor'+name,
                                             Algorithm = ConjugateNeutralPID('Conjugate_KsKs_LLLLFor'+name),RequiredSelections = [self.sel_D0_to_KsKs_LLLL])
        self.sel_Dstar_to_KsKs_LLLL = makeDstar('Dstar_KsKs_LLLLFor'+name, [self.sel_D0_to_KsKs_LLLL , self.selD0Conj_KsKs_LLLL],Dstar_cuts)
        
        self.sel_D0_to_KsKs_DDDD = Selection("D02KsKsDDDDfor"+name,Algorithm = self._D02KsKsFilter(),RequiredSelections = [self.selKSDD])
        self.selD0Conj_KsKs_DDDD = Selection('SelConjugate_KsKs_DDDDFor'+name,
                                             Algorithm = ConjugateNeutralPID('Conjugate_KsKs_DDDDFor'+name),RequiredSelections = [self.sel_D0_to_KsKs_DDDD])
        self.sel_Dstar_to_KsKs_DDDD = makeDstar('Dstar_KsKs_DDDDFor'+name, [self.sel_D0_to_KsKs_DDDD , self.selD0Conj_KsKs_DDDD],Dstar_cuts)

        self.sel_D0_to_KsKs_DDLL = Selection("D02KsKsDDLLfor"+name,Algorithm = self._D02KsKsFilter(),RequiredSelections = [self.selKSLL,self.selKSDD])
        self.selD0Conj_KsKs_DDLL = Selection('SelConjugate_KsKs_DDLLFor'+name,
                                             Algorithm = ConjugateNeutralPID('Conjugate_KsKs_DDLLFor'+name),RequiredSelections = [self.sel_D0_to_KsKs_DDLL])
        self.sel_Dstar_to_KsKs_DDLL = makeDstar('Dstar_KsKs_DDLLFor'+name, [self.sel_D0_to_KsKs_DDLL , self.selD0Conj_KsKs_DDLL],Dstar_cuts)

        ################ D0 -> 4H SELECTION ##########################

        Dstar_cuts = {
            "Dstar_preFitMassCut" : config["Dstar_preFitMassCut"],
            "Dstar_Chi2" : config["Dstar_Chi2"],
            "Dstar_SoftPion_PIDe" : config["Dstar_SoftPion_PIDe"],
            "Dstar_SoftPion_PT" : config["Dstar_SoftPion_PT"],
            "Dstar_wideDMCutLower" : config["Dstar_wideDMCutLower"],
            "Dstar_wideDMCutUpper" : config["Dstar_wideDMCutUpper"],
            }

        self.sel_D0_to_4Pi = Selection( "D0_to_4Pi_for" + name,Algorithm = self._D024PiFilter(),
                                        RequiredSelections = [self.selPionTight] )        
        self.selD0Conj_4Pi = Selection('SelConjugate_4PiFor'+name,
                                       Algorithm = ConjugateNeutralPID('Conjugate_4PiFor'+name),
                                       RequiredSelections = [self.sel_D0_to_4Pi])
        self.selDstar_to_4Pi = makeDstar('Dstar_4PiFor'+name, [self.sel_D0_to_4Pi , self.selD0Conj_4Pi],Dstar_cuts)


        self.sel_D0_to_K3Pi = Selection( "D0_to_K3Pi_for" + name,Algorithm = self._D02K3PiFilter(),
                                         RequiredSelections = [self.selPionTight,self.selKaon] )        
        self.selD0Conj_K3Pi = Selection('SelConjugate_K3PiFor'+name,
                                        Algorithm = ConjugateNeutralPID('Conjugate_K3PiFor'+name),
                                        RequiredSelections = [self.sel_D0_to_K3Pi])
        self.selDstar_to_K3Pi = makeDstar('Dstar_K3PiFor'+name, [self.sel_D0_to_K3Pi , self.selD0Conj_K3Pi],Dstar_cuts)


        self.sel_D0_to_2K2Pi = Selection( "D0_to_2K2Pi_for" + name,Algorithm = self._D022K2PiFilter(),
                                          RequiredSelections = [self.selPionTight,self.selKaon] )        
        self.selD0Conj_2K2Pi = Selection('SelConjugate_2K2PiFor'+name,
                                       Algorithm = ConjugateNeutralPID('Conjugate_2K2PiFor'+name),
                                       RequiredSelections = [self.sel_D0_to_2K2Pi])
        self.selDstar_to_2K2Pi = makeDstar('Dstar_2K2PiFor'+name, [self.sel_D0_to_2K2Pi , self.selD0Conj_2K2Pi],Dstar_cuts)
        

        self.sel_D0_to_3KPi = Selection( "D0_to_3KPi_for" + name,Algorithm = self._D023KPiFilter(),
                                         RequiredSelections = [self.selPionTight,self.selKaon] )        
        self.selD0Conj_3KPi = Selection('SelConjugate_3KPiFor'+name,
                                       Algorithm = ConjugateNeutralPID('Conjugate_3KPiFor'+name),
                                        RequiredSelections = [self.sel_D0_to_3KPi])
        self.selDstar_to_3KPi = makeDstar('Dstar_3KPiFor'+name, [self.sel_D0_to_3KPi , self.selD0Conj_3KPi],Dstar_cuts)
        
        
        ################## D0 -> Ks HH SELECTIONS ##############################

        Dstar_cuts = {
            "Dstar_preFitMassCut" : config["Dstar_preFitMassCut"],
            "Dstar_Chi2" : config["Dstar_Chi2"],
            "Dstar_SoftPion_PIDe" : config["Dstar_SoftPion_PIDe"],
            "Dstar_SoftPion_PT" : config["Dstar_SoftPion_PT"],
            "Dstar_wideDMCutLower" : config["Dstar_wideDMCutLower"],
            "Dstar_wideDMCutUpper" : config["Dstar_wideDMCutUpper"],
            }

        ### ks Pi Pi 
        self.seld02KsPiPiLL = Selection( "D02KsPiPiLLfor" + name,
                                         Algorithm = self._D02KsPiPiFilter(),
                                         RequiredSelections = [self.selKSLL,self.selPionloose] )           
        self.selD0Conj2KsPiPiLL = Selection('SelConjugateD02KsPiPiLLFor'+name,
                                            Algorithm = ConjugateNeutralPID('ConjugateD02KsPiPiLLFor'+name),
                                            RequiredSelections = [self.seld02KsPiPiLL])
        self.selDstar_2KsPiPiLL = makeDstar('DstarPiPiLLFor'+name, [self.seld02KsPiPiLL , self.selD0Conj2KsPiPiLL],Dstar_cuts)

        self.seld02KsPiPiDD = Selection( "D02KsPiPiDDfor" + name,
                                         Algorithm = self._D02KsPiPiFilter(),
                                         RequiredSelections = [self.selKSDD,self.selPionloose] )           
        self.selD0Conj2KsPiPiDD = Selection('SelConjugateD02KsPiPiDDFor'+name,
                                            Algorithm = ConjugateNeutralPID('ConjugateD02KsPiPiDDFor'+name),
                                            RequiredSelections = [self.seld02KsPiPiDD])
        self.selDstar_2KsPiPiDD = makeDstar('DstarPiPiDDFor'+name, [self.seld02KsPiPiDD , self.selD0Conj2KsPiPiDD],Dstar_cuts)

        ### ks K K 
        self.seld02KsKKLL = Selection( "D02KsKKLLfor" + name,
                                         Algorithm = self._D02KsKKFilter(),
                                         RequiredSelections = [self.selKSLL,self.selKaonloose] )           
        self.selD0Conj2KsKKLL = Selection('SelConjugateD02KsKKLLFor'+name,
                                            Algorithm = ConjugateNeutralPID('ConjugateD02KsKKLLFor'+name),
                                            RequiredSelections = [self.seld02KsKKLL])
        self.selDstar_2KsKKLL = makeDstar('DstarKKLLFor'+name, [self.seld02KsKKLL , self.selD0Conj2KsKKLL],Dstar_cuts)

        self.seld02KsKKDD = Selection( "D02KsKKDDfor" + name,
                                         Algorithm = self._D02KsKKFilter(),
                                         RequiredSelections = [self.selKSDD,self.selKaonloose] )           
        self.selD0Conj2KsKKDD = Selection('SelConjugateD02KsKKDDFor'+name,
                                            Algorithm = ConjugateNeutralPID('ConjugateD02KsKKDDFor'+name),
                                            RequiredSelections = [self.seld02KsKKDD])
        self.selDstar_2KsKKDD = makeDstar('DstarKKDDFor'+name, [self.seld02KsKKDD , self.selD0Conj2KsKKDD],Dstar_cuts)

        ### ks K Pi 
        self.seld02KsKPiLL = Selection( "D02KsKPiLLfor" + name,
                                         Algorithm = self._D02KsKPiFilter(),
                                         RequiredSelections = [self.selKSLL,self.selPionloose,self.selKaonloose] )           
        self.selD0Conj2KsKPiLL = Selection('SelConjugateD02KsKPiLLFor'+name,
                                            Algorithm = ConjugateNeutralPID('ConjugateD02KsKPiLLFor'+name),
                                            RequiredSelections = [self.seld02KsKPiLL])
        self.selDstar_2KsKPiLL = makeDstar('DstarKPiLLFor'+name, [self.seld02KsKPiLL , self.selD0Conj2KsKPiLL],Dstar_cuts)

        self.seld02KsKPiDD = Selection( "D02KsKPiDDfor" + name,
                                         Algorithm = self._D02KsKPiFilter(),
                                         RequiredSelections = [self.selKSDD,self.selPionloose,self.selKaonloose] )           
        self.selD0Conj2KsKPiDD = Selection('SelConjugateD02KsKPiDDFor'+name,
                                            Algorithm = ConjugateNeutralPID('ConjugateD02KsKPiDDFor'+name),
                                            RequiredSelections = [self.seld02KsKPiDD])
        self.selDstar_2KsKPiDD = makeDstar('DstarKPiDDFor'+name, [self.seld02KsKPiDD , self.selD0Conj2KsKPiDD],Dstar_cuts)
        
        ################## D0 -> HHPi0 WITH MERGED PI0 #######################
        
        Dstar_cuts = {
           ## wider mass window than other channels
            "Dstar_preFitMassCut" : config["Dstar_preFitMassCut_HHPi0"],
            "Dstar_Chi2" : config["Dstar_Chi2"],
            "Dstar_SoftPion_PIDe" : config["Dstar_SoftPion_PIDe"],
            "Dstar_SoftPion_PT" : config["Dstar_SoftPion_PT"],
            "Dstar_wideDMCutLower" : config["Dstar_wideDMCutLower"],
            "Dstar_wideDMCutUpper" : config["Dstar_wideDMCutUpper"],
            }

        self.seld02KPiPi0Merged = Selection( "D02KPiPi0Mergedfor" + name,
                                             Algorithm = self._D02HHPi0Filter(["[D0 -> K- pi+ pi0]cc"]),
                                             RequiredSelections = [self.selKaon,self.selPionTight,self.selPi0Merged])
        self.selD0Conj2KPiPi0Merged = Selection('SelConjugateKPiPi0MergedFor'+name,
                                                  Algorithm = ConjugateNeutralPID('ConjugateKPiPi0MergedFor'+name),
                                                  RequiredSelections = [self.seld02KPiPi0Merged])
        self.selDstar_2KPiPi0Merged = makeDstar('DstarKPiPi0MergedFor'+name, [self.seld02KPiPi0Merged , self.selD0Conj2KPiPi0Merged],Dstar_cuts)

        self.seld02KKPi0Merged = Selection( "D02KKPi0Mergedfor" + name,
                                            Algorithm = self._D02HHPi0Filter(["[D0 -> K- K+ pi0]cc"]),
                                            RequiredSelections = [self.selKaon,self.selPi0Merged])
        self.selD0Conj2KKPi0Merged = Selection('SelConjugateKKPi0MergedFor'+name,
                                                  Algorithm = ConjugateNeutralPID('ConjugateKKPi0MergedFor'+name),
                                                  RequiredSelections = [self.seld02KKPi0Merged])
        self.selDstar_2KKPi0Merged = makeDstar('DstarKKPi0MergedFor'+name, [self.seld02KKPi0Merged , self.selD0Conj2KKPi0Merged],Dstar_cuts)
        
        self.seld02PiPiPi0Merged = Selection( "D02PiPiPi0Mergedfor" + name,
                                              Algorithm = self._D02HHPi0Filter(["[D0 -> pi- pi+ pi0]cc"]),
                                             RequiredSelections = [self.selPionTight,self.selPi0Merged])
        self.selD0Conj2PiPiPi0Merged = Selection('SelConjugatePiPiPi0MergedFor'+name,
                                                  Algorithm = ConjugateNeutralPID('ConjugatePiPiPi0MergedFor'+name),
                                                  RequiredSelections = [self.seld02PiPiPi0Merged])
        self.selDstar_2PiPiPi0Merged = makeDstar('DstarPiPiPi0MergedFor'+name, [self.seld02PiPiPi0Merged , self.selD0Conj2PiPiPi0Merged],Dstar_cuts)
        
        ################## D0 -> HHPi0 WITH RESOLVED PI0 #######################

        Dstar_cuts = {
            "Dstar_preFitMassCut" : config["Dstar_preFitMassCut"],
            "Dstar_Chi2" : config["Dstar_Chi2"],
            "Dstar_SoftPion_PIDe" : config["Dstar_SoftPion_PIDe"],
            "Dstar_SoftPion_PT" : config["Dstar_SoftPion_PT"],
            "Dstar_wideDMCutLower" : config["Dstar_wideDMCutLower"],
            "Dstar_wideDMCutUpper" : config["Dstar_wideDMCutUpper"],
            }
        
        self.seld02KPiPi0Resolved = Selection( "D02KPiPi0Resolvedfor" + name,
                                               Algorithm = self._D02HHPi0Filter(["[D0 -> K- pi+ pi0]cc"]),
                                               RequiredSelections = [self.selKaon,self.selPionTight,self.selPi0Resolved])
        self.selD0Conj2KPiPi0Resolved = Selection('SelConjugateKPiPi0ResolvedFor'+name,
                                                  Algorithm = ConjugateNeutralPID('ConjugateKPiPi0ResolvedFor'+name),
                                                  RequiredSelections = [self.seld02KPiPi0Resolved])
        self.selDstar_2KPiPi0Resolved = makeDstar('DstarKPiPi0ResolvedFor'+name, [self.seld02KPiPi0Resolved , self.selD0Conj2KPiPi0Resolved],Dstar_cuts)
        

        self.seld02KKPi0Resolved = Selection( "D02KKPi0Resolvedfor" + name,
                                              Algorithm = self._D02HHPi0Filter(["[D0 -> K- K+ pi0]cc"]),
                                              RequiredSelections = [self.selKaon,self.selPi0Resolved])
        self.selD0Conj2KKPi0Resolved = Selection('SelConjugateKKPi0ResolvedFor'+name,
                                                 Algorithm = ConjugateNeutralPID('ConjugateKKPi0ResolvedFor'+name),
                                                 RequiredSelections = [self.seld02KKPi0Resolved])
        self.selDstar_2KKPi0Resolved = makeDstar('DstarKKPi0ResolvedFor'+name, [self.seld02KKPi0Resolved , self.selD0Conj2KKPi0Resolved],Dstar_cuts)

        self.seld02KKPi0SSResolved = Selection( "D02KKPi0SSResolvedfor" + name,
                                                Algorithm = self._D02HHPi0Filter(["[D0 -> K+ K+ pi0]cc"]),
                                                RequiredSelections = [self.selKaon,self.selPi0Resolved])
        self.selD0Conj2KKPi0SSResolved = Selection('SelConjugateKKPi0SSResolvedFor'+name,
                                                 Algorithm = ConjugateNeutralPID('ConjugateKKPi0SSResolvedFor'+name),
                                                 RequiredSelections = [self.seld02KKPi0SSResolved])
        self.selDstar_2KKPi0SSResolved = makeDstar('DstarKKPi0SSResolvedFor'+name, [self.seld02KKPi0SSResolved , self.selD0Conj2KKPi0SSResolved],Dstar_cuts)
        
        self.seld02PiPiPi0Resolved = Selection( "D02PiPiPi0Resolvedfor" + name,
                                                Algorithm = self._D02HHPi0Filter(["[D0 -> pi- pi+ pi0]cc"]),
                                                RequiredSelections = [self.selPionTight,self.selPi0Resolved])
        self.selD0Conj2PiPiPi0Resolved = Selection('SelConjugatePiPiPi0ResolvedFor'+name,
                                                   Algorithm = ConjugateNeutralPID('ConjugatePiPiPi0ResolvedFor'+name),
                                                   RequiredSelections = [self.seld02PiPiPi0Resolved])
        self.selDstar_2PiPiPi0Resolved = makeDstar('DstarPiPiPi0ResolvedFor'+name, [self.seld02PiPiPi0Resolved , self.selD0Conj2PiPiPi0Resolved],Dstar_cuts)

        self.seld02PiPiPi0SSResolved = Selection( "D02PiPiPi0SSResolvedfor" + name,
                                                Algorithm = self._D02HHPi0Filter(["[D0 -> pi+ pi+ pi0]cc"]),
                                                RequiredSelections = [self.selPionTight,self.selPi0Resolved])
        self.selD0Conj2PiPiPi0SSResolved = Selection('SelConjugatePiPiPi0SSResolvedFor'+name,
                                                   Algorithm = ConjugateNeutralPID('ConjugatePiPiPi0SSResolvedFor'+name),
                                                   RequiredSelections = [self.seld02PiPiPi0SSResolved])
        self.selDstar_2PiPiPi0SSResolved = makeDstar('DstarPiPiPi0SSResolvedFor'+name, [self.seld02PiPiPi0SSResolved , self.selD0Conj2PiPiPi0SSResolved],Dstar_cuts)
        
        
        ################### D+/Ds+ -> Ks H SELECTIONS ######################
        
        self.selds2KsLLK = Selection( "Ds2KsLLKfor" + name,
                                      Algorithm = self._Ds2KsKFilter(),
                                      RequiredSelections = [self.selKaon, self.selKSLL] )

        self.selds2KsDDK = Selection( "Ds2KsDDKfor" + name,
                                      Algorithm = self._Ds2KsKFilter(),
                                      RequiredSelections = [self.selKaon, self.selKSDD] )

        self.selds2KsLLPi = Selection( "Ds2KsLLPifor" + name,
                                      Algorithm = self._Ds2KsPiFilter(),
                                      RequiredSelections = [self.selPion, self.selKSLL] )

        self.selds2KsDDPi = Selection( "Ds2KsDDPifor" + name,
                                      Algorithm = self._Ds2KsPiFilter(),
                                      RequiredSelections = [self.selPion, self.selKSDD] )


        ################## D+/Ds+ -> H mu mu SELECTIONS ########################

        self.selds2pimumu = Selection( "Ds2PiMuMufor" + name,
                                       Algorithm = self._Ds2PiMuMuFilter(),
                                       RequiredSelections = [self.selPion,self.selmuon] )
        
        self.selds2kmumu = Selection( "Ds2KMuMufor" + name,
                                       Algorithm = self._Ds2KMuMuFilter(),
                                       RequiredSelections = [self.selKaon,self.selmuon] )
        
        #################### Lambda_c+ -> X SELECTIONS #########################

        self.selLc2L0Pi_DD = Selection( "Lc2LambdaDDPifor" + name,Algorithm = self._Lc2L0PiFilter(),
                                        RequiredSelections = [self.selPionTight, self.selLambdaDD])
        
        self.selLc2L0Pi_LL = Selection( "Lc2LambdaLLPifor" + name,Algorithm = self._Lc2L0PiFilter(),
                                        RequiredSelections = [self.selPionTight, self.selLambdaLL])
        
        self.selLc2L0K_DD = Selection( "Lc2LambdaDDKfor" + name,Algorithm = self._Lc2L0KFilter(),
                                        RequiredSelections = [self.selKaon, self.selLambdaDD])
        
        self.selLc2L0K_LL = Selection( "Lc2LambdaLLKfor" + name,Algorithm = self._Lc2L0KFilter(),
                                        RequiredSelections = [self.selKaon, self.selLambdaLL])

        self.selLc2pKK = Selection( "Lc2pKKfor" + name,Algorithm = self._Lc2pKKFilter(),
                                    RequiredSelections = [self.selKaon, StdLooseProtons])

        ## tighter pion PID needed here to reduce retention
        self.selLc2pPiPi = Selection( "Lc2pPiPifor" + name,Algorithm = self._Lc2pPiPiFilter(),
                                      RequiredSelections = [self.selPionTight, StdLooseProtons])
        
        self.sellambdac = Selection( "Lc2PKPifor" + name,
                                     Algorithm = self._Lc2PKPiFilter(),
                                     RequiredSelections = [self.selKaon, self.selPion, StdLooseProtons ] )
        
        self.sellambdacDCS = Selection( "Lc2PKPiDCSfor" + name,
                                        Algorithm = self._Lc2PKPiDCSFilter(),
                                        RequiredSelections = [self.selKaon, self.selPion, StdLooseProtons ] )

        
        #################### MAKE THE "B" CANDIDATES ##############################
        
        ####### D0 -> hhhh
        ### For stripping 17b, there was a muon Hlt2 TOS requirement on these lines
        MuSel = self.selmuon 
        BCuts = {'BVCHI2DOF': config['BVCHI2DOF'],
                 'BDIRA': config['BDIRA'],
                 'DZ': config['DZ']}
        
        DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        self.selb2D0MuXK3Pi = makeb2DMuX('b2D0MuXK3Pi' + name,DecayDescriptors,MuSel,self.sel_D0_to_K3Pi ,BCuts)
        self.selb2D0MuX4Pi = makeb2DMuX('b2D0MuX4Pi' + name,DecayDescriptors,MuSel,self.sel_D0_to_4Pi,BCuts)
        self.selb2D0MuX2K2Pi = makeb2DMuX('b2D0MuX2K2Pi' + name,DecayDescriptors,MuSel,self.sel_D0_to_2K2Pi,BCuts)
        self.selb2D0MuX3KPi = makeb2DMuX('b2D0MuX3KPi' + name,DecayDescriptors,MuSel,self.sel_D0_to_3KPi,BCuts)        

        DecayDescriptors = [ '[B0 -> D*(2010)+ mu-]cc', '[B0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXK3Pi = makeb2DMuX('b2DstarMuXK3Pi' + name,DecayDescriptors,MuSel,self.selDstar_to_K3Pi ,BCuts)
        self.selb2DstarMuX4Pi = makeb2DMuX('b2DstarMuX4Pi' + name,DecayDescriptors,MuSel,self.selDstar_to_4Pi,BCuts)
        self.selb2DstarMuX2K2Pi = makeb2DMuX('b2DstarMuX2K2Pi' + name,DecayDescriptors,MuSel,self.selDstar_to_2K2Pi,BCuts)
        self.selb2DstarMuX3KPi = makeb2DMuX('b2DstarMuX3KPi' + name,DecayDescriptors,MuSel,self.selDstar_to_3KPi,BCuts)        

        ############### B+ -> MU X D0 -> Ks HH #########################
        MuSel = self.selmuon
        DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        BCuts = {'BVCHI2DOF': config['BVCHI2DOF'],
                 'BDIRA': config['BDIRA'],
                 'DZ': config['DZ']}

        
        self.selb2D0MuXKsPiPiLL = makeb2DMuX('b2D0MuXKsPiPiLL' + name,DecayDescriptors,MuSel,self.seld02KsPiPiLL,BCuts)
        self.selb2D0MuXKsPiPiDD = makeb2DMuX('b2D0MuXKsPiPiDD' + name,DecayDescriptors,MuSel,self.seld02KsPiPiDD,BCuts)
        self.selb2D0MuXKsKKLL = makeb2DMuX('b2D0MuXKsKKLL' + name,DecayDescriptors,MuSel,self.seld02KsKKLL,BCuts)
        self.selb2D0MuXKsKKDD = makeb2DMuX('b2D0MuXKsKKDD' + name,DecayDescriptors,MuSel,self.seld02KsKKDD,BCuts)
        self.selb2D0MuXKsKPiLL = makeb2DMuX('b2D0MuXKsKPiLL' + name,DecayDescriptors,MuSel,self.seld02KsKPiLL,BCuts)
        self.selb2D0MuXKsKPiDD = makeb2DMuX('b2D0MuXKsKPiDD' + name,DecayDescriptors,MuSel,self.seld02KsKPiDD,BCuts)

        #### Dstar
        DecayDescriptors = [ '[B0 -> D*(2010)+ mu-]cc', '[B0 -> D*(2010)+ mu+]cc' ]
        
        self.selb2DstarMuXKsPiPiLL = makeb2DMuX('b2DstarMuXKsPiPiLL'+name,DecayDescriptors,MuSel,self.selDstar_2KsPiPiLL,BCuts)
        self.selb2DstarMuXKsPiPiDD = makeb2DMuX('b2DstarMuXKsPiPiDD'+name,DecayDescriptors,MuSel,self.selDstar_2KsPiPiDD,BCuts)
        self.selb2DstarMuXKsKKLL = makeb2DMuX('b2DstarMuXKsKKLL'+name,DecayDescriptors,MuSel,self.selDstar_2KsKKLL,BCuts)
        self.selb2DstarMuXKsKKDD = makeb2DMuX('b2DstarMuXKsKKDD'+name,DecayDescriptors,MuSel,self.selDstar_2KsKKDD,BCuts)
        self.selb2DstarMuXKsKPiLL = makeb2DMuX('b2DstarMuXKsKPiLL'+name,DecayDescriptors,MuSel,self.selDstar_2KsKPiLL,BCuts)
        self.selb2DstarMuXKsKPiDD = makeb2DMuX('b2DstarMuXKsKPiDD'+name,DecayDescriptors,MuSel,self.selDstar_2KsKPiDD,BCuts)


        ############# B+ -> MU X D0 -> Ks Ks #####################
        MuSel = self.selmuon
        DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        BCuts = {'BVCHI2DOF': 9999.,
                 'BDIRA': -999, ## switch off the DIRA cut for KsKs
                 'DZ': config['DZ']}
        self.selb2D0MuXKsKs_DDLL = makeb2DMuX('b2D0MuXKsKs_DDLL' + name,DecayDescriptors,MuSel,self.sel_D0_to_KsKs_DDLL,BCuts)
        self.selb2D0MuXKsKs_DDDD = makeb2DMuX('b2D0MuXKsKs_DDDD' + name,DecayDescriptors,MuSel,self.sel_D0_to_KsKs_DDDD,BCuts)
        self.selb2D0MuXKsKs_LLLL = makeb2DMuX('b2D0MuXKsKs_LLLL' + name,DecayDescriptors,MuSel,self.sel_D0_to_KsKs_LLLL,BCuts)

        ## Dstar
        DecayDescriptors = [ '[B0 -> D*(2010)+ mu-]cc', '[B0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXKsKs_DDLL = makeb2DMuX('b2DstarMuXKsKs_DDLL' + name,DecayDescriptors,MuSel,self.sel_Dstar_to_KsKs_DDLL,BCuts)
        self.selb2DstarMuXKsKs_DDDD = makeb2DMuX('b2DstarMuXKsKs_DDDD' + name,DecayDescriptors,MuSel,self.sel_Dstar_to_KsKs_DDDD,BCuts)
        self.selb2DstarMuXKsKs_LLLL = makeb2DMuX('b2DstarMuXKsKs_LLLL' + name,DecayDescriptors,MuSel,self.sel_Dstar_to_KsKs_LLLL,BCuts)

        ############### B+ -> MU X D0 -> H H Pi0  #########################

        MuSel = self.selmuon
        BCuts = {'BVCHI2DOF': config['BVCHI2DOF'],
                 'BDIRA': config['BDIRA'],
                 'DZ': config['DZ']}
        DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        self.selb2D0MuXKPiPi0Resolved = makeb2DMuX('b2D0MuXKPiPi0Resolved'+name,DecayDescriptors,MuSel,self.seld02KPiPi0Resolved,BCuts)
        self.selb2D0MuXKKPi0Resolved = makeb2DMuX('b2D0MuXKKPi0Resolved'+name,DecayDescriptors,MuSel,self.seld02KKPi0Resolved,BCuts)
        self.selb2D0MuXPiPiPi0Resolved = makeb2DMuX('b2D0MuXPiPiPi0Resolved'+name,DecayDescriptors,MuSel,self.seld02PiPiPi0Resolved,BCuts)
        self.selb2D0MuXKKPi0SSResolved = makeb2DMuX('b2D0MuXKKPi0SSResolved'+name,DecayDescriptors,MuSel,self.seld02KKPi0SSResolved,BCuts)
        self.selb2D0MuXPiPiPi0SSResolved = makeb2DMuX('b2D0MuXPiPiPi0SSResolved'+name,DecayDescriptors,MuSel,self.seld02PiPiPi0SSResolved,BCuts)
        self.selb2D0MuXKPiPi0Merged = makeb2DMuX('b2D0MuXKPiPi0Merged'+name,DecayDescriptors,MuSel,self.seld02KPiPi0Merged,BCuts)
        self.selb2D0MuXKKPi0Merged = makeb2DMuX('b2D0MuXKKPi0Merged'+name,DecayDescriptors,MuSel,self.seld02KKPi0Merged,BCuts)
        self.selb2D0MuXPiPiPi0Merged = makeb2DMuX('b2D0MuXPiPiPi0Merged'+name,DecayDescriptors,MuSel,self.seld02PiPiPi0Merged,BCuts)
        
        ## Dstar ###
        DecayDescriptors = [ '[B0 -> D*(2010)+ mu-]cc', '[B0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXKPiPi0Resolved = makeb2DMuX('b2DstarMuXKPiPi0Resolved'+name,DecayDescriptors,MuSel,self.selDstar_2KPiPi0Resolved,BCuts)
        self.selb2DstarMuXKKPi0Resolved = makeb2DMuX('b2DstarMuXKKPi0Resolved'+name,DecayDescriptors,MuSel,self.selDstar_2KKPi0Resolved,BCuts)
        self.selb2DstarMuXPiPiPi0Resolved = makeb2DMuX('b2DstarMuXPiPiPi0Resolved'+name,DecayDescriptors,MuSel,self.selDstar_2PiPiPi0Resolved,BCuts)
        self.selb2DstarMuXKKPi0SSResolved = makeb2DMuX('b2DstarMuXKKPi0SSResolved'+name,DecayDescriptors,MuSel,self.selDstar_2KKPi0SSResolved,BCuts)
        self.selb2DstarMuXPiPiPi0SSResolved = makeb2DMuX('b2DstarMuXPiPiPi0SSResolved'+name,DecayDescriptors,MuSel,self.selDstar_2PiPiPi0SSResolved,BCuts)
        self.selb2DstarMuXKPiPi0Merged = makeb2DMuX('b2DstarMuXKPiPi0Merged'+name,DecayDescriptors,MuSel,self.selDstar_2KPiPi0Merged,BCuts)
        self.selb2DstarMuXKKPi0Merged = makeb2DMuX('b2DstarMuXKKPi0Merged'+name,DecayDescriptors,MuSel,self.selDstar_2KKPi0Merged,BCuts)
        self.selb2DstarMuXPiPiPi0Merged = makeb2DMuX('b2DstarMuXPiPiPi0Merged'+name,DecayDescriptors,MuSel,self.selDstar_2PiPiPi0Merged,BCuts)
        
        ############### B0 -> MU X D+ -> Ks H  #########################

        MuSel = self.selmuon
        BCuts = {'BVCHI2DOF': config['BVCHI2DOF'],
                 'BDIRA': config['BDIRA'],
                 'DZ': config['DZ']}
        DecayDescriptors = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ]
        
        self.selb2DsMuXKsLLK = makeb2DMuX('b2DsMuXKsLLK'+name,DecayDescriptors,MuSel,self.selds2KsLLK,BCuts)
        self.selb2DsMuXKsDDK = makeb2DMuX('b2DsMuXKsDDK'+name,DecayDescriptors,MuSel,self.selds2KsDDK,BCuts)
        self.selb2DsMuXKsLLPi = makeb2DMuX('b2DsMuXKsLLPi'+name,DecayDescriptors,MuSel,self.selds2KsLLPi,BCuts)
        self.selb2DsMuXKsDDPi = makeb2DMuX('b2DsMuXKsDDPi'+name,DecayDescriptors,MuSel,self.selds2KsDDPi,BCuts)

        ############### B0 -> MU X (D+ -> H mu mu)  #########################

        MuSel = self.selmuon
        BCuts = {'BVCHI2DOF': config['BVCHI2DOF'],
                 'BDIRA': config['BDIRA'],
                 'DZ': config['DZ']}
        DecayDescriptors = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ]
        
        self.selb2DsMuXPiMuMu = makeb2DMuX('b2DsMuXPiMuMu'+name,DecayDescriptors,MuSel,self.selds2pimumu,BCuts)
        self.selb2DsMuXKMuMu = makeb2DMuX('b2DsMuXKMuMu'+name,DecayDescriptors,MuSel,self.selds2kmumu,BCuts)

        ############## Lambda_b0 -> MU X Lambda_c -> X ############################

        MuSel = self.selmuon
        BCuts = {'BVCHI2DOF': config['BVCHI2DOF'],
                 'BDIRA': config['BDIRA'],
                 'DZ': config['DZ']}
        DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc']

        

        self.selb2Lc2L0DDPiMuX = makeb2DMuX('b2Lc2L0DDPiMuX' + name,DecayDescriptors,MuSel,self.selLc2L0Pi_DD,BCuts)
        self.selb2Lc2L0LLPiMuX = makeb2DMuX('b2Lc2L0LLPiMuX' + name,DecayDescriptors,MuSel,self.selLc2L0Pi_LL,BCuts)
        self.selb2Lc2L0DDKMuX = makeb2DMuX('b2Lc2L0DDKMuX' + name,DecayDescriptors,MuSel,self.selLc2L0K_DD,BCuts)
        self.selb2Lc2L0LLKMuX = makeb2DMuX('b2Lc2L0LLKMuX' + name,DecayDescriptors,MuSel,self.selLc2L0K_LL,BCuts)

        ####### Lambda_c -> p K pi
        MuSel = self.selmuonnew
        DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc']
        BCuts = {'BVCHI2DOF': config['BVCHI2DOF'],
                 'BDIRA': config['BDIRA'],
                 'DZ': config['DZ']}
        
        self.selb2LcMuX = makeb2DMuX('b2LcMuX' + name,DecayDescriptors,MuSel, self.sellambdac ,BCuts)
        self.selb2LcDCSMuX = makeb2DMuX('b2LcDCSMuX' + name, DecayDescriptors,MuSel,self.sellambdacDCS,BCuts)
        self.selb2Lc2pPiPiMuX = makeb2DMuX('b2Lc2pPiPiMuX' + name, DecayDescriptors,MuSel,self.selLc2pPiPi,BCuts)
        self.selb2Lc2pKKMuX = makeb2DMuX('b2Lc2pKKMuX' + name, DecayDescriptors,MuSel,self.selLc2pKK,BCuts)

        
        ################# DECLARE THE STRIPPING LINES #################################

        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                     "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        self.registerLine( StrippingLine('b2D0MuXKsKs_DDDD' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuXKsKs_DDDD))
        self.registerLine( StrippingLine('b2D0MuXKsKs_LLLL' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuXKsKs_LLLL))
        self.registerLine( StrippingLine('b2D0MuXKsKs_DDLL' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuXKsKs_DDLL))
        ### D*+ versions
        self.registerLine( StrippingLine('b2DstarMuXKsKs_DDDD' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuXKsKs_DDDD))
        self.registerLine( StrippingLine('b2DstarMuXKsKs_LLLL' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuXKsKs_LLLL))
        self.registerLine( StrippingLine('b2DstarMuXKsKs_DDLL' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuXKsKs_DDLL))

        ########### D0 -> HHHHH
        self.registerLine( StrippingLine('b2D0MuXK3Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuXK3Pi) )
        self.registerLine( StrippingLine('b2D0MuX4Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuX4Pi) )
        self.registerLine( StrippingLine('b2D0MuX2K2Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuX2K2Pi) )
        self.registerLine( StrippingLine('b2D0MuX3KPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2D0MuX3KPi) )
        ## D*+ versions
        self.registerLine( StrippingLine('b2DstarMuXK3Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuXK3Pi) )
        self.registerLine( StrippingLine('b2DstarMuX4Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuX4Pi) )
        self.registerLine( StrippingLine('b2DstarMuX2K2Pi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuX2K2Pi) )
        self.registerLine( StrippingLine('b2DstarMuX3KPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DstarMuX3KPi) )
        
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
        self.registerLine( StrippingLine('b2DstarMuXKKPi0Resolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKKPi0Resolved) )
        self.registerLine( StrippingLine('b2DstarMuXPiPiPi0Resolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXPiPiPi0Resolved) )
        self.registerLine( StrippingLine('b2DstarMuXKKPi0SSResolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKKPi0SSResolved) )
        self.registerLine( StrippingLine('b2DstarMuXPiPiPi0SSResolved'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXPiPiPi0SSResolved) )
        self.registerLine( StrippingLine('b2DstarMuXKPiPi0Merged'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKPiPi0Merged) )
        self.registerLine( StrippingLine('b2DstarMuXKKPi0Merged'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXKKPi0Merged) )
        self.registerLine( StrippingLine('b2DstarMuXPiPiPi0Merged'+name+'Line',prescale = 1,FILTER=GECs,selection = self.selb2DstarMuXPiPiPi0Merged) )
        
        ########### D+ -> KsH
        self.registerLine( StrippingLine('b2DsMuXKsLLK' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKsLLK) )
        self.registerLine( StrippingLine('b2DsMuXKsDDK' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKsDDK) )
        self.registerLine( StrippingLine('b2DsMuXKsLLPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKsLLPi) )
        self.registerLine( StrippingLine('b2DsMuXKsDDPi' + name + 'Line', prescale = 1, FILTER=GECs,selection = self.selb2DsMuXKsDDPi) )

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
        

        
    def _muonFilter( self ):
        _code = "(PT > %(MuonPT)s *MeV) & (P> 3.0*GeV) & (TRCHI2DOF< %(TRCHI2Loose)s) & (MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s) & (PIDmu > %(PIDmu)s)" % self.__confdict__
        _mu = FilterDesktop( Code = _code )
        return _mu        

    def _pionFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                   "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDK)s)" % self.__confdict__
        _pi = FilterDesktop( Code = _code )
        return _pi

    def _kaonFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                   "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK> %(KaonPIDK)s)" % self.__confdict__
        _ka = FilterDesktop( Code = _code )
        return _ka 
  
    def _kaonlooseFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2Loose)s) &  (PIDK> %(KaonPIDKloose)s)" % self.__confdict__
        _kal = FilterDesktop( Code = _code )
        return _kal 
    
    def _pionlooseFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2Loose)s)" % self.__confdict__
        _pil = FilterDesktop( Code = _code )
        return _pil
    
    
    def _Pi0ResolvedFilter( self):
        _code = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV) & (CHILD(CL,1)> %(PhotonCL)s) & (CHILD(CL,2)> %(PhotonCL)s)" % self.__confdict__
        _pil = FilterDesktop( Code = _code )
        return _pil

    def _Pi0MergedFilter( self):
        _code = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV)" % self.__confdict__
        _pil = FilterDesktop( Code = _code )
        return _pil

    def _KsDDFilter( self):
        _code = " (P> %(KSDDPMin)s *MeV) & (PT> %(KSDDPTMin)s *MeV)" \
                " & (ADMASS('KS0') < %(KSDDCutMass)s *MeV) & (BPVVDCHI2> %(KSDDCutFDChi2)s)" \
                " & CHILDCUT((TRCHI2DOF < %(KSDaugTrackChi2)s),1)" \
                " & CHILDCUT((TRCHI2DOF < %(KSDaugTrackChi2)s),2)" \
                " & (VFASPF(VCHI2PDOF) < %(KSVertexChi2)s)" \
                " & (BPVDIRA > %(KSCutDIRA)s )" % self.__confdict__
        _pil = FilterDesktop( Code = _code)
        return _pil

    def _KsLLFilter( self):
        _code = " (P> %(KSLLPMin)s *MeV) & (PT> %(KSLLPTMin)s *MeV)" \
                " & (ADMASS('KS0') < %(KSLLCutMass)s *MeV) & (BPVVDCHI2> %(KSLLCutFDChi2)s)" \
                " & CHILDCUT((TRCHI2DOF < %(KSDaugTrackChi2)s),1)" \
                " & CHILDCUT((TRCHI2DOF < %(KSDaugTrackChi2)s),2)" \
                " & (VFASPF(VCHI2PDOF) < %(KSVertexChi2)s)" \
                " & (BPVDIRA > %(KSCutDIRA)s )" % self.__confdict__
        _pil = FilterDesktop( Code = _code)
        return _pil
    
    def _LambdaLLFilter( self):
        _code = " (P> %(LambdaLLPMin)s *MeV) & (PT> %(LambdaLLPTMin)s *MeV)" \
                " & (ADMASS('Lambda0') < %(LambdaLLCutMass)s *MeV) & (BPVVDCHI2> %(LambdaLLCutFDChi2)s)" \
                " & CHILDCUT((TRCHI2DOF < %(LambdaDaugTrackChi2)s),1)" \
                " & CHILDCUT((TRCHI2DOF < %(LambdaDaugTrackChi2)s),2)" \
                " & (VFASPF(VCHI2PDOF) < %(LambdaVertexChi2)s)" \
                " & (BPVDIRA > %(LambdaCutDIRA)s )" % self.__confdict__
        _pil = FilterDesktop( Code = _code)
        return _pil

    def _LambdaDDFilter( self):
        _code = " (P> %(LambdaDDPMin)s *MeV) & (PT> %(LambdaDDPTMin)s *MeV)" \
                " & (ADMASS('Lambda0') < %(LambdaDDCutMass)s *MeV) & (BPVVDCHI2> %(LambdaDDCutFDChi2)s)" \
                " & CHILDCUT((TRCHI2DOF < %(LambdaDaugTrackChi2)s),1)" \
                " & CHILDCUT((TRCHI2DOF < %(LambdaDaugTrackChi2)s),2)" \
                " & (VFASPF(VCHI2PDOF) < %(LambdaVertexChi2)s)" \
                " & (BPVDIRA > %(LambdaCutDIRA)s )" % self.__confdict__
        _pil = FilterDesktop( Code = _code)
        return _pil
    
    def _D02KPiFilter( self ):
        _decayDescriptors = [ '[D0 -> K- pi+]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1400.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1400.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02kpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _d02kpi
  
    def _D02KKFilter( self ):
        _decayDescriptors = [ 'D0 -> K- K+' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1400.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1400.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02kk = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _d02kk

    def _D02PiPiFilter( self ):
        _decayDescriptors = [ 'D0 -> pi- pi+' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1400.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1400.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02pipi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _d02pipi

    def _D02K3PiFilter( self ):
        _decayDescriptors = [ '[D0 -> K- pi+ pi- pi+]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(Dto4h_AMassWin)s *MeV) & (APT > 1500.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (PT > 250 *MeV) & (P>2.0*GeV)"\
                           "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__}
        _motherCut = " (ADMASS('D0') < %(Dto4h_MassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (INTREE((ABSID=='pi+')& (PT > %(KPiPT)s *MeV) &(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)))" \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02k3pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02k3pi    

    def _D024PiFilter( self ):
        _decayDescriptors = [ '[D0 -> pi+ pi- pi+ pi-]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(Dto4h_AMassWin)s *MeV) & (APT > 1500.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (PT > 250 *MeV) & (P>2.0*GeV)"\
                           "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__}
        _motherCut = " (ADMASS('D0') < %(Dto4h_MassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (INTREE((ABSID=='pi+')& (PT > %(KPiPT)s *MeV) &(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)))" \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02k3pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02k3pi    

    
    def _D022K2PiFilter( self ):
        _decayDescriptors = [ '[D0 -> K+ K- pi+ pi-]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(Dto4h_AMassWin)s *MeV) & (APT > 1500.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (PT > 250 *MeV) & (P>2.0*GeV)"\
                           "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__}
        _motherCut = " (ADMASS('D0') < %(Dto4h_MassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (INTREE((ABSID=='pi+')& (PT > %(KPiPT)s *MeV) &(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)))" \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02k3pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02k3pi

    def _D023KPiFilter( self ):
        _decayDescriptors = [ '[D0 -> K+ K- K- pi+]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(Dto4h_AMassWin)s *MeV) & (APT > 1500.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (PT > 250 *MeV) & (P>2.0*GeV)"\
                           "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__}
        _motherCut = " (ADMASS('D0') < %(Dto4h_MassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (INTREE((ABSID=='pi+')& (PT > %(KPiPT)s *MeV) &(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)))" \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02k3pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02k3pi    

    def _Dp2KPiPiFilter( self ):
        _decayDescriptors = [ '[D+ -> K- pi+ pi+]cc' ]
        _combinationCut = "(ADAMASS('D+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('D+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _dp2kpipi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                    
        return _dp2kpipi

    def _Ds2KKPiFilter( self ):
        _decayDescriptors = [ '[D+ -> K+ K- pi+]cc' ]
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                             "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                             "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2kkpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                             
        return _ds2kkpi



    def _Ds2PiMuMuFilter( self ):
        _decayDescriptors = [ '[D+ -> pi+ mu+ mu-]cc' ]
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                             "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                             "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2pimumu = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                             
        return _ds2pimumu

    def _Ds2KMuMuFilter( self ):
        _decayDescriptors = [ '[D+ -> K+ mu+ mu-]cc' ]
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                             "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                             "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2pimumu = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                             
        return _ds2pimumu
    
    def _Ds2KsKFilter( self ):
        _decayDescriptors = [ '[D+ -> KS0 K+]cc' ]
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV)"\
                          "& (ACHILD(PT,1)+ACHILD(PT,2) > 1500.*MeV)" \
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1500.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2ksk = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                             
        return _ds2ksk


    def _Ds2KsPiFilter( self ):
        _decayDescriptors = [ '[D+ -> KS0 pi+]cc' ]
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV)"\
                          "& (ACHILD(PT,1)+ACHILD(PT,2) > 1500.*MeV)" \
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1500.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2kspi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                             
        return _ds2kspi

    
    def _Lc2PKPiFilter( self ):
        _decayDescriptors = [ '[Lambda_c+ -> K- p+ pi+]cc' ]
        _daughtersCuts = {  "p+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(KPiPT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>1.0e-10)" % self.__confdict__}
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) & (SUMTREE( PT,  ISBASIC )>1800.*MeV) & (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _lambdac = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    DaughtersCuts = _daughtersCuts,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                                                         
        return _lambdac

    def _Lc2PKPiDCSFilter( self ):
        _decayDescriptors = [ '[Lambda_c+ -> K+ p+ pi-]cc' ]
        _daughtersCuts = {  "p+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(KPiPT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>1.0e-10)" % self.__confdict__}
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) & (SUMTREE( PT,  ISBASIC )>1800.*MeV) & (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _lambdac = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    DaughtersCuts = _daughtersCuts,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                                                         
        return _lambdac
    
    def _D02KsPiPiFilter( self ):
        _decayDescriptors = [ '[D0 -> KS0 pi+ pi-]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02KsPiPi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02KsPiPi


    def _D02KsKKFilter( self ):
        _decayDescriptors = [ '[D0 -> KS0 K+ K-]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02KsKK = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02KsKK
    
    def _D02KsKPiFilter( self ):
        _decayDescriptors =  [ "[D0 -> KS0 K- pi+]cc" ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02KsKPi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                      CombinationCut = _combinationCut,
                                      MotherCut = _motherCut)                            
        return _d02KsKPi


    def _D02KsKsFilter( self ):
        _decayDescriptors =  [ "[D0 -> KS0 KS0]cc" ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02KsKs = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02KsKs
    

    def _D02HHPi0Filter( self , _decayDescriptors):
        #_decayDescriptors = ["[D0 -> K- pi+ pi0]cc"]
        _combinationCut = "(ADAMASS('D0') < %(D02HHPi0AMassWin)s *MeV) " \
                          " & (APT> %(D02HHPi0PtCut)s *MeV)" \
                          " & (ADOCA(1,2) < %(D02HHPi0DocaCut)s)" % self.__confdict__
        _motherCut = "(ADMASS('D0') < %(D02HHPi0MassWin)s *MeV) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(D02HHPi0PtCut)s *MeV) " \
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02HHPi0 = CombineParticles( DecayDescriptors = _decayDescriptors,
                                       CombinationCut = _combinationCut,
                                       MotherCut = _motherCut)
        return _d02HHPi0

    
    def _D02KPiPi0Filter( self ):
        _decayDescriptors = ["[D0 -> K- pi+ pi0]cc"]
        _combinationCut = "(ADAMASS('D0') < %(D02HHPi0AMassWin)s *MeV) " \
                          " & (APT> %(D02HHPi0PtCut)s *MeV)" \
                          " & (ADOCA(1,2) < %(D02HHPi0DocaCut)s)" % self.__confdict__
        _motherCut = "(ADMASS('D0') < %(D02HHPi0MassWin)s *MeV) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(D02HHPi0PtCut)s *MeV) " \
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02KPiPi0 = CombineParticles( DecayDescriptors = _decayDescriptors,
                                       CombinationCut = _combinationCut,
                                       MotherCut = _motherCut)
        return _d02KPiPi0

    
    def _D02PiPiPi0Filter( self ):
        _decayDescriptors = ["[D0 -> pi- pi+ pi0]cc" ]
        _combinationCut = "(ADAMASS('D0') < %(D02HHPi0AMassWin)s *MeV) " \
                          " & (APT> %(D02HHPi0PtCut)s *MeV)" \
                          " & (ADOCA(1,2) < %(D02HHPi0DocaCut)s)" % self.__confdict__
        _motherCut = "(ADMASS('D0') < %(D02HHPi0MassWin)s *MeV) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(D02HHPi0PtCut)s *MeV) " \
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02PiPiPi0 = CombineParticles( DecayDescriptors = _decayDescriptors,
                                       CombinationCut = _combinationCut,
                                       MotherCut = _motherCut)
        return _d02PiPiPi0

    def _D02KKPi0Filter( self ):
        _decayDescriptors = ["[D0 -> K- K+ pi0]cc" ]
        _combinationCut = "(ADAMASS('D0') < %(D02HHPi0AMassWin)s *MeV) " \
                          " & (APT> %(D02HHPi0PtCut)s *MeV)" \
                          " & (ADOCA(1,2) < %(D02HHPi0DocaCut)s)" % self.__confdict__
        _motherCut = "(ADMASS('D0') < %(D02HHPi0MassWin)s *MeV) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(D02HHPi0PtCut)s *MeV) " \
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02KKPi0 = CombineParticles( DecayDescriptors = _decayDescriptors,
                                       CombinationCut = _combinationCut,
                                       MotherCut = _motherCut)
        return _d02KKPi0


    def _Lc2L0PiFilter( self ):
        _decayDescriptors = [ '[Lambda_c+ -> Lambda0 pi+]cc' ]
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) " \
                          " & (ADOCACHI2CUT( %(DDocaChi2Max)s, '')) " \
                          " & (ACHILD(PT,1)+ACHILD(PT,2) > 1800.*MeV)" % self.__confdict__
        _motherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) " \
                     " & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     " & (SUMTREE( PT,  ISBASIC )>1800.*MeV) " \
                     " & (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _lambdac = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                                                         
        return _lambdac


    def _Lc2L0KFilter( self ):
        _decayDescriptors = [ '[Lambda_c+ -> Lambda0 K+]cc' ]
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) " \
                          " & (ADOCACHI2CUT( %(DDocaChi2Max)s, '')) " \
                          " & (ACHILD(PT,1)+ACHILD(PT,2) > 1800.*MeV)" % self.__confdict__
        _motherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) " \
                     " & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     " & (SUMTREE( PT,  ISBASIC )>1800.*MeV) " \
                     " & (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _lambdac = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                                                         
        return _lambdac



    def  _Lc2pPiPiFilter( self ):
        _decayDescriptors = [ '[Lambda_c+ -> p+ pi- pi+]cc' ]
        _daughtersCuts = {  "p+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(KPiPT)s *MeV) & (P>2.0*GeV)"\
                            "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>1.0e-10)" % self.__confdict__ }
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) & (SUMTREE( PT,  ISBASIC )>1800.*MeV) & (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _lambdac = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                                                         
        return _lambdac
    
    def  _Lc2pKKFilter( self ):
        _decayDescriptors = [ '[Lambda_c+ -> p+ K- K+]cc' ]
        _daughtersCuts = {  "p+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(KPiPT)s *MeV) & (P>2.0*GeV) "\
                            "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>1.0e-10)" % self.__confdict__}
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) & (SUMTREE( PT,  ISBASIC )>1800.*MeV) & (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _lambdac = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                                                         
        return _lambdac

def makeDstar(name, inputD0,Dstar_cuts) : 
    """
    Given a list of D0, try to make D*+ -> D0 pi+
    """
    _softPi = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
    _cutsSoftPi = '( PIDe-PIDpi < %(Dstar_SoftPion_PIDe)s )' % Dstar_cuts
    _cutsDstarComb = "ADAMASS('D*(2010)+') < %(Dstar_preFitMassCut)s *MeV" % Dstar_cuts
    _cutsDstarMoth_base = '(PT > %(Dstar_SoftPion_PT)s * MeV) & (VFASPF(VCHI2/VDOF) < %(Dstar_Chi2)s )' % Dstar_cuts
    _cutsDstarMoth_DM = '(MM - CHILD(MM,1) - CHILD(MM,2) > %(Dstar_wideDMCutLower)s *MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < %(Dstar_wideDMCutUpper)s *MeV)' % Dstar_cuts
    _cutsDstarMoth = '(' + _cutsDstarMoth_base + ' & ' + _cutsDstarMoth_DM + ')'
    _Dstar = CombineParticles( DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc",
                               DaughtersCuts = { "pi+" : _cutsSoftPi },
                               CombinationCut = _cutsDstarComb,
                               MotherCut = _cutsDstarMoth)
    return Selection ( name,Algorithm = _Dstar,RequiredSelections = inputD0 + [_softPi])

    
def makeb2DMuX(name,
               DecayDescriptors,
               MuSel,
               DSel,
               BCuts):
    _combinationCut = "(AM<6.2*GeV)"
    _motherCut = "  (MM<6.0*GeV) & (MM>2.5*GeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)  " \
                   "& (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > %(DZ)s *mm ) "  % BCuts
    _B = CombineParticles(DecayDescriptors = DecayDescriptors,
                          CombinationCut = _combinationCut,
                          MotherCut = _motherCut)
    return Selection (name,
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



