from TrackSys.Configuration import *

from Configurables import ( TrackMatchVeloSeed )

tkMatch = TrackMatchVeloSeed("TrackMatch")

if TrackSys().fieldOff():
  tkMatch.Chi2MatchingCut = 4000
  tkMatch.MomentumCut = 0.
  tkMatch.pt2Cut = 0
  tkMatch.maxStepSize = 10000.
  tkMatch.yTParams = [2.5e7]
  tkMatch.tyTParams = [5e-4]
  tkMatch.chi2TCut = 1000
  tkMatch.ExtrapolatorSeed = "TrackLinearExtrapolator"

tkMatch.InputSeedTracks = "Rec/Track/Seed"
