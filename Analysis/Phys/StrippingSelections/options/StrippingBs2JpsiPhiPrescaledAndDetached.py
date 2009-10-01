# $Id: StrippingBs2JpsiPhiPrescaledAndDetached.py,v 1.3 2009-10-01 14:52:23 jpalac Exp $

__author__ = 'Juan Palacios'
__date__ = '25/09/2009'
__version__ = '$Revision: 1.3 $'

'''
Bs->JpsiPhi stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

from StrippingSelections import Bs2JpsiPhiPrescaled, Bs2JpsiPhiDetached
selSeqPresc = Bs2JpsiPhiPrescaled.sequence
selSeqDet = Bs2JpsiPhiDetached.sequence
############################################
# Create StrippingLine with this selection #
############################################
prescLine = StrippingLine("Bs2JpsiPhiPrescaledLine"
                          , prescale = 1.
                          , postscale = 0.1
                          , algos = [selSeqPresc]
                          , stream = "Bmuon" 
                          )
# Now do the detached
detLine = StrippingLine("Bs2JpsiPhiDetachedLine"
                        , prescale = 1.
                        , postscale = 1.
                        , algos = [selSeqDet]
                        , stream = "Bmuon" 
                        )
