#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Hlt2TopologicalLinesConf(HltLinesConfigurableUser) :
    '''Class to configure Hlt2 topological lines.'''


    #
    # Steering variables:
    # NB: these values can be (and most likely are) overridden by those
    # specified in HltSettings/TopoLines.py (so don't change their names!).
    #
    __slots__ = {
        # single track cuts
        'ALL_PT_MIN'        : 500.0,  # MeV
        'ALL_P_MIN'         : 5000.0, # MeV
        'ALL_MIPCHI2DV_MIN' : 4.0,    # unitless
        'ALL_TRCHI2DOF_MAX' : 4.0,    # unitless
        'KS_MASS_WINDOW'    : 30.0,   # MeV
        'KSPI_MIPCHI2DV_MIN': 16.0,    # unitless
        'KS_BPVVDCHI2_MIN'   : 1000.0,  # unitless 
        'USE_KS'            : True,
        # upfront combo cuts
        'AMAXDOCA_MAX'      : 0.2,    # mm        
        'BPVVDCHI2_MIN'     : 100.0,  # unitless
        'MIN_TRCHI2DOF_MAX' : 2.4,    # unitless
        'V2BODYCHI2_MAX'    : 10,     # unitless
        # bdt cuts
        'BDT_2BODY_MIN'     : 0.65,
        'BDT_3BODY_MIN'     : 0.7,
        'BDT_4BODY_MIN'     : 0.6,
        'BDT_2BODYMU_MIN'   : 0.3,
        'BDT_3BODYMU_MIN'   : 0.33,
        'BDT_4BODYMU_MIN'   : 0.28,
        # bdt param file versions
        'BDT_2BODY_PARAMS'  : 'v1r0',
        'BDT_3BODY_PARAMS'  : 'v1r0',
        'BDT_4BODY_PARAMS'  : 'v1r0',
        # global event cuts
        'USE_GEC'           : False,
        'GEC_MAX'           : 350,
        'HLT1FILTER'        : "",
        # pre- and post-scale values are set in HltSettings/TopoLines.py
        'Prescale' : {},
        'Postscale' : {},
        # HltANNSvc IDs for each line (need to be configurable)
        'HltANNSvcID' : {'Hlt2Topo2BodySimpleDecision' : 50736,
                         'Hlt2Topo3BodySimpleDecision' : 50776,
                         'Hlt2Topo4BodySimpleDecision' : 50816,
                         'Hlt2Topo2BodyBBDTDecision'   : 50737,
                         'Hlt2Topo3BodyBBDTDecision'   : 50777,
                         'Hlt2Topo4BodyBBDTDecision'   : 50817,
                         'Hlt2TopoMu2BodyBBDTDecision' : 50738,
                         'Hlt2TopoMu3BodyBBDTDecision' : 50778,
                         'Hlt2TopoMu4BodyBBDTDecision' : 50818
                         }
        }
    
    def __updateHltANNSvc(self,line):
        '''Updates the HltANNSvc after a new line has been constructed.'''
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        from Configurables import HltANNSvc
        HltANNSvc().Hlt2SelectionID.update({lineName:id})

    def __makeLine(self, lineName, algos):
        '''Constructs a new line and registers it to the HltANNSvc.'''
        # Prepend a filter on the number of tracks
        Hlt2TopoKillTooManyInTrk = self.__seqGEC()
        lclAlgos = [Hlt2TopoKillTooManyInTrk] 
        lclAlgos.extend(algos)
       
        hltfilter = self.getProp("HLT1FILTER")
        if hltfilter == "" : hltfilter = None
     
        from HltLine.HltLine import Hlt2Line
        Hlt2Line(lineName, HLT=hltfilter, prescale=self.prescale,
                 postscale=self.postscale,algos=lclAlgos) 
        self.__updateHltANNSvc(lineName)

    def __seqGEC(self):  
        '''Defines a global event cut (sets upper limit on n_tracks).'''
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        from HltTracking.Hlt2TrackingConfigurations \
             import Hlt2BiKalmanFittedForwardTracking
        modules =  CoreFactory('CoreFactory').Modules
        if 'LoKiTrigger.decorators' not in modules:
            modules.append('LoKiTrigger.decorators')
        
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

        # by default, configure as a pass-all filter with similar code.
        max = ' > -1'
        if self.getProp('USE_GEC'): max = '< ' + self.getProps()['GEC_MAX']
        filtCode = "CONTAINS('"+tracks.outputSelection()+"') " + max
        
        from Configurables import LoKi__VoidFilter as VoidFilter
        Hlt2TopoKillTooManyInTrkAlg = VoidFilter('Hlt2TopoKillTooManyInTrkAlg',
                                                 Code=filtCode)
        from HltLine.HltLine import bindMembers
        from HltTracking.HltPVs import PV3D
        return bindMembers(None,[PV3D(),tracks, Hlt2TopoKillTooManyInTrkAlg])
    
    def __inPartFilter(self, name, inputSeq,forKs=False):
        '''Filters input particles for topo lines.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        props = self.getProps()
        ipChi2 = props['ALL_MIPCHI2DV_MIN']
        if forKs: ipChi2 = props['KSPI_MIPCHI2DV_MIN']
        cuts = '(PT > %s*MeV) & (P > %s*MeV) ' \
               % (props['ALL_PT_MIN'],props['ALL_P_MIN'])
        cuts += '& (MIPCHI2DV(PRIMARY) > %s) & (TRCHI2DOF < %s)' % \
                (ipChi2,props['ALL_TRCHI2DOF_MAX'])
        
        filter = Hlt2Member(FilterDesktop,'Filter', InputLocations=inputSeq,
                            Code=cuts)
        return bindMembers(name, inputSeq+[filter])

    
    def __inPartKsFilter(self, name, inputSeq):
        '''Filters K-shorts for topo lines.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        props = self.getProps()
        cuts = '(PT > %s*MeV) & (P > %s*MeV)' \
               % (props['ALL_PT_MIN'],props['ALL_P_MIN'])
        cuts += '& (MIPCHI2DV(PRIMARY) > %s)' % props['ALL_MIPCHI2DV_MIN']
        cuts += "& (BPVDIRA > 0) & (BPVVDCHI2 > %s)" \
                % props['KS_BPVVDCHI2_MIN']
        filter = Hlt2Member(FilterDesktop ,'KsFilter', InputLocations=inputSeq,
                            Code=cuts)
        return bindMembers(name, inputSeq+[filter])

    def __filterNforN(self,n,input):
        '''Filters n-body combos for n-body line'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        pid = "('K+'==ABSID)"
        minPtSum = 3000        
        if n > 2: minPtSum = 4000
        cuts = '(SUMTREE(PT,%s,0.0) > %d*MeV)' % (pid,minPtSum)
        cuts += '& (MINTREE(%s,TRCHI2DOF) < %s)' \
                % (pid,self.getProps()['MIN_TRCHI2DOF_MAX'])
        filter = Hlt2Member(FilterDesktop, 'FilterNforN',
                            InputLocations=input,Code=cuts)
        return bindMembers('Topo%d' % n, input+[filter])

    def __filterBDT(self,n,input):
        '''Applies the BDT cut.'''
        from Configurables import BBDecTreeTool as BBDT
        from Configurables import FilterDesktop
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltLine.HltLine import Hlt1Tool as Tool
        props = self.getProps()
        file='Hlt2Topo%dBody_BDTParams_%s.txt'%(n,props['BDT_%dBODY_PARAMS'%n])
        bdttool = Tool(type=BBDT,name='TrgBBDT',
                       Threshold=props['BDT_%dBODY_MIN'%n],ParamFile=file)
        cuts = "FILTER('BBDecTreeTool/TrgBBDT')" 
        filter = Hlt2Member(FilterDesktop, 'FilterBDT', InputLocations=input,
                            Code=cuts,tools=[bdttool]) 
        return bindMembers('Topo%d' % n, input+[filter])

    def __filterMuonBDT(self,n,input):
        '''Applies the muon and BDT cuts.'''
        from Configurables import BBDecTreeTool as BBDT
        from Configurables import FilterDesktop
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltLine.HltLine import Hlt1Tool as Tool
        props = self.getProps()
        file='Hlt2Topo%dBody_BDTParams_%s.txt'%(n,props['BDT_%dBODY_PARAMS'%n])
        bdttool = Tool(type=BBDT,name='TrgBBDT',
                       Threshold=props['BDT_%dBODYMU_MIN'%n],ParamFile=file)
        cuts = "INTREE(HASPROTO & HASMUON & ISMUON)"
        cuts += "& FILTER('BBDecTreeTool/TrgBBDT') "
        filter = Hlt2Member(FilterDesktop, 'FilterMuonBDT',
                            InputLocations=input,Code=cuts,tools=[bdttool]) 
        return bindMembers('TopoMu%d' % n, input+[filter])

    def __filterSimple(self,n,input):
        '''Applies easy cuts factored out of BDT.'''
        from Configurables import BBDTSimpleTool as BBDTSimple
        from Configurables import FilterDesktop
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltLine.HltLine import Hlt1Tool as Tool
        simpletool = Tool(type=BBDTSimple,name='TrgSimple',NBody=n)
        cuts = "FILTER('BBDTSimpleTool/TrgSimple') "
        filter = Hlt2Member(FilterDesktop, 'FilterBBDTSimple',
                            InputLocations=input,Code=cuts,tools=[simpletool]) 
        return bindMembers('TopoMu%d' % n, input+[filter])

    def __filter2forN(self,n,input):
        '''Filters 2-body combos for 3- and 4-body lines.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        props = self.getProps()
        m = 6000
        if n == 4: m = 5000
        cuts = '(M < %d*MeV) & (VFASPF(VCHI2) < %s)' \
               % (m,props['V2BODYCHI2_MAX'])
        filter = Hlt2Member(FilterDesktop, 'Filter2forN', InputLocations=input,
                             Code=cuts)
        return bindMembers('Topo%d' % n, input+[filter])

    def __filter3for4(self,input):
        '''Filters 3-body combos for 4-body line.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        cuts = '(M < 6000*MeV)'
        filter = Hlt2Member(FilterDesktop, 'Filter3for4', InputLocations=input,
                            Code=cuts)
        return bindMembers('Topo', input+[filter])
    
    def __combine(self, name, input, decay):
        '''Configures common particle combos used by all topo lines.'''        
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        props = self.getProps() 
        comboCuts = '(AM < 7000*MeV) & (AALLSAMEBPV)' 
        comboCuts += " & (AMAXDOCA('LoKi::DistanceCalculator') < %s)" \
                     % props["AMAXDOCA_MAX"]
        momCuts = "(BPVDIRA > 0) & (BPVVDCHI2 > %s)" % props['BPVVDCHI2_MIN']
        combo = Hlt2Member(CombineParticles, 'Combine',DecayDescriptors=decay,
                           InputLocations=input, CombinationCut=comboCuts,
                           MotherCut=momCuts)
        return bindMembers(name, input+[combo])

    def __combineKS(self,input):
        '''Make K-shorts for topo (filtered later).'''
        from Hlt2SharedParticles.GoodParticles import GoodPions
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        decay = ["K+ -> pi+ pi-", "K- -> pi+ pi-"]
        mKs = 497.648
        dm = float(self.getProps()['KS_MASS_WINDOW'])
        comboCuts = '(AM > %.3f*MeV) & (AM < %.3f*MeV) & (AALLSAMEBPV)' \
                    % (mKs-1.5*dm,mKs+1.5*dm)
        momCuts = '(M > %.3f*MeV) & (M < %.3f*MeV) & (VFASPF(VCHI2)<%s)' \
                  % (mKs-dm,mKs+dm,self.getProps()['V2BODYCHI2_MAX'])
        
        createKs = Hlt2Member(CombineParticles, "KsLL",DecayDescriptors=decay, 
                              CombinationCut=comboCuts,MotherCut = momCuts,
                              InputLocations=input)
        return bindMembers("CreateKsLL", input+[createKs])

    def __makeInput(self,kaons,kshorts):
        '''Puts Ks and KSs into one container w/ same ID.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import GaudiSequencer, FilterDesktop
        inputK = GaudiSequencer("InputK",Members=kaons.members())
        inputKS = GaudiSequencer("InputKS", Members=kshorts.members())
        inputKandKS = GaudiSequencer("InputKandKs",Members=[inputK,inputKS],
                                     ModeOR=True,ShortCircuit=False)
        filter = Hlt2Member(FilterDesktop,'FilterKandKS',
                            InputLocations=[kaons, kshorts], Code='ALL')
        return bindMembers("InputKandKS", [inputKandKS, filter])

    def __allNBody(self,n,input):
        '''All n-body combos.'''
        decay = [["K*(892)0 -> K+ K+","K*(892)0 -> K+ K-","K*(892)0 -> K- K-"],
                 ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"],
                 ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]]
        return self.__combine('Topo%dBodyAll'%n,input,decay[n-2])

    def __TOSFilter(self, name, input):
        '''Require TOS from 1-track HLT1 lines.'''
        from Configurables import TisTosParticleTagger, FilterDesktop
        from HltLine.HltLine import Hlt2Member,bindMembers
        TOSParticleTagger = TisTosParticleTagger(name+"Tagger")
        TOSParticleTagger.TisTosSpecs = {"Hlt1TrackAllL0Decision%TOS":0,
                                         "Hlt1TrackMuonDecision%TOS":0,
                                         "Hlt1TrackPhotonDecision%TOS":0}
        TOSParticleTagger.NoRegex = True
        TOSParticleTagger.ProjectTracksToCalo = False
        TOSParticleTagger.CaloClustForCharged = False
        TOSParticleTagger.CaloClustForNeutral = False
        TOSParticleTagger.TOSFrac = {4:0.0,5:0.0 }
        #TOSParticleTagger.CompositeTPSviaPartialTOSonly = True
        TOSParticleTagger.InputLocations = [input.outputSelection()]
        filter = bindMembers(name+'ParticleFilter',[input,TOSParticleTagger])
        dummy = Hlt2Member(FilterDesktop, name+'DummyFilter',
                           InputLocations=[filter], Code="ALL")
        return bindMembers(name+'FullFilter',[filter,dummy])

    def __makeLines(self,name,seqs):
        '''Makes the lines.'''
        for n in [2,3,4]:
            lineName = name.replace('NBody','%dBody' % n)
            self.__makeLine(lineName,algos=[seqs[n-2]])

    def __apply_configuration__(self):
        '''Constructs all of the lines.'''
        from Hlt2SharedParticles.TrackFittedBasicParticles \
             import BiKalmanFittedKaonsWithMuonID
        from Hlt2SharedParticles.GoodParticles import GoodPions
        # input particles
        inputK = self.__inPartFilter('TopoInputKaons',
                                     [BiKalmanFittedKaonsWithMuonID])
        input = inputK
        if self.getProps()['USE_KS']:
            pi4ks = self.__inPartFilter('TopoInputPions4KS',[GoodPions],True)
            goodKS = self.__combineKS([pi4ks])
            inputKS = self.__inPartKsFilter('TopoInputKS',[goodKS])
            input = self.__makeInput(inputK,inputKS)
        
        # make 2-body line
        all2 = self.__allNBody(2,[input])
        topo2_all = self.__filterNforN(2,[all2])
        topo2_tos = self.__TOSFilter('Topo2BodyTOS',topo2_all)
        topo2_simple = self.__filterSimple(2,[topo2_all])
        topo2_bdt = self.__filterBDT(2,[topo2_all])
        topo2_mu = self.__filterMuonBDT(2,[topo2_all]) 
        # make 3-body line
        filt23 = self.__filter2forN(3,[all2])
        all3 = self.__allNBody(3,[input,filt23])
        topo3_all = self.__filterNforN(3,[all3])
        topo3_tos = self.__TOSFilter('Topo3BodyTOS',topo3_all)
        topo3_simple = self.__filterSimple(3,[topo3_all])
        topo3_bdt = self.__filterBDT(3,[topo3_all])
        topo3_mu = self.__filterMuonBDT(3,[topo3_all]) 
        # make 4-body line
        filt24 = self.__filter2forN(4,[filt23])
        filt3 = self.__filter3for4([all3])
        all4 = self.__allNBody(4,[input,filt3])
        topo4_all = self.__filterNforN(4,[all4])
        topo4_tos = self.__TOSFilter('Topo4BodyTOS',topo4_all)
        topo4_simple = self.__filterSimple(4,[topo4_all])
        topo4_bdt = self.__filterBDT(4,[topo4_all])
        topo4_mu = self.__filterMuonBDT(4,[topo4_all]) 

        # make the lines
        self.__makeLines('TopoNBodySimple',
                         [topo2_simple,topo3_simple,topo4_simple])
        self.__makeLines('TopoNBodyBBDT',[topo2_bdt,topo3_bdt,topo4_bdt])
        self.__makeLines('TopoMuNBodyBBDT',[topo2_mu,topo3_mu,topo4_mu])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
