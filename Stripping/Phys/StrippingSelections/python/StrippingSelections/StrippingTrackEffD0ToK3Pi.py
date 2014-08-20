__author__ = ['Mika Vesterinen']
__date__ = '23/07/2010'
__version__ = '$Revision: 4.0 $'

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import AutomaticData
from StandardParticles import StdLooseKaons, StdAllNoPIDsPions, StdAllLoosePions, StdLoosePions, StdLooseMuons, StdAllLooseKaons, StdLooseProtons, StdNoPIDsPions, StdNoPIDsUpPions, StdNoPIDsPions
from Configurables import ConjugateNeutralPID, FitDecayTrees
from SelPy.utils import ( UniquelyNamedObject,
                          ClonableObject,
                          SelectionBase )

from Configurables import DecodeVeloRawBuffer, FastVeloTracking, TrackPrepareVelo, NoPIDsParticleMaker, DataOnDemandSvc, ChargedProtoParticleMaker, PrTrackAssociator, DelegatingTrackSelector, TrackContainerCopy, TrackAssociator
from TrackFitter.ConfiguredFitters import ConfiguredFit, ConfiguredFitSeed, ConfiguredMasterFitter

__all__ = ('TrackEffD0ToK3PiAllLinesConf',
           'TOSFilter',
           'default_config')

default_config = {
    "HLT" : "HLT_PASS_RE('Hlt2.*CharmHadD02HHXDst.*Decision')",
    "TTSpecs" : {'Hlt2.*CharmHadD02HHXDst.*Decision%TOS':0},
    "Track_MIN_PT":250,
    "Track_MIN_IPCHI2":250,
    "Track_MAX_GPROB":0.35,
    "Slowpi_MIN_PT":300.,
    "Slowpi_MAX_IPCHI2":9.,
    "Kaon_MIN_PIDK":7,
    "Pion_MAX_PIDK":4
    }

VeloProtoOutputLocation = 'Rec/ProtoP/VeloProtos'

