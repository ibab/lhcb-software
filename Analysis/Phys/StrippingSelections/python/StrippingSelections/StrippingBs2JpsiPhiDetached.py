# $Id: StrippingBs2JpsiPhiDetached.py,v 1.2 2009-10-30 11:08:30 poluekt Exp $

__author__ = ['Juan Palacios']
__date__ = '01/10/2009'
__version__ = '$Revision: 1.2 $'

__all__ = ('name', 'Phi', 'Jpsi', 'Bs', 'sequence')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence

name = "Bs2JpsiPhiDetached"


from StrippingBs2JpsiPhiPrescaled import Bs

_Bs = Bs.algorithm().clone(name)
_Bs.MotherCut += "& (BPVLTIME()>0.15*ps)"

from StrippingBs2JpsiPhiPrescaled import Phi, Jpsi

Bs = Selection ( "Sel"+name,
                 Algorithm = _Bs,
                 RequiredSelections = [Phi, Jpsi])

sequence = SelectionSequence("Seq"+name, TopSelection = Bs)

from StrippingConf.StrippingLine import StrippingLine, StrippingMember

line = StrippingLine("Bs2JpsiPhiDetachedLine"
                        , prescale = 1.
                        , postscale = 1.
                        , algos = [ sequence ]
                        )
