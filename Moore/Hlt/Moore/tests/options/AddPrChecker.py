# adds the Track efficienicy checking 
def ConfigureEffCheckTools(EffCheck):
   EffCheck.addTool(MCReconstructible, name="Selector")
   EffCheck.Selector.addTool(MCParticleSelector, name="Selector")
   EffCheck.Selector.Selector.rejectElectrons = True
   EffCheck.Selector.Selector.rejectInteractions = True
   EffCheck.Selector.Selector.zInteraction = 9400.
   EffCheck.HistoPrint = False
   EffCheck.StatPrint = True
   EffCheck.FullDetail = False


def myAction() :   
    from Gaudi.Configuration import GaudiSequencer
    from Configurables import ( PrTrackAssociator, TrackResChecker, TrackOccupChecker,
                                PrChecker, TrackSelector, MCReconstructible,
                                MCParticleSelector, PrLHCbID2MCParticle,
                                UnpackMCParticle, UnpackMCVertex, DebugTrackingLosses )

    from HltTracking.HltTrackNames import Hlt1TrackLoc, Hlt1TrackRoot
    
    from DAQSys.Decoders import DecoderDB
    from DAQSys.DecoderClass import decodersForBank
    
    decs=[]

    decs=decs+decodersForBank(DecoderDB,"TT")

    DecSeq = GaudiSequencer("MyDecodingSeq")
    DecSeq.Members += [d.setup() for d in decs ]

    PatCheck = GaudiSequencer("CheckPatSeq")
    PatCheck.Members += [ UnpackMCParticle(), UnpackMCVertex(), PrLHCbID2MCParticle("PrLHCbID2MCParticle")]

    PrChecker("PrChecker").TriggerNumbers = True
    PrChecker("PrChecker").Eta25Cut = True

    PrChecker("PrChecker").WriteForwardHistos = 1
    PrChecker("PrChecker").WriteUpHistos  = 0
    PrChecker("PrChecker").WriteVeloHistos  = 1

    #(N.B.: container base for velo tracks was renamed to Hlt1)
    # Figure out which HLT is run from HltConf
    #PrChecker("PrChecker").VeloTracks = Hlt1TrackLoc["Velo"]
    PrChecker("PrChecker").ForwardTracks = Hlt1TrackLoc["ForwardHPT"] #HltSharedForwardLocation
    PrChecker("PrChecker").UpTracks = Hlt1TrackLoc["VeloTTHPT"];

    PrChecker("PrChecker").HistoPrint = True
    #PrChecker("PrChecker").OutputLevel = 1

    PrTrackAssociator("AssocAll").RootOfContainers = Hlt1TrackRoot
    
    PatCheck.Members += [ PrTrackAssociator("AssocAll") ]
    PatCheck.Members += [ PrChecker("PrChecker") ]

    EndMembers = GaudiSequencer("HltEndSequence").Members
    EndMembers.insert(0,  DecSeq )
    EndMembers.insert(1,  PatCheck )
    #end of myAction
    
from GaudiKernel.Configurable import appendPostConfigAction
appendPostConfigAction( myAction )


#seq = GaudiSequencer("CheckPatSeq")
#seq.Members += [ UnpackMCParticle(), UnpackMCVertex(), PrLHCbID2MCParticle("PrLHCbID2MCParticle")]

#seq.Members += [ TrackAssociator("AssocForward") ]
#seq.Members += [ TrackEffChecker("Forward") ]
#TrackAssociator("AssocForward").TracksInContainer    = HltSharedForwardLocation
#TrackEffChecker("Forward").TracksInContainer = HltSharedForwardLocation
#TrackEffChecker("Forward").SelectionCriteria = "";
#ConfigureEffCheckTools(TrackEffChecker("Forward"))







