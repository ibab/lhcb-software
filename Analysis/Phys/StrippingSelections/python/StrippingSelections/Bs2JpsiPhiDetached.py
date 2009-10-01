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

__all__ = (name, Phi, Jpsi, Bs, sequence)
