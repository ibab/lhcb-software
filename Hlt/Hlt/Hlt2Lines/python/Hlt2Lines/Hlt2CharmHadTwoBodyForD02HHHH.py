__author__  = [ 'Sandra Amato', 'Carlo Gobel', 'Patrick Spradlin', 'Benoit Viaud' ]
__date__    = '$Date: 24/02/2012$'
__version__ = '$Revision: 2$'


from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, GeV, mm

from Hlt2Lines.Hlt2CharmHadD02HHHHDstLines import Hlt2CharmHadD02HHHHDstLinesConf 
from Hlt2Lines.Hlt2CharmHadD02HHHHDstNoHlt1Lines  import Hlt2CharmHadD02HHHHDstNoHlt1LinesConf 

## Must inherit from HltLinesConfigurableUser for slots to be updated by
## HltSettings, even though this configurable does not directly define a
## line.
class Hlt2CharmHadTwoBodyForD02HHHHConf(HltLinesConfigurableUser) : # {
    """
    Defines a configurable to create a 2-body reconstruction that is used
    as a first stage for the D -> 4H.

    NOTE:  Any Hlt2Line configurable that uses this MUST be on the list of
    __used_configurables__.
    """

    ## Must be configured before every configurable that uses it.
    __used_configurables__ = [Hlt2CharmHadD02HHHHDstLinesConf,Hlt2CharmHadD02HHHHDstNoHlt1LinesConf]

    __slots__ = {
                    'Trk_PT_MIN'                :  300.0 * MeV
                  , 'Trk_P_MIN'                 : 3000.0 * MeV
                  , 'Trk_MIPCHI2DV_MIN'         :    6.      # unitless
                  , 'Trk_TRCHI2DOF_MAX'         :    3.0      # unitless
                  , 'Pair_AMINDOCA_MAX'         :    0.1 * mm
                  , 'Pair_BPVVDCHI2_MIN'        :   20.0      # unitless  
                  , 'Pair_BPVVD_MIN'            :    0.0 * mm
                  , 'Pair_SumAPT_MIN'           :    0.0 * MeV
                  , 'Pair_BPVCORRM_MAX'         : 3500.0 * MeV
                  , 'name_prefix'               : 'CharmHadTwoBodyForD02HHHH'
                  ##
                  , '__hlt2TwoBodySeq__'                        : None
                  , '__hlt2TwoBodyHHXSeq__'                     : None  
                  , '__hlt2TwoBodyHHXWithKSLLSeq__'             : None
                  , '__hlt2TwoBodyHHXWithKSDDSeq__'             : None
                  , '__hlt2TwoBodyBaryonHHXSeq__'               : None
                  , '__hlt2TwoBodyBaryonHHXWithKSLLSeq__'       : None
                  , '__hlt2TwoBodyBaryonHHXWithLambda0LLSeq__'  : None 
                  , '__hlt2TwoBodyBaryonHHXWithKSDDSeq__'       : None
                  , '__hlt2TwoBodyBaryonHHXWithLambda0DDSeq__'  : None 
                  , '__hlt2TwoBodyLeptonHHXSeq__'               : None  
                  , '__hlt2TwoBodyLeptonHHXWithKSLLSeq__'       : None
                  , '__hlt2TwoBodyLeptonHHXWithLambda0LLSeq__'  : None
                  , '__hlt2TwoBodyLeptonHHXWithKSDDSeq__'       : None
                  , '__hlt2TwoBodyLeptonHHXWithLambda0DDSeq__'  : None
                }


    ## Sequence will be empty if called before configured.
    ## This is why it is imperative that client configurables be on the
    ##   __used_configurables__ list.
    def twoBodySequence(self) : # {
        return self.getProp("__hlt2TwoBodySeq__")
    # }

    def twoBodyHHXSequence(self) :
        return self.getProp("__hlt2TwoBodyHHXSeq__")

    def twoBodyHHXWithKSLLSequence(self) :
        return self.getProp("__hlt2TwoBodyHHXWithKSLLSeq__")
    
    def twoBodyHHXWithKSDDSequence(self) :
        return self.getProp("__hlt2TwoBodyHHXWithKSDDSeq__")

    def twoBodyBaryonHHXSequence(self) :
        return self.getProp("__hlt2TwoBodyBaryonHHXSeq__")

    def twoBodyBaryonHHXWithKSLLSequence(self) :
        return self.getProp("__hlt2TwoBodyBaryonHHXWithKSLLSeq__")

    def twoBodyBaryonHHXWithLambda0LLSequence(self) :
        return self.getProp("__hlt2TwoBodyBaryonHHXWithLambda0LLSeq__")

    def twoBodyBaryonHHXWithKSDDSequence(self) :
        return self.getProp("__hlt2TwoBodyBaryonHHXWithKSDDSeq__")

    def twoBodyBaryonHHXWithLambda0DDSequence(self) :
        return self.getProp("__hlt2TwoBodyBaryonHHXWithLambda0DDSeq__")
    
    def twoBodyLeptonHHXSequence(self) :
        return self.getProp("__hlt2TwoBodyLeptonHHXSeq__")

    def twoBodyLeptonHHXWithKSLLSequence(self) :
        return self.getProp("__hlt2TwoBodyLeptonHHXWithKSLLSeq__")

    def twoBodyLeptonHHXWithLambda0LLSequence(self) :
        return self.getProp("__hlt2TwoBodyLeptonHHXWithLambda0LLSeq__")

    def twoBodyLeptonHHXWithKSDDSequence(self) :
        return self.getProp("__hlt2TwoBodyLeptonHHXWithKSDDSeq__")

    def twoBodyLeptonHHXWithLambda0DDSequence(self) :
        return self.getProp("__hlt2TwoBodyLeptonHHXWithLambda0DDSeq__")

    def __apply_configuration__(self) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedProtons, BiKalmanFittedMuons
        from Hlt2SharedParticles.Lambda import LambdaLLTrackFitted as Lambda0LLData 
        from Hlt2SharedParticles.Lambda import LambdaDDTrackFitted as Lambda0DDData
        from Hlt2SharedParticles.Ks import KsLLTF, KsDD
        from HltTracking.HltPVs import PV3D

        twoBodyDaughterCut = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
                             "& (PT> %(Trk_PT_MIN)s)" \
                             "& (P> %(Trk_P_MIN)s)" \
                             "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )" % self.getProps()
 
        twoBodyCombCut = "(AM<2100*MeV)" \
                         "& ((APT1+APT2)> %(Pair_SumAPT_MIN)s)" \
                         "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMINDOCA_MAX)s )" \
                         "& (AALLSAMEBPV)" % self.getProps()

        twoBodyCombCut_V0 = "(AM<2100*MeV)" \
                         "& ((APT1+APT2)> %(Pair_SumAPT_MIN)s)" % self.getProps()

        twoBodyMotherCut = "(BPVVD> %(Pair_BPVVD_MIN)s )" \
                           "& (BPVCORRM < %(Pair_BPVCORRM_MAX)s)" \
                           "& (BPVVDCHI2> %(Pair_BPVVDCHI2_MIN)s )" % self.getProps()
        #First stage - Combine 2 Body with pt > 500MeV        
        Charm2BodyCombine = Hlt2Member( CombineParticles
                          , "Combine"
                          , DecayDescriptors = ["K*(892)0 -> pi+ pi-","K*(892)+ -> K+ K+","K*(892)- -> K- K-"]
                          , DaughtersCuts = { "pi+" : twoBodyDaughterCut,
                                              "K+"  : twoBodyDaughterCut }
                          , CombinationCut = twoBodyCombCut 
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons ])

        CharmHHXCombine = Hlt2Member( CombineParticles
                          , "CombineHHX"                          
                          , DecayDescriptors = ["K*(892)0 -> pi+ pi-","K*(892)0 -> K+ K-",
                                                "K*(892)0 -> K- pi+","K*(892)0 -> K+ pi-"]
                          , DaughtersCuts = { "pi+" : twoBodyDaughterCut,
                                              "K+"  : twoBodyDaughterCut }
                          , CombinationCut = twoBodyCombCut 
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons ])

        CharmHHXCombineWithKSLL = Hlt2Member( CombineParticles
                          , "CombineHHXWithKSLL"                  
                          , DecayDescriptors = ["K*(892)0 -> K- KS0","K*(892)0 -> K+ KS0",
                                                "K*(892)0 -> pi- KS0","K*(892)0 -> pi+ KS0" ]
                          , DaughtersCuts = { "KS0" : "ALL",
                                              "pi+" : twoBodyDaughterCut,
                                              "K+"  : twoBodyDaughterCut }
                          , CombinationCut = twoBodyCombCut_V0 
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons,KsLLTF ])

        CharmHHXCombineWithKSDD = Hlt2Member( CombineParticles
                          , "CombineHHXWithKSDD"
                          , DecayDescriptors = ["K*(892)0 -> K- KS0","K*(892)0 -> K+ KS0",
                                                "K*(892)0 -> pi- KS0","K*(892)0 -> pi+ KS0" ]
                          , DaughtersCuts = { "KS0" : "ALL",
                                              "pi+" : twoBodyDaughterCut,
                                              "K+"  : twoBodyDaughterCut }
                          , CombinationCut = twoBodyCombCut_V0
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons,KsDD ])

        CharmHHXBaryonCombine = Hlt2Member( CombineParticles
                          , "CombineBaryonHHX"    
                          , DecayDescriptors = ["K*(892)0 -> p+ pi-","K*(892)0 -> p+ K-",
                                                "K*(892)0 -> p~- pi+","K*(892)0 -> p~- K+", 
                                                "K*(892)0 -> p+ p~-","K*(892)0 -> p+ p+","K*(892)0 -> p~- p~-"]
                          , DaughtersCuts = { "p+"  : twoBodyDaughterCut,
                                              "pi+" : twoBodyDaughterCut,
                                              "K+"  : twoBodyDaughterCut }
                          , CombinationCut = twoBodyCombCut 
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons, BiKalmanFittedProtons])

        CharmHHXBaryonCombineWithKSLL = Hlt2Member( CombineParticles
                          , "CombineBaryonHHXWithKSLL"        
                          , DecayDescriptors = ["K*(892)0 -> p~- KS0","K*(892)0 -> p+ KS0"]
                          , DaughtersCuts = { "KS0" : "ALL",
                                              "p+"  : twoBodyDaughterCut}
                          , CombinationCut = twoBodyCombCut_V0 
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedProtons,KsLLTF])

        CharmHHXBaryonCombineWithLambda0LL = Hlt2Member( CombineParticles
                          , "CombineBaryonHHXWithLambda0LL"
                          , DecayDescriptors = ["K*(892)0 -> Lambda0 pi-","K*(892)0 -> Lambda0 K-",
                                                "K*(892)0 -> Lambda0 pi+","K*(892)0 -> Lambda0 K+",
                                                "K*(892)0 -> Lambda~0 pi-","K*(892)0 -> Lambda~0 K-",
                                                "K*(892)0 -> Lambda~0 pi+","K*(892)0 -> Lambda~0 K+"]
                          , DaughtersCuts = { "Lambda0" : "ALL",
                                              "pi+" : twoBodyDaughterCut,
                                              "K+"  : twoBodyDaughterCut }
                          , CombinationCut = twoBodyCombCut_V0
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons, Lambda0LLData ])

        CharmHHXBaryonCombineWithKSDD = Hlt2Member( CombineParticles
                          , "CombineBaryonHHXWithKSDD"
                          , DecayDescriptors = ["K*(892)0 -> p~- KS0","K*(892)0 -> p+ KS0"]
                          , DaughtersCuts = { "KS0" : "ALL",
                                              "p+"  : twoBodyDaughterCut}
                          , CombinationCut = twoBodyCombCut_V0
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedProtons,KsDD])

        CharmHHXBaryonCombineWithLambda0DD = Hlt2Member( CombineParticles
                          , "CombineBaryonHHXWithLambda0DD"
                          , DecayDescriptors = ["K*(892)0 -> Lambda0 pi-","K*(892)0 -> Lambda0 K-",
                                                "K*(892)0 -> Lambda0 pi+","K*(892)0 -> Lambda0 K+",
                                                "K*(892)0 -> Lambda~0 pi-","K*(892)0 -> Lambda~0 K-",
                                                "K*(892)0 -> Lambda~0 pi+","K*(892)0 -> Lambda~0 K+"]
                          , DaughtersCuts = { "Lambda0" : "ALL",
                                              "pi+" : twoBodyDaughterCut,
                                              "K+"  : twoBodyDaughterCut }
                          , CombinationCut = twoBodyCombCut_V0
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons, Lambda0DDData ])
        
        CharmHHXLeptonCombine = Hlt2Member( CombineParticles
                          , "CombineLeptonHHX"    
                          , DecayDescriptors = ["K*(892)0 -> mu+ pi-","K*(892)0 -> mu+ K-", "K*(892)0 -> mu+ p~-",
                                                "K*(892)0 -> mu- pi+","K*(892)0 -> mu- K+", "K*(892)0 -> mu- p+",
                                                "K*(892)0 -> mu+ mu-","K*(892)0 -> mu+ mu+","K*(892)0 -> mu- mu-"]
                          , DaughtersCuts = { "mu+" : twoBodyDaughterCut,
                                              "p+"  : twoBodyDaughterCut,
                                              "pi+" : twoBodyDaughterCut,
                                              "K+"  : twoBodyDaughterCut }
                          , CombinationCut = twoBodyCombCut 
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons, BiKalmanFittedProtons, BiKalmanFittedMuons])

        CharmHHXLeptonCombineWithKSLL = Hlt2Member( CombineParticles
                          , "CombineLeptonHHXWithKSLL"
                          , DecayDescriptors = ["K*(892)0 -> mu- KS0","K*(892)0 -> mu+ KS0"]
                          , DaughtersCuts = { "KS0" : "ALL",
                                              "mu+" : twoBodyDaughterCut}
                          , CombinationCut = twoBodyCombCut_V0
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedMuons, KsLLTF])

        CharmHHXLeptonCombineWithLambda0LL = Hlt2Member( CombineParticles
                          , "CombineLeptonHHXWithLambda0LL"
                          , DecayDescriptors = ["K*(892)0 -> Lambda~0 mu-","K*(892)0 -> Lambda0 mu-",
                                                "K*(892)0 -> Lambda~0 mu+","K*(892)0 -> Lambda0 mu+"]
                          , DaughtersCuts = { "Lambda0" : "ALL",
                                              "mu+" : twoBodyDaughterCut}
                          , CombinationCut = twoBodyCombCut_V0
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedMuons,Lambda0LLData ])

        CharmHHXLeptonCombineWithKSDD = Hlt2Member( CombineParticles
                          , "CombineLeptonHHXWithKSDD"
                          , DecayDescriptors = ["K*(892)0 -> mu- KS0","K*(892)0 -> mu+ KS0"]
                          , DaughtersCuts = { "KS0" : "ALL",
                                              "mu+" : twoBodyDaughterCut}
                          , CombinationCut = twoBodyCombCut_V0
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedMuons, KsDD])

        CharmHHXLeptonCombineWithLambda0DD = Hlt2Member( CombineParticles
                          , "CombineLeptonHHXWithLambda0DD"
                          , DecayDescriptors = ["K*(892)0 -> Lambda~0 mu-","K*(892)0 -> Lambda0 mu-",
                                                "K*(892)0 -> Lambda~0 mu+","K*(892)0 -> Lambda0 mu+"]
                          , DaughtersCuts = { "Lambda0" : "ALL",
                                              "mu+" : twoBodyDaughterCut}
                          , CombinationCut = twoBodyCombCut_V0
                          , MotherCut = twoBodyMotherCut
                          , Inputs = [ BiKalmanFittedMuons,Lambda0DDData ])

        twoBodySeq = bindMembers(self.getProp('name_prefix'), [ BiKalmanFittedPions, BiKalmanFittedKaons, PV3D(), Charm2BodyCombine ])
        #
        twoBodyHHXSeq = bindMembers(self.getProp('name_prefix')+"HHX", [ BiKalmanFittedPions, BiKalmanFittedKaons, 
                                                                         PV3D(), CharmHHXCombine ])
        twoBodyHHXWithKSLLSeq = bindMembers(self.getProp('name_prefix')+"HHXWithKSLL", [ BiKalmanFittedPions, BiKalmanFittedKaons,
                                                                         KsLLTF, PV3D(), CharmHHXCombineWithKSLL ])
        twoBodyHHXWithKSDDSeq = bindMembers(self.getProp('name_prefix')+"HHXWithKSDD", [ BiKalmanFittedPions, BiKalmanFittedKaons,
                                                                         KsDD, PV3D(), CharmHHXCombineWithKSDD ])
        #
        twoBodyBaryonHHXSeq = bindMembers(self.getProp('name_prefix')+"BaryonHHX", 
                                [ BiKalmanFittedPions, BiKalmanFittedKaons, BiKalmanFittedProtons, 
                                  PV3D(), CharmHHXBaryonCombine ])
        twoBodyBaryonHHXWithKSLLSeq = bindMembers(self.getProp('name_prefix')+"BaryonHHXWithKSLL",
                                [ BiKalmanFittedProtons, KsLLTF, PV3D(), CharmHHXBaryonCombineWithKSLL ])
        twoBodyBaryonHHXWithLambda0LLSeq = bindMembers(self.getProp('name_prefix')+"BaryonHHXWithLambda0LL",
                                [ BiKalmanFittedPions, BiKalmanFittedKaons, 
                                  Lambda0LLData, PV3D(), CharmHHXBaryonCombineWithLambda0LL ])
        twoBodyBaryonHHXWithKSDDSeq = bindMembers(self.getProp('name_prefix')+"BaryonHHXWithKSDD",
                                [ BiKalmanFittedProtons, KsDD, PV3D(), CharmHHXBaryonCombineWithKSDD ])
        twoBodyBaryonHHXWithLambda0DDSeq = bindMembers(self.getProp('name_prefix')+"BaryonHHXWithLambda0DD",
                                [ BiKalmanFittedPions, BiKalmanFittedKaons, 
                                  Lambda0DDData, PV3D(), CharmHHXBaryonCombineWithLambda0DD ])
        #
        twoBodyLeptonHHXSeq = bindMembers(self.getProp('name_prefix')+"LeptonHHX", 
                                [ BiKalmanFittedPions, BiKalmanFittedKaons, BiKalmanFittedProtons, 
                                  BiKalmanFittedMuons, PV3D(), CharmHHXLeptonCombine ])
        twoBodyLeptonHHXWithKSLLSeq = bindMembers(self.getProp('name_prefix')+"LeptonHHXWithKSLL",
                                [ BiKalmanFittedMuons, KsLLTF, PV3D(), CharmHHXLeptonCombineWithKSLL ])
        twoBodyLeptonHHXWithLambda0LLSeq = bindMembers(self.getProp('name_prefix')+"LeptonHHXWithLambda0LL",
                                [ BiKalmanFittedMuons, Lambda0LLData, PV3D(), CharmHHXLeptonCombineWithLambda0LL ])
        twoBodyLeptonHHXWithKSDDSeq = bindMembers(self.getProp('name_prefix')+"LeptonHHXWithKSDD",
                                [ BiKalmanFittedMuons, KsDD, PV3D(), CharmHHXLeptonCombineWithKSDD ])
        twoBodyLeptonHHXWithLambda0DDSeq = bindMembers(self.getProp('name_prefix')+"LeptonHHXWithLambda0DD",
                                [ BiKalmanFittedMuons, Lambda0DDData, PV3D(), CharmHHXLeptonCombineWithLambda0DD ])  

        self.setProp('__hlt2TwoBodySeq__', twoBodySeq)
        #
        self.setProp('__hlt2TwoBodyHHXSeq__',twoBodyHHXSeq)
        self.setProp('__hlt2TwoBodyHHXWithKSLLSeq__',twoBodyHHXWithKSLLSeq)
        self.setProp('__hlt2TwoBodyHHXWithKSDDSeq__',twoBodyHHXWithKSDDSeq)
        #
        self.setProp('__hlt2TwoBodyBaryonHHXSeq__',twoBodyBaryonHHXSeq)
        self.setProp('__hlt2TwoBodyBaryonHHXWithKSLLSeq__',twoBodyBaryonHHXWithKSLLSeq)
        self.setProp('__hlt2TwoBodyBaryonHHXWithLambda0LLSeq__',twoBodyBaryonHHXWithLambda0LLSeq)
        self.setProp('__hlt2TwoBodyBaryonHHXWithKSDDSeq__',twoBodyBaryonHHXWithKSDDSeq)
        self.setProp('__hlt2TwoBodyBaryonHHXWithLambda0DDSeq__',twoBodyBaryonHHXWithLambda0DDSeq)
        #
        self.setProp('__hlt2TwoBodyLeptonHHXSeq__',twoBodyLeptonHHXSeq)
        self.setProp('__hlt2TwoBodyLeptonHHXWithKSLLSeq__',twoBodyLeptonHHXWithKSLLSeq)
        self.setProp('__hlt2TwoBodyLeptonHHXWithLambda0LLSeq__',twoBodyLeptonHHXWithLambda0LLSeq)
        self.setProp('__hlt2TwoBodyLeptonHHXWithKSDDSeq__',twoBodyLeptonHHXWithKSDDSeq)
        self.setProp('__hlt2TwoBodyLeptonHHXWithLambda0DDSeq__',twoBodyLeptonHHXWithLambda0DDSeq)
    # }

# }
