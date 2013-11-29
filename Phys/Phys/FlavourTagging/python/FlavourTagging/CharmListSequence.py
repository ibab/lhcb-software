#!/usr/bin/env python
# =============================================================================
# $Id: CharmListSequence.py
# =============================================================================
## @file  FlavourTagging/CharmListSequene.py
#  create sequence for charm candidate lists, to be used with Charm Tagger 
# =============================================================================
"""
Configuration file for 'Standard Loose D0'
"""
__author__  = "Riccardo Cenci"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $"
# =============================================================================
__all__ = (
    'StdD02KPi' ,
    'StdD02KPiPiPi' ,
    'StdD02KsPiPi' ,
    'StdDp2KPiPi' ,
    'StdDp2KsPi' ,
    'StdDp2KsPiPiPi' ,
    'CharmListSequence',
    'CharmList',
    'CharmInclList',
    'CharmStarList',
    'StdCharmFiltered',
    'locations'
    )

# =============================================================================
## initial settings

elist = ["Phys/StdAllNoPIDsElectrons/Particles", bcandsel]
mulist = ["Phys/StdAllNoPIDsMuons/Particles", bcandsel]
pilist = ["Phys/StdAllNoPIDsPions/Particles", bcandsel]
pisoftlist = ["Phys/StdAllNoPIDsPions/Particles", bcandsel]
pizlist = ["Phys/StdLooseMergedPi0/Particles", "Phys/StdLooseResolvedPi0/Particles", bcandsel]
klist = ["Phys/StdAllNoPIDsKaons/Particles", bcandsel]
kslist = ["Phys/StdLooseKsLL/Particles", "Phys/StdLooseKsLD/Particles", "Phys/StdLooseKsDD/Particles", bcandsel]

pikslist = ["Phys/StdNoPIDsDownPions/Particles", bcandsel]


print elist
print mulist
print pilist
print pisoftlist
print pizlist
print klist

#print kslist
print pikslist

from Gaudi.Configuration import *
from Configurables       import CombineParticles, GaudiSequencer
from CommonParticles.Utils import *

from PhysSelPython.Wrappers import Selection

from Configurables import PrintDecayTree, PrintDecayTreeTool

CharmListSequence = GaudiSequencer('CharmListSequence')
CharmList = []
CharmInclList = []
CharmStarList = []

charmPreambulo = [
    "from LoKiPhysMC.decorators import *" ,
    "from LoKiPhysMC.functions import mcMatch",
    "from GaudiKernel.SystemOfUnits import *"
    ]

# all tracks have TRCHI2NDOF < 3, (TRCHI2DOF<4.0) & (PT>100*MeV) & (P>1000*MeV) & (MIPCHI2DV(PRIMARY)>4.0) & (TRGHP<0.3)"
eSelection = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2()>4.0)" 
muSelection = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2()>4.0)"
piSelection = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2()>4.0)"
piksSelection = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2()>4.0)"
kSelection = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2()>4.0)"

dSelection = "(VFASPF(VCHI2/VDOF)<10) & (BPVVDCHI2>25) & (BPVDIRA>0.99)"
dStarSelection = "(VFASPF(VCHI2/VDOF)<25) & (BPVVDCHI2>25) & (BPVDIRA>0.99)"

##########################################################################################
# purge daughther lists

from Configurables       import PurgeDescendants

purgeE = PurgeDescendants("StdElectronsPurged")
purgeE.Inputs = elist
purgeE.Output = "Phys/" + purgeE.name() + "/Particles"
                              
purgeMu = PurgeDescendants("StdMuonsPurged")
purgeMu.Inputs = mulist
purgeMu.Output = "Phys/" + purgeMu.name() + "/Particles"
                              
purgePi = PurgeDescendants("StdPionsPurged")
purgePi.Inputs = pilist
purgePi.Output = "Phys/" + purgePi.name() + "/Particles"
                              
purgePiSoft = PurgeDescendants("StdSoftPionsPurged")
purgePiSoft.Inputs = pisoftlist
purgePiSoft.Output = "Phys/" + purgePiSoft.name() + "/Particles"
                              
purgePiz = PurgeDescendants("StdPionszPurged")
purgePiz.Inputs = pizlist
purgePiz.Output = "Phys/" + purgePiz.name() + "/Particles"
                              
purgeK = PurgeDescendants("StdKaonsPurged")
purgeK.Inputs = klist
purgeK.Output = "Phys/" + purgeK.name() + "/Particles"


purgeKs = PurgeDescendants("StdKsPurged")
purgeKs.Inputs = kslist
purgeKs.Output = "Phys/" + purgeKs.name() + "/Particles"

