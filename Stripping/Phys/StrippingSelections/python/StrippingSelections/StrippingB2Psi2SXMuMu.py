'''
Module for construction of B->Psi(2S)X lifetime unbiased 
stripping Selections and StrippingLine.
Provides functions to build Bs, Psi(2S), Phi selections.
Exported symbols (use python help!):
   B2Psi2SXMuMuLines
'''

__author__ = ['Neus Lopez March']
__date__ = '21/10/2010'
__version__ = '$Revision: 1.0 $'


# Need __all__ for ???
__all__ = ('Lines')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig
from GaudiKernel.SystemOfUnits import MeV


# a few functions that we will use. are there alternatives for this in Juan's framework?
B2Psi2SXMuMuLines = []

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
    return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)


# define input daughter lists for various B -> Psi(2S) X selections

Psi2SList = createCombinationSel( OutputList = "Psi2SToMuMuForBToPsi2SX",
                                  DaughterLists = [ DataOnDemand(Location = "Phys/StdLooseMuons") ],
                                  DaughterCuts = {"mu+":"PIDmu>0","mu-":"PIDmu>0" },
                                  DecayDescriptor = "psi(2S) -> mu+ mu-",
                                  PreVertexCuts = "(ADAMASS('psi(2S)') < 60.*MeV) & (ADOCACHI2CUT(30.,'')) ",
                                  PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 16)  & (MFIT)" )


KaonList = createSubSel( OutputList = "KaonsForBToPsi2SXMuMu",
                         InputList = DataOnDemand(Location = "Phys/StdLooseKaons"),
                         Cuts = "(TRCHI2DOF < 5) & (PIDK > -2)" )


PhiList = createSubSel( OutputList = "Phi2KKForBToPsi2SXMuMu",
                        InputList = DataOnDemand(Location = "Phys/StdLoosePhi2KK"),
                        Cuts = "(ADMASS('phi(1020)') < 20)" \
                        "& (PT > 500) " \
                        "& (VFASPF(VCHI2) < 16)" \
                        "& (MAXTREE('K+'==ABSID, TRCHI2DOF) <5)" \
                        "& (MINTREE('K+'==ABSID, PIDK) > -2)" )

KstarList = createCombinationSel( OutputList = "Kstar2KpiForBToPsi2SXMuMu",
                                  DaughterLists = [ KaonList,
                                                    DataOnDemand(Location = "Phys/StdLoosePions") ],
                                  DaughterCuts = { "pi+" : "(TRCHI2DOF < 5)"},
                                  DecayDescriptor = "[K*(892)0 -> K+ pi-]cc",
                                  PreVertexCuts = "(APT>500*MeV) & (ADAMASS('K*(892)0') < 90*MeV)",
                                  PostVertexCuts = "(VFASPF(VCHI2) < 16)" )

KsListLoose = MergedSelection("StdLooseKsMerged",
                              RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD"),
                                                    DataOnDemand(Location = "Phys/StdLooseKsLL")] )


KsList = createSubSel(OutputList = "KsForBToPsi2SXMuMu",
                      InputList = KsListLoose, Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)")



####################
### Inlusive psi(2S). We keep it for as long as we can.
####################

Psi2SToMuMuLine =  StrippingLine("Psi2SToMuMuLine",algos = [ Psi2SList ], prescale = 0.5)
Psi2SToMuMuDetachedLine = StrippingLine("Psi2SToMuMuDetachedLine",
                                              algos = [ createSubSel( InputList = Psi2SList,
                                                                      OutputList = Psi2SList.name() + "Detached",
                                                                      Cuts = "(BPVLTIME()>0.15*ps)" ) ] )
B2Psi2SXMuMuLines += [ Psi2SToMuMuLine,Psi2SToMuMuDetachedLine ]

#####################
### Bu->Psi(2S)K+ ###
#####################
Bu2Psi2SKMuMu = createCombinationSel( OutputList = "Bu2Psi2SKMuMu",
                                 DecayDescriptor = "[B+ -> psi(2S) K+]cc",
                                 DaughterLists = [ Psi2SList, KaonList ],
                                 DaughterCuts  = {"K+": "(PT > 500)" },
                                 PreVertexCuts = "in_range(5000,AM,5650)",
                                 PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )

Bu2Psi2SKMuMuPrescaledLine = StrippingLine("Bu2Psi2SKMuMuPrescaledLine", algos = [ Bu2Psi2SKMuMu ] , prescale = 1.00)
Bu2Psi2SKMuMuDetachedLine  = StrippingLine("Bu2Psi2SKMuMuDetachedLined",
                                           algos = [ createSubSel( InputList = Bu2Psi2SKMuMu,
                                                                   OutputList = Bu2Psi2SKMuMu.name() + "Detached",
                                                                   Cuts = "(BPVLTIME()>0.15*ps)" ) ] )
Bu2Psi2SKMuMuUnbiasedLine  = StrippingLine("Bu2Psi2SKMuMuUnbiasedLine",
                                           algos = [ createSubSel( InputList = Bu2Psi2SKMuMu,
                                                                   OutputList = Bu2Psi2SKMuMu.name() + "Unbiased",
                                                                   Cuts = "(MINTREE('K+'==ABSID, PT) > 1000.*MeV)") ] )

