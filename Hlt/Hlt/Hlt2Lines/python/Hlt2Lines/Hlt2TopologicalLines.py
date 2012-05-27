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
        'MU_TRCHI2DOF_MAX'  : 4.0,    # unitless
        'E_TRCHI2DOF_MAX'   : 5.0,    # unitless
        'HAD_TRCHI2DOF_MAX' : 3.0,    # unitless
        # V0's
        'USE_KS'            : True,
        'USE_LAMBDA'        : True,
        # upfront combo cuts
        'AMAXDOCA_MAX'      : 0.2,    # mm        
        'BPVVDCHI2_MIN'     : 100.0,  # unitless
        'MIN_TRCHI2DOF_MAX' : 2,      # unitless
        'ONETRACK_PT_MIN'   : 1700.0, # MeV
        'MU_ONETRACK_PT_MIN': 1200.0, # MeV
        'ONETRACK_IPCHI2_MIN' : 36,   # unitless
        'V2BODYCHI2_MAX'    : 10,     # unitless
        'NV0_2Body_MAX'     : 2,
        'NV0_3Body_MAX'     : 2,
        'NV0_4Body_MAX'     : 2,
        'MIN_V0_LT'         : '10*ps',
        # bdt cuts
        'BDT_2BODY_MIN'     : 0.4,
        'BDT_3BODY_MIN'     : 0.4,
        'BDT_4BODY_MIN'     : 0.3,
        'BDT_2BODYMU_MIN'   : 0.1,
        'BDT_3BODYMU_MIN'   : 0.1,
        'BDT_4BODYMU_MIN'   : 0.1,
        'BDT_2BODYE_MIN'    : 0.1,
        'BDT_3BODYE_MIN'    : 0.1,
        'BDT_4BODYE_MIN'    : 0.1,
        'BDT_RAD2_MIN'      : 0.1,
        'BDT_RAD2p1_MIN'    : 0.1,
        # bdt param file versions
        'BDT_2BODY_PARAMS'  : 'v1r0',
        'BDT_3BODY_PARAMS'  : 'v1r0',
        'BDT_4BODY_PARAMS'  : 'v1r0',
        # electron filters & cuts
        'PIDE_MIN'          : -2.0,
        'L0_ELECTRON_FILTER': "L0_CHANNEL('Electron')",
        'HLT1_ELECTRON_FILT':"HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
        # radiative filters
        'L0_RAD_FILTER'     : "L0_CHANNEL('Electron') | L0_CHANNEL('Photon')",
        'RAD_TOS'           : True,
        # global event cuts
        'USE_GEC'           : False,
        'GEC_MAX'           : 350,
        # simple cuts
        'SIMPLE_2BODY_CUTS' : [],
        'SIMPLE_3BODY_CUTS' : [],
        'SIMPLE_4BODY_CUTS' : [],
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
                         'Hlt2TopoMu4BodyBBDTDecision' : 50818,
                         'Hlt2TopoE2BodyBBDTDecision'  : 50739,
                         'Hlt2TopoE3BodyBBDTDecision'  : 50779,
                         'Hlt2TopoE4BodyBBDTDecision'  : 50819,
                         'Hlt2TopoRad2BodyBBDTDecision': 50781,
                         'Hlt2TopoRad2plus1BodyBBDTDecision': 50782
                         }
        }
    
    def __updateHltANNSvc(self,line):
        '''Updates the HltANNSvc after a new line has been constructed.'''
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        from Configurables import HltANNSvc
        HltANNSvc().Hlt2SelectionID.update({lineName:id})

    def __makeLine(self, lineName, algos, electron=False,photon=False):
        '''Constructs a new line and registers it to the HltANNSvc.'''
        from HltLine.HltDecodeRaw import DecodeL0CALO
        Hlt2TopoKillTooManyInTrk = self.__seqGEC()
        lclAlgos = [Hlt2TopoKillTooManyInTrk]
        if photon: lclAlgos.extend([DecodeL0CALO])
        lclAlgos.extend(algos)
        hlt=None
        l0=None
        if electron:
            l0 = self.getProps()['L0_ELECTRON_FILTER']
            hlt = self.getProps()['HLT1_ELECTRON_FILT']
        if photon and self.getProps()['L0_RAD_FILTER']:
            l0 = self.getProps()['L0_RAD_FILTER']
            
        from HltLine.HltLine import Hlt2Line
        Hlt2Line(lineName, prescale=self.prescale,
                 postscale=self.postscale,algos=lclAlgos,L0DU=l0,HLT=hlt) 
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
        if self.getProp('USE_GEC'): max = '< '+str(self.getProps()['GEC_MAX'])
        filtCode = "CONTAINS('"+tracks.outputSelection()+"') " + max
        
        from Configurables import LoKi__VoidFilter as VoidFilter
        Hlt2TopoKillTooManyInTrkAlg = VoidFilter('Hlt2TopoKillTooManyInTrkAlg',
                                                 Code=filtCode)
        from HltLine.HltLine import bindMembers
        from HltTracking.HltPVs import PV3D
        return bindMembers(None,[PV3D(),tracks, Hlt2TopoKillTooManyInTrkAlg])
    
    def __inPartFilter(self, name, inputSeq,forV0=False):
        '''Filters input particles for topo lines.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        props = self.getProps()
        cuts = '(PT > %s*MeV) & (P > %s*MeV) ' \
               % (props['ALL_PT_MIN'],props['ALL_P_MIN'])
        if forV0:
            cuts += '& (TRCHI2DOF < %s) & (MIPCHI2DV(PRIMARY) > %s)' % \
                    (props['HAD_TRCHI2DOF_MAX'],props['V0DT_MIPCHI2DV_MIN'])
        else:
            cuts += '& (MIPCHI2DV(PRIMARY) > %s)' % props['ALL_MIPCHI2DV_MIN']
            cuts += '&(((TRCHI2DOF < %s)& ISMUON)|(TRCHI2DOF < %s))' \
                    % (props['MU_TRCHI2DOF_MAX'],props['HAD_TRCHI2DOF_MAX'])
        
        filter = Hlt2Member(FilterDesktop,'Filter', Inputs=inputSeq,
                            Code=cuts)
        return bindMembers(name, inputSeq+[filter])

    def __inPartEFilter(self, name, inputSeq):
        '''Filters input particles for e topo lines.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        props = self.getProps()
        cuts = '(PT > %s*MeV) & (P > %s*MeV) ' \
               % (props['ALL_PT_MIN'],props['ALL_P_MIN'])
        cuts += '& (MIPCHI2DV(PRIMARY) > %s)' % props['ALL_MIPCHI2DV_MIN']
        cuts += '&(((TRCHI2DOF < %s)&(PIDe > %s))|(TRCHI2DOF < %s))' \
                % (props['E_TRCHI2DOF_MAX'],props['PIDE_MIN'],
                   props['HAD_TRCHI2DOF_MAX'])
        filter = Hlt2Member(FilterDesktop,'Filter', Inputs=inputSeq,
                            Code=cuts)
        return bindMembers(name, inputSeq+[filter])
    
    def __inPartV0Filter(self, name, inputSeq):
        '''Filters K-shorts and Lambdas for topo lines.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        props = self.getProps()
        cuts = '(PT > %s*MeV) & (P > %s*MeV)' \
               % (props['ALL_PT_MIN'],props['ALL_P_MIN'])
        cuts += '& (MIPCHI2DV(PRIMARY) > %s)' % props['ALL_MIPCHI2DV_MIN']
        cuts += "& (BPVDIRA > 0) & (BPVVDCHI2 > %s)" \
                % props[name+'_BPVVDCHI2_MIN']
        filter = Hlt2Member(FilterDesktop ,'TopoInput'+name+'Filter',
                            Inputs=inputSeq,Code=cuts)
        return bindMembers('TopoInput'+name+'V0Filter', inputSeq+[filter])

    def __filterNforN(self,n,input,tag):
        '''Filters n-body combos for n-body line'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        pid = "(('K+'==ABSID)|('KS0'==ID)|('Lambda0'==ABSID))"
        minPtSum = 3000        
        if n > 2: minPtSum = 4000
        cuts = '(SUMTREE(PT,%s,0.0) > %d*MeV)' % (pid,minPtSum)
        cuts += " & (NINTREE(('KS0'==ID)|('Lambda0'==ABSID)) <= %s)" \
                % self.getProps()['NV0_%dBody_MAX'%n]
        cuts += "&(INTREE(ISBASIC & (MIPCHI2DV(PRIMARY)>%s) "\
                " & ((PT > %s*MeV) | (ISMUON & (PT > %s*MeV)))))"\
                %  (self.getProps()['ONETRACK_IPCHI2_MIN'],
                    self.getProps()['ONETRACK_PT_MIN'],
                    self.getProps()['MU_ONETRACK_PT_MIN'])
        cuts += '& (MINTREE(HASTRACK & %s,TRCHI2DOF) < %s)' \
                % (pid,self.getProps()['MIN_TRCHI2DOF_MAX'])
        filter = Hlt2Member(FilterDesktop, 'FilterNforN',
                            Inputs=input,Code=cuts)
        return bindMembers('Topo%d%s' % (n,tag), input+[filter])

    def __filterBDT(self,n,input):
        '''Applies the BDT cut.'''
        from Configurables import BBDecTreeTool as BBDT
        from Configurables import FilterDesktop
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltLine.HltLine import Hlt1Tool as Tool
        props = self.getProps()
        file='Hlt2Topo%dBody_BDTParams_%s.txt'%(n,props['BDT_%dBODY_PARAMS'%n])
        bdttool = Tool(type=BBDT,name='TrgBBDT',
                       Threshold=props['BDT_%dBODY_MIN'%n],ParamFile=file,
                       ANNSvcKey=6300+n,PIDs=['K+','KS0','Lambda0','Lambda~0'])
        cuts = "FILTER('BBDecTreeTool/TrgBBDT')" 
        filter = Hlt2Member(FilterDesktop, 'FilterBDT', Inputs=input,
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
                       Threshold=props['BDT_%dBODYMU_MIN'%n],ParamFile=file,
                       ANNSvcKey=6300+n,PIDs=['K+','KS0','Lambda0','Lambda~0'])
        cuts = "INTREE(HASPROTO & HASMUON & ISMUON)"
        cuts += "& FILTER('BBDecTreeTool/TrgBBDT') "
        filter = Hlt2Member(FilterDesktop, 'FilterMuonBDT',
                            Inputs=input,Code=cuts,tools=[bdttool]) 
        return bindMembers('TopoMu%d' % n, input+[filter])
    
    def __filterElectronBDT(self,n,input):
        '''Applies the e and BDT cuts.'''
        from Configurables import BBDecTreeTool as BBDT
        from Configurables import FilterDesktop
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltLine.HltLine import Hlt1Tool as Tool
        props = self.getProps()
        file='Hlt2Topo%dBody_BDTParams_%s.txt'%(n,props['BDT_%dBODY_PARAMS'%n])
        bdttool = Tool(type=BBDT,name='TrgBBDT',
                       Threshold=props['BDT_%dBODYE_MIN'%n],ParamFile=file,
                       ANNSvcKey=6300+n,PIDs=['K+','KS0','Lambda0','Lambda~0'])
        cuts = "INTREE(HASPROTO & (PIDe > %s))" % props['PIDE_MIN']
        cuts += "& FILTER('BBDecTreeTool/TrgBBDT') "
        filter = Hlt2Member(FilterDesktop, 'FilterElectronBDT',
                            Inputs=input,Code=cuts,tools=[bdttool]) 
        return bindMembers('TopoE%d' % n, input+[filter])

    def __filterRadBDT(self,n,input):
        '''Applies the BDT cuts for radiative candidates.'''
        from Configurables import BBDecTreeTool as BBDT
        from Configurables import FilterDesktop
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltLine.HltLine import Hlt1Tool as Tool
        props = self.getProps()
        ver = props['BDT_%dBODY_PARAMS'%n]
        file='Hlt2Topo%dBody_BDTParams_%s.txt' % (n,ver)
        pids = ['K+','KS0','Lambda0','Lambda~0']
        if n == 3: pids.append('gamma')
        thresh = 1.0
        if n == 2: thresh = props['BDT_RAD2_MIN']
        else: thresh = props['BDT_RAD2p1_MIN']
        bdttool = Tool(type=BBDT,name='TrgBBDT',Threshold=thresh,
                       ParamFile=file,ANNSvcKey=6300+3,PIDs=pids)
        cuts = "FILTER('BBDecTreeTool/TrgBBDT')"
        filter = Hlt2Member(FilterDesktop, 'FilterRad%dBDT'%n,
                            Inputs=input,Code=cuts,tools=[bdttool]) 
        return bindMembers('TopoRad%d'%n, input+[filter])

    def __filterSimple(self,n,input):
        '''Applies easy cuts factored out of BDT.'''
        from Configurables import BBDTSimpleTool as BBDTSimple
        from Configurables import FilterDesktop
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltLine.HltLine import Hlt1Tool as Tool
        simpletool = Tool(type=BBDTSimple,name='TrgSimple',
                          Cuts=self.getProps()['SIMPLE_%dBODY_CUTS'%n])
        cuts = "FILTER('BBDTSimpleTool/TrgSimple') "
        filter = Hlt2Member(FilterDesktop, 'FilterBBDTSimple',
                            Inputs=input,Code=cuts,tools=[simpletool]) 
        return bindMembers('Topo%d' % n, input+[filter])

    def __filter2forN(self,n,input,tag):
        '''Filters 2-body combos for 3- and 4-body lines.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        props = self.getProps()
        m = 6000
        if n == 4: m = 5000
        cuts = '(M < %d*MeV) & (VFASPF(VCHI2) < %s)' \
               % (m,props['V2BODYCHI2_MAX'])
        filter = Hlt2Member(FilterDesktop, 'Filter2forN', Inputs=input,
                             Code=cuts)
        return bindMembers('Topo%d%s' % (n,tag), input+[filter])

    def __filter3for4(self,input,tag):
        '''Filters 3-body combos for 4-body line.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        cuts = '(M < 6000*MeV)'
        filter = Hlt2Member(FilterDesktop, 'Filter3for4', Inputs=input,
                            Code=cuts)
        return bindMembers('Topo'+tag, input+[filter])
    
    def __combine(self, name, input, decay, useComboCuts=True):
        '''Configures common particle combos used by all topo lines.'''        
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        props = self.getProps()
        comboCuts = 'AM > 0'
        if useComboCuts:
            comboCuts = "(AM < 7000*MeV) & ((ANUM((ID=='KS0')|(ABSID=='Lambda0'))>0) | "
            comboCuts += '((AALLSAMEBPV |(AMINCHILD(MIPCHI2DV(PRIMARY)) > 16))'
            comboCuts += " & (AMAXDOCA('LoKi::DistanceCalculator') < %s)))" \
                         % props["AMAXDOCA_MAX"]
        momCuts = "(BPVDIRA > 0) & (BPVVDCHI2 > %s)" % props['BPVVDCHI2_MIN']
        combo = Hlt2Member(CombineParticles, 'Combine',DecayDescriptors=decay,
                           Inputs=input, CombinationCut=comboCuts,
                           MotherCut=momCuts)
        return bindMembers(name, input+[combo])

    def __makeInput(self,kaons,useKS,useL):
        '''Puts Ks, KSs and Lambdas into one container w/ same ID.'''
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import GaudiSequencer, FilterDesktop
        from Hlt2SharedParticles.Ks import KsLLTF, KsDD
        from Hlt2SharedParticles.Lambda import LambdaLLTrackFitted as LLL
        from Hlt2SharedParticles.Lambda import LambdaDDTrackFitted as LDD
        inputK = GaudiSequencer("TopoInputK",Members=kaons.members())
        inputs = [inputK]
        finputs = [kaons]
        if useKS:
            ksll = GaudiSequencer("TopoInputKsLL", Members=KsLLTF.members())
            inputs.append(ksll)
            finputs.append(KsLLTF)
            ksdd = GaudiSequencer("TopoInputKsDD", Members=KsDD.members())
            inputs.append(ksdd)
            finputs.append(KsDD)
        if useL:
            lll = GaudiSequencer("TopoInputLambdaLL", Members=LLL.members())
            inputs.append(lll)
            finputs.append(LLL)
            ldd = GaudiSequencer("TopoInputLDD", Members=LDD.members())
            inputs.append(ldd)
            finputs.append(LDD)
        inputAll = GaudiSequencer("TopoInputALLSeq",Members=inputs,ModeOR=True,
                                  ShortCircuit=False)
        cuts = "((ABSID=='K+')|(BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %s))" % self.getProps()['MIN_V0_LT']
        filter = Hlt2Member(FilterDesktop,'TopoFilterAllInput',Inputs=finputs,
                            Code=cuts)
        return bindMembers("TopoInputALL", [inputAll, filter])

    def __allNBody(self,n,input,tag):
        '''All n-body combos.'''
        decay = [["K*(892)0 -> K+ K+","K*(892)0 -> K+ K-","K*(892)0 -> K- K-",
                  "K*(892)0 -> K+ KS0","K*(892)0 -> K- KS0",
                  "K*(892)0 -> K+ Lambda0","K*(892)0 -> K- Lambda0",
                  "K*(892)0 -> K+ Lambda~0","K*(892)0 -> K- Lambda~0",
                  "K*(892)0 -> KS0 KS0"],
                 ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-",
                  "D*(2010)+ -> K*(892)0 KS0", "D*(2010)+ -> K*(892)0 KS0",
                  "D*(2010)+ -> K*(892)0 Lambda0",
                  "D*(2010)+ -> K*(892)0 Lambda~0"],
                 ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+",
                  "B0 -> D*(2010)+ KS0",
                  "B0 -> D*(2010)+ Lambda0","B0 -> D*(2010)+ Lambda~0"]
                 ]
        return self.__combine('Topo%dBodyComb%s'%(n,tag),input,decay[n-2])

    def __TOSFilter(self, name, input):
        '''Require TOS from 1-track HLT1 lines.'''
        from Configurables import TisTosParticleTagger, FilterDesktop
        from HltLine.HltLine import Hlt2Member,bindMembers
        TOSParticleTagger = TisTosParticleTagger(name+"Tagger")
        TOSParticleTagger.TisTosSpecs = {"Hlt1TrackAllL0Decision%TOS":0,
                                         "Hlt1TrackAllL0TightDecision%TOS":0,
                                         "Hlt1TrackMuonDecision%TOS":0,
                                         "Hlt1TrackPhotonDecision%TOS":0}
        TOSParticleTagger.NoRegex = True
        TOSParticleTagger.ProjectTracksToCalo = False
        TOSParticleTagger.CaloClustForCharged = False
        TOSParticleTagger.CaloClustForNeutral = False
        TOSParticleTagger.TOSFrac = {4:0.0,5:0.0 }
        TOSParticleTagger.Inputs = [input.outputSelection()]
        TOSParticleTagger.Output = 'Hlt2/%s/Particles'%(name+'Tagger')
        filter = bindMembers(name+'ParticleFilter',[input,TOSParticleTagger])
        return filter

    def __makeLines(self,name,seqs,electron=False):
        '''Makes the lines.'''
        for n in [2,3,4]:
            lineName = name.replace('NBody','%dBody' % n)
            self.__makeLine(lineName,[seqs[n-2]],electron)

    def __apply_configuration__(self):
        '''Constructs all of the lines.'''
        from Hlt2SharedParticles.TrackFittedBasicParticles \
             import BiKalmanFittedKaonsWithEID,BiKalmanFittedKaonsWithMuonID
        from Hlt2SharedParticles.GoodParticles import GoodPions
        from Hlt2SharedParticles.TrackFittedBasicParticles \
             import BiKalmanFittedPhotonsFromL0 as Photons
        from Hlt2SharedParticles.TrackFittedBasicParticles \
             import BiKalmanFittedProtons as Protons
        
        # input particles
        inputKe = self.__inPartEFilter('TopoInputKaonsE',
                                       [BiKalmanFittedKaonsWithEID])
        inputK = self.__inPartFilter('TopoInputKaons',
                                     [BiKalmanFittedKaonsWithMuonID])
        input = inputK
        inpute = inputKe
        if self.getProps()['USE_KS'] or self.getProps()['USE_LAMBDA']:
            input = self.__makeInput(inputK,self.getProps()['USE_KS'],
                                     self.getProps()['USE_LAMBDA'])
        
        # make 2-body line
        all2 = self.__allNBody(2,[input],'All')
        topo2_all = self.__filterNforN(2,[all2],'All')
        topo2_tos = self.__TOSFilter('Topo2BodyTOS',topo2_all)
        topo2_simple = self.__filterSimple(2,[topo2_tos])
        topo2_bdt = self.__filterBDT(2,[topo2_tos])
        topo2_mu = self.__filterMuonBDT(2,[topo2_tos])
        all2e = self.__allNBody(2,[inpute],'E')
        topo2_alle = self.__filterNforN(2,[all2e],'E')
        topo2_tose = self.__TOSFilter('Topo2BodyTOSE',topo2_alle)
        topo2_e = self.__filterElectronBDT(2,[topo2_tose]) 
        # make 3-body line
        filt23 = self.__filter2forN(3,[all2],'All')
        all3 = self.__allNBody(3,[input,filt23],'All')
        topo3_all = self.__filterNforN(3,[all3],'All')
        topo3_tos = self.__TOSFilter('Topo3BodyTOS',topo3_all)
        topo3_simple = self.__filterSimple(3,[topo3_tos])
        topo3_bdt = self.__filterBDT(3,[topo3_tos])
        topo3_mu = self.__filterMuonBDT(3,[topo3_tos])
        filt23e = self.__filter2forN(3,[all2e],'E')
        all3e = self.__allNBody(3,[inpute,filt23e],'E')
        topo3_alle = self.__filterNforN(3,[all3e],'E')
        topo3_tose = self.__TOSFilter('Topo3BodyTOSE',topo3_alle)
        topo3_e = self.__filterElectronBDT(3,[topo3_tose]) 
        # make 4-body line
        filt24 = self.__filter2forN(4,[filt23],'All')
        filt3 = self.__filter3for4([all3],'All')
        all4 = self.__allNBody(4,[input,filt3],'All')
        topo4_all = self.__filterNforN(4,[all4],'All')
        topo4_tos = self.__TOSFilter('Topo4BodyTOS',topo4_all)
        topo4_simple = self.__filterSimple(4,[topo4_tos])
        topo4_bdt = self.__filterBDT(4,[topo4_tos])
        topo4_mu = self.__filterMuonBDT(4,[topo4_tos])
        filt24e = self.__filter2forN(4,[filt23e],'E')
        filt3e = self.__filter3for4([all3e],'E')
        all4e = self.__allNBody(4,[inpute,filt3e],'E')
        topo4_alle = self.__filterNforN(4,[all4e],'E')
        topo4_tose = self.__TOSFilter('Topo4BodyTOSE',topo4_alle)
        topo4_e = self.__filterElectronBDT(4,[topo4_tose])
        # make radiative line
        rad2 = self.__filterRadBDT(2,[all2])        
        rad3_all = self.__combine('TopoRad2+1Body',[filt23,Photons],
                                  ['B+ -> K*(892)0  gamma'],False)        
        rad3 = self.__filterRadBDT(3,[rad3_all])
        if self.getProps()['RAD_TOS']:
            rad2 = self.__TOSFilter('TopoRad2TOS',rad2)
            rad3 = self.__TOSFilter('TopoRad2plus1TOS',rad3)
        # make the lines
        self.__makeLines('TopoNBodySimple',
                         [topo2_simple,topo3_simple,topo4_simple])
        self.__makeLines('TopoNBodyBBDT',[topo2_bdt,topo3_bdt,topo4_bdt])
        self.__makeLines('TopoMuNBodyBBDT',[topo2_mu,topo3_mu,topo4_mu])
        self.__makeLines('TopoENBodyBBDT',[topo2_e,topo3_e,topo4_e],True)
        self.__makeLine('TopoRad2BodyBBDT',[rad2],False,True)
        self.__makeLine('TopoRad2plus1BodyBBDT',[rad3],False,True)

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
