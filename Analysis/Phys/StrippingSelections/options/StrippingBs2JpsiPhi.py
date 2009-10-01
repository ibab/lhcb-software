# $Id: StrippingBs2JpsiPhi.py,v 1.6 2009-10-01 14:52:23 jpalac Exp $

__author__ = 'Juan Palacios'
__date__ = '25/09/2009'
__version__ = '$Revision: 1.6 $'

'''
Bs->JpsiPhi stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

from StrippingSelections import Bs2JpsiPhi
selSeq = Bs2JpsiPhi.sequence
############################################
# Create StrippingLine with this selection #
############################################
Bs2JpsiPhiLine = StrippingLine('Bs2JpsiPhiLine'
               , prescale = 1
               , algos = [selSeq]
               , stream = 'Bmuon'
               )
