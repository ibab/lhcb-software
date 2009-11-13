############################################################################
#
# Stripping selection for prompt D->hh events, mass window includes D->K+K- and D->K+pi- (+cc)
# Current default selection is very loose
# _tight_sel = True switches to a selection close to the offline one
#
############################################################################

__author__ = ['Marco Gersabeck']
__date__ = '03/11/2009'
__version__ = '$Revision: 1.2 $'

__all__ = ('name', 'D0', 'sequence')

from Gaudi.Configuration import *
from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

name = "D2hhNoPID"

_tight_sel = False

_D0Cuts = "(MIPDV(PRIMARY)<0.3) & (BPVDIRA>0.999)"
_D0DauCuts = { "K+": "(PT>400) & (MIPDV(PRIMARY)>0.04)" }
_D0CombCut = "(ADAMASS(1975*MeV)<175*MeV) & (APT>1200)"

if _tight_sel:
    _D0Cuts = "(BPVIPCHI2()<25) & (BPVDIRA>0.9995)"
    _D0DauCuts = { "K+": "(PT>500) & (MIPCHI2DV(PRIMARY)>4)" }
    _D0CombCut = "(ADAMASS(1975*MeV)<175*MeV) & (APT>1500)"

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
#from Configurables import LoKi__HDRFilter as HDRFilter
#HDRFilter( 'NoLumiFilter', Code="HLT_PASS_RE('Hlt1(?!Lumi).*Decision')" )

line = StrippingLine(name+"Line"
                          , prescale = 1.
                          , algos = [ sequence ]
#                          , HLT = HDRFilter( 'NoLumiFilter', Code="HLT_PASS_RE('Hlt1(?!Lumi).*Decision')" )
                          )