purgePiKs = PurgeDescendants("StdPiKsPurged")
purgePiKs.Inputs = pikslist
purgePiKs.Output = "Phys/" + purgePiKs.name() + "/Particles"


##########################################################################################
# KS0 -> Pi Pi
#
StdKs2PiPi = CombineParticles ( 'StdKs2PiPi' )

StdKs2PiPi.Preambulo = charmPreambulo

StdKs2PiPi.Inputs = [ purgePi.Output, purgePiKs.Output ]
#StdKs2PiPi.Inputs = [ purgePi.Output ]

StdKs2PiPi.DecayDescriptors = ["KS0 -> pi+ pi-"]

StdKs2PiPi.DaughtersCuts = {
    #"pi+" : piksSelection + " & mcMatch ( 'KS0 --> ^pi- ^pi+ {gamma} {gamma} {gamma}' )" 
    "pi+" : piksSelection
    }

#StdKs2PiPi.CombinationCut = "(ADAMASS('KS0')<100*MeV) & (ADOCACHI2CUT(25, ''))"
StdKs2PiPi.CombinationCut = "(ADAMASS('KS0')<100*MeV)"
StdKs2PiPi.MotherCut = "(ADMASS('KS0')<100*MeV) & (VFASPF(VCHI2) < 25.)"

StdKs2PiPi.Output = "Phys/" + StdKs2PiPi.name() + "/Particles"


## configure Data-On-Demand service
locations = updateDoD ( StdKs2PiPi )


##########################################################################################
# D0 -> K Pi
#
StdD02KPi = CombineParticles ( 'StdD02KPi' )

StdD02KPi.Preambulo = charmPreambulo

StdD02KPi.Inputs = [ purgePi.Output, purgeK.Output ]

StdD02KPi.DecayDescriptors = ["[D0 -> K- pi+]cc"]

StdD02KPi.DaughtersCuts = {
    "K+" : kSelection,
    "pi+" : piSelection
    }

StdD02KPi.CombinationCut = "(ADAMASS('D0')<100*MeV)"
StdD02KPi.MotherCut = "(ADMASS('D0')<50*MeV)" + " & " + dSelection


## configure Data-On-Demand service
locations = updateDoD ( StdD02KPi )


##########################################################################################
# D0 -> K Pi Pi Pi
#
StdD02KPiPiPi = CombineParticles ("StdD02KPiPiPi")

StdD02KPiPiPi.Preambulo = charmPreambulo

StdD02KPiPiPi.Inputs = [ purgePi.Output, purgeK.Output ]

StdD02KPiPiPi.DecayDescriptors = ["[D0 -> K- pi+ pi+ pi-]cc"]

StdD02KPiPiPi.DaughtersCuts = {
    "K+" : kSelection,
    "pi+" : piSelection
    }

StdD02KPiPiPi.CombinationCut = "(ADAMASS('D0')<100*MeV)"
StdD02KPiPiPi.MotherCut = "(ADMASS('D0')<50*MeV)" + " & " + dSelection

StdD02KPiPiPi.Output = "Phys/" + StdD02KPiPiPi.name() + "/Particles"

## configure Data-On-Demand service
locations.update( updateDoD ( StdD02KPiPiPi ) )


# not used
## ##########################################################################################
StdDstar2D0Pi2KPiPiPi = CombineParticles ( 'StdDstar2D0Pi2KPiPiPi' )

StdDstar2D0Pi2KPiPiPi.Preambulo = charmPreambulo

StdDstar2D0Pi2KPiPiPi.Inputs = [ purgePi.Output, StdD02KPiPiPi.Output ]
StdDstar2D0Pi2KPiPiPi.DecayDescriptors = ["[D*(2010)+ -> D0 pi+]cc"]

StdDstar2D0Pi2KPiPiPi.CombinationCut = "(ADAMASS('D*(2010)+')<50*MeV) & (APT>1250*MeV)"
StdDstar2D0Pi2KPiPiPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (M-MAXTREE('D0'==ABSID,M)<165.5)"

## configure Data-On-Demand service
locations = updateDoD ( StdDstar2D0Pi2KPiPiPi )


##########################################################################################
# D0 -> Ks Pi Pi
#
StdD02KsPiPi = CombineParticles("StdD02KsPiPi")

StdD02KsPiPi.Preambulo = charmPreambulo

StdD02KsPiPi.Inputs = [ purgePi.Output, purgeKs.Output]

StdD02KsPiPi.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc"]

