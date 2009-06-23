# $Id: StrippingBs2JpsiPhiPrescaledAndDetached.py,v 1.1 2009-06-23 23:07:32 gcowan Exp $

'''
Bs -> J/psi Phi prescaled and detached stripping selection
'''
__author__ = "Greig Cowan"
__date__ = "2009-06-22"
__version__ = "$Revision: 1.1 $"

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles

# Make the J/psi
# CommonParticles now have this very basic J/psi
#jPsiCuts = "(ADMASS('J/psi(1S)')<100*MeV)"
#jPsiCombine = StrippingMember( CombineParticles
#                               , "JpsiCombine"
#                               , DecayDescriptor = "J/psi(1S) -> mu+ mu-"
#                               , InputLocations = ["StdLooseMuons"]
#                               , MotherCut = jPsiCuts
#                               )

# Filter the phi
phiFilter = FilterDesktop("PhiFilter")
phiFilter.InputLocations = ["StdLoosePhi2KK"]
phiFilter.Code = "(PT>500*MeV)"

# Make the Bs
BsCuts = "(ADMASS('B_s0')<300*MeV) & (BPVLTFITCHI2()<14) & (VFASPF(VCHI2)<20)"
BsCombine = StrippingMember( CombineParticles
                             , "BsCombine"
                             , DecayDescriptor = "[B_s0 -> J/psi(1S) phi(1020)]cc"
                             , InputLocations = ["StdLooseJpsi2MuMu", "PhiFilter"]
                             , MotherCut = BsCuts
                             )

line = StrippingLine("Bs2JpsiPhiPrescaledLine"
                     , prescale = 1.
                     , postscale = 0.1
                     , algos = [phiFilter, BsCombine]
                     , stream = "BExclusive" 
		    )

# Now do the detached
BsCutsDetached = BsCuts + "& (BPVLTIME()>0.15*ps)"
line.clone("Bs2JpsiPhiDetachedLine"
           , prescale = 1.
           , postscale = 1.
           , CombineParticlesBsCombine = {"MotherCut": BsCutsDetached}
           )
