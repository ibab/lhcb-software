#!/usr/bin/env python
# =============================================================================
# @file Hlt2B2XGammaLines.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-03-24
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from HltLine.HltLine import Hlt2Member, bindMembers
from Configurables import CombineParticles

class Hlt2RadiativeTopoConf(HltLinesConfigurableUser):
  __slots__ = {# Track cuts
               'Track_PT_MIN'       : 500.0  , # MeV
               'Track_P_MIN'        : 5000.0 , # MeV
               'Track_IPCHI2_MIN'   : 10.0   , # Dimensionless
               'Track_HARD_IPCHI2_MIN' : 16.0   , # Dimensionless
               'Track_CHI2_MAX'     : 5    , # Dimensionless
               # KS cuts
               'USE_KS'            : True,
               'KS_IPCHI2DV_MIN'   : 16.0,    # unitless
               'KS_TRACK_PT_MIN'   : 300.0  , # MeV
               'KS_TRACK_P_MIN'    : 3000.0 , # MeV
               'KS_BPVVDCHI2_MIN'  : 1000.0,  # unitless 
               'KS_MASS_WINDOW'    : 30.0,    # MeV
               # Track combination cuts
               'DITRACK_AMAXDOCA_MAX'      : 0.15   , # mm        
               'DITRACK_MINTRCHI2DOF_MAX'  : 3      , # Dimensionless 
               'DITRACK_VTXCHI2_MAX'       : 10     , # Dimensionless
               'DITRACK_M_MAX'             : 2000   , # MeV
               'DITRACK_PT_MIN'            : 1500   , # MeV
               'DITRACK_HARD_TRACKMAX_PT_MIN'   : 3000  , # MeV
               'DITRACK_SOFT_TRACKMAX_PT_MIN'   : 1200  , # MeV
               # Photon
               'photon_SOFT_PT_MIN'      : 2000.0 , # MeV
               'photon_HARD_PT_MIN'      : 2500.0 , # MeV
               # Track + photon combination cuts
               'B_SUMPT_MIN'        : 5000   , # MeV
               'B_FDCHI2_MIN'       : 64     , # Dimensionless
               'B_PT_MIN'           : 2000   , # MeV
               'B_CORRM_MAX'        : 7000   , # MeV
               'B_CORRM_MIN'        : 4000   , # MeV
               # GEC
               'USE_GEC'            : True   ,
               'GEC_NTRACK_MAX'     : 120    , # max number of tracks
               # L0
               'L0FilterPhoton'     : "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Photon','Electron'] ] ),
               'L0FilterTrack'      : "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Hadron'] ] ),
               # HLT1
               'HLT1Filter'    : "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!Velo)(?!BeamGas)(?!Incident).*Decision')" ,
               # TOS
               'USE_TRACKTOS'       : True   ,
               'TisTosParticleTaggerSpecs': { "Hlt1TrackAllL0Decision%TOS": 0, "Hlt1TrackPhotonDecision%TOS":0 } ,
               # IDs
               'HltANNSvcID'   : {'RadiativeTopoTrack' : 50515, 
                                  'RadiativeTopoPhoton': 50516} ,
               }
               
  def __combineTracks(self, name, inputParticles):
    from HltLine.Hlt2Monitoring import Hlt2MonitorMinMax
    props = self.getProps()
    #trackCuts = """(MIPCHI2DV(PRIMARY) > %(Track_IPCHI2_MIN)s)""" % props
    comboCuts = """((AALLSAMEBPV) | (AMINCHILD(MIPCHI2DV(PRIMARY)) > %(Track_HARD_IPCHI2_MIN)s )) &
                   (AM < 5000*MeV) &
                   (AMAXDOCA('LoKi::DistanceCalculator') < %(DITRACK_AMAXDOCA_MAX)s)""" % props
    motherCuts = """(BPVDIRA > 0) &
                    (M < %(DITRACK_M_MAX)s * MeV) &
                    (PT > %(DITRACK_PT_MIN)s * MeV) &
                    (VFASPF(VCHI2) < %(DITRACK_VTXCHI2_MAX)s) &
                    (MINTREE(HASTRACK, TRCHI2DOF) < %(DITRACK_MINTRCHI2DOF_MAX)s)""" % props
    combination = Hlt2Member(CombineParticles                                  ,
                             'CombineTracks'                                   ,
                             DecayDescriptors=["K*(892)0 -> K+ K+","K*(892)0 -> K+ K-","K*(892)0 -> K- K-"] ,
                             Inputs=inputParticles                             ,
                             #DaughtersCuts={"K+": trackCuts, "K-": trackCuts } ,
                             CombinationCut=comboCuts                          ,
                             MotherCut=motherCuts                              ,
                             )
    return bindMembers(name, inputParticles+[combination])

  def __seqGEC(self) : # {
    from Configurables import LoKi__VoidFilter as VoidFilter
    from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
    from HltLine.HltLine import bindMembers

    modules = CoreFactory('CoreFactory').Modules
    for i in [ 'LoKiTrigger.decorators' ]:
      if i not in modules : modules.append(i)

    from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
    tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

    filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()

    filtName = 'Hlt2RadiativeTopoKillTracksAlg'
    Hlt2TrackKillerAlg = VoidFilter('Hlt2RadiativeTopoKillTracksAlg', Code=filtCode)
    return bindMembers(None, [tracks, Hlt2TrackKillerAlg])

  def __filterHlt1TOS(self, name, inputComb) :
    from HltLine.HltLine import Hlt2Member, bindMembers
    from Configurables import TisTosParticleTagger
    from HltLine.HltLine import Hlt2Member,bindMembers
    TOSParticleTagger = TisTosParticleTagger(name+"Tagger")
    TOSParticleTagger.TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
    TOSParticleTagger.NoRegex = True
    TOSParticleTagger.ProjectTracksToCalo = False
    TOSParticleTagger.CaloClustForCharged = False
    TOSParticleTagger.CaloClustForNeutral = False
    TOSParticleTagger.TOSFrac = {4:0.0,5:0.0 }
    TOSParticleTagger.Inputs = [inputComb.outputSelection()]
    TOSParticleTagger.Output = 'Hlt2/%s/Particles'%(name+'Tagger')
    return bindMembers(name+'ParticleFilter', [inputComb,TOSParticleTagger])

  def __filterMaxTrackPt(self, name, inputTracks, cutProp):
    from HltLine.HltLine import Hlt2Member, bindMembers
    from Configurables import FilterDesktop
    props = self.getProps()
    # Cut on the max PT of the physical tracks
    filterCut = "MAXTREE(HASTRACK, PT)>%s" % self.getProp(cutProp)
    __filter = Hlt2Member( FilterDesktop          ,
                           name+'MaxTrackPtFilter'   ,
                           Inputs=[inputTracks] ,
                           Code=filterCut)
    return bindMembers(name, [inputTracks, __filter])

  def __buildInputTracks(self, name, inputContainers, isKS):
    """Filters input tracks for radiative topological."""
    from HltLine.HltLine import Hlt2Member, bindMembers
    from Configurables import FilterDesktop
    props = self.getProps()
    cuts = "(TRCHI2DOF < %(Track_CHI2_MAX)s) &" % props
    if isKS:
      cuts += """(PT > %(KS_TRACK_PT_MIN)s*MeV) &
                 (P > %(KS_TRACK_P_MIN)s*MeV) &
                 (MIPCHI2DV(PRIMARY) > %(KS_IPCHI2DV_MIN)s)""" % props
    else:
      cuts += """(PT > %(Track_PT_MIN)s*MeV) &
                 (P > %(Track_P_MIN)s*MeV) &
                 (MIPCHI2DV(PRIMARY) > %(Track_IPCHI2_MIN)s)""" % props    
    __filter = Hlt2Member(FilterDesktop,
                          'Filter',
                          Inputs=inputContainers,
                          Code=cuts)
    return bindMembers(name, inputContainers + [__filter])
    
  def __combineKS(self, name, inputTracks, mass):
    """Make KSs amd Lambdas."""
    from HltLine.HltLine import Hlt2Member, bindMembers
    from Configurables import CombineParticles
    props = self.getProps()
    dm = float(props['KS_MASS_WINDOW'])
    comboCuts = "in_range(%.3f*MeV, AM, %.3f*MeV)" % (mass-1.5*dm, mass+1.5*dm)
    momCuts = """in_range(%.3f*MeV, M, %.3f*MeV) &
                 (VFASPF(VCHI2)<%s) & 
                 (BPVDIRA > 0) &
                 (BPVVDCHI2 > %s)""" % (mass-dm,mass+dm, props['DITRACK_VTXCHI2_MAX'], props['KS_BPVVDCHI2_MIN'])
    createKS = Hlt2Member(CombineParticles,
                          name+"Combine",
                          DecayDescriptors=["K+ -> pi+ pi-"], 
                          CombinationCut=comboCuts,
                          MotherCut = momCuts,
                          Inputs=inputTracks)
    return bindMembers(name, inputTracks + [createKS])

  def __joinInputContainers(self, kaons, kshorts, tag):
      """Puts Ks and KSs into one container w/ same ID."""
      from HltLine.HltLine import Hlt2Member, bindMembers
      from Configurables import GaudiSequencer, FilterDesktop
      inputK = GaudiSequencer("InputK",Members=kaons.members())
      inputKS = GaudiSequencer("InputKS", Members=kshorts.members())
      inputs = [inputK, inputKS]
      finputs = [kaons, kshorts]
      inputAll = GaudiSequencer("InputRadTopoALL",
                                Members=inputs,
                                ModeOR=True,
                                ShortCircuit=False)
      __filter = Hlt2Member(FilterDesktop,
                            'FilterRadTopoAllInput',
                            Inputs=finputs,
                            Code='ALL')
      return bindMembers("InputRadTopoALL" + tag, [inputAll, __filter])

  def __buildPhotons(self, name, inputTracks, cutProp):
    from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotonsFromL0 as photons
    from HltLine.HltLine import Hlt2Member, bindMembers
    from Configurables import FilterDesktop
    filterCut = "PT>%s*MeV" % self.getProps()[cutProp]
    __filter = Hlt2Member( FilterDesktop       ,
                           name+'Filter'       ,
                           Inputs=[photons] ,
                           Code=filterCut)
    return bindMembers(name, [inputTracks, photons, __filter])

  def __combineTracksAndPhoton(self, name, *inputParticles):
    from HltLine.Hlt2Monitoring import Hlt2MonitorMinMax, Hlt2Monitor
    inputParticles = list(inputParticles)
    props = self.getProps()
    motherCuts = """(PT > %(B_PT_MIN)s) &
                    (((BPVCORRM > %(B_CORRM_MIN)s) & (BPVCORRM < %(B_CORRM_MAX)s))) &
                    (BPVVDSIGN > 0) &
                    (BPVVDCHI2 > %(B_FDCHI2_MIN)s) &
                    (SUMTREE(PT, (('K+'==ABSID)|('gamma'==ABSID)|('KS0'==ABSID)) , 0.0) > %(B_SUMPT_MIN)s*MeV)""" % props
    combination = Hlt2Member(CombineParticles                                         ,
                             "CombineTracksAndPhoton"+name                            ,
                             DecayDescriptors=["[B0 -> K*(892)0  gamma]cc"]           ,
                             MotherCut=motherCuts                                     ,
                             Preambulo=["from math import cos"]                       ,
                             Inputs=inputParticles)
    return bindMembers(name, inputParticles+[combination])    
  
  def __apply_configuration__(self):
    from HltLine.HltLine import Hlt2Line
    from HltTracking.HltPVs import PV3D
    from Configurables import HltANNSvc
    from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons
    from Hlt2SharedParticles.GoodParticles import GoodPions
    from HltLine.HltDecodeRaw import DecodeL0CALO
    # Configure trigger requirements
    l0filterphoton = self.getProp("L0FilterPhoton")
    if l0filterphoton == "":
      l0filterphoton = None
    l0filtertrack = self.getProp("L0FilterTrack")
    if l0filtertrack == "":
      l0filtertrack = None
    hltfilter = self.getProp("HLT1Filter")
    if hltfilter == "" :
      hltfilter = None
    # Decide if we need V0
    useKS = self.getProps()['USE_KS']
    # Initialize algorithm list
    algoList = []
    # GECs?
    if self.getProp('USE_GEC'):
      gec = self.__seqGEC()
      algoList.append(gec)
    # Build the di-track
    tracks  = self.__buildInputTracks('InputKaons', [BiKalmanFittedKaons], False)
    if useKS:
        # Build KS
        pionsForKS = self.__buildInputTracks('InputPionsForKS', [GoodPions], True)
        inputKS = self.__combineKS('InputKS', [pionsForKS], 497.648)
        tracks = self.__joinInputContainers(tracks, inputKS, 'All')
    ditracks = self.__combineTracks('DiTrack', [tracks])
    if self.getProp('USE_TRACKTOS'):
      ditracks = self.__filterHlt1TOS('DiTrackTOS', ditracks)
    # Prepare two sets of tracks
    ditracksSoft = self.__filterMaxTrackPt('DiTrackSoftPt', ditracks, 'DITRACK_SOFT_TRACKMAX_PT_MIN')
    ditracksHard = self.__filterMaxTrackPt('DiTrackHardPt', ditracks, 'DITRACK_HARD_TRACKMAX_PT_MIN')
    # Build photons
    photonsSoft = self.__buildPhotons('SoftPhotons', ditracksHard, 'photon_SOFT_PT_MIN')
    photonsHard = self.__buildPhotons('HardPhotons', ditracksSoft, 'photon_HARD_PT_MIN')
    # Build combinations
    softTracksAndHardPhoton = self.__combineTracksAndPhoton('Photon', ditracksSoft, photonsHard)
    hardTracksAndSoftPhoton = self.__combineTracksAndPhoton('Track', ditracksHard, photonsSoft)
    # Create the line
    algoListTrack = algoList[:]
    algoListTrack.extend([PV3D(), BiKalmanFittedKaons, ditracksHard, DecodeL0CALO, photonsSoft, hardTracksAndSoftPhoton])
    algoListPhoton  = algoList[:]
    algoListPhoton.extend([PV3D(), BiKalmanFittedKaons, ditracksSoft, DecodeL0CALO, photonsHard, softTracksAndHardPhoton])
    lineTOS = Hlt2Line('RadiativeTopoTrack' ,
                       L0DU=l0filtertrack      ,
                       HLT=hltfilter           ,
                       prescale=1.0            ,
                       algos=algoListTrack       ,
                       postscale=1.0  )
    lineL0 = Hlt2Line('RadiativeTopoPhoton'  ,
                       L0DU=l0filterphoton     ,
                       HLT=hltfilter           ,
                       prescale=1.0            ,
                       algos=algoListPhoton        ,
                       postscale=1.0  )
    HltANNSvc().Hlt2SelectionID.update( { "Hlt2RadiativeTopoTrackDecision" : self.getProp('HltANNSvcID')['RadiativeTopoTrack']} )
    HltANNSvc().Hlt2SelectionID.update( { "Hlt2RadiativeTopoPhotonDecision" : self.getProp('HltANNSvcID')['RadiativeTopoPhoton']} )
      
# EOF

