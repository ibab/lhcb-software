#$Id$
__author__ = ['Zhou Xing']
__date__ = '28/11/2012'
__version__ = '$Revision: 1.0 $'
"""
B->(D-> pi pi)3pi and /\b->(/\c-> K pi)3pi selections for Kaon and Proton tracking efficinecy study
"""
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseKaons


name = 'confB2DHHHForBXX'

__all__ = ('B2DHHHForBXXConf',
           'makeB2Da1',
           'TOSFilter',
           'confdict')

confdict =  {
    "PreScale"      : 1.0   # adimensiional        
    ,"TRCHI2"        : 3.0   # adimensiional
    ,"KaonPIDK"      : 4.0   # adimensiional
    ,"PionPIDK"      : 2.0   # adimensiional
    ,"DDocaChi2Max" : 10   #adimensiional
    }


class B2DHHHForBXXLinesConf(LineBuilder):

    __configuration_keys__ = (
        "PreScale"     
        ,"TRCHI2"       
        ,"KaonPIDK"     
        ,"PionPIDK"     
        ,"DDocaChi2Max" 
        )

    __confdict__={}

    def __init__(self, name, config) :
        
        LineBuilder.__init__(self, name, config)
        self.__confdict__=config        
        

        self.sela3pi = Selection("A2PiPiPifor" + name,
                                 Algorithm = self._a2PiPiPi(),
                                 RequiredSelections = [ StdLoosePions ])


        self.seld2pi = Selection("D2PiPifor" + name,
                                 Algorithm = self._D2PiPi(),
                                 RequiredSelections = [ StdLoosePions ])

        self.sellc2kpi = Selection("Lc2KPifor" + name,
                                   Algorithm = self._LcKPi(),
                                   RequiredSelections = [ StdLoosePions ,  StdLooseKaons ])        
        
        self.selB2Dhhh = makeB2Da1("B2DHHHwDPiPi" + name,
                                   DecayDescriptor = '[B0 -> D- a_1(1260)+]cc',
                                   DaughterLists = [ self.sela3pi , self.seld2pi])
                                     
        self.selB2Dhhh_tos = TOSFilter( "B2DHHHwDPiPi_TOS" + name,
                                        self.selB2Dhhh)        
        
        self.B2DhhhLine = StrippingLine('B2DHHHwD2PiPi' + name + 'Line'
                                        , prescale = config['PreScale']
                                        , selection = self.selB2Dhhh_tos
                                        )

        self.selLb2Lchhh = makeB2Da1("Lb2LcHHHwLcKPi" + name,
                                     DecayDescriptor = '[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc',
                                     DaughterLists = [ self.sela3pi, self.sellc2kpi])
        
        self.selLb2Lchhh_tos = TOSFilter( "Lb2LcHHHwLcKPi_TOS" + name,
                                        self.selLb2Lchhh)
        
        self.Lb2LchhhLine = StrippingLine('Lb2LcHHHwLc2KPi' + name + 'Line'
                                        , prescale = config['PreScale']
                                        , selection = self.selLb2Lchhh_tos
                                          )
        
        self.registerLine(self.B2DhhhLine)
        self.registerLine(self.Lb2LchhhLine)
 
        
    def _D2PiPi( self ):
            _decayDescriptors = ['[D- -> pi- pi-]cc']
            _combinationCut = "(AM<1.38*GeV) & (APT > 800*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, '')) & (AMAXDOCA('') < 0.5*mm)" % self.__confdict__
            _daughtersCuts = {  "pi+" : "(TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > 400 *MeV)"\
                                "& (MIPCHI2DV(PRIMARY)> 9.0) &  (PIDK< %(PionPIDK)s) & (PIDmu< 10) & (TRGHOSTPROB < 0.5)" % self.__confdict__,
                                }
            _motherCut = "(VFASPF(VCHI2/VDOF)<5.0) & (MM<1.38*GeV) & (BPVVDCHI2>120) & (BPVDIRA>0.99) "
            return CombineParticles(DecayDescriptors = _decayDescriptors,
                                    DaughtersCuts = _daughtersCuts,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut,
                                    )

    def _LcKPi( self ):
        _decayDescriptors = ['[Lambda_c+ -> K- pi+]cc']
        _combinationCut = "(AM<1.36*GeV) & (APT > 800*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, '')) & (AMAXDOCA('') < 0.5*mm)" % self.__confdict__
        _daughtersCuts = {  "pi+" : "(TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > 400 *MeV)"\
                            "& (MIPCHI2DV(PRIMARY)> 9.0) &  (PIDK< 2) & (PIDmu< 10) & (TRGHOSTPROB < 0.5)" % self.__confdict__,
                            "K+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > 400 *MeV) & (P>2.0*GeV) "\
                            "& (MIPCHI2DV(PRIMARY)> 9.0)  &  (PIDK> %(KaonPIDK)s) & (PIDK-PIDp> 0) & (TRGHOSTPROB < 0.5)" % self.__confdict__ }
        _motherCut = "(VFASPF(VCHI2/VDOF)<5.0) & (MM<1.36*GeV) & (BPVVDCHI2>120) & (BPVDIRA>0.99) "
        return CombineParticles(DecayDescriptors = _decayDescriptors,
                                DaughtersCuts = _daughtersCuts,
                                CombinationCut = _combinationCut,
                                MotherCut = _motherCut,
                                )
    
    
    
    def _a2PiPiPi( self ):
            _decayDescriptors = ['[a_1(1260)+ -> pi+ pi- pi+]cc']
            _combinationCut = "(AM>0.5*GeV) & (AM<3.4*GeV) & (APT > 2.1*GeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, '')) & (AMAXDOCA('') < 0.5*mm)" % self.__confdict__
            _daughtersCuts = {  "pi+" : "(TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > 400 *MeV)"\
                                "& (MIPCHI2DV(PRIMARY)> 9.0) &  (PIDK< %(PionPIDK)s) & (PIDmu< 10) & (TRGHOSTPROB < 0.5)" % self.__confdict__,
                                }
            _motherCut = "(VFASPF(VCHI2/VDOF)<5.0) & (MM<3.4*GeV) & (MM>0.5*GeV) & (BPVVDCHI2>120)  "
            return CombineParticles(DecayDescriptors = _decayDescriptors,
                                    DaughtersCuts = _daughtersCuts,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut,
                                    )
        
        