StdD02KsPiPi.DaughtersCuts = {
    "pi+" : piSelection #"(TRCHI2DOF<5.0) & (PT>100*MeV) & (P>1000*MeV)"
    }

StdD02KsPiPi.CombinationCut = "(ADAMASS('D0')<100*MeV)"
StdD02KsPiPi.MotherCut = "(ADMASS('D0')<50*MeV)"  + " & " + dSelection

StdD02KsPiPi.Output = "Phys/" + StdD02KsPiPi.name() + "/Particles"

## configure Data-On-Demand service
locations = updateDoD ( StdD02KsPiPi )



## ##########################################################################################
StdDstar2D0Pi2KsPiPi = CombineParticles ( 'StdDstar2D0Pi2KsPiPi' )

StdDstar2D0Pi2KsPiPi.Preambulo = charmPreambulo

StdDstar2D0Pi2KsPiPi.Inputs = [ purgePi.Output, StdD02KsPiPi.Output ]
StdDstar2D0Pi2KsPiPi.DecayDescriptors = ["[D*(2010)+ -> D0 pi+]cc"] 

StdDstar2D0Pi2KsPiPi.CombinationCut = "(ADAMASS('D*(2010)+')<50*MeV)"
StdDstar2D0Pi2KsPiPi.MotherCut = "(M-MAXTREE('D0'==ABSID,M)<165.5)" + " & " + dStarSelection

## configure Data-On-Demand service
locations = updateDoD ( StdDstar2D0Pi2KsPiPi )





##########################################################################################
# D0 -> K Pi Pi0
#
StdD02KPiPi0 = CombineParticles("StdD02KPiPi0")

StdD02KPiPi0.Preambulo = charmPreambulo

StdD02KPiPi0.Inputs = [ purgePi.Output, purgePiz.Output, purgeK.Output]

StdD02KPiPi0.DecayDescriptors = ["[D0 -> K- pi+ pi0]cc"]

StdD02KPiPi0.DaughtersCuts = {
    "K+" : kSelection,
    "pi+" : piSelection,
    "pi0" : "(ABSID==111) & (PT>500*MeV) & (P>1000*MeV)"
    }

StdD02KPiPi0.CombinationCut = "(ADAMASS('D0')<100*MeV)"
StdD02KPiPi0.MotherCut = "(ADMASS('D0')<50*MeV)" + " & " + dSelection

StdD02KPiPi0.Output = "Phys/" + StdD02KPiPi0.name() + "/Particles"

## configure Data-On-Demand service
locations = updateDoD ( StdD02KPiPi0 )


##########################################################################################
# D0 -> K Pi Pi0 partial
#
StdD02KPiPi0part = CombineParticles("StdD02KPiPi0part")

StdD02KPiPi0part.Preambulo = charmPreambulo

StdD02KPiPi0part.Inputs = [ purgePi.Output, purgeK.Output]

StdD02KPiPi0part.DecayDescriptors = ["[D0 -> K- pi+]cc"]

StdD02KPiPi0part.DaughtersCuts = {
    "K+" : kSelection + " & (MIPCHI2DV(PRIMARY)>10.0)",
    "pi+" : piSelection + " & (MIPCHI2DV(PRIMARY)>10.0)"
    }

StdD02KPiPi0part.CombinationCut = "( in_range(1450*MeV,AWM('K-','pi+'),1850*MeV) | in_range(842*MeV,AWM('K-','pi+'),955*MeV) )"
StdD02KPiPi0part.MotherCut = "( (ADMASS('K*(892)0')<50*MeV) | ( (DMASS('D0')>-400*MeV) & (DMASS('D0')<0*MeV) ) )" + " & " + dSelection

StdD02KPiPi0part.Output = "Phys/" + StdD02KPiPi0part.name() + "/Particles"

## configure Data-On-Demand service
locations = updateDoD ( StdD02KPiPi0part )





##########################################################################################
# D0 -> K e X 
#
StdD02Kepart = CombineParticles("StdD02Kepart")

StdD02Kepart.Preambulo = charmPreambulo

StdD02Kepart.Inputs = [ purgeE.Output, purgeK.Output]

StdD02Kepart.DecayDescriptors = ["[D0 -> K- e+]cc"]

StdD02Kepart.DaughtersCuts = {
    "K+" : kSelection + " & (MIPCHI2DV(PRIMARY)>10.0)",
    "e+" : eSelection + " & (MIPCHI2DV(PRIMARY)>10.0)"
    }

