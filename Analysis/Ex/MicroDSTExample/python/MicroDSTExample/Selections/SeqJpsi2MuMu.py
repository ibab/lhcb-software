# $Id: SeqJpsi2MuMu.py,v 1.1 2009-08-18 17:05:30 jpalac Exp $

__author__ = 'Juan Palacios'
__date__ = 'July 2009'
__version__ = '$Revision: 1.1 $'

'''
The roadmap Bs->JpsiPhi selection using LoKi::Hybrid and python
configurables. Based on unbiased selection by Greig Cowan
'''

from Jpsi2MuMu import SelJpsi2MuMu

from PhysSelPython.Wrappers import SelectionSequence

SeqJpsi2MuMu = SelectionSequence("SeqJpsi2MuMu",
                                 TopSelection = SelJpsi2MuMu)
