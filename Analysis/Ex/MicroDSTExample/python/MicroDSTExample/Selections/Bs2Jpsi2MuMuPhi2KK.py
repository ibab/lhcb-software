# $Id: Bs2Jpsi2MuMuPhi2KK.py,v 1.2 2009-08-09 16:30:38 jpalac Exp $

__author__ = 'Greig Cowan'
__date__ = 'April 2009'
__version__ = '$Revision: 1.2 $'

'''
The roadmap Bs->JpsiPhi selection using LoKi::Hybrid and python
configurables. PV refitting is done.
'''

import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import CombineParticles, OfflineVertexFitter

######
# Bs #
######
Bs2Jpsi2MuMuPhi2KK = CombineParticles('Bs2Jpsi2MuMuPhi2KK')
Bs2Jpsi2MuMuPhi2KK.DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Phi
Bs2Jpsi2MuMuPhi2KK.addTool( OfflineVertexFitter() )
Bs2Jpsi2MuMuPhi2KK.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bs2Jpsi2MuMuPhi2KK.OfflineVertexFitter.useResonanceVertex = False

Bs2Jpsi2MuMuPhi2KK.CombinationCut = "ADAMASS('B_s0') < 300.*MeV"
Bs2Jpsi2MuMuPhi2KK.MotherCut = "(VFASPF(VCHI2/VDOF)<5.) & (BPVIPCHI2() < 25.)"
Bs2Jpsi2MuMuPhi2KK.ReFitPVs = True

from Jpsi2MuMu import SelJpsi2MuMu
from Phi2KK import SelPhi2KK

from PhysSelPython.Wrappers import Selection

SelBs2Jpsi2MuMuPhi2KK = Selection("SelBs2Jpsi2MuMuPhi2KK",
                                  Algorithm = Bs2Jpsi2MuMuPhi2KK,
                                  RequiredSelections = [SelJpsi2MuMu,
                                                        SelPhi2KK] )


