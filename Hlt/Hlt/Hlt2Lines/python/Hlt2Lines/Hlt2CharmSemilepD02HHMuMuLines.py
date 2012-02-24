__author__  = [ 'Benoit F. Viaud','Olga Kochebina' ]
__date__    = '$Date: 16-01-2012$'
__version__ = '$Revision: 1$'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2CharmSemilepD02HHMuMuLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## HHmumu
                    'TrkPt_HHmumu'                 : 300.0 * MeV
                  , 'TrkP_HHmumu'                  : 3000.0 * MeV
                  , 'TrkPVIPChi2_HHmumu'           : 0.0      # unitless
                  , 'TrkPVIPChi2MAX_HHmumu'           : 12.0      # !!unitless
                  ,  'TrkPtMAX_HHmumu'                 : 0.0 * MeV
                  , 'TrkChi2_HHmumu'               : 4.0      # unitless
                  , 'PairMinDoca_HHmumu'           : 0.1 * mm
                  , 'PairMaxDoca_HHmumu'           : 0.2 * mm
                  , 'VtxPVDispChi2_HHmumu'         : 36.0    #  unitless
                  , 'VtxChi2_HHmumu'               : 15.0     # unitless
                  , 'DIPChi2_HHmumu'               : 25.0     #  unitless
                  , 'DSumPt_HHmumu'                : 3000.0 * MeV
                  , 'DDira'              : 0.9999     # adimensional                    
                  , 'MCOR_MAX_HHmumu'              : 3500.0 * MeV
                  , 'Sig_M_MIN'                   : 1800.0 * MeV
                  , 'Sig_M_MAX'                   : 1950.0 * MeV
                  , 'WideMass_M_MIN'              : 1700.0 * MeV
                  , 'WideMass_M_MAX'              : 2100.0 * MeV
                  ## GEC
                  , 'GEC_Filter_NTRACK'        : False       # do or do not
                  , 'GEC_NTRACK_MAX'           : 120000        # max number of tracks
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmSemilep2MuonForD02HHMuMuDecision' : 61026
                        ## For new lines
                        , 'Hlt2CharmSemilepD02PiPiMuMuDecision'         : 61030
                        , 'Hlt2CharmSemilepD02KKMuMuDecision'         : 61031
                        , 'Hlt2CharmSemilepD02KPiMuMuDecision'         : 61032
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

        Hlt2CharmKillTooManyInTrkAlg = VoidFilter('Hlt2CharmSemilepD02HHMuMuKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __InPartFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_HHmumu)s )" \
                 "& (PT> %(TrkPt_HHmumu)s)" \
                 "& (P> %(TrkP_HHmumu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_HHmumu)s )" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
  

    def __HHMuMuCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        massmin = min(self.getProp('Sig_M_MIN'), self.getProp('WideMass_M_MIN'))
        massmax = max(self.getProp('Sig_M_MAX'), self.getProp('WideMass_M_MAX'))

        masscut = "in_range(%s,  M, %s)" % (massmin, massmax)


        combcuts = "(AM<2100*MeV)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_HHmumu)s      * MeV) "  \
                   "& ((APT1+APT2+APT3+APT4) > %(DSumPt_HHmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_HHmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_HHmumu)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_HHmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_HHmumu)s)" \
                     "& (BPVDIRA                 > %(DDira)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_HHmumu)s )" \
                     "& ( SUMTREE( ( (ID=='K+') | (ID=='K-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_HHmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_HHmumu)s )" % self.getProps() 
        combineCharmHHmumu = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharmHHmumu])

    def __HHMuMuFilter(self, name, inputSeq, extracode = None) :

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
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons

        from HltTracking.HltPVs import PV3D
        
        # Filter pions and kaons with LowIP Cut
        Kaons = self.__InPartFilter('CharmSemilepHHmumuInputKaons', [ BiKalmanFittedKaons] )
        Pions = self.__InPartFilter('CharmSemilepHHmumuInputPions', [ BiKalmanFittedPions] )
      
        from   Hlt2CharmSemilepTwoMuonForMuMuHad import  Hlt2CharmSemilepTwoMuonForMuMuHadConf

        Hlt2Charm2MuonForHHmumu =  Hlt2CharmSemilepTwoMuonForMuMuHadConf().twoMuonSequence()
 
        # Make HHmumu 

        CharmPiPiMuMuCombine = self.__HHMuMuCombine ( name = 'CharmSemilepD02PiPiMuMu'
                                                  , inputSeq = [Hlt2Charm2MuonForHHmumu , Pions]
                                                  , decayDesc = [ "D0 -> J/psi(1S) pi+ pi-"]
                                                 )
        CharmKKMuMuCombine = self.__HHMuMuCombine ( name = 'CharmSemilepD02KKMuMu'
                                                  , inputSeq = [Hlt2Charm2MuonForHHmumu , Kaons] 
                                                  , decayDesc = [ "D0 -> J/psi(1S)  K+ K-"]
                                                 )
        CharmKPiMuMuCombine = self.__HHMuMuCombine ( name = 'CharmSemilepD02KPiMuMu'
                                                  , inputSeq = [Hlt2Charm2MuonForHHmumu , Kaons, Pions] 
                                                  , decayDesc = [ "D0 -> J/psi(1S)  K+ pi-","D0 -> J/psi(1S)  K- pi+" ]
                                                 )
        

        sigMassCut  = "in_range(%s, M, %s)" \
                      % (self.getProp('Sig_M_MIN'), self.getProp('Sig_M_MAX'))


        Hlt2CharmPiPiMuMu = self.__HHMuMuFilter ( name = 'CharmSemilepD02PiPiMuMu', inputSeq = [CharmPiPiMuMuCombine], extracode = sigMassCut )
        Hlt2CharmKKMuMu = self.__HHMuMuFilter ( name = 'CharmSemilepD02KKMuMu', inputSeq = [CharmKKMuMuCombine], extracode = sigMassCut )
        Hlt2CharmKPiMuMu = self.__HHMuMuFilter ( name = 'CharmSemilepD02KPiMuMu', inputSeq = [CharmKPiMuMuCombine], extracode = sigMassCut )

        ###########################################################################
        # Define the Hlt2 Lines
        # 
        ##########################################################################
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()

        line = Hlt2Line('CharmSemilep2MuonForD02HHMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHHmumu]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep2MuonForD02HHMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
       
        ## PiPiMuMu
        line = Hlt2Line('CharmSemilepD02PiPiMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHHmumu, Pions, Hlt2CharmPiPiMuMu] 
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilepD02PiPiMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
        ## KKMuMu
        line = Hlt2Line('CharmSemilepD02KKMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHHmumu, Kaons, Hlt2CharmKKMuMu] 
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilepD02KKMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ## KPiMuMu
        line = Hlt2Line('CharmSemilepD02KPiMuMu', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2MuonForHHmumu, Kaons, Pions, Hlt2CharmKPiMuMu] 
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilepD02KPiMuMuDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
    
