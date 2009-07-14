# $Id: SeqBs2Jpsi2MuMuPhi2KK.py,v 1.1 2009-07-14 10:14:31 jpalac Exp $

__author__ = 'Juan Palacios'
__date__ = 'July 2009'
__version__ = '$Revision: 1.1 $'

'''
The roadmap Bs->JpsiPhi selection using LoKi::Hybrid and python
configurables. Based on unbiased selection by Greig Cowan
'''

from Bs2Jpsi2MuMuPhi2KK import SelBs2Jpsi2MuMuPhi2KK

from PhysSelPython.Wrappers import SelectionSequence

SeqBs2Jpsi2MuMuPhi2KK = SelectionSequence(SelBs2Jpsi2MuMuPhi2KK)
