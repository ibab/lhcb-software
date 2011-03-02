from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2CharmHadD02HHHHLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## 4Body
                    'TrkPt_4Body'                 : 250.0    # in MeV
                  , 'TrkP_4Body'                  : 2000.0   # in MeV
                  , 'TrkPVIPChi2_4Body'           : 2.0      # unitless
                  , 'TrkChi2_4Body'               : 5.0      # unitless
                  , 'PairMinDoca_4Body'      : 0.10     # in mm
                  , 'VtxPVDispChi2_4Body'         : 100.0    # unitless
                  , 'VtxChi2_4Body'               : 20.0     # unitless
                  , 'DIPChi2_4Body'               : 25.0     # unitless
                  , 'DSumPt_4Body'                : 2000.0   # sum pT   
                  , 'MCOR_MAX_4Body'                : 3500.    # MeV
                  ## 2-body Input for 4Body
                  , 'TrkChi2_2BodyFor4Body'       : 5.0      # unitless
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

    def __InPartFilterLowIP(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """ (TRCHI2DOF< %(TrkChi2_2BodyFor4Body)s )
                    & (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_4Body)s )""" % self.getProps()

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

        incuts = """(PT> %(TrkPt_4Body)s *MeV)
                    & (P> %(TrkP_4Body)s *MeV)
                    & (TRCHI2DOF< %(TrkChi2_4Body)s )
                    & (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_4Body)s )""" % self.getProps()


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

        combcuts = """( (APT1+APT2+APT3+APT4) > %(DSumPt_4Body)s) 
                       & (AM<2100*MeV)
                       & (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_4Body)s)
                       & (AALLSAMEBPV)""" % self.getProps()
        mothercuts = """  (BPVCORRM < %(MCOR_MAX_4Body)s*MeV) 
                         & (abs(CHILD(1,SUMQ) + CHILD(2,Q) + CHILD(3,Q))==0)
                         & (VFASPF(VCHI2PDOF) < %(VtxChi2_4Body)s) 
                         & (BPVVDCHI2> %(VtxPVDispChi2_4Body)s )
                         & ( BPVIPCHI2() < %(DIPChi2_4Body)s )""" % self.getProps()
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
        # 4Body line
	Hlt2Charm4Body = self.__4BodyFilter ( name = 'CharmHadD02HHHH', inputSeq = [Charm4BodyCombine], extracode = "in_range(1800*MeV, M, 2040*MeV)")
        # 4Body WideMass line - with prescale
        Hlt2Charm4BodyWideMass = self.__4BodyFilter (name = 'CharmHadD02HHHHWideMass', inputSeq = [Charm4BodyCombine], extracode = "in_range(1700*MeV, M, 2100*MeV)")

        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        ##########################################################################
        line = Hlt2Line('CharmHad2BodyForD02HHHH', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2Charm2BodyFor4Body]
			, postscale = self.postscale
                        )
        decName = "Hlt2CharmHad2BodyForD02HHHHDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmHadD02HHHH', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2Charm2BodyFor4Body, pionsFor4Body, kaonsFor4Body, kaonsLowIP, pionsLowIP, Hlt2Charm4Body]
			, postscale = self.postscale
                        )
        decName = "Hlt2CharmHadD02HHHHDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmHadD02HHHHWideMass', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2Charm2BodyFor4Body, pionsFor4Body, kaonsFor4Body, kaonsLowIP, pionsLowIP, Hlt2Charm4BodyWideMass]
			, postscale = self.postscale
                        )
        decName = "Hlt2CharmHadD02HHHHWideMassDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
	
