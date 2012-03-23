"""
Module for constructing B semileptonic inclusive channels:
B->D0XMuNu, D+XMuNu, Ds+XMuNu, Lc+XMuNu with
D0->Kpi, D0->KK, D0->pipi, D+->Kpipi, KKPi, Ds+ -> KKPi and Lc+->PKPi, Ds->(Phi->KK)Pi

Also added for 17b (but now moved to charm muDST stream):
D0 -> KsHH, D0 -> HHPi0
D+ -> KsH, D+ -> HMuMu
Lc+ -> L0 H, Lc+ -> pHH
"""
__author__ = ['Liming Zhang, Alessandra Borgia']
__date__ = '23/07/2010'
__version__ = '$Revision: 1.8 $'

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
    ,"PrescalDsPi_fakes"  : 0.5    # for Bs->(Ds->PhiPi)Pi for Fakes line
    ,"MINIPCHI2"     : 9.0    # adimensiional
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
    ,"DsIP"          : 7.4    #mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"PIDmu"         : -0.0   # adimensiional
    ,"BDIRA"         : 0.999  #adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"DZLoose"       : -9999  #mm
    ,"DZ"            : 0  #mm
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
        ,"PrescalDsPi_fakes" 
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
        ,"DsIP"          
        ,"DsVCHI2DOF"    
        ,"PIDmu"         
        ,"BDIRA"         
        ,"BVCHI2DOF"     
        ,"DZ"
        ,"DZLoose"
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
                                       Algorithm = FilterDesktop( Code = "(PIDK< %(PionPIDKTight)s)" % self.__confdict__ ),
                                       RequiredSelections = [self.selPion])
        
        self.selPionloose = Selection( "Piloosefor" + name,
                                       Algorithm = self._pionlooseFilter(),
                                       RequiredSelections = [StdNoPIDsPions])
        
        self.selKaonloose = Selection( "Kloosefor" + name,
                                       Algorithm = self._kaonlooseFilter(),
                                       RequiredSelections = [StdLooseKaons])

        self.selpion_fakes = Selection( "Pion_fakes_for" + name,
                                        Algorithm = self._pionFilter_fakes(),
                                        RequiredSelections = [StdLoosePions])
        
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
                                    RequiredSelections = [self.selKaon, self.selPionTight] )

        self.seld02kk = Selection( "D02KKfor" + name,
                                    Algorithm = self._D02KKFilter(),
                                    RequiredSelections = [self.selKaon] )

        self.seld02pipi = Selection( "D02PiPifor" + name,
                                     Algorithm = self._D02PiPiFilter(),
                                     RequiredSelections = [self.selPionTight] )

        ################ D0 -> 4H SELECTION ##########################

        self.seld02k3pi = Selection( "D02K3Pifor" + name,
                                    Algorithm = self._D02K3PiFilter(),
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

        ############ Lambda_c+ -> pKpi ########################
        self.sellambdac = Selection( "Lc2PKPifor" + name,
                                     Algorithm = self._Lc2PKPiFilter(),
                                     RequiredSelections = [self.selKaon, self.selPion, StdLooseProtons ] )
        
        
        ################ MAKE THE B CANDIDATES #################################
        
        self.selb2D0MuX = makeb2DMuX('b2D0MuX' + name,
                                     DecayDescriptors = [ '[B- -> D0 mu-]cc' ],
                                     MuSel = self.selmuonhighPT, 
                                     DSel = self.seld02kpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA'],
                                     DZ = config['DZLoose']
                                     )
        
        self.selb2D0MuXKpiDCS = makeb2DMuX('b2D0MuXKpiDCS' + name,
                                           DecayDescriptors = [ '[B+ -> D0 mu+]cc' ],
                                           MuSel = self.selmuonhighPT, 
                                           DSel = self.seld02kpi,
                                           BVCHI2DOF = config['BVCHI2DOF'],
                                           BDIRA = config['BDIRA'],
                                           DZ = config['DZLoose']
                                           )
        
        self.selb2D0MuXKK = makeb2DMuX('b2D0MuXKK' + name,
                                       DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                       MuSel = self.selmuon, 
                                       DSel = self.seld02kk,
                                       BVCHI2DOF = config['BVCHI2DOF'],
                                       BDIRA = config['BDIRA'],
                                       DZ = config['DZLoose']
                                       )                   
        
        self.selb2D0MuXpipi = makeb2DMuX('b2D0MuXpipi' + name,
                                         DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                         MuSel = self.selmuon, 
                                         DSel = self.seld02pipi,
                                         BVCHI2DOF = config['BVCHI2DOF'],
                                         BDIRA = config['BDIRA'],
                                         DZ = config['DZLoose']
                                         )                   
        
        
        self.selb2D0MuXK3Pi = makeb2DMuX('b2D0MuXK3Pi' + name,
                                         DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                         MuSel = self.selmuonTOS, 
                                         DSel = self.seld02k3pi,
                                         BVCHI2DOF = config['BVCHI2DOF'],
                                         BDIRA = config['BDIRA'],
                                         DZ = config['DZ']
                                         )        

        
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
                                                    , prescale = config['PrescalDsPi_fakes']
                                                    , selection = self.selb2DsPi_PhiPi_fakes
                                                    )

        ########### D0 -> HHHHH
        self.b2D0MuXK3PiLine = StrippingLine('b2D0MuXK3Pi' + name + 'Line', prescale = 1, selection = self.selb2D0MuXK3Pi)

        ########## Lambda_c+ -> p HH 
        self.b2LcMuXLine = StrippingLine('b2LcMuX' + name + 'Line', prescale = 1, selection = self.selb2LcMuX)
        
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
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _ds2phipi_forfakes = CombineParticles( DecayDescriptors = _decayDescriptors,
                                               CombinationCut = _combinationCut,
                                               MotherCut = _motherCut)                             
        return _ds2phipi_forfakes
    
    
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

