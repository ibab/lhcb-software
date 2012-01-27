"""
Module for constructing B semileptonic inclusive channels:
B->D0XMuNu, D+XMuNu, Ds+XMuNu, Lc+XMuNu with
D0->Kpi, D0->KK, D0->pipi, D+->Kpipi, KKPi, Ds+ -> KKPi and Lc+->PKPi, Ds->(Phi->KK)Pi

Also added for 17b:
D0 -> KsHH, D0 -> HHPi0
D+ -> KsH, D+ -> HMuMu
Lc+ -> L0 H, Lc+ -> pHH
"""
__author__ = ['Liming Zhang, Alessandra Borgia']
__date__ = '23/07/2010'
__version__ = '$Revision: 1.5 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdLooseMergedPi0,StdLooseResolvedPi0

__all__ = ('B2DMuNuXAllLinesConf',
           'makeb2DMuX',
           'makeb2DX',
           'TOSFilter',
           'confdict')

confdict = {
    "PrescalD0Mu"    : 0.5    # for D0->KPi line
    ,"MINIPCHI2"     : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"TRCHI2Loose"   : 5.0    # adimensiional    
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PionPIDKTight" : 0.0    # adimensiional
    ,"MuonIPCHI2"    : 4.00   # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensiional
    ,"DsFDCHI2"      : 100.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"DsIPMin"       : 0.0498 #mm
    ,"DsIP"          : 7.4    #mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"PIDmu"         : -0.0   # adimensiional
    ,"BDIRA"         : 0.999  #adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"DZ"            : -9999  #mm
    ,"DDocaChi2Max"  : 20     #adimensiional
    ,"MINIPCHI2Loose" : 4.0   #adimensiional
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
    ,"D02HHPi0AMassWin" : 160  # MeV (mass window for combination)
    ,"D02HHPi0MassWin"  : 150  # MeV (mass window after vertex fit)
    ,"D02HHPi0DocaCut"  : 6    # mm
    ,"D02HHPi0PtCut"    : 2000 # MeV
    }

