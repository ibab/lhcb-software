from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2B2DXLinesConf(HltLinesConfigurableUser) :


    # steering variables
    #------------------------
    # Don't touch my variables!

    __slots__ = { 'ComRobAllTrkPtLL'        : 300        # in MeV
                  , 'ComRobAllTrkPLL'         : 2000       # in MeV
                  , 'ComRobAllTrkPVIPLL'      : 0.05       # in mm
                  , 'ComRobPairMinDocaUL'     : 0.10       # in mm
                  , 'ComRobPairMaxDocaUL'     : 1          # in mm
                  , 'ComRobTrkMaxPtLL'        : 1500       # in MeV
                  , 'ComRobVtxPVDispLL'       : 2          # in mm
                  , 'ComRobVtxPVRDispLL'      : 0.2        # in mm
                  , 'ComRobUseGEC'            : True       # do or do not 
                  , 'ComRobGEC'               : 120        # max number of tracks
                  , 'ComTFAllTrkPtLL'         : 300        # in MeV
                  , 'ComTFAllTrkPLL'          : 2000       # in MeV
                  , 'ComTFAllTrkPVIPChi2LL'   : 9          # unitless
                  , 'ComTFAllTrkChi2UL'       : 10         # unitless
                  , 'ComTFPairMinDocaUL'      : 0.10       # in mm
                  , 'ComTFPairMaxDocaUL'      : 1          # in mm
                  , 'ComTFTrkMaxPtLL'         : 1500       # in MeV
                  , 'ComTFVtxPVDispChi2LL'    : 169        # unitless
                  , 'ComTFVtxPVDDispChi2LL'   : 100        # for the D: unitless
                  , 'RobustCoplanUL'          : 0.02       # in mm
                  }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltLine import bindMembers
        from Configurables import FilterDesktop,CombineParticles
        from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
        from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles
        from Configurables import HltANNSvc, DecayTreeTuple
        from Hlt2SharedParticles.V0 import KsDDFit

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
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)
            
            Hlt2KillTooManyDXIP = VoidFilter('Hlt2KillTooManyDXIP'
                                             , Code = "TrSOURCE('Hlt/Track/Forward') >> (TrSIZE < %(ComRobGEC)s )" % self.getProps()
                                             )
            
        ###################################################################
        # Construct a combined sequence for the input particles to the robust
        # stage.
        ###################################################################
        orInput = GaudiSequencer('Hlt2SharedDX2Body_PionsORKaons'
                                 , Members =  GoodPions.members() + GoodKaons.members()
                                 , ModeOR = True
                                 , ShortCircuit = False
                                 )
        
        daugcuts = """(PT> %(ComRobAllTrkPtLL)s *MeV)
        & (P> %(ComRobAllTrkPLL)s *MeV)
        & (MIPDV(PRIMARY)> %(ComRobAllTrkPVIPLL)s )""" % self.getProps()
        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = [GoodPions, GoodKaons]
                             , Code = daugcuts
                             )
        if self.getProp('ComRobUseGEC') :
            lclInputParticles = bindMembers( 'DXInputParticles', [ Hlt2KillTooManyDXIP, orInput, filter ] )
        else :
            lclInputParticles = bindMembers( 'DXInputParticles', [ orInput, filter ] )
                
                
                
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
                
            RobustNBody = bindMembers( name, inputSeq + [ combineDXNBody ] )
            return RobustNBody
                
                    
        ###################################################################
        # Create sequences for the shared combinatorics of the robust stages.
        ##################################################################
                    
        # CombineParticles for the robust 2, 3 and 4 -body combinations.
        ###################################################################
        Robust2Body = robustCombine(  name = 'Robust2Body'
                                      , inputSeq = [ lclInputParticles ]
                                      , decayDesc = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"]
                                      , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMinDocaUL)s )" % self.getProps() }
                                      )
        
        Robust3Body = robustCombine(  name = 'Robust3Body'
                                      , inputSeq = [ lclInputParticles, Robust2Body ]
                                      , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
                                      , extracuts = {'MotherCut'     : "(BPVDVDOCA()< %(RobustCoplanUL)s )" % self.getProps() }
                                      )
        
        Robust4Body = robustCombine(  name = 'Robust4Body'
                                      , inputSeq = [lclInputParticles, Robust3Body ]
                                      , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                      , extracuts = {'MotherCut'     : "(BPVDVDOCA()< %(RobustCoplanUL)s )" % self.getProps()
                                                     }
                                  )
        
        # filter the 3 and 4 body combinations
        #####################################################
        def RobustFilter(name, inputSeq, extracode = None) :
            codestr = "(M>4000*MeV) & (M<6000*MeV)" % self.getProps()
            if extracode :
                codestr = codestr + '&' + extracode
            filter = Hlt2Member( FilterDesktop
                                 , 'DXRobustFilter'
                                 , InputLocations = inputSeq
                                 , Code = codestr
                                 )
            filterSeq = bindMembers(name, inputSeq + [ filter ])
            return filterSeq
            
        Robust3BodySeq = RobustFilter('Robust3BodySeq', [Robust3Body])
        Robust4BodySeq = RobustFilter('Robust4BodySeq', [Robust4Body])
        
    
        ###################################################################
        # Now absorb the post-track fit shared particles into this configurable
        # This largely repeats the work of the robust on a different set
        #   of input particles.
        ###################################################################
        # import options for the track fit
        importOptions("$HLTCONFROOT/options/Hlt2TrackFitForTopo.py")
        
        
        # Filter for the post-track-fit input particles
        ###################################################################
        incuts = """(PT> %(ComTFAllTrkPtLL)s *MeV)
        & (P> %(ComTFAllTrkPLL)s *MeV)
        & (MIPCHI2DV(PRIMARY)> %(ComTFAllTrkPVIPChi2LL)s )
        & (TRCHI2DOF< %(ComTFAllTrkChi2UL)s )""" % self.getProps()
        
        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = ['Hlt2TFPionsForTopo', 'Hlt2TFKaonsForTopo']
                             , Code = incuts
                             )
        lclTFInputParticles = bindMembers('TopoTFIn', [ GaudiSequencer('SeqHlt2TFParticlesForTopo'), filter ])
        
        
        ###################################################################
        # Sequences for new combinatorics with Ks candidates.
        # DX 2-body combinations with a Ks.
        ###################################################################
        
        ORInput = GaudiSequencer('Hlt2SharedDXTF2BodyInput'
                                 , Members =  lclTFInputParticles.members() + KsDDFit.members()
                                 , ModeOR = True
                                 , ShortCircuit = False
                                 )
        excode = "(PT>0*MeV)"
        Filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = [lclTFInputParticles, KsDDFit]
                             , Code =  excode
                             )
        DXTFInputParticles = bindMembers( 'DXTFInputParticles', [ ORInput, Filter ] )
        
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
            
            TFNBody = bindMembers( name, inputSeq + [ combineDXNBody ] )
            return TFNBody
        
        ################################################################### 
        # Create sequences for the shared combinatorics of the post-track-fit
        #   stages.
        ###################################################################
        
        # post-track-fit 2-body and 3 body combinations
        ###################################################################
         
        TF2Body = tfCombine(  name = 'TF2Body'
                              , inputSeq = [ DXTFInputParticles ]
                              , decayDesc = ["K*(892)0 -> KS0 pi+","K*(892)0 -> KS0 pi-","K*(892)0 -> KS0 K+","K*(892)0 -> KS0 K-","K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"]
                              , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMinDocaUL)s )" % self.getProps() }
                              )
        
        TF3Body = tfCombine(  name = 'TF3Body'
                              , inputSeq = [ DXTFInputParticles, TF2Body ]
                              , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
                              )
        
         
        # Filter the combinations
        ###################################################################
        def TFFilter(name, inputSeq, extracode = None) :
            codestr = "(PT>0*MeV)" % self.getProps()
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
        D2TF2BodySeq = TFFilter('D2TF2BodySeq'
                                , [TF2Body]
#                                , extracode = '(M>1600*MeV) & (M<2100*MeV) & (BPVVDCHI2> %(ComTFVtxPVDDispChi2LL)s ) & (SUMQ == 0)' % self.getProps() )
                                , extracode = '(BPVVDCHI2> %(ComTFVtxPVDDispChi2LL)s ) & (SUMQ == 0)' % self.getProps() )
        D2TF3BodySeq = TFFilter('D2TF3BodySeq'
                                , [TF3Body]
                                , extracode = '(BPVVDCHI2> %(ComTFVtxPVDDispChi2LL)s ) & ((SUMQ == -1) | (SUMQ == 1))' % self.getProps())
        
        # 2 Body combination for a K* or a Phi
        D2TF2BodyKstarPhiSeq = TFFilter('D2TF2BodyKstarPhiSeq'
                                        , [TF2Body]
                                        #, extracode = '(M>692*MeV) & (M<1292*MeV) & (SUMQ == 0)')
                                        , extracode = '(SUMQ == 0)')
        
        # Various combine particles B combinations : B -> D(hh)h ; B -> D(hh)K* ; B -> D(hh)Phi ; B -> D(hhh)h 
        B2D2hhBachelorSeq = tfCombine(name = 'B2D2hhBachelorSeq'
                                      , inputSeq = [ DXTFInputParticles, D2TF2BodySeq ]
                                      , decayDesc = ["B0 -> K*(892)0  pi-","B0 -> K*(892)0  pi+"]
                                      , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )" % self.getProps()})
        
        B2D2hhKstarPhiSeq  = tfCombine(name = 'B2D2hhKstarPhiSeq'
                                       , inputSeq = [ D2TF2BodyKstarPhiSeq, D2TF2BodySeq ]
                                       , decayDesc = ["B0 -> K*(892)0 K*(892)0  ","B0 -> K*(892)0 K*(892)0"]
                                       , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )" % self.getProps()})
        
        B2D2hhhBachelorSeq = tfCombine(name = 'B2D2hhhBachelorSeq'
                                       , inputSeq = [ DXTFInputParticles, D2TF3BodySeq]
                                       , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)- pi+"]
                                       , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )" % self.getProps()})
        

        # Filter everything in signal and broad mass windows
        B2D2hhBachelorSeqSignalFilter = TFFilter('B2D2hhBachelorSeqSignalFilter', [B2D2hhBachelorSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhKstarPhiSeqSignalFilter = TFFilter('B2D2hhKstarPhiSeqSignalFilter', [B2D2hhKstarPhiSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhhBachelorSeqSignalFilter = TFFilter('B2D2hhhBachelorSeqSignalFilter', [B2D2hhhBachelorSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        #Broad MW
        B2D2hhBachelorSeqMWFilter = TFFilter('B2D2hhBachelorSeqMWFilter', [B2D2hhBachelorSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhKstarPhiSeqMWFilter = TFFilter('B2D2hhKstarPhiSeqMWFilter', [B2D2hhKstarPhiSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhhBachelorSeqMWFilter = TFFilter('B2D2hhhBachelorSeqMWFilter', [B2D2hhhBachelorSeq], extracode = "(M>4*GeV) & (M<6*GeV)")


        # Make the lines for signal
        line = Hlt2Line('B2D2hhBachelorReq3BodySignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhBachelorSeqSignalFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq3BodySignalDecision" : 52100 } )
        line = Hlt2Line('B2D2hhBachelorReq4BodySignal', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhBachelorSeqSignalFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq4BodySignalDecision" : 52110 } )
        line = Hlt2Line('B2D2hhhBachelorReq3BodySignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhhBachelorSeqSignalFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq3BodySignalDecision" : 52120 } )
        line = Hlt2Line('B2D2hhhBachelorReq4BodySignal', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhhBachelorSeqSignalFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq4BodySignalDecision" : 52130 } )
        line = Hlt2Line('B2D2hhKstarReq3BodySignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhKstarPhiSeqSignalFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq3BodySignalDecision" : 52140 } )
        line = Hlt2Line('B2D2hhKstarReq4BodySignal', prescale = self.prescale , algos = [ Robust4BodySeq, B2D2hhKstarPhiSeqSignalFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq4BodySignalDecision" : 52150 } )

        # and for the broader mass windows
        line = Hlt2Line('B2D2hhBachelorReq3BodyMW', prescale = 0.01 , algos = [ Robust3BodySeq, B2D2hhBachelorSeqMWFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq3BodyMWDecision" : 52160 } )
        line = Hlt2Line('B2D2hhBachelorReq4BodyMW', prescale = 0.01 , algos = [ Robust4BodySeq, B2D2hhBachelorSeqMWFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorReq4BodyMWDecision" : 52170 } )
        line = Hlt2Line('B2D2hhhBachelorReq3BodyMW', prescale = 0.01 , algos = [ Robust3BodySeq, B2D2hhhBachelorSeqMWFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq3BodyMWDecision" : 52180 } )
        line = Hlt2Line('B2D2hhhBachelorReq4BodyMW', prescale = 0.01 , algos = [ Robust4BodySeq, B2D2hhhBachelorSeqMWFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorReq4BodyMWDecision" : 52190 } )
        line = Hlt2Line('B2D2hhKstarReq3BodyMW', prescale = 0.01 , algos = [ Robust3BodySeq, B2D2hhKstarPhiSeqMWFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq3BodyMWDecision" : 52200 } )
        line = Hlt2Line('B2D2hhKstarReq4BodyMW', prescale = 0.01 , algos = [ Robust4BodySeq, B2D2hhKstarPhiSeqMWFilter], postscale = self.postscale, PV = True)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarReq4BodyMWDecision" : 52210 } )
        


