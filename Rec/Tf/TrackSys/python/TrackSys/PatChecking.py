from Gaudi.Configuration import GaudiSequencer
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from Configurables import ( TrackAssociator, TrackResChecker, TrackOccupChecker,
                            TrackEffChecker, TrackSelector, MCReconstructible,
                            MCParticleSelector, PrimaryVertexChecker, PatLHCbID2MCParticle)
                            
def PatChecking():
   
   if not TrackSys().veloOpen():
      GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocVeloRZ") ]

   GaudiSequencer("CheckPatSeq").Members    += [ TrackAssociator("AssocVelo"),
                                                 TrackAssociator("AssocVeloTT"),
                                                 TrackAssociator("AssocForward"),
                                                 TrackAssociator("AssocTTrack"),
                                                 TrackAssociator("AssocMatch"),
                                                 TrackAssociator("AssocDownstream"),
                                                 PatLHCbID2MCParticle("PatLHCbID2MCParticle")]
   
   
   TrackAssociator("AssocVeloRZ").TracksInContainer     = "Rec/Track/RZVelo";
   TrackAssociator("AssocVelo").TracksInContainer       = "Rec/Track/Velo";
   TrackAssociator("AssocVeloTT").TracksInContainer     = "Rec/Track/VeloTT";
   TrackAssociator("AssocForward").TracksInContainer    = "Rec/Track/Forward";
   TrackAssociator("AssocTTrack").TracksInContainer     = "Rec/Track/Seed";
   TrackAssociator("AssocMatch").TracksInContainer      = "Rec/Track/Match";
   TrackAssociator("AssocDownstream").TracksInContainer = "Rec/Track/Downstream";
   
   
   GaudiSequencer("CheckPatSeq").Members  += [ TrackResChecker("TrackResChecker")];
   TrackResChecker("TrackResChecker").SplitByType = True;
   ConfigureEffCheckTools(TrackResChecker("TrackResChecker"))
   
   if not TrackSys().veloOpen():
     GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("VeloRZ") ]
     
   GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("Velo"),
                                                TrackEffChecker("VeloTT"),
                                                TrackEffChecker("Forward"),
                                                TrackEffChecker("TTrack"),
                                                TrackEffChecker("Match"),
                                                TrackEffChecker("Downstream")]
   
   GaudiSequencer("CheckPatSeq").Members   += [ TrackEffChecker("BestTracks") ]
   
   TrackEffChecker("BestTracks").TracksInContainer = "Rec/Track/Best";
   TrackEffChecker("BestTracks").SelectionCriteria = "ChargedLong";
   TrackEffChecker("BestTracks").RequireLongTrack = True;
   ConfigureEffCheckTools(TrackEffChecker("BestTracks"))
   
   TrackEffChecker("VeloRZ").TracksInContainer = "Rec/Track/RZVelo";
   TrackEffChecker("VeloRZ").SelectionCriteria = "ChargedLong";
   ConfigureEffCheckTools(TrackEffChecker("VeloRZ"))
   
   
   TrackEffChecker("Velo").TracksInContainer = "Rec/Track/Velo";
   TrackEffChecker("Velo").SelectionCriteria = "ChargedLong";
   ConfigureEffCheckTools(TrackEffChecker("Velo"))
   
   TrackEffChecker("VeloTT").TracksInContainer = "Rec/Track/VeloTT";
   TrackEffChecker("VeloTT").SelectionCriteria = "ChargedLong";
   ConfigureEffCheckTools(TrackEffChecker("VeloTT"))
   
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
   
   GaudiSequencer("CheckPatSeq").Members  += [TrackOccupChecker("OccupancyCheck")]

   #don't do this for DC06 because there's an FPE.
   if TrackSys().getProp('DataType')!="DC06":
      GaudiSequencer("CheckPatSeq").Members  += [PrimaryVertexChecker("PVOfflineCheck")]

def ConfigureEffCheckTools(EffCheck):
   EffCheck.addTool(MCReconstructible, name="Selector")
   EffCheck.Selector.addTool(MCParticleSelector, name="Selector")
   EffCheck.Selector.Selector.rejectElectrons = True
   EffCheck.Selector.Selector.rejectInteractions = True
   EffCheck.Selector.Selector.zInteraction = 9400.
   EffCheck.HistoPrint = False
   EffCheck.StatPrint = False
   EffCheck.FullDetail = False

