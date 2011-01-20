#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from Gaudi.Configuration import *
from Configurables import HltANNSvc, FilterDesktop, CombineParticles
from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
from HltTracking.HltPVs import PV3D

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Hlt2TopologicalLinesConf(HltLinesConfigurableUser) :
    '''Class to configure Hlt2 topological lines.'''

    # Steering variables:
    #
    # NB: these values can be (and most likely are) overridden by those
    # specified in HltSettings/TopoLines.py (so don't change their names!).
    __slots__ = {        
        # global cuts
        'MCOR_MAX'          : 7000.0, # MeV
        'MCOR_MIN'          : 4000.0, # MeV
        'SUM_PT_MIN'        : 4000.0, # MeV
        'MAX_PT_MIN'        : 1500.0, # MeV
        'ALL_PT_MIN'        : 500.0,  # MeV
        'ALL_P_MIN'         : 5000.0, # MeV
        'AMAXDOCA_MAX'      : 0.12,   # mm
        'AMINDOCA_MAX'      : 0.12,   # mm
        'USE_GEC'           : False,
        'GEC_MAX'           : 350,
        'HLT1FILTER'        : "",
        'M_CHARM_VETO'      : 2500,   # MeV
        'SUM_IPCHI2_MIN'    : 100,
        # fit cuts
        'ALL_MIPCHI2DV_MIN' : 16.0,    # unitless
        'ALL_TRCHI2DOF_MAX' : 5.0,   # unitless
        'BPVVDCHI2_MIN'     : 64.0,   # unitless
        'MIN_TRCHI2DOF_MAX' : 3,
        # robust
        'ALL_MIPDV_MIN'     : 0.025,  # mm
        'BPVVD_MIN'         : 2.0,    # mm
        'BPVVDR_MIN'        : 0.2,    # mm
        # pre- and post-scale values are set in HltSettings/TopoLines.py
        'Prescale' : {},
        'Postscale' : {},
        # HltANNSvc IDs for each line (need to be configurable)
        'HltANNSvcID' : {'Hlt2Topo2BodySADecision'        : 50700,
                         'Hlt2Topo3BodySADecision'        : 50710,
                         'Hlt2Topo4BodySADecision'        : 50720,
                         'Hlt2TopoTF2BodySADecision'      : 50730,
                         'Hlt2TopoTF3BodySADecision'      : 50770,
                         'Hlt2TopoTF4BodySADecision'      : 50810,
                         'Hlt2TopoTF2BodyReq2YesDecision' : 50740,
                         'Hlt2TopoTF2BodyReq3YesDecision' : 50750,
                         'Hlt2TopoTF2BodyReq4YesDecision' : 50760,
                         'Hlt2TopoTF3BodyReq2YesDecision' : 50780,
                         'Hlt2TopoTF3BodyReq3YesDecision' : 50790,
                         'Hlt2TopoTF3BodyReq4YesDecision' : 50800,
                         'Hlt2TopoTF4BodyReq2YesDecision' : 50820,
                         'Hlt2TopoTF4BodyReq3YesDecision' : 50830,
                         'Hlt2TopoTF4BodyReq4YesDecision' : 50840,
                         'Hlt2TopoOSTF2BodyDecision'      : 50733,
                         'Hlt2TopoOSTF3BodyDecision'      : 50773,
                         'Hlt2TopoOSTF4BodyDecision'      : 50813,
                         'Hlt2TopoRobTF2BodyDecision'     : 50735,
                         'Hlt2TopoRobTF3BodyDecision'     : 50775,
                         'Hlt2TopoRobTF4BodyDecision'     : 50815
                         }
        }

    def __updateHltANNSvc(self,line):
        '''Updates the HltANNSvc after a new line has been constructed.'''
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update({lineName:id})

    def __makeLine(self, lineName, algos):
        '''Constructs a new line and registers it to the HltANNSvc.'''
        # Prepend a filter on the number of tracks
        Hlt2TopoKillTooManyInTrk = self.__seqGEC()
        lclAlgos = [Hlt2TopoKillTooManyInTrk] 
        lclAlgos.extend(algos)
       
        hltfilter = self.getProp("HLT1FILTER")
        if hltfilter == "" : hltfilter = None
     
        Hlt2Line(lineName, HLT=hltfilter, prescale=self.prescale,
                 postscale=self.postscale,algos=lclAlgos) 
        self.__updateHltANNSvc(lineName)
        
    def __combine(self, name, stage, inputSeq, decayDesc, extraCuts=None):
        '''Configures common particle combos used by all topo lines.'''        
        props = self.getProps() 
        # combo cuts
        comboCuts = '(AM < %s*MeV)' % props['MCOR_MAX']
        comboCuts += "& (AALLSAMEBPV) & " \
                     "(AMAXDOCA('LoKi::DistanceCalculator') < %s)" \
                     % props["AMAXDOCA_MAX"]
        
        if extraCuts and extraCuts.has_key('CombinationCut') :
            comboCuts =  extraCuts['CombinationCut'] + ' & ' + comboCuts

        # cuts for the vertexed combo
        #momCuts = '(VFASPF(VCHI2) < 10) & '
        momCuts = "(BPVDIRA > 0) & (BPVVDCHI2 > %s)" % props['BPVVDCHI2_MIN']
        momCuts += "& ((M > %s*MeV) | (BPVIPCHI2() > %s))" \
                   % (props['M_CHARM_VETO'],props['ALL_MIPCHI2DV_MIN'])
        if stage == 'ComRob':
            momCuts = "(BPVVD > %s) & (BPVVDR > %s)" % \
                      (props['BPVVD_MIN'],props['BPVVDR_MIN'])
            
        if extraCuts and extraCuts.has_key('MotherCut') :
            momCuts = extraCuts['MotherCut'] + ' & ' + momCuts

        # put it all together
        combo = Hlt2Member(CombineParticles, 'Combine',
                           DecayDescriptors=decayDesc,
                           InputLocations=inputSeq, CombinationCut=comboCuts,
                           MotherCut=momCuts)

        return bindMembers(name, inputSeq+[combo])

    # old code from the POINT days
    #def __getPointingCut(self,stage):
    #    if stage == 'ComRob': pt = 'RobustPointingUL'
    #    elif stage == 'ComTF': pt = 'TFPointUL'
    #    else: pt = stage + 'PointUL'               
    #    return "(BPVTRGPOINTINGWPT < %s)" % self.getProps()[pt]
        
    def __filter(self, name, stage, inputSeq, extraCode=None):
        '''Configures filter for all stages of topo lines.'''
        props = self.getProps()
        
        preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                     "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
        pid = "(('pi+'==ABSID) | ('K+'==ABSID))"
        
        cuts = "(MAXTREE(%s,PT) > %s*MeV) &" \
               % (pid,props["MAX_PT_MIN"])
        sum_pt_min = float(props['SUM_PT_MIN'])
        sum_ipchi2_min = float(props['SUM_IPCHI2_MIN'])
        if name.find('3Body') >=0:
            sum_pt_min += 250
            sum_ipchi2_min += 50
        if name.find('4Body') >= 0:
            sum_pt_min += 500
            sum_ipchi2_min += 100
        
        cuts += '(SUMTREE(PT,%s,0.0) > %.1f*MeV)' % (pid,sum_pt_min)
        cuts += '& (in_range(%s*MeV,MCOR,%s*MeV))' \
                % (props['MCOR_MIN'],props['MCOR_MAX'])
        if stage != 'ComRob':            
            cuts += '& (SUMTREE(MIPCHI2DV(PRIMARY),%s,0.0) > %s)' \
                    % (pid,sum_ipchi2_min)
            cuts += '& (MINTREE(%s,TRCHI2DOF) < %s)' \
                    % (pid,props['MIN_TRCHI2DOF_MAX'])
        if extraCode: cuts = cuts + ' & ' + extraCode
        filter = Hlt2Member(FilterDesktop, 'Filter', InputLocations=inputSeq,
                            Code=cuts,Preambulo=preambulo)
        return bindMembers(name, inputSeq+[filter])

    def __seqGEC(self):  
        '''Defines a global event cut (sets upper limit on n_tracks).'''
        modules =  CoreFactory('CoreFactory').Modules
        if 'LoKiTrigger.decorators' not in modules:
            modules.append('LoKiTrigger.decorators')
        
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

        # by default, configure as a pass-all filter with similar code.
        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        #filtCode = "CONTAINS('/Event/Raw/Velo/LiteClusters') < 3000"
        if self.getProp('USE_GEC'):
            filtCode = "CONTAINS('" + tracks.outputSelection() + \
                       "') < %(GEC_MAX)s" % self.getProps()

        Hlt2TopoKillTooManyInTrkAlg = VoidFilter('Hlt2TopoKillTooManyInTrkAlg',
                                                 Code=filtCode)
        return bindMembers(None,[tracks, Hlt2TopoKillTooManyInTrkAlg])

    def __inPartFilter(self, name, stage, inputSeq):
        '''Filters input particles for all stages of topo lines.'''
        props = self.getProps()        
        cuts = '(PT > %s*MeV) & (P > %s*MeV) ' \
               % (props['ALL_PT_MIN'],props['ALL_P_MIN'])
        if stage == 'ComRob':
            cuts += '& (MIPDV(PRIMARY) > %(ALL_MIPDV_MIN)s)' % props
        else:
            cuts += '& (MIPCHI2DV(PRIMARY) > %s) & (TRCHI2DOF < %s)' % \
                    (props['ALL_MIPCHI2DV_MIN'],
                     props['ALL_TRCHI2DOF_MAX'])
        
        filter = Hlt2Member(FilterDesktop,'Filter', InputLocations=inputSeq,
                            Code=cuts)
        # require PV3D reconstruction before our cut on IP!
        return bindMembers(name, [PV3D()]+inputSeq+[filter])

    def __buildNBodySeqs(self,lineName,seqName,stage,input):
        '''Builds a set of 2, 3 and 4 body lines for a given stage.'''
        # NB: Decay descriptors are just dummies
        props = self.getProps()

        # 2-body
        name = lineName.replace('NBody','2Body')        
        decay = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
        extraCut = "(AMINDOCA('LoKi::DistanceCalculator') < %s)" \
                   % props['AMINDOCA_MAX']
        topo2Body = self.__combine(name,stage,[input],decay,
                                   {'CombinationCut':extraCut})
        name = seqName.replace('NBody','2Body')        
        seq2Body = self.__filter(name,stage,[topo2Body])
        # 3-body
        name = lineName.replace('NBody','3Body')        
        decay = ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"]
        topo3Body = self.__combine(name,stage,[input,topo2Body],decay)
        name = seqName.replace('NBody','3Body')        
        seq3Body = self.__filter(name,stage,[topo3Body])
        # 4-body
        name = lineName.replace('NBody','4Body')        
        decay = ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]
        ptcut = '(ASUM(SUMTREE(PT,BASIC)) > %s*MeV' % props['SUM_PT_MIN']
        extraCuts = {'CombinationCut' : '(AM > 2*GeV) & (%s)' % ptcut}
        extraCuts = {}
        topo4Body = self.__combine(name,stage,[input,topo3Body],decay,
                                   extraCuts)
        name = seqName.replace('NBody','4Body')        
        seq4Body = self.__filter(name,stage,[topo4Body])
        return (seq2Body,seq3Body,seq4Body)

    def __makeLines(self,name,seqs):
        for n in [2,3,4]:
            lineName = name.replace('NBody','%dBody' % n)
            self.__makeLine(lineName,algos=[seqs[n-2]])

    def __makeReqLines(self,robSeqs,tfSeqs):
        for n in [2,3,4]:
            for m in [2,3,4]:
                lineName = 'TopoTF%dBodyReq%dYes' % (n,m)
                self.__makeLine(lineName,algos=[robSeqs[m-2],tfSeqs[n-2]])
                        
    def __apply_configuration__(self):
        '''Constructs all of the lines. Currently just assigns all particle
        Kaon ID'''
        from Hlt2SharedParticles.GoodParticles import GoodKaons
        from Hlt2SharedParticles.TrackFittedBasicParticles \
             import BiKalmanFittedKaons

        # robust lines
        stage = 'ComRob'
        input = self.__inPartFilter('TopoInputKaons',stage,[GoodKaons])
        robustSeqs = self.__buildNBodySeqs('TopoNBody','RobustTopoNBody',stage,
                                           input)
        self.__makeLines('TopoNBodySA',robustSeqs)
        
        # post-track-fit lines
        stage = 'ComTF'
        input = self.__inPartFilter('TopoTFInputKaons',stage,
                                    [BiKalmanFittedKaons])
        tfSeqs = self.__buildNBodySeqs('TopoTFNBody','PostTFTopoNBody',stage,
                                       input)
        self.__makeLines('TopoTFNBodySA',tfSeqs)
        self.__makeReqLines(robustSeqs,tfSeqs)

        # one-stage track-fit lines
        stage = 'OSTF'
        input = self.__inPartFilter('TopoOSTFInputKaons',stage,
                                    [BiKalmanFittedKaons])
        ostfSeqs = self.__buildNBodySeqs('TopoOSTFNBody','OSTFTopoNBody',
                                         stage, input)
        self.__makeLines('TopoOSTFNBody',ostfSeqs)
        
        # pseudo-robust lines
        stage = 'ComRob'
        input = self.__inPartFilter('TopoRobTFInputKaons',stage,
                                    [BiKalmanFittedKaons])
        robTFSeqs = self.__buildNBodySeqs('TopoRobTFNBody','RobTFTopoNBody',
                                          stage, input)
        self.__makeLines('TopoRobTFNBody',robTFSeqs)


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
