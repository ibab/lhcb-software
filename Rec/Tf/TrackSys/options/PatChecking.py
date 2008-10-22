
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


GaudiSequencer("CheckPatSeq").Members  += [ TrackResChecker("CheckFitForward"),
                                            TrackResChecker("CheckFitMatch"),
                                            TrackResChecker("CheckFitTTrack"),
                                            TrackResChecker("CheckFitVelo"),
                                            TrackResChecker("CheckFitBest") ];

#TrackResChecker("CheckFitForward").fullDetail = False;
TrackResChecker("CheckFitForward").TracksInContainer = "Rec/Track/Forward";
TrackResChecker("CheckFitForward").addTool(MCReconstructible, name="Selector");
TrackResChecker("CheckFitForward").Selector.addTool(MCParticleSelector, name="Selector");
TrackResChecker("CheckFitForward").Selector.Selector.rejectElectrons = True;
TrackResChecker("CheckFitForward").Selector.Selector.rejectInteractions = True;
TrackResChecker("CheckFitForward").Selector.Selector.zInteraction = 9400.;
TrackResChecker("CheckFitForward").HistoPrint = False;
TrackResChecker("CheckFitForward").SelectionCriteria = "ChargedLong";

#TrackResChecker("CheckFitBest").fullDetail = False;
TrackResChecker("CheckFitBest").TracksInContainer = "Rec/Track/Best";
TrackResChecker("CheckFitBest").addTool(MCReconstructible, name="Selector");
TrackResChecker("CheckFitBest").Selector.addTool(MCParticleSelector, name="Selector");
TrackResChecker("CheckFitBest").Selector.Selector.rejectElectrons = True;
TrackResChecker("CheckFitBest").Selector.Selector.rejectInteractions = True;
TrackResChecker("CheckFitBest").Selector.Selector.zInteraction = 9400.;
TrackResChecker("CheckFitBest").HistoPrint = False;
TrackResChecker("CheckFitBest").SelectionCriteria = "ChargedLong";

#TrackResChecker("CheckFitMatch").fullDetail = False;
TrackResChecker("CheckFitMatch").TracksInContainer = "Rec/Track/Match";
TrackResChecker("CheckFitMatch").addTool(MCReconstructible, name="Selector");
TrackResChecker("CheckFitMatch").Selector.addTool(MCParticleSelector, name="Selector");
TrackResChecker("CheckFitMatch").Selector.Selector.rejectElectrons = True;
TrackResChecker("CheckFitMatch").Selector.Selector.rejectInteractions = True;
TrackResChecker("CheckFitMatch").Selector.Selector.zInteraction = 9400.;
TrackResChecker("CheckFitMatch").HistoPrint = False;
TrackResChecker("CheckFitMatch").SelectionCriteria = "ChargedLong";

#TrackResChecker("CheckFitTTrack").fullDetail = False;
TrackResChecker("CheckFitTTrack").TracksInContainer = "Rec/Track/Seed";
TrackResChecker("CheckFitTTrack").addTool(MCReconstructible, name="Selector");
TrackResChecker("CheckFitTTrack").Selector.addTool(MCParticleSelector, name="Selector");
TrackResChecker("CheckFitTTrack").Selector.Selector.rejectElectrons = True;
TrackResChecker("CheckFitTTrack").Selector.Selector.rejectInteractions = True;
TrackResChecker("CheckFitTTrack").Selector.Selector.zInteraction = 9400.;
TrackResChecker("CheckFitTTrack").HistoPrint = False;
TrackResChecker("CheckFitTTrack").SelectionCriteria = "ChargedLong";


#TrackResChecker("CheckFitVelo").fullDetail = False;
TrackResChecker("CheckFitVelo").TracksInContainer = "Rec/Track/Velo";
TrackResChecker("CheckFitVelo").addTool(MCReconstructible, name="Selector");
TrackResChecker("CheckFitVelo").Selector.addTool(MCParticleSelector, name="Selector");
TrackResChecker("CheckFitVelo").Selector.Selector.rejectElectrons = True;
TrackResChecker("CheckFitVelo").Selector.Selector.rejectInteractions = True;
TrackResChecker("CheckFitVelo").Selector.Selector.zInteraction = 9400.;
TrackResChecker("CheckFitVelo").HistoPrint = False;
TrackResChecker("CheckFitVelo").SelectionCriteria = "ChargedLong";


# example of how to use the
GaudiSequencer("CheckPatSeq").Members  += [ TrackEffChecker("VeloRZ"),
                                            TrackEffChecker("Velo"),
                                            TrackEffChecker("VeloTT"),
                                            TrackEffChecker("Forward"),
                                            TrackEffChecker("TTrack"),
                                            TrackEffChecker("Match"),
                                            TrackEffChecker("Downstream"),
                                            TrackEffChecker("BestTracks")];


#TrackEffChecker("BestTracks").fullDetail = False;
TrackEffChecker("BestTracks").TracksInContainer = "Rec/Track/Best";
TrackEffChecker("BestTracks").addTool(MCReconstructible, name="Selector");
TrackEffChecker("BestTracks").Selector.addTool(MCParticleSelector, name="Selector");
TrackEffChecker("BestTracks").Selector.Selector.rejectElectrons = True;
TrackEffChecker("BestTracks").Selector.Selector.rejectInteractions = True;
TrackEffChecker("BestTracks").Selector.Selector.zInteraction = 9400.;
TrackEffChecker("BestTracks").SelectionCriteria = "ChargedLong";
TrackEffChecker("BestTracks").HistoPrint = False;
TrackEffChecker("BestTracks").StatPrint = False;


