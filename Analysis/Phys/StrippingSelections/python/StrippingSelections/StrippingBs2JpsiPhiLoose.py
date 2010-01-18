# $Id: StrippingBs2JpsiPhiLoose.py,v 1.1 2010-01-18 10:15:48 gcowan Exp $

__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '23/09/2009'
__version__ = '$Revision: 1.1 $'

'''
Bs->JpsiPhi stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

__all__ = ('name', 'Phi', 'Jpsi', 'Bs', 'sequence')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

name = "Bs2JpsiPhiLoose"

_stdPhi2KK = DataOnDemand("stdLoosePhi2KK", Location = "Phys/StdLoosePhi2KK")

_phiFilter = FilterDesktop("PhiFilterFor"+name)
_phiFilter.Code = "  (MINTREE('K+'==ABSID, TRCHI2DOF) < 10.0)" \
                  "& (MINTREE('K+'==ABSID, PT) > 250.*MeV)"\
                  "& (ADMASS('phi(1020)') < 30.*MeV)" \
                  "& (PT > 500.*MeV)" \
                  "& (VFASPF(VCHI2/VDOF) < 20.0)"

Phi =  Selection ("Phi2KKFor"+name,
                 Algorithm = _phiFilter,
                 RequiredSelections = [_stdPhi2KK])

_stdVeryLooseJpsi = DataOnDemand("stdVeryLooseJpsi", Location = "StdVeryLooseJpsi2MuMu")

_JpsiFilter = FilterDesktop('JpsiFilterFor' + name)
_JpsiFilter.Code = "  (MINTREE('mu+'==ABSID, TRCHI2DOF) < 10.0)" \
                   "& (ADMASS('J/psi(1S)') < 80.*MeV)" \
                   "& (VFASPF(VCHI2/VDOF) < 20.0)"

Jpsi = Selection("Jpsi2MuMuFor" + name,
		Algorithm = _JpsiFilter,
		RequiredSelections = [_stdVeryLooseJpsi])

_Bs = CombineParticles(name,
                       DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                       CombinationCut = "ADAMASS('B_s0') < 300.*MeV",
                       MotherCut = "(VFASPF(VCHI2/VDOF) < 20.0)",
                       ReFitPVs = True)

# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Phi
_Bs.addTool( OfflineVertexFitter() )
_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
_Bs.OfflineVertexFitter.useResonanceVertex = False

Bs = Selection ( "Sel"+name,
                 Algorithm = _Bs,
                 RequiredSelections = [Jpsi, Phi])

sequence = SelectionSequence("Seq"+name, TopSelection = Bs)

from StrippingConf.StrippingLine import StrippingLine, StrippingMember

############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine('Bs2JpsiPhiLooseLine'
               , prescale = 1
               , algos = [ sequence ]
               )
