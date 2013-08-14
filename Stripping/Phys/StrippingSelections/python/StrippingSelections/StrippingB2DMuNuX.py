"""
Module for constructing B semileptonic inclusive channels:
B->D0XMuNu, D+XMuNu, Ds+XMuNu, Lc+XMuNu with
D0->Kpi, D0->KK, D0->pipi, D+->Kpipi, KKPi, Ds+ -> KKPi and Lc+->PKPi, Ds->(Phi->KK)Pi
D+->KKK (for mass measurement)
The following three lines have flavour tagging enabled:
- B->D* mu nu, D*->D0pi, D0->K3pi/Kpi
- B->D mu nu, D->Kpipi
"""
__author__ = ['Liming Zhang, Alessandra Borgia']
__date__ = '23/07/2010'
__version__ = '$Revision: 3.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdLooseMergedPi0,StdLooseResolvedPi0
from Configurables import ConjugateNeutralPID

__all__ = ('B2DMuNuXAllLinesConf',
           'makeb2DMuX',
           'makeb2DX',
           'makeDstar',
           'TOSFilter',
           'confdict')

confdict = {
    "GEC_nLongTrk" : 250 # adimensional
    ,"TRGHOSTPROB" : 0.5 # adimensional
    ,"MuonGHOSTPROB" : 0.5 #adimensional
    ,"PrescalD0Mu"    : 0.5  # for D0->KPi line
    ,"PrescalDsPi_fakes" : 0.5  # for Bs->(Ds->PhiPi)Pi for Fakes line
    ,"MINIPCHI2"     : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"TRCHI2Loose"   : 5.0    # adimensiional    
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"KaonPIDKTight" : 8.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PionPIDKTight" : 4.0    # adimensiional
    ,"MuonIPCHI2"    : 4.00   # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensiional
    ,"DsFDCHI2"      : 100.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"D02HHHHMassWin": 60.0   # MeV
    ,"D02HHHHSumPT"  : 1800.0   # MeV
    ,"DsIP"          : 7.4    #mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"PIDmu"         : -0.0   # adimensiional
    ,"BDIRA"         : 0.999  #adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"DZLoose"       : -9999  #mm
    ,"DZ"            : 0  #mm
    ,"DDocaChi2Max"  : 20     #adimensiional
    ,"DDocaChi2MaxTight"  : 9.0     #adimensiional
    ,"MINIPCHI2Loose" : 4.0   #adimensiional
    ,"KaonPIDKloose" : -5     #adimensiional
    ,"PhiVCHI2"      :25.0    #adimensiional
    ,"PhiMassWin"    :40      #adimensiional
    ,"Dstar_Chi2"       :  8.0 ## unitless
    ,"Dstar_SoftPion_PIDe" : 2. ## unitless
    ,"Dstar_SoftPion_PT" : 180. ## MeV ###
    ,"Dstar_wideDMCutLower" : 0. ## MeV
    ,"Dstar_wideDMCutUpper" : 170. ## MeV
    }

