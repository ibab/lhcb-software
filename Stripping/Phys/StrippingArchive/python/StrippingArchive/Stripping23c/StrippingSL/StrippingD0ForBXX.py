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
from StandardParticles import StdLoosePions, StdLooseKaons, StdLooseProtons, StdAllLoosePions


name = 'confD0forBXX'

__all__ = ('D0forBXXConf',
           'makeDst2D0Pi',
           'TOSFilter',
           'confdict')
default_config = {
  'D0forBXX' : {
        'WGs'         : ['Semileptonic'],
	'BUILDERTYPE' : 'D0forBXXLinesConf',
        'CONFIG'      :{
	  "PreScale"      : 0.005   # adimensiional        
	  ,"PreScaleRS"   : 1.0    # adimensiional
	  ,"PreScaleWS"   : 0.2    # adimensiional
	  ,"MINIPCHI2"     : 9.0   # adimensiional
	  ,"TRCHI2"        : 3.0   # adimensiional
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
	},
       'STREAMS'     : ['Semileptonic']	  
      }
    }


class D0forBXXLinesConf(LineBuilder):

    __configuration_keys__ = (
         "PreScale"     
        ,"PreScaleRS"
        ,"PreScaleWS" 
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

        self.seld02k2pi = Selection("D02K2Pifor" + name,
                                    Algorithm = self._D02K2Pi(),
                                    RequiredSelections = [ StdLooseKaons, StdLoosePions ])

        self.seldstrs = makeDst2D0Pi("DstRSwD0K2Pi" + name,
                                  DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc',
                                  DaughterLists = [ self.seld02k2pi, StdAllLoosePions ])

        self.seldstws = makeDst2D0Pi("DstWSwD0K2Pi" + name,
                                     DecayDescriptor = '[D*(2010)- -> D0 pi-]cc',
                                     DaughterLists = [ self.seld02k2pi, StdAllLoosePions ])
                                     
        self.seldstrs_tos = TOSFilter( "DstRSwD0K2Pi_TOS" + name,
                                     self.seldstrs,
                                     "Hlt2Global")

        self.seldstws_tos = TOSFilter( "DstWSwD0K2Pi_TOS" + name,
                                     self.seldstws,
                                     "Hlt2Global")        
        
        self.D02KPiLine = StrippingLine('D02KPi' + name + 'Line'
                                        , prescale = config['PreScale']
                                        , selection = self.seld02kpi
                                        )
        self.D02K3PiLine = StrippingLine('D02K3Pi' + name + 'Line'
                                         , prescale = config['PreScale']
                                         , selection = self.seld02k3pi
                                        )

        self.DstRSLine = StrippingLine('DstRSwD02K2Pi' + name + 'Line'
                                       , prescale = config['PreScaleRS']
                                       , selection = self.seldstrs_tos
                                       )
        
        self.DstWSLine = StrippingLine('DstWSwD02K2Pi' + name + 'Line'
                                       , prescale = config['PreScaleWS']
                                       , selection = self.seldstws_tos
                                       )        
        
        self.registerLine(self.D02KPiLine)
        self.registerLine(self.D02K3PiLine)
        self.registerLine(self.DstRSLine)
        self.registerLine(self.DstWSLine)
        
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
    def _D02K2Pi( self ):
        _decayDescriptors = ['[D0 -> pi+ pi- K-]cc']
        _combinationCut = "(AM>1.4*GeV) & (AM<1.7*GeV) & (AM12>575.5*MeV) & (AM12<975.5*MeV) & (APT > 3000*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = {  "pi+" : "(TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > 400 *MeV)"\
                     "& (MIPCHI2DV(PRIMARY)> 4.0) &  (PIDK< %(PionPIDK)s) & (PIDmu< 10)" % self.__confdict__,
                            "K+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > 400 *MeV) & (P>2.0*GeV) "\
                    "& (MIPCHI2DV(PRIMARY)> 4.0)  &  (PIDK> %(KaonPIDK)s)" % self.__confdict__  }
        _motherCut = "(VFASPF(VCHI2/VDOF)<6.0) & (BPVVD>4*mm) & (BPVVDCHI2>120) & (BPVDIRA>0.9997) & (BPVIPCHI2()<25.)"
        return CombineParticles(DecayDescriptors = _decayDescriptors,
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


def makeDst2D0Pi(name,
                 DecayDescriptor,
                 DaughterLists):
    #    _decayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
    _combinationCut = "(AM-AM1-139.57*MeV<40.0*MeV) & (APT>3.0*GeV)"
    _daughtersCuts = { "pi+" : "(PT > 250*MeV) & (TRCHI2DOF < 3.0) & (MIPDV(PRIMARY)< 0.3*mm) & (MIPCHI2DV(PRIMARY)<4.0) & (PIDK < 10)"}
    _motherCut = "(VFASPF(VCHI2/VDOF)<5.0) & (BPVVDCHI2 < 25) & (BPVIPCHI2() < 25.)"# & (MINTREE((ABSID=='D0') , VFASPF(VZ))-VFASPF(VZ) > 3 *mm )"
    _dst = CombineParticles(DecayDescriptor = DecayDescriptor,
                            DaughtersCuts = _daughtersCuts,
                            CombinationCut = _combinationCut,
                            MotherCut = _motherCut,
                            )        
    return Selection(name,
                     Algorithm = _dst,
                     RequiredSelections = DaughterLists)

    
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
