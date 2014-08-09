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
           'confdict')

confdict = {
    }

VeloProtoOutputLocation = 'Rec/ProtoP/VeloProtos'

class TrackEffD0ToK3PiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        
        
        ####################### BASIC FINAL STATE PARTICLE SELECTIONS ##########################

        SelLongPions = Selection("SelLongPionsFor"+name,
                                 Algorithm = FilterDesktop(name="LongPionFilterFor"+name,
                                                           Code="(PIDK < 10) & (TRGHOSTPROB < 0.35)"),
                                 RequiredSelections = [StdLoosePions])
        
        SelLongKaons = Selection("SelLongKaonsFor"+name,
                                 Algorithm = FilterDesktop(name="LongKaonFilterFor"+name,
                                                           Code="(PIDK > 4) & (TRGHOSTPROB < 0.35)"),
                                 RequiredSelections = [StdLooseKaons])
        
        SelLongPionsTight = Selection("SelLongPionsTightFor"+name,
                                      Algorithm = FilterDesktop(name="LongPionFilterTightFor"+name,
                                                                Code="(PT > 1700*MeV) & (PIDK < 2) & (MIPDV(PRIMARY) > 0.5*mm)"),
                                      RequiredSelections = [SelLongPions])
        
        SelLongKaonsTight = Selection("SelLongKaonsTightFor"+name,
                                      Algorithm = FilterDesktop(name="LongKaonFilterTightFor"+name,
                                                                Code="(PT > 1700*MeV) & (PIDK > 8) & (MIPDV(PRIMARY) > 0.5*mm)"),
                                      RequiredSelections = [SelLongKaons])
        
        SelLongPionsTOS = TOSFilter( "SelLongPionsTOSfor"+name
                                     ,SelLongPionsTight
                                     ,{'L0Hadron%TOS' : 0})
        
        SelLongKaonsTOS = TOSFilter( "SelLongKaonsTOSfor"+name
                                     ,SelLongKaonsTight
                                     ,{'L0Hadron%TOS' : 0})
        
        ############### THE VELO TRACKING ##########################################
        #self.prefilter = Selection("OneHighPTTrack"+name,
        #                           Algorithm = FilterDesktop(name="OneHighPTTrackFilterFor"+name,
        #                                                     Code="(PT > 1700*MeV) & ((PIDK > 8) | (PIDK < 2)) & (~ISMUON) & (TRGHOSTPROB < 0.35) & (MIPDV(PRIMARY) > 0.15*mm)")#,
        #RequiredSelections = [StdNoPIDsPions])
                              
        self.VeloTracks = MakeVeloTracks("MyVeloTracks",[]) # [self.prefilter])
        self.VeloPions = MakeVeloParticles("VeloPions","pion",self.VeloTracks)
        self.VeloKaons = MakeVeloParticles("VeloKaons","kaon",self.VeloTracks)
        self.VeloProtons = MakeVeloParticles("VeloProtons","proton",self.VeloTracks)
        
        ##################### MAKE THE LINES #############
        
        self.MissingPion4BodyLine = MakeLine("MissingPion4Body",
                                             '[K*(892)+ -> K- pi+ pi+]cc',
                                             [SelLongKaons,SelLongPions],
                                             ['D0 -> K*(892)+ pi+','D~0 -> K*(892)- pi+'],## both pion charges
                                             ["[D*(2010)+ -> D0 pi+]cc"],
                                             self.VeloPions)

        self.MissingKaon4BodyLine = MakeLine("MissingKaon4Body",
                                             '[K*(892)+ -> pi- pi+ pi+]cc',
                                             [SelLongPions],
                                             ['D0 -> K*(892)+ K+','D~0 -> K*(892)- K+'],
                                             ["[D*(2010)+ -> D0 pi+]cc"],
                                             self.VeloKaons)
        
        self.MissingPion2BodyLine = MakeLine("MissingPion2Body",
                                             'NONE',
                                             SelLongKaonsTight,
                                             ['D0 -> K- pi+','D~0 -> K+ pi+'], 
                                             ["[D*(2010)+ -> D0 pi+]cc"],
                                             self.VeloPions)
        
        self.MissingKaon2BodyLine = MakeLine("MissingKaon2Body",
                                             'NONE',
                                             SelLongPionsTight,
                                             ['D0 -> K+ pi+','D~0 -> K+ pi-'], 
                                             ["[D*(2010)+ -> D0 pi+]cc"],
                                             self.VeloKaons)
        
        self.MissingProtonLine = MakeLine("MissingProton",
                                          '[K*(892)+ -> K- pi+]cc',
                                          [SelLongKaons, SelLongPions],
                                          ['Lambda_c+ -> K*(892)+ p+','Lambda_c~- -> K*(892)- p+'],
                                          ['[Sigma_c++ -> Lambda_c+ pi+]cc','[Sigma_c0 -> Lambda_c+ pi-]cc'],
                                          self.VeloProtons)
        
        ### for testing the speed of the VELO algos
        #self.registerLine(StrippingLine('PreFilterLine', selection = self.prefilter))
        #self.registerLine(StrippingLine('VeloPionLine', selection = self.VeloPions))
        #self.registerLine(StrippingLine('VeloKaonLine', selection = self.VeloKaons))
        #self.registerLine(StrippingLine('VeloProtonLine', selection = self.VeloProtons))
                                        
        
        #CombDstTag = CombineParticles(name="CombDstTagFor"+name,
        #                              DaughtersCuts = {"pi+":"(PT > 250*MeV) & (ETA > 2.5) & (ETA < 4.0)"}, # & (MIPCHI2DV(PRIMARY) < 9)"},
        #                              DecayDescriptor = "[D*(2010)+ -> K- pi+]cc",
        #                              CombinationCut = "(AM > 0.8*GeV)", #AALLSAMEBPV)",
        #                              MotherCut = "ALL",
        #                              ParticleCombiners = { "" : "ParticleAdder" })
        
        #SelDstTag = Selection('SelDstTagfor'+name,
        #                      Algorithm = CombDstTag,
        #                      RequiredSelections = [SelLongKaonsTight,StdAllLoosePions])
        
        #self.registerLine(StrippingLine('StdAllLoosePionsLine', selection = StdAllLoosePions))
        #self.registerLine(StrippingLine('SelLongKaonsTightLine', selection = SelLongKaonsTight))
        #self.registerLine(StrippingLine('SelLongPionsTightLine', selection = SelLongPionsTight))
        #self.registerLine(StrippingLine('SelLongKaonsTOSLine', selection = SelLongKaonsTOS))
        #self.registerLine(StrippingLine('SelLongPionsTOSLine', selection = SelLongPionsTOS))
        #self.registerLine(StrippingLine('VeloDstTagLine', selection = SelDstTag))
        
        
        self.registerLine(self.MissingPion4BodyLine)
        self.registerLine(self.MissingKaon4BodyLine)
        self.registerLine(self.MissingPion2BodyLine)
        self.registerLine(self.MissingKaon2BodyLine)
        self.registerLine(self.MissingProtonLine)
        
        
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
             KstDecayDescriptor,
             KstRequirements,
             D0DecayDescriptor,
             DstDecayDescriptors,
             Probes):
        
    if '2Body' in name:
        D0CombinationCut = "(AM > 0.8*GeV) & (AM < 2.3*GeV)"
        D0CombinationCut += " & (AMAXDOCA('') < 0.05 * mm)"
        #D0CombinationCut += " & (AHASCHILD((ISLONG) & (PT>1400*MeV) & (MIPDV(PRIMARY) > 0.15*mm)))"
        D0CombinationCut += " & (ACHILD(MIPDV(PRIMARY),1)+ACHILD(MIPDV(PRIMARY),2) > 1.0*mm)" # & (AMAXDOCA('') < 0.03 * mm)"
        D0CombinationCut += " & ( (ACHILDCUT(ISLONG,1)) | (ACHILDCUT(MIPCHI2DV(PRIMARY) > 25,1)) )"
        D0CombinationCut += " & ( (ACHILDCUT(ISLONG,2)) | (ACHILDCUT(MIPCHI2DV(PRIMARY) > 25,2)) )"
        D0MotherCut ="(VFASPF(VZ) > 10*mm) & (VFASPF(VCHI2/VDOF)< 3.0) & (abs(DTF_FUN(M,True)-1865) < 200*MeV) & (DTF_CHI2NDOF(True,'D0') < 2.5)"
        DstMotherCut = "(DTF_FUN( M ,True, 'D0') < 2030*MeV) & (DTF_CHI2NDOF(True,'D0') < 3)"
    elif '4Body' in name:
        KstCombinationCut = '(APT > 1500*MeV) & (AM > 0 *MeV) & (AM < 1900*MeV)'\
            '& (ANUM(MIPCHI2DV(PRIMARY)>25)>=2)'\
            '& (ANUM(PT > 500*MeV)>=2)'\
            "& (ADOCACHI2CUT(15,''))"
        KstMotherCut = '(VFASPF(VZ) > 3*mm) & (VFASPF(VCHI2/VDOF)< 4.0)'
        D0CombinationCut = "(AMAXDOCA('') < 0.10 * mm )"
        #D0CombinationCut += " & (AHASCHILD((!ISLONG) & (MIPCHI2DV(PRIMARY) > 25))"
        D0MotherCut ='(DTF_FUN(M,True) > 1565*MeV) & (DTF_FUN(M,True) < 2165*MeV)'
        DstMotherCut = "(DTF_FUN( M ,True, 'D0') < 2050*MeV) & (DTF_CHI2NDOF(True,'D0') < 10)"
    elif 'Proton' in name:
        KstCombinationCut = '(APT > 1500*MeV) & (AM > 600 *MeV) & (AM < 1300*MeV)'\
            '& (ANUM(MIPCHI2DV(PRIMARY)>25)>=2)'\
            '& (ANUM(PT > 500*MeV)>=2)'\
            "& (ADOCACHI2CUT(15,''))"
        KstMotherCut = '(VFASPF(VZ) > 3*mm) & (VFASPF(VCHI2/VDOF)< 4.0)'
        D0CombinationCut = "(AMAXDOCA('') < 0.10 * mm )"
        D0MotherCut ='(DTF_FUN(M,True) > 1886*MeV) & (DTF_FUN(M,True) < 2686*MeV)'
        DstMotherCut = "(DTF_FUN( M ,True, 'Lambda_c+') < 2550*MeV) & (DTF_CHI2NDOF(True,'Lambda_c+') < 10)"
    else:
        print name + ' = Bad line name'
        
    if "2Body" in name:
        CombD0 = CombineParticles(name="CombD0for"+name,
                                  DecayDescriptors = D0DecayDescriptor,
                                  CombinationCut = D0CombinationCut,
                                  MotherCut = D0MotherCut)
        
        SelD0 = Selection('SelD0for'+name,
                          Algorithm = CombD0,
                          RequiredSelections = [KstRequirements,Probes])
    else:
        CombKst = CombineParticles(name="CombKstfor"+name,
                                   DecayDescriptor = KstDecayDescriptor,
                                   CombinationCut = KstCombinationCut,
                                   MotherCut = KstMotherCut)
        
        SelKst = Selection('SelKstfor'+name,
                           Algorithm = CombKst,
                           RequiredSelections = KstRequirements)

        CombD0 = CombineParticles(name="CombD0For"+name,
                                  DecayDescriptors = D0DecayDescriptor,
                                  CombinationCut = D0CombinationCut,
                                  MotherCut = D0MotherCut)
        
        SelD0 = Selection('SelD0for'+name,
                          Algorithm = CombD0,
                          RequiredSelections = [SelKst,Probes])
        
    ####################### ADD THE TAGGING SLOW PION ###################################################
    
    SlowpionCuts = "(MIPCHI2DV(PRIMARY) < 9) & (PIDe < 5) & (PT > 300*MeV)"
    if "2Body" in name:
        SlowpionCuts = "(MIPCHI2DV(PRIMARY) < 9) & (PIDe < 5) & (PT > 200*MeV)"
    CombDstar = CombineParticles(name = "CombDstarFor"+name,
                                 DecayDescriptors = DstDecayDescriptors,
                                 DaughtersCuts = {"pi+": SlowpionCuts},
                                 CombinationCut = "(AALLSAMEBPV)",
                                 MotherCut = DstMotherCut)
    
    SelDstar = Selection("SelDstarfor"+name,
                          Algorithm = CombDstar,
                         RequiredSelections = [SelD0,StdAllLoosePions])

    ################## MAKE THE STRIPPING LINE ########################
    HLT = "HLT_PASS_RE('Hlt2.*CharmHadD02HHXDst_hhX.*Decision')"
    TTSpecs = {'Hlt2.*CharmHadD02HHXDst_hhX.*Decision%TOS':0}
    if '2Body' in name:
        HLT = "HLT_PASS_RE('Hlt2.*Charm.*Decision')"
        #HLT = None
        TTSpecs = {}

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
        
        
