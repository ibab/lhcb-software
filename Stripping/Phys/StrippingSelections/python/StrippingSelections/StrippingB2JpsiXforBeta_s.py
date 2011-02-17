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
    return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)

# create a selection using a ParticleCombiner
def createCombinationsSel( OutputList,
                          DecayDescriptors,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL" ) :
    combinerName = "CombinerParticlesFor" + OutputList
    if allConfigurables.get( combinerName ) :
        raise ValueError, 'CombineParticles instance with name '+ combinerName+' already exists'
    combiner = CombineParticles( combinerName,
                                 DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = False)#True)
    return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)


# define input daughter lists for various B -> J/psi X selections
JpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu")

NoPIDKaonList = createSubSel( OutputList = "NoPIDKaonsForBToJpsiX",
                         InputList = DataOnDemand(Location = "Phys/StdNoPIDsKaons"),
                         Cuts = "(TRCHI2DOF < 5)" )

KaonList = createSubSel( OutputList = "KaonsForBToJpsiX",
                         InputList = DataOnDemand(Location = "Phys/StdLooseKaons"),
                         Cuts = "(TRCHI2DOF < 5) & (PIDK > -2)" )

PhiList = createSubSel( OutputList = "Phi2KKForBetaS",
                        InputList = DataOnDemand(Location = "Phys/StdLoosePhi2KK"),
                        Cuts = "(in_range(980,M,1050))" \
                        "& (PT > 500) " \
                        "& (VFASPF(VCHI2) < 16)" \
                        "& (MAXTREE('K+'==ABSID, TRCHI2DOF) <5)" \
                        "& (MINTREE('K+'==ABSID, PIDK) > -2)" )

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
                                  DaughterCuts = { "pi+" : "(TRCHI2DOF < 5)"},
                                  DecayDescriptor = "[K*(892)0 -> K+ pi-]cc",
                                  PreVertexCuts = "(APT>500*MeV) & (ADAMASS('K*(892)0') < 90*MeV)",
                                  PostVertexCuts = "(VFASPF(VCHI2) < 16)" )

KsListLoose = MergedSelection("StdLooseKsMerged",
                              RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD"),
                                                    DataOnDemand(Location = "Phys/StdLooseKsLL")] )

KsList = createSubSel(OutputList = "KsForBetaS",
                      InputList = KsListLoose, Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)")

f0List = createCombinationsSel( OutputList = "f02PiPiForBetaS",
                               DaughterLists = [ KaonList,
                                                 DataOnDemand(Location = "Phys/StdLoosePions") ],
                               DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+", "f_0(980) -> K+ K-", "f_0(980) -> K- K-", "f_0(980) -> K+ K+"],
                               DaughterCuts = { "pi+" : " (MIPCHI2DV(PRIMARY)>6) & (TRCHI2DOF < 5)", 
                                                "K+"  : " (MIPCHI2DV(PRIMARY)>6) & (TRCHI2DOF < 5)" },
                               PreVertexCuts = "(ACHILD(PT,1)+ACHILD(PT,2) > 900.*MeV) & (AM < 2700 *MeV) & (ADOCACHI2CUT(20., ''))",
                               PostVertexCuts = "(VFASPF(VCHI2) < 16)" )

LambdaListLoose = MergedSelection("StdLooseLambdaMerged",
                                  RequiredSelections =  [DataOnDemand(Location = "Phys/StdLooseLambdaDD"),
                                                         DataOnDemand(Location = "Phys/StdLooseLambdaLL")])
LambdaList =  createSubSel(OutputList = "LambdaForBetaS",
                           InputList = LambdaListLoose ,
                           Cuts = "(MAXTREE('p+'==ABSID, PT) > 500.*MeV) & (MAXTREE('pi-'==ABSID, PT) > 100.*MeV)&(ADMASS('Lambda0')< 15 *MeV)& (VFASPF(VCHI2)<20)")


EtaList = createSubSel( OutputList = "EtaForBetaS",
			InputList = DataOnDemand(Location = "Phys/StdLooseResolvedEta"),
			Cuts = "(PT > 1500)"\
			"& (MINTREE('gamma'==ABSID, PT) >300)")



##################
### Inlusive J/psi. We keep it for as long as we can.
##################

Jpsi2MuMuForBetasLine =  StrippingLine("Jpsi2MuMuForBetasLine",algos = [ JpsiList ], prescale = 0.5)
Jpsi2MuMuForBetasDetachedLine = StrippingLine("Jpsi2MuMuForBetasDetachedLine",
                                              algos = [ createSubSel( InputList = JpsiList,
                                                                      OutputList = JpsiList.name() + "Detached",
                                                                      Cuts = "(BPVLTIME()>0.15*ps)" ) ] )
B2JpsiXLines += [ Jpsi2MuMuForBetasLine,Jpsi2MuMuForBetasDetachedLine ]

