from Gaudi.Configuration import GaudiSequencer
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from Configurables import ( TrackAssociator, TrackAssociatorUpgrade, TrackResChecker, TrackOccupChecker,
                            TrackEffChecker, TrackSelector, MCReconstructible,
                            MCParticleSelector, PrimaryVertexChecker, PatLHCbID2MCParticle,
                            VeloPixChecker )
                            

if not TrackSys().veloOpen() and  'Velo' in TrackSys().getProp("TrackPatRecAlgorithms") :
   GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocVeloRZ") ]
   
if 'Velo' in TrackSys().TrackPatRecAlgorithms :
   GaudiSequencer("CheckPatSeq").Members    += [ TrackAssociator("AssocVelo"),
                                                 TrackAssociator("AssocVeloTT")]
   TrackAssociator("AssocVeloRZ").TracksInContainer     = "Rec/Track/RZVelo";
   TrackAssociator("AssocVelo").TracksInContainer       = "Rec/Track/Velo";
   TrackAssociator("AssocVeloTT").TracksInContainer     = "Rec/Track/VeloTT";
   
if 'VeloPix' in TrackSys().getProp("TrackPatRecAlgorithms") :
   print 'to uncomment'
   GaudiSequencer("CheckPatSeq").Members    += [ TrackAssociatorUpgrade("AssocVeloPix"),
                                                 TrackAssociatorUpgrade("AssocVeloPixFitted")]
   TrackAssociatorUpgrade("AssocVeloPix").TracksInContainer       = "Rec/Track/VeloPix";
   TrackAssociatorUpgrade("AssocVeloPix").UseVeloPix       = True;
   TrackAssociatorUpgrade("AssocVeloPixFitted").TracksInContainer       = "Rec/Track/PreparedVeloPix";
   TrackAssociatorUpgrade("AssocVeloPixFitted").UseVeloPix       = True;
if 'VeloPixTT' in TrackSys().getProp("TrackPatRecAlgorithms") :
   GaudiSequencer("CheckPatSeq").Members    += [ TrackAssociatorUpgrade("AssocVeloPixTT")]
   TrackAssociatorUpgrade("AssocVeloPixTT").TracksInContainer     = "Rec/Track/VeloPixTT";
   TrackAssociatorUpgrade("AssocVeloPixTT").UseVeloPix       = True;

   TrackAssociatorUpgrade("AssocForward").TracksInContainer    = "Rec/Track/Forward";
   TrackAssociatorUpgrade("AssocForward").UseVeloPix       = True;

   TrackAssociator("AssocTTrack").TracksInContainer     = "Rec/Track/Seed";
   TrackAssociatorUpgrade("AssocMatch").TracksInContainer      = "Rec/Track/Match";
   TrackAssociatorUpgrade("AssocMatch").UseVeloPix       = True;
   TrackAssociator("AssocDownstream").TracksInContainer = "Rec/Track/Downstream";


if 'VeloPix' in TrackSys().getProp("TrackPatRecAlgorithms") :
   
   from Configurables import DataPacking__Unpack_LHCb__MCVeloPixHitPacker_
   
   unfitChecker = VeloPixChecker("VeloPixChecker")
   unfitChecker.InputTracks  = "Rec/Track/VeloPix"
   unfitChecker.NtupleName   = "Tracks"
   #unfitChecker.addTool( LHCbIDsToMCHits(), name = "IDsToMCHits")
   #unfitChecker.IDsToMCHits 
   #           fitter.Fitter.MeasProvider.VeloPixProvider = VeloPixLiteMeasurementProvider()
    
   fitChecker = VeloPixChecker("VeloPixCheckerFitted")
   fitChecker.InputTracks  = "Rec/Track/PreparedVeloPix"
   fitChecker.NtupleName = "TracksFitted"
##    fitTTChecker = VeloPixChecker("VeloPixTTCheckerFitted")
##    fitTTChecker.InputTracks  = "Rec/Track/VeloPixTT"
##    fitTTChecker.NtupleName = "TracksVeloPixTTFitted"
##    fitFwdChecker = VeloPixChecker("ForwardChecker")
##    fitFwdChecker.InputTracks  = "Rec/Track/Forward"
##    fitFwdChecker.NtupleName = "TracksForward"
                                                
   GaudiSequencer("CheckPatSeq").Members    += [ DataPacking__Unpack_LHCb__MCVeloPixHitPacker_("UnpackMCVeloPixHits"),
                                                 unfitChecker, fitChecker]#,fitTTChecker,fitFwdChecker]

