#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from Gaudi.Configuration import *
from Configurables import HltANNSvc, FilterDesktop, CombineParticles
from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
from HltTracking.HltPVs import PV3D

# Basic File: Htl2TopologicalLines in v6r0p1 of Hlt2/Hlt2Lines
# Subsequent Author: A. Shires (alexander.shires@cern.ch)
# 


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Hlt2MuNTrackLinesConf(HltLinesConfigurableUser) :
    '''Class for prototype of Hlt2MuNTrack (topo + muon) line.'''

    # Steering variables:
    #
    # NB: these values can be (and most likely are) overridden by those
    # specified in HltSettings/TopoLines.py (so don't change their names!).
    #   
    #Keep these variables  - will want to ovveride them anyway
    __slots__ = {        
        #HLT1 filter?
        'L0FILTER'          : "L0_CHANNEL('Muon')",                 
        'HLT1FILTER'        : "",#"HLT_PASS_RE('Hlt1Track.*Decision')",
        #mu + n tracks filter cuts
        'MCOR_MAX'          : 7000.0,  # MeV
        'MCOR_MIN'          : 0.0,     # MeV 
        'MCOR_NTR_MIN'      : 4000.0,  # MeV 
        'SUM_PT_MIN'        : 2000.0,  # MeV
        #'SUM_PT_2TR_MIN'    : 2000.0, # MeV
        #'SUM_PT_3TR_MIN'    : 2000.0, # MeV
        'MAX_PT_MIN'        : 1500.0,  # MeV 
        'SUM_IPCHI2_MIN'    : 60,      #
        #'SUM_IPCHI2_2TR_MIN': 80,     #
        #'SUM_IPCHI2_2TR_MIN': 100,    #
        'BPVVDCHI2_MIN'     : 32.0,    # unitless
        'MIN_TRCHI2DOF_MAX' : 5,
        #m + n tracks and n-1 tracks combination cuts
        'AMAXDOCA_MAX'      : 0.12,    # mm  # loose doca for mu + n tracks
        'AMAXDOCA_MIN'      : 0.12,    # mm  # tight doca for mu + 1 track
        'AMINDOCA_MAX'      : 0.12,    # mm  # minimum doca for 2 body combinations
        'DIRA_MIN'          : 0.99,    #     # mu + tr line + 2,3 track combos
        'DIRA_TIGHT_MIN'    : 0.995,   #     # mu + 2, 3 tr line
        #'DIRA_MuNTr_MIN'    : 0.995,   #     # mu + 2, 3 tr line
        'MASS_VETO'      : 2000.0,     # MeV # moved from 2.5 GeV, 0 for track combos?
        # cuts on input particles
        'ALL_MIPCHI2DV_MIN' : 25.0,    # unitless #mother IP CHI2 > then this if less than mass cut
        'ALL_TRCHI2DOF_MAX' : 3.0,     # unitless
        'ALL_MU_PT_MIN'     : 800.0,   # MeV
        'ALL_TR_PT_MIN'     : 600.0,   # MeV
        'ALL_P_MIN'         : 5000.0,  # MeV
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
        Hlt2Line(lineName, L0DU=l0filter, HLT=None, prescale=self.prescale,
                 postscale=self.postscale,algos=Algos) 
        self.__updateHltANNSvc(lineName)
        
    def __combine(self, name, inputSeq, decayDesc, extraCuts=None):
        '''Configures common particle combos used by all topo lines.'''        
        props = self.getProps() 
        # combo cuts
        # high end mass cut
        comboCuts = '(AM < %s*MeV)' % props['MCOR_MAX']
        # same PV cut
        #comboCuts += "& (AALLSAMEBPV)"
        # default DIRA value
        diraval = 10.0
        # optional choices
        if ("Combo" in name): 
            # tight DOCA for n tracks
            comboCuts += "& (ACUTDOCA(%s,'LoKi::DistanceCalculator'))" % props["AMAXDOCA_MIN"]
            # loose DIRA for n tracks
            diraval = props['DIRA_MIN']
        elif ("Mu1Tr" in name):
            # tight DOCA for mu + track
            comboCuts += "& (ACUTDOCA(%s,'LoKi::DistanceCalculator'))" % props["AMAXDOCA_MIN"]
            # tight DIRA for mu + track
            diraval = props['DIRA_TIGHT_MIN']
        else:
            # loose DOCA for mu + 2,3 tracks
            comboCuts += "& (ACUTDOCA(%s,'LoKi::DistanceCalculator')) " % props["AMAXDOCA_MIN"]
            # tight DIRA for mu + 2,3 tracks
            diraval = props['DIRA_TIGHT_MIN']
               
        if extraCuts and extraCuts.has_key('CombinationCut') :
            comboCuts =  extraCuts['CombinationCut'] + ' & ' + comboCuts
        
        

        # mass veto for mu + n track combinations
        mass_veto = float(props['MASS_VETO'])
        # reset for n tracks combinations

        # Mother cuts
        momCuts = "(BPVDIRA > %s)" % (diraval)
        momCuts += "& (BPVVDCHI2 > %s)" % (props['BPVVDCHI2_MIN'])
        # USE CORRELATION OF MASS AND IPCHI2
        if not "Combo" in name: 
            momCuts += "& ((M > %s*MeV) | (BPVIPCHI2() > %s))" % (mass_veto,props['ALL_MIPCHI2DV_MIN'])
            
        if extraCuts and extraCuts.has_key('MotherCut') :
            momCuts = extraCuts['MotherCut'] + ' & ' + momCuts
    
        # put it all together
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
        sum_pt_min = float(props['SUM_PT_MIN'])
        max_pt_min = float(props['MAX_PT_MIN'])
        sum_ipchi2_min = float(props['SUM_IPCHI2_MIN'])
        mcor_min = float(props['MCOR_MIN'])

        #print "AS DEBUG:", name, mcor_min, sum_pt_min
        #adjust for different lines
        if 'Mu2Tr' in name:
            sum_pt_min += 500
            #max_pt_min += 250
            sum_ipchi2_min += 20
            mcor_min = float(props['MCOR_NTR_MIN'])
        if 'Mu3Tr' in name:
            sum_pt_min += 1000
            #max_pt_min += 500
            sum_ipchi2_min += 40
            mcor_min = float(props['MCOR_NTR_MIN'])
            
        #print "AS DEBUG:", name, mcor_min, sum_pt_min

        cuts = "(MAXTREE(%s,PT) > %s*MeV) &" \
               % (pid,props["MAX_PT_MIN"])
        
        cuts += '(SUMTREE(PT,%s,0.0) > %.1f*MeV)' % (pid,sum_pt_min)
        cuts += '& (in_range(%s*MeV,MCOR,%s*MeV))' \
                % (mcor_min,props['MCOR_MAX'])
        cuts += '& (SUMTREE(MIPCHI2DV(PRIMARY),%s,0.0) > %s)' \
                    % (pid,sum_ipchi2_min)
        cuts += '& (MINTREE(%s,TRCHI2DOF) < %s)' \
                    % (pid,props['MIN_TRCHI2DOF_MAX'])
        if extraCode: cuts = cuts + ' & ' + extraCode

        # print "AS DEBUG", cuts

        _filter = Hlt2Member(FilterDesktop, 'Filter', InputLocations=inputSeq,
                            Code=cuts,Preambulo=preambulo)
        return bindMembers(name, inputSeq+[_filter])

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
        _filter = Hlt2Member(FilterDesktop,'Filter', InputLocations=inputSeq,
                            Code=cuts)
        # require PV3D reconstruction before our cut on IP!
        return bindMembers(name, [PV3D()]+inputSeq+[_filter])

    def __buildNBodySeqs(self,lineName,seqName,_inputMuons,_inputKaons):
        '''Builds a set of mu + 1, 2 and 4 track lines.'''
        props = self.getProps()
        # 2-body     
        name = lineName.replace('MuNTr','Mu1Tr')        
        decay = ["B0 -> mu+ K+", "B0 -> mu+ K-", "B0 -> mu- K-"]
        extraCut = ""
        mu1track = self.__combine(name,[_inputKaons,_inputMuons],decay)
        name = seqName.replace('MuNTr','Mu1Tr')        
        seqMu1Track = self.__filter(name,[mu1track])
        #2 track input to three body
        name = "MuNTrack2TrackCombo"
        track2decay = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
        track2body = self.__combine(name, [_inputKaons], track2decay)
        # 3-body
        name = lineName.replace('MuNTr','Mu2Tr')        
        decay = ["B+ -> K*(892)0 mu+", "B- -> K*(892)0 mu-"]
        mu2track = self.__combine(name,[track2body, _inputMuons],decay)
        name = seqName.replace('MuNTr','Mu2Tr')        
        seqMu2Track = self.__filter(name,[mu2track])
        #3 track input to four body
        name = "MuNTrack3TrackCombo"
        track3decay = ["D*(2010)+ -> K*(892)0 K+", "D*(2010)- -> K*(892)0 K-"]
        track3body = self.__combine(name, [track2body, _inputKaons], track3decay)
        # 4-body
        name = lineName.replace('MuNTr','Mu3Tr')        
        decay = ["B0 -> D*(2010)+ mu-","B0 -> D*(2010)+ mu+","B0 -> D*(2010)- mu-"]
        mu3track = self.__combine(name,[track3body, _inputMuons],decay)
        #                           extraCuts)
        name = seqName.replace('MuNTr','Mu3Tr')        
        seqMu3Track = self.__filter(name,[mu3track])
        return (seqMu1Track,seqMu2Track,seqMu3Track)

    def __makeLines(self,name,seqs):
        for n in [1,2,3]:
            lineName = name.replace('MuNTr','Mu%dTr' % n)
            self.__makeLine(lineName,algos=[seqs[n-1]])

    def __apply_configuration__(self):
        '''Constructs all of the lines. ToDo: Sort out particle ID'''
        from Hlt2SharedParticles.TrackFittedBasicParticles \
             import BiKalmanFittedKaons, BiKalmanFittedMuons
        # lines
        _inputKaons = self.__inputParticleFilter('MuNTrackInputKaons'
                                    ,[BiKalmanFittedKaons])
        _inputMuons = self.__inputParticleFilter('MuNTrackInputMuons'
                                    ,[BiKalmanFittedMuons])
        Seqs = self.__buildNBodySeqs('MuNTrack','MuNTrack',
                                         _inputMuons, _inputKaons)
        self.__makeLines('MuNTrack',Seqs)        


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