##################
### Bu->JpsiK+ ###
##################
Bu2JpsiK = createCombinationSel( OutputList = "Bu2JpsiK",
                                 DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                 DaughterLists = [ JpsiList, KaonList ],
                                 DaughterCuts  = {"K+": "(PT > 500)" },
                                 PreVertexCuts = "in_range(5000,AM,5650)",
                                 PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )

Bu2JpsiKPrescaledLine = StrippingLine("Bu2JpsiKPrescaledLine", algos = [ Bu2JpsiK ] , prescale = 0.125)

Bu2JpsiKDetached = createSubSel( InputList = Bu2JpsiK, OutputList = Bu2JpsiK.name() + "Detached",
                                 Cuts = "(BPVLTIME()>0.15*ps)" )
Bu2JpsiKDetachedLine  = StrippingLine("Bu2JpsiKDetachedLine", algos = [ Bu2JpsiKDetached ] )

Bu2JpsiKUnbiased = createSubSel( InputList = Bu2JpsiK,
                                 OutputList = Bu2JpsiK.name() + "Unbiased",
                                 Cuts = "(MINTREE('K+'==ABSID, PT) > 1000.*MeV)")
Bu2JpsiKUnbiasedLine  = StrippingLine("Bu2JpsiKUnbiasedLine",
                                      algos = [ Bu2JpsiKUnbiased ] )

B2JpsiXLines += [ Bu2JpsiKPrescaledLine, Bu2JpsiKDetachedLine, Bu2JpsiKUnbiasedLine]

##################
### Bu->Jpsih+ ###
##################
# WH: could in fact make lines above a sublist of this one
Bu2JpsiH = createCombinationSel( OutputList = "Bu2JpsiKNoPID",
                                 DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                 DaughterLists = [ JpsiList, NoPIDKaonList ],
                                 DaughterCuts  = {"K+": "(PT > 500)"},
                                 PreVertexCuts = "in_range(5000,AM,5650)",
                                 PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )
Bu2JpsiHDetachedLine  = StrippingLine("Bu2JpsiKNoPIDDetachedLine",
                                      algos = [ createSubSel( InputList = Bu2JpsiH,
                                                              OutputList = Bu2JpsiH.name() + "Detached",
                                                              Cuts = "(BPVLTIME()>0.15*ps)" ) ] )
B2JpsiXLines += [ Bu2JpsiHDetachedLine ]

##################
### Bs->JpsiPhi ##
##################

Bs2JpsiPhi = createCombinationSel( OutputList = "Bs2JpsiPhi",
                                   DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                                   DaughterLists  = [ JpsiList, PhiList ],
                                   PreVertexCuts = "in_range(5000,AM,5650)",
                                   PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )
Bs2JpsiPhiPrescaledLine = StrippingLine("Bs2JpsiPhiPrescaledLine", algos = [ Bs2JpsiPhi ] , prescale = 1.00)

Bs2JpsiPhiDetached = createSubSel( InputList = Bs2JpsiPhi,
                                   OutputList = Bs2JpsiPhi.name() + "Detached",
                                   Cuts = "(BPVLTIME()>0.15*ps)" )
Bs2JpsiPhiDetachedLine  = StrippingLine("Bs2JpsiPhiDetachedLine", algos = [ Bs2JpsiPhiDetached ] )

Bs2JpsiPhiUnbiased = createSubSel( InputList = Bs2JpsiPhi,
                                   OutputList = Bs2JpsiPhi.name() + "Unbiased",
                                   Cuts = "(MINTREE('phi(1020)'==ABSID, PT) > 1000.*MeV)")
Bs2JpsiPhiUnbiasedLine =  StrippingLine("Bs2JpsiPhiUnbiasedLine", algos = [ Bs2JpsiPhiUnbiased ] )

B2JpsiXLines += [ Bs2JpsiPhiPrescaledLine, Bs2JpsiPhiDetachedLine, Bs2JpsiPhiUnbiasedLine ]


##################
### B0->JpsiK*  ##
##################

Bd2JpsiKstar = createCombinationSel( OutputList = "Bd2JpsiKstar",
                                     DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                                     DaughterLists  = [ JpsiList, KstarList ],
                                     PreVertexCuts = "in_range(5000,AM,5650)",
                                     PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)" )

Bd2JpsiKstarPrescaledLine = StrippingLine("Bd2JpsiKstarPrescaledLine", algos = [ Bd2JpsiKstar ] , prescale = 0.125)

Bd2JpsiKstarDetached = createSubSel( InputList = Bd2JpsiKstar,
                                     OutputList = Bd2JpsiKstar.name() + "Detached",
                                     Cuts = "(BPVLTIME()>0.15*ps)" )
Bd2JpsiKstarDetachedLine  = StrippingLine("Bd2JpsiKstarDetachedLine",
                                          algos = [ Bd2JpsiKstarDetached ] )

Bd2JpsiKstarUnbiased = createSubSel( InputList = Bd2JpsiKstar,
                                     OutputList = Bd2JpsiKstar.name() + "Unbiased",
                                     Cuts = "(PT>2.*GeV) " \
                                     "& (MINTREE('K*(892)0'==ABSID, PT)> 1000.*MeV)")
