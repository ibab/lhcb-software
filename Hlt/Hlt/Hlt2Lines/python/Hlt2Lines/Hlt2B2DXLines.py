###
#
# Hlt2 B2DX lines
# Author Jacopo Nardulli
#
###

from Gaudi.Configuration import * 

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2B2DXLinesConf(HltLinesConfigurableUser) :


    # steering variables
    #------------------------
    # Don't touch my variables!

    __slots__ = { 'ComRobAllTrkPtLL'        : 200        # in MeV
                  , 'ComRobAllTrkPLL'         : 2000       # in MeV
                  , 'ComRobAllTrkPVIPLL'      : 0.125       # in mm
                  , 'ComRobPairMinDocaUL'     : 0.2       # in mm
                  , 'ComRobPairMaxDocaUL'     : 1          # in mm
                  , 'ComRobTrkMaxPtLL'        : 1500       # in MeV
                  , 'ComRobVtxPVDispLL'       : 0.0        # in mm
                  , 'ComRobVtxPVRDispLL'      : 0.2        # in mm
                  , 'ComRobUseGEC'            : True       # do or do not 
                  , 'ComRobGEC'               : 120        # max number of tracks
                  , 'ComTFAllTrkPtLL'         : 200        # in MeV
                  , 'ComTFAllTrkPLL'          : 2000       # in MeV
                  , 'ComTFAllTrkPVIPChi2LL'   : 4          # unitless
                  , 'ComTFAllTrkChi2UL'       : 10         # unitless
                  , 'ComTFPairMinDocaUL'      : 0.2        # in mm
                  , 'ComTFPairMaxDocaUL'      : 1          # in mm
                  , 'ComTFTrkMaxPtLL'         : 1500       # in MeV
                  , 'ComTFVtxPVDispChi2LL'    : 225        # unitless
                  , 'ComTFVtxPVDDispChi2LL'   : 196        # for the D: unitless
                  , 'RobustCoplanUL'          : 1.0        # in mm
                  , 'Prescale'                : {'Hlt2B2D2hhBachelorReq3BodyWithKsBroadMW' : 0.01 # These are the broad MW lines 
                                                 , 'Hlt2B2D2hhBachelorReq4BodyWithKsBroadMW' : 0.01
                                                 , 'Hlt2B2D2hhhBachelorReq3BodyWithKsBroadMW' : 0.01
                                                 , 'Hlt2B2D2hhhBachelorReq4BodyWithKsBroadMW' : 0.01
                                                 , 'Hlt2B2D2hhKstarReq3BodyWithKsBroadMW': 0.01
                                                 , 'Hlt2B2D2hhKstarReq4BodyWithKsBroadMW': 0.01
                                                 , 'Hlt2B2D2hhBachelorReq3BodyWithoutKsBroadMW' : 0.01
                                                 , 'Hlt2B2D2hhBachelorReq4BodyWithoutKsBroadMW' : 0.01
                                                 , 'Hlt2B2D2hhhBachelorReq3BodyWithoutKsBroadMW' : 0.01
                                                 , 'Hlt2B2D2hhhBachelorReq4BodyWithoutKsBroadMW' : 0.01
                                                 , 'Hlt2B2D2hhKstarReq3BodyWithoutKsBroadMW': 0.01
                                                 , 'Hlt2B2D2hhKstarReq4BodyWithoutKsBroadMW': 0.01
                                                 , 'Hlt2B2D2XRobust3Body' : 0.001 # Robust stage monitoring lines
                                                 , 'Hlt2B2D2XRobust4Body' : 0.001
                                                 , 'Hlt2B2D2hhBachelorWithKsSignal' : 0.001 # Post TF stage monitoring lines
                                                 , 'Hlt2B2D2hhhBachelorWithKsSignal' : 0.001
                                                 , 'Hlt2B2D2hhKstarWithKsSignal' : 0.001
                                                 , 'Hlt2B2D2hhBachelorWithoutKsSignal' : 0.001 
                                                 , 'Hlt2B2D2hhhBachelorWithoutKsSignal' : 0.001
                                                 , 'Hlt2B2D2hhKstarWithoutKsSignal' : 0.001
                                                 , 'Hlt2B2D2hhBachelorWithKsBroadMW' : 0.001 
                                                 , 'Hlt2B2D2hhhBachelorWithKsBroadMW' : 0.001
                                                 , 'Hlt2B2D2hhKstarWithKsBroadMW' : 0.001
                                                 , 'Hlt2B2D2hhBachelorWithoutKsBroadMW' : 0.001 
                                                 , 'Hlt2B2D2hhhBachelorWithoutKsBroadMW' : 0.001
                                                 , 'Hlt2B2D2hhKstarWithoutKsBroadMW' : 0.001
                                                 }
                  }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltLine import bindMembers
        from Configurables import FilterDesktop,CombineParticles
        from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
        from Configurables import HltANNSvc
	from Hlt2SharedParticles.Ks import KsDD 
	from HltTracking.HltPVs import PV3D

        ###################################################################
        # Absorb the shared particle reconstruction into this configurable.
        # Start with a GEC on all events with more than 120 tracks
        #  
        # This is a temporary fix only and will be removed once proper
        # GEC lines are implemented
        ###################################################################
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        modules =  CoreFactory('CoreFactory').Modules
        
	from HltTracking.Hlt2TrackingConfigurations import Hlt2UnfittedForwardTracking
	Hlt2UnfittedForwardTracking = Hlt2UnfittedForwardTracking()
	

	for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)
            
            Hlt2KillTooManyDXIPFilter = VoidFilter('Hlt2KillTooManyDXIP'
                                             , Code = "TrSOURCE('"+(Hlt2UnfittedForwardTracking.hlt2PrepareTracks()).outputSelection()+"') >> (TrSIZE < %(ComRobGEC)s )" % self.getProps()
                                             )
            Hlt2KillTooManyDXIP = bindMembers( None, [Hlt2UnfittedForwardTracking.hlt2PrepareTracks(), Hlt2KillTooManyDXIPFilter])
            
        ###################################################################
        # Construct a combined sequence for the input particles to the robust
        # stage.
        ###################################################################
              
        daugcuts = """(PT> %(ComRobAllTrkPtLL)s *MeV)
        & (P> %(ComRobAllTrkPLL)s *MeV)
        & (MIPDV(PRIMARY)> %(ComRobAllTrkPVIPLL)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = [GoodKaons]
                             , Code = daugcuts
                             )
        if self.getProp('ComRobUseGEC') :
            lclInputParticles = bindMembers( 'DXInputParticles', [ Hlt2KillTooManyDXIP, GoodKaons, PV3D(), filter ] )
        else :
            lclInputParticles = bindMembers( 'DXInputParticles', [GoodKaons, PV3D(), filter ] )
                
                
                
        ############################################################
        # Function to configure common particle combinations used by inclusive
        #   hadronic lines.
        ###################################################################
        def robustCombine(name, inputSeq, decayDesc, extracuts = None) :
            # Construct a cut string for the combination.
            combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()
            
            # extracuts allows additional cuts to be applied for special
            #   cases, including the tight doca requirement of the 2-body and
            #   the additional cuts to reduce stored combinations in the 4-body.
            if extracuts and extracuts.has_key('CombinationCut') :
                combcuts = combcuts + '&' + extracuts['CombinationCut']
                
                # Construct a cut string for the vertexed combination.
            parentcuts = """
                 (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(ComRobTrkMaxPtLL)s *MeV)
                 & (BPVVD> %(ComRobVtxPVDispLL)s )
                 & (BPVVDR> %(ComRobVtxPVRDispLL)s )""" % self.getProps()
            if extracuts and extracuts.has_key('MotherCut') :
                parentcuts = parentcuts  + '&' + extracuts['MotherCut']
                
            combineDXNBody = Hlt2Member( CombineParticles
                                         , 'Combine'
                                         , DecayDescriptors = decayDesc
                                         , InputLocations = inputSeq
                                         , CombinationCut = combcuts
                                         , MotherCut = parentcuts
                                         )
            #explicitly demand the PV reco because we are paranoid    
            RobustNBody = bindMembers( name, inputSeq + [ PV3D(), combineDXNBody ] )
            return RobustNBody
                
                    
        ###################################################################
        # Create sequences for the shared combinatorics of the robust stages.
        ##################################################################
                    
        # CombineParticles for the robust 2, 3 and 4 -body combinations.
        ###################################################################
        Robust2Body = robustCombine(  name = 'Robust2Body'
                                      , inputSeq = [ lclInputParticles ]
                                      , decayDesc = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
                                      , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMinDocaUL)s )" % self.getProps() }
                                      )
        
        Robust3Body = robustCombine(  name = 'Robust3Body'
                                      , inputSeq = [ lclInputParticles, Robust2Body ]
                                      , decayDesc = ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"]
                                      , extracuts = {'MotherCut'     : "(BPVDVDOCA()< %(RobustCoplanUL)s )" % self.getProps() }
                                      )
        
        Robust4Body = robustCombine(  name = 'Robust4Body'
                                      , inputSeq = [lclInputParticles, Robust3Body ]
                                      , decayDesc = ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]
                                      , extracuts = {'MotherCut'     : "(BPVDVDOCA()< %(RobustCoplanUL)s )" % self.getProps()
                                                     }
                                  )
        
        # filter the 3 and 4 body combinations
        #####################################################
        def RobustFilter(name, inputSeq, extracode = None) :
            codestr = "(M>4000*MeV)" 
            if extracode :
                codestr = codestr + '&' + extracode
            filter = Hlt2Member( FilterDesktop
                                 , 'DXRobustFilter'
                                 , InputLocations = inputSeq
                                 , Code = codestr
                                 )
	    #explicitly require the primary vertex as we are paranoid
            filterSeq = bindMembers(name, inputSeq + [ filter ])
            return filterSeq
            
        Robust3BodySeq = RobustFilter('Robust3BodySeq', [Robust3Body])
        Robust4BodySeq = RobustFilter('Robust4BodySeq', [Robust4Body])
        
    
        ###################################################################
        # Now absorb the post-track fit shared particles into this configurable
        # This largely repeats the work of the robust on a different set
        #   of input particles.
        ###################################################################
        
        # Filter for the post-track-fit input particles
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons,BiKalmanFittedPions
        incuts = """(PT> %(ComTFAllTrkPtLL)s *MeV)
        & (P> %(ComTFAllTrkPLL)s *MeV)
        & (MIPCHI2DV(PRIMARY)> %(ComTFAllTrkPVIPChi2LL)s )
        & (TRCHI2DOF< %(ComTFAllTrkChi2UL)s )""" % self.getProps()
        
        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = [BiKalmanFittedPions,BiKalmanFittedKaons]
                             , Code = incuts
                             )
        #explicitly require the primary vertex as we are paranoid
        lclTFInputParticles = bindMembers('TopoTFIn', [ BiKalmanFittedPions, BiKalmanFittedKaons, PV3D(), filter ])
        
        
        ###################################################################
        # Sequences for new combinatorics with Ks candidates.
        # DX 2-body combinations with a Ks.
        ###################################################################
        excode = " ALL "
        
        FilterDXTFParticles = Hlt2Member( FilterDesktop
                                          , 'FilterDXTFParticles'
                                          , InputLocations = [lclTFInputParticles]
                                          , Code = excode
                                          ) 
   
        FilterKsDD = Hlt2Member( FilterDesktop
                                 , 'FilterKsDD'
                                 , InputLocations = [KsDD]
                                 , Code =  excode
                                 )
        
        DXTFInputParticles = bindMembers( 'DXTFInputParticles', [ lclTFInputParticles, FilterDXTFParticles ] )
        KS0DXTFInputParticles = bindMembers( 'KS0DXTFInputParticles', [ KsDD, FilterKsDD ] )
        
        ################################################################
        # Function to configure post-track-fit common particle combinations
        #   used by inclusive hadronic lines.  
        ###################################################################
        def tfCombine(name, inputSeq, decayDesc, extracuts = None) :
            # Construct a cut string for the combination.
            combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()
            
            # extracuts allows additional cuts to be applied for special
            #   cases, including the tight doca requirement of the 2-body and
            #   the additional cuts to reduce stored combinations in the 4-body.
            if extracuts and extracuts.has_key('CombinationCut') :
                combcuts = combcuts + '&' + extracuts['CombinationCut']
                
                # Construct a cut string for the vertexed combination.
            parentcuts = """(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(ComTFTrkMaxPtLL)s *MeV)""" % self.getProps()
                
            if extracuts and extracuts.has_key('MotherCut') :
                parentcuts = parentcuts  + '&' + extracuts['MotherCut']
                
            combineDXNBody = Hlt2Member( CombineParticles
                                         , 'Combine'
                                         , DecayDescriptors = decayDesc
                                         , InputLocations = inputSeq
                                         , CombinationCut = combcuts
                                         , MotherCut = parentcuts
                                         )
            #explicitly require the primary vertex as we are paranoid
            TFNBody = bindMembers( name, inputSeq + [ PV3D(), combineDXNBody ] )
            return TFNBody
        
        ################################################################### 
        # Create sequences for the shared combinatorics of the post-track-fit
        #   stages.
        ###################################################################
        
        # post-track-fit 2-body and 3 body combinations
        ###################################################################
         
        TF2BodyWithKs = tfCombine(  name = 'TF2BodyWithKs'
                                    , inputSeq = [ KS0DXTFInputParticles, DXTFInputParticles ]
                                    , decayDesc = ["K*(892)0 -> KS0 pi+","K*(892)0 -> KS0 pi-","K*(892)0 -> KS0 K+","K*(892)0 -> KS0 K-"]
                                    , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMinDocaUL)s )" % self.getProps() }
                                    )

        TF2BodyWithoutKs = tfCombine(  name = 'TF2BodyWithoutKs'
                                       , inputSeq = [ DXTFInputParticles ]
                                       , decayDesc = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"]
                                       , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMinDocaUL)s )" % self.getProps() }
                                       )
        
        TF3BodyWithKs = tfCombine(  name = 'TF3BodyWithKs'
                                    , inputSeq = [ KS0DXTFInputParticles, TF2BodyWithoutKs ]
                                    , decayDesc =[ "D*(2010)+ -> K*(892)0 KS0"]
                                    )

        TF3BodyWithoutKs = tfCombine(  name = 'TF3BodyWithoutKs'
                                       , inputSeq = [ DXTFInputParticles, TF2BodyWithoutKs ]
                                       , decayDesc =[ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-","D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"]
                                       )
        
         
        # Filter the combinations
        ###################################################################
        def TFFilter(name, inputSeq, extracode = None) :
            codestr = " ALL "
            if extracode :
                codestr = codestr + '&' + extracode
                filter = Hlt2Member( FilterDesktop
                                     , 'TFFilter'
                                     , InputLocations = inputSeq
                                     , Code = codestr
                                     )
                filterSeq = bindMembers( name, inputSeq + [ filter ] )
                return filterSeq
            
        # Main 2 and 3 body charm post-track-fit combinations
        D2TF2BodyWithKsSeq = TFFilter('D2TF2BodyWithKsSeq'
                                      ,[TF2BodyWithKs]
                                      , extracode = '(BPVVDCHI2> %(ComTFVtxPVDDispChi2LL)s )' % self.getProps() )

        D2TF2BodyWithoutKsSeq = TFFilter('D2TF2BodyWithoutKsSeq'
                                         ,[TF2BodyWithoutKs]
                                         , extracode = '(BPVVDCHI2> %(ComTFVtxPVDDispChi2LL)s ) & (SUMQ == 0)' % self.getProps() )

        D2TF3BodyWithKsSeq = TFFilter('D2TF3BodyWithKsSeq'
                                      , [TF3BodyWithKs]
                                      , extracode = '(BPVVDCHI2> %(ComTFVtxPVDDispChi2LL)s )' % self.getProps())

        D2TF3BodyWithoutKsSeq = TFFilter('D2TF3BodyWithoutKsSeq'
                                         , [TF3BodyWithoutKs]
                                         , extracode = '(BPVVDCHI2> %(ComTFVtxPVDDispChi2LL)s )' % self.getProps())

        # 2 Body combination for a K* or a Phi
        D2TF2BodyKstarPhiSeq = TFFilter('D2TF2BodyKstarPhiSeq'
                                        , [TF2BodyWithoutKs]
                                        , extracode = '(SUMQ == 0)')

        
        # Various combine particles B combinations : B -> D(hh)h ; B -> D(hh)K* ; B -> D(hh)Phi ; B -> D(hhh)h 
        B2D2hhBachelorWithKsSeq = tfCombine(name = 'B2D2hhBachelorWithKsSeq'
                                            , inputSeq = [ DXTFInputParticles, D2TF2BodyWithKsSeq ]
                                            , decayDesc = ["B0 -> K*(892)0  pi-","B0 -> K*(892)0  pi+"]
                                            , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )" % self.getProps()})

        B2D2hhBachelorWithoutKsSeq = tfCombine(name = 'B2D2hhBachelorWithoutKsSeq'
                                            , inputSeq = [ DXTFInputParticles, D2TF2BodyWithoutKsSeq ]
                                            , decayDesc = ["B0 -> K*(892)0  pi-","B0 -> K*(892)0  pi+"]
                                            , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )" % self.getProps()})
        
        B2D2hhKstarPhiWithKsSeq  = tfCombine(name = 'B2D2hhKstarPhiWithKsSeq'
                                             , inputSeq = [ D2TF2BodyKstarPhiSeq, D2TF2BodyWithKsSeq ]
                                             , decayDesc = ["B0 -> K*(892)0 K*(892)0  ","B0 -> K*(892)0 K*(892)0"]
                                             , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )" % self.getProps()})

        B2D2hhKstarPhiWithoutKsSeq  = tfCombine(name = 'B2D2hhKstarPhiWithoutKsSeq'
                                             , inputSeq = [ D2TF2BodyKstarPhiSeq, D2TF2BodyWithoutKsSeq ]
                                             , decayDesc = ["B0 -> K*(892)0 K*(892)0  ","B0 -> K*(892)0 K*(892)0"]
                                             , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )" % self.getProps()})

        
        B2D2hhhBachelorWithKsSeq = tfCombine(name = 'B2D2hhhBachelorWithKsSeq'
                                             , inputSeq = [ DXTFInputParticles, D2TF3BodyWithKsSeq]
                                             , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)- pi+"]
                                             , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )" % self.getProps()})

        B2D2hhhBachelorWithoutKsSeq = tfCombine(name = 'B2D2hhhBachelorWithoutKsSeq'
                                                , inputSeq = [ DXTFInputParticles, D2TF3BodyWithoutKsSeq]
                                                , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)- pi+"]
                                                , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )" % self.getProps()})

        
        # Filter everything in signal and broad mass windows
        B2D2hhBachelorWithKsSeqSignalFilter = TFFilter('B2D2hhBachelorWithKsSeqSignalFilter', [B2D2hhBachelorWithKsSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhKstarPhiWithKsSeqSignalFilter = TFFilter('B2D2hhKstarPhiWithKsSeqSignalFilter', [B2D2hhKstarPhiWithKsSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhhBachelorWithKsSeqSignalFilter = TFFilter('B2D2hhhBachelorWithKsSeqSignalFilter', [B2D2hhhBachelorWithKsSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhBachelorWithKsSeqMWFilter = TFFilter('B2D2hhBachelorWithKsSeqMWFilter', [B2D2hhBachelorWithKsSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhKstarPhiWithKsSeqMWFilter = TFFilter('B2D2hhKstarPhiWithKsSeqMWFilter', [B2D2hhKstarPhiWithKsSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhhBachelorWithKsSeqMWFilter = TFFilter('B2D2hhhBachelorWithKsSeqMWFilter', [B2D2hhhBachelorWithKsSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhBachelorWithoutKsSeqSignalFilter = TFFilter('B2D2hhBachelorWithoutKsSeqSignalFilter', [B2D2hhBachelorWithoutKsSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhKstarPhiWithoutKsSeqSignalFilter = TFFilter('B2D2hhKstarPhiWithoutKsSeqSignalFilter', [B2D2hhKstarPhiWithoutKsSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhhBachelorWithoutKsSeqSignalFilter = TFFilter('B2D2hhhBachelorWithoutKsSeqSignalFilter', [B2D2hhhBachelorWithoutKsSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhBachelorWithoutKsSeqMWFilter = TFFilter('B2D2hhBachelorWithoutKsSeqMWFilter', [B2D2hhBachelorWithoutKsSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhKstarPhiWithoutKsSeqMWFilter = TFFilter('B2D2hhKstarPhiWithoutKsSeqMWFilter', [B2D2hhKstarPhiWithoutKsSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhhBachelorWithoutKsSeqMWFilter = TFFilter('B2D2hhhBachelorWithoutKsSeqMWFilter', [B2D2hhhBachelorWithoutKsSeq], extracode = "(M>4*GeV) & (M<6*GeV)")


        # Make the lines for signal
        line = Hlt2Line('B2D2hhBachelorReq3BodyWithKsSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhBachelorWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq3BodyWithKsSignalDecision" : 52100 } )
        line = Hlt2Line('B2D2hhBachelorReq4BodyWithKsSignal', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhBachelorWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq4BodyWithKsSignalDecision" : 52110 } )
        line = Hlt2Line('B2D2hhhBachelorReq3BodyWithKsSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhhBachelorWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq3BodyWithKsSignalDecision" : 52120 } )
        line = Hlt2Line('B2D2hhhBachelorReq4BodyWithKsSignal', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhhBachelorWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq4BodyWithKsSignalDecision" : 52130 } )
        line = Hlt2Line('B2D2hhKstarReq3BodyWithKsSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhKstarPhiWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq3BodyWithKsSignalDecision" : 52140 } )
        line = Hlt2Line('B2D2hhKstarReq4BodyWithKsSignal', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhKstarPhiWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq4BodyWithKsSignalDecision" : 52150 } )
        line = Hlt2Line('B2D2hhBachelorReq3BodyWithoutKsSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhBachelorWithoutKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq3BodyWithoutKsSignalDecision" : 52160 } )
        line = Hlt2Line('B2D2hhBachelorReq4BodyWithoutKsSignal', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhBachelorWithoutKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq4BodyWithoutKsSignalDecision" : 52170 } )
        line = Hlt2Line('B2D2hhhBachelorReq3BodyWithoutKsSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhhBachelorWithoutKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq3BodyWithoutKsSignalDecision" : 52180 } )
        line = Hlt2Line('B2D2hhhBachelorReq4BodyWithoutKsSignal', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhhBachelorWithoutKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq4BodyWithoutKsSignalDecision" : 52190 } )
        line = Hlt2Line('B2D2hhKstarReq3BodyWithoutKsSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhKstarPhiWithoutKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq3BodyWithoutKsSignalDecision" : 52200 } )
        line = Hlt2Line('B2D2hhKstarReq4BodyWithoutKsSignal', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhKstarPhiWithoutKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq4BodyWithoutKsSignalDecision" : 52210 } )

        # and for the broader mass windows
        line = Hlt2Line('B2D2hhBachelorReq3BodyWithKsBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhBachelorWithKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq3BodyWithKsBroadMWDecision" : 52300 } )
        line = Hlt2Line('B2D2hhBachelorReq4BodyWithKsBroadMW', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhBachelorWithKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq4BodyWithKsBroadMWDecision" : 52310 } )
        line = Hlt2Line('B2D2hhhBachelorReq3BodyWithKsBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhhBachelorWithKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq3BodyWithKsBroadMWDecision" : 52320 } )
        line = Hlt2Line('B2D2hhhBachelorReq4BodyWithKsBroadMW', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhhBachelorWithKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq4BodyWithKsBroadMWDecision" : 52330 } )
        line = Hlt2Line('B2D2hhKstarReq3BodyWithKsBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhKstarPhiWithKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq3BodyWithKsBroadMWDecision" : 52340 } )
        line = Hlt2Line('B2D2hhKstarReq4BodyWithKsBroadMW', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhKstarPhiWithKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq4BodyWithKsBroadMWDecision" : 52350 } )
        line = Hlt2Line('B2D2hhBachelorReq3BodyWithoutKsBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhBachelorWithoutKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq3BodyWithoutKsBroadMWDecision" : 52360 } )
        line = Hlt2Line('B2D2hhBachelorReq4BodyWithoutKsBroadMW', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhBachelorWithoutKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq4BodyWithoutKsBroadMWDecision" : 52370 } )
        line = Hlt2Line('B2D2hhhBachelorReq3BodyWithoutKsBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhhBachelorWithoutKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq3BodyWithoutKsBroadMWDecision" : 52380 } )
        line = Hlt2Line('B2D2hhhBachelorReq4BodyWithoutKsBroadMW', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhhBachelorWithoutKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq4BodyWithoutKsBroadMWDecision" : 52390 } )
        line = Hlt2Line('B2D2hhKstarReq3BodyWithoutKsBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhKstarPhiWithoutKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq3BodyWithoutKsBroadMWDecision" : 52400 } )
        line = Hlt2Line('B2D2hhKstarReq4BodyWithoutKsBroadMW', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhKstarPhiWithoutKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq4BodyWithoutKsBroadMWDecision" : 52410 } )

        #Robust monitoring Lines
        line = Hlt2Line('B2D2XRobust3Body', prescale = self.prescale , algos = [ Robust3BodySeq], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2XRobust3BodyDecision" : 52420 } )
        line = Hlt2Line('B2D2XRobust4Body', prescale = self.prescale , algos = [ Robust4BodySeq], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2XRobust4BodyDecision" : 52430 } )


        # Post TF monitoring lines
        line = Hlt2Line('B2D2hhBachelorWithKsSignal', prescale = self.prescale , algos = [ B2D2hhBachelorWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorWithKsSignalDecision" : 52500 } )
        line = Hlt2Line('B2D2hhhBachelorWithKsSignal', prescale = self.prescale , algos = [ B2D2hhhBachelorWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithKsSignalDecision" : 52510 } )
        line = Hlt2Line('B2D2hhKstarWithKsSignal', prescale = self.prescale , algos = [ B2D2hhKstarPhiWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarWithKsSignalDecision" : 52520 } )
        line = Hlt2Line('B2D2hhBachelorWithoutKsSignal', prescale = self.prescale , algos = [ B2D2hhBachelorWithoutKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorWithoutKsSignalDecision" : 52530 } )
        line = Hlt2Line('B2D2hhhBachelorWithoutKsSignal', prescale = self.prescale , algos = [ B2D2hhhBachelorWithoutKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithoutKsSignalDecision" : 52540 } )
        line = Hlt2Line('B2D2hhKstarWithoutKsSignal', prescale = self.prescale , algos = [ B2D2hhKstarPhiWithoutKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarWithoutKsSignalDecision" : 52550 } )
        line = Hlt2Line('B2D2hhBachelorWithKsBroadMW', prescale = self.prescale , algos = [ B2D2hhBachelorWithKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorWithKsBroadMWDecision" : 52560 } )
        line = Hlt2Line('B2D2hhhBachelorWithKsBroadMW', prescale = self.prescale , algos = [ B2D2hhhBachelorWithKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithKsBroadMWDecision" : 52570 } )
        line = Hlt2Line('B2D2hhKstarWithKsBroadMW', prescale = self.prescale , algos = [ B2D2hhKstarPhiWithKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarWithKsBroadMWDecision" : 52580 } )
        line = Hlt2Line('B2D2hhBachelorWithoutKsBroadMW', prescale = self.prescale , algos = [ B2D2hhBachelorWithoutKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorWithoutKsBroadMWDecision" : 52590 } )
        line = Hlt2Line('B2D2hhhBachelorWithoutKsBroadMW', prescale = self.prescale , algos = [ B2D2hhhBachelorWithoutKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithoutKsBroadMWDecision" : 52600 } )
        line = Hlt2Line('B2D2hhKstarWithoutKsBroadMW', prescale = self.prescale , algos = [ B2D2hhKstarPhiWithoutKsSeqMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarWithoutKsBroadMWDecision" : 52610 } )


