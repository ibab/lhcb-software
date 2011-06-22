__author__  = [ 'Benoit Viaud','Sandra Amato', 'Carlo Gobel', 'Patrick Spradlin' ]
__date__    = '$Date: 22-06-2011$'
__version__ = '$Revision: 0$'


from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, GeV, mm


from Hlt2Lines.Hlt2CharmSemilepD02HHMuMu2SoftMuonsLines import Hlt2CharmSemilepD02HHMuMu2SoftMuonsLinesConf
from Hlt2Lines.Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsLines import Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsLinesConf
from Hlt2Lines.Hlt2CharmSemilep2Had1MuForHHMuMu import Hlt2CharmSemilep2Had1MuForHHMuMuConf

## Must inherit from HltLinesConfigurableUser for slots to be updated by
## HltSettings, even though this configurable does not directly define a
## line.
class Hlt2CharmSemilepTwoHadForMuMuHHConf(HltLinesConfigurableUser) : # {
    """
    Defines a configurable to create a 2-body reconstruction that is used
    as a first stage for the D0 -> HHmumu prior to the SecondLoop *Muon*
    track reconstruction.

    NOTE:  Any Hlt2Line configurable that uses this MUST be on the list of
    __used_configurables__.
    """

    ## Must be configured before every configurable that uses it.
    __used_configurables__ = [   Hlt2CharmSemilepD02HHMuMu2SoftMuonsLinesConf, Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsLinesConf, Hlt2CharmSemilep2Had1MuForHHMuMuConf ]

    __slots__ = {
                  ## 2-body Input for 3Body
                    'Trk_PT_MIN'                :  500.0 * MeV
                  , 'Trk_P_MIN'                 : 5000.0 * MeV
                  , 'Trk_MIPCHI2DV_MIN'         :   4.0      # unitless
                  , 'Trk_TRCHI2DOF_MAX'         :    5.0      # unitless
                  , 'Pair_AMINDOCA_MAX'         :    0.12 * mm
                  , 'Pair_BPVVDCHI2_MIN'        :   20.0      # unitless  
                  , 'Pair_BPVVD_MIN'            :    0.0 * mm
                  , 'Pair_SumAPT_MIN'           :   0.0 * MeV
                  , 'Pair_BPVCORRM_MAX'         : 3500.0 * MeV
                  ##
                  , '__hlt2twoHadforHHMuMuSeq__'       : None
                }


    ## Sequence will be empty if called before configured.
    ## This is why it is imperative that client configurables be on the
    ##   __used_configurables__ list.
    def twoHadforHHMuMuSequence(self) : # {
        return self.getProp("__hlt2twoHadforHHMuMuSeq__")
    # }


    def __apply_configuration__(self) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons
        from HltTracking.HltPVs import PV3D
        

        twoHadDaughterCut = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
                             "& (PT> %(Trk_PT_MIN)s)" \
                             "& (P> %(Trk_P_MIN)s)" \
                             "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )" % self.getProps()
 
        twoHadCombCut = "(AM<2100*MeV)" \
                         "& ((APT1+APT2)> %(Pair_SumAPT_MIN)s)" \
                         "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMINDOCA_MAX)s )" \
                         "& (AALLSAMEBPV)" % self.getProps()

        twoHadMotherCut = "(BPVVD> %(Pair_BPVVD_MIN)s )" \
                           "& (BPVCORRM < %(Pair_BPVCORRM_MAX)s)" \
                           "& (BPVVDCHI2> %(Pair_BPVVDCHI2_MIN)s )" % self.getProps()
        #First stage - Combine 2 Body with pt > 500MeV        
        Charm2HadCombine = Hlt2Member( CombineParticles
                          , "Combine"
                          , DecayDescriptors = ["K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-"]
                          , DaughtersCuts = { "pi+" : twoHadDaughterCut,
                                              "K+"  : twoHadDaughterCut }
                          , CombinationCut = twoHadCombCut 
                          , MotherCut = twoHadMotherCut
                          , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons ])

        twoHadSeq = bindMembers("CharmSemilepTwoHadForMuMuHH",
                                [ BiKalmanFittedPions, BiKalmanFittedKaons, PV3D(), Charm2HadCombine ])
 
        self.setProp('__hlt2twoHadforHHMuMuSeq__', twoHadSeq)
    # }

# }
