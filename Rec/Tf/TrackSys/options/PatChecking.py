from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from Configurables import ( TrackAssociator, TrackResChecker,
                            TrackEffChecker, TrackSelector, MCReconstructible,
                            MCParticleSelector)
                            

GaudiSequencer("CheckPatSeq").Members  += [ TrackAssociator("AssocVeloRZ"),
                                            TrackAssociator("AssocVelo"),
                                            TrackAssociator("AssocVeloTT"),
                                            TrackAssociator("AssocForward"),
                                            TrackAssociator("AssocTTrack"),
                                            TrackAssociator("AssocMatch"),
                                            TrackAssociator("AssocDownstream")];

	 
TrackAssociator("AssocVeloRZ").TracksInContainer     = "Rec/Track/RZVelo";
TrackAssociator("AssocVelo").TracksInContainer       = "Rec/Track/Velo";
TrackAssociator("AssocVeloTT").TracksInContainer     = "Rec/Track/VeloTT";
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

# example of how to use the
GaudiSequencer("CheckPatSeq").Members  += [ TrackEffChecker("VeloRZ"),
                                            TrackEffChecker("Velo"),
                                            TrackEffChecker("VeloTT"),
                                            TrackEffChecker("Forward"),
                                            TrackEffChecker("TTrack"),
                                            TrackEffChecker("Match"),
                                            TrackEffChecker("Downstream"),
                                            TrackEffChecker("BestTracks")];


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
TrackEffChecker("Downstream").SelectionCriteria = "ChargedLong";
TrackEffChecker("Downstream").HistoPrint = False;
TrackEffChecker("Downstream").StatPrint = False;

if TrackSys().getProp( "veloOpen" ):
   GaudiSequencer("CheckPatSeq").Members.remove("TrackAssociator/AssocVeloRZ")
   GaudiSequencer("CheckPatSeq").Members.remove("TrackAssociator/AssocDownstream")
   GaudiSequencer("CheckPatSeq").Members.remove("TrackEffChecker/VeloRZ")
   GaudiSequencer("CheckPatSeq").Members.remove("TrackEffChecker/Downstream")
     