StdD02Kepart.CombinationCut = "( in_range(1450*MeV,AWM('K-','e+'),1850*MeV) | in_range(842*MeV,AWM('K-','e+'),955*MeV) )"
StdD02Kepart.MotherCut = "( (ADMASS('K*(892)0')<50*MeV) | (DMASS('D0')>-400*MeV) & (DMASS('D0')<0*MeV) )" + " & " + dSelection

StdD02Kepart.Output = "Phys/" + StdD02Kepart.name() + "/Particles"

## configure Data-On-Demand service
locations = updateDoD ( StdD02Kepart )



##########################################################################################
# D0 -> K mu X 
#
StdD02Kmupart = CombineParticles("StdD02Kmupart")

StdD02Kmupart.Preambulo = charmPreambulo

StdD02Kmupart.Inputs = [ purgeMu.Output, purgeK.Output]

StdD02Kmupart.DecayDescriptors = ["[D0 -> K- mu+]cc"]

StdD02Kmupart.DaughtersCuts = {
    "K+" : kSelection + " & (MIPCHI2DV(PRIMARY)>10.0)",
    "mu+" : muSelection + " & (MIPCHI2DV(PRIMARY)>10.0)"
    }

StdD02Kmupart.CombinationCut = "( in_range(1450*MeV,AWM('K-','mu+'),1850*MeV) | in_range(842*MeV,AWM('K-','mu+'),955*MeV))"
StdD02Kmupart.MotherCut = "( (ADMASS('K*(892)0')<50*MeV) | (DMASS('D0')>-400*MeV) & (DMASS('D0')<0*MeV) )" + " & " + dSelection

StdD02Kmupart.Output = "Phys/" + StdD02Kmupart.name() + "/Particles"

## configure Data-On-Demand service
locations = updateDoD ( StdD02Kmupart )



##########################################################################################
# D+ -> K Pi partial
#
StdDp2KPipart = CombineParticles("StdDp2KPipart")

StdDp2KPipart.Preambulo = charmPreambulo

StdDp2KPipart.Inputs = [ purgePi.Output, purgeK.Output]

StdDp2KPipart.DecayDescriptors = ["[D+ -> K- pi+]cc"]

StdDp2KPipart.DaughtersCuts = {
    "K+" : kSelection + " & (MIPCHI2DV(PRIMARY)>10.0)",
    "pi+" : piSelection + " & (MIPCHI2DV(PRIMARY)>10.0)"
    }

StdDp2KPipart.CombinationCut = "( in_range(1450*MeV,AWM('K-','pi+'),1850*MeV) | in_range(842*MeV,AWM('K-','pi+'),955*MeV))"
StdDp2KPipart.MotherCut = "( (ADMASS('K*(892)0')<50*MeV) | ( (DMASS('D+')>-400*MeV) & (DMASS('D+')<0*MeV) ) )" + " & " + dSelection

StdDp2KPipart.Output = "Phys/" + StdDp2KPipart.name() + "/Particles"

## configure Data-On-Demand service
locations = updateDoD ( StdDp2KPipart )


##########################################################################################
# D+ -> K e X 
#
StdDp2Kepart = CombineParticles("StdDp2Kepart")

StdDp2Kepart.Preambulo = charmPreambulo

StdDp2Kepart.Inputs = [ purgeE.Output, purgeK.Output]

StdDp2Kepart.DecayDescriptors = ["[D+ -> K- e+]cc"]

StdDp2Kepart.DaughtersCuts = {
    "K+" : kSelection + " & (MIPCHI2DV(PRIMARY)>10.0)",
    "e+" : eSelection + " & (MIPCHI2DV(PRIMARY)>10.0)"
    }

StdDp2Kepart.CombinationCut = "( in_range(1450*MeV,AWM('K-','e+'),1850*MeV) | in_range(842*MeV,AWM('K-','e+'),955*MeV) )"
StdDp2Kepart.MotherCut = "( (ADMASS('K*(892)0')<50*MeV) | (DMASS('D+')>-400*MeV) & (DMASS('D+')<0*MeV) )" + " & " + dSelection

StdDp2Kepart.Output = "Phys/" + StdDp2Kepart.name() + "/Particles"

## configure Data-On-Demand service
locations = updateDoD ( StdDp2Kepart )



##########################################################################################
# D+ -> K mu X 
#
StdDp2Kmupart = CombineParticles("StdDp2Kmupart")

StdDp2Kmupart.Preambulo = charmPreambulo

StdDp2Kmupart.Inputs = [ purgeMu.Output, purgeK.Output]

StdDp2Kmupart.DecayDescriptors = ["[D+ -> K- mu+]cc"]

