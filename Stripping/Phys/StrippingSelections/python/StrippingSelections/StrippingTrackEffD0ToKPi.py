__author__ = ['Mika Vesterinen']
__date__ = '12/08/2010'
__version__ = '$Revision: 0.0 $'

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons, StdAllNoPIDsPions, StdAllLoosePions, StdLoosePions, StdLooseMuons, StdAllLooseKaons, StdLooseProtons, StdNoPIDsPions, StdNoPIDsUpPions, StdNoPIDsPions, StdNoPIDsKaons
from SelPy.utils import ( UniquelyNamedObject,
                          ClonableObject,
                          SelectionBase )

## for the fast velo tracking
from Configurables import DecodeVeloRawBuffer, FastVeloTracking, TrackPrepareVelo, NoPIDsParticleMaker, DataOnDemandSvc, ChargedProtoParticleMaker, DelegatingTrackSelector, TrackContainerCopy
from TrackFitter.ConfiguredFitters import ConfiguredFit, ConfiguredFitSeed, ConfiguredMasterFitter

__all__ = ('TrackEffD0ToKPiAllLinesConf',
           'TOSFilter',
           'default_config')

default_config = {
    "debug":False,
    "Tag_MIN_PT":1200.,
    "Tag_MIN_IP":0.1,
    "Tag_MAX_GHPROB":0.35,
    "Probe_MIN_IP":0.1,
    "Probe_MIN_ETA":1.9,
    "Probe_MAX_ETA":4.9,
    "Kaon_MIN_PIDK":8,
    "Pion_MAX_PIDK":2
    }

class TrackEffD0ToKPiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "debug",
        "Tag_MIN_PT",
        "Tag_MIN_IP",
        "Tag_MAX_GHPROB",
        "Probe_MIN_IP",
        "Probe_MIN_ETA",
        "Probe_MAX_ETA",
        "Kaon_MIN_PIDK",
        "Pion_MAX_PIDK"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        
        
        ####################### BASIC FINAL STATE PARTICLE SELECTIONS ##########################
        
        self.PionCuts = "(PIDK < %(Pion_MAX_PIDK)s) & (MIPDV(PRIMARY) > %(Tag_MIN_IP)s*mm) & (PT > %(Tag_MIN_PT)s*MeV) & (TRGHOSTPROB < %(Tag_MAX_GHPROB)s)" %self.__confdict__
        self.KaonCuts = "(PIDK > %(Kaon_MIN_PIDK)s) & (MIPDV(PRIMARY) > %(Tag_MIN_IP)s*mm) & (PT > %(Tag_MIN_PT)s*MeV) & (TRGHOSTPROB < %(Tag_MAX_GHPROB)s)" %self.__confdict__
        self.ProbeCuts = "(MIPDV(PRIMARY) > %(Probe_MIN_IP)s*mm) & (ETA > %(Probe_MIN_ETA)s) & (ETA < %(Probe_MAX_ETA)s)" %self.__confdict__
        
        self.SelLongPions = Selection("SelLongPionsFor"+name,
                                      Algorithm = FilterDesktop(name="LongPionFilterFor"+name,
                                                                Code=self.PionCuts),
                                      RequiredSelections = [StdNoPIDsPions])
        
        self.SelLongKaons = Selection("SelLongKaonsFor"+name,
                                      Algorithm = FilterDesktop(name="LongKaonFilterFor"+name,
                                                                Code=self.KaonCuts),
                                      RequiredSelections = [StdNoPIDsKaons])

        self.VeloTracks = MakeVeloTracks("MyVeloTracks",[]) 
        self.VeloPions = MakeVeloParticles("VeloPions","pion",self.VeloTracks)
        self.VeloKaons = MakeVeloParticles("VeloKaons","kaon",self.VeloTracks)
        self.VeloProtons = MakeVeloParticles("VeloProtons","proton",self.VeloTracks)
        
        self.FilteredVeloPions = Selection("SelFilteredVeloPionsFor"+name,
                                           Algorithm = FilterDesktop(name="VeloPionFilterFor"+name,
                                                                        Code=self.ProbeCuts),
                                           RequiredSelections = [self.VeloPions])
        
        self.FilteredVeloKaons = Selection("SelFilteredVeloKaonsFor"+name,
                                           Algorithm = FilterDesktop(name="VeloPionFilterFor"+name,
                                                                     Code=self.ProbeCuts),
                                           RequiredSelections = [self.VeloKaons])
        
        ##################### MAKE THE LINES #############
        
        self.MissingPion2BodyLine = MakeLine("MissingPion2Body",
                                             ['D0 -> K- pi+','D~0 -> K+ pi+'], 
                                             ["[D*(2010)+ -> D0 pi+]cc"],
                                             [self.SelLongKaons,self.FilteredVeloPions])
        
        self.MissingKaon2BodyLine = MakeLine("MissingKaon2Body",
                                             ['D0 -> K+ pi+','D~0 -> K+ pi-'], 
                                             ["[D*(2010)+ -> D0 pi+]cc"],
                                             [self.SelLongPions,self.FilteredVeloKaons])
                
        if config["debug"] == True:
            self.registerLine(StrippingLine('SelLongKaonsLine', selection = self.SelLongKaons))
            self.registerLine(StrippingLine('SelLongPionsLine', selection = self.SelLongPions))
            self.registerLine(StrippingLine('VeloPionLine', selection = self.FilteredVeloPions))
            self.registerLine(StrippingLine('VeloKaonLine', selection = self.FilteredVeloKaons))
        self.registerLine(self.MissingPion2BodyLine)
        self.registerLine(self.MissingKaon2BodyLine)
        
        
