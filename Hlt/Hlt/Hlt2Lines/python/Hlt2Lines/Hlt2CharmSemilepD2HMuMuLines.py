__author__  = [ 'Benoit F. Viaud' ]
__date__    = '$Date: 27/03/2011$'
__version__ = '$Revision: 0.$'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2CharmSemilepD2HMuMuLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## Hmumu
                    'TrkPt_Hmumu'                 : 350.0 * MeV
                    , 'TrkP_Hmumu'                  : 2500.0 * MeV
                    , 'TrkPVIPChi2_Hmumu'           : 0.0      # unitless
                    , 'TrkPVIPChi2MAX_Hmumu'           : 9.0      # unitless
                    ,  'TrkPtMAX_Hmumu'                 : 1000.0 * MeV
                    , 'TrkChi2_Hmumu'               : 5.0      # unitless
                    , 'PairMinDoca_Hmumu'           : 0.10 * mm
                    , 'PairMaxDoca_Hmumu'           : 0.2 * mm
                    , 'VtxPVDispChi2_Hmumu'         : 4.0    # unitless
                    , 'VtxChi2_Hmumu'               : 15.0     # unitless
                    , 'DIPChi2_Hmumu'               : 36.0     # unitless
                    , 'DSumPt_Hmumu'                : 2000.0 * MeV
                    ,'DDira'              : 0.9996     # adimensional
                    , 'MCOR_MAX_Hmumu'              : 3500.0 * MeV
                    , 'Sig_M_MIN'                   : 1800.0 * MeV
                    , 'Sig_M_MAX'                   : 2000.0 * MeV
                    , 'WideMass_M_MIN'              : 1700.0 * MeV
                    , 'WideMass_M_MAX'              : 2100.0 * MeV
                    ## 2-muonInput for Hmumu
                    , 'TrkChi2_2MuonForHmumu'       : 5.0      # unitless
                    ## GEC
                    , 'GEC_Filter_NTRACK'        : False       # do or do not
                    , 'GEC_NTRACK_MAX'           : 120000        # max number of tracks
                    # prescales
                  , 'Prescale'                  : {
                         'Hlt2CharmSemilepD2HMuMuWideMass'    : 0.1
                        }
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmSemilep2MuonForD2HMuMuDecision' : 60003
                        , 'Hlt2CharmSemilepD2HMuMuDecision'         : 60004
                        , 'Hlt2CharmSemilepD2HMuMuWideMassDecision' : 60005
                        }
                }


    def __seqGEC(self) : # {
        """
        # Defines a Global Event Cut (mild badness) on all events with more
        # than a configurable upper limit of tracks.
        #
        # This is a temporary fix only and will be removed once proper
        # GEC lines are implemented
        """
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        from HltLine.HltLine import bindMembers

        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('GEC_Filter_NTRACK') : # {
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()
        # }

        Hlt2CharmKillTooManyInTrkAlg = VoidFilter('Hlt2CharmSemilepD2HMuMuKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __InPartFilterLowIP(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_2MuonForHmumu)s )" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_Hmumu)s )" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq

    def __InPartFilterHmumu(self, name, inputContainers) :  
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_Hmumu)s)" \
                 "& (PT> %(TrkPt_Hmumu)s)" \
                 "& (P> %(TrkP_Hmumu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_Hmumu)s)" % self.getProps()


        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq

    def __HmumuCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        massmin = min(self.getProp('Sig_M_MIN'), self.getProp('WideMass_M_MIN'))
        massmax = max(self.getProp('Sig_M_MAX'), self.getProp('WideMass_M_MAX'))

        masscut = "in_range(%s,  M, %s)" % (massmin, massmax)


        combcuts = "(AM<2100*MeV)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_Hmumu)s      * MeV) "  \
                   "& ((APT1+APT2+APT3) > %(DSumPt_Hmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_Hmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Hmumu)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = masscut + \
                     "& INGENERATION(BPVIPCHI2() > %(TrkPVIPChi2MAX_Hmumu)s, 1 ) "\
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_Hmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_Hmumu)s)" \
                     "& (BPVDIRA                 > %(DDira)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_Hmumu)s )" \
                     "& (BPVIPCHI2() < %(DIPChi2_Hmumu)s )" % self.getProps()
        combineCharmHmumu = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharmHmumu])

    def __HmumuFilter(self, name, inputSeq, extracode = None) :

        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        if extracode :
            filter = Hlt2Member( FilterDesktop
                                 , 'Filter'
                                 , Inputs = inputSeq
                                 , Code = extracode
                                 )
        else :
            filter = Hlt2Member( FilterDesktop
                                 , 'Filter'
                                 , Inputs = inputSeq
                                 , Code = 'ALL'
                                 )

        return bindMembers(name, inputSeq + [filter])
 
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions,BiKalmanFittedSecondLoopPions, BiKalmanFittedKaons, BiKalmanFittedSecondLoopKaons, BiKalmanFittedMuons

        from HltTracking.HltPVs import PV3D
        
        # Filter pions and kaons with LowIP Cut  
        KaonsLowIP = self.__InPartFilterLowIP('CharmHmumuInputKaonsLowIP', [ BiKalmanFittedKaons] )
        PionsLowIP = self.__InPartFilterLowIP('CharmHmumuInputPionsLowIP', [ BiKalmanFittedPions] )
         
        from   Hlt2CharmSemilepTwoMuonForMuMuHad import  Hlt2CharmSemilepTwoMuonForMuMuHadConf

        Hlt2Charm2MuonForHmumu =  Hlt2CharmSemilepTwoMuonForMuMuHadConf().twoMuonSequence()

        #Second Stage - picks up two low pt tracks too

        # Filter low PT pions and kaons 

        pionsForHmumu = self.__InPartFilterHmumu('CharmInputPionsHmumu', [ BiKalmanFittedSecondLoopPions] )
        kaonsForHmumu = self.__InPartFilterHmumu('CharmInputKaonsHmumu', [ BiKalmanFittedSecondLoopKaons] )

 
        # Make Hmumu 
        CharmHmumuCombine = self.__HmumuCombine ( name = 'CharmSemilepD2HMuMu'
                                                  , inputSeq = [Hlt2Charm2MuonForHmumu , pionsForHmumu, kaonsForHmumu, KaonsLowIP, PionsLowIP]
                                                  , decayDesc = [ "D+ -> J/psi(1S) pi+", "D- -> J/psi(1S) pi-",
                                                                  "D+ -> J/psi(1S) K+",  "D- -> J/psi(1S) K-",
                                                                  "D+ -> phi(1020) pi-", "D- -> rho(770)0 pi+",
                                                                  "D+ -> phi(1020) K-",  "D- -> rho(770)0 K+"]                                                    
                                                 )   

        sigMassCut  = "in_range(%s, M, %s)" \
                      % (self.getProp('Sig_M_MIN'), self.getProp('Sig_M_MAX'))

        wideMassCut = "in_range(%s, M, %s)" \
                      % (self.getProp('WideMass_M_MIN'), \
                         self.getProp('WideMass_M_MAX'))

        # Hmumu line
        Hlt2CharmHmumu = self.__HmumuFilter ( name = 'CharmSemilepD2HMuMu', inputSeq = [CharmHmumuCombine], extracode = sigMassCut )
        # Hmumu WideMass line - with prescale
        Hlt2CharmHmumuWideMass = self.__HmumuFilter (name = 'CharmSemilepD2HMuMuWideMass', inputSeq = [CharmHmumuCombine], extracode = wideMassCut )

        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        ##########################################################################
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()

        line = Hlt2Line('CharmSemilep2MuonForD2HMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep2MuonForD2HMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
 
        line = Hlt2Line('CharmSemilepD2HMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu, pionsForHmumu, kaonsForHmumu, KaonsLowIP, PionsLowIP, Hlt2CharmHmumu]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilepD2HMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmSemilepD2HMuMuWideMass', prescale = self.prescale
                        , algos =  [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu, pionsForHmumu, kaonsForHmumu, KaonsLowIP, PionsLowIP, Hlt2CharmHmumuWideMass]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilepD2HMuMuWideMassDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