StdDp2Kmupart.DaughtersCuts = {
    "K+" : kSelection + " & (MIPCHI2DV(PRIMARY)>10.0)",
    "mu+" : muSelection + " & (MIPCHI2DV(PRIMARY)>10.0)"
    }

StdDp2Kmupart.CombinationCut = "( in_range(1450*MeV,AWM('K-','mu+'),1850*MeV) | in_range(842*MeV,AWM('K-','mu+'),955*MeV))"
StdDp2Kmupart.MotherCut = "( (ADMASS('K*(892)0')<50*MeV) | (DMASS('D+')>-400*MeV) & (DMASS('D+')<0*MeV) )" + " & " + dSelection

StdDp2Kmupart.Output = "Phys/" + StdDp2Kmupart.name() + "/Particles"

## configure Data-On-Demand service
locations = updateDoD ( StdDp2Kmupart )







##########################################################################################
# D+ -> K Pi Pi
#
StdDp2KPiPi = CombineParticles ( 'StdDp2KPiPi' )

StdDp2KPiPi.Preambulo = charmPreambulo

StdDp2KPiPi.Inputs = [ purgePi.Output, purgeK.Output ]

StdDp2KPiPi.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc"]
StdDp2KPiPi.DaughtersCuts = {
    "K+"  : kSelection,
    "pi+" : piSelection
    }

StdDp2KPiPi.CombinationCut = "(ADAMASS('D+')<100*MeV)"
StdDp2KPiPi.MotherCut = "(ADMASS('D+')<50*MeV)" + " & " + dSelection

## Configure Data-On-Demand service
locations = updateDoD ( StdDp2KPiPi )


## ##########################################################################################
# Dp -> Ks Pi
#
StdDp2KsPi = CombineParticles("StdDp2KsPi")

StdDp2KsPi.Preambulo = charmPreambulo

StdDp2KsPi.Inputs = [ purgePi.Output, purgeKs.Output]

StdDp2KsPi.DecayDescriptors = ["[D+ -> KS0 pi+]cc"]

StdDp2KsPi.DaughtersCuts = {
    "pi+" : piSelection
    }

StdDp2KsPi.CombinationCut = "(ADAMASS('D+')<100*MeV)"
StdDp2KsPi.MotherCut = "(ADMASS('D+')<50*MeV)" + " & " + dSelection

## configure Data-On-Demand service
locations = updateDoD ( StdDp2KsPi )




##########################################################################################
##########################################################################################
CharmListSequence.IgnoreFilterPassed  =   True


CharmListSequence.Members += [purgeE]
CharmListSequence.Members += [purgeMu]
CharmListSequence.Members += [purgePi]
CharmListSequence.Members += [purgePiz]
CharmListSequence.Members += [purgeK]
CharmListSequence.Members += [purgeKs]
CharmListSequence.Members += [purgePiKs]


CharmListSequence.Members += [StdKs2PiPi]


CharmListSequence.Members += [StdD02KPi]
CharmList += ['Phys/StdD02KPi/Particles']

CharmListSequence.Members += [ StdD02KPiPiPi]
CharmList += ['Phys/StdD02KPiPiPi/Particles']

CharmListSequence.Members += [ StdD02KsPiPi]
CharmListSequence.Members += [ StdDstar2D0Pi2KsPiPi ]
CharmStarList += ['Phys/StdDstar2D0Pi2KsPiPi/Particles']

CharmListSequence.Members += [StdD02KPiPi0]
CharmList += ['Phys/StdD02KPiPi0/Particles']

CharmListSequence.Members += [StdD02KPiPi0part]
CharmInclList += ['Phys/StdD02KPiPi0part/Particles']

CharmListSequence.Members += [StdD02Kepart]
CharmInclList += ['Phys/StdD02Kepart/Particles']

CharmListSequence.Members += [StdD02Kmupart]
CharmInclList += ['Phys/StdD02Kmupart/Particles']

CharmListSequence.Members += [StdDp2KPipart]
CharmInclList += ['Phys/StdDp2KPipart/Particles']

CharmListSequence.Members += [StdDp2Kepart]
CharmInclList += ['Phys/StdDp2Kepart/Particles']

CharmListSequence.Members += [StdDp2Kmupart]
CharmInclList += ['Phys/StdDp2Kmupart/Particles']



CharmListSequence.Members += [ StdDp2KPiPi]
CharmList += ['Phys/StdDp2KPiPi/Particles']

CharmListSequence.Members += [ StdDp2KsPi]
CharmList += ['Phys/StdDp2KsPi/Particles']



## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

# =============================================================================
# The END
# =============================================================================