class B2DMuNuXAllLinesConf(LineBuilder) :
    """
    """

    __configuration_keys__ = (
        "PrescalD0Mu" 
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
        ,"DsIPMin"  
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

        self.selpion_fakes = Selection( "Pion_fakes_for" + name,
                                        Algorithm = self._pionFilter_fakes(),
                                        RequiredSelections = [StdLoosePions])
        
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

        ################ PHI->KK SELECTION ###########################
        
        self.selphi2kk = Selection( "Phi2KKfor" + name,
                                    Algorithm = self._Phi2KKFilter(),
                                    RequiredSelections = [self.selKaonloose] )
        
        self.selphi2kk_forfakes = Selection( "Phi2KK_forfakes_for" + name,
                                             Algorithm = self._Phi2KK_forfakes_Filter(),
                                             RequiredSelections = [self.selKaonloose] )
        
        ################ D0 -> HH SELECTION ##########################
        
        self.seld02kpi = Selection( "D02KPifor" + name,
                                    Algorithm = self._D02KPiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion] )

        self.seld02kk = Selection( "D02KKfor" + name,
                                    Algorithm = self._D02KKFilter(),
                                    RequiredSelections = [self.selKaon] )

        self.seld02pipi = Selection( "D02PiPifor" + name,
                                     Algorithm = self._D02PiPiFilter(),
                                     RequiredSelections = [self.selPion] )

        ################ D0 -> 4H SELECTION ##########################

        self.seld024Pi = Selection( "D024Pifor" + name,
                                    Algorithm = self._D024PiFilter(),
                                    RequiredSelections = [StdLoosePions] )        
        
        self.seld02k3pi = Selection( "D02K3Pifor" + name,
                                    Algorithm = self._D02K3PiFilter(),
                                    RequiredSelections = [self.selKaon, StdLoosePions] )        

        self.seld022K2Pi = Selection( "D022K2Pifor" + name,
                                    Algorithm = self._D022K2PiFilter(),
                                    RequiredSelections = [self.selKaon, StdLoosePions] )        

        self.seld023KPi = Selection( "D023KPifor" + name,
                                    Algorithm = self._D023KPiFilter(),
                                      RequiredSelections = [self.selKaon, StdLoosePions] )        

        ################ D+/Ds+ -> HHH SELECTIONS ##########################
        
        self.seldp2kpipi = Selection( "Dp2KPiPifor" + name,
                                      Algorithm = self._Dp2KPiPiFilter(),
                                      RequiredSelections = [self.selKaon, self.selPion] )

        self.selds2kkpi = Selection( "Ds2KKPifor" + name,
                                     Algorithm = self._Ds2KKPiFilter(),
                                     RequiredSelections = [self.selKaon, self.selPion] )

        
        self.selds2phipi = Selection( "Ds2PhiPifor" + name,
                                     Algorithm = self._Ds2PhiPiFilter(),
                                     RequiredSelections = [self.selphi2kk, self.selPionloose] )

        self.selds2phipi_forfakes = Selection( "Ds2PhiPi_forfakes_for" + name,
                                               Algorithm = self._Ds2PhiPi_forfakes_Filter(),
                                               RequiredSelections = [self.selphi2kk_forfakes, self.selPionloose] )
        
        ################## D+/Ds+ -> H mu mu SELECTIONS ########################

        self.selds2pimumu = Selection( "Ds2PiMuMufor" + name,
                                       Algorithm = self._Ds2PiMuMuFilter(),
                                       RequiredSelections = [self.selPion,self.selmuon] )

        self.selds2kmumu = Selection( "Ds2KMuMufor" + name,
                                       Algorithm = self._Ds2KMuMuFilter(),
                                       RequiredSelections = [self.selKaon,self.selmuon] )
        
        ################## D0 -> Ks HH SELECTIONS ##############################
        
        self.seld02KsPiPiLL = Selection( "D02KsPiPiLLfor" + name,
                                         Algorithm = self._D02KsPiPiFilter(),
                                         RequiredSelections = [self.selKSLL,self.selPionloose] )           
        
        self.seld02KsPiPiDD = Selection( "D02KsPiPiDDfor" + name,
                                         Algorithm = self._D02KsPiPiFilter(),
                                         RequiredSelections = [self.selKSDD,self.selPionloose] )           
        
        self.seld02KsKKLL = Selection( "D02KsKKLLfor" + name,
                                         Algorithm = self._D02KsKKFilter(),
                                         RequiredSelections = [self.selKSLL,self.selKaonloose] )           
        
        self.seld02KsKKDD = Selection( "D02KsKKDDfor" + name,
                                         Algorithm = self._D02KsKKFilter(),
                                         RequiredSelections = [self.selKSDD,self.selKaonloose] )           


        self.seld02KsKPiLL = Selection( "D02KsKPiLLfor" + name,
                                        Algorithm = self._D02KsKPiFilter(),
                                        RequiredSelections = [self.selKSLL,self.selKaonloose,self.selPionloose] )           
        
        
        self.seld02KsKPiDD = Selection( "D02KsKPiDDfor" + name,
                                        Algorithm = self._D02KsKPiFilter(),
                                        RequiredSelections = [self.selKSDD,self.selKaonloose,self.selPionloose] )           

        self.seld02KsKPiWSLL = Selection( "D02KsKPiWSLLfor" + name,
                                        Algorithm = self._D02KsKPiWSFilter(),
                                        RequiredSelections = [self.selKSLL,self.selKaonloose,self.selPionloose] )           
        
        self.seld02KsKPiWSDD = Selection( "D02KsKPiWSDDfor" + name,
                                        Algorithm = self._D02KsKPiWSFilter(),
                                        RequiredSelections = [self.selKSDD,self.selKaonloose,self.selPionloose] )           

        ################## D0 -> HHPi0 WITH RESOLVED PI0 #######################
        
        self.seld02KPiPi0Resolved = Selection( "D02KPiPi0Resolvedfor" + name,
                                               Algorithm = self._D02KPiPi0Filter(),
                                               RequiredSelections = [self.selKaon,self.selPionTight,self.selPi0Resolved])
        
        self.seld02PiPiPi0Resolved = Selection( "D02PiPiPi0Resolvedfor" + name,
                                               Algorithm = self._D02PiPiPi0Filter(),
                                               RequiredSelections = [self.selPionTight,self.selPi0Resolved])

        self.seld02KKPi0Resolved = Selection( "D02KKPi0Resolvedfor" + name,
                                               Algorithm = self._D02KKPi0Filter(),
                                               RequiredSelections = [self.selKaon,self.selPi0Resolved])
        
        self.seld02KPiPi0ResolvedWS = Selection( "D02KPiPi0ResolvedWSfor" + name,
                                                 Algorithm = self._D02KPiPi0WSFilter(),
                                                 RequiredSelections = [self.selKaon,self.selPionTight,self.selPi0Resolved])
        
        ################## D0 -> HHPi0 WITH MERGED PI0 #######################
        
        self.seld02KPiPi0Merged = Selection( "D02KPiPi0Mergedfor" + name,
                                             Algorithm = self._D02KPiPi0Filter(),
                                             RequiredSelections = [self.selKaon,self.selPionTight,self.selPi0Merged])
        
        
        self.seld02PiPiPi0Merged = Selection( "D02PiPiPi0Mergedfor" + name,
                                              Algorithm = self._D02PiPiPi0Filter(),
                                              RequiredSelections = [self.selPionTight,self.selPi0Merged])
        
        self.seld02KKPi0Merged = Selection( "D02KKPi0Mergedfor" + name,
                                            Algorithm = self._D02KKPi0Filter(),
                                            RequiredSelections = [self.selKaon,self.selPi0Merged])
        
        self.seld02KPiPi0MergedWS = Selection( "D02KPiPi0MergedWSfor" + name,
                                               Algorithm = self._D02KPiPi0WSFilter(),
                                               RequiredSelections = [self.selKaon,self.selPionTight,self.selPi0Merged])
        
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

        
        ################ MAKE THE B CANDIDATES #################################
        
        self.selb2D0MuX = makeb2DMuX('b2D0MuX' + name,
                                     DecayDescriptors = [ '[B- -> D0 mu-]cc' ],
                                     MuSel = self.selmuonhighPT, 
                                     DSel = self.seld02kpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA'],
                                     DZ = config['DZ']
                                     )

        self.selb2D0MuXKpiDCS = makeb2DMuX('b2D0MuXKpiDCS' + name,
                                     DecayDescriptors = [ '[B+ -> D0 mu+]cc' ],
                                     MuSel = self.selmuonhighPT, 
                                     DSel = self.seld02kpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA'],
                                     DZ = config['DZ']
                                     )

        self.selb2D0MuXKK = makeb2DMuX('b2D0MuXKK' + name,
                                 DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                 MuSel = self.selmuon, 
                                 DSel = self.seld02kk,
                                 BVCHI2DOF = config['BVCHI2DOF'],
                                 BDIRA = config['BDIRA'],
                                 DZ = config['DZ']
                                 )                   

        self.selb2D0MuXpipi = makeb2DMuX('b2D0MuXpipi' + name,
                                 DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                 MuSel = self.selmuon, 
                                 DSel = self.seld02pipi,
                                 BVCHI2DOF = config['BVCHI2DOF'],
                                 BDIRA = config['BDIRA'],
                                 DZ = config['DZ']
                                 )                   

        ############## D0 -> HHHH ###########################################
        
        self.selb2D0MuXK3Pi = makeb2DMuX('b2D0MuXK3Pi' + name,
                                         DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                         MuSel = self.selmuonTOS, 
                                         DSel = self.seld02k3pi,
                                         BVCHI2DOF = config['BVCHI2DOF'],
                                         BDIRA = config['BDIRA'],
                                         DZ = config['DZ']
                                 )        


        self.selb2D0MuX4Pi = makeb2DMuX('b2D0MuX4Pi' + name,
                                        DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                        MuSel = self.selmuonTOS, DSel = self.seld024Pi,BVCHI2DOF = config['BVCHI2DOF'],
                                        BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2D0MuX2K2Pi = makeb2DMuX('b2D0MuX2K2Pi' + name,
                                           DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                           MuSel = self.selmuonTOS, DSel = self.seld022K2Pi,BVCHI2DOF = config['BVCHI2DOF'],
                                           BDIRA = config['BDIRA'],DZ = config['DZ'])
        

        self.selb2D0MuX3KPi = makeb2DMuX('b2D0MuX3KPi' + name,
                                          DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                          MuSel = self.selmuonTOS, DSel = self.seld023KPi,BVCHI2DOF = config['BVCHI2DOF'],
                                          BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        
        
        self.selb2DpMuX = makeb2DMuX('b2DpMuX' + name,
                                 DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                 MuSel = self.selmuonnew, 
                                 DSel = self.seldp2kpipi,
                                 BVCHI2DOF = config['BVCHI2DOF'],
                                 BDIRA = config['BDIRA'],
                                 DZ = config['DZ']
                                 )

        self.selb2DsMuX = makeb2DMuX('b2DsMuX' + name,
                                 DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                 MuSel = self.selmuonnew, 
                                 DSel = self.selds2kkpi,
                                 BVCHI2DOF = config['BVCHI2DOF'],
                                 BDIRA = config['BDIRA'],
                                 DZ = config['DZ']
                                     )
                                 
        self.selb2LcMuX = makeb2DMuX('b2LcMuX' + name,
                                     DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                     MuSel = self.selmuonnew, 
                                     DSel = self.sellambdac,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA'],
                                     DZ = config['DZ']
                                     )         
        
        self.selb2DsMuXPhiPi = makeb2DMuX('b2DsMuXPhiPi' + name,
                                          DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                          MuSel = self.selmuon, 
                                          DSel = self.selds2phipi,
                                          BVCHI2DOF = config['BVCHI2DOF'],
                                          BDIRA = config['BDIRA'],
                                          DZ = config['DZ']
                                          )
        ################ Using another Algo for DHadron ################
        self.selb2DsPi_PhiPi_fakes = makeb2DX('b2DsPi_PhiPi_fakes' + name,
                                              DecayDescriptors = [ '[B0 -> D- pi+]cc', '[B0 -> D- pi-]cc' ],
                                              HadSel = self.selpion_fakes, 
                                              DSel = self.selds2phipi_forfakes,
                                              BVCHI2DOF = config['BVCHI2DOF'],
                                              BDIRA = config['BDIRA'],
                                              DZ = config['DZ']
                                              )
        
        ############### B+ -> MU X D0 -> Ks HH #########################
        
        self.selb2D0MuXKsPiPiLL = makeb2DMuX('b2D0MuXKsPiPiLL' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                             MuSel = self.selmuon, DSel = self.seld02KsPiPiLL,
                                             BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2D0MuXKsPiPiDD = makeb2DMuX('b2D0MuXKsPiPiDD' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                             MuSel = self.selmuon, DSel = self.seld02KsPiPiDD,
                                             BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2D0MuXKsKKLL = makeb2DMuX('b2D0MuXKsKKLL' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                           MuSel = self.selmuon, DSel = self.seld02KsKKLL,
                                           BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
                                           
        self.selb2D0MuXKsKKDD = makeb2DMuX('b2D0MuXKsKKDD' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                           MuSel = self.selmuon, DSel = self.seld02KsKKDD,
                                           BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
                                           
        self.selb2D0MuXKsKPiLL = makeb2DMuX('b2D0MuXKsKPiLL' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                            MuSel = self.selmuon, DSel = self.seld02KsKPiLL,
                                            BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2D0MuXKsKPiDD = makeb2DMuX('b2D0MuXKsKPiDD' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                            MuSel = self.selmuon, DSel = self.seld02KsKPiDD,
                                            BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2D0MuXKsKPiWSLL = makeb2DMuX('b2D0MuXKsKPiWSLL' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                            MuSel = self.selmuon, DSel = self.seld02KsKPiWSLL,
                                            BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2D0MuXKsKPiWSDD = makeb2DMuX('b2D0MuXKsKPiWSDD' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                            MuSel = self.selmuon, DSel = self.seld02KsKPiWSDD,
                                            BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        ############### B+ -> MU X D0 -> H H Pi0  #########################
        
        self.selb2D0MuXKPiPi0Resolved = makeb2DMuX('b2D0MuXKPiPi0Resolved' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                                   MuSel = self.selmuon, DSel = self.seld02KPiPi0Resolved,
                                                   BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2D0MuXPiPiPi0Resolved = makeb2DMuX('b2D0MuXPiPiPi0Resolved' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                                    MuSel = self.selmuon, DSel = self.seld02PiPiPi0Resolved,
                                                    BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])        
        
        self.selb2D0MuXKKPi0Resolved = makeb2DMuX('b2D0MuXKKPi0Resolved' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                                  MuSel = self.selmuon, DSel = self.seld02KKPi0Resolved,
                                                  BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2D0MuXKPiPi0ResolvedWS = makeb2DMuX('b2D0MuXKPiPi0ResolvedWS' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                                      MuSel = self.selmuon, DSel = self.seld02KPiPi0ResolvedWS,
                                                      BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2D0MuXKKPi0Merged = makeb2DMuX('b2D0MuXKKPi0Merged' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                                MuSel = self.selmuon, DSel = self.seld02KKPi0Merged,
                                                BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2D0MuXKPiPi0Merged = makeb2DMuX('b2D0MuXKPiPi0Merged' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                                 MuSel = self.selmuon, DSel = self.seld02KPiPi0Merged,
                                                 BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2D0MuXPiPiPi0Merged = makeb2DMuX('b2D0MuXPiPiPi0Merged' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                                  MuSel = self.selmuon, DSel = self.seld02PiPiPi0Merged,
                                                  BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2D0MuXKPiPi0MergedWS = makeb2DMuX('b2D0MuXKPiPi0MergedWS' + name,DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                                   MuSel = self.selmuon, DSel = self.seld02KPiPi0MergedWS,
                                                   BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        ############### B0 -> MU X D+ -> Ks H  #########################

        self.selb2DsMuXKsLLK = makeb2DMuX('b2DsMuXKsLLK'+name, DecayDescriptors = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ],
                                          MuSel = self.selmuon,DSel = self.selds2KsLLK,
                                          BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2DsMuXKsDDK = makeb2DMuX('b2DsMuXKsDDK'+name,DecayDescriptors = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ],
                                          MuSel = self.selmuon,DSel = self.selds2KsDDK,
                                          BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2DsMuXKsLLPi = makeb2DMuX('b2DsMuXKsLLPi'+name,DecayDescriptors = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ],
                                           MuSel = self.selmuon,DSel = self.selds2KsLLPi,
                                          BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2DsMuXKsDDPi = makeb2DMuX('b2DsMuXKsDDPi'+name,DecayDescriptors = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ],
                                           MuSel = self.selmuon,DSel = self.selds2KsDDPi,
                                          BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        ############### B0 -> MU X (D+ -> H mu mu)  #########################

        self.selb2DsMuXPiMuMu = makeb2DMuX('b2DsMuXPiMuMu'+name,DecayDescriptors = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ],
                                           MuSel = self.selmuon,DSel = self.selds2pimumu,
                                           BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2DsMuXKMuMu = makeb2DMuX('b2DsMuXKMuMu'+name,DecayDescriptors = [ '[B0 -> D+ mu-]cc', '[B0 -> D+ mu+]cc' ],
                                           MuSel = self.selmuon,DSel = self.selds2kmumu,
                                           BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        ############## Lambda_b0 -> MU X Lambda_c -> X ############################

        self.selb2Lc2L0DDPiMuX = makeb2DMuX('b2Lc2L0DDPiMuX' + name,DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                            MuSel = self.selmuon,DSel = self.selLc2L0Pi_DD,
                                            BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2Lc2L0LLPiMuX = makeb2DMuX('b2Lc2L0LLPiMuX' + name,DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                            MuSel = self.selmuon,DSel = self.selLc2L0Pi_LL,
                                            BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2Lc2L0DDKMuX = makeb2DMuX('b2Lc2L0DDKMuX' + name,DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                            MuSel = self.selmuon,DSel = self.selLc2L0K_DD,
                                            BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2Lc2L0LLKMuX = makeb2DMuX('b2Lc2L0LLKMuX' + name,DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                            MuSel = self.selmuon,DSel = self.selLc2L0K_LL,
                                            BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        self.selb2LcDCSMuX = makeb2DMuX('b2LcDCSMuX' + name, DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                        MuSel = self.selmuonnew, DSel = self.sellambdacDCS,
                                        BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2Lc2pPiPiMuX = makeb2DMuX('b2Lc2pPiPiMuX' + name, DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                           MuSel = self.selmuonnew, DSel = self.selLc2pPiPi,
                                           BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])

        self.selb2Lc2pKKMuX = makeb2DMuX('b2Lc2pKKMuX' + name, DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                           MuSel = self.selmuonnew, DSel = self.selLc2pKK,
                                           BVCHI2DOF = config['BVCHI2DOF'],BDIRA = config['BDIRA'],DZ = config['DZ'])
        
        ################# DECLARE THE STRIPPING LINES #################################

        ########## D0 -> HH ###########
        self.b2D0MuXLine = StrippingLine('b2D0MuX' + name + 'Line', prescale = config['PrescalD0Mu'], selection = self.selb2D0MuX)
        self.b2D0MuXKpiDCSLine = StrippingLine('b2D0MuXKpiDCS' + name + 'Line', prescale = 1, selection = self.selb2D0MuXKpiDCS)
        self.b2D0MuXKKLine = StrippingLine('b2D0MuXKK' + name + 'Line', prescale = 1, selection = self.selb2D0MuXKK)
        self.b2D0MuXpipiLine = StrippingLine('b2D0MuXpipi' + name + 'Line', prescale = 1, selection = self.selb2D0MuXpipi)

        ######## Lines for time integrated A_SL #########
        self.b2DpMuXLine = StrippingLine('b2DpMuX' + name + 'Line', prescale = 1, selection = self.selb2DpMuX)
        self.b2DsMuXLine = StrippingLine('b2DsMuX' + name + 'Line', prescale = 1, selection = self.selb2DsMuX)
        self.b2DsMuXPhiPiLine = StrippingLine('b2DsMuXPhiPi' + name + 'Line', prescale = 1, selection = self.selb2DsMuXPhiPi)
        self.b2DsPi_PhiPi_fakesLine = StrippingLine('b2DsPi_PhiPi_fakes' + name + 'Line'
                                                    , HLT     = "HLT_PASS_RE('Hlt2IncPhi.*Decision')"
                                                    , prescale = 1.0
                                                    , selection = self.selb2DsPi_PhiPi_fakes
                                                    )

        ########### D0 -> HHHHH
        self.b2D0MuXK3PiLine = StrippingLine('b2D0MuXK3Pi' + name + 'Line', prescale = 1, selection = self.selb2D0MuXK3Pi)
        self.b2D0MuX4PiLine = StrippingLine('b2D0MuX4Pi' + name + 'Line', prescale = 1, selection = self.selb2D0MuX4Pi)
        self.b2D0MuX2K2PiLine = StrippingLine('b2D0MuX2K2Pi' + name + 'Line', prescale = 1, selection = self.selb2D0MuX2K2Pi)
        self.b2D0MuX3KPiLine = StrippingLine('b2D0MuX3KPi' + name + 'Line', prescale = 1, selection = self.selb2D0MuX3KPi)
        
        ########### D0 -> Ks HH 
        self.b2D0MuXKsPiPiLL  = StrippingLine('b2D0MuXKsPiPiLL'+name+'Line',prescale = 1,selection = self.selb2D0MuXKsPiPiLL)
        self.b2D0MuXKsPiPiDD  = StrippingLine('b2D0MuXKsPiPiDD'+name+'Line',prescale = 1,selection = self.selb2D0MuXKsPiPiDD)
        self.b2D0MuXKsKKLL    = StrippingLine('b2D0MuXKsKKLL'+name+'Line',prescale = 1,selection = self.selb2D0MuXKsKKLL)
        self.b2D0MuXKsKKDD    = StrippingLine('b2D0MuXKsKKDD'+name+'Line',prescale = 1,selection = self.selb2D0MuXKsKKDD)
        self.b2D0MuXKsKPiLL   = StrippingLine('b2D0MuXKsKPiLL'+name+'Line',prescale = 1,selection = self.selb2D0MuXKsKPiLL)
        self.b2D0MuXKsKPiDD   = StrippingLine('b2D0MuXKsKPiDD'+name+'Line',prescale = 1,selection = self.selb2D0MuXKsKPiDD)
        self.b2D0MuXKsKPiWSLL = StrippingLine('b2D0MuXKsKPiWSLL'+name+'Line',prescale = 1,selection = self.selb2D0MuXKsKPiWSLL)
        self.b2D0MuXKsKPiWSDD = StrippingLine('b2D0MuXKsKPiWSDD'+name+'Line',prescale = 1,selection = self.selb2D0MuXKsKPiWSDD)

        ########### D0 -> HHPi0 (resolved)
        self.b2D0MuXKPiPi0Resolved = StrippingLine('b2D0MuXKPiPi0Resolved'+name+'Line',prescale = 1,selection = self.selb2D0MuXKPiPi0Resolved)
        self.b2D0MuXKKPi0Resolved = StrippingLine('b2D0MuXKKPi0Resolved'+name+'Line',prescale = 1,selection = self.selb2D0MuXKKPi0Resolved)
        self.b2D0MuXPiPiPi0Resolved = StrippingLine('b2D0MuXPiPiPi0Resolved'+name+'Line',prescale = 1,selection = self.selb2D0MuXPiPiPi0Resolved)
        self.b2D0MuXKPiPi0ResolvedWS = StrippingLine('b2D0MuXKPiPi0ResolvedWS'+name+'Line',prescale = 1,selection = self.selb2D0MuXKPiPi0ResolvedWS)
        
        
        ########### D0 -> HHPi0 (merged)
        self.b2D0MuXKPiPi0Merged = StrippingLine('b2D0MuXKPiPi0Merged'+name+'Line',prescale = 1,selection = self.selb2D0MuXKPiPi0Merged)
        self.b2D0MuXKKPi0Merged = StrippingLine('b2D0MuXKKPi0Merged'+name+'Line',prescale = 1,selection = self.selb2D0MuXKKPi0Merged)
        self.b2D0MuXPiPiPi0Merged = StrippingLine('b2D0MuXPiPiPi0Merged'+name+'Line',prescale = 1,selection = self.selb2D0MuXPiPiPi0Merged)
        self.b2D0MuXKPiPi0MergedWS = StrippingLine('b2D0MuXKPiPi0MergedWS'+name+'Line',prescale = 1,selection = self.selb2D0MuXKPiPi0MergedWS)
        
        ########### D+ -> KsH
        self.b2DsMuXKsLLKLine = StrippingLine('b2DsMuXKsLLK' + name + 'Line', prescale = 1, selection = self.selb2DsMuXKsLLK)
        self.b2DsMuXKsDDKLine = StrippingLine('b2DsMuXKsDDK' + name + 'Line', prescale = 1, selection = self.selb2DsMuXKsDDK)
        self.b2DsMuXKsLLPiLine = StrippingLine('b2DsMuXKsLLPi' + name + 'Line', prescale = 1, selection = self.selb2DsMuXKsLLPi)
        self.b2DsMuXKsDDPiLine = StrippingLine('b2DsMuXKsDDPi' + name + 'Line', prescale = 1, selection = self.selb2DsMuXKsDDPi)

        ########## D+ -> H mu mu
        self.b2DsMuXPiMuMuLine = StrippingLine('b2DsMuXPiMuMu' + name + 'Line', prescale = 1, selection = self.selb2DsMuXPiMuMu)
        self.b2DsMuXKMuMuLine = StrippingLine('b2DsMuXKMuMu' + name + 'Line', prescale = 1, selection = self.selb2DsMuXKMuMu)
        
        ########## Lambda_c+ -> Lambda H
        self.b2MuXLc2L0LLPiLine = StrippingLine('b2MuXLc2L0LLPi' + name + 'Line', prescale = 1, selection = self.selb2Lc2L0LLPiMuX)
        self.b2MuXLc2L0DDPiLine = StrippingLine('b2MuXLc2L0DDPi' + name + 'Line', prescale = 1, selection = self.selb2Lc2L0DDPiMuX)
        self.b2MuXLc2L0LLKLine = StrippingLine('b2MuXLc2L0LLK' + name + 'Line', prescale = 1, selection = self.selb2Lc2L0LLKMuX)
        self.b2MuXLc2L0DDKLine = StrippingLine('b2MuXLc2L0DDK' + name + 'Line', prescale = 1, selection = self.selb2Lc2L0DDKMuX)

        ########## Lambda_c+ -> p HH 
        self.b2LcMuXLine = StrippingLine('b2LcMuX' + name + 'Line', prescale = 1, selection = self.selb2LcMuX)
        self.b2LcDCSMuXLine = StrippingLine('b2LcDCSMuX' + name + 'Line', prescale = 1, selection = self.selb2LcDCSMuX)
        self.b2Lc2pPiPiMuXLine = StrippingLine('b2Lc2pPiPiMuX' + name + 'Line', prescale = 1, selection = self.selb2Lc2pPiPiMuX)
        self.b2Lc2pKKMuXLine = StrippingLine('b2Lc2pKKMuX' + name + 'Line', prescale = 1, selection = self.selb2Lc2pKKMuX)
        
        ############## REGISTER THE LINES #####################

        ######### lines from stripping 17 #######
        self.registerLine(self.b2D0MuXLine)        
        self.registerLine(self.b2D0MuXKpiDCSLine)        
        self.registerLine(self.b2DpMuXLine)
        self.registerLine(self.b2DsMuXLine)
        self.registerLine(self.b2LcMuXLine)
        self.registerLine(self.b2D0MuXK3PiLine)
        self.registerLine(self.b2D0MuXKKLine)
        self.registerLine(self.b2D0MuXpipiLine)
        self.registerLine(self.b2DsMuXPhiPiLine)
        self.registerLine(self.b2DsPi_PhiPi_fakesLine)

        ######### D -> HHHH #####################
        self.registerLine(self.b2D0MuX3KPiLine)
        self.registerLine(self.b2D0MuX2K2PiLine)
        self.registerLine(self.b2D0MuX4PiLine)
        
        
        ######### D -> Ks H H ####################
        self.registerLine(self.b2D0MuXKsPiPiLL)
        self.registerLine(self.b2D0MuXKsPiPiDD)
        self.registerLine(self.b2D0MuXKsKKLL)
        self.registerLine(self.b2D0MuXKsKKDD)
        self.registerLine(self.b2D0MuXKsKPiLL)
        self.registerLine(self.b2D0MuXKsKPiDD)
        self.registerLine(self.b2D0MuXKsKPiWSLL)
        self.registerLine(self.b2D0MuXKsKPiWSDD)
        
        ######## D -> H H Pi0 (resolved) ###############
        self.registerLine(self.b2D0MuXKPiPi0Resolved)
        self.registerLine(self.b2D0MuXPiPiPi0Resolved)
        self.registerLine(self.b2D0MuXKKPi0Resolved)
        self.registerLine(self.b2D0MuXKPiPi0ResolvedWS)

        ######## D -> H H Pi0 (merged) #################
        self.registerLine(self.b2D0MuXKPiPi0Merged)
        self.registerLine(self.b2D0MuXPiPiPi0Merged)
        self.registerLine(self.b2D0MuXKKPi0Merged)
        self.registerLine(self.b2D0MuXKPiPi0MergedWS)

        ######## D+ -> Ks H ####################
        self.registerLine(self.b2DsMuXKsLLKLine)
        self.registerLine(self.b2DsMuXKsDDKLine)
        self.registerLine(self.b2DsMuXKsLLPiLine)
        self.registerLine(self.b2DsMuXKsDDPiLine)

        ####### D+ -> H Mu Mu ###################
        self.registerLine(self.b2DsMuXPiMuMuLine)
        self.registerLine(self.b2DsMuXKMuMuLine)

        ####### Lc+ -> L0 H #####################
        self.registerLine(self.b2MuXLc2L0LLPiLine)
        self.registerLine(self.b2MuXLc2L0DDPiLine)
        self.registerLine(self.b2MuXLc2L0LLKLine)
        self.registerLine(self.b2MuXLc2L0DDKLine)

        ####### Lc+ -> p H H ###################
        self.registerLine(self.b2LcDCSMuXLine)
        self.registerLine(self.b2Lc2pPiPiMuXLine)
        self.registerLine(self.b2Lc2pKKMuXLine)
        
        
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
    
    def _pionFilter_fakes( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2Loose)s) & (PIDmu < %(PIDmu)s)" % self.__confdict__
        _pil_fakes = FilterDesktop( Code = _code )
        return _pil_fakes

    
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
    
    def _Phi2KKFilter( self ):
        _decayDescriptor = 'phi(1020) -> K- K+'
        _combinationCut = "(ADAMASS('phi(1020)') < %(PhiMassWin)s *MeV)" % self.__confdict__
        _motherCut = "(VFASPF(VCHI2) < %(PhiVCHI2)s) " % self.__confdict__
        _phi2kk = CombineParticles( DecayDescriptor = _decayDescriptor,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _phi2kk
    
    def _Phi2KK_forfakes_Filter( self ):
        _decayDescriptor = 'phi(1020) -> K- K+'
        _combinationCut = "(ADAMASS('phi(1020)') < %(PhiMassWin)s *MeV)" % self.__confdict__
        _motherCut = "(MM < 1050*MeV) & (VFASPF(VCHI2) < %(PhiVCHI2)s)" % self.__confdict__
        _phi2kk_forfakes = CombineParticles( DecayDescriptor = _decayDescriptor,
                                             CombinationCut = _combinationCut,
                                             MotherCut = _motherCut)                            
        return _phi2kk_forfakes
    
    
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
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (APT > 1500.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (PT > 250 *MeV) & (P>2.0*GeV)"\
                           "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__}
        _motherCut = " (ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (INTREE((ABSID=='pi+')& (PT > %(KPiPT)s *MeV) &(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)))" \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02k3pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02k3pi    

    def _D024PiFilter( self ):
        _decayDescriptors = [ '[D0 -> pi+ pi- pi+ pi-]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (APT > 1500.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (PT > 250 *MeV) & (P>2.0*GeV)"\
                           "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__}
        _motherCut = " (ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (INTREE((ABSID=='pi+')& (PT > %(KPiPT)s *MeV) &(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)))" \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02k3pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02k3pi    

    
    def _D022K2PiFilter( self ):
        _decayDescriptors = [ '[D0 -> K+ K- pi+ pi-]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (APT > 1500.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (PT > 250 *MeV) & (P>2.0*GeV)"\
                           "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__}
        _motherCut = " (ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (INTREE((ABSID=='pi+')& (PT > %(KPiPT)s *MeV) &(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)))" \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02k3pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                            
        return _d02k3pi

    def _D023KPiFilter( self ):
        _decayDescriptors = [ '[D0 -> K+ K- K- pi+]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (APT > 1500.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (PT > 250 *MeV) & (P>2.0*GeV)"\
                           "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__}
        _motherCut = " (ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
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
    
    def _Ds2PhiPiFilter( self ):
        _decayDescriptors = [ '[D+ -> phi(1020) pi+]cc' ]
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 800.*MeV) & (ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2)> %(MINIPCHI2Loose)s) " % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>800.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2phipi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                      CombinationCut = _combinationCut,
                                      MotherCut = _motherCut)                             
        return _ds2phipi

    def _Ds2PhiPi_forfakes_Filter( self ):
        _decayDescriptors = [ '[D+ -> phi(1020) pi+]cc' ]
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 800.*MeV) & (ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2)> %(MINIPCHI2Loose)s) " % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>800.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s) & (BPVIP()> %(DsIPMin)s *mm)"  % self.__confdict__
        _ds2phipi_forfakes = CombineParticles( DecayDescriptors = _decayDescriptors,
                                               CombinationCut = _combinationCut,
                                               MotherCut = _motherCut)                             
        return _ds2phipi_forfakes
    
    
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


    def _D02KsKPiWSFilter( self ):
        _decayDescriptors =  [ "[D0 -> KS0 K+ pi-]cc" ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02KsKPi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                      CombinationCut = _combinationCut,
                                      MotherCut = _motherCut)                            
        return _d02KsKPi


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

    def _D02KPiPi0WSFilter( self ):
        _decayDescriptors = ["[D0 -> K+ pi- pi0]cc"]
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
    
    
def makeb2DMuX(name,
               DecayDescriptors,
               MuSel,
               DSel,
               BVCHI2DOF,
               BDIRA,
               DZ):
    _combinationCut = "(AM<6.2*GeV)"
    _motherCut = "  (MM<6.0*GeV) & (MM>2.5*GeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)  " \
                   "& (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > %(DZ)s *mm ) "  % locals()
    #        _B.ReFitPVs = True
    _B = CombineParticles(DecayDescriptors = DecayDescriptors,
                          CombinationCut = _combinationCut,
                          MotherCut = _motherCut)
                          
    return Selection (name,
                      Algorithm = _B,
                      RequiredSelections = [MuSel, DSel])



def makeb2DX(name,
             DecayDescriptors,
             HadSel,
             DSel,
             BVCHI2DOF,
             BDIRA,
             DZ):
    _combinationCut = "(AM<6.2*GeV)"
    _motherCut = "  (MM<6.0*GeV) & (MM>2.5*GeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)  " \
                 "& (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > %(DZ)s *mm ) "  % locals()
    #        _B.ReFitPVs = True
    _BX = CombineParticles(DecayDescriptors = DecayDescriptors,
                           CombinationCut = _combinationCut,
                           MotherCut = _motherCut)
    
    return Selection (name,
                      Algorithm = _BX,
                      RequiredSelections = [HadSel, DSel])



## Generic functions, added for functionality

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

