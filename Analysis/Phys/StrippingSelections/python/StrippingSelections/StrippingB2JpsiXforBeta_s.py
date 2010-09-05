# $Id: StrippingB2JpsiXforBeta_s.py,v 1.3 2010-09-05 21:31:13 wouter Exp $
'''
Module for construction of B->JpsiX roadmap lifetime unbiased 
stripping Selections and StrippingLine.
Provides functions to build Bs, Jpsi, Phi selections.
Provides class Bs2JpsiPhiConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   B2JpsiXLines
'''

__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '26/08/2010'
__version__ = '$Revision: 1.3 $'


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
B2JpsiXLines = []

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
    combiner.OfflineVertexFitter.useResonanceVertex = False
    return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)


# define input daughter lists for various B -> J/psi X selections
JpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu")

KaonList = createSubSel( OutputList = "KaonsForBToJpsiX",
                         InputList = DataOnDemand(Location = "Phys/StdLooseKaons"),
                         Cuts = "(TRCHI2DOF < 10) & (PIDK > -5)" )

PhiList = createSubSel( OutputList = "Phi2KKForBetaS",
                        InputList = DataOnDemand(Location = "Phys/StdLoosePhi2KK"),
                        Cuts = "(ADMASS('phi(1020)') < 20)" \
                        "& (PT > 500) " \
                        "& (VFASPF(VCHI2) < 16)" \
                        "& (MAXTREE('K+'==ABSID, TRCHI2DOF) <10)" \
                        "& (MINTREE('K+'==ABSID, PIDK) > -5)" )

#KstarList = createSubSel( OutputList = "Kstar2KpiForBetaS",
#                          InputList = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi"),
#                          Cuts = "(ADMASS('K*(892)0') < 90)" \
#                          "& (PT > 500)" \
#                          "& (VFASPF(VCHI2) < 16)" \
#                          "& (MINTREE('K+'==ABSID, PIDK) > -5)" \
#                          "& (MAXTREE('K+'==ABSID, TRCHI2DOF)  <10)" \
#                          "& (MAXTREE('pi+'==ABSID, TRCHI2DOF) <10)")

KstarList = createCombinationSel( OutputList = "Kstar2KpiForBetaS",
                                  DaughterLists = [ KaonList,
                                                    DataOnDemand(Location = "Phys/StdLoosePions") ],
                                  DaughterCuts = { "pi+" : "(TRCHI2DOF < 10)"},
                                  DecayDescriptor = "[K*(892)0 -> K+ pi-]cc",
                                  PreVertexCuts = "(APT>500*MeV) & (ADAMASS('K*(892)0') < 90*MeV)",
                                  PostVertexCuts = "(VFASPF(VCHI2) < 16)" )

KsList = MergedSelection("KsForBetaS",
                         RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD"),
                                               DataOnDemand(Location = "Phys/StdLooseKsLL")] )

f0List = createCombinationSel( OutputList = "f02PiPiForBetaS",
                               DaughterLists = [ DataOnDemand(Location = "Phys/StdLoosePions") ],
                               DecayDescriptor = "[f_0(980) -> pi+ pi-]cc",
                               DaughterCuts = { "pi+" : " (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF < 10)"},
                               PreVertexCuts = "(APT>900*MeV) & (ADAMASS('f_0(980)') < 500*MeV) & (AMAXDOCA('') < 0.5*mm)",
                               PostVertexCuts = "(VFASPF(VCHI2) < 16)" )

##################
### Inlusive J/psi. We keep it for as long as we can.
##################

Jpsi2MuMuForBetasLine =  StrippingLine("Jpsi2MuMuForBetasLine",algos = [ JpsiList ], prescale = 1)
B2JpsiXLines += [ Jpsi2MuMuForBetasLine ]

##################
### Bu->JpsiK+ ###
##################
Bu2JpsiK = createCombinationSel( OutputList = "Bu2JpsiK",
                                 DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                 DaughterLists = [ KaonList, JpsiList],
                                 DaughterCuts  = {"K+": "(PT > 500)" },
                                 PreVertexCuts = "ADAMASS('B+') < 300",
                                 PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 10)" )

Bu2JpsiKPrescaledLine = StrippingLine("Bu2JpsiKPrescaledLine", algos = [ Bu2JpsiK ] , prescale = 1.00)
Bu2JpsiKDetachedLine  = StrippingLine("Bu2JpsiKDetachedLine",
                                      algos = [ createSubSel( InputList = Bu2JpsiK,
                                                              OutputList = Bu2JpsiK.name() + "Detached",
                                                              Cuts = "(BPVLTIME()>0.15*ps)" ) ] )
Bu2JpsiKUnbiasedLine  = StrippingLine("Bu2JpsiKUnbiasedLine",
                                      algos = [ createSubSel( InputList = Bu2JpsiK,
                                                              OutputList = Bu2JpsiK.name() + "Unbiased",
                                                              Cuts = "(MINTREE('K+'==ABSID, PT) > 1000.*MeV)") ] )

B2JpsiXLines += [ Bu2JpsiKPrescaledLine, Bu2JpsiKDetachedLine, Bu2JpsiKUnbiasedLine]

##################
### Bs->JpsiPhi ##
##################

Bs2JpsiPhi = createCombinationSel( OutputList = "Bs2JpsiPhi",
                                   DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                                   DaughterLists  = [ PhiList, JpsiList],
                                   PreVertexCuts = "ADAMASS('B_s0') < 300",
                                   PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 10)" )

