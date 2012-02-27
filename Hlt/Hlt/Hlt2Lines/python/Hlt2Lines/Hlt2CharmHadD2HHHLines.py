from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2CharmHadD2HHHLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## 3Body
                    'TrkPt_3Body'                 : 1000.0    # in MeV
                  , 'TrkP_3Body'                  : 10000.0   # in MeV
                  , 'TrkPVIPChi2_3Body'           : 5.0      # unitless
			, 'Daughter2IPChi2'             : 10.0     # unitless Added by Feng ZHANG
                  , 'TrkChi2_3Body'               : 3.0      # unitless
                  , 'PairMinDoca_3Body'           : 0.08     # in mm
                  , 'VtxPVDispChi2_3Body'         : 150.0    # unitless
                  , 'VtxChi2_3Body'               : 20.0     # unitless
                  , 'DIPChi2_3Body'               : 15.0     # unitless
                  , 'DSumPt_3Body'                : 2500.0   # sum pT   
			, 'DOCAChi2'                    : 50.0     # Maximum Chi2 Between two daughters BY Feng ZHANG
                  #, 'MCOR_MAX_3Body'              : 3500.    # MeV
                  , 'TrkChi2_2BodyFor3Body'       : 3.0      # unitless
                  , 'GEC_Filter_NTRACK'           : True     # do or do not
                  , 'GEC_NTRACK_MAX'              : 180      # max number of tracks
                  , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                  , 'name_prefix'              : 'CharmHadD2HHH'
                  # prescales
                  , 'Prescale'                  : {
                        'Hlt2CharmHadD2HHHWideMass'    : 0.1
                        }
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmHadD2HHH2BodyDecision'    : 50910
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

        filtName = 'Hlt2' + self.getProp('name_prefix')  + 'KillTooManyInTrkAlg'
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter(filtName
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

        incuts = "(PT> %(TrkPt_3Body)s *MeV)" \
		     "& (TRCHI2DOF< %(TrkChi2_3Body)s )" \
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


    def __filterHlt1TOS(self, name, input) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import TisTosParticleTagger

        filterTOS = Hlt2Member( TisTosParticleTagger
                                , 'Hlt1TOSFilter'
                                , Inputs = [ input.outputSelection() ]
                                , TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
                              )

        return bindMembers(name, [ input, filterTOS ])
    # }


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions,BiKalmanFittedSecondLoopPions, BiKalmanFittedKaons, BiKalmanFittedSecondLoopKaons
        from HltTracking.HltPVs import PV3D
        
        # Filter pions and kaons with LowIP Cut
        pionLowIPName = self.getProp('name_prefix') + 'PionsLowIP'
        kaonLowIPName = self.getProp('name_prefix') + 'KaonsLowIP'
        pionsLowIP = self.__InPartFilterLowIP(pionLowIPName, [ BiKalmanFittedPions] )
        kaonsLowIP = self.__InPartFilterLowIP(kaonLowIPName, [ BiKalmanFittedKaons] )


        #from Hlt2CharmHadTwoBodyForMultiBody import Hlt2CharmHadTwoBodyForMultiBodyConf
 
        #Hlt2Charm2BodyFor3Body =  Hlt2CharmHadTwoBodyForMultiBodyConf().twoBodySequence()
                          
        #Second Stage - picks up a low pt track too

        # Filter low PT pions and kaons 
        pionSLName = self.getProp('name_prefix') + 'Pions2ndLoop'
        kaonSLName = self.getProp('name_prefix') + 'Kaons2ndLoop'
        pionsFor3Body = self.__InPartFilter3Body(pionSLName, [ BiKalmanFittedPions] )
        kaonsFor3Body = self.__InPartFilter3Body(kaonSLName, [ BiKalmanFittedKaons] )
 
        # Make 3Body 
        Charm3BodyCombine = self.__3BodyCombine (  name = self.getProp('name_prefix')
                                                  , inputSeq = [  pionsFor3Body, kaonsFor3Body]
                                                  , decayDesc = ["[D+ -> pi+ pi+ K-]cc", "[D+ -> pi+ K+ K-]cc", "[D+ -> K+ K+ K-]cc", "[D+ -> K+ K+ pi-]cc" , "[D+ -> pi+ pi+ pi-]cc", "[D+ -> pi+ pi- K+]cc"]

                                                 )   

        tosName = self.getProp('name_prefix')
        Charm3BodyCombineToS =  self.__filterHlt1TOS( tosName, Charm3BodyCombine )

        # 3Body line
        modeName = self.getProp('name_prefix')
        wideMassName = modeName + 'WideMass'
        Hlt2Charm3Body = self.__3BodyFilter ( name = modeName, inputSeq = [Charm3BodyCombineToS], extracode = "in_range(1800*MeV, M, 2040*MeV)")
        # 3Body WideMass line - with prescale
        Hlt2Charm3BodyWideMass = self.__3BodyFilter (name = wideMassName, inputSeq = [Charm3BodyCombineToS], extracode = "in_range(1700*MeV, M, 2100*MeV)")

        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        ##########################################################################
        #twoBodyName = self.getProp('name_prefix') + '2Body'
        #line = Hlt2Line(twoBodyName, prescale = self.prescale
        #                , algos = [ PV3D(), self.__seqGEC(), Hlt2Charm2BodyFor3Body]
        #                , postscale = self.postscale
        #                )
        #decName = 'Hlt2' + twoBodyName + 'Decision'
        #annSvcID = self._scale(decName,'HltANNSvcID')
        #HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(modeName, prescale = self.prescale
                        , algos = [ PV3D(), self.__seqGEC(), pionsFor3Body, kaonsFor3Body, Hlt2Charm3Body]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassName , prescale = self.prescale
                        , algos = [ PV3D(), self.__seqGEC(), pionsFor3Body, kaonsFor3Body, Hlt2Charm3BodyWideMass]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
