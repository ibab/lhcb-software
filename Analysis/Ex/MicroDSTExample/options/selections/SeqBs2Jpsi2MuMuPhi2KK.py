# $Id: SeqBs2Jpsi2MuMuPhi2KK.py,v 1.2 2009-07-02 12:05:06 jpalac Exp $

__author__ = 'Greig Cowan'
__date__ = 'April 2009'
__version__ = '$Revision: 1.2 $'

'''
The roadmap Bs->JpsiPhi selection using LoKi::Hybrid and python
configurables. PV refitting is done.
'''

import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import CombineParticles, FilterDesktop, OfflineVertexFitter

#######################################
# The Main sequence to build up the B #
#######################################
SeqBs2Jpsi2MuMuPhi2KK = GaudiSequencer('SeqBs2Jpsi2MuMuPhi2KK')

#################
# Make the Jpsi #
#################
# The Jpsi is made elsewhere and to get it you need to 
# make sure that you have these lines in your main options file:
#
# importOptions( '$CCBARROOT/options/DoSelJpsi2MuMu.py')
# SeqJpsi2MuMu = GaudiSequencer('SeqStdUnbiasedJpsi2MuMu')
# DaVinci().UserAlgorithms += [SeqJpsi2MuMu]
#
# This approach means that all the Jpsi channels for beta_s 
# can use exactly the same file containing the Jpsi selection code.

importOptions( '$MICRODSTEXAMPLEROOT/options/selections/Phi2KK.py' )
Phi2KK = getConfigurable('Phi2KK')

importOptions('$MICRODSTEXAMPLEROOT/options/selections/Jpsi2MuMu.py')
Jpsi2MuMu = getConfigurable('Jpsi2MuMu')

SeqBs2Jpsi2MuMuPhi2KK.Members = [Jpsi2MuMu, Phi2KK]

importOptions('$MICRODSTEXAMPLEROOT/options/selections/Bs2Jpsi2MuMuPhi2KK.py')
Bs2Jpsi2MuMuPhi2KK = getConfigurable('Bs2Jpsi2MuMuPhi2KK')

SeqBs2Jpsi2MuMuPhi2KK.Members += [Bs2Jpsi2MuMuPhi2KK]


