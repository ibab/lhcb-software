"""
Configuration file for 'Standard D02Nbody (N>2) with MC assoc'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"

__all__ = (
    'StdMCD02KKPiPi' ,
    'StdMCD02KsKK' ,
    'StdMCD02KsPiPi' ,
    'StdMCD02KKMergedPi0' ,
    'StdMCD02KKResolvedPi0' ,
    'StdMCD02KKAnyPi0' ,
    'StdMCD02KPiMergedPi0' ,
    'StdMCD02KPiResolvedPi0',
    'StdMCD02KPiAnyPi0',
    'locations'	
	  )
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCKs, StandardMCKaons, StandardMCPions

#locations of nopidsparticles: 
npkaons =  "Phys/StdMCKaons/Particles"
nppions = "Phys/StdMCPions/Particles"
mpizeroes = "Phys/StdLooseMergedPi0/Particles"
rpizeroes = "Phys/StdLooseResolvedPi0/Particles"
mckshorts = "Phys/StdMCKs/Particles"

#matchKaons = "mcMatch( '[K+]cc' )"
#matchPions = "mcMatch( '[pi+]cc' )"
matchPizeroes = "mcMatch('pi0')"
#matchKshorts = "mcMatch('[KS0]cc')"

#Create matching strings
matchD02KKPiPi    = "(mcMatch('[D0 ==> K+ K- pi+ pi-]CC'))"
matchD02KsKK      = "(mcMatch('[D0 ==> KS0 K+ K-]CC'))"
matchD02KsPiPi    = "(mcMatch('[D0 ==> KS0 pi+ pi-]CC'))"
matchD02KKAnyPi0  = "(mcMatch('[D0 ==> K- K+ {pi0}]CC'))"
matchD02KKPi0     = "(mcMatch('[D0 ==> K- K+ pi0]CC'))"
matchD02KPiAnyPi0 = "(mcMatch('[D0 ==> K- pi+ {pi0}]CC'))"
matchD02KPiPi0    = "(mcMatch('[D0 ==> K- pi+ pi0]CC'))"


#Algorithms
StdMCD02KKPiPi=CombineParticles ('StdMCD02KKPiPi')
StdMCD02KKPiPi.Inputs = [ npkaons, nppions]
StdMCD02KKPiPi.DecayDescriptor = "[D0 -> K+ K- pi+ pi-]cc"
StdMCD02KKPiPi.MotherCut =  matchD02KKPiPi
#StdMCD02KKPiPi.DaughtersCuts = {
#		"K+" : matchKaons,
#		"pi+" : matchPions
#	}
StdMCD02KKPiPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCD02KKPiPi )

#Algorithms
StdMCD02KsKK=CombineParticles ('StdMCD02KsKK')
StdMCD02KsKK.Inputs = [ npkaons, mckshorts]
StdMCD02KsKK.DecayDescriptor = "[D0 -> KS0 K+ K-]cc"
StdMCD02KsKK.MotherCut =  matchD02KsKK
#StdMCD02KsKK.DaughtersCuts = {
#		"K+" : matchKaons,
#		"KS0" : matchKshorts
#	}
StdMCD02KsKK.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCD02KsKK )

StdMCD02KsPiPi=CombineParticles ('StdMCD02KsPiPi')
StdMCD02KsPiPi.Inputs = [ nppions, mckshorts]
StdMCD02KsPiPi.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc"
StdMCD02KsPiPi.MotherCut =  matchD02KsPiPi 
#StdMCD02KsPiPi.DaughtersCuts = {
#		"pi+" : matchPions,
#		"KS0" : matchKshorts
#	}
StdMCD02KsPiPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCD02KsPiPi )

StdMCD02KKMergedPi0=CombineParticles ('StdMCD02KKMergedPi0')
StdMCD02KKMergedPi0.Inputs = [ npkaons, mpizeroes]
StdMCD02KKMergedPi0.DecayDescriptor = "[D0 -> K- K+ pi0]cc"
StdMCD02KKMergedPi0.MotherCut =  matchD02KKPi0
StdMCD02KKMergedPi0.DaughtersCuts = {
#		"K+" : matchKaons,
		"pi0" : matchPizeroes
	}
StdMCD02KKMergedPi0.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCD02KKMergedPi0 )

StdMCD02KKResolvedPi0=CombineParticles ('StdMCD02KKResolvedPi0')
StdMCD02KKResolvedPi0.Inputs = [ npkaons, rpizeroes]
StdMCD02KKResolvedPi0.DecayDescriptor = "[D0 -> K- K+ pi0]cc"
StdMCD02KKResolvedPi0.MotherCut =  matchD02KKPi0
StdMCD02KKResolvedPi0.DaughtersCuts = {
#		"K+" : matchKaons,
		"pi0" : matchPizeroes
	}
StdMCD02KKResolvedPi0.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCD02KKResolvedPi0 )

StdMCD02KKAnyPi0=CombineParticles ('StdMCD02KKAnyPi0')
StdMCD02KKAnyPi0.Inputs = [ npkaons]
StdMCD02KKAnyPi0.DecayDescriptor = "[D0 -> K- K+]cc"
StdMCD02KKAnyPi0.MotherCut =  matchD02KKAnyPi0
#StdMCD02KKAnyPi0.DaughtersCuts = {
#		"K+" : matchKaons
#	}
StdMCD02KKAnyPi0.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCD02KKAnyPi0 )

StdMCD02KPiMergedPi0=CombineParticles ('StdMCD02KPiMergedPi0')
StdMCD02KPiMergedPi0.Inputs = [ npkaons, nppions, mpizeroes]
StdMCD02KPiMergedPi0.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StdMCD02KPiMergedPi0.MotherCut =  matchD02KPiPi0
StdMCD02KPiMergedPi0.DaughtersCuts = {
#	"K+" : matchKaons,
#	"pi+" : matchPions,
	"pi0" : matchPizeroes
	}
StdMCD02KPiMergedPi0.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCD02KPiMergedPi0 )

StdMCD02KPiResolvedPi0=CombineParticles ('StdMCD02KPiResolvedPi0')
StdMCD02KPiResolvedPi0.Inputs = [ npkaons, nppions, rpizeroes]
StdMCD02KPiResolvedPi0.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StdMCD02KPiResolvedPi0.MotherCut =  matchD02KPiPi0
StdMCD02KPiResolvedPi0.DaughtersCuts = {
#	"K+" : matchKaons,
#	"pi+" : matchPions,
	"pi0" : matchPizeroes
	}
StdMCD02KPiResolvedPi0.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCD02KPiResolvedPi0 )

StdMCD02KPiAnyPi0=CombineParticles ('StdMCD02KPiAnyPi0')
StdMCD02KPiAnyPi0.Inputs = [ npkaons, nppions]
StdMCD02KPiAnyPi0.DecayDescriptor = "[D0 -> K- pi+]cc"
StdMCD02KPiAnyPi0.MotherCut =  matchD02KPiAnyPi0
#StdMCD02KPiAnyPi0.DaughtersCuts = {
#	        "K+" : matchKaons,
#	        "pi+" : matchPions
#	}
StdMCD02KPiAnyPi0.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCD02KPiAnyPi0 )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