########## TISTOS FILTERING ##################################
def TOSFilter( name = None, sel = None, trigger = None ):
    if len(trigger) == 0:
        return sel
    from Configurables import TisTosParticleTagger
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = trigger
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel

####################### MAKE THE INTERMEDIATE THREE TRACK VERTEX ##########################
def MakeLine(name,
             D0DecayDescriptor,
             DstDecayDescriptors,
             D0Requirements):
    
    ######## possible filtering
    HLT = None 
    TTSpecs = {}

    ######## the pre-vertex fit D0 -> K- pi+ combination cuts
    D0CombinationCut = "(AM > 0.8*GeV) & (AM < 2.3*GeV)"
    D0CombinationCut += " & (ACHILD(MIPDV(PRIMARY),1)+ACHILD(MIPDV(PRIMARY),2) > 2.0*mm)" 
    D0CombinationCut += " & (AMAXDOCA('') < 0.05 * mm)"
    #D0CombinationCut += " & ( (ACHILDCUT(ISLONG,1)) | (ACHILDCUT(MIPCHI2DV(PRIMARY) > 25,1)) )"
    #D0CombinationCut += " & ( (ACHILDCUT(ISLONG,2)) | (ACHILDCUT(MIPCHI2DV(PRIMARY) > 25,2)) )"
    
    ######## the post-vertex-fit  D0 -> K- pi+ combination cuts
    D0MotherCut ="(VFASPF(VZ) > 10*mm) & (VFASPF(VCHI2/VDOF)< 3.0) & (abs(DTF_FUN(M,True)-1865) < 200*MeV) & (DTF_CHI2NDOF(True,'D0') < 2.5)"
    
    ######## the post-vertex-fit D* -> D0 pi combination cuts
    SlowpionCuts = "(MIPCHI2DV(PRIMARY) < 9) & (PIDe < 5) & (PT > 200*MeV)"
    DstMotherCut = "(DTF_FUN( M ,True, 'D0') < 2030*MeV) & (DTF_CHI2NDOF(True,'D0') < 3)"

    ######## the combinatorics sequence
    
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

    
    SelDstarTOS = TOSFilter( "SelDstarTOSfor"+name
                             ,SelDstar
                             ,TTSpecs)
    LineDstar = StrippingLine(name+'DstarLine', 
                              FILTER = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 180 )",
                                         "Preambulo": ["from LoKiTracks.decorators import *"]},
                              HLT = HLT,
                              selection = SelDstarTOS)
    return LineDstar


def MakeVeloTracks(name, prefilter):
    VeloProtoOutputLocation = 'Rec/ProtoP/VeloProtos'
    
    #### the fast velo tracking
    preve = TrackPrepareVelo("PrepareVeloFor"+name)
    preve.inputLocation = "Rec/Track/Velo"
    preve.outputLocation = "Rec/Track/UnFittedVelo"
    preve.bestLocation = ""
    VeloDecoding = DecodeVeloRawBuffer("VeloDecodingFor"+name,DecodeToVeloLiteClusters=True,DecodeToVeloClusters=True)
    MyFastVeloTracking = FastVeloTracking("FastVeloFor"+name,OutputTracksName="Rec/Track/Velo")
    MyFastVeloTracking.OnlyForward = True
    
    #### fitting
    copyVelo = TrackContainerCopy( "CopyVeloFor"+name )
    copyVelo.inputLocation = "Rec/Track/UnFittedVelo";
    copyVelo.outputLocation = "Rec/Track/PreparedVelo";
    MyVeloFit = ConfiguredFit("FitVeloFor"+name,"Rec/Track/PreparedVelo")
    
    #### making the proto particles
    VeloProtos = ChargedProtoParticleMaker('VeloProtoMakerFor'+name)
    VeloProtos.Inputs=['Rec/Track/PreparedVelo']
    VeloProtos.Output = VeloProtoOutputLocation
    VeloProtos.addTool( DelegatingTrackSelector, name="TrackSelector" )
    VeloProtos.TrackSelector.TrackTypes = [ "Velo" ]
    
    #### the full sequence
    makeparts = GaudiSequencer('MakeVeloTracksGS'+name)
    makeparts.Members += [ VeloDecoding,MyFastVeloTracking,preve]
    makeparts.Members += [ copyVelo , MyVeloFit ]
    makeparts.Members += [VeloProtos] 
    
    #### some python magic to maek this appear like a "Selection"
    return GSWrapper(name="WrappedVeloTrackingFor"+name,
                     sequencer=makeparts,
                     output=VeloProtoOutputLocation,
                     requiredSelections =  prefilter)

def MakeVeloParticles(name,particle, protoParticlesMaker):
    VeloProtoOutputLocation = 'Rec/ProtoP/VeloProtos'
    particleMaker =  NoPIDsParticleMaker(name+"ParticleMaker" , Particle = particle)
    particleMaker.Input = VeloProtoOutputLocation
    
    DataOnDemandSvc().AlgMap.update( {
            "/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
            "/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName()
            } )
    
    return Selection(name+"SelVeloMuonParts", Algorithm = particleMaker, RequiredSelections = [protoParticlesMaker], InputDataSetter=None)
    

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
        
        