Bs2JpsiPhiPrescaledLine = StrippingLine("Bs2JpsiPhiPrescaledLine", algos = [ Bs2JpsiPhi ] , prescale = 1.00)

Bs2JpsiPhiDetachedLine  = StrippingLine("Bs2JpsiPhiDetachedLine",
                                        algos = [ createSubSel( InputList = Bs2JpsiPhi,
                                                                OutputList = Bs2JpsiPhi.name() + "Detached",
                                                                Cuts = "(BPVLTIME()>0.15*ps)" )] )

Bs2JpsiPhiUnbiasedLine =  StrippingLine("Bs2JpsiPhiUnbiasedLine",
                                        algos = [ createSubSel( InputList = Bs2JpsiPhi,
                                                                OutputList = Bs2JpsiPhi.name() + "Unbiased",
                                                                Cuts = "(MINTREE('phi(1020)'==ABSID, PT) > 1000.*MeV)") ] )

B2JpsiXLines += [ Bs2JpsiPhiPrescaledLine, Bs2JpsiPhiDetachedLine, Bs2JpsiPhiUnbiasedLine ]


##################
### B0->JpsiK*  ##
##################

Bd2JpsiKstar = createCombinationSel( OutputList = "Bd2JpsiKstar",
                                     DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                                     DaughterLists  = [ KstarList, JpsiList ],
                                     PreVertexCuts = "ADAMASS('B0') < 300",
                                     PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 10)" )

Bd2JpsiKstarPrescaledLine = StrippingLine("Bd2JpsiKstarPrescaledLine", algos = [ Bd2JpsiKstar ] , prescale = 0.25)
Bd2JpsiKstarDetachedLine  = StrippingLine("Bd2JpsiKstarDetachedLine",
                                          algos = [ createSubSel( InputList = Bd2JpsiKstar,
                                                                  OutputList = Bd2JpsiKstar.name() + "Detached",
                                                                  Cuts = "(BPVLTIME()>0.15*ps)" ) ] )
Bd2JpsiKstarUnbiasedLine  = StrippingLine("Bd2JpsiKstarUnbiasedLine",
                                          algos = [ createSubSel( InputList = Bd2JpsiKstar,
                                                                  OutputList = Bd2JpsiKstar.name() + "Unbiased",
                                                                  Cuts = "(PT>2.*GeV) " \
                                                                  "& (MINTREE('K*(892)0'==ABSID, PT)> 1000.*MeV)") ] )
B2JpsiXLines += [ Bd2JpsiKstarPrescaledLine, Bd2JpsiKstarDetachedLine, Bd2JpsiKstarUnbiasedLine]


##################
### Bd->JpsiKS ###
##################
Bd2JpsiKs = createCombinationSel( OutputList = "Bd2JpsiKS",
                                  DecayDescriptor = "[B0 -> J/psi(1S) KS0]cc",
                                  DaughterLists  = [ KsList, JpsiList ],
                                  PreVertexCuts = "ADAMASS('B0') < 500",
                                  PostVertexCuts = "ALL" #"(VFASPF(VCHI2/VDOF) < 20)"
                                  )

Bd2JpsiKsPrescaledLine = StrippingLine("Bd2JpsiKsPrescaledLine", algos = [ Bd2JpsiKs ] , prescale = 1.00)
Bd2JpsiKsDetachedLine  = StrippingLine("Bd2JpsiKsDetachedLine",
                                       algos = [ createSubSel( InputList = Bd2JpsiKs,
                                                               OutputList = Bd2JpsiKs.name() + "Detached",
                                                               Cuts = "(BPVLTIME()>0.15*ps)" )] )
Bd2JpsiKsUnbiasedLine  = StrippingLine("Bd2JpsiKsUnbiasedLine",
                                       algos = [ createSubSel( InputList = Bd2JpsiKs,
                                                               OutputList = Bd2JpsiKs.name() + "Unbiased",
                                                               Cuts = "(MINTREE('KS0'==ABSID, PT) > 1000.*MeV)") ] )
B2JpsiXLines += [ Bd2JpsiKsPrescaledLine, Bd2JpsiKsDetachedLine, Bd2JpsiKsUnbiasedLine ]

##################
### Bs->Jpsif0 ##
### Note: the input list is already heavily lifetime biased.
##################
#Bs2Jpsif0 = createCombinationSel( OutputList = "Bs2Jpsif0",
#                                  DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)",
#                                  DaughterLists  = [ f0List, JpsiList],
#                                  PreVertexCuts = "ADAMASS('B_s0') < 300",
#                                  PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 10)" )
#
#Bs2Jpsif0Line = StrippingLine("Bs2Jpsif0Line", algos = [ Bs2Jpsif0 ])
#
#B2JpsiXLines += [ Bs2Jpsif0Line ]

###################
### Lambdab->JpsiLambda ###
##################

# to be done



############
### Todo ###
############
#
# Bd->JpsiKS
# Bs->Jpsif0
# Lambdab->JpsiLambda
# Jpsi inclusive
# common combined particles (like phi, K*)?
# everything in one file
# "copy" lines in function #new_line = NewStrippingLine( oldLine, requiredSels, extraString, extraCut , pre/postScale)
# use FilterDesktop for the copying
# mass windows Bs, Bd, Bu all the same?
#
#################
### questions ###
#################
#
# Retention?
# Be careful: prescaling/postscaling in stripping/hlt
#
