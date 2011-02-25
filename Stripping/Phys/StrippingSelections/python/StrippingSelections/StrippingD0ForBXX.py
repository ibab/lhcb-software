#$Id$
__author__ = ['Liming Zhang']
__date__ = '20/05/2010'
__version__ = '$Revision: 1.4 $'
"""
D0->Kpi and D0->K3pi selections for pi tracking efficinecy check
"""
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseKaons, StdLooseProtons


name = 'confD0forBXX'

__all__ = ('D0forBXXConf',
           'confdict')

confdict =  {
    "PreScale"      : 0.03   # adimensiional        
    ,"MINIPCHI2"     : 9.0   # adimensiional
    ,"TRCHI2"        : 5.0   # adimensiional
    ,"KaonPIDK"      : 4.0   # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PT"            : 300.0  # MeV
    ,"D0DIRA"        : 0.9999    # adimensiional
    ,"D0DIRAK3Pi"    : 0.9999    # adimensiional        
    ,"FDCHI2"        : 100.0   # adimensiional
    ,"D0AMassWin"    : 100.0   # MeV
    ,"D0MassWin"     : 80.0  # MeV
    ,"D0IP"          : 0.2    #mm
    ,"D0K3PiIP"      : 0.2    #mm        
    ,"D0VCHI2DOF"    : 6.0   # adimensiional
    ,"DDocaChi2Max" : 20   #adimensiional
    }


class D0forBXXLinesConf(LineBuilder):

    __configuration_keys__ = (
         "PreScale"     
        ,"MINIPCHI2"    
        ,"TRCHI2"       
        ,"KaonPIDK"     
        ,"PionPIDK"     
        ,"PT"        
        ,"D0DIRA"       
        ,"D0DIRAK3Pi"   
        ,"FDCHI2"       
        ,"D0AMassWin"   
        ,"D0MassWin"    
        ,"D0IP"         
        ,"D0K3PiIP"     
        ,"D0VCHI2DOF"   
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

        self.seld02kpi = Selection("D02KPifor" + name,
                                   Algorithm = self._D02KPi(),
                                   RequiredSelections = [ StdLooseKaons, StdLoosePions ])
        self.seld02k3pi = Selection("D02K3Pifor" + name,
                                   Algorithm = self._D02K3Pi(),
                                   RequiredSelections = [ StdLooseKaons, StdLoosePions ])
        self.D02KPiLine = StrippingLine('D02KPi' + name + 'Line'
                                        , prescale = config['PreScale']
                                        , selection = self.seld02kpi
                                        )
        self.D02K3PiLine = StrippingLine('D02K3Pi' + name + 'Line'
                                         , prescale = config['PreScale']
                                         , selection = self.seld02k3pi
                                        )

        self.registerLine(self.D02KPiLine)
        self.registerLine(self.D02K3PiLine)
        
    def _D02KPi( self ):
        _decayDescriptor = '[D0 -> pi+ K-]cc'
        _combinationCut = "(ADAMASS('D0') < %(D0AMassWin)s *MeV) & (APT > 1500*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = {  "pi+" : self.PiCut,
                            "K+" :  self.KCut  }
        _motherCut = "(VFASPF(VCHI2/VDOF) < %(D0VCHI2DOF)s) & (BPVIP()< %(D0IP)s *mm)" \
                        "& (BPVVDCHI2 > %(FDCHI2)s) & (BPVDIRA> %(D0DIRA)s) & (ADMASS('D0') < %(D0MassWin)s *MeV)"  % self.__confdict__
        return CombineParticles(DecayDescriptor = _decayDescriptor,
                                DaughtersCuts = _daughtersCuts,
                                CombinationCut = _combinationCut,
                                MotherCut = _motherCut,
                                )
    
    def _D02K3Pi( self ):
        _decayDescriptor = '[D0 -> K- pi+ pi- pi+]cc' 
        _combinationCut = "(ADAMASS('D0') < %(D0AMassWin)s *MeV) & (APT > 1500.*MeV) &  (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (PT > 250 *MeV) & (P>2.0*GeV)"\
                           "& (TRCHI2DOF < %(TRCHI2)s)" % self.__confdict__,
                           "K+" :  self.KCut }
        _motherCut = " (ADMASS('D0') < %(D0MassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(D0VCHI2DOF)s) " \
                     "& (INTREE((ABSID=='pi+')& (PT > %(PT)s *MeV) &(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (PIDK< %(PionPIDK)s)))"  \
                     "& (BPVVDCHI2 > %(FDCHI2)s) &  (BPVDIRA> %(D0DIRAK3Pi)s) & (BPVIP()< %(D0K3PiIP)s *mm)"  % self.__confdict__
        return CombineParticles( DecayDescriptor = _decayDescriptor,
                                 DaughtersCuts = _daughtersCuts,
                                 CombinationCut = _combinationCut,
                                 MotherCut = _motherCut)

        
    
