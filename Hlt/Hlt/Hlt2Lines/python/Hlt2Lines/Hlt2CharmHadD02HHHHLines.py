__author__  = [ 'Tom Hampson' ]
__date__    = '$Date: $'
__version__ = '$Revision: $'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2CharmHadD02HHHHLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## 4Body
                    'TrkPt_4Body'                 : 250.0 * MeV
                  , 'TrkP_4Body'                  : 2000.0 * MeV
                  , 'TrkPVIPChi2_4Body'           : 2.0      # unitless
                  , 'TrkChi2_4Body'               : 3.0      # unitless
                  , 'PairMinDoca_4Body'           : 0.10 * mm
                  , 'VtxPVDispChi2_4Body'         : 100.0    # unitless
                  , 'VtxChi2_4Body'               : 20.0     # unitless
                  , 'DIPChi2_4Body'               : 25.0     # unitless
                  , 'DSumPt_4Body'                : 2000.0 * MeV
                  , 'MCOR_MAX_4Body'              : 3500.0 * MeV
                  , 'Sig_M_MIN'                   : 1800.0 * MeV
                  , 'Sig_M_MAX'                   : 2040.0 * MeV
                  , 'WideMass_M_MIN'              : 1700.0 * MeV
                  , 'WideMass_M_MAX'              : 2100.0 * MeV
                  ## 2-body Input for 4Body
                  , 'TrkChi2_2BodyFor4Body'       : 3.0      # unitless
                  ## GEC
                  , 'GEC_Filter_NTRACK'        : False       # do or do not
                  , 'GEC_NTRACK_MAX'           : 120        # max number of tracks
                  # prescales
                  , 'Prescale'                  : {
                        'Hlt2CharmHadD02HHHHWideMass'    : 0.1
                        }
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmHad2BodyForD02HHHHDecision' : 50990
                        , 'Hlt2CharmHadD02HHHHDecision'         : 50991
                        , 'Hlt2CharmHadD02HHHHWideMassDecision' : 50992
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

        Hlt2CharmKillTooManyInTrkAlg = VoidFilter('Hlt2CharmHadD02HHHHKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __InPartFilterLowIP(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_2BodyFor4Body)s )" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_4Body)s )" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq

    def __InPartFilter4Body(self, name, inputContainers) :  
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_4Body)s)" \
                 "& (PT> %(TrkPt_4Body)s)" \
                 "& (P> %(TrkP_4Body)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_4Body)s)" % self.getProps()


        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq

    def __4BodyCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        massmin = min(self.getProp('Sig_M_MIN'), self.getProp('WideMass_M_MIN'))
        massmax = max(self.getProp('Sig_M_MAX'), self.getProp('WideMass_M_MAX'))

        masscut = "in_range(%s,  M, %s)" % (massmin, massmax)


        combcuts = "(AM<2100*MeV)" \
                   "& ((APT1+APT2+APT3+APT4) > %(DSumPt_4Body)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_4Body)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_4Body)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_4Body)s)" \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_4Body)s )" \
                     "& (BPVIPCHI2() < %(DIPChi2_4Body)s )" % self.getProps()
        combineCharm4Body = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharm4Body])

    def __4BodyFilter(self, name, inputSeq, extracode = None) :

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
        pionsLowIP = self.__InPartFilterLowIP('Charm4BodyInputPionsLowIP', [ BiKalmanFittedPions] )
        kaonsLowIP = self.__InPartFilterLowIP('Charm4BodyInputKaonsLowIP', [ BiKalmanFittedKaons] )

         
        from Hlt2CharmHadTwoBodyForMultiBody import Hlt2CharmHadTwoBodyForMultiBodyConf

        Hlt2Charm2BodyFor4Body =  Hlt2CharmHadTwoBodyForMultiBodyConf().twoBodySequence()

        #Second Stage - picks up two low pt tracks too

        # Filter low PT pions and kaons 

        pionsFor4Body = self.__InPartFilter4Body('CharmInputPions4Body', [ BiKalmanFittedSecondLoopPions] )
        kaonsFor4Body = self.__InPartFilter4Body('CharmInputKaons4Body', [ BiKalmanFittedSecondLoopKaons] )
 
        # Make 4Body 
        Charm4BodyCombine = self.__4BodyCombine ( name = 'CharmHadD02HHHH'
                                                  , inputSeq = [ Hlt2Charm2BodyFor4Body, pionsFor4Body, kaonsFor4Body, kaonsLowIP, pionsLowIP ]
                                                  , decayDesc = [ "D0 -> K*(892)- pi+ pi+", "D0 -> K*(892)+ pi- pi-"
                                                                  ,"D0 -> K*(892)- K+ K+",  "D0 -> K*(892)+ K- K-"  
                                                                  ,"D0 -> K*(892)- pi+ K+",  "D0 -> K*(892)+ pi- K-"  
                                                                  ,"D0 -> K*(892)0 pi+ K-",  "D0 -> K*(892)0 pi- K+"  
                                                                  ,"D0 -> K*(892)0 pi+ pi-",  "D0 -> K*(892)0 K+ K-" ]
                                                 )   

        sigMassCut  = "in_range(%s, M, %s)" \
                      % (self.getProp('Sig_M_MIN'), self.getProp('Sig_M_MAX'))

        wideMassCut = "in_range(%s, M, %s)" \
                      % (self.getProp('WideMass_M_MIN'), \
                         self.getProp('WideMass_M_MAX'))

        # 4Body line
        Hlt2Charm4Body = self.__4BodyFilter ( name = 'CharmHadD02HHHH', inputSeq = [Charm4BodyCombine], extracode = sigMassCut )
        # 4Body WideMass line - with prescale
        Hlt2Charm4BodyWideMass = self.__4BodyFilter (name = 'CharmHadD02HHHHWideMass', inputSeq = [Charm4BodyCombine], extracode = wideMassCut )

        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        ##########################################################################
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()

        line = Hlt2Line('CharmHad2BodyForD02HHHH', prescale = self.prescale
                        , algos = [ Hlt2CharmKillTooManyInTrk, PV3D(), Hlt2Charm2BodyFor4Body]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmHad2BodyForD02HHHHDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmHadD02HHHH', prescale = self.prescale
                        , algos = [ Hlt2CharmKillTooManyInTrk, PV3D(), Hlt2Charm2BodyFor4Body, pionsFor4Body, kaonsFor4Body, kaonsLowIP, pionsLowIP, Hlt2Charm4Body]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmHadD02HHHHDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmHadD02HHHHWideMass', prescale = self.prescale
                        , algos = [ Hlt2CharmKillTooManyInTrk, PV3D(), Hlt2Charm2BodyFor4Body, pionsFor4Body, kaonsFor4Body, kaonsLowIP, pionsLowIP, Hlt2Charm4BodyWideMass]
                        , postscale = self.postscale
                        )
        decName = "Hlt2CharmHadD02HHHHWideMassDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