else :
   GaudiSequencer("CheckPatSeq").Members    += [ TrackAssociator("AssocForward"),
                                                 TrackAssociator("AssocTTrack"),
                                                 TrackAssociator("AssocMatch"),
                                                 TrackAssociator("AssocDownstream"),
                                                 PatLHCbID2MCParticle("PatLHCbID2MCParticle")]
   TrackAssociator("AssocForward").TracksInContainer    = "Rec/Track/Forward";
   TrackAssociator("AssocTTrack").TracksInContainer     = "Rec/Track/Seed";
   TrackAssociator("AssocMatch").TracksInContainer      = "Rec/Track/Match";
   TrackAssociator("AssocDownstream").TracksInContainer = "Rec/Track/Downstream";


   GaudiSequencer("CheckPatSeq").Members  += [ TrackResChecker("TrackResChecker")];


   TrackResChecker("TrackResChecker").SplitByType = True;
   TrackResChecker("TrackResChecker").addTool(MCReconstructible, name="Selector");
   TrackResChecker("TrackResChecker").Selector.addTool(MCParticleSelector, name="Selector");
   TrackResChecker("TrackResChecker").Selector.Selector.rejectElectrons = True;
   TrackResChecker("TrackResChecker").Selector.Selector.rejectInteractions = True;
   TrackResChecker("TrackResChecker").Selector.Selector.zInteraction = 9400.;
   TrackResChecker("TrackResChecker").FullDetail = False;
   TrackResChecker("TrackResChecker").HistoPrint = False;
   TrackResChecker("TrackResChecker").StatPrint = False;

   if not TrackSys().veloOpen():
     GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("VeloRZ") ]

   GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("Velo"),
                                                TrackEffChecker("VeloTT"),
                                                TrackEffChecker("Forward"),
                                                TrackEffChecker("TTrack"),
                                                TrackEffChecker("Match"),
                                                TrackEffChecker("Downstream")]

   GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("BestTracks") ]

   TrackEffChecker("BestTracks").FullDetail = False;
   TrackEffChecker("BestTracks").TracksInContainer = "Rec/Track/Best";
   TrackEffChecker("BestTracks").addTool(MCReconstructible, name="Selector");
   TrackEffChecker("BestTracks").Selector.addTool(MCParticleSelector, name="Selector");
   TrackEffChecker("BestTracks").Selector.Selector.rejectElectrons = True;
   TrackEffChecker("BestTracks").Selector.Selector.rejectInteractions = True;
   TrackEffChecker("BestTracks").Selector.Selector.zInteraction = 9400.;
   TrackEffChecker("BestTracks").SelectionCriteria = "ChargedLong";
   TrackEffChecker("BestTracks").HistoPrint = False;
   TrackEffChecker("BestTracks").StatPrint = False;
   TrackEffChecker("BestTracks").RequireLongTrack = True;

   TrackEffChecker("VeloRZ").FullDetail = False;
   TrackEffChecker("VeloRZ").TracksInContainer = "Rec/Track/RZVelo";
   TrackEffChecker("VeloRZ").addTool(MCReconstructible, name="Selector");
   TrackEffChecker("VeloRZ").Selector.addTool(MCParticleSelector, name="Selector");
   TrackEffChecker("VeloRZ").Selector.Selector.rejectElectrons = True;
   TrackEffChecker("VeloRZ").Selector.Selector.rejectInteractions = True;
   TrackEffChecker("VeloRZ").Selector.Selector.zInteraction = 9400.;
   TrackEffChecker("VeloRZ").SelectionCriteria = "ChargedLong";
   TrackEffChecker("VeloRZ").HistoPrint = False;
   TrackEffChecker("VeloRZ").StatPrint = False;


   TrackEffChecker("Velo").FullDetail = False;
   TrackEffChecker("Velo").TracksInContainer = "Rec/Track/Velo";
   TrackEffChecker("Velo").addTool(MCReconstructible, name="Selector");
   TrackEffChecker("Velo").Selector.addTool(MCParticleSelector, name="Selector");
   TrackEffChecker("Velo").Selector.Selector.rejectElectrons = True;
   TrackEffChecker("Velo").Selector.Selector.rejectInteractions = True;
   TrackEffChecker("Velo").Selector.Selector.zInteraction = 9400.;
   TrackEffChecker("Velo").SelectionCriteria = "ChargedLong";
   TrackEffChecker("Velo").HistoPrint = False;
   TrackEffChecker("Velo").StatPrint = False;

   TrackEffChecker("VeloTT").FullDetail = False;
   TrackEffChecker("VeloTT").TracksInContainer = "Rec/Track/VeloTT";
   TrackEffChecker("VeloTT").addTool(MCReconstructible, name="Selector");
   TrackEffChecker("VeloTT").Selector.addTool(MCParticleSelector, name="Selector");
   TrackEffChecker("VeloTT").Selector.Selector.rejectElectrons = True;
   TrackEffChecker("VeloTT").Selector.Selector.rejectInteractions = True;
   TrackEffChecker("VeloTT").Selector.Selector.zInteraction = 9400.;
   TrackEffChecker("VeloTT").SelectionCriteria = "ChargedLong";
   TrackEffChecker("VeloTT").HistoPrint = False;
   TrackEffChecker("VeloTT").StatPrint = False;

   TrackEffChecker("Forward").FullDetail = False;
   TrackEffChecker("Forward").TracksInContainer = "Rec/Track/Forward";
   TrackEffChecker("Forward").addTool(MCReconstructible, name="Selector");
   TrackEffChecker("Forward").Selector.addTool(MCParticleSelector, name="Selector");
   TrackEffChecker("Forward").Selector.Selector.rejectElectrons = True;
   TrackEffChecker("Forward").Selector.Selector.rejectInteractions = True;
   TrackEffChecker("Forward").Selector.Selector.zInteraction = 9400.;
   TrackEffChecker("Forward").SelectionCriteria = "ChargedLong";
   TrackEffChecker("Forward").HistoPrint = False;
   TrackEffChecker("Forward").StatPrint = False;

   TrackEffChecker("TTrack").FullDetail = False;
   TrackEffChecker("TTrack").TracksInContainer = "Rec/Track/Seed";
   TrackEffChecker("TTrack").addTool(MCReconstructible, name="Selector");
   TrackEffChecker("TTrack").Selector.addTool(MCParticleSelector, name="Selector");
   TrackEffChecker("TTrack").Selector.Selector.rejectElectrons = True;
   TrackEffChecker("TTrack").Selector.Selector.rejectInteractions = True;
   TrackEffChecker("TTrack").Selector.Selector.zInteraction = 9400.;
   TrackEffChecker("TTrack").SelectionCriteria = "ChargedLong";
   TrackEffChecker("TTrack").HistoPrint = False;
   TrackEffChecker("TTrack").StatPrint = False;

   TrackEffChecker("Match").FullDetail = False;
   TrackEffChecker("Match").TracksInContainer = "Rec/Track/Match";
   TrackEffChecker("Match").addTool(MCReconstructible, name="Selector");
   TrackEffChecker("Match").Selector.addTool(MCParticleSelector, name="Selector");
   TrackEffChecker("Match").Selector.Selector.rejectElectrons = True;
   TrackEffChecker("Match").Selector.Selector.rejectInteractions = True;
   TrackEffChecker("Match").Selector.Selector.zInteraction = 9400.;
   TrackEffChecker("Match").SelectionCriteria = "ChargedLong";
   TrackEffChecker("Match").HistoPrint = False;
   TrackEffChecker("Match").StatPrint = False;

   TrackEffChecker("Downstream").FullDetail = False;
   TrackEffChecker("Downstream").TracksInContainer = "Rec/Track/Downstream";
   TrackEffChecker("Downstream").addTool(MCReconstructible, name="Selector");
   TrackEffChecker("Downstream").Selector.addTool(MCParticleSelector, name="Selector");
   TrackEffChecker("Downstream").Selector.Selector.rejectElectrons = True;
   TrackEffChecker("Downstream").Selector.Selector.rejectInteractions = True;
   TrackEffChecker("Downstream").Selector.Selector.zInteraction = 9400.;
   TrackEffChecker("Downstream").SelectionCriteria = "ChargedDownstream";
   TrackEffChecker("Downstream").HistoPrint = False;
   TrackEffChecker("Downstream").StatPrint = False;

   GaudiSequencer("CheckPatSeq").Members  += [TrackOccupChecker("OccupancyCheck")]
GaudiSequencer("CheckPatSeq").Members  += [PrimaryVertexChecker("PVOfflineCheck")]

