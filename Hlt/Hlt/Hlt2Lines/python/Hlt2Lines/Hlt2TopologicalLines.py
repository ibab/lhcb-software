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
        # upfront combo cuts
        'AMAXDOCA_MAX'      : 0.2,    # mm        
        'BPVVDCHI2_MIN'     : 100.0,  # unitless
        'MIN_TRCHI2DOF_MAX' : 2.4,    # unitless
        'V2BODYCHI2_MAX'    : 10,     # unitless
        # bdt cuts
        'BDT_2BODY_MIN'     : 0.65,
        'BDT_3BODY_MIN'     : 0.7,
        'BDT_4BODY_MIN'     : 0.2,
        'BDT_2BODYMU_MIN'   : 0.3,
        'BDT_3BODYMU_MIN'   : 0.33,
        'BDT_4BODYMU_MIN'   : 0.09,
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
        'HltANNSvcID' : {'Hlt2Topo2BodyDecision'      : 50736,
                         'Hlt2Topo3BodyDecision'      : 50776,
                         'Hlt2Topo4BodyDecision'      : 50816,
                         'Hlt2TopoMu2BodyDecision'    : 50737,
                         'Hlt2TopoMu3BodyDecision'    : 50777,
                         'Hlt2TopoMu4BodyDecision'    : 50817
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
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
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
        return bindMembers(None,[tracks, Hlt2TopoKillTooManyInTrkAlg])
    
    def __inPartFilter(self, name, inputSeq):
        '''Filters input particles for topo lines.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        props = self.getProps()        
        cuts = '(PT > %s*MeV) & (P > %s*MeV) ' \
               % (props['ALL_PT_MIN'],props['ALL_P_MIN'])
        cuts += '& (MIPCHI2DV(PRIMARY) > %s) & (TRCHI2DOF < %s)' % \
                (props['ALL_MIPCHI2DV_MIN'],props['ALL_TRCHI2DOF_MAX'])
        
        filter = Hlt2Member(FilterDesktop,'Filter', InputLocations=inputSeq,
                            Code=cuts)
        # require PV3D reconstruction before our cut on IP!
        from HltTracking.HltPVs import PV3D
        return bindMembers(name, [PV3D()]+inputSeq+[filter])

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
        file='$PARAMFILESROOT/data/Hlt2Topo%dBody_BDTParams_%s.txt' \
              % (n,props['BDT_%dBODY_PARAMS'%n])
        bdttool = Tool(type=BBDT,name='TrgBBDT',NBody=n,
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
        file='$PARAMFILESROOT/data/Hlt2Topo%dBody_BDTParams_%s.txt' \
              % (n,props['BDT_%dBODY_PARAMS'%n])
        bdttool = Tool(type=BBDT,name='TrgBBDT',NBody=n,
                       Threshold=props['BDT_%dBODYMU_MIN'%n],ParamFile=file)
        cuts = "INTREE(HASPROTO & HASMUON & ISMUON) & FILTER('BBDecTreeTool/TrgBBDT') "
        filter = Hlt2Member(FilterDesktop, 'FilterMuonBDT',
                            InputLocations=input,Code=cuts,tools=[bdttool]) 
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

    def __allNBody(self,n,input):
        '''All n-body combos.'''
        decay = [["K*(892)0 -> K+ K+","K*(892)0 -> K+ K-","K*(892)0 -> K- K-"],
                 ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"],
                 ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]]
        return self.__combine('Topo%dBodyAll'%n,input,decay[n-2])

    def __makeLines(self,name,seqs):
        '''Makes the lines.'''
        for n in [2,3,4]:
            lineName = name.replace('NBody','%dBody' % n)
            self.__makeLine(lineName,algos=[seqs[n-2]])

    def __apply_configuration__(self):
        '''Constructs all of the lines.'''
        from Hlt2SharedParticles.TrackFittedBasicParticles \
             import BiKalmanFittedKaonsWithMuonID
        # input particles
        input = self.__inPartFilter('TopoInputKaons',
                                    [BiKalmanFittedKaonsWithMuonID])

        # make 2-body line
        all2 = self.__allNBody(2,[input])
        topo2_all = self.__filterNforN(2,[all2])
        topo2 = self.__filterBDT(2,[topo2_all])
        topo2_mu = self.__filterMuonBDT(2,[topo2_all]) 
        # make 3-body line
        filt23 = self.__filter2forN(3,[all2])
        all3 = self.__allNBody(3,[input,filt23])
        topo3_all = self.__filterNforN(3,[all3])
        topo3 = self.__filterBDT(3,[topo3_all])
        topo3_mu = self.__filterMuonBDT(3,[topo3_all]) 
        # make 4-body line
        filt24 = self.__filter2forN(4,[filt23])
        filt3 = self.__filter3for4([all3])
        all4 = self.__allNBody(4,[input,filt3])
        topo4_all = self.__filterNforN(4,[all4]) 
        topo4 = self.__filterBDT(4,[topo4_all])
        topo4_mu = self.__filterMuonBDT(4,[topo4_all]) 

        # make the lines
        self.__makeLines('TopoNBody',[topo2,topo3,topo4])
        self.__makeLines('TopoMuNBody',[topo2_mu,topo3_mu,topo4_mu])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
