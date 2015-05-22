"""
Module for selecting charm decays from semileptonic B decays.
Following decays are included:
* D0 -> HHKs,HHPi0,HHHH,KsKs,HHKsPi0 (also lines with D* tag)
* D+ -> HHH(some),HKs,HMuMu,HHHPi0,HHHKs
* Lc+ -> pHH, pKs, pMuMu, HLambda
"""
__author__ = ['Mika Vesterinen']
__date__ = '08/03/2012'
__version__ = '$Revision: 0.9 $'

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, DaVinci__N4BodyDecays
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdLooseMergedPi0,StdLooseResolvedPi0, StdAllNoPIDsPions, StdNoPIDsUpPions, StdLooseAllPhotons, StdAllLooseGammaLL, StdAllLooseGammaDD
from Configurables import ConjugateNeutralPID
from PhysSelPython.Wrappers import MergedSelection

__all__ = ('CharmFromBSemiAllLinesConf',
           'makeb2DMuX',
           'makeb2DX',
           'makeDstar',
           'TOSFilter',
           'confdict')

default_config = {
    'NAME' : 'CharmFromBSemi',
    'BUILDERTYPE' : 'CharmFromBSemiAllLinesConf',
    'STREAMS' : ['Charm'] ,
    'WGs'    : ['Charm'],
    'CONFIG':{
    "GEC_nLongTrk"   : 250    # adimensional
    ,"TTSpecs"      : {'Hlt1.*Track.*Decision%TOS':0,'Hlt2Topo.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0,"Hlt2Global%TIS":0}
    ,"HLT2"   : "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision')"
    ,"TrGhostProbMax": 0.5    # adimensional
    ,"MinBMass"      : 2500   # MeV
    ,"MaxBMass"      : 6000   # MeV
    ,"MINIPCHI2"     : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PionPIDKTight" : 4.0    # adimensiional
    ,"MuonIPCHI2"    : 4.0    # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 250.0  # MeV
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
    ,"Dto4hADocaChi2Max"  : 7     #adimensiional
    ,"MINIPCHI2Loose": 4.0   #adimensiional
    ,"KaonPIDKloose" : -5     #adimensiional
    ,'KSCutZFDFromD' :   10.0  #mm
    ,'KSLLPMin'         : 2000  ## MeV
    ,'KSLLPTMin'        : 250   ## MeV
    ,'KSDDPMin'         : 3000  ## MeV
    ,'KSDDPTMin'        : 250   ## MeV
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
    ,"Pi0PtMin"         : 1000   # Minimum Pt of pi0 (MeV)
    ,"Pi0PMin"          : 3000   # Minimum P of pi0 (MeV)
    ,"PhotonCL"         : 0.25   # Confidence level for Pi0 photons
    ,"D02HHPi0AMassWin" : 220  # MeV (mass window for combination)
    ,"D02HHPi0MassWin"  : 210  # MeV (mass window after vertex fit)
    ,"D02KSHHPi0AMassWin" : 220  # MeV (mass window for combination)
    ,"D02KSHHPi0MassWin"  : 210  # MeV (mass window after vertex fit)
    ,"D02KSHHPi0_D0PTComb"	: 1000 # MeV (PT of D0 combination)
    ,"D02KSHHPi0_D0PT"	: 1000 # MeV (PT of D0 combination)
    ,"D02KSHHPi0_PTSUMLoose"  : 1000 # MeV (sum PT of D0 daughters)
    ,"D02HHPi0DocaCut"  : 6.0    # mm
    ,"Dstar_Chi2"       :  8.0 ## unitless
    ,"Dstar_SoftPion_PIDe" : 2. ## unitless
    ,"Dstar_SoftPion_PT" : 80. ## MeV ### 
    ,"Dstar_wideDMCutLower" : 0. ## MeV
    ,"Dstar_wideDMCutUpper" : 170. ## MeV
    ,"PTSUMLoose"  : 1400. ## MeV
    ,"PTSUM"       : 1800. ## MeV
    ,"PTSUM_HHPi0" : 1800. ## MeV
    #Photons
    ,"PhotonPT"                : 1500 ## MeV
    ,"MaxConvPhLLMass"         : 100 ## MeV
    ,"MaxConvPhLLChi"          : 9
    ,"MinConvPhLLPT"           : 800 ## MeV    
    ,"MaxConvPhDDMass"         : 100 ## MeV
    ,"MaxConvPhDDChi"          : 9
    ,"MinConvPhDDPT"           : 800 ## MeV
      # X -> HH
    ,'CombMassLow_HH'          : 0 ## MeV
    ,'CombMassHigh_HH'         : 1820 ## MeV
    ,'MassLow_HH'              : 0 ## MeV
    ,'MassHigh_HH'             : 1810 ## MeV
    ,'MaxVCHI2NDOF_HH'         : 10.0
    ,'MinVDCHI2_HH'            : 1000.0
      # D0 -> X Gamma
    ,'D02HHGammaAMassWin'      : 220  # MeV (mass window for combination)
    ,'D02HHGammaMassWin'       : 210  # MeV (mass window after vertex fit)
    ,'PTSUM_HHGamma'           : 1800. ## MeV
    ,"DtoXgammaADocaChi2Max"   : 10     #adimensiional     
        }
        }

class CharmFromBSemiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "GEC_nLongTrk"
        ,"TTSpecs"
        ,"HLT2" 
        ,"TrGhostProbMax"
        ,"MinBMass"      
        ,"MaxBMass"      
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
        ,"Dto4hADocaChi2Max"
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
        ,"Pi0PMin" 
        ,"PhotonCL"
        ,"D02HHPi0AMassWin"
        ,"D02HHPi0MassWin"
        ,"D02KSHHPi0AMassWin"
        ,"D02KSHHPi0MassWin"
        ,"D02KSHHPi0_D0PTComb"
        ,"D02KSHHPi0_D0PT"
        ,"D02KSHHPi0_PTSUMLoose"
        ,"D02HHPi0DocaCut"
        ,"Dstar_Chi2"
        ,"Dstar_SoftPion_PIDe"
        ,"Dstar_SoftPion_PT"
        ,"Dstar_wideDMCutLower"
        ,"Dstar_wideDMCutUpper"
        ,"PTSUMLoose"       
        ,"PTSUM"       
        ,"PTSUM_HHPi0"
        ,"PhotonPT"
        ,"MaxConvPhLLMass"
        ,"MaxConvPhLLChi"
        ,"MinConvPhLLPT"
        ,"MaxConvPhDDMass"
        ,"MaxConvPhDDChi"
        ,"MinConvPhDDPT"
        ,"CombMassLow_HH"
        ,"CombMassHigh_HH"
        ,"MassLow_HH"
        ,"MassHigh_HH"
        ,"MaxVCHI2NDOF_HH"
        ,"MinVDCHI2_HH"
        ,"D02HHGammaAMassWin"
        ,"D02HHGammaMassWin"
        ,"PTSUM_HHGamma"
        ,"DtoXgammaADocaChi2Max"
    )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                     "Preambulo": ["from LoKiTracks.decorators import *"]}
        
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
                 'DZ': config['DZ'],
                 "TTSpecs":config["TTSpecs"]}
        
        ##########################################################################
        ## FINAL STATE PARTICLE SELECTION ##
        ##########################################################################
        
        self.selmuon = Selection( "SelMufor" + name,
                                  Algorithm = self._muonFilter("Mufor"+name),
                                  RequiredSelections = [StdLooseMuons])
        
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
        
        self.AllKaonsAndPions = MergedSelection("SelAllKaonsAndPionsFor"+name, 
                                                RequiredSelections = [ self.selKaon,self.selPion])
        

        self.selPi0Resolved = Selection( "SelPi0Resolvedfor" + name,
                                         Algorithm = self._Pi0ResolvedFilter("Pi0Resolvedfor"+name),
                                         RequiredSelections = [StdLooseResolvedPi0])
        
        self.selPi0Merged = Selection( "SelPi0Mergedfor" + name,
                                       Algorithm = self._Pi0MergedFilter("PiMergedfor"+name),
                                       RequiredSelections = [StdLooseMergedPi0])

        self.AllPi0s = MergedSelection("SelAllPi0sFor"+name, 
                                       RequiredSelections = [ self.selPi0Resolved,self.selPi0Merged])

        self.selGamma = Selection("SelGammaFor"+name,
                                  Algorithm = self._PhotonFilter("GammaFor"+name),
                                  RequiredSelections = [ StdLooseAllPhotons ] )
        self.selGammaResLL = Selection("SelGammaResLLFor"+name,
                                       Algorithm = self._PhotonConvLLFilter("GammaLLFor"+name),
                                       RequiredSelections = [ StdAllLooseGammaLL ] )
        self.selGammaResDD = Selection("SelGammaResDDFor"+name,
                                       Algorithm = self._PhotonConvDDFilter("GammaDDFor"+name),
                                       RequiredSelections = [ StdAllLooseGammaDD ] )
        self.AllGammas = MergedSelection("SelAllGammasFor"+name,
                                         RequiredSelections = [self.selGamma, self.selGammaResLL, self.selGammaResDD] )

        self.selSlowPion = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
        self.selSlowPionUp = DataOnDemand(Location = 'Phys/StdLooseANNUpPions/Particles')
        self.selSlowPionAll = MergedSelection("AllSlowPionfor", RequiredSelections = [self.selSlowPion,self.selSlowPionUp])
        
        self.selKSLL = Selection("SelKsLLfor"+name,
                                 Algorithm = self._KsLLFilter("KsLLfor"+name),
                                 RequiredSelections = [DataOnDemand("Phys/StdLooseKsLL/Particles")])

        self.selKSDD = Selection("SelKsDDfor"+name,
                                 Algorithm = self._KsDDFilter("KsDDfor"+name),
                                 RequiredSelections = [DataOnDemand("Phys/StdLooseKsDD/Particles")])
        
        self.combLooseKsLL = CombineParticles ("KsLLCombFor"+name,
                                               DecayDescriptor = "KS0 -> pi+ pi-",
                                               CombinationCut = "(ADAMASS('KS0') < 27.*MeV) & (ADOCACHI2CUT(25, '')) ",
                                               MotherCut = "(ADMASS('KS0') < 25.*MeV) & ( BPVLTIME() > 3.0*ps) & (VFASPF(VCHI2) < 9.)")
        
        self.selLooseKsLL = Selection("SelKsLLFor"+name,
                                      Algorithm = self.combLooseKsLL,
                                      RequiredSelections = [ StdAllNoPIDsPions ])
        
        self.combKsLU_PosLong = CombineParticles("KsLUComb_PosLongFor"+name,
                                                 DecayDescriptor = "KS0 -> pi+ pi-",
                                                 DaughtersCuts = { "pi+" : "(ISLONG)",
                                                                   "pi-" : "(ISUP)" } ,
                                                 CombinationCut = "(ADAMASS('KS0') < 27.*MeV) & (ADOCACHI2CUT(25, '')) ",
                                                 MotherCut = "(ADMASS('KS0') < 25.*MeV) & ( BPVLTIME() > 3.0*ps) & (VFASPF(VCHI2) < 9.)")
        
        self.combKsLU_NegLong = CombineParticles("KsLUComb_NegLongFor"+name,
                                                 DecayDescriptor = "KS0 -> pi+ pi-",
                                                 DaughtersCuts = { "pi+" : "(ISUP)",
                                                                   "pi-" : "(ISLONG)" } ,
                                                 CombinationCut = "(ADAMASS('KS0') < 27.*MeV) & (ADOCACHI2CUT(25, '')) ",
                                                 MotherCut = "(ADMASS('KS0') < 25.*MeV) & ( BPVLTIME() > 3.0*ps) & (VFASPF(VCHI2) < 9.)")
        
        self.selKsLU_PosLong = Selection("SelKsLU_PosLongFor"+name,
                                         Algorithm = self.combKsLU_PosLong,
                                         RequiredSelections = [ StdAllNoPIDsPions,StdNoPIDsUpPions])
        
        self.selKsLU_NegLong = Selection("SelKsLU_NegLongFor"+name,
                                         Algorithm = self.combKsLU_NegLong,
                                         RequiredSelections = [ StdAllNoPIDsPions,StdNoPIDsUpPions])
        
        self.selAllKs_Loose = MergedSelection("SelMergedAllKsLooseFor"+name, 
                                              RequiredSelections = [ self.selKsLU_PosLong,self.selKsLU_NegLong,self.selLooseKsLL,self.selKSDD])
        
        _stdLooseLambdaLL = DataOnDemand("Phys/StdLooseLambdaLL/Particles")
        _stdLooseLambdaDD = DataOnDemand("Phys/StdLooseLambdaDD/Particles")

        self.selLambdaLL = Selection("SelLambdaLLfor"+name,
                                 Algorithm = self._LambdaLLFilter("LambdaLLfor"+name),
                                 RequiredSelections = [_stdLooseLambdaLL])

        self.selLambdaDD = Selection("SelLambdaDDfor"+name,
                                 Algorithm = self._LambdaDDFilter("LambdaDDfor"+name),
                                 RequiredSelections = [_stdLooseLambdaDD])
        
        self.selAllLambda = MergedSelection("SelAllLambdaFor"+name, 
                                            RequiredSelections = [ self.selLambdaLL,self.selLambdaDD])
        
        ##########################################################################
        ## D0 -> KsKs ##
        ## Contact: Mika Vesterinen
        ##########################################################################
        
        BCutsKsKs = BCuts.copy()
        BCutsKsKs["DZ"] = -9999
        
        self.sel_D0_to_KsKs = Selection('SelD02KsKsfor'+name,
                                        Algorithm = self._D02KsKsFilter(['D0 -> KS0 KS0'],'D02KsKsfor'+name),
                                        RequiredSelections = [self.selAllKs_Loose])
        self.sel_Dstar_to_KsKs = makeDstar('Dstar_KsKs_For'+name,self.sel_D0_to_KsKs,self.selSlowPion,Dstar_cuts)
        
        self.selb2D0MuXKsKs = makeb2DMuX('b2D0MuXKsKs' + name,
                                         [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                         self.selmuon,self.sel_D0_to_KsKs,BCuts)

        self.selb2DstarMuXKsKs = makeb2DMuX('b2DstarMuXKsKs' + name,
                                            [ '[B0 -> D*(2010)+ mu-]cc', '[B0 -> D*(2010)+ mu+]cc' ],
                                            self.selmuon,self.sel_Dstar_to_KsKs,BCuts)

        ##########################################################################
        ## D0 -> 4H SELECTION ##
        ## Contact: Maurizio Martinelli
        ##########################################################################

        self.sel_D0_to_4Pi = Selection( 'Sel_D0_to_4Pi_for' + name,
                                        Algorithm = self._D02HHHHFilter(['D0 -> pi- pi+ pi- pi+'],'D0_to_4Pi_for' + name),
                                        RequiredSelections = [self.selPion] )        
        self.sel_Dstar_to_4Pi = makeDstar('Dstar_4PiFor'+name,self.sel_D0_to_4Pi,self.selSlowPion,Dstar_cuts)


        self.sel_D0_to_K3Pi = Selection( 'Sel_D0_to_K3Pi_for' + name,
                                         Algorithm = self._D02HHHHFilter(['[D0 -> K- pi+ pi- pi+]cc'],'D0_to_K3Pi_for' + name),
                                         RequiredSelections = [self.selPion,self.selKaon] )        
        self.sel_Dstar_to_K3Pi = makeDstar('Dstar_K3PiFor'+name,self.sel_D0_to_K3Pi,self.selSlowPion,Dstar_cuts)


        self.sel_D0_to_2K2Pi = Selection( 'Sel_D0_to_2K2Pi_for' + name,
                                          Algorithm = self._D02HHHHFilter(['D0 -> K- K+ pi- pi+'],'D0_to_2K2Pi_for' + name),
                                          RequiredSelections = [self.selPion,self.selKaon] )        
        self.sel_Dstar_to_2K2Pi = makeDstar('Dstar_2K2PiFor'+name,self.sel_D0_to_2K2Pi,self.selSlowPion,Dstar_cuts)
        
        self.sel_D0_to_3KPi = Selection( 'Sel_D0_to_3KPi_for' + name,
                                         Algorithm = self._D02HHHHFilter(['[D0 -> K+ K- K- pi+]cc'],'D0_to_3KPi_for' + name),
                                         RequiredSelections = [self.selPion,self.selKaon] )        
        self.sel_Dstar_to_3KPi = makeDstar('Dstar_3KPiFor'+name,self.sel_D0_to_3KPi,self.selSlowPion,Dstar_cuts)
        
        BDecays = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        self.selb2D0MuXK3Pi = makeb2DMuX('b2D0MuXK3Pi' + name,BDecays,self.selmuon,self.sel_D0_to_K3Pi ,BCuts)
        self.selb2D0MuX4Pi = makeb2DMuX('b2D0MuX4Pi' + name,BDecays,self.selmuon,self.sel_D0_to_4Pi,BCuts)
        self.selb2D0MuX2K2Pi = makeb2DMuX('b2D0MuX2K2Pi' + name,BDecays,self.selmuon,self.sel_D0_to_2K2Pi,BCuts)
        self.selb2D0MuX3KPi = makeb2DMuX('b2D0MuX3KPi' + name,BDecays,self.selmuon,self.sel_D0_to_3KPi,BCuts)        

        BDecays = [ '[B~0 -> D*(2010)+ mu-]cc', '[B~0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXK3Pi = makeb2DMuX('b2DstarMuXK3Pi' + name,BDecays,self.selmuon,self.sel_Dstar_to_K3Pi ,BCuts)
        self.selb2DstarMuX4Pi = makeb2DMuX('b2DstarMuX4Pi' + name,BDecays,self.selmuon,self.sel_Dstar_to_4Pi,BCuts)
        self.selb2DstarMuX2K2Pi = makeb2DMuX('b2DstarMuX2K2Pi' + name,BDecays,self.selmuon,self.sel_Dstar_to_2K2Pi,BCuts)
        self.selb2DstarMuX3KPi = makeb2DMuX('b2DstarMuX3KPi' + name,BDecays,self.selmuon,self.sel_Dstar_to_3KPi,BCuts)        
        
        ##########################################################################
        ## D0 -> HHKs ##
        ## Contact: Stephanie Reichert
        ##########################################################################
        
        BCutsKsHH = BCuts.copy()
        BCutsKsHH["DZ"] = -9999
        
        ### ks Pi Pi 
        self.seld02KsPiPiLL = Selection( 'SelD02KsPiPiLLfor' + name,
                                         Algorithm = self._D02KsHHFilter(['[D0 -> KS0 pi+ pi-]cc'],'D02KsPiPiLLfor' + name),
                                         RequiredSelections = [self.selKSLL,self.selPionloose] )           
        self.selDstar_2KsPiPiLL = makeDstar('DstarPiPiLLFor'+name,self.seld02KsPiPiLL,self.selSlowPion,Dstar_cuts,True)

        self.seld02KsPiPiDD = Selection( 'SelD02KsPiPiDDfor' + name,
                                         Algorithm = self._D02KsHHFilter(['[D0 -> KS0 pi+ pi-]cc'],'D02KsPiPiDDfor' + name),
                                         RequiredSelections = [self.selKSDD,self.selPionloose] )           
        self.selDstar_2KsPiPiDD = makeDstar('DstarPiPiDDFor'+name,self.seld02KsPiPiDD,self.selSlowPion,Dstar_cuts,True)

        ### ks K K 
        self.seld02KsKKLL = Selection( 'SelD02KsKKLLfor' + name,
                                       Algorithm = self._D02KsHHFilter(['[D0 -> KS0 K+ K-]cc'],'D02KsKKLLfor' + name),
                                       RequiredSelections = [self.selKSLL,self.selKaonloose] )           
        self.selDstar_2KsKKLL = makeDstar('DstarKKLLFor'+name,self.seld02KsKKLL,self.selSlowPion,Dstar_cuts,True)

        self.seld02KsKKDD = Selection( 'SelD02KsKKDDfor' + name,
                                       Algorithm = self._D02KsHHFilter(['[D0 -> KS0 K+ K-]cc'],'D02KsKKDDfor' + name),
                                       RequiredSelections = [self.selKSDD,self.selKaonloose] )           
        self.selDstar_2KsKKDD = makeDstar('DstarKKDDFor'+name,self.seld02KsKKDD,self.selSlowPion,Dstar_cuts,True)

        ### ks K Pi 
        self.seld02KsKPiLL = Selection( 'SelD02KsKPiLLfor' + name,
                                        Algorithm = self._D02KsHHFilter(['[D0 -> KS0 K- pi+]cc','[D0 -> KS0 K+ pi-]cc'],'D02KsKPiLLfor' + name),
                                        RequiredSelections = [self.selKSLL,self.selPionloose,self.selKaonloose] )           
        self.selDstar_2KsKPiLL = makeDstar('DstarKPiLLFor'+name,self.seld02KsKPiLL,self.selSlowPion,Dstar_cuts,True)
        
        self.seld02KsKPiDD = Selection( 'SelD02KsKPiDDfor' + name,
                                        Algorithm = self._D02KsHHFilter(['[D0 -> KS0 K- pi+]cc','[D0 -> KS0 K+ pi-]cc'],'D02KsKPiDDfor' + name),
                                        RequiredSelections = [self.selKSDD,self.selPionloose,self.selKaonloose] )           
        self.selDstar_2KsKPiDD = makeDstar('DstarKPiDDFor'+name,self.seld02KsKPiDD,self.selSlowPion,Dstar_cuts,True)
        
        BDecays = [ '[B- -> D0 mu-]cc']
        self.selb2D0MuXKsPiPiLL = makeb2DMuX('b2D0MuXKsPiPiLL' + name,BDecays,self.selmuon,self.seld02KsPiPiLL,BCutsKsHH)
        self.selb2D0MuXKsPiPiDD = makeb2DMuX('b2D0MuXKsPiPiDD' + name,BDecays,self.selmuon,self.seld02KsPiPiDD,BCutsKsHH)
        self.selb2D0MuXKsKKLL = makeb2DMuX('b2D0MuXKsKKLL' + name,BDecays,self.selmuon,self.seld02KsKKLL,BCutsKsHH)
        self.selb2D0MuXKsKKDD = makeb2DMuX('b2D0MuXKsKKDD' + name,BDecays,self.selmuon,self.seld02KsKKDD,BCutsKsHH)
        BDecays = [ '[B- -> D0 mu-]cc'] # since already have both D0 -> KS0 K-pi+ and KS0 K+pi-
        self.selb2D0MuXKsKPiLL = makeb2DMuX('b2D0MuXKsKPiLL' + name,BDecays,self.selmuon,self.seld02KsKPiLL,BCutsKsHH)
        self.selb2D0MuXKsKPiDD = makeb2DMuX('b2D0MuXKsKPiDD' + name,BDecays,self.selmuon,self.seld02KsKPiDD,BCutsKsHH)

        #### Dstar
        BDecays = [ '[B~0 -> D*(2010)+ mu-]cc', '[B~0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXKsPiPiLL = makeb2DMuX('b2DstarMuXKsPiPiLL'+name,BDecays,self.selmuon,self.selDstar_2KsPiPiLL,BCutsKsHH)
        self.selb2DstarMuXKsPiPiDD = makeb2DMuX('b2DstarMuXKsPiPiDD'+name,BDecays,self.selmuon,self.selDstar_2KsPiPiDD,BCutsKsHH)
        self.selb2DstarMuXKsKKLL = makeb2DMuX('b2DstarMuXKsKKLL'+name,BDecays,self.selmuon,self.selDstar_2KsKKLL,BCutsKsHH)
        self.selb2DstarMuXKsKKDD = makeb2DMuX('b2DstarMuXKsKKDD'+name,BDecays,self.selmuon,self.selDstar_2KsKKDD,BCutsKsHH)
        self.selb2DstarMuXKsKPiLL = makeb2DMuX('b2DstarMuXKsKPiLL'+name,BDecays,self.selmuon,self.selDstar_2KsKPiLL,BCutsKsHH)
        self.selb2DstarMuXKsKPiDD = makeb2DMuX('b2DstarMuXKsKPiDD'+name,BDecays,self.selmuon,self.selDstar_2KsKPiDD,BCutsKsHH)
        
        ##########################################################################
        ## D0 -> HHKsPi0 ##
        ## Contact: Daniel Johnson
        ##########################################################################

        ### ks Pi Pi Pi0 (merged PI0)
        self.seld02KsPiPiPi0MergedLL = Selection( 'SelD02KsPiPiPi0MergedLLfor' + name,
                                         Algorithm = self._D02KsHHPi0Filter(['[D0 -> KS0 pi+ pi- pi0]cc'],'D02KsPiPiPi0MergedLLfor' + name),
                                         RequiredSelections = [self.selKSLL,self.selPionloose,self.selPi0Merged] )           
        self.selDstar_2KsPiPiPi0MergedLL = makeDstar('DstarPiPiPi0MergedLLFor'+name,self.seld02KsPiPiPi0MergedLL,self.selSlowPion,Dstar_cuts,True)

        self.seld02KsPiPiPi0MergedDD = Selection( 'SelD02KsPiPiPi0MergedDDfor' + name,
                                         Algorithm = self._D02KsHHPi0Filter(['[D0 -> KS0 pi+ pi- pi0]cc'],'D02KsPiPiPi0MergedDDfor' + name),
                                         RequiredSelections = [self.selKSDD,self.selPionloose,self.selPi0Merged] )           
        self.selDstar_2KsPiPiPi0MergedDD = makeDstar('DstarPiPiPi0MergedDDFor'+name,self.seld02KsPiPiPi0MergedDD,self.selSlowPion,Dstar_cuts,True)
        ### ks Pi Pi Pi0 (resolved PI0)
        self.seld02KsPiPiPi0ResolvedLL = Selection( 'SelD02KsPiPiPi0ResolvedLLfor' + name,
                                         Algorithm = self._D02KsHHPi0Filter(['[D0 -> KS0 pi+ pi- pi0]cc'],'D02KsPiPiPi0ResolvedLLfor' + name),
                                         RequiredSelections = [self.selKSLL,self.selPionloose,self.selPi0Resolved] )           
        self.selDstar_2KsPiPiPi0ResolvedLL = makeDstar('DstarPiPiPi0ResolvedLLFor'+name,self.seld02KsPiPiPi0ResolvedLL,self.selSlowPion,Dstar_cuts,True)

        self.seld02KsPiPiPi0ResolvedDD = Selection( 'SelD02KsPiPiPi0ResolvedDDfor' + name,
                                         Algorithm = self._D02KsHHPi0Filter(['[D0 -> KS0 pi+ pi- pi0]cc'],'D02KsPiPiPi0ResolvedDDfor' + name),
                                         RequiredSelections = [self.selKSDD,self.selPionloose,self.selPi0Resolved] )           
        self.selDstar_2KsPiPiPi0ResolvedDD = makeDstar('DstarPiPiPi0ResolvedDDFor'+name,self.seld02KsPiPiPi0ResolvedDD,self.selSlowPion,Dstar_cuts,True)
        
        BDecays = [ '[B- -> D0 mu-]cc']
        self.selb2D0MuXKsPiPiPi0MergedLL = makeb2DMuX('b2D0MuXKsPiPiPi0MergedLL' + name,BDecays,self.selmuon,self.seld02KsPiPiPi0MergedLL,BCuts)
        self.selb2D0MuXKsPiPiPi0MergedDD = makeb2DMuX('b2D0MuXKsPiPiPi0MergedDD' + name,BDecays,self.selmuon,self.seld02KsPiPiPi0MergedDD,BCuts)
        self.selb2D0MuXKsPiPiPi0ResolvedLL = makeb2DMuX('b2D0MuXKsPiPiPi0ResolvedLL' + name,BDecays,self.selmuon,self.seld02KsPiPiPi0ResolvedLL,BCuts)
        self.selb2D0MuXKsPiPiPi0ResolvedDD = makeb2DMuX('b2D0MuXKsPiPiPi0ResolvedDD' + name,BDecays,self.selmuon,self.seld02KsPiPiPi0ResolvedDD,BCuts)

        BDecays = [ '[B~0 -> D*(2010)+ mu-]cc', '[B~0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXKsPiPiPi0MergedLL = makeb2DMuX('b2DstarMuXKsPiPiPi0MergedLL' + name,BDecays,self.selmuon,self.selDstar_2KsPiPiPi0MergedLL,BCuts)
        self.selb2DstarMuXKsPiPiPi0MergedDD = makeb2DMuX('b2DstarMuXKsPiPiPi0MergedDD' + name,BDecays,self.selmuon,self.selDstar_2KsPiPiPi0MergedDD,BCuts)
        self.selb2DstarMuXKsPiPiPi0ResolvedLL = makeb2DMuX('b2DstarMuXKsPiPiPi0ResolvedLL' + name,BDecays,self.selmuon,self.selDstar_2KsPiPiPi0ResolvedLL,BCuts)
        self.selb2DstarMuXKsPiPiPi0ResolvedDD = makeb2DMuX('b2DstarMuXKsPiPiPi0ResolvedDD' + name,BDecays,self.selmuon,self.selDstar_2KsPiPiPi0ResolvedDD,BCuts)
        
        ##########################################################################
        ## D0 -> HHPi0 ##
        ## Contact: Mika Vesterinen
        ##########################################################################
        
        self.CombD0ToHH_SEED = CombineParticles("CombD0ToHH_SEED_for"+name,
                                                DecayDescriptors = ["K*(892)0 -> pi+ pi-",
                                                                    "K*(892)0 -> K+ K-",
                                                                    "K*(892)0 -> K- pi+",
                                                                    "K*(892)0 -> K+ pi-"],
                                                CombinationCut = "(ACHILD(PT,1)+ACHILD(PT,2) > 1500*MeV)"\
                                                    "& (AM < 2000*MeV)"\
                                                    "& (AM - ACHILD(M,1)-ACHILD(M,2) > 50*MeV)"\
                                                    "& (ADOCACHI2CUT(25, ''))"\
                                                    "& (ACHILD(MIPDV(PRIMARY),1)+ACHILD(MIPDV(PRIMARY),2) > 0.5*mm)",
                                                MotherCut = "(VFASPF(VCHI2PDOF) < 4.0) & (BPVVD > 3*mm) & (BPVDIRA > 0.99)")
        
        self.SelD0ToHH_SEED = Selection("SelD0ToHH_SEED_for"+name,
                                        Algorithm = self.CombD0ToHH_SEED,
                                        RequiredSelections = [self.AllKaonsAndPions])
        
        self.selD0ToHHPi0 = self.GenericCharmMaker('DToHHPi0For'+name,
                                                   ["[D0 -> K*(892)0 pi0]cc"],
                                                   "(ADAMASS('D+') < 180*MeV) & (ACHILD(PT,1)+ACHILD(PT,2)> 2500*MeV)",
                                                   "(abs(DTF_FUN(M,False,'pi0')-1865) < 150*MeV)",        
                                                   [self.SelD0ToHH_SEED,self.AllPi0s])

        self.selDstar_D0ToHHPi0 = makeDstar('DstarD0ToHHPi0For'+name,self.selD0ToHHPi0,self.selSlowPion,Dstar_cuts)
        
        BDecays = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        self.selb2D0MuXHHPi0 = makeb2DMuX('b2D0MuXHHPi0'+name,
                                          [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                          self.selmuon,self.selD0ToHHPi0,BCuts)
        
        self.selb2DstarMuXHHPi0 = makeb2DMuX('b2DstarMuXHHPi0'+name,
                                             [ '[B~0 -> D*(2010)+ mu-]cc','[B~0 -> D*(2010)+ mu+]cc'],
                                             self.selmuon,self.selDstar_D0ToHHPi0,BCuts)

        ##########################################################################
        ## D+ -> HHH(Ks/Pi0) ##
        ## Contact: Maurizo Martinelli
        ##########################################################################
        
        self.CombDToHHH_SEED = CombineParticles("CombDToHHH_SEED",
                                                DecayDescriptors = ["[K*(892)+ -> pi+ pi- pi+]cc",
                                                                    "[K*(892)+ -> K+ K- pi+]cc",
                                                                    "[K*(892)+ -> K- pi+ pi+]cc",
                                                                    "[K*(892)+ -> K+ pi+ pi-]cc",
                                                                    "[K*(892)+ -> K+ K+ pi-]cc",
                                                                    "[K*(892)+ -> K- K+ K+]cc"],
                                                CombinationCut = "(ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 2000*MeV)"\
                                                    "& (AM < 2000*MeV)"\
                                                    "& (AM - ACHILD(M,1)-ACHILD(M,2)-ACHILD(M,3) > 50*MeV)"\
                                                    "& (ADOCACHI2CUT(25, ''))"\
                                                    "& (ACHILD(MIPDV(PRIMARY),1)+ACHILD(MIPDV(PRIMARY),2)+ACHILD(MIPDV(PRIMARY),3) > 1.0*mm)",
                                                MotherCut = "(VFASPF(VCHI2PDOF) < 4.0) & (BPVVD > 5*mm)")
                                              
        self.SelDToHHH_SEED = Selection("SelDToHHH_SEED",
                                        Algorithm = self.CombDToHHH_SEED,
                                        RequiredSelections = [self.AllKaonsAndPions])
        
        self.selDToHHHPi0 = self.GenericCharmMaker('DToHHHPi0For'+name,
                                                   ["[D+ -> K*(892)+ pi0]cc"],
                                                   "(DAMASS('D_s+') < 120 *MeV) & (DAMASS('D+')> -120 *MeV)"\
                                                       "& (ACHILD(PT,1)+ACHILD(PT,2)> 3000*MeV)",
                                                   "  (DTF_FUN(M,False,'pi0') > 1869-110*MeV)"\
                                                       "& (DTF_FUN(M,False,'pi0') < 1968+110*MeV)",
                                                   [self.SelDToHHH_SEED,self.AllPi0s])
        
        self.selDToHHHKs = self.GenericCharmMaker('DToHHHKsFor'+name,
                                                  ["[D+ -> K*(892)+ KS0]cc"],
                                                  "(DAMASS('D_s+') < 90 *MeV) & (DAMASS('D+')> -90 *MeV)"\
                                                      "& (ACHILD(PT,1)+ACHILD(PT,2)> 3000*MeV)"\
                                                      "& (ACHILD(VFASPF(VZ),2)-ACHILD(VFASPF(VZ),1) > 5*mm)",
                                                  "  (DTF_FUN(M,False,'KS0') > 1869-80*MeV)"\
                                                      "& (DTF_FUN(M,False,'KS0') < 1968+80*MeV)",
                                                  [self.SelDToHHH_SEED,self.selAllKs_Loose])
        
        self.selb2DpMuX_HHHPi0 = makeb2DMuX('b2D0MuX_HHHPi0' + name,[ '[B~0 -> D+ mu-]cc'],self.selmuon,self.selDToHHHPi0,BCuts)
        self.selb2DpMuX_HHHKs = makeb2DMuX('b2D0MuX_HHHKs' + name,[ '[B~0 -> D+ mu-]cc'],self.selmuon,self.selDToHHHKs,BCuts)

        ##########################################################################
        ## D+ -> HHH/HKs/HMuMu ##
        ## Contact: Mika Vesterinen
        ##########################################################################

        self.selds2KKPiDCS = self._D2HHHFilter('Ds2KKPiDCSfor'+name,['[D+ -> K+ K+ pi-]cc'],[self.selKaon,self.selPion])
        self.selds2KKK = self._D2HHHFilter('Ds2KKKfor'+name,['[D+ -> K+ K+ K-]cc'],[self.selKaon])
        
        self.selds2KsK = Selection( 'SelDs2KsKfor' + name,
                                      Algorithm = self._Ds2KsHFilter(['[D+ -> KS0 K+]cc'],'Ds2KsLLKfor' + name),
                                      RequiredSelections = [self.selKaon, self.selAllKs_Loose] )
        
        self.selds2KsPi = Selection( 'SelDs2KsPifor' + name,
                                     Algorithm = self._Ds2KsHFilter(['[D+ -> KS0 pi+]cc'],'Ds2KsLLPifor' + name),
                                     RequiredSelections = [self.selPion, self.selAllKs_Loose] )
        
        self.sel_D_to_HMuMu = Selection( 'SelDs2HMuMufor' + name,
                                         Algorithm = self._Ds2HMuMuFilter([ '[D+ -> pi+ mu+ mu-]cc','[D+ -> K+ mu+ mu-]cc' ],'Ds2HMuMufor' + name),
                                         RequiredSelections = [self.selmuon,self.AllKaonsAndPions])

        BDecays = [ '[B~0 -> D+ mu-]cc', '[B~0 -> D+ mu+]cc' ]
        self.selb2DsMuXKKPiDCS = makeb2DMuX('b2DsMuXKKPiDCS'+name,BDecays,self.selmuon,self.selds2KKPiDCS,BCuts)
        self.selb2DsMuXKKK = makeb2DMuX('b2DsMuXKKK'+name,BDecays,self.selmuon,self.selds2KKK,BCuts)
        self.selb2DsMuXKsK = makeb2DMuX('b2DsMuXKsK'+name,BDecays,self.selmuon,self.selds2KsK,BCuts)
        self.selb2DsMuXKsPi = makeb2DMuX('b2DsMuXKsPi'+name,BDecays,self.selmuon,self.selds2KsPi,BCuts)
        self.selb2DsMuXHMuMu = makeb2DMuX('b2DsMuXHMuMu'+name,BDecays,self.selmuon,self.sel_D_to_HMuMu,BCuts)

        ##########################################################################
        ## Lc-X ##
        ## Contact: Alex Pearce
        ##########################################################################

        self.selLc2L0Pi = Selection( 'SelLc2LambdaPifor' + name,
                                     Algorithm = self._Lc2L0HFilter([ '[Lambda_c+ -> Lambda0 pi+]cc' ],'Lc2LambdaPifor' + name),
                                     RequiredSelections = [self.selPion, self.selAllLambda])
        
        self.selLc2L0K = Selection( 'SelLc2LambdaKfor' + name,
                                    Algorithm = self._Lc2L0HFilter([ '[Lambda_c+ -> Lambda0 K+]cc' ],'Lc2LambdaKfor' + name),
                                    RequiredSelections = [self.selKaon, self.selAllLambda])

        self.selLc2pKK = Selection( 'SelLc2pKKfor' + name,
                                    Algorithm = self._Lc2pHHFilter([ '[Lambda_c+ -> p+ K- K+]cc' ],'Lc2pKKfor' + name),
                                    RequiredSelections = [self.selKaon, self.selProton])

        self.selLc2pPiPi = Selection( 'SelLc2pPiPifor' + name,
                                      Algorithm = self._Lc2pHHFilter([ '[Lambda_c+ -> p+ pi- pi+]cc' ],'Lc2pPiPifor' + name),
                                      RequiredSelections = [self.selPionTight, self.selProton])## tighter pion PID needed here to reduce retention
        
        self.selLc2pMuMu = Selection( 'SelLc2pMuMufor' + name,
                                      Algorithm = self._Lc2pHHFilter([ '[Lambda_c+ -> p+ mu- mu+]cc', '[Lambda_c+ -> p+ mu+ mu+]cc', 
                                          '[Lambda_c+ -> p~- mu+ mu-]cc', '[Lambda_c+ -> p~- mu+ mu+]cc' ],'Lc2pMuMufor' + name),
                                      RequiredSelections = [StdLooseMuons, self.selProton])
        
        self.selLc2pKpi = Selection( 'SelLc2PKPifor' + name,
                                     Algorithm = self._Lc2pHHFilter([ '[Lambda_c+ -> K- p+ pi+]cc' ],'Lc2PKPifor' + name),
                                     RequiredSelections = [self.selKaon, self.selPion, self.selProton ] )
        
        self.selLc2pKpiDCS = Selection( 'SelLc2PKPiDCSfor' + name,
                                        Algorithm = self._Lc2pHHFilter([ '[Lambda_c+ -> K+ p+ pi-]cc' ],'Lc2PKPiDCSfor' + name),
                                        RequiredSelections = [self.selKaon, self.selPion, self.selProton ] )
        
        self.sel_Lc2pKs = Selection("selLc2pKsfor"+name,
                                    Algorithm =  self._Lc2pKsFilter( ['[Lambda_c+ -> p+ KS0]cc'], 'Lc2pKsfor' + name),
                                    RequiredSelections = [self.selProton, self.selAllKs_Loose ] )
        
        
        BDecays = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc']
        self.selb2Lc2L0PiMuX = makeb2DMuX('b2Lc2L0PiMuX' + name,BDecays,self.selmuon,self.selLc2L0Pi,BCuts)
        self.selb2Lc2L0KMuX = makeb2DMuX('b2Lc2L0KMuX' + name,BDecays,self.selmuon,self.selLc2L0K,BCuts)
        self.selb2LcMuX = makeb2DMuX('b2LcMuX' + name,BDecays,self.selmuon, self.selLc2pKpi ,BCuts)
        self.selb2LcDCSMuX = makeb2DMuX('b2LcDCSMuX' + name, BDecays,self.selmuon,self.selLc2pKpiDCS,BCuts)
        self.selb2Lc2pPiPiMuX = makeb2DMuX('b2Lc2pPiPiMuX' + name, BDecays,self.selmuon,self.selLc2pPiPi,BCuts)
        self.selb2Lc2pMuMuX = makeb2DMuX('b2Lc2pMuMuX' + name, BDecays,self.selmuon,self.selLc2pMuMu,BCuts)
        self.selb2Lc2pKKMuX = makeb2DMuX('b2Lc2pKKMuX' + name, BDecays,self.selmuon,self.selLc2pKK,BCuts)
        self.selb2LcMuXpKs = makeb2DMuX('b2LcMuXpKs' + name,BDecays,self.selmuon, self.sel_Lc2pKs,BCuts)

        ##########################################################################
        ## D0 -> X gamma SELECTION ##
        ## Contact: Maurizio Martinelli
        ##########################################################################

        # Two-body combinations
        self.sel_X_to_PiPi = self._makeHHForD2XGamma('X_to_PiPi_for'+ name, ['rho(770)0 -> pi+ pi-'], [self.selPion])
        self.sel_X_to_KPi  = self._makeHHForD2XGamma('X_to_KPi_for'+ name , ['K*(892)0 -> K- pi+', 'K*(892)0 -> K+ pi-'], [self.selPion, self.selKaon])
        self.sel_X_to_KK   = self._makeHHForD2XGamma('X_to_KK_for'+ name  , ['phi(1020) -> K+ K-'], [self.selKaon])
        # D0 candidate
        self.sel_D0_to_PiPiGamma = self._D02XGammaFilter('D0_to_PiPiGamma_for' + name, ['D0 -> rho(770)0 gamma'], [self.sel_X_to_PiPi, self.AllGammas] )
        self.sel_D0_to_KPiGamma  = self._D02XGammaFilter('D0_to_KPiGamma_for' + name , ['D0 -> K*(892)0 gamma'] , [self.sel_X_to_KPi , self.AllGammas] )
        self.sel_D0_to_KKGamma   = self._D02XGammaFilter('D0_to_KKGamma_for' + name  , ['D0 -> phi(1020) gamma'], [self.sel_X_to_KK  , self.AllGammas] )
        # D* candidate
        self.sel_Dstar_to_PiPiGamma = makeDstar('Dstar_PiPiGammaFor'+name,self.sel_D0_to_PiPiGamma,self.selSlowPion,Dstar_cuts)
        self.sel_Dstar_to_KPiGamma  = makeDstar('Dstar_KPiGammaFor'+name ,self.sel_D0_to_KPiGamma ,self.selSlowPion,Dstar_cuts)
        self.sel_Dstar_to_KKGamma   = makeDstar('Dstar_KKGammaFor'+name  ,self.sel_D0_to_KKGamma  ,self.selSlowPion,Dstar_cuts)
        # B->D0muX candidate
        BDecays = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
        self.selb2D0MuXPiPiGamma = makeb2DMuX('b2D0MuXPiPiGamma' + name,BDecays,self.selmuon,self.sel_D0_to_PiPiGamma ,BCuts)
        self.selb2D0MuXKPiGamma  = makeb2DMuX('b2D0MuXKPiGamma'  + name,BDecays,self.selmuon,self.sel_D0_to_KPiGamma  ,BCuts)
        self.selb2D0MuXKKGamma   = makeb2DMuX('b2D0MuXKKGamma'   + name,BDecays,self.selmuon,self.sel_D0_to_KKGamma   ,BCuts)
        # B->D*muX candidate
        BDecays = [ '[B~0 -> D*(2010)+ mu-]cc', '[B~0 -> D*(2010)+ mu+]cc' ]
        self.selb2DstarMuXPiPiGamma = makeb2DMuX('b2DstarMuXPiPiGamma' + name,BDecays,self.selmuon,self.sel_Dstar_to_PiPiGamma,BCuts)
        self.selb2DstarMuXKPiGamma  = makeb2DMuX('b2DstarMuXKPiGamma'  + name,BDecays,self.selmuon,self.sel_Dstar_to_KPiGamma ,BCuts)
        self.selb2DstarMuXKKGamma   = makeb2DMuX('b2DstarMuXKKGamma'   + name,BDecays,self.selmuon,self.sel_Dstar_to_KKGamma  ,BCuts)

        ##########################################################################
        ## Line registration ##
        ##########################################################################
        
        ########### D0 decays
        for line in [['b2D0MuXHHPi0',self.selb2D0MuXHHPi0],
                     ['b2D0MuXKsPiPiPi0MergedLL',self.selb2D0MuXKsPiPiPi0MergedLL],
                     ['b2D0MuXKsPiPiPi0MergedDD',self.selb2D0MuXKsPiPiPi0MergedDD],
                     ['b2D0MuXKsPiPiPi0ResolvedLL',self.selb2D0MuXKsPiPiPi0ResolvedLL],
                     ['b2D0MuXKsPiPiPi0ResolvedDD',self.selb2D0MuXKsPiPiPi0ResolvedDD],
                     ['b2D0MuXKsPiPiLL',self.selb2D0MuXKsPiPiLL],
                     ['b2D0MuXKsPiPiDD',self.selb2D0MuXKsPiPiDD],
                     ['b2D0MuXKsKKLL',self.selb2D0MuXKsKKLL],
                     ['b2D0MuXKsKKDD',self.selb2D0MuXKsKKDD],
                     ['b2D0MuXKsKPiLL',self.selb2D0MuXKsKPiLL],
                     ['b2D0MuXKsKPiDD',self.selb2D0MuXKsKPiDD],
                     ['b2D0MuXK3Pi',self.selb2D0MuXK3Pi],
                     ['b2D0MuX4Pi',self.selb2D0MuX4Pi],
                     ['b2D0MuX2K2Pi',self.selb2D0MuX2K2Pi],
                     ['b2D0MuX3KPi',self.selb2D0MuX3KPi],
                     ['b2D0MuXKsKs',self.selb2D0MuXKsKs],
                     ['b2D0MuXPiPiGamma',self.selb2D0MuXPiPiGamma],
                     ['b2D0MuXKPiGamma',self.selb2D0MuXKPiGamma],
                     ['b2D0MuXKKGamma',self.selb2D0MuXKKGamma]
                     ]:
            self.registerLine( StrippingLine('%s%sLine'%(line[0],name),
                                             prescale = 1,
                                             FILTER = GECs,
                                             HLT2 = config["HLT2"],
                                             selection = line[1]))
        ########### D* decays
        for line in [['b2DstarMuXHHPi0',self.selb2DstarMuXHHPi0],
                     ['b2DstarMuXKsPiPiPi0MergedLL',self.selb2DstarMuXKsPiPiPi0MergedLL],
                     ['b2DstarMuXKsPiPiPi0MergedDD',self.selb2DstarMuXKsPiPiPi0MergedDD],
                     ['b2DstarMuXKsPiPiPi0ResolvedLL',self.selb2DstarMuXKsPiPiPi0ResolvedLL],
                     ['b2DstarMuXKsPiPiPi0ResolvedDD',self.selb2DstarMuXKsPiPiPi0ResolvedDD],
                     ['b2DstarMuXKsPiPiLL',self.selb2DstarMuXKsPiPiLL],
                     ['b2DstarMuXKsPiPiDD',self.selb2DstarMuXKsPiPiDD],
                     ['b2DstarMuXKsKKLL',self.selb2DstarMuXKsKKLL],
                     ['b2DstarMuXKsKKDD',self.selb2DstarMuXKsKKDD],
                     ['b2DstarMuXKsKPiLL',self.selb2DstarMuXKsKPiLL],
                     ['b2DstarMuXKsKPiDD',self.selb2DstarMuXKsKPiDD],
                     ['b2DstarMuXK3Pi',self.selb2DstarMuXK3Pi],
                     ['b2DstarMuX4Pi',self.selb2DstarMuX4Pi],
                     ['b2DstarMuX2K2Pi',self.selb2DstarMuX2K2Pi],
                     ['b2DstarMuX3KPi',self.selb2DstarMuX3KPi],
                     ['b2DstarMuXKsKs',self.selb2DstarMuXKsKs],
                     ['b2DstarMuXPiPiGamma',self.selb2DstarMuXPiPiGamma],
                     ['b2DstarMuXKPiGamma',self.selb2DstarMuXKPiGamma],
                     ['b2DstarMuXKKGamma',self.selb2DstarMuXKKGamma]
                     ]:
            self.registerLine( StrippingLine('%s%sLine'%(line[0],name),
                                             prescale = 1,
                                             FILTER = GECs,
                                             HLT2 = config["HLT2"],
                                             selection = line[1]))
                
        ########### D(s)+ decays
        for line in [['b2DpMuXKKPiDCS',self.selb2DsMuXKKPiDCS],
                     ['b2DpMuXKKK',self.selb2DsMuXKKK],
                     ['b2DpMuXKsK',self.selb2DsMuXKsK],
                     ['b2DpMuXKsPi',self.selb2DsMuXKsPi],
                     ['b2DpMuXHMuMu',self.selb2DsMuXHMuMu],
                     ['b2DpMuXHHHKs',self.selb2DpMuX_HHHKs],
                     ['b2DpMuXHHHPi0',self.selb2DpMuX_HHHPi0]]:
            self.registerLine( StrippingLine('%s%sLine'%(line[0],name),
                                             prescale = 1,
                                             FILTER = GECs,
                                             HLT2 = config["HLT2"],
                                             selection = line[1]))
        
        ########## Lambda_c+ -> decays
        for line in [['b2LcMuXL0Pi',self.selb2Lc2L0PiMuX],
                     ['b2LcMuXL0K',self.selb2Lc2L0KMuX],
                     ['b2LcMuX',self.selb2LcMuX],
                     ['b2LcMuXDCS',self.selb2LcDCSMuX],
                     ['b2LcMuXpPiPi',self.selb2Lc2pPiPiMuX],
                     ['b2LcMuXpKK',self.selb2Lc2pKKMuX],
                     ['b2LcMuXpMuMu',self.selb2Lc2pMuMuX],
                     ['b2LcMuXpKs',self.selb2LcMuXpKs]]:
            self.registerLine( StrippingLine('%s%sLine'%(line[0],name),
                                             prescale = 1,
                                             FILTER = GECs,
                                             HLT2 = config["HLT2"],
                                             selection = line[1]))
        
        
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
                " & (BPVDIRA > %(KSCutDIRA)s)" % self.__confdict__
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

    def _PhotonFilter( self, _name):
        _code = "(PT> %(PhotonPT)s * MeV)" % self.__confdict__
        _ph = FilterDesktop( name = _name, Code = _code)
        return _ph

    def _PhotonConvLLFilter( self, _name):
        _code = " ( MM < %(MaxConvPhLLMass)s * MeV ) " \
                "&( HASVERTEX ) " \
                "&( VFASPF(VCHI2/VDOF)<%(MaxConvPhLLChi)s ) " \
                "&( PT >  %(MinConvPhLLPT)s * MeV)" % self.__confdict__
        _phConv = FilterDesktop(name = _name, Code = _code)
        return _phConv
                    

    def _PhotonConvDDFilter( self, _name):
        _code = " ( MM < %(MaxConvPhDDMass)s * MeV ) " \
                "&( HASVERTEX ) " \
                "&( VFASPF(VCHI2/VDOF)<%(MaxConvPhDDChi)s ) " \
                "&( PT >  %(MinConvPhDDPT)s * MeV)" % self.__confdict__
        _phConv = FilterDesktop(name = _name, Code = _code) 
        return _phConv
    
    def _D02HHHHFilter( self , _decayDescriptors,_name):
        '''
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
        '''
        _c12_cuts = (" ( AM < (1865+%(Dto4h_AMassWin)s - 279)* MeV ) " \
                     "&( ACHI2DOCA(1,2) < %(Dto4hADocaChi2Max)s ) " %self.__confdict__ )
        _c123_cuts =(" ( AM < (1865+%(Dto4h_AMassWin)s - 139.5)* MeV ) " \
                     "&( ACHI2DOCA(1,3) < %(Dto4hADocaChi2Max)s ) " \
                     "&( ACHI2DOCA(2,3) < %(Dto4hADocaChi2Max)s ) " %self.__confdict__ )
        _combination_cuts =  (" (ADAMASS('D0') < %(Dto4h_AMassWin)s *MeV) " \
                              "&( (APT1+APT2+APT3+APT4) > %(PTSUM)s )" \
                              "&( ACHI2DOCA(1,4) < %(Dto4hADocaChi2Max)s ) " \
                              "&( ACHI2DOCA(2,4) < %(Dto4hADocaChi2Max)s ) " \
                              "&( ACHI2DOCA(3,4) < %(Dto4hADocaChi2Max)s ) " %self.__confdict__ )
        _mother_cuts = (" (ADMASS('D0') < %(Dto4h_MassWin)s *MeV) " \
                        "&(VFASPF(VCHI2PDOF) < %(DsVCHI2DOF)s)" \
                        "&(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" \
                        "&(SUMTREE( PT,  ISBASIC )> %(PTSUM)s*MeV)" \
                        "&(BPVVDCHI2 > %(DsFDCHI2)s)" \
                        "&(BPVDIRA > %(DsDIRA)s )" %self.__confdict__ )

        _d02hhhh = DaVinci__N4BodyDecays(DecayDescriptors = _decayDescriptors,
                                         Combination12Cut = _c12_cuts, Combination123Cut = _c123_cuts,
                                         CombinationCut = _combination_cuts,
                                         MotherCut = _mother_cuts)
                                     
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
            "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
            " & (MINTREE(((ABSID=='KS0')) , VFASPF(VZ))-VFASPF(VZ) > 0.0 *mm )"\
            " & (CHILD(BPVLTIME(),1) > 3.0*ps)"\
            " & (BPVLTIME() > 0.1*ps)"\
            " & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) & (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2ksh = CombineParticles( name = _name,
                                    DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                             
        return _ds2ksh
    
    def _D02KsHHFilter( self , _decayDescriptors, _name):
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (APT > 2000 *MeV)" \
                          "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(PTSUMLoose)s *MeV)" \
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(PTSUMLoose)s*MeV) & (PT > 2000 *MeV)" \
                     "& (MINTREE(((ABSID=='KS0')) , VFASPF(VZ))-VFASPF(VZ) > %(KSCutZFDFromD)s *mm )"  % self.__confdict__
        _d02KsHH = CombineParticles( name = _name,
                                     DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02KsHH

    def _D02KsHHPi0Filter( self , _decayDescriptors, _name):
        _combinationCut = "(ADAMASS('D0') < %(D02KSHHPi0AMassWin)s *MeV) & (APT > %(D02KSHHPi0_D0PTComb)s *MeV)" \
                          "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(D02KSHHPi0_PTSUMLoose)s *MeV)" \
                          "& (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(ADMASS('D0') < %(D02KSHHPi0MassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (SUMTREE( PT,  ISBASIC )> %(D02KSHHPi0_PTSUMLoose)s*MeV) & (PT > %(D02KSHHPi0_D0PT)s *MeV)" \
                     "& (MINTREE(((ABSID=='KS0')) , VFASPF(VZ))-VFASPF(VZ) > %(KSCutZFDFromD)s *mm )"  % self.__confdict__
        _d02KsHHPi0 = CombineParticles( name = _name,
                                     DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02KsHHPi0


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
                          "& (ADOCACUT( 2.*mm, ''))" % self.__confdict__
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
        return Selection( "Sel" + _name,
                          Algorithm = _d02HHPi0,
                          RequiredSelections = _requiredSelections)        

    def _Lc2L0HFilter( self , _decayDescriptors, _name):
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) " \
            " & (ACHILD(PT,1)+ACHILD(PT,2) > %(PTSUM)s *MeV)"\
            " & (ADOCACUT( 2*mm, '')) "  % self.__confdict__
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

    def GenericCharmMaker(self,_name,_decayDescriptors,_combinationCuts,_motherCuts,_requiredSelections):
        CHARM_COMB = CombineParticles( name = "CharmComb"+_name,
                                       DecayDescriptors = _decayDescriptors,
                                       CombinationCut = _combinationCuts,
                                       MotherCut = _motherCuts)                             
        return Selection (name = "SelCharm"+_name,
                          Algorithm = CHARM_COMB,
                          RequiredSelections = _requiredSelections)

    def _makeHHForD2XGamma(self, name, decayDescriptors, _requiredSelections) :
        _daughters_cuts = (" (TRGHOSTPROB < %(TrGhostProbMax)s)" \
                           "&(TRCHI2DOF < %(TRCHI2)s)" \
                           "&(PT > %(KPiPT)s)" \
                           "&(MIPCHI2DV(PRIMARY) > %(MINIPCHI2)s )"  % self.__confdict__ )
        _pidPi = "&(PIDK < %(PionPIDK)s)"  % self.__confdict__ # May use tight for reducing combinatorial
        _pidK  = "&(PIDK > %(KaonPIDK)s)"  % self.__confdict__ # May use tight for reducing combinatorial
        _combination_cuts = (" (in_range( %(CombMassLow_HH)s * MeV, AM, %(CombMassHigh_HH)s * MeV ) ) " \
                             "&( (APT1+APT2) > %(PTSUM)s )"  \
                             "&( ACHI2DOCA(1,2) < %(DtoXgammaADocaChi2Max)s )" % self.__confdict__ )
        _mother_cuts = (" ( in_range( %(MassLow_HH)s * MeV, M, %(MassHigh_HH)s * MeV)) " \
                        "&( VFASPF(VCHI2PDOF) < %(MaxVCHI2NDOF_HH)s)" \
                        "&( BPVVDCHI2 > %(MinVDCHI2_HH)s )" % self.__confdict__ )
        CombineX2HH = CombineParticles(name = 'Comb'+name,
                                       DecayDescriptors = decayDescriptors,
                                       DaughtersCuts = { "pi+" : _daughters_cuts+_pidPi, "K+" : _daughters_cuts+_pidK },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts)
        return Selection(name = 'Sel'+name,
                         Algorithm = CombineX2HH,
                         RequiredSelections = _requiredSelections )
    
    def _D02XGammaFilter( self, name, decayDescriptors, _requiredSelections ) :
        _combination_cuts =  (" ( ADAMASS('D0') < %(D02HHGammaAMassWin)s *MeV) " \
                              "&( ACHILD(PT,1)+ACHILD(PT,2) > %(PTSUM_HHGamma)s *MeV)" % self.__confdict__ )
        _mother_cuts = (" (ADMASS('D0') < %(D02HHGammaMassWin)s *MeV) " \
                        "&(VFASPF(VCHI2PDOF) < %(DsVCHI2DOF)s)" % self.__confdict__ )
        CombineD2XGamma = CombineParticles(name = 'Comb'+name,
                                           DecayDescriptors = decayDescriptors,
                                           CombinationCut = _combination_cuts,
                                           MotherCut = _mother_cuts)
        return Selection(name = 'Sel'+name,
                         Algorithm = CombineD2XGamma,
                         RequiredSelections = _requiredSelections )
                                                                
def makeDstar(_name, inputD0,_softPi,Dstar_cuts,_isSelfConjFS=False) : 
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
    if(_isSelfConjFS==True):	return Selection (name = "Sel"+_name,Algorithm = _Dstar,RequiredSelections = [inputD0] + [_softPi])
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

    selB = Selection (name = "Sel"+_name,
                      Algorithm = _B,
                      RequiredSelections = [MuSel, DSel])
    TOSB = TOSFilter(_name,selB,BCuts["TTSpecs"])
    return TOSB

def TOSFilter( name = None, sel = None, Specs = None ):
    if len(Specs) == 0:
        return sel
    """
    Function to return a selection object, filtering for TOS candidates from input selection
    """
    from Configurables import TisTosParticleTagger
    
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = Specs
    
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel



