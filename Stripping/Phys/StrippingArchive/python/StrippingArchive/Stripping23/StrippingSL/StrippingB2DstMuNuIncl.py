__author__ = ['Mika Vesterinen']
__date__ = '04/04/2015'
__version__ = '$Revision: 0.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdAllLoosePions

__all__ = ('B2DstMuNuInclAllLinesConf',
           'TOSFilter',
           'default_config')

default_config = {
    'B2DstMuNuIncl' : {
        'WGs'         : ['Semileptonic'],
        'BUILDERTYPE' : 'B2DstMuNuInclAllLinesConf',
        'CONFIG'      : {
            "TTSpecs"      : {}#{'Hlt1.*Track.*Decision%TOS':0,'Hlt2TopoMu(2|3|4)Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0} 
            ,"HLT_FILTER"   : ""#"HLT_PASS_RE('Hlt2.*SingleMuon.*Decision') | HLT_PASS_RE('Hlt2TopoMu(2|3|4)Body.*Decision')"
            },
        'STREAMS'     : ['Semileptonic']
        }
    }

class B2DstMuNuInclAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "TTSpecs"
        ,"HLT_FILTER"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        
        self.D0 = Selection("D0For"+name,
                            Algorithm=CombineParticles(DecayDescriptors = ["[D0 -> K- pi+]cc"],
                                                       DaughtersCuts = {"K-":"(PIDK > 10) & (PT > 800*MeV)",
                                                                        "pi+":"(PIDK < 4) & (PT > 500*MeV)"},
                                                       CombinationCut = "(AM > 900*MeV) & (AM < 1900*MeV) & (ADOCACHI2CUT(10, ''))",
                                                       MotherCut =    "(VFASPF(VCHI2/VDOF) < 3) & (BPVVDCHI2 > 100.) & (BPVDIRA> 0.99)"),
                            RequiredSelections = [StdLooseKaons,StdLoosePions])
        
        self.Dst = Selection("DstFor"+name,
                             Algorithm=CombineParticles(DecayDescriptors = ["[D*(2010)+ -> D0 pi+]cc"],
                                                        DaughtersCuts = {"pi+":"(PIDe < 5) & (PT > 120*MeV)"},
                                                        CombinationCut = "(AM - ACHILD(1,M) < 220*MeV)",
                                                        MotherCut =    "(VFASPF(VCHI2/VDOF) < 3)"),
                             RequiredSelections = [self.D0,StdAllLoosePions])
        
        self.DstWS = Selection("DstWSFor"+name,
                               Algorithm=CombineParticles(DecayDescriptors = ["[D*(2010)- -> D0 pi-]cc"],
                                                          DaughtersCuts = {"pi+":"(PIDe < 5) & (PT > 120*MeV)"},
                                                          CombinationCut = "(AM - ACHILD(1,M) < 220*MeV)",
                                                          MotherCut =    "(VFASPF(VCHI2/VDOF) < 3)"),
                               RequiredSelections = [self.D0,StdAllLoosePions])
        
        self.B = Selection("BFor"+name,
                           Algorithm=CombineParticles(DecayDescriptors = ["[B~0 -> D*(2010)+ mu-]cc"],
                                                      DaughtersCuts = {"mu-":"(PT > 1200*MeV)"},
                                                      CombinationCut = "(AM > 2500*MeV) & (AM < 5000*MeV)",
                                                      MotherCut =    "(VFASPF(VCHI2/VDOF) < 3) & (BPVVDCHI2 > 25.) & (BPVDIRA> 0.99)"),
                           RequiredSelections = [self.Dst,StdLooseMuons])
        
        self.BWS = Selection("BWSFor"+name,
                             Algorithm=CombineParticles(DecayDescriptors = ["[B~0 -> D*(2010)+ mu-]cc"],
                                                        DaughtersCuts = {"mu-":"(PT > 1200*MeV)"},
                                                        CombinationCut = "(AM > 2500*MeV) & (AM < 5000*MeV)",
                                                        MotherCut =    "(VFASPF(VCHI2/VDOF) < 3) & (BPVVDCHI2 > 25.) & (BPVDIRA> 0.99)"),
                             RequiredSelections = [self.DstWS,StdLooseMuons])
        
        
        self.line =  StrippingLine(name+'Line', 
                                   #HLT = config["HLT_FILTER"],
                                   prescale = 1.0,
                                   selection = self.B) #TOSFilter("TOSB"+name,B,config["TTSpecs"]))

        self.lineWS =  StrippingLine(name+'WSLine', 
                                   prescale = 1.0,
                                   selection = self.BWS) 
        
        self.registerLine(self.line)
        self.registerLine(self.lineWS)


def TOSFilter( name = None, sel = None, trigger = None ):
    if len(trigger) == 0:
        return sel
    from Configurables import TisTosParticleTagger
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = trigger
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel



