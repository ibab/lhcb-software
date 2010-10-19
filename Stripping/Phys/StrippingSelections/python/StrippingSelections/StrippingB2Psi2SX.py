'''
Module for construction of B->psi(2S)X
Provides functions to build B, Psi(2S), Phi selections.
Exported symbols (use python help!):
   B2Psi(2S)XLines
'''

__author__ = ['Fred Blanc']
__date__ = '03/10/2010'
__version__ = '$Revision: 1.0 $'


# Need __all__ for ???
__all__ = ('Lines')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig
from GaudiKernel.SystemOfUnits import MeV

##############################################################################
## This is the common Jpsi that is used in all of the subsequent selections ##
##############################################################################
#Jpsi = DataOnDemand(Location = "Phys/StdLTUnbiasedJpsi2MuMu")


# a few functions that we will use. are there alternatives for this in Juan's framework?
B2Psi2SXLines = []

# create a selection using a FilterDesktop
def createSubSel( OutputList, InputList, Cuts ) :
    # create a unique filterdesktop
    filterName = "FilterDesktopFor" + OutputList
    if allConfigurables.get( filterName ) :
        raise ValueError, 'FilterDesktop instance with name '+filterName+' already exists'
    filter = FilterDesktop(filterName, Code = Cuts)
    return Selection( OutputList,
                      Algorithm = filter,
                      RequiredSelections = [ InputList ] )

# create a selection using a ParticleCombiner
def createCombinationSel( OutputList,
                          DecayDescriptor,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL" ) :
    combinerName = "CombinerParticlesFor" + OutputList
    if allConfigurables.get( combinerName ) :
        raise ValueError, 'CombineParticles instance with name '+ combinerName+' already exists'
    combiner = CombineParticles( combinerName,
                                 DecayDescriptor = DecayDescriptor,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = True)
    combiner.addTool( OfflineVertexFitter() )
    combiner.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    combiner.OfflineVertexFitter.useResonanceVertex = True
    return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)


# define input daughter lists for various B -> J/psi X selections
PionList = createSubSel( OutputList = "PionsForBToPsi2SX",
                         InputList = DataOnDemand(Location = "Phys/StdLoosePions"),
                         Cuts = "(TRCHI2DOF < 5)" )

JpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu")

Psi2SList = createCombinationSel( OutputList = "Psi2SToJpsiPiPiForBToPsi2SX",
                                  DaughterLists = [ JpsiList, PionList ],
                                  DaughterCuts = { "J/psi(1S)" : "MIPCHI2DV(PRIMARY) > 4" , "pi+" : "MIPCHI2DV(PRIMARY) > 4" },
                                  DecayDescriptor = "[psi(2S) -> J/psi(1S) pi+ pi-]cc",
                                  PreVertexCuts = "(AM23>400.*MeV) & (AM23<800.*MeV) &(APT>500*MeV) & (ADAMASS('psi(2S)') < 30*MeV)",
                                  PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 16)" )

KaonList = createSubSel( OutputList = "KaonsForBToPsi2SX",
                         InputList = DataOnDemand(Location = "Phys/StdLooseKaons"),
                         Cuts = "(TRCHI2DOF < 5) & (PIDK > -5)" )

PhiList = createSubSel( OutputList = "Phi2KKForBToPsi2SX",
                        InputList = DataOnDemand(Location = "Phys/StdLoosePhi2KK"),
                        Cuts = "(ADMASS('phi(1020)') < 20)" \
                        "& (PT > 500) " \
                        "& (VFASPF(VCHI2) < 16)" \
                        "& (MAXTREE('K+'==ABSID, TRCHI2DOF) < 10)" \
                        "& (MINTREE('K+'==ABSID, PIDK) > -5)" )

KstarList = createCombinationSel( OutputList = "Kstar2KpiForBToPsi2SX",
                                  DaughterLists = [ KaonList, PionList ],
                                  DaughterCuts = { },
                                  DecayDescriptor = "[K*(892)0 -> K+ pi-]cc",
                                  PreVertexCuts = "(APT>500*MeV) & (ADAMASS('K*(892)0') < 90*MeV)",
                                  PostVertexCuts = "(VFASPF(VCHI2) < 16)" )

KsList = MergedSelection("KsForBToPsi2SX",
                         RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD"),
                                               DataOnDemand(Location = "Phys/StdLooseKsLL")] )

####################
### Inlusive psi(2S). We keep it for as long as we can.
####################

Psi2SToJpsiPiPiLine =  StrippingLine("Psi2SToJpsiPiPiLine",algos = [ Psi2SList ], prescale = 1)
B2Psi2SXLines += [ Psi2SToJpsiPiPiLine ]

#####################
### Bu->Psi(2S)K+ ###
#####################
Bu2Psi2SK = createCombinationSel( OutputList = "Bu2Psi2SK",
                                 DecayDescriptor = "[B+ -> psi(2S) K+]cc",
                                 DaughterLists = [ KaonList, Psi2SList],
                                 DaughterCuts  = {"K+": "(PT > 500)" },
                                 PreVertexCuts = "ADAMASS('B+') < 300",
                                 PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 10)" )

Bu2Psi2SKPrescaledLine = StrippingLine("Bu2Psi2SKPrescaledLine", algos = [ Bu2Psi2SK ] , prescale = 1.00)
Bu2Psi2SKDetachedLine  = StrippingLine("Bu2Psi2SKDetachedLine",
                                      algos = [ createSubSel( InputList = Bu2Psi2SK,
                                                              OutputList = Bu2Psi2SK.name() + "Detached",
                                                              Cuts = "(BPVLTIME()>0.15*ps)" ) ] )
