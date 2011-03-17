from Gaudi.Configuration import GaudiSequencer
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from TrackSys.PatChecking import ConfigureEffCheckTools

from Configurables import ( TrackAssociator, TrackAssociatorUpgrade, TrackResChecker, TrackOccupChecker,
                            TrackEffChecker, TrackSelector, MCReconstructible,
                            MCParticleSelector, PrimaryVertexChecker, PatLHCbID2MCParticle,PatLHCbIDUp2MCParticle,
                            VeloPixChecker,VeloPixV0s,PatPVOffline, PatCheckerUpgrade, PatCounterUpgrade)

def PatCheckingUpgrade():
    
    if 'VeloPix' in TrackSys().getProp("TrackPatRecAlgorithms") :
        while len(GaudiSequencer("CheckPatSeq").Members)>0:
            membertokill = GaudiSequencer("CheckPatSeq").Members.pop(0)
        GaudiSequencer("CheckPatSeq").Members    += [ TrackAssociatorUpgrade("AssocVeloPix"),
                                                      TrackAssociatorUpgrade("AssocVeloPixTT"),
                                                      TrackAssociatorUpgrade("AssocVeloPixFitted"),
                                                      TrackAssociatorUpgrade("AssocUpForward"),
                                                      TrackAssociatorUpgrade("AssocUpMatch"),
                                                      TrackAssociatorUpgrade("AssocUpTTrack"),
                                                      TrackAssociatorUpgrade("AssocUpDownstream"),
                                                      TrackAssociatorUpgrade("AssocUpBest"),
                                                      PatLHCbIDUp2MCParticle("PatLHCbIDUp2MCParticle")
                                                      ]


        
        TrackAssociatorUpgrade("AssocVeloPix").TracksInContainer        = "Rec/Track/VeloPix"
        TrackAssociatorUpgrade("AssocVeloPixTT").TracksInContainer      = "Rec/Track/VeloPixTT"
        TrackAssociatorUpgrade("AssocVeloPixFitted").TracksInContainer  = "Rec/Track/PreparedVeloPix"
        TrackAssociatorUpgrade("AssocUpForward").TracksInContainer      = "Rec/Track/Forward"
        TrackAssociatorUpgrade("AssocUpTTrack").TracksInContainer       = "Rec/Track/Seed"
        TrackAssociatorUpgrade("AssocUpMatch").TracksInContainer        = "Rec/Track/Match"
        TrackAssociatorUpgrade("AssocUpBest").TracksInContainer        = "Rec/Track/Best"
        TrackAssociatorUpgrade("AssocUpDownstream").TracksInContainer   = "Rec/Track/Downstream"
        
        TrackAssociatorUpgrade("AssocVeloPix").UseVeloPix       = True
        TrackAssociatorUpgrade("AssocVeloPixTT").UseVeloPix     = True
        TrackAssociatorUpgrade("AssocVeloPixFitted").UseVeloPix = True
        TrackAssociatorUpgrade("AssocUpForward").UseVeloPix     = True
        TrackAssociatorUpgrade("AssocUpTTrack").UseVeloPix      = True
        TrackAssociatorUpgrade("AssocUpDownstream").UseVeloPix  = True
        TrackAssociatorUpgrade("AssocUpMatch").UseVeloPix       = True
        TrackAssociatorUpgrade("AssocUpBest").UseVeloPix       = True
        
        PatLHCbIDUp2MCParticle("PatLHCbIDUp2MCParticle").LinkVELO = False
        PatLHCbIDUp2MCParticle("PatLHCbIDUp2MCParticle").LinkVELOPIX = True

        GaudiSequencer("CheckPatSeq").Members  += [ TrackResChecker("TrackResChecker")];

        GaudiSequencer("CheckPatSeq").Members  += [ PatCheckerUpgrade("PatChecker")];
        ## remove GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("Velo"),TrackEffChecker("VeloRZ")
        
        GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("VeloPix"),
                                                     TrackEffChecker("VeloPixTT"),
                                                     TrackEffChecker("Forward"),
                                                     TrackEffChecker("TTrack"),
                                                     TrackEffChecker("Match"),
                                                     TrackEffChecker("Downstream")]
        
        GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("BestTracks") ]
   
        
        GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("VeloPix") ]
        TrackEffChecker("VeloPix").TracksInContainer = "Rec/Track/VeloPix";
        TrackEffChecker("VeloPix").SelectionCriteria = "ChargedLong";
        ConfigureEffCheckTools(TrackEffChecker("VeloPix"))
        
        GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("VeloPixTT") ]
        TrackEffChecker("VeloPixTT").TracksInContainer = "Rec/Track/VeloPixTT";
        TrackEffChecker("VeloPixTT").SelectionCriteria = "ChargedLong";
        ConfigureEffCheckTools(TrackEffChecker("VeloPixTT"))
        
        GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("VeloPixTT") ]
        TrackEffChecker("VeloPixTT").TracksInContainer = "Rec/Track/VeloPixTT";
        TrackEffChecker("VeloPixTT").SelectionCriteria = "ChargedLong";
        ConfigureEffCheckTools(TrackEffChecker("VeloPixTT"))
        

        TrackEffChecker("BestTracks").TracksInContainer = "Rec/Track/Best";
        TrackEffChecker("BestTracks").SelectionCriteria = "ChargedLong";
        TrackEffChecker("BestTracks").RequireLongTrack = True;
        ConfigureEffCheckTools(TrackEffChecker("BestTracks"))
   
        TrackEffChecker("Forward").TracksInContainer = "Rec/Track/Forward";
        TrackEffChecker("Forward").SelectionCriteria = "ChargedLong";
        ConfigureEffCheckTools(TrackEffChecker("Forward"))
   
        TrackEffChecker("TTrack").TracksInContainer = "Rec/Track/Seed";
        TrackEffChecker("TTrack").SelectionCriteria = "ChargedLong";
        ConfigureEffCheckTools(TrackEffChecker("TTrack"))
        
        TrackEffChecker("Match").TracksInContainer = "Rec/Track/Match";
        TrackEffChecker("Match").SelectionCriteria = "ChargedLong";
        ConfigureEffCheckTools(TrackEffChecker("Match"))
   
        TrackEffChecker("Downstream").TracksInContainer = "Rec/Track/Downstream";
        TrackEffChecker("Downstream").SelectionCriteria = "ChargedDownstream";
        ConfigureEffCheckTools(TrackEffChecker("Downstream"))


    if 'VeloPix' in TrackSys().getProp("TrackPatRecAlgorithms") :
        from Configurables import DataPacking__Unpack_LHCb__MCVeloPixHitPacker_,VeloPixVertices
        
        unfitChecker = VeloPixChecker("VeloPixChecker")
        unfitChecker.InputTracks  = "Rec/Track/VeloPix"
        unfitChecker.NtupleName   = "Tracks"
        #unfitChecker.addTool( LHCbIDsToMCHits(), name = "IDsToMCHits")
       
        ##fitChecker = VeloPixChecker("VeloPixCheckerFitted")
        #fitChecker.InputTracks  = "Rec/Track/PreparedVeloPix"
        #fitChecker.NtupleName = "TracksFitted"
        
         #if 'VeloPixTT' in TrackSys().getProp("TrackPatRecAlgorithms") : 
         #    fitTTChecker = VeloPixChecker("VeloPixTTCheckerFitted")
         #    fitTTChecker.InputTracks  = "Rec/Track/VeloPixTT"
         #    fitTTChecker.NtupleName = "TracksVeloPixTTFitted"
            
        fitFwdChecker = VeloPixChecker("ForwardChecker")
        fitFwdChecker.InputTracks  = "Rec/Track/Forward"
        fitFwdChecker.NtupleName = "TracksForward"

        fitBestChecker = VeloPixChecker("BestChecker")
        fitBestChecker.InputTracks  = "Rec/Track/Best"
        fitBestChecker.NtupleName = "TracksBest"
       
        ##veloPixVertices = VeloPixVertices()
        
         ##                                            trackV0Finder,veloPixVertices
        GaudiSequencer("CheckPatSeq").Members    += [ DataPacking__Unpack_LHCb__MCVeloPixHitPacker_("UnpackMCVeloPixHits")
                                                       ]#,unfitChecker, fitChecker,fitFwdChecker]
        
        GaudiSequencer("CheckPatSeq").Members    += [unfitChecker,fitFwdChecker,fitBestChecker]
        
       # if 'VeloPixTT' in TrackSys().getProp("TrackPatRecAlgorithms") : 
        #    GaudiSequencer("CheckPatSeq").Members    += [fitTTChecker]
        
        
        trackV0Finder = VeloPixV0s()
        trackV0Finder.Extrapolator.MaterialLocator='DetailedMaterialLocator'
        trackV0Finder.Interpolator.Extrapolator.MaterialLocator='DetailedMaterialLocator'
        pvAlg = PatPVOffline()
        if TrackSys().getProp( "OutputType" ).upper() != "RDST":
            # Velo tracks not copied to Rec/Track/Best for RDST 
            from Configurables import PVOfflineTool
            pvAlg.addTool( PVOfflineTool() )
            pvAlg.PVOfflineTool.InputTracks = [ "Rec/Track/Best", "Rec/Track/PreparedVeloPix" ]
        
