__author__ = ['Mika Vesterinen']
__date__ = '09/11/2015'
__version__ = '$Revision: 0.0 $'

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection 
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons, StdLoosePions, StdAllLoosePions,StdAllNoPIDsPions
from SelPy.utils import ( UniquelyNamedObject,
                          ClonableObject,
                          SelectionBase )
from Configurables import (NoPIDsParticleMaker, DataOnDemandSvc, ChargedProtoParticleMaker,
                           DelegatingTrackSelector)

__all__ = ('TrackEffD0ToKPiAllLinesConf',
           'TOSFilter',
           'default_config')

default_config = {
    'NAME'        : 'TrackEffD0ToKPi',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'TrackEffD0ToKPiAllLinesConf',
    'STREAMS':["Calibration"],
    'CONFIG'      : {
        "Monitor":False,
        "HLT1" :"HLT_PASS_RE('Hlt1TrackMVADecision')",
        "HLT2" :"HLT_PASS_RE('Hlt2TrackEff_D0.*Decision')",
        "TTSpecs" :{'Hlt1TrackMVADecision%TOS':0,'Hlt2TrackEff_D0.*Decision%TOS':0},
        "Tag_MIN_PT":1000.,
        "VeloMINIPCHI2":4.0,
        "Kaon_MIN_PIDK":0,
        "Pion_MAX_PIDK":20,
        "Dst_M_MAX":2100,
        "Dst_DTFCHI2_MAX":10
        }
    }

