# $Id: StrippingBd2JpsiKstarLoose.py,v 1.1 2010-01-18 10:15:48 gcowan Exp $

__author__ = ['Greig Cowan']
__date__ = '15/01/2010'
__version__ = '$Revision: 1.1 $'

'''
Bd->JpsiKstar stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

__all__ = ('name', 'Kstar', 'Jpsi', 'Bd', 'sequence')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

name = "Bd2JpsiKstarLoose"

_stdKstar2Kpi = DataOnDemand("stdLooseKstar2Kpi", Location = "Phys/StdLooseKstar2Kpi")

_kstarFilter = FilterDesktop("KstarFilterFor"+name)
_kstarFilter.Code = "  (MINTREE('K+'==ABSID, TRCHI2DOF) < 10.0)" \
                    "& (MINTREE('K+'==ABSID, PT) > 250.*MeV)"\
		    "& (MINTREE('pi-'==ABSID, TRCHI2DOF) < 10.0)" \
                    "& (MINTREE('pi-'==ABSID, PT) > 250.*MeV)"\
                    "& (ADMASS('K*(892)0') < 300.*MeV)" \
                    "& (PT > 500.*MeV)" \
                    "& (VFASPF(VCHI2/VDOF) < 20.0)"

Kstar =  Selection ("Kstar2KpiFor"+name,
                 Algorithm = _kstarFilter,
                 RequiredSelections = [_stdKstar2Kpi])

_stdVeryLooseJpsi = DataOnDemand("stdVeryLooseJpsi", Location = "StdVeryLooseJpsi2MuMu")

_JpsiFilter = FilterDesktop('JpsiFilterFor' + name)
_JpsiFilter.Code = "  (MINTREE('mu+'==ABSID, TRCHI2DOF) < 10.0)" \
		   "& (MINTREE('mu+'==ABSID, PT) > 250.*MeV)" \
                   "& (PT > 500.*MeV)" \
                   "& (ADMASS('J/psi(1S)') < 80.*MeV)" \
                   "& (VFASPF(VCHI2/VDOF) < 20.0)"

Jpsi = Selection("Jpsi2MuMuFor" + name,
		Algorithm = _JpsiFilter,
		RequiredSelections = [_stdVeryLooseJpsi])

_Bd = CombineParticles(name,
                       DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                       CombinationCut = "ADAMASS('B0') < 300.*MeV",
                       MotherCut = "(VFASPF(VCHI2/VDOF) < 20.0)",
                       ReFitPVs = True)

# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Phi
_Bd.addTool( OfflineVertexFitter() )
_Bd.VertexFitters.update( { "" : "OfflineVertexFitter"} )
_Bd.OfflineVertexFitter.useResonanceVertex = False

Bd = Selection ( "Sel"+name,
                 Algorithm = _Bd,
                 RequiredSelections = [Jpsi, Kstar])

sequence = SelectionSequence("Seq"+name, TopSelection = Bd)

from StrippingConf.StrippingLine import StrippingLine, StrippingMember

############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine('Bd2JpsiKstarLooseLine'
               , prescale = 1
               , algos = [ sequence ]
               )
