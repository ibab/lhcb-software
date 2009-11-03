############################################################################
#
# Stripping selection for prompt D->hh events, mass window includes D->K+K- and D->K+pi- (+cc)
# Current default selection is very loose (starting point for tuning),
# _tight_sel = True switches to a selection close to the offline one
#
############################################################################

__author__ = ['Marco Gersabeck']
__date__ = '03/11/2009'
__version__ = '$Revision: 1.1 $'

__all__ = ('name', 'D0', 'sequence')

from Gaudi.Configuration import *
from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

name = "D2hhNoPID"

_tight_sel = False

_D0Cuts = "(MIPDV(PRIMARY)<0.2) & (BPVDIRA>0.999)"
_D0DauCuts = { "K+": "(PT>300) & (MIPDV(PRIMARY)>0.05)" }
_D0CombCut = "(ADAMASS(1950*MeV)<250*MeV) & (APT>1000)"

if _tight_sel:
    _D0Cuts = "(VFASPF(VCHI2/VDOF)<10) & (BPVVDCHI2>16) & (BPVIPCHI2()<9) & (BPVDIRA>0.999)"
    _D0DauCuts = { "K+": "(PT>500) & (MIPCHI2DV(PRIMARY)>16)" }
    _D0CombCut = "(ADAMASS(1950*MeV)<250*MeV) & (APT>1500) & (AP>5000)"

_D0 = CombineParticles( name,
                        DecayDescriptor = "D0 -> K- K+",
                        MotherCut = _D0Cuts,
                        DaughtersCuts = _D0DauCuts,
                        CombinationCut = _D0CombCut,
                        InputLocations = [ "Phys/StdNoPIDsKaons" ]
                      )

D0 = Selection ( "Sel"+name,
                 Algorithm = _D0
               )

sequence = SelectionSequence("Seq"+name, TopSelection = D0)

############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine(name+"Line"
                          , prescale = 1.
                          , algos = [ sequence ]
                          )