Bd2JpsiKstarUnbiasedLine  = StrippingLine("Bd2JpsiKstarUnbiasedLine",
                                          algos = [ Bd2JpsiKstarUnbiased ] )

B2JpsiXLines += [ Bd2JpsiKstarPrescaledLine, Bd2JpsiKstarDetachedLine, Bd2JpsiKstarUnbiasedLine]

##################
### Bd->JpsiKS ###
##################
Bd2JpsiKs = createCombinationSel( OutputList = "Bd2JpsiKS",
                                  DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                  DaughterLists  = [ JpsiList, KsList ],
                                  PreVertexCuts = "in_range(5000,AM,5650)",
                                  PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)"
                                  )

Bd2JpsiKsPrescaledLine = StrippingLine("Bd2JpsiKsPrescaledLine", algos = [ Bd2JpsiKs ] , prescale = 1.00)

Bd2JpsiKsDetached = createSubSel( InputList = Bd2JpsiKs,
                                  OutputList = Bd2JpsiKs.name() + "Detached",
                                  Cuts = "(BPVLTIME()>0.15*ps)" )
Bd2JpsiKsDetachedLine  = StrippingLine("Bd2JpsiKsDetachedLine",
                                       algos = [ Bd2JpsiKsDetached ] )

Bd2JpsiKsUnbiased = createSubSel( InputList = Bd2JpsiKs,
                                  OutputList = Bd2JpsiKs.name() + "Unbiased",
                                  Cuts = "(MINTREE('KS0'==ABSID, PT)> 1000.*MeV)")
Bd2JpsiKsUnbiasedLine  = StrippingLine("Bd2JpsiKsUnbiasedLine",
                                       algos = [ Bd2JpsiKsUnbiased ] )

B2JpsiXLines += [ Bd2JpsiKsPrescaledLine, Bd2JpsiKsDetachedLine, Bd2JpsiKsUnbiasedLine ]

##################
### Bs->Jpsif0 ##
### Note: the input list is already heavily lifetime biased.
##################
Bs2Jpsif0 = createCombinationSel( OutputList = "Bs2Jpsif0",
                                  DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)",
                                  DaughterLists  = [ JpsiList, f0List ],
                                  PreVertexCuts = "ADAMASS('B_s0') < 300",
                                  PostVertexCuts = "(VFASPF(VCHI2PDOF) < 10) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" )

Bs2Jpsif0Line = StrippingLine("Bs2Jpsif0Line", algos = [ Bs2Jpsif0 ])

B2JpsiXLines += [ Bs2Jpsif0Line ]

###############################
### Lambdab->JpsiLambda ###
##############################“
# Added by Yasmine
Lambdab2JpsiLambda = createCombinationSel( OutputList = "Lambdab2JpsiLambda",
                                  DecayDescriptor = "[Lambda_b0 -> Lambda0 J/psi(1S) ]cc",
                                  DaughterLists  = [ JpsiList, LambdaList ],
                                  PreVertexCuts = "in_range(5020,AM,6220)",
                                  PostVertexCuts = "in_range(5120,M,6120) & (VFASPF(VCHI2PDOF) < 20)"
                                  )

Lambdab2JpsiLambdaUnbiasedLine = StrippingLine("Lambdab2JpsiLambdaUnbiasedLine", algos = [ Lambdab2JpsiLambda ])

B2JpsiXLines += [Lambdab2JpsiLambdaUnbiasedLine ]


##################
## Bs->Jpsi Eta ##
##################
Bs2JpsiEta = createCombinationSel( OutputList = "Bs2JpsiEta",
                                  DecayDescriptor = "B_s0 -> J/psi(1S) eta",
                                  DaughterLists  = [ JpsiList, EtaList ],
                                  PreVertexCuts = "in_range(5000,AM,5650)",
                                  PostVertexCuts = "in_range(5100,M,5550) & (VFASPF(VCHI2PDOF) < 10)"
                                  )

Bs2JpsiEtaPrescaledLine = StrippingLine("Bs2JpsiEtaPrescaledLine", algos = [ Bs2JpsiEta ] , prescale = 1.00)
Bs2JpsiEtaDetachedLine  = StrippingLine("Bs2JpsiEtaDetachedLine",
                                       algos = [ createSubSel( InputList = Bs2JpsiEta,
                                                               OutputList = Bs2JpsiEta.name() + "Detached",
                                                               Cuts = "(BPVLTIME()>0.15*ps)" )] )
Bs2JpsiEtaUnbiasedLine  = StrippingLine("Bs2JpsiEtaUnbiasedLine",
                                       algos = [ createSubSel( InputList = Bs2JpsiEta,
                                                               OutputList = Bs2JpsiEta.name() + "Unbiased",
                                                               Cuts = "(MINTREE('eta'==ABSID, PT) > 1000.*MeV)") ] )
B2JpsiXLines += [ Bs2JpsiEtaPrescaledLine, Bs2JpsiEtaDetachedLine, Bs2JpsiEtaUnbiasedLine ]


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