B2Psi2SXMuMuLines += [ Bu2Psi2SKMuMuPrescaledLine, Bu2Psi2SKMuMuDetachedLine, Bu2Psi2SKMuMuUnbiasedLine]

#####################
### Bs->Psi(2S)Phi ##
#####################

Bs2Psi2SPhiMuMu = createCombinationSel( OutputList = "Bs2Psi2SPhiMuMu",
                                        DecayDescriptor = "B_s0 -> psi(2S) phi(1020)",
                                        DaughterLists  = [ Psi2SList, PhiList ],
                                        PreVertexCuts = "in_range(5000,AM,5650)",
                                        PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )

Bs2Psi2SPhiMuMuPrescaledLine = StrippingLine("Bs2Psi2SPhiMuMuPrescaledLine", algos = [ Bs2Psi2SPhiMuMu ] , prescale = 1.00)

Bs2Psi2SPhiMuMuDetachedLine  = StrippingLine("Bs2Psi2SPhiMuMuDetachedLine",
                                             algos = [ createSubSel( InputList = Bs2Psi2SPhiMuMu,
                                                                     OutputList = Bs2Psi2SPhiMuMu.name() + "Detached",
                                                                     Cuts = "(BPVLTIME()>0.15*ps)" )] )

Bs2Psi2SPhiMuMuUnbiasedLine =  StrippingLine("Bs2Psi2SPhiMuMuUnbiasedLine",
                                             algos = [ createSubSel( InputList = Bs2Psi2SPhiMuMu,
                                                                     OutputList = Bs2Psi2SPhiMuMu.name() + "Unbiased",
                                                                     Cuts = "(MINTREE('phi(1020)'==ABSID, PT) > 1000.*MeV)") ] )

B2Psi2SXMuMuLines += [ Bs2Psi2SPhiMuMuPrescaledLine, Bs2Psi2SPhiMuMuDetachedLine, Bs2Psi2SPhiMuMuUnbiasedLine ]

#####################
### B0->Psi(2S)K*  ##
#####################

Bd2Psi2SKstarMuMu = createCombinationSel( OutputList = "Bd2Psi2SKstarMuMu",
                                          DecayDescriptor = "[B0 -> psi(2S) K*(892)0]cc",
                                          DaughterLists  = [ Psi2SList, KstarList ],
                                          PreVertexCuts = "in_range(5000,AM,5650)",
                                          PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )

Bd2Psi2SKstarMuMuPrescaledLine = StrippingLine("Bd2Psi2SKstarMuMuPrescaledLine", algos = [Bd2Psi2SKstarMuMu  ] , prescale = 0.5)
Bd2Psi2SKstarMuMuDetachedLine  = StrippingLine("Bd2Psi2SKstarMuMuDetachedLine",
                                               algos = [ createSubSel( InputList = Bd2Psi2SKstarMuMu,
                                                                       OutputList = Bd2Psi2SKstarMuMu.name() + "Detached",
                                                                       Cuts = "(BPVLTIME()>0.15*ps)" ) ] )
Bd2Psi2SKstarMuMuUnbiasedLine  = StrippingLine("Bd2Psi2SKstarMuMuUnbiasedLine",
                                               algos = [ createSubSel( InputList = Bd2Psi2SKstarMuMu,
                                                                       OutputList = Bd2Psi2SKstarMuMu.name() + "Unbiased",
                                                                       Cuts = "(PT>2.*GeV) " \
                                                                       "& (MINTREE('K*(892)0'==ABSID, PT)> 1000.*MeV)") ] )
B2Psi2SXMuMuLines += [ Bd2Psi2SKstarMuMuPrescaledLine, Bd2Psi2SKstarMuMuDetachedLine, Bd2Psi2SKstarMuMuUnbiasedLine]

#####################
### Bd->Psi(2S)KS ###
#####################

Bd2Psi2SKsMuMu = createCombinationSel( OutputList = "Bd2Psi2SKsMuMu",
                                       DecayDescriptor = "B0 -> psi(2S) KS0",
                                       DaughterLists  = [ Psi2SList, KsList ],
                                       PreVertexCuts = "in_range(5000,AM,5650)",
                                       PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 20)"
                                       )


Bd2Psi2SKsMuMuPrescaledLine = StrippingLine("Bd2Psi2SKsMuMuPrescaledLine", algos = [ Bd2Psi2SKsMuMu ] , prescale = 1.00)
Bd2Psi2SKsMuMuDetachedLine  = StrippingLine("Bd2Psi2SKsMuMuDetachedLine",
                                            algos = [ createSubSel( InputList = Bd2Psi2SKsMuMu,
                                                                    OutputList = Bd2Psi2SKsMuMu.name() + "Detached",
                                                                    Cuts = "(BPVLTIME()>0.15*ps)" )] )
Bd2Psi2SKsMuMuUnbiasedLine  = StrippingLine("Bd2Psi2SKsMuMuUnbiasedLine",
                                            algos = [ createSubSel( InputList = Bd2Psi2SKsMuMu,
                                                                    OutputList = Bd2Psi2SKsMuMu.name() + "Unbiased",
                                                                    Cuts = "(MINTREE('KS0'==ABSID, PT)> 1000.*MeV)") ] )
B2Psi2SXMuMuLines += [ Bd2Psi2SKsMuMuPrescaledLine, Bd2Psi2SKsMuMuDetachedLine, Bd2Psi2SKsMuMuUnbiasedLine ]






