#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


# Basic File: Htl2TopologicalLines in v6r0p1 of Hlt2/Hlt2Lines
# Aim: Mu + n Track Lines, inclusive muon trigger for lines
# which need low pt muon and associated tracks
# Background reduction from filters and FD, IP Chi2 cuts
# Author: A. Shires (alexander.shires@cern.ch)
# 


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Hlt2MuNTrackLinesConf(HltLinesConfigurableUser) :
    '''Class for prototype of Hlt2MuNTrack (topo + muon) line.'''

    # Steering variables:
    #
    # NB: these values are overridden by those
    # specified in HltSettings/Muons_draft2011.py.
    #   
    __slots__ = {        
        #L0 and HLT1 filter
        'L0FILTER'           : "L0_CHANNEL_RE('.*Muon')",                 
        'HLT1FILTER'         : "",#"HLT_PASS_RE('Hlt1Track.*Decision')",
        #mu + n tracks filter cuts
        'MCOR_MAX'           : 7000.0,  # MeV
        'MCOR_MIN'           : 0.0,     # MeV 
        'MCOR_NTR_MIN'       : 4000.0,  # MeV 
        'SUM_PT_1TR_MIN'     : 2000.0,  # MeV
        'SUM_PT_2TR_MIN'     : 2000.0,  # MeV
        'SUM_PT_3TR_MIN'     : 2600.0,  # MeV
        'MAX_PT_MIN'         : 1500.0,  # MeV 
        'MAX_PT_NTR_MIN'     : 1500.0,  # MeV 
        'SUM_IPCHI2_1TR_MIN' : 50,      # unitless
        'SUM_IPCHI2_2TR_MIN' : 75,      # unitless
        'SUM_IPCHI2_3TR_MIN' : 100,     # unitless
        'BPVVDCHI2_MIN'      : 32.0,    # unitless
        'MIN_TRCHI2DOF_MAX'  : 3,       # unitless
        #combination cuts
        'AMAXDOCA_MAX'       : 0.12,    # mm  # loose doca for mu + 2,3 tracks
        'AMAXDOCA_MIN'       : 0.12,    # mm  # tight doca for mu + 1 track, 2, 3 track combos
        'DIRA_LOOSE_MIN'     : 0.99,    # rad # 2,3 track combos
        'DIRA_TIGHT_MIN'     : 0.995,   # rad # mu + n tr line
        # mother cuts
        'MASS_1TR_VETO'      : 2000.0,  # MeV 
        'MASS_2TR_VETO'      : 3000.0,  # MeV 
        'MASS_3TR_VETO'      : 4000.0,  # MeV 
        # cuts on input particles
        'ALL_MIPCHI2DV_MIN'  : 16.0,    # unitless
        'ALL_TRCHI2DOF_MAX'  : 3.0,     # unitless
        'ALL_MU_PT_MIN'      : 800.0,   # MeV
        'ALL_TR_PT_MIN'      : 600.0,   # MeV
        'ALL_P_MIN'          : 5000.0,  # MeV
        # pre- and post-scale values
        'Prescale' : {},
        'Postscale' : {},
        # HltANNSvc IDs for each line (need to be configurable)
        'HltANNSvcID' : {'Hlt2Mu1TrackDecision'      : 50734,
                         'Hlt2Mu2TrackDecision'      : 50774,
                         'Hlt2Mu3TrackDecision'      : 50814
                         }
        }

    def __updateHltANNSvc(self,line):
        '''Updates the HltANNSvc after a new line has been constructed.'''
        from Configurables import HltANNSvc
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update({lineName:id})

    def __makeLine(self, lineName, algos):
        '''Constructs a new line and registers it to the HltANNSvc.'''
        Algos = algos
        # Prepend a filter on the number of tracks
        hltfilter = self.getProp("HLT1FILTER")
        if not hltfilter: 
            hltfilter = None
        l0filter = self.getProp("L0FILTER")
        if not l0filter:
            l0filter = None
        #create Hlt line
        from HltLine.HltLine import Hlt2Line
        Hlt2Line(lineName, L0DU=l0filter, HLT=hltfilter, prescale=self.prescale,
                 postscale=self.postscale,algos=Algos) 
        self.__updateHltANNSvc(lineName)


    def __combineTracks(self, name, inputSeq, decayDesc, extraCuts=None):
        '''Configures track combinations'''  
        props = self.getProps()      
        # combo cuts
        comboCuts = '(AM < %s*MeV)' % props['MCOR_MAX']
        # tight DOCA for n tracks
        comboCuts += "& (ACUTDOCA(%s,'LoKi::DistanceCalculator'))" % props["AMAXDOCA_MIN"]
       # loose DIRA for n tracks
        momCuts = "(BPVDIRA > %s)" % (props['DIRA_LOOSE_MIN'])
        momCuts += "& (BPVVDCHI2 > %s)" % (props['BPVVDCHI2_MIN'])
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        combo = Hlt2Member(CombineParticles, 'Combine',
                           DecayDescriptors=decayDesc,
                           InputLocations=inputSeq, 
                           CombinationCut=comboCuts,
                           MotherCut=momCuts)
    
        return bindMembers(name, inputSeq+[combo])

    def __combine(self, name, inputSeq, decayDesc, extraCuts=None):
        '''Configures mu + track combinations'''
        props = self.getProps() 
        # combo cuts
        comboCuts = '(AM < %s*MeV)' % props['MCOR_MAX']
        # same PV cut
        #comboCuts += "& (AALLSAMEBPV)"
        mass_veto = float(props['MASS_1TR_VETO'])
        # optional choices
        if "Mu1Tr" in name:
            # tight DOCA for mu + track
            comboCuts += "& (ACUTDOCA(%s,'LoKi::DistanceCalculator'))" % props["AMAXDOCA_MIN"]
        elif "Mu2Tr" in name:
            # loose DOCA for mu + 2,3 tracks
            comboCuts += "& (ACUTDOCA(%s,'LoKi::DistanceCalculator')) " % props["AMAXDOCA_MAX"]
            # mass veto
            mass_veto = float(props['MASS_2TR_VETO'])                                   
        elif "Mu3Tr" in name:  
            # loose DOCA for mu + 2,3 tracks
            comboCuts += "& (ACUTDOCA(%s,'LoKi::DistanceCalculator')) " % props["AMAXDOCA_MAX"]
            # mass veto
            mass_veto = float(props['MASS_3TR_VETO'])

        # Mother cuts
        momCuts = "(BPVDIRA > %s)" % (props['DIRA_TIGHT_MIN'])
        momCuts += "& (BPVVDCHI2 > %s)" % (props['BPVVDCHI2_MIN'])
        momCuts += "& (M > %s*MeV)" % (mass_veto)
        #optional cuts
        if extraCuts and extraCuts.has_key('CombinationCut') :
            comboCuts =  extraCuts['CombinationCut'] + ' & ' + comboCuts            
        if extraCuts and extraCuts.has_key('MotherCut') :
            momCuts = extraCuts['MotherCut'] + ' & ' + momCuts 
        # put it all together
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        combo = Hlt2Member(CombineParticles, 'Combine',
                           DecayDescriptors=decayDesc,
                           InputLocations=inputSeq, 
                           CombinationCut=comboCuts,
                           MotherCut=momCuts)
    
        return bindMembers(name, inputSeq+[combo])
        
    def __filter(self, name, inputSeq, extraCode=None):
        '''Configures filter for all mu + n track lines.'''
        props = self.getProps()
        
        preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                     "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]

        pid = "(('mu+'==ABSID) | ('K+'==ABSID) | ('pi+'==ABSID))"
        # cut values
        sum_pt_min = float(props['SUM_PT_1TR_MIN'])
        max_pt_min = float(props['MAX_PT_MIN'])
        sum_ipchi2_min = float(props['SUM_IPCHI2_1TR_MIN'])
        mcor_min = float(props['MCOR_MIN'])
        #adjust for different lines
        if 'Mu2Tr' in name:
            sum_pt_min = float(props['SUM_PT_2TR_MIN'])
            sum_ipchi2_min = float(props['SUM_IPCHI2_2TR_MIN'])
            mcor_min = float(props['MCOR_NTR_MIN'])
            max_pt_min = float(props['MAX_PT_NTR_MIN'])
        if 'Mu3Tr' in name:
            sum_pt_min = float(props['SUM_PT_3TR_MIN'])
            sum_ipchi2_min = float(props['SUM_IPCHI2_3TR_MIN'])
            mcor_min = float(props['MCOR_NTR_MIN'])
            max_pt_min = float(props['MAX_PT_NTR_MIN'])

        cuts = "(MAXTREE(%s,PT) > %s*MeV) &" \
                % (pid,props["MAX_PT_MIN"])
        cuts += '(SUMTREE(PT,%s,0.0) > %.1f*MeV)' \
                % (pid,sum_pt_min)
        cuts += '& (in_range(%s*MeV,MCOR,%s*MeV))' \
                % (mcor_min,props['MCOR_MAX'])
        cuts += '& (SUMTREE(MIPCHI2DV(PRIMARY),%s,0.0) > %s)' \
                % (pid,sum_ipchi2_min)
        cuts += '& (MINTREE(%s,TRCHI2DOF) < %s)' \
                % (pid,props['MIN_TRCHI2DOF_MAX'])
        if extraCode: cuts = cuts + ' & ' + extraCode

        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        _filter = Hlt2Member(FilterDesktop, 'Filter', InputLocations=inputSeq,
                            Code=cuts,Preambulo=preambulo)
        return bindMembers(name, inputSeq+[_filter])


    def __inputMuonL0Filter(self, name, inputSeq):
        '''Filters input muons and requires l0 TOS.'''
        props = self.getProps()     
        #configure tistostool
        from Configurables import DataOnDemandSvc, L0SelReportsMaker, L0DecReportsMaker
        DataOnDemandSvc().AlgMap["HltLikeL0/DecReports"] = L0DecReportsMaker( OutputLevel = 4 )
        DataOnDemandSvc().AlgMap["HltLikeL0/SelReports"] = L0SelReportsMaker( OutputLevel = 4 )
        from Configurables import TisTosParticleTagger
        TOSInputMuonsFilter = TisTosParticleTagger("TOSInputMuonsFilter") 
        TOSInputMuonsFilter.TriggerTisTosName= "L0TriggerTisTos"
        TOSInputMuonsFilter.TisTosSpecs = { "L0.*Muon.*Decision%TOS":0 }
        TOSInputMuonsFilter.ProjectTracksToCalo = FALSE
        TOSInputMuonsFilter.CaloClustForCharged = FALSE
        TOSInputMuonsFilter.CaloClustForNeutral = FALSE
        TOSInputMuonsFilter.TOSFrac = { 4:0.0, 5:0.0 }
        TOSInputMuonsFilter.InputLocations = [inputSeq[-1].outputSelection() ]
        #cuts
        cuts = '(PT > %s*MeV) ' % (props['ALL_MU_PT_MIN'])
        cuts += '& (P > %s*MeV) ' % (props['ALL_P_MIN'])
        cuts += '& (MIPCHI2DV(PRIMARY) > %s)' % (props['ALL_MIPCHI2DV_MIN'])
        cuts += '& (TRCHI2DOF < %s)' % (props['ALL_TRCHI2DOF_MAX'])
        #create filter
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        _filter = Hlt2Member(FilterDesktop,'Filter', InputLocations=inputSeq,
                            Code=cuts)
        # require PV3D reconstruction before our cut on IP!
        from HltTracking.HltPVs import PV3D
        return bindMembers(name, [PV3D()]+inputSeq+[_filter,TOSInputMuonsFilter])

    def __inputMuonHlt1Filter(self, name, inputSeq):
        '''Filters input muons and requires htl1 TOS.'''
        props = self.getProps()     
        #configure tistostool
        from Configurables import TisTosParticleTagger
        TOSInputMuonsFilter = TisTosParticleTagger("TOSInputMuonsFilter")
        TOSInputMuonsFilter.TisTosSpecs = { "Hlt1Track.*Decision%TOS":0 }
        TOSInputMuonsFilter.ProjectTracksToCalo = FALSE
        TOSInputMuonsFilter.CaloClustForCharged = FALSE
        TOSInputMuonsFilter.CaloClustForNeutral = FALSE
        TOSInputMuonsFilter.TOSFrac = { 4:0.0, 5:0.0 }
        TOSInputMuonsFilter.InputLocations = [inputSeq[-1].outputSelection() ]
        #cuts
        cuts = '(PT > %s*MeV) ' % (props['ALL_MU_PT_MIN'])
        #equal for all tracks
        cuts += '& (P > %s*MeV) ' % (props['ALL_P_MIN'])
        cuts += '& (MIPCHI2DV(PRIMARY) > %s)' % (props['ALL_MIPCHI2DV_MIN'])
        cuts += '& (TRCHI2DOF < %s)' % (props['ALL_TRCHI2DOF_MAX'])
        #create filter
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        _filter = Hlt2Member(FilterDesktop,'Filter', InputLocations=inputSeq,
                            Code=cuts)
        # require PV3D reconstruction before our cut on IP!
        from HltTracking.HltPVs import PV3D
        return bindMembers(name, [PV3D()]+inputSeq+[_filter,TOSInputMuonsFilter])

    def __inputParticleFilter(self, name, inputSeq):
        '''Filters input particles for all mu + n track lines.'''
        props = self.getProps()     
        if "Muon" in name:
            cuts = '(PT > %s*MeV) ' % (props['ALL_MU_PT_MIN'])
        else:
            cuts = '(PT > %s*MeV) ' % (props['ALL_TR_PT_MIN'])
        #equal for all tracks
        cuts += '& (P > %s*MeV) ' % (props['ALL_P_MIN'])
        cuts += '& (MIPCHI2DV(PRIMARY) > %s)' % (props['ALL_MIPCHI2DV_MIN'])
        cuts += '& (TRCHI2DOF < %s)' % (props['ALL_TRCHI2DOF_MAX'])
        #create filter
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        _filter = Hlt2Member(FilterDesktop,'Filter', InputLocations=inputSeq,
                            Code=cuts)
        # require PV3D reconstruction before our cut on IP!
        from HltTracking.HltPVs import PV3D
        return bindMembers(name, [PV3D()]+inputSeq+[_filter])

    def __buildNBodySeqs(self,lineName,_inputMuons,_inputKaons):
        '''Builds a set of mu + 1, 2 and 3 track lines.'''
        props = self.getProps()
        # 2-body     
        name = lineName.replace('MuNTr','Mu1Tr')        
        decay = ["B0 -> mu+ K+", "B0 -> mu+ K-", "B0 -> mu- K-"]
        extraCut = ""
        mu1track = self.__combine(name,[_inputKaons,_inputMuons],decay)
        seqMu1Track = self.__filter(name,[mu1track])
        #2 track input to three body
        name = "MuNTrack2Track"
        track2decay = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
        track2body = self.__combineTracks(name, [_inputKaons], track2decay)
        # 3-body
        name = lineName.replace('MuNTr','Mu2Tr')
        decay = ["B+ -> K*(892)0 mu+", "B- -> K*(892)0 mu-"]
        mu2track = self.__combine(name,[track2body, _inputMuons],decay)
        seqMu2Track = self.__filter(name,[mu2track])
        #3 track input to four body
        name = "MuNTrack3Track"
        track3decay = ["D*(2010)+ -> K*(892)0 K+", "D*(2010)- -> K*(892)0 K-"]
        track3body = self.__combineTracks(name, [track2body, _inputKaons], track3decay)
        # 4-body
        name = lineName.replace('MuNTr','Mu3Tr')        
        decay = ["B0 -> D*(2010)+ mu-","B0 -> D*(2010)+ mu+","B0 -> D*(2010)- mu-"]
        mu3track = self.__combine(name,[track3body, _inputMuons],decay)
        seqMu3Track = self.__filter(name,[mu3track])
        return (seqMu1Track,seqMu2Track,seqMu3Track)

    def __makeLines(self,name,seqs):
        for n in [1,2,3]:
            lineName = name.replace('MuNTr','Mu%dTr' % n)
            self.__makeLine(lineName,algos=[seqs[n-1]])

    def __apply_configuration__(self):
        '''Constructs all of the lines'''
        from Hlt2SharedParticles.TrackFittedBasicParticles \
             import BiKalmanFittedKaons, BiKalmanFittedMuons
        # lines
        _inputKaons = self.__inputParticleFilter('MuNTrackInputKaons'
                                    ,[BiKalmanFittedKaons])
        _inputMuons = self.__inputParticleFilter('MuNTrackInputMuons'
                                    ,[BiKalmanFittedMuons])
        Seqs = self.__buildNBodySeqs('MuNTrack',
                                         _inputMuons, _inputKaons)
        self.__makeLines('MuNTrack',Seqs)        


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