class TrackEffD0ToK3PiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "HLT",
        "TTSpecs",
        "Track_MIN_PT",
        "Track_MIN_IPCHI2",
        "Track_MAX_GPROB",
        "Slowpi_MIN_PT",
        "Slowpi_MAX_IPCHI2",
        "Kaon_MIN_PIDK",
        "Pion_MAX_PIDK"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        
        
        ####################### BASIC FINAL STATE PARTICLE SELECTIONS ##########################

        self.PionCuts = "(PIDK < %(Pion_MAX_PIDK)s) & (TRGHOSTPROB < 0.35)" %self.__confdict__
        self.KaonCuts = "(PIDK > %(Kaon_MIN_PIDK)s) & (TRGHOSTPROB < 0.35)" %self.__confdict__
        
        SelLongPions = Selection("SelLongPionsFor"+name,
                                 Algorithm = FilterDesktop(name="LongPionFilterFor"+name,
                                                           Code=self.PionCuts),
                                 RequiredSelections = [StdLoosePions])
        
        SelLongKaons = Selection("SelLongKaonsFor"+name,
                                 Algorithm = FilterDesktop(name="LongKaonFilterFor"+name,
                                                           Code=self.KaonCuts),
                                 RequiredSelections = [StdLooseKaons])
        
        self.VeloTracks = MakeVeloTracks("MyVeloTracks",[]) 
        self.VeloPions = MakeVeloParticles("VeloPions","pion",self.VeloTracks)
        self.VeloKaons = MakeVeloParticles("VeloKaons","kaon",self.VeloTracks)
        self.VeloProtons = MakeVeloParticles("VeloProtons","proton",self.VeloTracks)
        
        ##################### MAKE THE LINES #############
        
        self.MissingPion4BodyLine = MakeLine("MissingPion4Body",
                                             self.__confdict__,
                                             ['[K*(892)+ -> K- pi+ pi+]cc','[K*(892)+ -> K- pi+ pi-]cc','[K*(892)+ -> K- pi- pi-]cc',
                                              '[K*(892)+ -> K+ pi+ pi+]cc','[K*(892)+ -> K- pi+ pi-]cc','[K*(892)+ -> K- pi- pi-]cc'],
                                             [SelLongKaons,SelLongPions],
                                             ['D0 -> K*(892)+ pi+','D~0 -> K*(892)- pi+'],## both pion charges
                                             ["[D*(2010)+ -> D0 pi+]cc"],
                                             ['[K*_0(1430)0 -> K*(892)+ pi-]cc','[K*_0(1430)0 -> K*(892)+ pi+]cc'],
                                             self.VeloPions)

        self.MissingKaon4BodyLine = MakeLine("MissingKaon4Body",
                                             self.__confdict__,
                                             ['[K*(892)+ -> pi- pi+ pi+]cc','[K*(892)+ -> pi- pi- pi+]cc','[K*(892)+ -> pi- pi- pi-]cc','[K*(892)+ -> pi+ pi+ pi+]cc'],
                                             [SelLongPions],
                                             ['D0 -> K*(892)+ K+','D~0 -> K*(892)- K+'],
                                              ["[D*(2010)+ -> D0 pi+]cc"],
                                             ['[K*_0(1430)0 -> K*(892)+ pi-]cc','[K*_0(1430)0 -> K*(892)+ pi+]cc'],
                                             self.VeloKaons)
        
        self.MissingProtonLine = MakeLine("MissingProton",
                                          self.__confdict__,
                                          ['[K*(892)+ -> K- pi+]cc'],
                                          [SelLongKaons, SelLongPions],
                                          ['Lambda_c+ -> K*(892)+ p+','Lambda_c~- -> K*(892)- p+'],
                                          ['[Sigma_c0 -> Lambda_c+ pi+]cc','[Sigma_c0 -> Lambda_c+ pi-]cc'],
                                          ['[K*_0(1430)0 -> K*(892)+ pi-]cc','[K*_0(1430)0 -> K*(892)+ pi+]cc'],
                                          self.VeloProtons)

        self.registerLine(self.MissingPion4BodyLine[0])
        self.registerLine(self.MissingKaon4BodyLine[0])
        self.registerLine(self.MissingProtonLine[0])
        
        self.registerLine(self.MissingPion4BodyLine[1])
        self.registerLine(self.MissingKaon4BodyLine[1])
        self.registerLine(self.MissingProtonLine[1])
        
        
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
             config,
             KstDecayDescriptors,
             KstRequirements,
             D0DecayDescriptor,
             DstDecayDescriptors,
             MatchDecayDescriptor,
             Probes):
        
    if '4Body' in name:
        D0Preambulo = ["mfit = DTF_FUN(M,True,'D0')"]
        KstCombinationCut = '(APT > 1700*MeV) & (AM > 0 *MeV) & (AM < 1800*MeV)'\
            '& (ANUM(MIPCHI2DV(PRIMARY)>25)>=2)'\
            '& (ANUM(PT > 500*MeV)>=2)'\
            "& (ADOCACHI2CUT(10,''))"
        if "MissingKaon" in name:
            KstCombinationCut += '& (AM > 400*MeV) & (AM < 1400*MeV)'
        KstMotherCut = '(VFASPF(VZ) > 5*mm) & (VFASPF(VCHI2/VDOF)< 3.0)'
        D0CombinationCut = "(AMAXDOCA('') < 0.10 * mm )"
        D0MotherCut ='(abs(DTF_FUN(M,True) - 1865) < 250*MeV)'
        DstMotherCut = "(in_range ( 2 * GeV , mfit , 2.035 * GeV )) & (DTF_CHI2NDOF(True,'D0') < 5)"
    elif 'Proton' in name:
        D0Preambulo = ["mfit = DTF_FUN(M,True,'Lambda_c+')"]
        KstCombinationCut = '(APT > 1500*MeV) & (AM > 600 *MeV) & (AM < 1500*MeV)'\
            '& (ANUM(MIPCHI2DV(PRIMARY)>25)>=2)'\
            '& (ANUM(PT > 500*MeV)>=2)'\
            "& (ADOCACHI2CUT(15,''))"
        KstMotherCut = '(VFASPF(VZ) > 5*mm) & (VFASPF(VCHI2/VDOF)< 4.0)'
        D0CombinationCut = "(AMAXDOCA('') < 0.10 * mm )"
        D0MotherCut ='(abs(DTF_FUN(M,True) - 2286) < 250*MeV)'
        DstMotherCut = "(in_range ( 2.4 * GeV , mfit , 2.5 * GeV )) & (DTF_CHI2NDOF(True,'Lambda_c+') < 5)"
    else:
        print name + ' = Bad line name'
        
    CombKst = CombineParticles(name="CombKstfor"+name,
                               DecayDescriptors = KstDecayDescriptors,
                               CombinationCut = KstCombinationCut,
                               MotherCut = KstMotherCut)
        
    SelKst = Selection('SelKstfor'+name,
                       Algorithm = CombKst,
                       RequiredSelections = KstRequirements)
    
    CombD0 = CombineParticles(name="CombD0For"+name,
                              Preambulo = D0Preambulo,
                              DecayDescriptors = D0DecayDescriptor,
                              CombinationCut = D0CombinationCut,
                              MotherCut = D0MotherCut)
    
    SelD0 = Selection('SelD0for'+name,
                      Algorithm = CombD0,
                      RequiredSelections = [SelKst,Probes])
    
    ####################### ADD THE TAGGING SLOW PION ###################################################
    
    SlowpionCuts = "(MIPCHI2DV(PRIMARY) < 9) & (PIDe < 5) & (PT > 300*MeV)"
    CombDstar = CombineParticles(name = "CombDstarFor"+name,
                                 DecayDescriptors = DstDecayDescriptors,
                                 DaughtersCuts = {"pi+": SlowpionCuts},
                                 CombinationCut = "(AALLSAMEBPV)",
                                 MotherCut = DstMotherCut)
    
    SelDstar = Selection("SelDstarfor"+name,
                          Algorithm = CombDstar,
                         RequiredSelections = [SelD0,StdAllLoosePions])

    ################## MAKE THE STRIPPING LINE ########################
    SelDstarTOS = TOSFilter( "SelDstarTOSfor"+name
                             ,SelDstar
                             ,config["TTSpecs"])
    LineDstar = StrippingLine(name+'DstarLine', 
                              FILTER = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 180 )",
                                         "Preambulo": ["from LoKiTracks.decorators import *"]},
                              HLT = config["HLT"],
                              selection = SelDstarTOS)

    ######### match to the long line
    CombD0MatchLong = CombineParticles(name="CombD0MatchLongFor"+name,
                                       DaughtersCuts = {"pi+":"(ISLONG) & (TRGHOSTPROB < 0.35)"},
                                       DecayDescriptors = MatchDecayDescriptor,
                                       CombinationCut = "(AMAXDOCA('') < 0.10 * mm )",
                                       MotherCut = "ALL")
    
    SelD0MatchLong = Selection('SelD0MatchLongfor'+name,
                               Algorithm = CombD0MatchLong,
                               RequiredSelections = [SelDstarTOS,SelKst,StdAllNoPIDsPions])
        
    LineMatchLong = StrippingLine(name+'MatchLongLine', 
                              FILTER = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 180 )",
                                         "Preambulo": ["from LoKiTracks.decorators import *"]},
                              HLT = config["HLT"],
                              selection = SelD0MatchLong)
    return [LineDstar,LineMatchLong]


def MakeVeloTracks(name, prefilter):
    
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
        
        
