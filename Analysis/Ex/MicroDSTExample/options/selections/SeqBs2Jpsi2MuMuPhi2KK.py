# $Id: SeqBs2Jpsi2MuMuPhi2KK.py,v 1.3 2009-07-02 13:48:55 jpalac Exp $

__author__ = 'Greig Cowan'
__date__ = 'April 2009'
__version__ = '$Revision: 1.3 $'

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

importOptions( '$MICRODSTEXAMPLEROOT/options/selections/Phi2KK.py' )
Phi2KK = getConfigurable('Phi2KK')

importOptions('$MICRODSTEXAMPLEROOT/options/selections/Jpsi2MuMu.py')
Jpsi2MuMu = getConfigurable('Jpsi2MuMu')

SeqBs2Jpsi2MuMuPhi2KK.Members = [Jpsi2MuMu, Phi2KK]

importOptions('$MICRODSTEXAMPLEROOT/options/selections/Bs2Jpsi2MuMuPhi2KK.py')
Bs2Jpsi2MuMuPhi2KK = getConfigurable('Bs2Jpsi2MuMuPhi2KK')

SeqBs2Jpsi2MuMuPhi2KK.Members += [Bs2Jpsi2MuMuPhi2KK]


