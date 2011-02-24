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
                  , 'TrkPt_2BodyFor4Body'         : 500.0    # in MeV
                  , 'TrkP_2BodyFor4Body'          : 5000.0   # in MeV
                  , 'TrkPVIPChi2_2BodyFor4Body'   : 7.0      # unitless
                  , 'TrkChi2_2BodyFor4Body'       : 5.0      # unitless
                  , 'Doca_2BodyFor4Body'          : 0.085     # in mm
                  , 'VtxPVDispChi2_2BodyFor4Body' : 40.      # unitless  
                  , 'VtxPVDisp_2BodyFor4Body'     : 3.0      # in mm
                  , 'DSumPt_2BodyFor4Body'        : 1400.0   # in MeV
                  , 'MCOR_MAX_2BodyFor4Body'        : 3500.    # MeV
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

    def __InPartFilter2BodyFor4Body(self, name, inputContainers) :  
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """(MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_2BodyFor4Body)s )
                    & (PT> %(TrkPt_2BodyFor4Body)s *MeV)
                    & (P> %(TrkP_2BodyFor4Body)s *MeV)  """ % self.getProps()


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

        preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                     "BPVCORRM = sqrt(M**2 + PTRANS**2) + PTRANS"]

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
                          , Preambulo = preambulo 
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
        pionsLowIP = self.__InPartFilterLowIP('CharmInputPionsLowIP', [ BiKalmanFittedPions] )
        kaonsLowIP = self.__InPartFilterLowIP('CharmInputKaonsLowIP', [ BiKalmanFittedKaons] )

         
        # Tighten IP cut
        pions2BodyFor4Body = self.__InPartFilter2BodyFor4Body('CharmInputPions2BodyFor4Body', [ pionsLowIP ] )
        kaons2BodyFor4Body = self.__InPartFilter2BodyFor4Body('CharmInputKaons2BodyFor4Body', [ kaonsLowIP ] )
 
       
        # Evaluate mcor for 2 Body
        preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                     "BPVCORRM = sqrt(M**2 + PTRANS**2) + PTRANS"]

        #First stage - Combine 2 Body with pt > 500MeV	
	Charm2BodyCombine = Hlt2Member( CombineParticles
                          , "Combine_Stage1"
                          , DecayDescriptors = ["K*(892)0 -> pi+ pi+" , "K*(892)0 -> pi+ pi-"
                          , "K*(892)0 -> pi- pi-" , "K*(892)0 -> K+ K+"
                          , "K*(892)0 -> K+ K-"   , "K*(892)0 -> K- K-"
                          , "K*(892)0 -> K+ pi-"  , "K*(892)0 -> pi+ K-"
                          , "K*(892)0 -> K+ pi+"  , "K*(892)0 -> K- pi-" ]
			  , CombinationCut = """((APT1+APT2)> %(DSumPt_2BodyFor4Body)s) 
			       & (AM<2100*MeV) 
			       & (AMINDOCA('LoKi::TrgDistanceCalculator') < %(Doca_2BodyFor4Body)s )
                               & (AALLSAMEBPV)""" % self.getProps()
                          , MotherCut = """(BPVCORRM < %(MCOR_MAX_2BodyFor4Body)s*MeV)
                                         & (BPVVD> %(VtxPVDisp_2BodyFor4Body)s )
                                         & (BPVVDCHI2> %(VtxPVDispChi2_2BodyFor4Body)s )""" % self.getProps()
                          , Preambulo = preambulo 
                          , Inputs = [ pions2BodyFor4Body , kaons2BodyFor4Body ])

        Hlt2Charm2BodyFor4Body =  bindMembers('CharmHadD02HHHH', [ pions2BodyFor4Body , kaons2BodyFor4Body, Charm2BodyCombine ])
			  
        #Second Stage - picks up two low pt tracks too

        # Filter low PT pions and kaons 

        pionsFor4Body = self.__InPartFilter4Body('CharmInputPions4Body', [ BiKalmanFittedSecondLoopPions] )
        kaonsFor4Body = self.__InPartFilter4Body('CharmInputKaons4Body', [ BiKalmanFittedSecondLoopKaons] )
 
        # Make 4Body 
        Charm4BodyCombine = self.__4BodyCombine ( name = 'CharmHadD02HHHH'
                                                  , inputSeq = [ Hlt2Charm2BodyFor4Body, pionsFor4Body, kaonsFor4Body, kaonsLowIP, pionsLowIP ]
                                                  , decayDesc = [ "D0 -> K*(892)0 pi+ pi+", "D0 -> K*(892)0 pi- pi-"
                                                                  ,"D0 -> K*(892)0 K+ K+",  "D0 -> K*(892)0 K- K-"  
                                                                  ,"D0 -> K*(892)0 pi+ K+",  "D0 -> K*(892)0 pi- K-"  
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
                        , algos = [ PV3D(), pions2BodyFor4Body , kaons2BodyFor4Body, Hlt2Charm2BodyFor4Body]
			, postscale = self.postscale
                        )
        decName = "Hlt2CharmHad2BodyForD02HHHHDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmHadD02HHHH', prescale = self.prescale
                        , algos = [ PV3D(), pions2BodyFor4Body , kaons2BodyFor4Body, Hlt2Charm2BodyFor4Body, pionsFor4Body, kaonsFor4Body, kaonsLowIP, pionsLowIP, Hlt2Charm4Body]
			, postscale = self.postscale
                        )
        decName = "Hlt2CharmHadD02HHHHDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line('CharmHadD02HHHHWideMass', prescale = self.prescale
                        , algos = [ PV3D(), pions2BodyFor4Body , kaons2BodyFor4Body, Hlt2Charm2BodyFor4Body, pionsFor4Body, kaonsFor4Body, kaonsLowIP, pionsLowIP, Hlt2Charm4BodyWideMass]
			, postscale = self.postscale
                        )
        decName = "Hlt2CharmHadD02HHHHWideMassDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
	