class B2DMuNuXAllLinesConf(LineBuilder) :
    """
    """
    
    __configuration_keys__ = (
        "GEC_nLongTrk"
        ,"TRGHOSTPROB"
        ,"MuonGHOSTPROB"
        ,"PrescalD0Mu"
        ,"PrescalDsPi_fakes" 
        ,"MINIPCHI2"     
        ,"TRCHI2"     
        ,"TRCHI2Loose"   
        ,"KaonPIDK"
        ,"KaonPIDKTight"      
        ,"PionPIDK"
        ,"PionPIDKTight"      
        ,"MuonIPCHI2"    
        ,"MuonPT"        
        ,"KPiPT"               
        ,"DsDIRA"        
        ,"DsFDCHI2"      
        ,"DsMassWin"     
        ,"DsAMassWin"    
        ,"D02HHHHMassWin"
        ,"D02HHHHSumPT" 
        ,"DsIP"          
        ,"DsVCHI2DOF"    
        ,"PIDmu"         
        ,"BDIRA"         
        ,"BVCHI2DOF"     
        ,"DZ"
        ,"DZLoose"
        ,"DDocaChi2Max"
        ,"DDocaChi2MaxTight"
        ,"MINIPCHI2Loose"
        ,"KaonPIDKloose"
        ,"PhiVCHI2"
        ,"PhiMassWin"
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

        Dstar_cuts = {
            "Dstar_Chi2" : config["Dstar_Chi2"],
            "Dstar_SoftPion_PIDe" : config["Dstar_SoftPion_PIDe"],
            "Dstar_SoftPion_PT" : config["Dstar_SoftPion_PT"],
            "Dstar_wideDMCutLower" : config["Dstar_wideDMCutLower"],
            "Dstar_wideDMCutUpper" : config["Dstar_wideDMCutUpper"]
            }
        

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

        self.selKaonTight = Selection( "KTightfor" + name,
                                       Algorithm = FilterDesktop( Code = "(PIDK> %(KaonPIDKTight)s)" % self.__confdict__ ),
                                       RequiredSelections = [self.selKaon])
        
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
        self.seld02kpiDst = makeDstar('D02KPiDstarfor'+name,self.seld02kpi,Dstar_cuts)
        
        self.seld02kk = Selection( "D02KKfor" + name,
                                    Algorithm = self._D02KKFilter(),
                                    RequiredSelections = [self.selKaon] )
        self.seld02kkDst = makeDstar('D02KkDstarfor'+name,self.seld02kk,Dstar_cuts)

        self.seld02pipi = Selection( "D02PiPifor" + name,
                                     Algorithm = self._D02PiPiFilter(),
                                     RequiredSelections = [self.selPionTight] )
        self.seld02pipiDst = makeDstar('D02PipiDstarfor'+name,self.seld02pipi,Dstar_cuts)
        
        ################ D0 -> 4H SELECTION ##########################

        self.seld02k3pi = Selection( "D02K3Pifor" + name,
                                     Algorithm = self._D02HHHHFilter([ '[D0 -> K- pi+ pi- pi+]cc' ]),
                                     RequiredSelections = [self.selKaonTight, self.selPion])
        self.seld02k3piDst = makeDstar('D02K3PiDstarfor'+name,self.seld02k3pi,Dstar_cuts)

        self.sel_D0_to_3KPi = Selection( "D023KPifor"+name,
                                         Algorithm = self._D02HHHHFilter([ '[D0 -> K- K+ K- pi+]cc' ]),
                                         RequiredSelections = [self.selKaon, self.selPion])

        self.sel_D0_to_2K2Pi = Selection( "D022K2Pifor"+name,
                                         Algorithm = self._D02HHHHFilter([ 'D0 -> K- K+ pi- pi+' ]),
                                         RequiredSelections = [self.selKaon, self.selPion])
        
        ################ D+/Ds+ -> HHH SELECTIONS ##########################

        self.selD2KKK = Selection( "D2KKKfor" + name,
                                   Algorithm = self._D2HHHFilter(['[D+ -> K- K+ K+]cc']),
                                   RequiredSelections = [self.selKaonTight] )
        
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
                                     DecayDescriptors = [ '[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
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
        
        self.selb2D0MuXDst = makeb2DMuX('b2D0MuXDst' + name,
                                        DecayDescriptors = [ '[B0 -> D*(2010)- mu+]cc','[B0 -> D*(2010)- mu-]cc'],
                                        MuSel = self.selmuonhighPT, 
                                        DSel = self.seld02kpiDst,
                                        BVCHI2DOF = config['BVCHI2DOF'],
                                        BDIRA = config['BDIRA'],
                                        DZ = config['DZLoose']
                                        )

        self.selDstarOfb2D0MuX = Selection("DstarOfb2D0MuX" + name,
                                           Algorithm = self._LooseDstarFilter(),
                                           RequiredSelections = [self.selb2D0MuX, self.seld02kpi , StdAllNoPIDsPions] )
        
        self.selb2D0MuXKK = makeb2DMuX('b2D0MuXKK' + name,
                                       DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                       MuSel = self.selmuonhighPT, 
                                       DSel = self.seld02kk,
                                       BVCHI2DOF = config['BVCHI2DOF'],
                                       BDIRA = config['BDIRA'],
                                       DZ = config['DZLoose']
                                       )                   
        
        self.selb2D0MuXKKDst = makeb2DMuX('b2D0MuXKKDst' + name,
                                          DecayDescriptors = [ '[B0 -> D*(2010)- mu+]cc','[B0 -> D*(2010)- mu-]cc'],
                                          MuSel = self.selmuonhighPT, 
                                          DSel = self.seld02kkDst,
                                          BVCHI2DOF = config['BVCHI2DOF'],
                                          BDIRA = config['BDIRA'],
                                          DZ = config['DZLoose']
                                          )

        self.selDstarOfb2D0MuXKK = Selection("DstarOfb2D0MuXKK" + name,
                                             Algorithm = self._LooseDstarFilter(),
                                             RequiredSelections = [self.selb2D0MuXKK, self.seld02kk , StdAllNoPIDsPions] )

        
        self.selb2D0MuXpipi = makeb2DMuX('b2D0MuXpipi' + name,
                                         DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                         MuSel = self.selmuonhighPT, 
                                         DSel = self.seld02pipi,
                                         BVCHI2DOF = config['BVCHI2DOF'],
                                         BDIRA = config['BDIRA'],
                                         DZ = config['DZLoose']
                                         )                   
        
        self.selb2D0MuXpipiDst = makeb2DMuX('b2D0MuXpipiDst' + name,
                                            DecayDescriptors = [ '[B0 -> D*(2010)- mu+]cc','[B0 -> D*(2010)- mu-]cc'],
                                            MuSel = self.selmuonhighPT, 
                                            DSel = self.seld02pipiDst,
                                            BVCHI2DOF = config['BVCHI2DOF'],
                                            BDIRA = config['BDIRA'],
                                            DZ = config['DZLoose']
                                            )

        self.selDstarOfb2D0MuXpipi = Selection("DstarOfb2D0MuXpipi" + name,
                                             Algorithm = self._LooseDstarFilter(),
                                             RequiredSelections = [self.selb2D0MuXpipi, self.seld02kk , StdAllNoPIDsPions] )

        
        self.selb2D0MuXK3Pi = makeb2DMuX('b2D0MuXK3Pi' + name,
                                         DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                         MuSel = self.selmuonhighPT, 
                                         DSel = self.seld02k3pi,
                                         BVCHI2DOF = config['BVCHI2DOF'],
                                         BDIRA = config['BDIRA'],
                                         DZ = config['DZ']
                                         )        

        self.selb2D0MuXK3PiDst = makeb2DMuX('b2D0MuXK3PiDst' + name,
                                         DecayDescriptors = [ '[B0 -> D*(2010)- mu+]cc', '[B0 -> D*(2010)- mu-]cc' ],
                                         MuSel = self.selmuonhighPT, 
                                         DSel = self.seld02k3piDst,
                                         BVCHI2DOF = config['BVCHI2DOF'],
                                         BDIRA = config['BDIRA'],
                                         DZ = config['DZ']
                                         )        
        
        self.selb2D0MuX2K2Pi = makeb2DMuX('b2D0MuX2K2Pi' + name,
                                         DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                         MuSel = self.selmuonhighPT, 
                                         DSel = self.sel_D0_to_2K2Pi,
                                         BVCHI2DOF = config['BVCHI2DOF'],
                                         BDIRA = config['BDIRA'],
                                         DZ = config['DZLoose']
                                         )        

        self.selb2D0MuX3KPi = makeb2DMuX('b2D0MuX3KPi' + name,
                                         DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                         MuSel = self.selmuonhighPT, 
                                         DSel = self.sel_D0_to_3KPi,
                                         BVCHI2DOF = config['BVCHI2DOF'],
                                         BDIRA = config['BDIRA'],
                                         DZ = config['DZLoose']
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
        
        self.selb2DMuXKKK = makeb2DMuX('b2DMuXKKK' + name,
                                       DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                       MuSel = self.selmuonhighPT, 
                                       DSel = self.selD2KKK,
                                       BVCHI2DOF = config['BVCHI2DOF'],
                                       BDIRA = config['BDIRA'],
                                       DZ = config['DZLoose']
                                       )
        
        self.selb2DsMuXPhiPi = makeb2DMuX('b2DsMuXPhiPi' + name,
                                          DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                          MuSel = self.selmuonhighPT, 
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

        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}

        ########## D0 -> HH ###########
        self.b2D0MuXLine = StrippingLine('b2D0MuX' + name + 'Line', 
                                         prescale = config['PrescalD0Mu'], 
                                         selection = self.selb2D0MuX)

        self.DstarOfb2D0MuXLine = StrippingLine('DstarOfb2D0MuX' + name + 'Line', 
                                                prescale = config['PrescalD0Mu'], 
                                                selection = self.selDstarOfb2D0MuX)
        
        self.b2D0MuXKpiDCSLine = StrippingLine('b2D0MuXKpiDCS' + name + 'Line', 
                                               prescale = 1, 
                                               selection = self.selb2D0MuXKpiDCS)
        
        self.b2D0MuXKKLine = StrippingLine('b2D0MuXKK' + name + 'Line', 
                                           prescale = 1, 
                                           selection = self.selb2D0MuXKK)

        self.DstarOfb2D0MuXKKLine = StrippingLine('DstarOfb2D0MuXKK' + name + 'Line', 
                                                  prescale = 1, 
                                                  selection = self.selDstarOfb2D0MuXKK)
        
        self.b2D0MuXpipiLine = StrippingLine('b2D0MuXpipi' + name + 'Line', 
                                             prescale = 1, 
                                             selection = self.selb2D0MuXpipi)
        
        self.DstarOfb2D0MuXpipiLine = StrippingLine('DstarOfb2D0MuXpipi' + name + 'Line', 
                                                  prescale = 1, 
                                                  selection = self.selDstarOfb2D0MuXpipi)

        
        self.b2D0MuXDstLine = StrippingLine('b2D0MuXDst' + name + 'Line', 
                                            selection = self.selb2D0MuXDst,
                                            EnableFlavourTagging = True)

        self.b2D0MuXKKDstLine = StrippingLine('b2D0MuXKKDst' + name + 'Line', 
                                              selection = self.selb2D0MuXKKDst)
                                              
        self.b2D0MuXpipiDstLine = StrippingLine('b2D0MuXpipiDst' + name + 'Line', 
                                                selection = self.selb2D0MuXpipiDst)
        
        ########## D+ -> 3H ################
        self.b2DpMuXLine = StrippingLine('b2DpMuX' + name + 'Line', 
                                         prescale = 1, 
                                         FILTER=GECs,
                                         selection = self.selb2DpMuX,
                                         EnableFlavourTagging = True)
        
        self.b2DsMuXLine = StrippingLine('b2DsMuX' + name + 'Line', 
                                         prescale = 1, FILTER=GECs,
                                         selection = self.selb2DsMuX)
        
        self.b2DMuXKKKLine = StrippingLine('b2DMuXKKK' + name + 'Line', 
                                           prescale = 1, 
                                           FILTER=GECs,
                                           selection = self.selb2DMuXKKK)

        self.b2DsMuXPhiPiLine = StrippingLine('b2DsMuXPhiPi' + name + 'Line', 
                                              prescale = 1, 
                                              FILTER=GECs,
                                              selection = self.selb2DsMuXPhiPi)

        self.b2DsPi_PhiPi_fakesLine = StrippingLine('b2DsPi_PhiPi_fakes' + name + 'Line'
                                                    , HLT     = "HLT_PASS_RE('Hlt2IncPhi.*Decision')"
                                                    , prescale = config['PrescalDsPi_fakes']
                                                    , FILTER = GECs
                                                    , selection = self.selb2DsPi_PhiPi_fakes)

        ########### D0 -> 4H
        self.b2D0MuXK3PiLine = StrippingLine('b2D0MuXK3Pi' + name + 'Line', 
                                             prescale = 1, 
                                             FILTER=GECs, 
                                             selection = self.selb2D0MuXK3Pi)
        
        self.b2D0MuXK3PiDstLine = StrippingLine('b2D0MuXK3PiDst' + name + 'Line', 
                                                prescale = 1, 
                                                FILTER=GECs, 
                                                selection = self.selb2D0MuXK3PiDst,
                                                EnableFlavourTagging = True)
        
        self.b2D0MuX3KPiLine = StrippingLine('b2D0MuX3KPi' + name + 'Line', 
                                             prescale = 1, 
                                             FILTER=GECs, 
                                             selection = self.selb2D0MuX3KPi)

        self.b2D0MuX2K2PiLine = StrippingLine('b2D0MuX2K2Pi' + name + 'Line', 
                                              prescale = 1, 
                                              FILTER=GECs, 
                                              selection = self.selb2D0MuX2K2Pi)
        
        ########## Lambda_c+ -> p HH 
        self.b2LcMuXLine = StrippingLine('b2LcMuX' + name + 'Line', 
                                         prescale = 1, 
                                         FILTER=GECs,
                                         selection = self.selb2LcMuX)
        
        ############## REGISTER THE LINES #####################

        ######### lines from stripping 17 #######
        self.registerLine(self.b2D0MuXLine)        
        self.registerLine(self.b2D0MuXKpiDCSLine)        
        self.registerLine(self.b2DpMuXLine)
        self.registerLine(self.b2DMuXKKKLine)
        self.registerLine(self.b2DsMuXLine)
        self.registerLine(self.b2LcMuXLine)

        self.registerLine(self.b2D0MuXK3PiLine)
        self.registerLine(self.b2D0MuXK3PiDstLine)
        self.registerLine(self.b2D0MuX2K2PiLine)
        self.registerLine(self.b2D0MuX3KPiLine)
        
        self.registerLine(self.b2D0MuXKKLine)
        self.registerLine(self.b2D0MuXpipiLine)

        self.registerLine(self.b2D0MuXDstLine)        
        self.registerLine(self.b2D0MuXKKDstLine)
        self.registerLine(self.b2D0MuXpipiDstLine)

        self.registerLine(self.DstarOfb2D0MuXLine)
        self.registerLine(self.DstarOfb2D0MuXKKLine)
        self.registerLine(self.DstarOfb2D0MuXpipiLine)
        
        self.registerLine(self.b2DsMuXPhiPiLine)
        self.registerLine(self.b2DsPi_PhiPi_fakesLine)

    def _muonFilter( self ):
        _code = "(PT > %(MuonPT)s *MeV) & (P> 3.0*GeV)"\
                "&(TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
                "& (TRCHI2DOF< %(TRCHI2Loose)s) & (MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s) & (PIDmu > %(PIDmu)s)" % self.__confdict__
        _mu = FilterDesktop( Code = _code )
        return _mu        

    def _pionFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "&(TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDK)s)" % self.__confdict__
        _pi = FilterDesktop( Code = _code )
        return _pi

    def _kaonFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "&(TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK> %(KaonPIDK)s)" % self.__confdict__
        _ka = FilterDesktop( Code = _code )
        return _ka 
  
    def _kaonlooseFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "&(TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2Loose)s) &  (PIDK> %(KaonPIDKloose)s)" % self.__confdict__
        _kal = FilterDesktop( Code = _code )
        return _kal 
    
    def _pionlooseFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "&(TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2Loose)s)" % self.__confdict__
        _pil = FilterDesktop( Code = _code )
        return _pil
    
    def _pionFilter_fakes( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "&(TRGHOSTPROB < %(TRGHOSTPROB)s)"\
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

    def _D02HHHHFilter( self , _decayDescriptors):
        ### 5 MeV wider mass window for combination
        _combinationCut = "(ADAMASS('D0')-5.0 < %(D02HHHHMassWin)s *MeV)"\
                          "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(D02HHHHSumPT)s *MeV)"\
                          "& (ADOCACHI2CUT( %(DDocaChi2MaxTight)s, ''))" % self.__confdict__
        _motherCut = " (ADMASS('D0') < %(D02HHHHMassWin)s *MeV)"\
                     " & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     " & (SUMTREE( PT,  ISBASIC )> %(D02HHHHSumPT)s *MeV)"\
                     " & (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02k3pi = CombineParticles( DecayDescriptors = _decayDescriptors,
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
    
    def _D2HHHFilter( self,_decayDescriptors ):
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        D2HHH = CombineParticles( DecayDescriptors = _decayDescriptors,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut)                             
        return D2HHH

    
    def _Lc2PKPiFilter( self ):
        _decayDescriptors = [ '[Lambda_c+ -> K- p+ pi+]cc' ]
        _daughtersCuts = {  "p+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(KPiPT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>1.0e-10)" % self.__confdict__}
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2MaxTight)s, ''))" % self.__confdict__
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
    

    def _LooseDstarFilter( self ) : 
        _cutsDstarComb = '(AM - ACHILD(M,1) + 5 > %(Dstar_wideDMCutLower)s *MeV) & (AM - ACHILD(M,1) - 5 < %(Dstar_wideDMCutUpper)s *MeV)' % self.__confdict__
        _cutsDstarMoth_base = '(VFASPF(VCHI2/VDOF) < %(Dstar_Chi2)s )' % self.__confdict__
        _cutsDstarMoth_DM = '(M - CHILD(M,1) > %(Dstar_wideDMCutLower)s *MeV) & (M - CHILD(M,1) < %(Dstar_wideDMCutUpper)s *MeV)' % self.__confdict__
        _cutsDstarMoth = '(' + _cutsDstarMoth_base + ' & ' + _cutsDstarMoth_DM + ')'
        _Dstar = CombineParticles( DecayDescriptors = ["[D*(2010)+ -> D0 pi+]cc" , "[D*(2010)+ -> D~0 pi+]cc"] ,
                                   CombinationCut = _cutsDstarComb,
                                   MotherCut = _cutsDstarMoth)
        return _Dstar

    
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


def makeDstar(_name, inputD0,Dstar_cuts) : 
    _softPi = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles')
    _inputD0_conj = Selection("SelConjugateD0For"+_name,
                             Algorithm = ConjugateNeutralPID('ConjugateD0For'+_name),
                             RequiredSelections = [inputD0])
    _cutsSoftPi = '( PT > %(Dstar_SoftPion_PT)s *MeV )' % Dstar_cuts
    _cutsDstarComb = '(AM - ACHILD(M,1) + 5 > %(Dstar_wideDMCutLower)s *MeV) & (AM - ACHILD(M,1) - 5 < %(Dstar_wideDMCutUpper)s *MeV)' % Dstar_cuts
    _cutsDstarMoth_base = '(VFASPF(VCHI2/VDOF) < %(Dstar_Chi2)s )' % Dstar_cuts
    _cutsDstarMoth_DM = '(M - CHILD(M,1) > %(Dstar_wideDMCutLower)s *MeV) & (M - CHILD(M,1) < %(Dstar_wideDMCutUpper)s *MeV)' % Dstar_cuts
    _cutsDstarMoth = '(' + _cutsDstarMoth_base + ' & ' + _cutsDstarMoth_DM + ')'
    _Dstar = CombineParticles( DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc",
                               DaughtersCuts = { "pi+" : _cutsSoftPi },
                               CombinationCut = _cutsDstarComb,
                               MotherCut = _cutsDstarMoth)
    return Selection (name = "Sel"+_name,Algorithm = _Dstar,RequiredSelections = [inputD0,_inputD0_conj] + [_softPi])


