__author__ = ['Mika Vesterinen']
__date__ = '23/07/2014'
__version__ = '$Revision: 5.0 $'

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons, StdLoosePions, StdLooseProtons,StdAllLoosePions,StdAllNoPIDsPions
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from SelPy.utils import ( UniquelyNamedObject,
                          ClonableObject,
                          SelectionBase )
from Configurables import (DecodeVeloRawBuffer, FastVeloTracking, TrackPrepareVelo, 
                           NoPIDsParticleMaker, DataOnDemandSvc, ChargedProtoParticleMaker, 
                           PrTrackAssociator, DelegatingTrackSelector, TrackContainerCopy, TrackAssociator,
                          TrackStateInitAlg, TrackStateInitTool) 
from TrackFitter.ConfiguredFitters import (ConfiguredEventFitter,
                                           ConfiguredForwardStraightLineEventFitter)
    

__all__ = ('TrackEffD0ToK3PiAllLinesConf',
           'TOSFilter',
           'default_config')

default_config = {
  'TrackEffD0ToK3Pi' : {
    "WGs": ["Semileptonic"],
    "BUILDERTYPE": "TrackEffD0ToK3PiAllLinesConf",
    "CONFIG":{
      "HLT" : "HLT_PASS_RE('Hlt2.*CharmHadD02HHXDst.*Decision')",
      "TTSpecs" : {'Hlt2.*CharmHadD02HHXDst.*Decision%TOS':0},
      "VeloLineForTiming":False,
      "VeloFitter":"SimplifiedGeometry",
      "DoVeloDecoding": False,
      "RequireDstFirst":False,
      "Dst_MAX_M":2.035*GeV,
      "Dst_MAX_DTFCHI2":3.0,
      "Sc_MAX_M":2.5*GeV,
      "Sc_MAX_DTFCHI2":3.0,
      "D0_MIN_FD":5.0*mm,
      "LC_MIN_FD":2.0*mm,
      "D0_MAX_DOCA":0.05*mm,
      "VeloMINIP":0.05*mm,
      "Kaon_MIN_PIDK":7,
      "Pion_MAX_PIDK":4
      },
     'STREAMS'     : ['Semileptonic']	  
    }
  }

