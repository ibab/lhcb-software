from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2CharmHadD2HHHLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## 3Body
                    'TrkPt_3Body'                 : 250.0    # in MeV
                  , 'TrkP_3Body'                  : 2000.0   # in MeV
                  , 'TrkPVIPChi2_3Body'           : 5.0      # unitless
                  , 'TrkChi2_3Body'               : 3.0      # unitless
                  , 'PairMinDoca_3Body'           : 0.08     # in mm
                  , 'VtxPVDispChi2_3Body'         : 150.0    # unitless
                  , 'VtxChi2_3Body'               : 20.0     # unitless
                  , 'DIPChi2_3Body'               : 15.0     # unitless
                  , 'DSumPt_3Body'                : 2500.0   # sum pT   
                  , 'MCOR_MAX_3Body'              : 3500.    # MeV
                  , 'TrkChi2_2BodyFor3Body'       : 3.0      # unitless
                  , 'GEC_Filter_NTRACK'           : True     # do or do not
                  , 'GEC_NTRACK_MAX'              : 110      # max number of tracks
                  # prescales
                  , 'Prescale'                  : {
                        'Hlt2CharmHadD2HHHWideMass'    : 0.1
                        }
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmHad2BodyForD2HHHDecision' : 50910
                        , 'Hlt2CharmHadD2HHHDecision'         : 50911
                        , 'Hlt2CharmHadD2HHHWideMassDecision' : 50912
                        }
                }

    def __seqGEC(self) : 
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
        if self.getProp('GEC_Filter_NTRACK') : 
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()

        Hlt2CharmKillTooManyInTrkAlg = VoidFilter('Hlt2CharmHadD2HHHKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk


    def __InPartFilterLowIP(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """ (TRCHI2DOF< %(TrkChi2_2BodyFor3Body)s )
                    & (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_3Body)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq


    def __InPartFilter3Body(self, name, inputContainers) :  
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_3Body)s )" \
                 "& (PT> %(TrkPt_3Body)s *MeV)" \
                 "& (P> %(TrkP_3Body)s *MeV)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_3Body)s )" % self.getProps()


        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq

    def __3BodyCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "(AM<2100*MeV)" \
                   "& ((APT1+APT2+APT3) > %(DSumPt_3Body)s) " \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_3Body)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = "(VFASPF(VCHI2PDOF) < %(VtxChi2_3Body)s)" \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_3Body)s )" \
                     "& (BPVCORRM < %(MCOR_MAX_3Body)s*MeV)" \
                     "& (BPVIPCHI2() < %(DIPChi2_3Body)s)" % self.getProps()
        combineCharm3Body = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharm3Body])

    def __3BodyFilter(self, name, inputSeq, extracode = None) :

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
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions,BiKalmanFittedSecondLoopPions, BiKalmanFittedKaons, BiKalmanFittedSecondLoopKaons
        from HltTracking.HltPVs import PV3D
        
        # Filter pions and kaons with LowIP Cut
        pionsLowIP = self.__InPartFilterLowIP('Charm3BodyInputPionsLowIP', [ BiKalmanFittedPions] )
        kaonsLowIP = self.__InPartFilterLowIP('Charm3BodyInputKaonsLowIP', [ BiKalmanFittedKaons] )


        from Hlt2CharmHadTwoBodyForMultiBody import Hlt2CharmHadTwoBodyForMultiBodyConf
 
        Hlt2Charm2BodyFor3Body =  Hlt2CharmHadTwoBodyForMultiBodyConf().twoBodySequence()
                          
        #Second Stage - picks up a low pt track too

        # Filter low PT pions and kaons 

        pionsFor3Body = self.__InPartFilter3Body('CharmInputPions3Body', [ BiKalmanFittedSecondLoopPions] )
        kaonsFor3Body = self.__InPartFilter3Body('CharmInputKaons3Body', [ BiKalmanFittedSecondLoopKaons] )
 
        # Make 3Body 
        Charm3BodyCombine = self.__3BodyCombine (  name = 'CharmHadD2HHH'
                                                  , inputSeq = [ Hlt2Charm2BodyFor3Body, pionsFor3Body, kaonsFor3Body, kaonsLowIP, pionsLowIP ]
                                                  , decayDesc = ["D+ -> K*(892)0 pi+", "D+ -> K*(892)0 pi-"
                                                                ,"D+ -> K*(892)0 K+",  "D+ -> K*(892)0 K-"
                                                                ,"D+ -> K*(892)- pi+", "D+ -> K*(892)+ pi-"
                                                                ,"D+ -> K*(892)- K+",  "D+ -> K*(892)+ K-" ]
                                                 )   
        # 3Body line
        Hlt2Charm3Body = self.__3BodyFilter ( name = 'CharmHadD2HHH', inputSeq = [Charm3BodyCombine], extracode = "in_range(1800*MeV, M, 2040*MeV)")
        # 3Body WideMass line - with prescale
        Hlt2Charm3BodyWideMass = self.__3BodyFilter (name = 'CharmHadD2HHHWideMass', inputSeq = [Charm3BodyCombine], extracode = "in_range(1700*MeV, M, 2100*MeV)")

        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        ##########################################################################
        line = Hlt2Line('CharmHad2BodyForD2HHH', prescale = self.prescale
                        , algos = [ self.__seqGEC(), PV3D(), Hlt2Charm2BodyFor3Body]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmHad2BodyForD2HHHDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmHadD2HHH', prescale = self.prescale
                        , algos = [self.__seqGEC(), PV3D(), Hlt2Charm2BodyFor3Body, pionsFor3Body, kaonsFor3Body, kaonsLowIP, pionsLowIP, Hlt2Charm3Body]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmHadD2HHHDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmHadD2HHHWideMass', prescale = self.prescale
                        , algos = [self.__seqGEC(), PV3D(), Hlt2Charm2BodyFor3Body, pionsFor3Body, kaonsFor3Body, kaonsLowIP, pionsLowIP, Hlt2Charm3BodyWideMass]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmHadD2HHHWideMassDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
