"""
Stripping lines for B -> h h pi0
both resolved and merged pi0's
modification of StrippingB2HHPi0.py by Regis Lefevre
"""

__author__  = "Jason E Andrews"
__date__    = "29/08/2013"
__version__ = "1.0"
__all__     = ( "StrippingBu2Kpi0Conf",
                "makeBu2Kpi0Resolved",
                "makeBu2Kpi0Merged")

from Gaudi.Configuration import *

#from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from Configurables import FilterDesktop
from StandardParticles                     import StdNoPIDsKaons, StdLooseMergedPi0, StdLooseResolvedPi0

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder


default_config = {
    "KuMinPTasy"     : -0.6,
    "RKuMIPCHI2DV"   : 25,
    "KuMinPROBNNK"   : 0.8,
    "KuMaxTRCHI2DOF" : 1.5,
    "Rpi0MinP"       : 10000,
    "Rpi0MinPT"      : 1600,
    "Rpi0MinCL"      : 0.2,
    "RBuMinM"        : 4800, # 4550 4800
    "RBuMaxM"        : 5800, # 6050 5800
    "RBuMinPT"       : 0,
    "PrescaleBu2Kpi0Resolved" : 1.0,
    "MKuMIPCHI2DV"   : 49,
    "Mpi0MinP"       : 10000,
    "Mpi0MinPT"      : 2600,
    "Mpi0MinCL"      : 0.9,
    "MBuMinM"        : 4000,
    "MBuMaxM"        : 6050,
    "MBuMinPT"       : 1500,
    "PrescaleBu2Kpi0Merged"   : 1.0
    }

class StrippingBu2Kpi0Conf(LineBuilder) :

    __configuration_keys__ = ( "KuMinPTasy",
                               "RKuMIPCHI2DV",
                               "KuMinPROBNNK",
                               "KuMaxTRCHI2DOF",
			       "Rpi0MinP",
			       "Rpi0MinPT",
			       "Rpi0MinCL",
			       "RBuMinM",
			       "RBuMaxM",
                               "RBuMinPT",
			       "PrescaleBu2Kpi0Resolved",
			       "MKuMIPCHI2DV",
			       "Mpi0MinP",
			       "Mpi0MinPT",
			       "Mpi0MinCL",
                               "MBuMinM",
                               "MBuMaxM",
                               "MBuMinPT",
                               "PrescaleBu2Kpi0Merged"
                               )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        Bu2Kpi0Name = "Bu2Kpi0"

        PTasyFilter = FilterDesktop( "PTasyFilter",
                                     Preambulo = [ "ptC1 = SUMCONE ( 1.2 , PT , '/Event/Phys/StdNoPIDsPions/Particles' )" ],
                                     Code = "( PT - ptC1 ) / ( ptC1 + PT ) > %(KuMinPTasy)s" % locals()["config"]
                                     )

        filteredKaons = Selection( "FilteredKaons",
                                   Algorithm = PTasyFilter,
                                   RequiredSelections = [ StdNoPIDsKaons ]
                                   )
        
	self.selResolved = makeBu2Kpi0Resolved( Bu2Kpi0Name + "Resolved",
                                                config,
                                                DecayDescriptor = "[B+ -> K+ pi0]cc",
#                                                inputSel = [ StdNoPIDsKaons, StdLooseResolvedPi0 ]
                                                inputSel = [ filteredKaons, StdLooseResolvedPi0 ]
                                                )				       

        self.selMerged = makeBu2Kpi0Merged( Bu2Kpi0Name + "Merged",
                                            config,
                                            DecayDescriptor = "[B+ -> K+ pi0]cc",
#                                            inputSel = [ StdNoPIDsKaons, StdLooseMergedPi0 ]
                                            inputSel = [ filteredKaons, StdLooseMergedPi0 ]
                                            )

        self.Bu2Kpi0Resolved_line = StrippingLine(Bu2Kpi0Name + "ResolvedLine",
                                                  prescale = config["PrescaleBu2Kpi0Resolved"],
                                                  checkPV = True,
                                                  algos = [ self.selResolved ]
                                                  )
        self.Bu2Kpi0Merged_line = StrippingLine(Bu2Kpi0Name + "MergedLine",
                                                prescale = config["PrescaleBu2Kpi0Merged"],
                                                selection = self.selMerged
                                                )

        self.registerLine(self.Bu2Kpi0Resolved_line)
        self.registerLine(self.Bu2Kpi0Merged_line)


def makeBu2Kpi0Resolved( name,
                         config,
                         DecayDescriptor,
                         inputSel
                         ) :

    _KuCuts = "(PROBNNK>%(KuMinPROBNNK)s) & (MIPCHI2DV(PRIMARY)>%(RKuMIPCHI2DV)s) & (TRCHI2DOF<%(KuMaxTRCHI2DOF)s)" % locals()["config"]
    _pi0Cuts = "(P>%(Rpi0MinP)s *MeV) & (PT>%(Rpi0MinPT)s *MeV) & (CL>%(Rpi0MinCL)s)" % locals()["config"]
    _daughterCuts = { "K+" : _KuCuts, "pi0" : _pi0Cuts }
    _combCuts = "(AM>%(RBuMinM)s *MeV) & (AM<%(RBuMaxM)s *MeV)" % locals()["config"]
    _motherCuts = "(PT>%(RBuMinPT)s *MeV)" % locals()["config"]

    _B = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts,
                           ParticleCombiners = { "" : "ParticleAdder" }
                           )

    return Selection( name + "Sel",
                      Algorithm = _B,
                      RequiredSelections = inputSel
                      )

def makeBu2Kpi0Merged( name,
                       config,
                       DecayDescriptor,
                       inputSel
                       ) :

    _KuCuts = "(PROBNNK>%(KuMinPROBNNK)s) & (MIPCHI2DV(PRIMARY)>%(MKuMIPCHI2DV)s) & (TRCHI2DOF<%(KuMaxTRCHI2DOF)s)" % locals()["config"]
    _pi0Cuts = "(P>%(Mpi0MinP)s *MeV) & (PT>%(Mpi0MinPT)s *MeV) & (CL>%(Mpi0MinCL)s)" % locals()["config"]
    _daughterCuts = { "K+" : _KuCuts, "pi0" : _pi0Cuts }
    _combCuts = "(AM>%(MBuMinM)s *MeV) & (AM<%(MBuMaxM)s *MeV)" % locals()["config"]
    _motherCuts = "(PT>%(MBuMinPT)s *MeV)" % locals()["config"]

    _B = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts,
                           ParticleCombiners = { "" : "ParticleAdder" }
                           )

    return Selection( name + "Sel",
                      Algorithm = _B,
                      RequiredSelections = inputSel
                      )
