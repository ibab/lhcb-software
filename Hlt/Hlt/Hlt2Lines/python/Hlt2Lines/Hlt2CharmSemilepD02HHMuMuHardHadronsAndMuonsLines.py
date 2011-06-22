__author__  = [ 'Benoit F. Viaud' ]
__date__    = '$Date: 22-06-2011$'
__version__ = '$Revision: 0$'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV, GeV, mm
        
class Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## HHmumu
                    'TrkPt_HHmumu'                 : 500.0 * MeV
                  , 'TrkP_HHmumu'                  : 5000.0 * MeV
                  , 'TrkPVIPChi2_HHmumu'           : 0.0      # unitless
                  , 'TrkPVIPChi2MAX_HHmumu'           : 10.0      # unitless
                  ,  'TrkPtMAX_HHmumu'                 : 0.0 * MeV
                  , 'TrkChi2_HHmumu'               : 5.0      # unitless
                  , 'PairMinDoca_HHmumu'           : 0.15 * mm
                  , 'PairMaxDoca_HHmumu'           : 0.25 * mm
                  , 'VtxPVDispChi2_HHmumu'         : 20.0    # unitless
                  , 'VtxChi2_HHmumu'               : 20.0     # unitless
                  , 'DIPChi2_HHmumu'               : 36.0     # unitless
                  , 'DSumPt_HHmumu'                : 0.0 * MeV
                  , 'DDira'              : 0.9996     # adimensional                    
                  , 'MCOR_MAX_HHmumu'              : 3500.0 * MeV
                  , 'Sig_M_MIN'                   : 1800.0 * MeV
                  , 'Sig_M_MAX'                   : 1950.0 * MeV
                  , 'WideMass_M_MIN'              : 1700.0 * MeV
                  , 'WideMass_M_MAX'              : 2100.0 * MeV
                  ## 2-muonInput for HHmumu
                  , 'TrkChi2_2MuonForHHmumu'       : 5.0      # unitless
                  ## GEC
                  , 'GEC_Filter_NTRACK'        : False       # do or do not
                  , 'GEC_NTRACK_MAX'           : 120000        # max number of tracks
                  # prescales
                  , 'Prescale'                  : {
                        'Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsWideMass'    : 0.1
                        }
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmSemilep2HadForD02HHMuMuHardHadronsAndMuonsDecision' : 60009
                        , 'Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsDecision'         : 60010
                        , 'Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsWideMassDecision' : 60011
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

        Hlt2CharmKillTooManyInTrkAlg = VoidFilter('Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __InPartFilter(self, name, inputContainers) :  
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_HHmumu)s)" \
                 "& (PT> %(TrkPt_HHmumu)s)" \
                 "& (P> %(TrkP_HHmumu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_HHmumu)s)" % self.getProps()


        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq

    def __HHmumuCombine(self, name, inputSeq, decayDesc) :
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

    def __HHmumuFilter(self, name, inputSeq, extracode = None) :

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
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons

        from HltTracking.HltPVs import PV3D
        

        from  Hlt2CharmSemilepTwoHadForMuMuHH import  Hlt2CharmSemilepTwoHadForMuMuHHConf

        Hlt2Charm2HadForHHmumu =   Hlt2CharmSemilepTwoHadForMuMuHHConf().twoHadforHHMuMuSequence()

        #Second Stage - picks up two low pt tracks too

        # Filter  second loop 

        muonsForHHmumu = self.__InPartFilter('CharmInputMuonsForHHMuMuHardHadronsAndMuons',[BiKalmanFittedMuons] )
  
        # Make HHmumu 
        CharmHHmumuCombine = self.__HHmumuCombine ( name = 'CharmSemilepD02HHMuMuHardHadronsAndMuons'
                                                  , inputSeq = [Hlt2Charm2HadForHHmumu, muonsForHHmumu]
                                                  , decayDesc = [ "D0 -> K*(892)0  mu+ mu-"]
                                                 )   

        sigMassCut  = "in_range(%s, M, %s)" \
                      % (self.getProp('Sig_M_MIN'), self.getProp('Sig_M_MAX'))

        wideMassCut = "in_range(%s, M, %s)" \
                      % (self.getProp('WideMass_M_MIN'), \
                         self.getProp('WideMass_M_MAX'))

        # HHmumu line
        Hlt2CharmHHmumuHardHadronsAndMuons = self.__HHmumuFilter ( name = 'CharmSemilepD02HHMuMuHardHadronsAndMuons', inputSeq = [CharmHHmumuCombine], extracode = sigMassCut )
        # HHmumu WideMass line - with prescale
        Hlt2CharmHHmumuHardHadronsAndMuonsWideMass = self.__HHmumuFilter (name = 'CharmSemilepD02HHMuMuHardHadronsAndMuonsWideMass', inputSeq = [CharmHHmumuCombine], extracode = wideMassCut )

        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        ##########################################################################
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()
                        
        line = Hlt2Line('CharmSemilep2HadForD02HHMuMuHardHadronsAndMuons', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2HadForHHmumu]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilep2HadForD02HHMuMuHardHadronsAndMuonsDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
 
        line = Hlt2Line('CharmSemilepD02HHMuMuHardHadronsAndMuons', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2HadForHHmumu, muonsForHHmumu, Hlt2CharmHHmumuHardHadronsAndMuons]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmSemilepD02HHMuMuHardHadronsAndMuonsWideMass', prescale = self.prescale
                        , algos =  [ PV3D(), Hlt2CharmKillTooManyInTrk, Hlt2Charm2HadForHHmumu, muonsForHHmumu, Hlt2CharmHHmumuHardHadronsAndMuonsWideMass]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsWideMassDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

