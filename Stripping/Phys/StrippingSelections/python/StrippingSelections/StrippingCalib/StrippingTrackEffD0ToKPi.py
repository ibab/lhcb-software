__author__ = ['Mika Vesterinen']
__date__ = '12/08/2010'
__version__ = '$Revision: 0.0 $'

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons, StdLoosePions, StdLooseProtons,StdAllLoosePions,StdAllNoPIDsPions
from Configurables import ConjugateNeutralPID, FitDecayTrees
from SelPy.utils import ( UniquelyNamedObject,
                          ClonableObject,
                          SelectionBase )
from Configurables import (DecodeVeloRawBuffer, FastVeloTracking, TrackPrepareVelo,
                           NoPIDsParticleMaker, DataOnDemandSvc, ChargedProtoParticleMaker,
                           PrTrackAssociator, DelegatingTrackSelector, TrackContainerCopy, TrackAssociator,
                          TrackStateInitAlg, TrackStateInitTool)
from TrackFitter.ConfiguredFitters import (ConfiguredEventFitter,
                                           ConfiguredForwardStraightLineEventFitter)

__all__ = ('TrackEffD0ToKPiAllLinesConf',
           'TOSFilter',
           'default_config')

default_config = {
        'NAME'        : 'TrackEffD0ToKPi',
        'WGs'         : ['ALL'],
        'BUILDERTYPE' : 'TrackEffD0ToKPiAllLinesConf',
        'STREAMS':["Calibration"],
        'CONFIG'      : {
            "VeloLineForTiming":False,
            "VeloFitter":"SimplifiedGeometry",
            "HLT2" : None, #"HLT_PASS_RE('Hlt2.*Charm.*Decision')",
            "TTSpecs" : {}, #{'Hlt2.*Charm.*Decision%TIS':0},
            "Tag_MIN_PT":1500.,
            "Tag_MIN_IP":0.1,
            "Tag_MAX_GHPROB":0.35,
            "VeloMINIP":0.05,#mm
            "Probe_MIN_IP":0.05,
            "Probe_MIN_ETA":1.9,
            "Probe_MAX_ETA":4.9,
            "Kaon_MIN_PIDK":2,
            "Pion_MAX_PIDK":20,
            "Dst_M_MAX":2030,
            "Dst_DTFCHI2_MAX":3
            }
    }