def makeB2Da1(name,
              DecayDescriptor,
              DaughterLists):
    #    _decayDescriptor = '[B0 -> D- a_1(1260+)]cc' '[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc'
    _combinationCut = "(AM<4.8*GeV) & (AM>3*GeV) & (APT>1.5*GeV) & (AP>20.0*GeV) & (AM-AM1<4.5*GeV) & (AM-AM1>1.0*GeV) & (AM-AM2<4.0*GeV) & (AM-AM2>0.3*GeV) "
    _daughtersCuts = { 'D-': "ALL",'a_1(1260)+':"ALL",'Lambda_c+': "ALL" }
    _motherCut = "(MM<4.8*GeV) & (MM>3*GeV) & (VFASPF(VCHI2/VDOF)<10.0) & (BPVVDCHI2>120.0) & (BPVIPCHI2()<25.0) &  (BPVDIRA> 0.999) & (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > 0 *mm )"
    _dst = CombineParticles(DecayDescriptor = DecayDescriptor,
                            DaughtersCuts = _daughtersCuts,
                            CombinationCut = _combinationCut,
                            MotherCut = _motherCut,
                            )        
    return Selection(name,
                     Algorithm = _dst,
                     RequiredSelections = DaughterLists)

        
        
        
def TOSFilter( name = None, sel = None ):
    """
    Function to return a selection object, filtering for TOS candidates from input selection
    """
    from Configurables import TisTosParticleTagger
    
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = { "Hlt1Global%TOS" : 0, "Hlt2Global%TOS" : 0 }
    _filter.NoRegex = True
    
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel

