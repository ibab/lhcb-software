# $Id: Bs2JpsiPhiDetached.py,v 1.2 2009-10-02 07:37:27 jpalac Exp $

__author__ = ['Juan Palacios']
__date__ = '01/10/2009'
__version__ = '$Revision: 1.2 $'

__all__ = ('name', 'Phi', 'Jpsi', 'Bs', 'sequence')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

name = "Bs2JpsiPhiDetached"


from Bs2JpsiPhiPrescaled import Bs

_Bs = Bs.algorithm().clone(name)
_Bs.MotherCut += "& (BPVLTIME()>0.15*ps)"

from Bs2JpsiPhiPrescaled import Phi, Jpsi

Bs = Selection ( "Sel"+name,
                 Algorithm = _Bs,
                 RequiredSelections = [Phi, Jpsi])
Bs.__apply_configuration__()

sequence = SelectionSequence("Seq"+name, TopSelection = Bs)
sequence.__apply_configuration__()

