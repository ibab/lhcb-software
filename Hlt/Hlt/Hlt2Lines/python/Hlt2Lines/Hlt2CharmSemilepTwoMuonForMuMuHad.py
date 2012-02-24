__author__  = [ 'B. Viaud', 'Sandra Amato', 'Carlo Gobel', 'Patrick Spradlin' ]
__date__    = '$Date: 07-01-2012$'
__version__ = '$Revision: 1$'


from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, GeV, mm

from Hlt2Lines.Hlt2CharmSemilepD02HHMuMuLines   import Hlt2CharmSemilepD02HHMuMuLinesConf
from Hlt2Lines.Hlt2CharmSemilepD2HMuMuLines   import Hlt2CharmSemilepD2HMuMuLinesConf


## Must inherit from HltLinesConfigurableUser for slots to be updated by
## HltSettings, even though this configurable does not directly define a
## line.
class Hlt2CharmSemilepTwoMuonForMuMuHadConf(HltLinesConfigurableUser) : # {
    """
    Defines a configurable to create a 2-muon reconstruction that is used
    as a first stage for the D -> Hmumu and D -> HHmumu lines prior to the SecondLoop
    track reconstruction.

    NOTE:  Any Hlt2Line configurable that uses this MUST be on the list of
    __used_configurables__.
    """

    ## Must be configured before every configurable that uses it.
    __used_configurables__ = [ Hlt2CharmSemilepD02HHMuMuLinesConf, Hlt2CharmSemilepD2HMuMuLinesConf]

    __slots__ = {
                  ## 2-body Input for 3Body
                    'Trk_PT_MIN'                :  300.0 * MeV# 500.0 /300
                  , 'Trk_P_MIN'                 : 3000.0 * MeV # 5000.0 /3000
                  , 'Trk_MIPCHI2DV_MIN'         :    2.0      # 2.0 /0  unitless
                  , 'Trk_TRCHI2DOF_MAX'         :    4.0      # unitless
                  , 'Pair_AMINDOCA_MAX'         :    0.1 * mm
                  , 'Pair_BPVVDCHI2_MIN'        :    9.0      # 0.0 unitless
                  , 'Pair_BPVVD_MIN'            :    0.0 * mm
                  , 'Pair_SumAPT_MIN'           : 0.0 * MeV
                  , 'Pair_BPVCORRM_MAX'         : 3500.0 * MeV
                  ##
                  , '__hlt2TwoMuonSeq__'       : None
                }


    ## Sequence will be empty if called before configured.
    ## This is why it is imperative that client configurables be on the
    ##   __used_configurables__ list.
    def twoMuonSequence(self) : # {
        return self.getProp("__hlt2TwoMuonSeq__")
    # }


    def __apply_configuration__(self) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons
        from HltTracking.HltPVs import PV3D
        

        twoMuonDaughterCut = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
                             "& (PT> %(Trk_PT_MIN)s)" \
                             "& (P> %(Trk_P_MIN)s)" \
                             "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )" % self.getProps()
 
        twoMuonCombCut = "(AM<2100*MeV)" \
                         "& ((APT1+APT2)> %(Pair_SumAPT_MIN)s)" \
                         "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMINDOCA_MAX)s )" \
                         "& (AALLSAMEBPV)" % self.getProps()

        twoMuonMotherCut = "(BPVVD> %(Pair_BPVVD_MIN)s )" \
                           "& (BPVCORRM < %(Pair_BPVCORRM_MAX)s)" \
                           "& (BPVVDCHI2> %(Pair_BPVVDCHI2_MIN)s )" % self.getProps()
        #First stage - Combine 2 Body with pt > 500MeV        
        Charm2MuonCombine = Hlt2Member( CombineParticles
                          , "Combine"
                          , DecayDescriptors = ["J/psi(1S) -> mu+ mu-","phi(1020) -> mu+ mu+", " rho(770)0 -> mu- mu-"]
                          , DaughtersCuts = { "mu+" : twoMuonDaughterCut}
                          , CombinationCut = twoMuonCombCut 
                          , MotherCut = twoMuonMotherCut
                          , Inputs = [BiKalmanFittedMuons])

        twoMuonSeq = bindMembers('CharmCharmSemilepTwoMuonForMuMuHad', [ PV3D(), BiKalmanFittedMuons,  Charm2MuonCombine ])
 
        self.setProp('__hlt2TwoMuonSeq__', twoMuonSeq)
    # }A

# }
