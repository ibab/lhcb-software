#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Inputs.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   05.03.2015
# =============================================================================
"""Import the needed inputs for the HLT2 radiative lines."""

from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons     as Hlt2Photons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichPions   as Hlt2LoosePions
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichKaons   as Hlt2LooseKaons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotonsFromL0 as L0Photons
from Hlt2SharedParticles.ConvPhoton import ConvPhotonLL as Hlt2DiElectron_LL
from Hlt2SharedParticles.ConvPhoton import ConvPhotonDD as Hlt2DiElectron_DD
from Hlt2SharedParticles.Lambda import LambdaLLTrackFitted as LambdaLL
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownKaons as Hlt2DownKaons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions as Hlt2DownPions
from Hlt2SharedParticles.Lambda import LambdaDDTrackFitted as LambdaDD

# No PIDs
# from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichProtons as Hlt2LooseProtons
# from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions       as Hlt2NoPIDsPions
# from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons       as Hlt2NoPIDsKaons
# from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedProtons     as Hlt2NoPIDsProtons

# EOF
