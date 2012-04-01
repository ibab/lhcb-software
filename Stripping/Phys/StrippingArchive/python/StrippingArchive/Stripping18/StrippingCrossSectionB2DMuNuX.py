"""
Module for constructing B semileptonic inclusive channels:
B->D0XMuNu, D+XMuNu, Ds+XMuNu, Lc+XMuNu with
D0->Kpi, D+->Kpipi, KKPi, Ds+ -> KKPi and Lc+->PKPi
"""
__author__ = ['Liming Zhang, Alessandra Borgia']
__date__ = '23/07/2010 - modified on 30-March-2012 for CrossSection'
__version__ = '$Revision: 1.8 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdLooseMergedPi0,StdLooseResolvedPi0

__all__ = ('CrossSectionB2DMuNuXAllLinesConf',
           'makeCrossSectionb2DMuX',
           'confdict')

confdict = {
    "MINIPCHI2"     : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"TRCHI2Loose"   : 5.0    # adimensiional    
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
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
    ,"DZ"            : 0.0    #mm
    ,"DDocaChi2Max"  : 20     #adimensiional
    ,"MINIPCHI2Loose" : 4.0   #adimensiional
    ,"KaonPIDKloose" : -5     #adimensiional
    }

class CrossSectionB2DMuNuXAllLinesConf(LineBuilder) :
    """
    """

    __configuration_keys__ = (
        "MINIPCHI2"     
        ,"TRCHI2"     
        ,"TRCHI2Loose"   
        ,"KaonPIDK"      
        ,"PionPIDK"
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
        ,"DDocaChi2Max"
        ,"MINIPCHI2Loose"
        ,"KaonPIDKloose"
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

        ############### KAON AND PION SELECTIONS ################
        
        self.selKaon = Selection( "Kfor" + name,
                                  Algorithm = self._kaonFilter(),
                                  RequiredSelections = [StdLooseKaons])
        
        self.selPion = Selection( "Pifor" + name,
                                  Algorithm = self._pionFilter(),
                                  RequiredSelections = [StdLoosePions])

        ################ D0 -> HH SELECTION ##########################
        
        self.seld02kpi = Selection( "D02KPifor" + name,
                                    Algorithm = self._D02KPiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion] )


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

        
        ############ Lambda_c+ -> pKpi ########################
        self.sellambdac = Selection( "Lc2PKPifor" + name,
                                     Algorithm = self._Lc2PKPiFilter(),
                                     RequiredSelections = [self.selKaon, self.selPion, StdLooseProtons ] )
        
        
        ################ MAKE THE B CANDIDATES #################################
        
        self.selb2D0MuX = makeCrossSectionb2DMuX('b2D0MuX' + name,
                                     DecayDescriptors = [ '[B- -> D0 mu-]cc' ],
                                     MuSel = self.selmuonhighPT, 
                                     DSel = self.seld02kpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA'],
                                     DZ = config['DZ']
                                     )
        
        
        self.selb2D0MuXK3Pi = makeCrossSectionb2DMuX('b2D0MuXK3Pi' + name,
                                         DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                         MuSel = self.selmuontight, 
                                         DSel = self.seld02k3pi,
                                         BVCHI2DOF = config['BVCHI2DOF'],
                                         BDIRA = config['BDIRA'],
                                         DZ = config['DZ']
                                         )        

        
        self.selb2DpMuX = makeCrossSectionb2DMuX('b2DpMuX' + name,
                                     DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                     MuSel = self.selmuonnew, 
                                     DSel = self.seldp2kpipi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA'],
                                     DZ = config['DZ']
                                     )
        
        self.selb2DsMuX = makeCrossSectionb2DMuX('b2DsMuX' + name,
                                     DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                     MuSel = self.selmuonnew, 
                                     DSel = self.selds2kkpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA'],
                                     DZ = config['DZ']
                                     )
        
        self.selb2LcMuX = makeCrossSectionb2DMuX('b2LcMuX' + name,
                                     DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                     MuSel = self.selmuonnew, 
                                     DSel = self.sellambdac,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA'],
                                     DZ = config['DZ']
                                     )         
        
         
        ################# DECLARE THE STRIPPING LINES #################################
        self.b2D0MuXLine = StrippingLine('b2D0MuX' + name + 'Line', prescale = 1, selection = self.selb2D0MuX)
        self.b2DpMuXLine = StrippingLine('b2DpMuX' + name + 'Line', prescale = 1, selection = self.selb2DpMuX)
        self.b2DsMuXLine = StrippingLine('b2DsMuX' + name + 'Line', prescale = 1, selection = self.selb2DsMuX)
        self.b2D0MuXK3PiLine = StrippingLine('b2D0MuXK3Pi' + name + 'Line', prescale = 1, selection = self.selb2D0MuXK3Pi)
        self.b2LcMuXLine = StrippingLine('b2LcMuX' + name + 'Line', prescale = 1, selection = self.selb2LcMuX)
        
        ############## REGISTER THE LINES #####################
        self.registerLine(self.b2D0MuXLine)        
        self.registerLine(self.b2DpMuXLine)
        self.registerLine(self.b2DsMuXLine)
        self.registerLine(self.b2LcMuXLine)
        self.registerLine(self.b2D0MuXK3PiLine)
        
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
  
    
    def _D02KPiFilter( self ):
        _decayDescriptors = [ '[D0 -> K- pi+]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1400.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1400.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02kpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _d02kpi
  

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

    
    
def makeCrossSectionb2DMuX(name,
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


