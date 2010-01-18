# $Id: StrippingBu2JpsiKLoose.py,v 1.1 2010-01-18 10:15:48 gcowan Exp $

__author__ = ['Greig Cowan']
__date__ = '15/01/2010'
__version__ = '$Revision: 1.1 $'

'''
Bu->JpsiK stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

__all__ = ('name', 'Jpsi', 'Bu', 'sequence')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

name = "Bu2JpsiKLoose"

_stdVeryLooseJpsi = DataOnDemand("stdVeryLooseJpsi", Location = "StdVeryLooseJpsi2MuMu")
_stdLooseKaons = DataOnDemand("stdLooseKaons", Location = "StdLooseKaons")

_JpsiFilter = FilterDesktop('JpsiFilterFor' + name)
_JpsiFilter.Code = "  (MINTREE('mu+'==ABSID, TRCHI2DOF) < 10.0)" \
		   "& (MINTREE('mu+'==ABSID, PT) > 250.*MeV)" \
                   "& (PT > 500.*MeV)" \
                   "& (ADMASS('J/psi(1S)') < 80.*MeV)" \
                   "& (VFASPF(VCHI2/VDOF) < 20.0)"

Jpsi = Selection("Jpsi2MuMuFor" + name,
		Algorithm = _JpsiFilter,
		RequiredSelections = [_stdVeryLooseJpsi])

_Bu = CombineParticles(name,
                       DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
		       DaughtersCuts = {"K+": "(PT > 250.*MeV) & (TRCHI2DOF < 10.0)"},
                       CombinationCut = "ADAMASS('B+') < 300.*MeV",
                       MotherCut = "(VFASPF(VCHI2/VDOF) < 20.0)",
                       ReFitPVs = True)

Bu = Selection ( "Sel"+name,
                 Algorithm = _Bu,
                 RequiredSelections = [Jpsi, _stdLooseKaons])

sequence = SelectionSequence("Seq"+name, TopSelection = Bu)

from StrippingConf.StrippingLine import StrippingLine, StrippingMember

############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine('Bu2JpsiKLooseLine'
               , prescale = 1
               , algos = [ sequence ]
               )
