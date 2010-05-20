############################################################################
#
# Stripping selection for prompt D->hh events, mass window includes D->K+K- and D->K+pi- (+cc)
# This version is optimised for x-section measurements
#
############################################################################

__author__ = ['Marco Gersabeck']
__date__ = '26/04/2010'
__version__ = '$Revision: 1.1 $'

__all__ = ('name', 'D0', 'sequence')

from Gaudi.Configuration import *
from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

name = "D2hhNoPIDXsec"

_D0DauCuts = { "K+": "ISLONG & (PT>700) & (TRCHI2DOF<9) & (MIPCHI2DV(PRIMARY)>9)" }
_D0CombCut = "(ADAMASS(1940*MeV)<250*MeV)"
_D0Cuts = "(ADMASS(1940*MeV)<125*MeV) & (BPVVDCHI2>9) & (BPVIPCHI2()<9) & (BPVDIRA>0.9999)"

_D0 = CombineParticles( name,
                        DecayDescriptor = "D0 -> K- K+",
                        MotherCut = _D0Cuts,
                        DaughtersCuts = _D0DauCuts,
                        CombinationCut = _D0CombCut
                      )

_stdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons" )

D0 = Selection ( "Sel"+name,
                 Algorithm = _D0,
                 RequiredSelections = [_stdNoPIDsKaons]
               )

sequence = SelectionSequence("Seq"+name, TopSelection = D0)

############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine(name+"Line"
                          , prescale = 1.
                          , algos = [ sequence ]
                          )
