###
#
# Hlt2 B2DX lines
# Author Jacopo Nardulli
#
###

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2B2DXLinesConf(HltLinesConfigurableUser) :


    # steering variables
    #------------------------
    # Don't touch my variables!

    __slots__ = { 'RobAllTrkPtLL'        : 200          # in MeV
                  , 'RobAllTrkPLL'         : 2000       # in MeV
                  , 'RobAllTrkPVIPLL'      : 0.125      # in mm
                  , 'RobPairMinDocaUL'     : 0.2        # in mm
                  , 'RobPairMaxDocaUL'     : 1          # in mm
                  , 'RobTrkMaxPtLL'        : 1500       # in MeV
                  , 'RobVtxPVDispLL'       : 0.0        # in mm
                  , 'RobVtxPVRDispLL'      : 0.2        # in mm
                  , 'RobUseGEC'            : True       # do or do not 
                  , 'RobGEC'               : 120        # max number of tracks
                  , 'TFAllTrkPtLL'         : 200        # in MeV
                  , 'TFKsDDPtLL'           : 100        # in MeV
                  , 'TFPtBachelorLL'       : 350        # in MeV
                  , 'TFAllTrkPLL'          : 2000       # in MeV
                  , 'TFKsDDPLL'            : 500       # in MeV
                  , 'TFAllTrkPVIPChi2LL'   : 4          # unitless
                  , 'TFKsDDPVIPChi2LL'     : 2          # unitless
                  , 'TFAllTrkChi2UL'       : 10         # unitless
                  , 'TFPairMinDocaUL'      : 0.2        # in mm
                  , 'TFPairMaxDocaUL'      : 1          # in mm
                  , 'TFTrkMaxPtLL'         : 1500       # in MeV
                  , 'TFVtxPVDispChi2LL'    : 225        # unitless
                  , 'TFVtxPVDDispChi2LL'   : 196        # for the D: unitless
                  , 'TFVtxDispUL'          : 20
                  , 'RobCoplanUL'          : 1.0        # in mm
                  , 'Prescale'                : {'Hlt2B2D2hhhBachelorWithKsBroadMW'            : 0.01           # Broad MW sidebands lines
                                                 , 'Hlt2B2D2hhBachelorBroadMW'                 : 0.01
                                                 , 'Hlt2B2D2hhhBachelorBroadMW'                : 0.01
                                                 , 'Hlt2B2D2hhKstarBroadMW'                    : 0.01
                                                 , 'Hlt2B2D2XRobust3BodyMonitor'               : 0.001          # Robust stage monitoring lines
                                                 , 'Hlt2B2D2hhhBachelorWithKsSignalMonitor'    : 0.001          # Post TF stage monitoring lines
                                                 , 'Hlt2B2D2hhBachelorSignalMonitor'           : 0.001
                                                 , 'Hlt2B2D2hhhBachelorSignalMonitor'          : 0.001
                                                 , 'Hlt2B2D2hhKstarSignalMonitor'              : 0.001
                                                 , 'Hlt2B2D2hhhBachelorWithKsBroadMWMonitor'   : 0.001
                                                 , 'Hlt2B2D2hhBachelorBroadMWMonitor'          : 0.001
                                                 , 'Hlt2B2D2hhhBachelorBroadMWMonitor'         : 0.001
                                                 , 'Hlt2B2D2hhKstarBroadMWMonitor'             : 0.001
                                                 }
                  }

    ############################################################
    # Function to configure common particle combinations at the
    # robust stage
    ###################################################################
    def __RobustCombine(self, name, inputSeq, decayDesc, extracuts = None) :

        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
        # Construct a cut string for the combination.
        combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(RobPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()
        
        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body 
        
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']
            
        # Construct a cut string for the vertexed combination.
        parentcuts = """
        (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(RobTrkMaxPtLL)s *MeV)
        & (BPVVD> %(RobVtxPVDispLL)s )
        & (BPVVDR> %(RobVtxPVRDispLL)s )""" % self.getProps()

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
        return bindMembers( name, inputSeq + [ PV3D(), combineDXNBody ] )

    ############################################################
    # Function to perform a filter on the robust-stage combinations
    # previously created
    ############################################################
    def __RobustFilter(self, name, inputSeq, extracode = None) :

        """
        # Function to configure a filter for the robust stages of the
        # B2DX.  It lashes the new FilterDesktop to a bindMembers
        # with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #  produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        
        codestr = "(M>3000*MeV)" 
        if extracode : codestr = codestr + '&' + extracode

        filter = Hlt2Member( FilterDesktop
                             , 'DXRobustFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
                             )
        #explicitly require the primary vertex as we are paranoid
        return bindMembers(name, inputSeq + [ filter ])
    
    ############################################################
    # Function to configure common particle combinations at the
    # post-track-fitting stage
    ###################################################################
    def __TfCombine(self, name, inputSeq, decayDesc, extracuts = None) :

        """
        # Function to configure post-track-fit particle combinations
        #   used by the B2DX lines.  It lashes the new
        #   CombineParticles to a bindMembers with its antecedents.
        # Its arguments:
        #     name      : string name
        #     inputSeq  : list of input particle sequences
        #     decayDesc : list of string decay descriptors
        #     extracuts : dictionary of extra cuts to be applied.
        #                 Can include cuts at the CombinationCut or at
        #                 the MotherCut level.
        #                 e.g. : { 'CombinationCut' : '(AM>4*GeV)'
        #                        , 'MotherCut'      : '(BPVDIRA>0.5)' }
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
        
        # Construct a cut string for the combination.
        combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(TFPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()
            
        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body 
        
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']
                
        # Construct a cut string for the vertexed combination.
        parentcuts = """(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(TFTrkMaxPtLL)s *MeV)""" % self.getProps()
                
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
        return bindMembers( name, inputSeq + [ PV3D(), combineDXNBody ] )
        
    ############################################################
    # Function to perform a filter on the input particles before the
    # robust combinations
    ############################################################
    def __RobInputParticleFilter(self, name, inputSeq) : # {
        """
        # Function to configure a filter for the input particles of the
        #   robust stages of the B2DX.  It lashes the new FilterDesktop
        #   to a bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        daugcuts = """(PT> %(RobAllTrkPtLL)s *MeV)
                      & (P> %(RobAllTrkPLL)s *MeV)
                      & (MIPDV(PRIMARY)> %(RobAllTrkPVIPLL)s )""" % self.getProps()
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputSeq
                            , Code = daugcuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        return bindMembers( name, [ PV3D() ] + inputSeq + [ filter ] )
    
    ############################################################
    # Function to perform a filter on the input particles after the
    # track-fitting
    ############################################################
    def __TfInputParticleFilter(self, name, inputContainers) : # {

        """
        # Function to configure a filter for the input particles of the
        #   post-track-fit stages of the B2DX.  It lashes the new
        #   FilterDesktop to a bindMembers with its antecedents.
        # The argument inputContainer should be 
        #   a list of bindMember sequences that produces the particles
        #   to filter.
        """
        
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
        
        incuts = """(PT> %(TFAllTrkPtLL)s *MeV)
        & (P> %(TFAllTrkPLL)s *MeV)
        & (MIPCHI2DV(PRIMARY)> %(TFAllTrkPVIPChi2LL)s )
        & (TRCHI2DOF< %(TFAllTrkChi2UL)s )""" % self.getProps()
        
        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = inputContainers
                             , Code = incuts
                             )
        
        ## Require the PV3D reconstruction before our cut on IP.
        return bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

    ############################################################
    # Function to perform a filter on the KS0 input particles after the
    # track-fitting
    ############################################################
    def __TfKS0InputParticleFilter(self, name, inputContainers) : # {

        """
        # Function to configure a filter for the KS0 input particles of the
        #   post-track-fit stages of the B2DX.  It lashes the new
        #   FilterDesktop to a bindMembers with its antecedents.
        # The argument inputContainer should be 
        #   a list of bindMember sequences that produces the particles
        #   to filter.
        """
        
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
        
        incuts = """(PT> %(TFKsDDPtLL)s *MeV)
        & (P> %(TFKsDDPLL)s *MeV)
        & (MIPCHI2DV(PRIMARY)> %(TFKsDDPVIPChi2LL)s ) """ % self.getProps()
        
        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = inputContainers
                             , Code = incuts
                             )
        
        ## Require the PV3D reconstruction before our cut on IP.
        return bindMembers( name, [ PV3D()] + inputContainers + [filter ] )
    
    ###################################################################
    # Function to filter the post-Tf particle combinations
    ###################################################################
    def __TFFilter(self, name, inputSeq, extracode = None) :
        """
        # Function to configure a post-track-fit filter for the B2DX
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """

        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        if extracode :
            filter = Hlt2Member( FilterDesktop
                                 , 'TFFilter'
                                 , InputLocations = inputSeq
                                 , Code = extracode
                                 )
        return bindMembers( name, inputSeq + [ filter ] )
        

    def __apply_configuration__(self) :
        ###################################################################
        # Start with a GEC on all events with more than 120 tracks
        # This is a temporary fix only and will be removed once proper
        # GEC lines are implemented
        ###################################################################
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc
        modules =  CoreFactory('CoreFactory').Modules
        
        from HltTracking.Hlt2TrackingConfigurations import Hlt2UnfittedForwardTracking
        tracks = Hlt2UnfittedForwardTracking().hlt2PrepareTracks()

        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('RobUseGEC') : 
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(RobGEC)s" % self.getProps()
            
        Hlt2KillTooManyDXIPFilter = VoidFilter('Hlt2KillTooManyDXIP'
                                               , Code = filtCode
                                              )
        Hlt2KillTooManyDXIP = bindMembers( None, [tracks, Hlt2KillTooManyDXIPFilter])

        ###################################################################
        ## Filter the input particles.
        ###################################################################
        from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
        B2DXRobInputKaons = self.__RobInputParticleFilter('DXInputKaons', [ GoodKaons ])
                
        ###################################################################
        # Create sequences for the shared 2 and 3-body combinatorics of the
        # robust stages.
        ##################################################################

        Robust2Body = self.__RobustCombine(  name = 'Robust2Body'
                                             , inputSeq = [ B2DXRobInputKaons ]
                                             , decayDesc = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
                                             , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(RobPairMinDocaUL)s )" % self.getProps() }
                                             )
        
        Robust3Body = self.__RobustCombine(  name = 'Robust3Body'
                                             , inputSeq = [ B2DXRobInputKaons, Robust2Body ]
                                             , decayDesc = ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"]
                                             , extracuts = {'MotherCut'     : "(BPVDVDOCA()< %(RobCoplanUL)s )" % self.getProps() }
                                             )

        #####################################################
        # filter the 3-body combinations
        #####################################################
        Robust3BodySeq = self.__RobustFilter('Robust3BodySeq', [Robust3Body])
        
        ###################################################################
        # Now absorb the post-track fit shared particles into this configurable
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons,BiKalmanFittedPions
        from Hlt2SharedParticles.Ks import KsDD        
        
        B2DXTFInputKaons = self.__TfInputParticleFilter('B2DXTFInputKaons', [ BiKalmanFittedKaons ] )
        B2DXTFInputPions = self.__TfInputParticleFilter('B2DXTFInputPions', [ BiKalmanFittedPions ] )
        B2DXTFInputKS0DD = self.__TfKS0InputParticleFilter('B2DXTFInputKS0', [ KsDD ])
        
        ################################################################### 
        # Create sequences for the shared combinatorics of the post-track-fit
        # stages: D->hh, D->hshh and D->Kshh
        ###################################################################        
        TF2Body = self.__TfCombine(  name = 'TF2Body'
                                     , inputSeq = [ B2DXTFInputKaons ]
                                     , decayDesc = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
                                     , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(TFPairMinDocaUL)s )" % self.getProps() }
                                     )

        TF3Body = self.__TfCombine(  name = 'TF3Body'
                                     , inputSeq = [ B2DXTFInputPions, TF2Body ]
                                     , decayDesc =[ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
                                     , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(TFVtxPVDDispChi2LL)s )" % self.getProps()})

        TF3BodyWithKs = self.__TfCombine(  name = 'TF3BodyWithKs'
                                           , inputSeq = [ B2DXTFInputKS0DD, TF2Body ]
                                           , decayDesc =[ "D*(2010)+ -> K*(892)0 KS0"]
                                           , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(TFVtxPVDDispChi2LL)s )" % self.getProps()})
        
        ###################################################################
        # Filter the 2-body object as a D with a cut on the FDChi2 
        ###################################################################
        TF2BodySeq = self.__TFFilter('TF2BodySeq'
                                     ,[TF2Body]
                                     , extracode = '(BPVVDCHI2> %(TFVtxPVDDispChi2LL)s )' % self.getProps() )
        
        ###################################################################
        # 2 Body combination for a K* or a Phi.
        # Just a cut on the Charge is applied, nothing on the FDChi2
        ###################################################################
        TF2BodyKstarPhiSeq = self.__TFFilter('TF2BodyKstarPhiSeq'
                                             , [TF2Body]
                                             , extracode = '(SUMQ == 0)')
        
        ###################################################################
        # Various combine particles B combinations :
        # B -> D(hh)h ; B->D(hhh)h ; B -> D(hh)K* ; B -> D(Kshh)h 
        ###################################################################
        B2D2hhBachelorSeq = self.__TfCombine(name = 'B2D2hhBachelorSeq'
                                             , inputSeq = [ B2DXTFInputPions, TF2BodySeq ]
                                             , decayDesc = ["B0 -> K*(892)0  pi-","B0 -> K*(892)0  pi+"]
                                             , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(TFVtxPVDispChi2LL)s ) & (D2DVVDSIGN(1)< %(TFVtxDispUL)s)" % self.getProps(),
                                                            'pi+':"(PT> %(TFPtBachelorLL)s *MeV)" % self.getProps()})
        
        B2D2hhhBachelorSeq = self.__TfCombine(name = 'B2D2hhhBachelorSeq'
                                              , inputSeq = [ B2DXTFInputKaons, TF3Body ]
                                              , decayDesc = ["B0 -> D*(2010)+ K-","B0 -> D*(2010)- K+"]
                                              , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(TFVtxPVDispChi2LL)s ) & (D2DVVDSIGN(1)< %(TFVtxDispUL)s)" % self.getProps(),
                                                             'pi+':"(PT> %(TFPtBachelorLL)s *MeV)" % self.getProps()})

        B2D2hhKstarPhiSeq  = self.__TfCombine(name = 'B2D2hhKstarPhiSeq'
                                              , inputSeq = [ TF2BodyKstarPhiSeq, TF2BodySeq ]
                                              , decayDesc = ["B0 -> K*(892)0 K*(892)0  ","B0 -> K*(892)0 K*(892)0"]
                                              , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(TFVtxPVDispChi2LL)s )  & (D2DVVDSIGN(1)< %(TFVtxDispUL)s)" % self.getProps()})
        
        
        B2D2hhhBachelorWithKsSeq = self.__TfCombine(name = 'B2D2hhhBachelorWithKsSeq'
                                                    , inputSeq = [ B2DXTFInputPions, TF3BodyWithKs]
                                                    , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)- pi+"]
                                                    , extracuts = {'MotherCut'      : "(BPVVDCHI2> %(TFVtxPVDispChi2LL)s )  & (D2DVVDSIGN(1)< %(TFVtxDispUL)s)" % self.getProps(),
                                                                   'pi+':"(PT> %(TFPtBachelorLL)s *MeV)" % self.getProps()})

        ###################################################################
        # Filter everything in signal and broad mass windows
        ###################################################################
        B2D2hhhBachelorWithKsSeqSignalFilter = self.__TFFilter('B2D2hhhBachelorWithKsSeqSignalFilter', [B2D2hhhBachelorWithKsSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhhBachelorWithKsSeqBroadMWFilter = self.__TFFilter('B2D2hhhBachelorWithKsSeqBroadMWFilter', [B2D2hhhBachelorWithKsSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhBachelorSeqSignalFilter = self.__TFFilter('B2D2hhBachelorSeqSignalFilter', [B2D2hhBachelorSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhhBachelorSeqSignalFilter = self.__TFFilter('B2D2hhhBachelorSeqSignalFilter', [B2D2hhhBachelorSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhKstarPhiSeqSignalFilter = self.__TFFilter('B2D2hhKstarPhiSeqSignalFilter', [B2D2hhKstarPhiSeq], extracode = "(M>5.0*GeV) & (M<5.5*GeV)")
        B2D2hhBachelorSeqBroadMWFilter = self.__TFFilter('B2D2hhBachelorSeqBroadMWFilter', [B2D2hhBachelorSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhhBachelorSeqBroadMWFilter = self.__TFFilter('B2D2hhhBachelorSeqBroadMWFilter', [B2D2hhhBachelorSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
        B2D2hhKstarPhiSeqBroadMWFilter = self.__TFFilter('B2D2hhKstarPhiSeqBroadMWFilter', [B2D2hhKstarPhiSeq], extracode = "(M>4*GeV) & (M<6*GeV)")
     
        ###################################################################
        # Make the lines for signal
        ###################################################################
        line = Hlt2Line('B2D2hhhBachelorWithKsSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhhBachelorWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithKsSignalDecision" : 52100 } )
        line = Hlt2Line('B2D2hhBachelorSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhBachelorSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorSignalDecision" : 52110 } )
        line = Hlt2Line('B2D2hhhBachelorSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhhBachelorSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorSignalDecision" : 52120 } )
        line = Hlt2Line('B2D2hhKstarSignal', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhKstarPhiSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarSignalDecision" : 52130 } )

        ###################################################################
        # and for the broader mass window
        ###################################################################
        line = Hlt2Line('B2D2hhhBachelorWithKsBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhhBachelorWithKsSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithKsBroadMWDecision" : 52140 } )
        line = Hlt2Line('B2D2hhBachelorBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhBachelorSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorBroadMWDecision" : 52150 } )
        line = Hlt2Line('B2D2hhhBachelorBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhhBachelorSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorBroadMWDecision" : 52160 } )
        line = Hlt2Line('B2D2hhKstarBroadMW', prescale = self.prescale , algos = [ Robust3BodySeq, B2D2hhKstarPhiSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarBroadMWDecision" : 52170 } )

        ###################################################################
        #Robust monitoring Lines
        ###################################################################
        line = Hlt2Line('B2D2XRobust3BodyMonitor', prescale = self.prescale , algos = [ Robust3BodySeq], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2XRobust3BodyMonitorDecision" : 52180 } )

        ###################################################################
        # Post TF monitoring lines
        ###################################################################
        line = Hlt2Line('B2D2hhhBachelorWithKsSignalMonitor', prescale = self.prescale , algos = [ B2D2hhhBachelorWithKsSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithKsSignalMonitorDecision" : 52190 } )
        line = Hlt2Line('B2D2hhBachelorSignalMonitor', prescale = self.prescale , algos = [ B2D2hhBachelorSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorSignalMonitorDecision" : 52200 } )
        line = Hlt2Line('B2D2hhhBachelorSignalMonitor', prescale = self.prescale , algos = [ B2D2hhhBachelorSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorSignalMonitorDecision" : 52210 } )
        line = Hlt2Line('B2D2hhKstarSignalMonitor', prescale = self.prescale , algos = [ B2D2hhKstarPhiSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarSignalMonitorDecision" : 52220 } )
        line = Hlt2Line('B2D2hhhBachelorWithKsBroadMWMonitor', prescale = self.prescale , algos = [ B2D2hhhBachelorWithKsSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithKsBroadMWMonitorDecision" : 52230 } )
        line = Hlt2Line('B2D2hhBachelorBroadMWMonitor', prescale = self.prescale , algos = [ B2D2hhBachelorSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorBroadMWMonitorDecision" : 52240 } )
        line = Hlt2Line('B2D2hhhBachelorBroadMWMonitor', prescale = self.prescale , algos = [ B2D2hhhBachelorSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorBroadMWMonitorDecision" : 52250 } )
        line = Hlt2Line('B2D2hhKstarBroadMWMonitor', prescale = self.prescale , algos = [ B2D2hhKstarPhiSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarBroadMWMonitorDecision" : 52260 } )




        