class TrackEffD0ToKPiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "VeloLineForTiming",
        "VeloFitter",
        "HLT2",
        "TTSpecs",
        "Tag_MIN_PT",
        "Tag_MIN_IP",
        "Tag_MAX_GHPROB",
        "VeloMINIP",
        "Probe_MIN_IP",
        "Probe_MIN_ETA",
        "Probe_MAX_ETA",
        "Kaon_MIN_PIDK",
        "Pion_MAX_PIDK",
        "Dst_M_MAX",
        "Dst_DTFCHI2_MAX"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        
        
        ####################### BASIC FINAL STATE PARTICLE SELECTIONS ##########################
        
        self.PionCuts = "(PIDK < %(Pion_MAX_PIDK)s) & (MIPDV(PRIMARY) > %(Tag_MIN_IP)s*mm) & (PT > %(Tag_MIN_PT)s*MeV) & (TRGHOSTPROB < %(Tag_MAX_GHPROB)s)" %self.__confdict__
        self.KaonCuts = "(PIDK > %(Kaon_MIN_PIDK)s) & (MIPDV(PRIMARY) > %(Tag_MIN_IP)s*mm) & (PT > %(Tag_MIN_PT)s*MeV) & (TRGHOSTPROB < %(Tag_MAX_GHPROB)s)" %self.__confdict__
        
        self.SelLongPions = Selection("SelLongPionsFor"+name,
                                      Algorithm = FilterDesktop(name="LongPionFilterFor"+name,
                                                                Code=self.PionCuts),
                                      RequiredSelections = [StdLoosePions])
        
        self.SelLongKaons = Selection("SelLongKaonsFor"+name,
                                      Algorithm = FilterDesktop(name="LongKaonFilterFor"+name,
                                                                Code=self.KaonCuts),
                                      RequiredSelections = [StdLooseKaons])

        ###### the velo tracking
        self.VeloProtoOutputLocation = 'Rec/ProtoP/VeloProtosFor%s'%self.name()
        self.VeloTrackOutputLocation="Rec/Track/MyVeloFor%s"%self.name()
        self.FittedVeloTrackOutputLocation = "Rec/Track/PreparedVeloFor%s"%self.name()
        
        self.VeloTracks = self.MakeVeloTracks([]) 
        self.VeloPions = self.MakeVeloParticles("VeloPions","pion",self.VeloTracks)
        self.VeloKaons = self.MakeVeloParticles("VeloKaons","kaon",self.VeloTracks)
        
        ##################### MAKE THE LINES #############
        
        self.MissingPion2BodyLine = self.MakeLine(name+"MissingPion2Body",
                                                  ['[D0 -> K- pi+]cc','[D~0 -> K+ pi+]cc'], 
                                                  ["[D*(2010)+ -> D0 pi+]cc"],
                                                  [self.SelLongKaons,self.VeloPions])
        
        self.MissingKaon2BodyLine = self.MakeLine(name+"MissingKaon2Body",
                                                  ['[D0 -> K+ pi+]cc','[D~0 -> K+ pi-]cc'], 
                                                  ["[D*(2010)+ -> D0 pi+]cc"],
                                                  [self.SelLongPions,self.VeloKaons])
                
        if config["VeloLineForTiming"] == True:
            self.registerLine(StrippingLine(name+'SelLongKaonsLine', selection = self.SelLongKaons))
            self.registerLine(StrippingLine(name+'SelLongPionsLine', selection = self.SelLongPions))
            self.registerLine(StrippingLine(name+'VeloPionLine', selection = self.VeloPions))
            self.registerLine(StrippingLine(name+'VeloKaonLine', selection = self.VeloKaons))
        self.registerLine(self.MissingPion2BodyLine)
        self.registerLine(self.MissingKaon2BodyLine)        
        

    def MakeLine(self,name,
                 D0DecayDescriptor,
                 DstDecayDescriptors,
                 D0Requirements):
        
        
        D0CombinationCut = "(AM > 0.1*GeV) & (AM < 3.0*GeV)"
        D0CombinationCut += " & (AMAXDOCA('') < 0.05 * mm)"
        D0CombinationCut += " & (ACHILD(MIPDV(PRIMARY),1)+ACHILD(MIPDV(PRIMARY),2) > 1.0*mm)" 
        D0MotherCut ="(VFASPF(VZ) > 5*mm) & (VFASPF(VCHI2/VDOF)< 3.0) & (abs(DTF_FUN(M,True)-1865) < 200*MeV) & (DTF_CHI2NDOF(True,'D0') < 3.0)"
        SlowpionCuts = "(MIPCHI2DV(PRIMARY) < 9) & (PIDe < 5) & (PT > 200*MeV)"
        DstMotherCut = "(DTF_FUN( M ,True, 'D0') > 2000*MeV) & (DTF_FUN( M ,True, 'D0') <%(Dst_M_MAX)s*MeV) & (DTF_CHI2NDOF(True,'D0') < %(Dst_DTFCHI2_MAX)s)" %self.__confdict__


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
                                 ,self.__confdict__["TTSpecs"])
        LineDstar = StrippingLine(name+'DstarLine', 
                                  FILTER = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 180 )",
                                             "Preambulo": ["from LoKiTracks.decorators import *"]},
                                  HLT2 = self.__confdict__["HLT2"],
                                  selection = SelDstarTOS)
        return LineDstar
        
    def MakeVeloTracks(self,prefilter):
        
        #from DAQSys.Decoders import DecoderDB
        #from DAQSys.DecoderClass import decodersForBank
        #decs=[]
        #vdec=DecoderDB["DecodeVeloRawBuffer/createBothVeloClusters"]
        #vdec.Active=True
        #DecoderDB["DecodeVeloRawBuffer/createVeloClusters"].Active=False
        #DecoderDB["DecodeVeloRawBuffer/createVeloLiteClusters"].Active=False
        #decs=decs+[vdec]
        #VeloDecoding = GaudiSequencer("RecoDecodingSeq")
        #VeloDecoding.Members += [d.setup() for d in decs ]
        
        MyFastVeloTracking = FastVeloTracking("For%sFastVelo"%self.name(),OutputTracksName=self.VeloTrackOutputLocation)
        MyFastVeloTracking.OnlyForward = True
        MyFastVeloTracking.ResetUsedFlags = True
        ### prepare for fitting
        preve = TrackStateInitAlg("For%sInitSeedFit"%self.name(),TrackLocation = self.VeloTrackOutputLocation)
        preve.StateInitTool.VeloFitterName = "FastVeloFitLHCbIDs"
        copyVelo = TrackContainerCopy( "For%sCopyVelo"%self.name() )
        copyVelo.inputLocations = [self.VeloTrackOutputLocation]
        copyVelo.outputLocation = self.FittedVeloTrackOutputLocation
        
        ### fitting
        if self.__confdict__["VeloFitter"] == "ForwardStraightLine":
            MyVeloFit = ConfiguredForwardStraightLineEventFitter(Name="For%sVeloRefitterAlg"%self.name(),
                                                                 TracksInContainer=self.FittedVeloTrackOutputLocation)
        elif self.__confdict__["VeloFitter"] == "SimplifiedGeometry":
            MyVeloFit = ConfiguredEventFitter(Name="For%sVeloRefitterAlg"%self.name(),
                                              TracksInContainer=self.FittedVeloTrackOutputLocation,
                                              SimplifiedGeometry = True)
        else:
            MyVeloFit = ConfiguredEventFitter(Name="For%sVeloRefitterAlg"%self.name(),
                                              TracksInContainer=self.FittedVeloTrackOutputLocation)
            
        #### making the proto particles
        MakeVeloProtos = ChargedProtoParticleMaker('For%sVeloProtoMaker'%self.name())
        MakeVeloProtos.Inputs=[self.FittedVeloTrackOutputLocation]
        MakeVeloProtos.Output = self.VeloProtoOutputLocation
        MakeVeloProtos.addTool( DelegatingTrackSelector, name="TrackSelector" )
        MakeVeloProtos.TrackSelector.TrackTypes = [ "Velo" ]
    
        #### the full sequence
        makeparts = GaudiSequencer('For%sMakeVeloTracksGS'%self.name())
        #makeparts.Members += [ VeloDecoding ] 
        makeparts.Members += [ MyFastVeloTracking ] 
        makeparts.Members += [ preve ] 
        makeparts.Members += [ copyVelo ] 
        makeparts.Members += [ MyVeloFit ] 
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
                                                   Code="(MIPDV(PRIMARY) > %(VeloMINIP)s)" %self.__confdict__),
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
        
        