#TrackEffChecker("VeloRZ").fullDetail = False;
TrackEffChecker("VeloRZ").TracksInContainer = "Rec/Track/RZVelo";
TrackEffChecker("VeloRZ").addTool(MCReconstructible, name="Selector");
TrackEffChecker("VeloRZ").Selector.addTool(MCParticleSelector, name="Selector");
TrackEffChecker("VeloRZ").Selector.Selector.rejectElectrons = True;
TrackEffChecker("VeloRZ").Selector.Selector.rejectInteractions = True;
TrackEffChecker("VeloRZ").Selector.Selector.zInteraction = 9400.;
TrackEffChecker("VeloRZ").SelectionCriteria = "ChargedLong";
TrackEffChecker("VeloRZ").HistoPrint = False;
TrackEffChecker("VeloRZ").StatPrint = False;


#TrackEffChecker("Velo").fullDetail = False;
TrackEffChecker("Velo").TracksInContainer = "Rec/Track/Velo";
TrackEffChecker("Velo").addTool(MCReconstructible, name="Selector");
TrackEffChecker("Velo").Selector.addTool(MCParticleSelector, name="Selector");
TrackEffChecker("Velo").Selector.Selector.rejectElectrons = True;
TrackEffChecker("Velo").Selector.Selector.rejectInteractions = True;
TrackEffChecker("Velo").Selector.Selector.zInteraction = 9400.;
TrackEffChecker("Velo").SelectionCriteria = "ChargedLong";
TrackEffChecker("Velo").HistoPrint = False;
TrackEffChecker("Velo").StatPrint = False;

#TrackEffChecker("VeloTT").fullDetail = False;
TrackEffChecker("VeloTT").TracksInContainer = "Rec/Track/VeloTT";
TrackEffChecker("VeloTT").addTool(MCReconstructible, name="Selector");
TrackEffChecker("VeloTT").Selector.addTool(MCParticleSelector, name="Selector");
TrackEffChecker("VeloTT").Selector.Selector.rejectElectrons = True;
TrackEffChecker("VeloTT").Selector.Selector.rejectInteractions = True;
TrackEffChecker("VeloTT").Selector.Selector.zInteraction = 9400.;
TrackEffChecker("VeloTT").SelectionCriteria = "ChargedLong";
TrackEffChecker("VeloTT").HistoPrint = False;
TrackEffChecker("VeloTT").StatPrint = False;

#TrackEffChecker("Forward").fullDetail = False;
TrackEffChecker("Forward").TracksInContainer = "Rec/Track/Forward";
TrackEffChecker("Forward").addTool(MCReconstructible, name="Selector");
TrackEffChecker("Forward").Selector.addTool(MCParticleSelector, name="Selector");
TrackEffChecker("Forward").Selector.Selector.rejectElectrons = True;
TrackEffChecker("Forward").Selector.Selector.rejectInteractions = True;
TrackEffChecker("Forward").Selector.Selector.zInteraction = 9400.;
TrackEffChecker("Forward").SelectionCriteria = "ChargedLong";
TrackEffChecker("Forward").HistoPrint = False;
TrackEffChecker("Forward").StatPrint = False;

#TrackEffChecker("TTrack").fullDetail = False;
TrackEffChecker("TTrack").TracksInContainer = "Rec/Track/Seed";
TrackEffChecker("TTrack").addTool(MCReconstructible, name="Selector");
TrackEffChecker("TTrack").Selector.addTool(MCParticleSelector, name="Selector");
TrackEffChecker("TTrack").Selector.Selector.rejectElectrons = True;
TrackEffChecker("TTrack").Selector.Selector.rejectInteractions = True;
TrackEffChecker("TTrack").Selector.Selector.zInteraction = 9400.;
TrackEffChecker("TTrack").SelectionCriteria = "ChargedLong";
TrackEffChecker("TTrack").HistoPrint = False;
TrackEffChecker("TTrack").StatPrint = False;

#TrackEffChecker("Match").fullDetail = False;
TrackEffChecker("Match").TracksInContainer = "Rec/Track/Match";
TrackEffChecker("Match").addTool(MCReconstructible, name="Selector");
TrackEffChecker("Match").Selector.addTool(MCParticleSelector, name="Selector");
TrackEffChecker("Match").Selector.Selector.rejectElectrons = True;
TrackEffChecker("Match").Selector.Selector.rejectInteractions = True;
TrackEffChecker("Match").Selector.Selector.zInteraction = 9400.;
TrackEffChecker("Match").SelectionCriteria = "ChargedLong";
TrackEffChecker("Match").HistoPrint = False;
TrackEffChecker("Match").StatPrint = False;

#TrackEffChecker("Downstream").fullDetail = False;
TrackEffChecker("Downstream").TracksInContainer = "Rec/Track/Downstream";
TrackEffChecker("Downstream").addTool(MCReconstructible, name="Selector");
TrackEffChecker("Downstream").Selector.addTool(MCParticleSelector, name="Selector");
TrackEffChecker("Downstream").Selector.Selector.rejectElectrons = True;
TrackEffChecker("Downstream").Selector.Selector.rejectInteractions = True;
TrackEffChecker("Downstream").Selector.Selector.zInteraction = 9400.;
TrackEffChecker("Downstream").SelectionCriteria = "ChargedLong";
TrackEffChecker("Downstream").HistoPrint = False;
TrackEffChecker("Downstream").StatPrint = False;


if TrackSys().veloOpen():
   GaudiSequencer("CheckPatSeq").Members.remove("TrackAssociator/AssocVeloRZ")
   GaudiSequencer("CheckPatSeq").Members.remove("TrackAssociator/AssocDownstream")
   GaudiSequencer("CheckPatSeq").Members.remove("TrackEffChecker/VeloRZ")
   GaudiSequencer("CheckPatSeq").Members.remove("TrackEffChecker/Downstream")
     
