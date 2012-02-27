__author__  = [ 'Benoit F. Viaud','Olga Kochebina' ]
__date__    = '$Date: 18/01/2012$'
__version__ = '$Revision: 1.$'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2CharmSemilepD2HMuMuLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## Hmumu
                    'TrkPt_Hmumu'                 : 300.0 * MeV
                    , 'TrkP_Hmumu'                  : 3000.0 * MeV ## 1
                    , 'TrkPVIPChi2_Hmumu'           : 0.0      # unitless
                    , 'TrkPVIPChi2MAX_Hmumu'       : 17.0      #0 unitless
                    , 'TrkPtMAX_Hmumu'             : 0.0 * MeV
                    , 'TrkChi2_Hmumu'               : 4.0      # unitless
                    , 'PairMinDoca_Hmumu'           : 0.1 * mm
                    , 'PairMaxDoca_Hmumu'           : 0.25* mm
                    , 'VtxPVDispChi2_Hmumu'         : 20.0    #20 unitless
                    , 'VtxChi2_Hmumu'               : 20.0     # unitless
                    , 'DIPChi2_Hmumu'               : 36.0     #4  unitless
                   , 'DSumPt_Hmumu'                : 500.0 * MeV ## 1
                    , 'DDira'              : 0.9999     # adimensional
                    , 'MCOR_MAX_Hmumu'              : 3500.0 * MeV
                    , 'Sig_M_MIN'                   : 1800.0 * MeV
                    , 'Sig_M_MAX'                   : 2050.0 * MeV
                    , 'WideMass_M_MIN'              : 1700.0 * MeV
                    , 'WideMass_M_MAX'              : 2100.0 * MeV
                    , 'Lambda_c_Sig_M_MIN'                   : 2200.0 * MeV
                    , 'Lambda_c_Sig_M_MAX'                   : 2370.0 * MeV
                    , 'Lambda_c_WideMass_M_MIN'              : 1700.0 * MeV
                    , 'Lambda_c_WideMass_M_MAX'              : 2300.0 * MeV
                    ## GEC
                    , 'GEC_Filter_NTRACK'        : False       # do or do not
                    , 'GEC_NTRACK_MAX'           : 180        # max number of tracks
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmSemilep2MuonForD2HMuMuDecision' : 61023
                        ## For new lines
                        , 'Hlt2CharmSemilep3bodyD2PiMuMuDecision'         : 61038
                        , 'Hlt2CharmSemilep3bodyD2KMuMuDecision'         : 61039
                        , 'Hlt2CharmSemilep3bodyD2PiMuMuSSDecision'         : 61040
                        , 'Hlt2CharmSemilep3bodyD2KMuMuSSDecision'         : 61043
                        , 'Hlt2CharmSemilep3bodyLambdac2PMuMuDecision'       : 60066
                        , 'Hlt2CharmSemilep3bodyLambdac2PMuMuSSDecision'       : 60067
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


    def __InPartFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_Hmumu)s )" \
                 "& (PT> %(TrkPt_Hmumu)s)" \
                 "& (P> %(TrkP_Hmumu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_Hmumu)s )" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq

    def __HMuMuCombine(self, name, inputSeq, decayDesc) :
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
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_Hmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_Hmumu)s)" \
                     "& (BPVDIRA                 > %(DDira)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_Hmumu)s )" \
                     "& ( SUMTREE( ( (ID=='K+') | (ID=='K-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_Hmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_Hmumu)s )" % self.getProps()
        combineCharmHmumu = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharmHmumu])

    def __HMuMuFilter(self, name, inputSeq, extracode = None) :

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

    def __PMuMuCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D


        Lambda_c_massmin = min(self.getProp('Lambda_c_Sig_M_MIN'), self.getProp('Lambda_c_WideMass_M_MIN'))
        Lambda_c_massmax = max(self.getProp('Lambda_c_Sig_M_MAX'), self.getProp('Lambda_c_WideMass_M_MAX'))

        Lambda_c_masscut = "in_range(%s,  M, %s)" % (Lambda_c_massmin,Lambda_c_massmax)

        combcuts = "(AM<2100*MeV)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_Hmumu)s      * MeV) "  \
                   "& ((APT1+APT2+APT3) > %(DSumPt_Hmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_Hmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Hmumu)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = Lambda_c_masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_Hmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_Hmumu)s)" \
                     "& (BPVDIRA                 > %(DDira)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_Hmumu)s )" \
                     "& ( SUMTREE( ( (ID=='p+') | (ID=='p~-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_Hmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_Hmumu)s )" % self.getProps()
        combineCharmPMuMu = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharmPMuMu])

    def __PMuMuFilter(self, name, inputSeq, extracode = None) :

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
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons, BiKalmanFittedProtons

        from HltTracking.HltPVs import PV3D
        
        # Filter pions and kaons with LowIP Cut  
        Kaons = self.__InPartFilter('CharmSemilepHmumuInputKaons', [ BiKalmanFittedKaons] )
        Pions = self.__InPartFilter('CharmSemilepHmumuInputPions', [ BiKalmanFittedPions] )
        Protons = self.__InPartFilter('CharmSemilepPMuMuInputProtons', [ BiKalmanFittedProtons] )
        

        from   Hlt2CharmSemilepTwoMuonForMuMuHad import  Hlt2CharmSemilepTwoMuonForMuMuHadConf

        Hlt2Charm2MuonForHmumu =  Hlt2CharmSemilepTwoMuonForMuMuHadConf().twoMuonSequence()


        ################################################################################################################################
        CharmPiMuMuCombine = self.__HMuMuCombine ( name = 'CharmSemilep3bodyD2PiMuMu'
                                                  , inputSeq = [Hlt2Charm2MuonForHmumu , Pions] 
                                                  , decayDesc = [ "D+ -> J/psi(1S) pi+", "D- -> J/psi(1S) pi-"]                                                    
                                                  )

        CharmPiMuMuSSCombine = self.__HMuMuCombine ( name = 'CharmSemilep3bodyD2PiMuMuSS'
                                                  , inputSeq = [Hlt2Charm2MuonForHmumu , Pions] 
                                                  , decayDesc = ["D+ -> phi(1020) pi-", "D- -> rho(770)0 pi+"]                                                    
                                                  )

        CharmKMuMuCombine = self.__HMuMuCombine ( name = 'CharmSemilep3bodyD2KMuMu'
                                                  , inputSeq = [Hlt2Charm2MuonForHmumu , Kaons] 
                                                  , decayDesc = [ "D+ -> J/psi(1S) K+", "D- -> J/psi(1S) K-"]                                                    
                                                  )

        CharmKMuMuSSCombine = self.__HMuMuCombine ( name = 'CharmSemilep3bodyD2KMuMuSS'
                                                  , inputSeq = [Hlt2Charm2MuonForHmumu , Kaons] 
                                                  , decayDesc = [ "D+ -> phi(1020) K-", "D- -> rho(770)0 K+"]                                                    
                                                  )

        CharmPMuMuCombine = self.__PMuMuCombine ( name = 'CharmSemilep3bodyLambdac2PMuMu'
                                                  , inputSeq = [Hlt2Charm2MuonForHmumu , Protons ] 
                                                  , decayDesc = ["Lambda_c+ -> J/psi(1S) p+" , "Lambda_c~- -> J/psi(1S) p~- "
                                                                 ]                                                 
                                                  )

        CharmPMuMuSSCombine = self.__PMuMuCombine ( name = 'CharmSemilep3bodyLambdac2PMuMuSS'
                                                    , inputSeq = [Hlt2Charm2MuonForHmumu , Protons ] 
                                                    , decayDesc = ["Lambda_c+ -> phi(1020) p~-" , "Lambda_c~- ->  rho(770)0 p+ "
                                                                 ]                                                 
                                                    )
        
        ###################################################################################


        sigMassCut  = "in_range(%s, M, %s)" \
                      % (self.getProp('Sig_M_MIN'), self.getProp('Sig_M_MAX'))
   
        Hlt2CharmPiMuMu = self.__HMuMuFilter ( name = 'CharmSemilep3bodyD2PiMuMu', inputSeq = [CharmPiMuMuCombine], extracode = sigMassCut )
        Hlt2CharmPiMuMuSS = self.__HMuMuFilter ( name = 'CharmSemilep3bodyD2PiMuMuSS', inputSeq = [CharmPiMuMuSSCombine], extracode = sigMassCut )
        Hlt2CharmKMuMu = self.__HMuMuFilter ( name = 'CharmSemilep3bodyD2KMuMu', inputSeq = [CharmKMuMuCombine], extracode = sigMassCut )
        Hlt2CharmKMuMuSS = self.__HMuMuFilter ( name = 'CharmSemilep3bodyD2KMuMuSS', inputSeq = [CharmKMuMuSSCombine], extracode = sigMassCut )


        Lambda_c_sigMassCut  = "in_range(%s, M, %s)" \
                      % (self.getProp('Lambda_c_Sig_M_MIN'), self.getProp('Lambda_c_Sig_M_MAX'))
        
        
        Hlt2CharmPMuMu = self.__PMuMuFilter ( name = 'CharmSemilep3bodyLambdac2PMuMu', inputSeq = [CharmPMuMuCombine], extracode = Lambda_c_sigMassCut )
        Hlt2CharmPMuMuSS = self.__PMuMuFilter ( name = 'CharmSemilep3bodyLambdac2PMuMuSS', inputSeq = [CharmPMuMuSSCombine], extracode = Lambda_c_sigMassCut )
      
        ###########################################################################
        # Define the Hlt2 Lines
        # 
        ##########################################################################
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()

        line = Hlt2Line('CharmSemilep2MuonForD2HMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep2MuonForD2HMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )



    
        #############################New lines############################################

        ## PiMuMu ##
        line = Hlt2Line('CharmSemilep3bodyD2PiMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu, Pions, Hlt2CharmPiMuMu]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep3bodyD2PiMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ## PiMuMuSS #
        line = Hlt2Line('CharmSemilep3bodyD2PiMuMuSS', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu, Pions, Hlt2CharmPiMuMuSS]  
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep3bodyD2PiMuMuSSDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ## KMuMu ##
        line = Hlt2Line('CharmSemilep3bodyD2KMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu, Kaons, Hlt2CharmKMuMu] 
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep3bodyD2KMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ## KMuMuSS ##
        line = Hlt2Line('CharmSemilep3bodyD2KMuMuSS', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu, Kaons, Hlt2CharmKMuMuSS] 
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep3bodyD2KMuMuSSDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ## Lambda_c ##
            
        line = Hlt2Line('CharmSemilep3bodyLambdac2PMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu, Protons, Hlt2CharmPMuMu]  
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep3bodyLambdac2PMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ## Lambda_c SS ##
            
        line = Hlt2Line('CharmSemilep3bodyLambdac2PMuMuSS', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHmumu, Protons, Hlt2CharmPMuMuSS]  
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep3bodyLambdac2PMuMuSSDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

