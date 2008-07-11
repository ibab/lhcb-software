from Brunel.Configuration import *

from Configurables import ( TrackMatchVeloSeed )

if Brunel().getProp("fieldOff"):
  TrackMatchVeloSeed("TrackMatch").Chi2MatchingCut = 4000;
  TrackMatchVeloSeed("TrackMatch").MomentumCut = 0.;
  TrackMatchVeloSeed("TrackMatch").pt2Cut = 0;
  TrackMatchVeloSeed("TrackMatch").maxStepSize = 10000.;
  TrackMatchVeloSeed("TrackMatch").yTParams = [2.5e7];
  TrackMatchVeloSeed("TrackMatch").tyTParams = [5e-4];
  TrackMatchVeloSeed("TrackMatch").chi2TCut = 1000;
  TrackMatchVeloSeed("TrackMatch").ExtrapolatorSeed = "TrackLinearExtrapolator";

TrackMatchVeloSeed("TrackMatch").InputSeedTracks = "Rec/Track/Seed"
