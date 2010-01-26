############################################################################
#
# Stripping selection for prompt D->hh events, mass window includes D->K+K- and D->K+pi- (+cc)
#
############################################################################

__author__ = ['Marco Gersabeck']
__date__ = '03/11/2009'
__version__ = '$Revision: 1.4 $'

__all__ = ('name', 'D0', 'sequence')

from Gaudi.Configuration import *
from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

name = "D2hhNoPID"

_D0Cuts = "(BPVVDCHI2>9) & (BPVIPCHI2()<9) & (BPVDIRA>0.99975)"
_D0DauCuts = { "K+": "(PT>500) & (MIPCHI2DV(PRIMARY)>9) & (TRCHI2DOF<9)" }
_D0CombCut = "(ADAMASS(1940*MeV)<125*MeV) & (APT>1500)"

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

# Global event cuts
#from Configurables import LoKi__VoidFilter as VoidFilter
#from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
#modules = CoreFactory('CoreFactory').Modules
#for i in [ 'LoKiTrigger.decorators' ]:
#    if i not in modules: modules.append( i )
#gec_cuts = VoidFilter('gec_cuts',
#                      Code = "( VSOURCE('Rec/Vertex/Primary', NTRACKS >= 10) >> (VSIZE == 1) )\
#                              & ( TrSOURCE('Rec/Track/Best') >> (TrSIZE < 150 ) )",
#                      Preambulo = [ "from LoKiPhys.decorators import *",
#                                    "from LoKiCore.functions import *" ]
#                     )

############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine(name+"Line"
                          , prescale = 1.
                          , algos = [ sequence ]
#                          , algos = [ gec_cuts, sequence ]
#                          , HLT = HDRFilter( 'NoLumiFilter', Code="HLT_PASS_RE('Hlt1(?!Lumi).*Decision')" )
                          )
