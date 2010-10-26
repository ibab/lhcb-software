'''
Module for construction of B->psi(2S)X
Provides functions to build B, Psi(2S), Phi selections.
Exported symbols (use python help!):
   B2Psi(2S)XLines
'''

__author__ = ['Fred Blanc']
__date__ = '26/10/2010'
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
B2Psi2SXJpsiPiPiLines = []

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
PionList = createSubSel( OutputList = "PionsForBToPsi2SXJpsiPiPi",
                         InputList = DataOnDemand(Location = "Phys/StdLoosePions"),
                         Cuts = "(TRCHI2DOF < 5)" )

JpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu")

Psi2SList = createCombinationSel( OutputList = "Psi2SToJpsiPiPiForBToPsi2SXJpsiPiPi",
                                  DaughterLists = [ JpsiList, PionList ],
                                  DaughterCuts = { "J/psi(1S)" : "MIPCHI2DV(PRIMARY) > 4" , "pi+" : "MIPCHI2DV(PRIMARY) > 4" },
                                  DecayDescriptor = "psi(2S) -> J/psi(1S) pi+ pi-",
                                  PreVertexCuts = "(AM23>400.*MeV) & (AM23<800.*MeV) &(APT>500*MeV) & (ADAMASS('psi(2S)') < 30*MeV)",
                                  PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 16)" )

KaonList = createSubSel( OutputList = "KaonsForBToPsi2SXJpsiPiPi",
                         InputList = DataOnDemand(Location = "Phys/StdLooseKaons"),
                         Cuts = "(TRCHI2DOF < 5) & (PIDK > -2)" )

PhiList = createSubSel( OutputList = "Phi2KKForBToPsi2SXJpsiPiPi",
                        InputList = DataOnDemand(Location = "Phys/StdLoosePhi2KK"),
                        Cuts = "(ADMASS('phi(1020)') < 20)" \
                        "& (PT > 500) " \
                        "& (VFASPF(VCHI2) < 16)" \
                        "& (MAXTREE('K+'==ABSID, TRCHI2DOF) < 5)" \
                        "& (MINTREE('K+'==ABSID, PIDK) > -2)" )

KstarList = createCombinationSel( OutputList = "Kstar2KpiForBToPsi2SXJpsiPiPi",
                                  DaughterLists = [ KaonList, PionList ],
                                  DaughterCuts = { },
                                  DecayDescriptor = "[K*(892)0 -> K+ pi-]cc",
                                  PreVertexCuts = "(APT>800*MeV) & (ADAMASS('K*(892)0') < 90*MeV)",
                                  PostVertexCuts = "(VFASPF(VCHI2) < 16)" )

KsList = MergedSelection("KsForBToPsi2SXJpsiPiPi",
                         RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD"),
                                               DataOnDemand(Location = "Phys/StdLooseKsLL")] )

####################
### Inlusive psi(2S). We keep it for as long as we can.
####################

Psi2SToJpsiPiPiLine =  StrippingLine("Psi2SToJpsiPiPiLine",algos = [ Psi2SList ], prescale = 1)
B2Psi2SXJpsiPiPiLines += [ Psi2SToJpsiPiPiLine ]

#####################
### Bu->Psi(2S)K+ ###
#####################
Bu2Psi2SKJpsiPiPi = createCombinationSel( OutputList = "Bu2Psi2SKJpsiPiPi",
                                          DecayDescriptor = "[B+ -> psi(2S) K+]cc",
                                          DaughterLists = [ KaonList, Psi2SList],
                                          DaughterCuts  = {"K+": "(PT > 500)" },
                                          PreVertexCuts = "in_range(5000,AM,5650)",
                                          PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )

Bu2Psi2SKJpsiPiPiLine = StrippingLine("Bu2Psi2SKJpsiPiPiLine", algos = [ Bu2Psi2SKJpsiPiPi ] , prescale = 1.00)

B2Psi2SXJpsiPiPiLines += [ Bu2Psi2SKJpsiPiPiLine]

#####################
### Bs->Psi(2S)Phi ##
#####################

Bs2Psi2SPhiJpsiPiPi = createCombinationSel( OutputList = "Bs2Psi2SPhiJpsiPiPi",
                                            DecayDescriptor = "B_s0 -> psi(2S) phi(1020)",
                                            DaughterLists  = [ PhiList, Psi2SList],
                                            PreVertexCuts = "in_range(5000,AM,5650)",
                                            PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )

Bs2Psi2SPhiJpsiPiPiLine = StrippingLine("Bs2Psi2SPhiJpsiPiPiLine", algos = [ Bs2Psi2SPhiJpsiPiPi ] , prescale = 1.00)

B2Psi2SXJpsiPiPiLines += [ Bs2Psi2SPhiJpsiPiPiLine]


#####################
### B0->Psi(2S)K*  ##
#####################

Bd2Psi2SKstarJpsiPiPi = createCombinationSel( OutputList = "Bd2Psi2SKstarJpsiPiPi",
                                              DecayDescriptor = "[B0 -> psi(2S) K*(892)0]cc",
                                              DaughterLists  = [ KstarList, Psi2SList ],
                                              PreVertexCuts = "in_range(5000,AM,5650)",
                                              PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )

Bd2Psi2SKstarJpsiPiPiLine = StrippingLine("Bd2Psi2SKstarJpsiPiPiLine", algos = [ Bd2Psi2SKstarJpsiPiPi ] , prescale = 1.00)

B2Psi2SXJpsiPiPiLines += [ Bd2Psi2SKstarJpsiPiPiLine]


#####################
### Bd->Psi(2S)KS ###
#####################
Bd2Psi2SKsJpsiPiPi = createCombinationSel( OutputList = "Bd2Psi2SKSJpsiPiPi",
                                           DecayDescriptor = "B0 -> psi(2S) KS0",
                                           DaughterLists  = [ KsList, Psi2SList ],
                                           PreVertexCuts = "in_range(5000,AM,5650)",
                                           PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)")


Bd2Psi2SKsJpsiPiPiLine = StrippingLine("Bd2Psi2SKsJpsiPiPiLine", algos = [ Bd2Psi2SKsJpsiPiPi ] , prescale = 1.00)

B2Psi2SXJpsiPiPiLines += [ Bd2Psi2SKsJpsiPiPiLine]