class TrackEffD0ToK3PiAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "HLT",
        "TTSpecs",
        "VeloLineForTiming",
        "VeloFitter",
        "DoVeloDecoding",
        "RequireDstFirst",
        "Dst_MAX_M",
        "Dst_MAX_DTFCHI2",
        "Sc_MAX_M",
        "Sc_MAX_DTFCHI2",
        "D0_MIN_FD",
        "LC_MIN_FD",
        "D0_MAX_DOCA",
        "VeloMINIP",
        "Kaon_MIN_PIDK",
        "Pion_MAX_PIDK"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        self.name = name
        

        ##### long track kaons and pions
        self.PionCuts = "(PIDK < %(Pion_MAX_PIDK)s) & (TRGHOSTPROB < 0.35)" %config
        self.KaonCuts = "(PIDK > %(Kaon_MIN_PIDK)s) & (TRGHOSTPROB < 0.35)" %config
        
        SelLongPions = Selection("SelLongPionsFor"+name,
                                 Algorithm = FilterDesktop(name="LongPionFilterFor"+name,
                                                           Code=self.PionCuts),
                                 RequiredSelections = [StdLoosePions])
        
        SelLongKaons = Selection("SelLongKaonsFor"+name,
                                 Algorithm = FilterDesktop(name="LongKaonFilterFor"+name,
                                                           Code=self.KaonCuts),
                                 RequiredSelections = [StdLooseKaons])
        
        ###### the velo tracking
        self.VeloProtoOutputLocation = 'Rec/ProtoP/VeloProtosFor%s'%self.name
        self.VeloTrackOutputLocation="Rec/Track/MyVeloFor%s"%self.name
        self.FittedVeloTrackOutputLocation = "Rec/Track/PreparedVeloFor%s"%self.name
        
        self.VeloTracks = self.MakeVeloTracks([]) 
        self.VeloPions = self.MakeVeloParticles("VeloPions","pion",self.VeloTracks)
        self.VeloKaons = self.MakeVeloParticles("VeloKaons","kaon",self.VeloTracks)
        self.VeloProtons = self.MakeVeloParticles("VeloProtons","proton",self.VeloTracks)
        
        ##################### MAKE THE LINES #############
        
        self.MissingPion4BodyLine = self.MakeLine("%sMissingPion4Body"%self.name,
                                                  ['[K*(892)+ -> K- pi+ pi+]cc','[K*(892)+ -> K- pi+ pi-]cc','[K*(892)+ -> K- pi- pi-]cc'],## RS-A, RS-B , SS
                                                  [SelLongKaons,SelLongPions],
                                                  ['[D0 -> K*(892)+ pi+]cc','[D0 -> K*(892)+ pi-]cc'], ## allow both probe charges
                                                  ["[D*(2010)+ -> D0 pi+]cc","[D*(2010)+ -> D~0 pi+]cc"], ## add also wrong sign
                                                  ['[K*_0(1430)0 -> K*(892)+ pi-]cc','[K*_0(1430)0 -> K*(892)+ pi+]cc'], ## allow also wrong sign StdAllNoPIDs "pions"
                                                  self.VeloPions)

        self.MissingKaon4BodyLine = self.MakeLine("%sMissingKaon4Body"%self.name,
                                                  ['[K*(892)+ -> pi+ pi+ pi-]cc','[K*(892)+ -> pi+ pi+ pi+]cc'],## RS, SS
                                                  [SelLongPions],
                                                  ['[D0 -> K*(892)+ K+]cc','[D0 -> K*(892)+ K-]cc'], ## allow both probe charges
                                                  ["[D*(2010)+ -> D0 pi+]cc","[D*(2010)+ -> D~0 pi+]cc"], ## add also wrong sign
                                                  ['[K*_0(1430)0 -> K*(892)+ pi-]cc','[K*_0(1430)0 -> K*(892)+ pi+]cc'], ## allow also wrong sign StdAllNoPIDs "pions"
                                                  self.VeloKaons)
        
        self.MissingProtonLine = self.MakeLine("%sMissingProton"%self.name,
                                               ['[K*(892)+ -> K- pi+]cc','[K*(892)+ -> K- pi-]cc'], ## RS, SS
                                               [SelLongKaons, SelLongPions],
                                               ['[Lambda_c+ -> K*(892)+ p+]cc','[Lambda_c+ -> K*(892)+ p~-]cc'],## both probe charges
                                               ['[Sigma_c0 -> Lambda_c+ pi+]cc','[Sigma_c0 -> Lambda_c+ pi-]cc'], ## Allows Sigma_c0 and Sigma_c++
                                               ['[K*_0(1430)0 -> K*(892)+ pi-]cc','[K*_0(1430)0 -> K*(892)+ pi+]cc'], ## allow also wrong sign StdAllNoPIDs "pions"
                                               self.VeloProtons)

        if config["VeloLineForTiming"] == True:
            self.registerLine(StrippingLine("VeloLine"+name,selection=self.VeloPions))
            
        ### these are the partially reconstructed signals
        self.registerLine(self.MissingKaon4BodyLine[0])
        self.registerLine(self.MissingPion4BodyLine[0])
        self.registerLine(self.MissingProtonLine[0])
        
        ### and these contain the candidate matching long tracks
        self.registerLine(self.MissingPion4BodyLine[1])
        self.registerLine(self.MissingKaon4BodyLine[1])
        self.registerLine(self.MissingProtonLine[1])

    
    #### function to make the stripping line
    def MakeLine(self,
                 name,
                 KstDecayDescriptors,
                 KstRequirements,
                 D0DecayDescriptor,
                 DstDecayDescriptors,
                 MatchDecayDescriptor,
                 Probes):
        
        #########################################################
        ## DEFINE ALL OF THE CUTS
        #########################################################
        
        MatchingLongTrackCuts = "(ISLONG) & (TRGHOSTPROB < 0.35)"
        D0CombinationCut = "(AMAXDOCA('') < %(D0_MAX_DOCA)s )" %self.__confdict__
        SlowpionCuts = "(MIPCHI2DV(PRIMARY) < 9) & (PIDe < 5) & (PT > 300*MeV)"
        #DstPrelimComboCuts = "(AM - ACHILD(M,1) < 225*MeV) & (AALLSAMEBPV)"
        DstPrelimComboCuts = "(APT > 3500*MeV) & (AALLSAMEBPV)"
        if '4Body' in name:
            D0Preambulo = ["mfit = DTF_FUN(M,True,'D0')"]
            KstCombinationCut = "(APT > 2000*MeV) & (AM < 1800*MeV)"\
                "& (AM - ACHILD(M,1) - ACHILD(M,2) - ACHILD(M,3) > 100*MeV)"\
                "& (ANUM(PT > 500*MeV)>=2)"\
                "& (AMAXCHILD(MIPCHI2DV(PRIMARY)) > 30)"\
                "& (ADOCACHI2CUT(10,''))"
            if "MissingKaon" in name:
                KstCombinationCut += '& (AM < 1400*MeV)'
            KstMotherCut = '(VFASPF(VZ) > %(D0_MIN_FD)s) & (BPVDIRA > 0.99) & (VFASPF(VCHI2/VDOF)< 3.0)' %self.__confdict__
            D0MotherCut ='(abs(DTF_FUN(M,True) - 1865) < 250*MeV)'
            DstMotherCut = "(in_range ( 2 * GeV , mfit , %(Dst_MAX_M)s )) & (DTF_CHI2NDOF(True,'D0') < %(Dst_MAX_DTFCHI2)s)" %self.__confdict__
        elif 'Proton' in name:
            D0Preambulo = ["mfit = DTF_FUN(M,True,'Lambda_c+')"]
            KstCombinationCut = '(APT > 1500*MeV) & (AM < 1500*MeV)'\
                '& (AM - ACHILD(M,1) - ACHILD(M,2) > 100*MeV)'\
                '& (ANUM(PT > 500*MeV)>=2)'\
                "& (AMAXCHILD(MIPCHI2DV(PRIMARY)) > 30)"\
                "& (ADOCACHI2CUT(15,''))"
            KstMotherCut = '(VFASPF(VZ) > %(LC_MIN_FD)s) & (BPVDIRA > 0.99) & (VFASPF(VCHI2/VDOF)< 3.0)' %self.__confdict__
            D0MotherCut ='(abs(DTF_FUN(M,True) - 2286) < 250*MeV)'
            DstMotherCut = "(in_range ( 2.4 * GeV , mfit , %(Sc_MAX_M)s )) & (DTF_CHI2NDOF(True,'Lambda_c+') < %(Sc_MAX_DTFCHI2)s)" %self.__confdict__
        else:
            print name + ' = Bad line name'
        
        #########################################################
        ## STEP 1: 
        ## make the partially reconstructed D0/Lamda_c candidate
        #########################################################
            
        CombKst = CombineParticles(name="For%sCombKst"%name,
                                   DecayDescriptors = KstDecayDescriptors,
                                   CombinationCut = KstCombinationCut,
                                   MotherCut = KstMotherCut)
        
        SelKst = Selection('For%sSelKst'%name,
                           Algorithm = CombKst,
                           RequiredSelections = KstRequirements)
    
        #########################################################
        ## STEP 2 (OPTIONAL): 
        ## Does this make an "inclusive D*" candidate?
        #########################################################
        if self.__confdict__["RequireDstFirst"]:

            CombDstarPrelim = CombineParticles(name = "For%sCombDstarPrelimFor"%name,
                                               DecayDescriptors = ["[D*(2010)+ -> K*(892)+ pi+]cc"],
                                               DaughtersCuts = {"pi+": SlowpionCuts},
                                               CombinationCut = DstPrelimComboCuts,
                                               MotherCut = "ALL")
            SelDstarPrelim = Selection("For%sSelDstarPrelimfor"%name,
                                       Algorithm = CombDstarPrelim,
                                       RequiredSelections = [SelKst,StdAllLoosePions])
        
            SelDstarPrelimTOS = TOSFilter( "For%sSelDstarPrelimTOS"%name
                                           ,SelDstarPrelim
                                           ,self.__confdict__["TTSpecs"])

        #########################################################
        ## STEP 3: 
        ## Add the probe velo track to the D0
        #########################################################
    
        CombD0 = CombineParticles(name="For%sCombD0"%name,
                                  Preambulo = D0Preambulo,
                                  DecayDescriptors = D0DecayDescriptor,
                                  CombinationCut = D0CombinationCut,
                                  MotherCut = D0MotherCut)
    
        if self.__confdict__["RequireDstFirst"]:
            SelD0 = Selection('For%sSelD0'%name,
                              Algorithm = CombD0,
                              RequiredSelections = [SelDstarPrelimTOS,SelKst,Probes])
        else:
            SelD0 = Selection('For%sSelD0'%name,
                              Algorithm = CombD0,
                              RequiredSelections = [SelKst,Probes])
            
        #########################################################
        ## STEP 4: 
        ## Build a D* candidate
        #########################################################
            
        

        CombDstar = CombineParticles(name = "For%sCombDstar"%name,
                                     DecayDescriptors = DstDecayDescriptors,
                                     DaughtersCuts = {"pi+": SlowpionCuts},
                                     CombinationCut = DstPrelimComboCuts,
                                     MotherCut = DstMotherCut)
        
        SelDstar = Selection("For%sSelDstarf"%name,
                             Algorithm = CombDstar,
                             RequiredSelections = [SelD0,StdAllLoosePions])
        
        SelDstarTOS = TOSFilter( "For%sSelDstarTOS"%name
                                 ,SelDstar
                                 ,self.__confdict__["TTSpecs"])
        
        
        #########################################################
        ## STEP 5: 
        ## Find any interesting matching long tracks
        #########################################################

        CombD0MatchLong = CombineParticles(name="For%sCombD0MatchLong"%name,
                                           DaughtersCuts = {"pi+":MatchingLongTrackCuts},
                                           DecayDescriptors = MatchDecayDescriptor,
                                           CombinationCut = "(AMAXDOCA('') < 0.10 * mm )",
                                           MotherCut = "ALL")
        
        SelD0MatchLong = Selection('For%sSelD0MatchLong'%name,
                                   Algorithm = CombD0MatchLong,
                                   RequiredSelections = [SelDstarTOS,SelKst,StdAllNoPIDsPions])

        #########################################################
        ## STEP 6: 
        ## Make the stripping lines
        #########################################################
        
        LineDstar = StrippingLine(name, 
                                  FILTER = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 180 )",
                                             "Preambulo": ["from LoKiTracks.decorators import *"]},
                                  HLT2 = self.__confdict__["HLT"],
                                  selection = SelDstarTOS) 
        
        LineMatchLong = StrippingLine(name+'MatchLong', 
                                      FILTER = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 180 )",
                                                 "Preambulo": ["from LoKiTracks.decorators import *"]},
                                      HLT2 = self.__confdict__["HLT"],
                                      selection = SelD0MatchLong)
        return [LineDstar,LineMatchLong]


    def MakeVeloTracks(self,prefilter):
        
        if self.__confdict__["DoVeloDecoding"]:
            from DAQSys.Decoders import DecoderDB
            from DAQSys.DecoderClass import decodersForBank
            decs=[]
            vdec=DecoderDB["DecodeVeloRawBuffer/createBothVeloClusters"]
            vdec.Active=True
            DecoderDB["DecodeVeloRawBuffer/createVeloClusters"].Active=False
            DecoderDB["DecodeVeloRawBuffer/createVeloLiteClusters"].Active=False
            decs=decs+[vdec]
            VeloDecoding = GaudiSequencer("RecoDecodingSeq")
            VeloDecoding.Members += [d.setup() for d in decs ]
        
        MyFastVeloTracking = FastVeloTracking("For%sFastVelo"%self.name,OutputTracksName=self.VeloTrackOutputLocation)
        MyFastVeloTracking.OnlyForward = True
        MyFastVeloTracking.ResetUsedFlags = True
        ### prepare for fitting
        preve = TrackStateInitAlg("For%sInitSeedFit"%self.name,TrackLocation = self.VeloTrackOutputLocation)
        preve.StateInitTool.VeloFitterName = "FastVeloFitLHCbIDs"
        copyVelo = TrackContainerCopy( "For%sCopyVelo"%self.name )
        copyVelo.inputLocations = [self.VeloTrackOutputLocation]
        copyVelo.outputLocation = self.FittedVeloTrackOutputLocation
        
        ### fitting
        if self.__confdict__["VeloFitter"] == "ForwardStraightLine":
            MyVeloFit = ConfiguredForwardStraightLineEventFitter(Name="For%sVeloRefitterAlg"%self.name,
                                                                 TracksInContainer=self.FittedVeloTrackOutputLocation)
        elif self.__confdict__["VeloFitter"] == "SimplifiedGeometry":
            MyVeloFit = ConfiguredEventFitter(Name="For%sVeloRefitterAlg"%self.name,
                                              TracksInContainer=self.FittedVeloTrackOutputLocation,
                                              SimplifiedGeometry = True)
        else:
            MyVeloFit = ConfiguredEventFitter(Name="For%sVeloRefitterAlg"%self.name,
                                              TracksInContainer=self.FittedVeloTrackOutputLocation)
            
        #### making the proto particles
        MakeVeloProtos = ChargedProtoParticleMaker('For%sVeloProtoMaker'%self.name)
        MakeVeloProtos.Inputs=[self.FittedVeloTrackOutputLocation]
        MakeVeloProtos.Output = self.VeloProtoOutputLocation
        MakeVeloProtos.addTool( DelegatingTrackSelector, name="TrackSelector" )
        MakeVeloProtos.TrackSelector.TrackTypes = [ "Velo" ]
    
        #### the full sequence
        makeparts = GaudiSequencer('For%sMakeVeloTracksGS'%self.name)
        if self.__confdict__["DoVeloDecoding"]:
            makeparts.Members += [ VeloDecoding ] 
        makeparts.Members += [ MyFastVeloTracking ] 
        makeparts.Members += [ preve ] 
        makeparts.Members += [ copyVelo ] 
        makeparts.Members += [ MyVeloFit ] 
        makeparts.Members += [ MakeVeloProtos ] 
    
        #### some python magic to maek this appear like a "Selection"
        return GSWrapper(name="For%sWrappedVeloTrackingFor"%self.name,
                         sequencer=makeparts,
                         output=self.VeloProtoOutputLocation,
                         requiredSelections =  prefilter)

    def MakeVeloParticles(self,name,
                          particle, 
                          protoParticlesMaker):
        
        particleMaker =  NoPIDsParticleMaker("For%sParticleMaker%s"%(self.name,name) , Particle = particle)
        particleMaker.Input = self.VeloProtoOutputLocation
        
        DataOnDemandSvc().AlgMap.update( {
                "/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
                "/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName()
                } )
        
        AllVeloParticles = Selection("For%sSelAllVeloParts%s"%(self.name,name), 
                                     Algorithm = particleMaker, 
                                     RequiredSelections = [protoParticlesMaker], InputDataSetter=None)
        
        ### filter on the IP of the velo tracks
        return Selection("For%sSelVeloParts%s"%(self.name,name), 
                         Algorithm = FilterDesktop(name+"For%sFilterVeloTrackIP%s"%(self.name,name), 
                                                   Code="(MIPDV(PRIMARY) > %(VeloMINIP)s)" %self.__confdict__),
                         RequiredSelections = [AllVeloParticles])
    
###### OTHER FUNCTIONS ###############
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
def TOSFilter( name = None, sel = None, trigger = None ):
    if len(trigger) == 0:
        return sel
    from Configurables import TisTosParticleTagger
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = trigger
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel
###### OTHER FUNCTIONS ###############
