###
#
# Hlt2 B2DX lines
#
# Author Jacopo Nardulli
#
# Modifications by Chris Thomas
#
###

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2B2DXLinesConf(HltLinesConfigurableUser) :


    # steering variables
    #------------------------
    # Don't touch my variables!

    __slots__ = { ## Global event cut
                  'UseGEC'              : True       # use global event cut?
                  , 'GEC'               : 120        # max number of tracks allowed in GEC
                  ## One-stage track-fit cuts
                  , 'OSTFAllTrkPtLL'       : 500        # track pT (MeV)
                  , 'OSTFAllTrkPLL'        : 5000       # track p (MeV)
                  , 'OSTFAllTrkPVIPChi2LL' : 16         # track-PV IP chi2 (unitless)
                  , 'OSTFAllTrkChi2UL'     : 5          # track fit chi2 (unitless)
                  , 'OSTFTrkMaxPtLL'       : 1500       # track max pT (MeV)
                  , 'OSTFKsDDPtLL'         : 100        # KsDD pT (MeV)
                  , 'OSTFKsDDPLL'          : 500        # KsDD p (MeV)
                  , 'OSTFKsDDPVIPChi2LL'   : 2          # KsDD-PV IP chi2 (unitless)
                  , 'OSTFPtBachelorLL'     : 500        # bachelor pT (MeV)
                  , 'OSTFPairMinDocaUL'    : 0.2        # min DOCA (mm)
                  , 'OSTFPairMaxDocaUL'    : 0.5        # max DOCA (mm)
                  , 'OSTFVtxPVDispChi2LL'  : 225        # B endvertex-PV disp. chi2 (unitless)
                  , 'OSTFVtxPVDDispChi2LL' : 196        # D endvertex-PV disp. chi2 (unitless)
                  , 'OSTFVtxDispUL'        : 20         # B-D signed vertex-vertex disp.
                  ## Mass windows
                  , 'MWSignalMin'          : 5
                  , 'MWSignalMax'          : 5.5
                  , 'MWBroadMin'           : 4
                  , 'MWBroadMax'           : 6
                  ## Pre- and post-scales
                  , 'Prescale'                : { #Broad MW sidebands
                                                 'Hlt2B2D2hhBachelorOSTFTUSBroadMW'          : 0.01
                                                 , 'Hlt2B2D2hhhBachelorOSTFTUSBroadMW'         : 0.01
                                                 , 'Hlt2B2D2hhKstarOSTFTUSBroadMW'             : 0.01
                                                 , 'Hlt2B2D2hhhBachelorWithKsOSTFTUSBroadMW'   : 0.01                                                 
                                                 }
                  }

    ############################################################
    # Function to perform a filter on the input particles after the
    # track-fitting (one-stage track fit)
    ############################################################
    def __OSTFInputParticleFilter(self, name, inputContainers) : # {
        
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
        
        incuts = """(PT> %(OSTFAllTrkPtLL)s *MeV)
        & (P> %(OSTFAllTrkPLL)s *MeV)
        & (MIPCHI2DV(PRIMARY)> %(OSTFAllTrkPVIPChi2LL)s )
        & (TRCHI2DOF< %(OSTFAllTrkChi2UL)s )""" % self.getProps()
        
        filter = Hlt2Member( FilterDesktop
                             , 'InPartFilter'
                             , Inputs = inputContainers
                             , Code = incuts
                             )
        
        ## Require the PV3D reconstruction before our cut on IP.
        return bindMembers(name, [PV3D()] + inputContainers + [filter])

    ############################################################
    # Function to perform a filter on the input particles after the
    # track-fitting (one-stage track fit)
    ############################################################
    def __OSTFTUSInputParticleFilter(self, name, inputContainers) : # {
        
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
        
        incuts = """(PT> %(OSTFAllTrkPtLL)s *MeV)
        & (P> %(OSTFAllTrkPLL)s *MeV)
        & (MIPCHI2DV(PRIMARY)> %(OSTFAllTrkPVIPChi2LL)s )
        & (TRCHI2DOF< %(OSTFAllTrkChi2UL)s )""" % self.getProps()

        filterPrelim = Hlt2Member( FilterDesktop
                                   , 'InPartFilter'
                                   , Inputs = inputContainers
                                   , Code = incuts
                                   )        

        bindPrelim = bindMembers('bindPrelim', [PV3D()] + inputContainers + [filterPrelim])
        
        # TODO: make this an Hlt2Member...
        from Configurables import TisTosParticleTagger
        TUSInputParticlesFilter = TisTosParticleTagger("Hlt2B2DXTUSFilter")
        TUSInputParticlesFilter.TisTosSpecs = { "Hlt1.*Hadron.*Decision%TUS":0 }
        TUSInputParticlesFilter.ProjectTracksToCalo = False
        TUSInputParticlesFilter.CaloClustForCharged = False
        TUSInputParticlesFilter.CaloClustForNeutral = False
        TUSInputParticlesFilter.TOSFrac = { 4:0.0, 5:0.0 }
        TUSInputParticlesFilter.CompositeTPSviaPartialTOSonly = True
        TUSInputParticlesFilter.Inputs = [ bindPrelim.outputSelection() ]
        TUSInputParticlesFilter.Output = 'Hlt2/Hlt2B2DXTUSFilter/Particles'

        return bindMembers(name, [bindPrelim, TUSInputParticlesFilter])

    ############################################################
    # Function to perform a filter on the KS0 input particles after the
    # track-fitting (one-stage track fit)
    ############################################################
    def __OSTFKS0InputParticleFilter(self, name, inputContainers) : # {
        
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
        
        incuts = """(PT> %(OSTFKsDDPtLL)s *MeV)
        & (P> %(OSTFKsDDPLL)s *MeV)
        & (MIPCHI2DV(PRIMARY)> %(OSTFKsDDPVIPChi2LL)s ) """ % self.getProps()
        
        filter = Hlt2Member( FilterDesktop
                             , 'InPartFilter'
                             , Inputs = inputContainers
                             , Code = incuts
                             )
        
        ## Require the PV3D reconstruction before our cut on IP.
        return bindMembers(name, [PV3D()] + inputContainers + [filter])

    ############################################################
    # Function to configure common particle combinations at the
    # post-track-fitting stage
    ###################################################################
    def __OSTFCombine(self, name, inputSeq, decayDesc, extracuts = None) :

        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
        
        # Construct a cut string for the combination.
        combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(OSTFPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()
            
        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body 
        
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']
                
        # Construct a cut string for the vertexed combination.
        parentcuts = """(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(OSTFTrkMaxPtLL)s *MeV)""" % self.getProps()
                
        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']
            
        combineDXNBody = Hlt2Member( CombineParticles
                                     , 'Combine'
                                     , DecayDescriptors = decayDesc
                                     , Inputs = inputSeq
                                     , CombinationCut = combcuts
                                     , MotherCut = parentcuts
                                     )

        #explicitly require the primary vertex as we are paranoid
        return bindMembers(name, inputSeq + [PV3D(), combineDXNBody])

    ###################################################################
    # Function to filter the post-OSTF particle combinations
    ###################################################################
    def __OSTFFilter(self, name, inputSeq, extracode = None) :

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
        from Configurables import TisTosParticleTagger
        
        modules = CoreFactory('CoreFactory').Modules
        
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('UseGEC') : 
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC)s" % self.getProps()
            
        Hlt2KillTooManyDXIPFilter = VoidFilter('Hlt2KillTooManyDXIP'
                                               , Code = filtCode
                                              )
        Hlt2KillTooManyDXIP = bindMembers(None, [tracks, Hlt2KillTooManyDXIPFilter])

        ###################################################################
        # Absorb the post-track fit kaons into this configurable
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons
        B2DXOSTFInputKaons = self.__OSTFInputParticleFilter('B2DXOSTFInputKaons', [ BiKalmanFittedKaons ] )

        ###################################################################
        # Absorb the post-track fit pions into this configurable
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        B2DXOSTFInputPions = self.__OSTFTUSInputParticleFilter('B2DXOSTFInputPions', [ BiKalmanFittedPions ] )

        ###################################################################
        # Create one-stage track fit sequences
        # with the additional requirement that one of the input tracks is TUS with respect to the Hlt1 hadronic triggers
        ###################################################################
        OSTFTUS2Body = self.__OSTFCombine(  name = 'OSTFTUS2Body'
                                            , inputSeq = [ B2DXOSTFInputKaons, B2DXOSTFInputPions ]
                                            , decayDesc = ["K*(892)0 -> K+ pi+", "K*(892)0 -> K+ pi-", "K*(892)0 -> K- pi-"]
                                            , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(OSTFPairMinDocaUL)s )" % self.getProps() }
                                            )

        ###################################################################
        # Create 3-body object
        ###################################################################
        OSTFTUS3Body = self.__OSTFCombine(  name = 'OSTFTUS3Body'
                                            , inputSeq = [ OSTFTUS2Body, B2DXOSTFInputKaons ]
                                            , decayDesc =[ "D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"]
                                            , extracuts = {'MotherCut' : "(BPVVDCHI2> %(OSTFVtxPVDDispChi2LL)s )" % self.getProps()}
                                            )
                
        ###################################################################
        # Filter the 2-body OSTF Hlt1TUS object as a D with a cut on the FDChi2
        ###################################################################
        OSTFTUS2BodySeq = self.__OSTFFilter('OSTFTUS2BodySeq'
                                            , [OSTFTUS2Body]
                                            , extracode = '(BPVVDCHI2> %(OSTFVtxPVDDispChi2LL)s )' % self.getProps()
                                            )
        
        ###################################################################
        # Filter the 2-body OSTF Hlt1TUS object as a K* or a Phi.
        # Just a cut on the Charge is applied, nothing on the FDChi2
        ###################################################################
        OSTFTUS2BodyKstarPhiSeq = self.__OSTFFilter('OSTFTUS2BodyKstarPhiSeq'
                                                    , [OSTFTUS2Body]
                                                    , extracode = '(SUMQ == 0)'
                                                    )
        
        ###################################################################
        # Various B combinations for the OSTF Hlt1TUS objects
        # B -> D(hh)h ; B->D(hhh)h ; B -> D(hh)K*
        ###################################################################
        B2D2hhBachelorOSTFTUSSeq = self.__OSTFCombine(name = 'B2D2hhBachelorOSTFTUSSeq'
                                                      , inputSeq = [ OSTFTUS2BodySeq, B2DXOSTFInputKaons ]
                                                      , decayDesc = ["B0 -> K*(892)0  K-","B0 -> K*(892)0  K+"]
                                                      , extracuts = {'MotherCut' : "(BPVVDCHI2> %(OSTFVtxPVDispChi2LL)s ) & (D2DVVDSIGN(1)< %(OSTFVtxDispUL)s)" % self.getProps(),
                                                                     'K+':"(PT> %(OSTFPtBachelorLL)s *MeV)" % self.getProps()}
                                                      )
        
        B2D2hhhBachelorOSTFTUSSeq = self.__OSTFCombine(name = 'B2D2hhhBachelorOSFTUSSeq'
                                                       , inputSeq = [ OSTFTUS3Body, B2DXOSTFInputKaons ]
                                                       , decayDesc = ["B0 -> D*(2010)+ K-","B0 -> D*(2010)- K+"]
                                                       , extracuts = {'MotherCut' : "(BPVVDCHI2> %(OSTFVtxPVDispChi2LL)s ) & (D2DVVDSIGN(1)< %(OSTFVtxDispUL)s)" \
                                                                      % self.getProps(),
                                                                      'K+':"(PT> %(OSTFPtBachelorLL)s *MeV)" % self.getProps()}
                                                       )
        
        B2D2hhKstarPhiOSTFTUSSeq  = self.__OSTFCombine(name = 'B2D2hhKstarPhiOSTFTUSSeq'
                                                       , inputSeq = [ OSTFTUS2BodyKstarPhiSeq, OSTFTUS2BodySeq ]
                                                       , decayDesc = ["B0 -> K*(892)0 K*(892)0  ","B0 -> K*(892)0 K*(892)0"]
                                                       , extracuts = {'MotherCut' : "(BPVVDCHI2> %(OSTFVtxPVDispChi2LL)s )  & (D2DVVDSIGN(1)< %(OSTFVtxDispUL)s)" \
                                                                      % self.getProps()}
                                                       )

        ###################################################################
        # Absorb the downstream-downstream Kshorts into this configurable
        ###################################################################
        from Hlt2SharedParticles.Ks import KsDD
        B2DXOSTFInputKS0DD = self.__OSTFKS0InputParticleFilter('B2DXOSTFInputKS0DD', [ KsDD ])

        ###################################################################
        # Create post track fit D->Kshh sequence using the OSTF Hlt1TUS particles
        # (the same Ks as for the OSTF lines are used)
        ###################################################################
        OSTFTUS3BodyWithKs = self.__OSTFCombine( name = 'OSTFTUS3BodyWithKs'
                                                 , inputSeq = [ OSTFTUS2Body, B2DXOSTFInputKS0DD ]
                                                 , decayDesc =[ "D*(2010)+ -> K*(892)0 KS0"]
                                                 , extracuts = {'MotherCut' : "(BPVVDCHI2> %(OSTFVtxPVDDispChi2LL)s )" % self.getProps()}
                                                 )

        ###################################################################
        # Create B -> D(Kshh) h combination with OSTF Hlt1TUS particles
        ###################################################################
        B2D2hhhBachelorWithKsOSTFTUSSeq = self.__OSTFCombine(name = 'B2D2hhhBachelorWithKsOSTFTUSSeq'
                                                             , inputSeq = [ OSTFTUS3BodyWithKs, B2DXOSTFInputKaons ]
                                                             , decayDesc = ["B0 -> D*(2010)+ K-","B0 -> D*(2010)- K+"]
                                                             , extracuts = {'MotherCut' : "(BPVVDCHI2> %(OSTFVtxPVDispChi2LL)s )  & (D2DVVDSIGN(1)< %(OSTFVtxDispUL)s)" \
                                                                            % self.getProps(),
                                                                            'K+':"(PT> %(OSTFPtBachelorLL)s *MeV)" % self.getProps()}
                                                             )
        
        ###################################################################
        # Filter everything in signal and broad mass windows
        ###################################################################
        B2D2hhBachelorOSTFTUSSeqSignalFilter = self.__OSTFFilter('B2D2hhBachelorOSTFTUSSeqSignalFilter',
                                                                 [B2D2hhBachelorOSTFTUSSeq],
                                                                 extracode = "(M>%(MWSignalMin)s*GeV) & (M<%(MWSignalMax)s*GeV)" % self.getProps())
        B2D2hhhBachelorOSTFTUSSeqSignalFilter = self.__OSTFFilter('B2D2hhhBachelorOSTFTUSSeqSignalFilter',
                                                                  [B2D2hhhBachelorOSTFTUSSeq],
                                                                  extracode = "(M>%(MWSignalMin)s*GeV) & (M<%(MWSignalMax)s*GeV)" % self.getProps())
        B2D2hhKstarPhiOSTFTUSSeqSignalFilter = self.__OSTFFilter('B2D2hhKstarPhiOSTFTUSSeqSignalFilter',
                                                                 [B2D2hhKstarPhiOSTFTUSSeq],
                                                                 extracode = "(M>%(MWSignalMin)s*GeV) & (M<%(MWSignalMax)s*GeV)" % self.getProps())
        B2D2hhhBachelorWithKsOSTFTUSSeqSignalFilter = self.__OSTFFilter('B2D2hhhBachelorWithKsOSTFTUSSeqSignalFilter',
                                                                        [B2D2hhhBachelorWithKsOSTFTUSSeq],
                                                                        extracode = "(M>%(MWSignalMin)s*GeV) & (M<%(MWSignalMax)s*GeV)" % self.getProps())
        
        B2D2hhBachelorOSTFTUSSeqBroadMWFilter = self.__OSTFFilter('B2D2hhBachelorSeqBroadMWFilter',
                                                                  [B2D2hhBachelorOSTFTUSSeq],
                                                                  extracode = "(M>%(MWBroadMin)s*GeV) & (M<%(MWBroadMax)s*GeV)" % self.getProps())
        B2D2hhhBachelorOSTFTUSSeqBroadMWFilter = self.__OSTFFilter('B2D2hhhBachelorSeqBroadMWFilter',
                                                                   [B2D2hhhBachelorOSTFTUSSeq],
                                                                   extracode = "(M>%(MWBroadMin)s*GeV) & (M<%(MWBroadMax)s*GeV)" % self.getProps())
        B2D2hhKstarPhiOSTFTUSSeqBroadMWFilter = self.__OSTFFilter('B2D2hhKstarPhiSeqBroadMWFilter',
                                                                  [B2D2hhKstarPhiOSTFTUSSeq],
                                                                  extracode = "(M>%(MWBroadMin)s*GeV) & (M<%(MWBroadMax)s*GeV)" % self.getProps())        
        B2D2hhhBachelorWithKsOSTFTUSSeqBroadMWFilter = self.__OSTFFilter('B2D2hhhBachelorWithKsSeqBroadMWFilter',
                                                                         [B2D2hhhBachelorWithKsOSTFTUSSeq],
                                                                         extracode = "(M>%(MWBroadMin)s*GeV) & (M<%(MWBroadMax)s*GeV)" % self.getProps())

        ###################################################################
        # OSTF TUS signal lines
        ###################################################################
        line = Hlt2Line('B2D2hhBachelorOSTFTUSSignal', prescale = self.prescale , algos = [ B2D2hhBachelorOSTFTUSSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorOSTFTUSSignalDecision" : 52100 } )
        line = Hlt2Line('B2D2hhhBachelorOSTFTUSSignal', prescale = self.prescale , algos = [ B2D2hhhBachelorOSTFTUSSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorOSTFTUSSignalDecision" : 52110 } )
        line = Hlt2Line('B2D2hhKstarOSTFTUSSignal', prescale = self.prescale , algos = [ B2D2hhKstarPhiOSTFTUSSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarOSTFTUSSignalDecision" : 52120 } )
        line = Hlt2Line('B2D2hhhBachelorWithKsOSTFTUSSignal', prescale = self.prescale , algos = [ B2D2hhhBachelorWithKsOSTFTUSSeqSignalFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithKsOSTFTUSSignalDecision" : 52130 } )

        ###################################################################
        # OSTF TUS lines, broad mass window
        ###################################################################
        line = Hlt2Line('B2D2hhBachelorOSTFTUSBroadMW', prescale = self.prescale , algos = [ B2D2hhBachelorOSTFTUSSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhBachelorOSTFTUSBroadMWDecision" : 52140 } )
        line = Hlt2Line('B2D2hhhBachelorOSTFTUSBroadMW', prescale = self.prescale , algos = [ B2D2hhhBachelorOSTFTUSSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorOSTFTUSBroadMWDecision" : 52150 } )
        line = Hlt2Line('B2D2hhKstarOSTFTUSBroadMW', prescale = self.prescale , algos = [ B2D2hhKstarPhiOSTFTUSSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhKstarOSTFTUSBroadMWDecision" : 52160 } )
        line = Hlt2Line('B2D2hhhBachelorWithKsOSTFTUSBroadMW', prescale = self.prescale , algos = [ B2D2hhhBachelorWithKsOSTFTUSSeqBroadMWFilter], postscale = self.postscale)
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2D2hhhBachelorWithKsOSTFTUSBroadMWDecision" : 52170 } )
