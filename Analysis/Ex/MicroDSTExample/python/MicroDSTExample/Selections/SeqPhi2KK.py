# $Id: SeqPhi2KK.py,v 1.2 2009-08-19 19:01:40 jpalac Exp $

__author__ = 'Juan Palacios'
__date__ = 'July 2009'
__version__ = '$Revision: 1.2 $'

'''
The roadmap Bs->JpsiPhi selection using LoKi::Hybrid and python
configurables. Based on unbiased selection by Greig Cowan
'''

from Phi2KK import SelPhi2KK

from PhysSelPython.Wrappers import SelectionSequence

SeqPhi2KK = SelectionSequence("Seq_Phi2KK",
                              TopSelection = SelPhi2KK)
