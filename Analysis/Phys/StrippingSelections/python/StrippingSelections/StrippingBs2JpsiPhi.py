# $Id: StrippingBs2JpsiPhi.py,v 1.2 2009-10-30 11:08:30 poluekt Exp $

__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '23/09/2009'
__version__ = '$Revision: 1.2 $'

'''
Bs->JpsiPhi stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

__all__ = ('name', 'Phi', 'Jpsi', 'Bs', 'sequence')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

name = "Bs2JpsiPhi"

_stdPhi2KK = DataOnDemand("stdLoosePhi2KK", Location = "Phys/StdLoosePhi2KK")

_phiFilter = FilterDesktop("PhiFilterFor"+name)
_phiFilter.Code = "  (MINTREE('K+'==ABSID, PIDK) > -5.0)" \
                  "& (MINTREE('K+'==ABSID, PT) > 500.0)"\
                  "& (ADMASS('phi(1020)') < 15.*MeV)" \
                  "& (PT > 1000.*MeV)" \
                  "& (VFASPF(VCHI2/VDOF) < 20.0)"

Phi =  Selection ("Phi2KKFor"+name,
                 Algorithm = _phiFilter,
                 RequiredSelections = [_stdPhi2KK])

Jpsi = DataOnDemand("stdLTUJpsi2MuMu", Location = "StdLTUnbiasedJpsi2MuMu")

_Bs = CombineParticles(name,
                       DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                       CombinationCut = "ADAMASS('B_s0') < 300.*MeV",
                       MotherCut = "(VFASPF(VCHI2/VDOF) < 5.) & (BPVIPCHI2() < 25.)",
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
line = StrippingLine('Bs2JpsiPhiLine'
               , prescale = 1
               , algos = [ sequence ]
               )
