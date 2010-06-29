# $Id: StrippingBs2JpsiPhiDetached.py,v 1.3 2010-06-29 15:18:51 jpalac Exp $

__author__ = ['Juan Palacios']
__date__ = '01/10/2009'
__version__ = '$Revision: 1.3 $'

__all__ = ('name', 'Phi', 'Jpsi', 'Bs')

from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine

name = "Bs2JpsiPhiDetached"


from StrippingBs2JpsiPhiPrescaled import Bs

_Bs = Bs.algorithm().clone(name)
_Bs.MotherCut += "& (BPVLTIME()>0.15*ps)"

from StrippingBs2JpsiPhiPrescaled import Phi, Jpsi

Bs = Selection ( "Sel"+name,
                 Algorithm = _Bs,
                 RequiredSelections = [Phi, Jpsi])



line = StrippingLine("Bs2JpsiPhiDetachedLine"
                        , prescale = 1.
                        , postscale = 1.
                        , algos = [ Bs ]
                        )
