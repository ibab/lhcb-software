# $Id$
__author__ = ['Liming Zhang']
__date__ = '10/08/2010'
__version__ = '$Revision: 1.2 $'
"""
D0, D+, Ds, /\c selections as control sample for B->DMuNuX 
"""
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseKaons, StdLooseProtons


__all__ = ('DforBSemiLinesConf',
           'confdict')

confdict = {
     "DpPreScale"    : 0.02
    ,"DsPreScale"    : 0.02   
    ,"LcPreScale"    : 0.05   
    ,"MINIPCHI2"     : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0   # adimensiional
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PT"            : 300.0  # MeV
    ,"DsDIRA"        : 0.99    # adimensiional
    ,"DsFDCHI2"      : 100.0   # adimensiional
    ,"DsMassWin"     : 80.0  # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"DsIP"          : 7.4    # mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"DDocaChi2Max"  : 20   #adimensiional
    }


    
class DforBSemiLinesConf(LineBuilder):
    
    __configuration_keys__ = (
        "DpPreScale"       
        ,"DsPreScale"               
        ,"LcPreScale"               
        ,"MINIPCHI2"     
        ,"TRCHI2"        
        ,"KaonPIDK"      
        ,"PionPIDK"      
        ,"PT"            
        ,"DsDIRA"        
        ,"DsFDCHI2"      
        ,"DsMassWin"     
        ,"DsAMassWin"    
        ,"DsIP"          
        ,"DsVCHI2DOF"    
        ,"DDocaChi2Max"  
        )
    __confdict__={}

    def __init__(self, name, config) :
        
        LineBuilder.__init__(self, name, config)
        self.__confdict__=config

        self.PiCut = "(TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(PT)s *MeV)"\
                     "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDK)s)" % self.__confdict__
        self.KCut = "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDK> %(KaonPIDK)s)" % self.__confdict__
        self.PCut = "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>1.0e-10)" % self.__confdict__
        
        self.selDp2KPiPi = Selection("Dp2KPiPifor" + name,
                                     Algorithm = self._Dp2KPiPiFilter(),
                                     RequiredSelections = [StdLooseKaons, StdLoosePions])
        
        self.selDs2KKPi = Selection("Ds2KKPifor" + name,
                                    Algorithm = self._Ds2KKPiFilter(),
                                    RequiredSelections = [StdLooseKaons, StdLoosePions])
        
        self.selLc2PKPi = Selection("Lc2PKPi" + name,
                                    Algorithm = self._Lc2PKPiFilter(),
                             RequiredSelections = [StdLooseProtons, StdLooseKaons, StdLoosePions])
                                                   
        
        self.Dp2KPiPiLine = StrippingLine('Dp2KPiPi' + name + 'Line'
                                          , prescale = config['DpPreScale']
                                          , selection = self.selDp2KPiPi
                                          )

        self.Ds2KKPiLine = StrippingLine('Ds2KKPi' + name + 'Line'
                                         , prescale = config['DsPreScale']
                                         , selection = self.selDs2KKPi
                                         )
        self.Lc2PKPiLine = StrippingLine('Lc2PKPi' + name + 'Line'
                                         , prescale = config['LcPreScale']
                                         , selection = self.selLc2PKPi
                                         )
        self.registerLine(self.Dp2KPiPiLine)
        self.registerLine(self.Ds2KKPiLine)
        self.registerLine(self.Lc2PKPiLine)
        

    def _Dp2KPiPiFilter( self ):
        _decayDescriptors = [ '[D+ -> K- pi+ pi+]cc' ]
        _daughtersCuts = { "pi+" : self.PiCut,
                           "K+" :  self.KCut }
        _combinationCut = "(ADAMASS('D+') < %(DsAMassWin)s *MeV)  & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) & (ADMASS('D+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) & (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.__confdict__
        _dp2kpipi = CombineParticles(DecayDescriptors = _decayDescriptors,
                                     DaughtersCuts = _daughtersCuts,
                                     CombinationCut = _combinationCut,
                                     MotherCut = _motherCut,
                                     )
        return _dp2kpipi
    
    
    def _Ds2KKPiFilter( self ):
        _decayDescriptors = [ '[D+ -> K+ K- pi+]cc' ]
        _daughtersCuts = { "pi+" : self.PiCut,
                           "K+" :  self.KCut }
        _combinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV)  & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) & (DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                     "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) & (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.__confdict__
        _ds2kkpi = CombineParticles(DecayDescriptors = _decayDescriptors,
                                    DaughtersCuts = _daughtersCuts,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut,
                                    )
        return _ds2kkpi
    
    
    def _Lc2PKPiFilter( self ):
        _decayDescriptors = [ '[Lambda_c+ -> K- p+ pi+]cc' ]
        _daughtersCuts = { "pi+" : self.PiCut,
                           "K+" :  self.KCut,
                           "p+" :  self.PCut}
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(DsFDCHI2)s) & (BPVDIRA> %(DsDIRA)s) &   (BPVIP()< %(DsIP)s *mm)"  % self.__confdict__
        _lambdac = CombineParticles(DecayDescriptors = _decayDescriptors,
                                    DaughtersCuts = _daughtersCuts,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut,
                                    )
        return _lambdac
    
    