class TrackEffD0ToKPiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "Monitor",
        "HLT1",
        "HLT2",
        "TTSpecs",
        "Tag_MIN_PT",
        "VeloMINIPCHI2",
        "Kaon_MIN_PIDK",
        "Pion_MAX_PIDK",
        "Dst_M_MAX",
        "Dst_DTFCHI2_MAX"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        
        self.TagCommonCuts = "(PT > %(Tag_MIN_PT)s*MeV)" %self.__confdict__

        self.PionCuts = self.TagCommonCuts + "&(PIDK < %(Pion_MAX_PIDK)s)" %self.__confdict__
        
        self.KaonCuts = self.TagCommonCuts + "&(PIDK > %(Kaon_MIN_PIDK)s)" %self.__confdict__
        
        self.SelLongPions = Selection("SelLongPionsFor"+name,
                                      Algorithm = FilterDesktop(name="LongPionFilterFor"+name,
                                                                Code=self.PionCuts),
                                      RequiredSelections = [StdLoosePions])
        
        self.SelLongKaons = Selection("SelLongKaonsFor"+name,
                                      Algorithm = FilterDesktop(name="LongKaonFilterFor"+name,
                                                                Code=self.KaonCuts),
                                      RequiredSelections = [StdLooseKaons])

        self.VeloTrackInputLocaton = 'Rec/Track/FittedHLT1VeloTracks'
        self.VeloProtoOutputLocation = 'Rec/ProtoP/VeloProtosFor%s'%self.name()
        
        self.VeloTracks = self.MakeVeloProtos([]) 
        self.VeloPions = self.MakeVeloParticles("VeloPions","pion",self.VeloTracks)
        self.VeloKaons = self.MakeVeloParticles("VeloKaons","kaon",self.VeloTracks)
        
        
        self.MissingPion2BodyLine = self.MakeLine(name+"_PionProbe",
                                                  ['[D0 -> K- pi+]cc','[D~0 -> K+ pi+]cc'], 
                                                  ["[D*(2010)+ -> D0 pi+]cc"],
                                                  [self.SelLongKaons,self.VeloPions])

        self.MissingKaon2BodyLine = self.MakeLine(name+"_KaonProbe",
                                                  ['[D0 -> K+ pi+]cc','[D~0 -> K+ pi-]cc'], 
                                                  ["[D*(2010)+ -> D0 pi+]cc"],
                                                  [self.SelLongPions,self.VeloKaons])
                
        self.registerLine(self.MissingPion2BodyLine)
        self.registerLine(self.MissingKaon2BodyLine)        

    def MakeLine(self,name,
                 D0DecayDescriptor,
                 DstDecayDescriptors,
                 D0Requirements):
        
        
        D0CombinationCut = "(AM > 0.1*GeV) & (AM < 3.0*GeV)"
        D0CombinationCut += " & (AMAXDOCA('') < 0.05 * mm)"
        D0CombinationCut += " & (ACHILD(MIPDV(PRIMARY),1)+ACHILD(MIPDV(PRIMARY),2) > 0.2*mm)" 
        D0MotherCut  = "(VFASPF(VZ) > 2*mm) & (VFASPF(VCHI2/VDOF) < 4.0)"
        SlowpionCuts = "(MIPCHI2DV(PRIMARY) < 9)" 
        DstMotherCut = "(PT > 1*MeV)" 

        CombD0 = CombineParticles(name="CombD0for"+name,
                                  DecayDescriptors = D0DecayDescriptor,
                                  CombinationCut = D0CombinationCut,
                                  MotherCut = D0MotherCut)
        
        SelD0 = Selection('SelD0for'+name,
                          Algorithm = CombD0,
                          RequiredSelections = D0Requirements)

        CombDstar = CombineParticles(name = "CombDstarFor"+name,
                                     DecayDescriptors = DstDecayDescriptors,
                                     DaughtersCuts = {"pi+": SlowpionCuts},
                                     CombinationCut = "(AALLSAMEBPV)",
                                     MotherCut = DstMotherCut)
    
        SelDstar = Selection("SelDstarfor"+name,
                             Algorithm = CombDstar,
                             RequiredSelections = [SelD0,StdAllLoosePions])
        
        SelDstarTOS = TOSFilter("TOSDstarfor"+name,
                                SelDstar,
                                self.__confdict__["TTSpecs"])
        
        MassFilter =FilterDesktop(name="MassFilter"+name, 
                                  Code="(DTF_CHI2NDOF(True,'D0') < %(Dst_DTFCHI2_MAX)s) & (DTF_FUN( M ,True,'D0') < %(Dst_M_MAX)s)" %self.__confdict__)
     
        MassFilterSel = Selection("MassFilterSel"+name,
                                  Algorithm = MassFilter,
                                  RequiredSelections = [SelDstarTOS])

        if self.__confdict__["Monitor"]:
            MassFilter.Preambulo    = [
                "Histo  = Gaudi.Histo1DDef"  ,
                "mass     = monitor ( DTF_FUN( M ,True,'D0') ,             Histo ( 'Title' , 2000. , 2100. , 100 ) , 'HistoNameInMemory'    ) "
                ]
            MassFilter.Monitor      = True  ,
            MassFilter.HistoProduce = True  ,
            MassFilter.PostMonitor  = """ process ( mass ) >> EMPTY """                                                                                                        
        LineDstar = StrippingLine(name, 
                                  HLT1 = self.__confdict__["HLT1"],
                                  HLT2 = self.__confdict__["HLT2"],
                                  selection = MassFilterSel)
        
        return LineDstar
        
    def MakeVeloProtos(self,prefilter):
                    
        #### making the proto particles
        MakeVeloProtos = ChargedProtoParticleMaker('For%sVeloProtoMaker'%self.name())
        MakeVeloProtos.Inputs = [self.VeloTrackInputLocaton] 
        MakeVeloProtos.Output = self.VeloProtoOutputLocation
        MakeVeloProtos.addTool( DelegatingTrackSelector, name="TrackSelector" )
        MakeVeloProtos.TrackSelector.TrackTypes = [ "Velo" ]
    
        #### the full sequence
        makeparts = GaudiSequencer('For%sMakeVeloProtosGS'%self.name())
        makeparts.Members += [ MakeVeloProtos ] 
    
        #### some python magic to maek this appear like a "Selection"
        return GSWrapper(name="For%sWrappedVeloTrackingFor"%self.name(),
                         sequencer=makeparts,
                         output=self.VeloProtoOutputLocation,
                         requiredSelections =  prefilter)

    def MakeVeloParticles(self,name,
                          particle, 
                          protoParticlesMaker):
        
        particleMaker =  NoPIDsParticleMaker("For%sParticleMaker%s"%(self.name(),name) , Particle = particle)
        particleMaker.Input = self.VeloProtoOutputLocation
        
        DataOnDemandSvc().AlgMap.update( {
                "/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
                "/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName()
                } )
        
        AllVeloParticles = Selection("For%sSelAllVeloParts%s"%(self.name(),name), 
                                     Algorithm = particleMaker, 
                                     RequiredSelections = [protoParticlesMaker], InputDataSetter=None)
        
        ### filter on the IP of the velo tracks
        return Selection("For%sSelVeloParts%s"%(self.name(),name), 
                         Algorithm = FilterDesktop(name+"For%sFilterVeloTrackIP%s"%(self.name(),name), 
                                                   Code="(MIPCHI2DV(PRIMARY) > %(VeloMINIPCHI2)s)" %self.__confdict__),
                         RequiredSelections = [AllVeloParticles])

########## TISTOS FILTERING ##################################
def TOSFilter( name = None, sel = None, trigger = None ):
    if len(trigger) == 0:
        return sel
    from Configurables import TisTosParticleTagger
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = trigger
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel

class GSWrapper(UniquelyNamedObject,
                ClonableObject,
                SelectionBase) :
    
    def __init__(self, name, sequencer, output, requiredSelections) :
        UniquelyNamedObject.__init__(self, name)
        ClonableObject.__init__(self, locals())
        SelectionBase.__init__(self,
                               algorithm = sequencer,
                               outputLocation = output,
                               requiredSelections = requiredSelections )
        
        
