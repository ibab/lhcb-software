## --------------------------------------------------------------------------------
## Lines for decays with out a decay vertex
## Eg, B+ -> K+ pi0, Lambda_b -> Lambda0 Gamma, B0 -> K0 pi0
##
## Author: Jason Andrews, jea@umd.edu
## Shameless copy of Albert Puig's Radiative lines
## --------------------------------------------------------------------------------
## switch to kaons from GoodParticles after tuning?
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichKaons as Hlt2LooseKaons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotonsFromL0 as L0Photons

## L0Photons causing the error?
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons     as Hlt2Photons
