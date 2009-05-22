from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import TrackEventFitter, TrackMasterFitter
from Configurables import ProtoParticleCALOFilter, ProtoParticleMUONFilter
from TrackFitter.ConfiguredFitters import ConfiguredFastFitter

#Fit SeedTT Tracks
SeqFitPionsForKS0 = GaudiSequencer('SeqFitPionsForKS0')
SeqFitPionsForKS0.MeasureTime = 1

FitSeedTTTracks = TrackEventFitter('FitSeedTTTracks')
SeqFitPionsForKS0.Members += [ FitSeedTTTracks]

FitSeedTTTracks.TracksInContainer  = "Hlt/Track/SeedTT"
FitSeedTTTracks.TracksOutContainer = "Hlt/Track/FitSeedTT"

FitSeedTTTracks.addTool(TrackMasterFitter, name = 'Fitter')
ConfiguredFastFitter( getattr(FitSeedTTTracks,'Fitter'))