Bu2Psi2SKUnbiasedLine  = StrippingLine("Bu2Psi2SKUnbiasedLine",
                                      algos = [ createSubSel( InputList = Bu2Psi2SK,
                                                              OutputList = Bu2Psi2SK.name() + "Unbiased",
                                                              Cuts = "(MINTREE('K+'==ABSID, PT) > 1000.*MeV)") ] )

B2Psi2SXLines += [ Bu2Psi2SKPrescaledLine, Bu2Psi2SKDetachedLine, Bu2Psi2SKUnbiasedLine]

#####################
### Bs->Psi(2S)Phi ##
#####################

Bs2Psi2SPhi = createCombinationSel( OutputList = "Bs2Psi2SPhi",
                                   DecayDescriptor = "B_s0 -> psi(2S) phi(1020)",
                                   DaughterLists  = [ PhiList, Psi2SList],
                                   PreVertexCuts = "ADAMASS('B_s0') < 300",
                                   PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 10)" )

Bs2Psi2SPhiPrescaledLine = StrippingLine("Bs2Psi2SPhiPrescaledLine", algos = [ Bs2Psi2SPhi ] , prescale = 1.00)

Bs2Psi2SPhiDetachedLine  = StrippingLine("Bs2Psi2SPhiDetachedLine",
                                        algos = [ createSubSel( InputList = Bs2Psi2SPhi,
                                                                OutputList = Bs2Psi2SPhi.name() + "Detached",
                                                                Cuts = "(BPVLTIME()>0.15*ps)" )] )

Bs2Psi2SPhiUnbiasedLine =  StrippingLine("Bs2Psi2SPhiUnbiasedLine",
                                        algos = [ createSubSel( InputList = Bs2Psi2SPhi,
                                                                OutputList = Bs2Psi2SPhi.name() + "Unbiased",
                                                                Cuts = "(MINTREE('phi(1020)'==ABSID, PT) > 1000.*MeV)") ] )

B2Psi2SXLines += [ Bs2Psi2SPhiPrescaledLine, Bs2Psi2SPhiDetachedLine, Bs2Psi2SPhiUnbiasedLine ]


#####################
### B0->Psi(2S)K*  ##
#####################

Bd2Psi2SKstar = createCombinationSel( OutputList = "Bd2Psi2SKstar",
                                     DecayDescriptor = "[B0 -> psi(2S) K*(892)0]cc",
                                     DaughterLists  = [ KstarList, Psi2SList ],
                                     PreVertexCuts = "ADAMASS('B0') < 300",
                                     PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 10)" )

Bd2Psi2SKstarPrescaledLine = StrippingLine("Bd2Psi2SKstarPrescaledLine", algos = [ Bd2Psi2SKstar ] , prescale = 0.25)
Bd2Psi2SKstarDetachedLine  = StrippingLine("Bd2Psi2SKstarDetachedLine",
                                          algos = [ createSubSel( InputList = Bd2Psi2SKstar,
                                                                  OutputList = Bd2Psi2SKstar.name() + "Detached",
                                                                  Cuts = "(BPVLTIME()>0.15*ps)" ) ] )
Bd2Psi2SKstarUnbiasedLine  = StrippingLine("Bd2Psi2SKstarUnbiasedLine",
                                          algos = [ createSubSel( InputList = Bd2Psi2SKstar,
                                                                  OutputList = Bd2Psi2SKstar.name() + "Unbiased",
                                                                  Cuts = "(PT>2.*GeV) " \
                                                                  "& (MINTREE('K*(892)0'==ABSID, PT)> 1000.*MeV)") ] )
B2Psi2SXLines += [ Bd2Psi2SKstarPrescaledLine, Bd2Psi2SKstarDetachedLine, Bd2Psi2SKstarUnbiasedLine]


#####################
### Bd->Psi(2S)KS ###
#####################
Bd2Psi2SKs = createCombinationSel( OutputList = "Bd2Psi2SKS",
                                  DecayDescriptor = "B0 -> psi(2S) KS0",
                                  DaughterLists  = [ KsList, Psi2SList ],
                                  PreVertexCuts = "ADAMASS('B0') < 500",
                                  PostVertexCuts = "ALL" #"(VFASPF(VCHI2/VDOF) < 20)"
                                  )

Bd2Psi2SKsPrescaledLine = StrippingLine("Bd2Psi2SKsPrescaledLine", algos = [ Bd2Psi2SKs ] , prescale = 1.00)
Bd2Psi2SKsDetachedLine  = StrippingLine("Bd2Psi2SKsDetachedLine",
                                       algos = [ createSubSel( InputList = Bd2Psi2SKs,
                                                               OutputList = Bd2Psi2SKs.name() + "Detached",
                                                               Cuts = "(BPVLTIME()>0.15*ps)" )] )
Bd2Psi2SKsUnbiasedLine  = StrippingLine("Bd2Psi2SKsUnbiasedLine",
                                       algos = [ createSubSel( InputList = Bd2Psi2SKs,
                                                               OutputList = Bd2Psi2SKs.name() + "Unbiased",
                                                               Cuts = "(MINTREE('KS0'==ABSID, PT) > 1000.*MeV)") ] )
B2Psi2SXLines += [ Bd2Psi2SKsPrescaledLine, Bd2Psi2SKsDetachedLine, Bd2Psi2SKsUnbiasedLine ]
