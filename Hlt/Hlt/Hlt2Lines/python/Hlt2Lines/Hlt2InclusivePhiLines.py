# =============================================================================
# @file Hlt2InclusivePhiLines.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# based on Magnus Lieng (magnus.lieng@cern.ch)
#          Kim Vervink (Kim.Vervink@cern.ch)
# @date 2011-01-04
# =============================================================================
"""Inclusive Phi line usign two steps:
  - Track cuts (IPS, PT, P, DOCA, MM, VCHI2, TRACKCHI2)
  - RICH PID calculated. (RICHPID(K-Pi))

Heavily based on the 3-step Inclusive Phi Line implementation by Magnus Lieng
and Kim Vervink.

"""

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusivePhiLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'KaonPT'             : 800      # MeV
                  ,'KaonIPS'            : 6        # dimensionless
                  ,'TrackChi2DOF'       : 5        # dimensionless
                  ,'PhiMassWin'         : 20       # MeV
                  ,'PhiMassWinSB'       : 30       # MeV
                  ,'PhiPT'              : 1800     # MeV
                  ,'PhiDOCA'            : 0.2      # mm
                  ,'PhiVCHI2'           : 20       # dimensionless
                  ,'KaonRichPID'        : 0        # dimensionless
                  # GEC
                  ,'GEC_Filter_NTRACK'  : True       # do or do not
                  ,'GEC_NTRACK_MAX'     : 180        # max number of tracks
                  # TOS
                  ,'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                  # Pre-/Post-scales
                  ,'Prescale'           : {'Hlt2IncPhi'           : 1.0
                                          ,'Hlt2IncPhiSidebands'  : 0.1
                                          ,'Hlt2IncPhiTrackFit'   : 0.001
                                          }
                  ,'Postscale'          : {'Hlt2IncPhi'           : 1.0
                                          ,'Hlt2IncPhiSidebands'  : 1.0
                                          ,'Hlt2IncPhiTrackFit'   : 1.0
                                          }
                  ,'HltANNSvcID'        : {'IncPhi'           : 50000
                                          ,'IncPhiSidebands'  : 50003
                                          ,'IncPhiTrackFit'   : 50002
                                          }
                  }
    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import CombineParticles
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedRichKaons

        ############################################################################
        #    Filter Kaons
        ############################################################################
        kaons = self.__filterTracks('IncPhiKaons', [BiKalmanFittedKaons])
        kaonsRich = self.__filterTracks('IncPhiKaonsRich', [BiKalmanFittedRichKaons], ['(PIDK>%(KaonRichPID)s)'])

        ############################################################################
        #    Create wide mass Phi combination
        ############################################################################
        widePhi = self.__combine('IncPhiWide', [kaons])
        widePhiRich = self.__combine('IncPhiWideRich', [kaonsRich])

        ############################################################################
        #    TOS
        ############################################################################
        widePhiTOS = self.__filterHlt1TOS('IncPhiWideTOS', widePhi)
        widePhiRichTOS = self.__filterHlt1TOS('IncPhiWideRichTOS', widePhiRich)

        ############################################################################
        #    Add tight mass cuts
        ############################################################################
        tightPhi = self.__tightenMass('IncPhiTight', [widePhiTOS])
        tightPhiRich = self.__tightenMass('IncPhiTightRich', [widePhiRichTOS])
        
        ############################################################################
        #    Make lines
        ############################################################################
        incPhiLine = self.__makeLine('IncPhi', algos=[tightPhi, tightPhiRich])
        incPhiTrackLine = self.__makeLine('IncPhiTrackFit', algos=[tightPhi])
        incPhiSBLine = self.__makeLine('IncPhiSidebands', algos=[widePhiTOS, widePhiRichTOS])
    
    def __makeLine(self, name, algos):
        # Prepend GEC to the algo list
        gec = self.__seqGEC()
        algoList = [gec] + algos
        # Now build the line
        from HltLine.HltLine import Hlt2Line
        line = Hlt2Line(name
                        ,prescale=self.prescale
                        ,postscale=self.postscale
                        ,algos=algoList
                       )
        from Configurables import HltANNSvc
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2%sDecision" % name : self.getProp('HltANNSvcID')[name] } )               
    
    def __filterTracks(self, name, inputContainers, extraCuts=[]):
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        from HltTracking.HltPVs import PV3D
        # Build the cuts
        baseCuts = ["(TRCHI2DOF<%(TrackChi2DOF)s)", "(PT>%(KaonPT)s*MeV)", "(MIPCHI2DV(PRIMARY)>%(KaonIPS)s)"]+extraCuts
        _code = (' & '.join(baseCuts)) % self.getProps()
        # Create filter
        _filter = Hlt2Member(FilterDesktop
                            ,'Filter'
                            ,Inputs=inputContainers
                            ,Code=_code
                            )
        # Require the PV3D reconstruction before our cut on IPS
        return bindMembers(name, [PV3D()] + inputContainers + [_filter])
    
    def __combine(self, name, inputSeq):
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles      
        # Build cuts
        combCutBase = ["(ADAMASS('phi(1020)')<%(PhiMassWinSB)s*MeV)", "(AMINDOCA('LoKi::TrgDistanceCalculator')<%(PhiDOCA)s)"]
        motherCutBase = ["(PT>%(PhiPT)s*MeV)", "(VFASPF(VCHI2PDOF)<%(PhiVCHI2)s)"]
        _combinationCut =  (" & ".join(combCutBase)) % self.getProps()
        _motherCut = (" & ".join(motherCutBase)) % self.getProps()
        # Combine particles
        _combineParticles = Hlt2Member(CombineParticles
                                       ,'Combine'
                                       ,DecayDescriptors=["phi(1020) -> K+ K-"]
                                       ,Inputs=inputSeq
                                       ,CombinationCut=_combinationCut
                                       ,MotherCut=_motherCut
                                      )
        combSeq = bindMembers(name, inputSeq+[_combineParticles])
        return combSeq
    
    def __filterHlt1TOS(self, name, inputComb) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import TisTosParticleTagger

        filterTOS = Hlt2Member(TisTosParticleTagger
                               ,'Hlt1TOSFilter'
                               ,Inputs = [inputComb.outputSelection()]
                               ,TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
                              )
        filterTOSSeq = bindMembers(name, [inputComb, filterTOS])
        return filterTOSSeq
        
    def __tightenMass(self, name, inputSeq):
      from HltLine.HltLine import Hlt2Member, bindMembers
      from Configurables import FilterDesktop

      _code = "(ADMASS('phi(1020)')<%(PhiMassWin)s*MeV)" % self.getProps()
      _filter = Hlt2Member(FilterDesktop
                           ,'Filter'
                           ,Inputs=inputSeq
                           ,Code=_code
                           )
      filterSeq = bindMembers(name, inputSeq+[_filter] )
      return filterSeq

    def __seqGEC(self) : # {
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        from HltLine.HltLine import bindMembers

        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('GEC_Filter_NTRACK') : # {
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()
        # }

        KillTooManyTracksAlg = VoidFilter('Hlt2IncPhiKillTooManyTracksAlg', Code=filtCode)
        Hlt2CharmKillTooManyInTrk = bindMembers(None, [tracks, KillTooManyTracksAlg])
        return Hlt2CharmKillTooManyInTrk

# EOF
