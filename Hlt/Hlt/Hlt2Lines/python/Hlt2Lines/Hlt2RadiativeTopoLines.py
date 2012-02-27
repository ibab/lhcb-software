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
               'Track_PT_MIN'       : 700.0  , # MeV
               'Track_P_MIN'        : 5000.0 , # MeV
               'Track_IPCHI2_MIN'   : 10.0   , # Dimensionless
               'Track_CHI2_MAX'     : 5    , # Dimensionless
               # Track combination cuts
               'DITRACK_AMAXDOCA_MAX'      : 0.15   , # mm        
               'DITRACK_MINTRCHI2DOF_MAX'  : 3      , # Dimensionless 
               'DITRACK_VTXCHI2_MAX'       : 10     , # Dimensionless
               'DITRACK_M_MAX'             : 2000   , # MeV
               'DITRACK_PT_MIN'            : 1500   , # MeV
               # Photon
               'photon_PT_MIN'      : 2500.0 , # MeV
               # Track + photon combination cuts
               'B_SUMPT_MIN'        : 5000   , # MeV
               'B_FDCHI2_MIN'       : 64     , # Dimensionless
               'B_PT_MIN'           : 1000   , # MeV
               #'B_MASS_MAX'         : 7000   , # MeV
               #'B_MASS_MIN'         : 4000   , # MeV
               'B_CORRM_MAX'        : 7000  , # MeV
               'B_CORRM_MIN'        : 4000   , # MeV
               # GEC
               'GEC_USE'            : True   ,
               'GEC_NTRACK_MAX'     : 180    , # max number of tracks
               # L0
               'L0Filter'      : "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Photon','Electron'] ] ),
               # HLT1
               'HLT1Filter'    : "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!Velo)(?!BeamGas)(?!Incident).*Decision')" ,
               # TOS
               'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 } ,
               # IDs
               'HltANNSvcID'   : {'RadiativeTopoTrackTOS': 50515, 
                                  'RadiativeTopoPhotonL0': 50516} ,
               }
               
  def __combineTracks(self, name, inputParticles, decayDescriptor):
    from HltLine.Hlt2Monitoring import Hlt2MonitorMinMax
    props = self.getProps()
    trackCuts = """(TRCHI2DOF < %(Track_CHI2_MAX)s) & (MIPCHI2DV(PRIMARY) > %(Track_IPCHI2_MIN)s) & (PT > %(Track_PT_MIN)s *MeV) & (P > %(Track_P_MIN)s *MeV)""" % props
    #trackMonitor = Hlt2MonitorMinMax("TRCHI2DOF", "Track chi2/dof", 0, 10, nbins=100) + '&' +\
    #               Hlt2MonitorMinMax("MIPCHI2DV(PRIMARY)", "Track IP chi2", 0, 200, nbins=100) + '&' +\
    #               Hlt2MonitorMinMax("PT", "Track p_{T}", 0, 3000, nbins=50)
    comboCuts = """(AALLSAMEBPV) &
                   (AM < 5000*MeV) &
                   (AMAXDOCA('LoKi::DistanceCalculator') < %(DITRACK_AMAXDOCA_MAX)s)""" % props
    motherCuts = """(BPVDIRA > 0) &
                    (M < %(DITRACK_M_MAX)s * MeV) &
                    (PT > %(DITRACK_PT_MIN)s * MeV) &
                    (VFASPF(VCHI2) < %(DITRACK_VTXCHI2_MAX)s) &
                    (MINTREE(HASTRACK, TRCHI2DOF) < %(DITRACK_MINTRCHI2DOF_MAX)s)""" % props
    #motherMonitor = Hlt2MonitorMinMax("BPVVDCHI2", "FD chi2", 0 , 200, nbins=100) + '&' +\
    #                Hlt2MonitorMinMax("MINTREE(HASTRACK, TRCHI2DOF)", "Best track chi2/dof", 0, 25, nbins=50) + '&' +\
    #                Hlt2MonitorMinMax("BPVIPCHI2()", "vtx IP #chi^2", 0, 300, nbins=60) + '&' +\
    #                Hlt2MonitorMinMax("SUMTREE(MIPCHI2DV(PRIMARY), M<4000, 0.0)", "#sum_{IP chi^{2}}", 0, 500, nbins=50) + ' & ' +\
    #                Hlt2MonitorMinMax("VFASPF(VCHI2)", "Vertex chi2/dof", 0, 16, nbins=100) + '&' +\
    #                Hlt2MonitorMinMax("M", "M(2Track)", 0, 6000, nbins=60) + '&' +\
    #                Hlt2MonitorMinMax("BPVCORRM", "CorrM(2Track)", 0, 10000, nbins=100)
    combination = Hlt2Member(CombineParticles                                  ,
                             'CombineTracks'                                   ,
                             DecayDescriptors=decayDescriptor                  ,
                             Inputs=inputParticles                             ,
                             DaughtersCuts={"K+": trackCuts, "K-": trackCuts } ,
    #                         DaughtersMonitors={"K+": trackMonitor, "K-": trackMonitor} ,
                             CombinationCut=comboCuts                          ,
                             # CombinationMonitor=comboMonitor                   ,
                             MotherCut=motherCuts                              ,
    #                         MotherMonitor=motherMonitor
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
    filterTOS = Hlt2Member(TisTosParticleTagger                                  ,
                           'Hlt1TOSFilter'                                       ,
                           Inputs=[inputComb.outputSelection()]                  ,
                           TisTosSpecs=self.getProp('TisTosParticleTaggerSpecs') )
    return bindMembers(name, [inputComb, filterTOS])

  def __buildInputTracks(self):
    from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons  
    decays = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
    return self.__combineTracks("DiTrack", [BiKalmanFittedKaons], decays)
    
  def __buildPhotons(self):
    from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotonsFromL0 as photons
    return photons

  def __combineTracksAndPhoton(self, name, inputTracks, inputPhotons):
    from HltLine.Hlt2Monitoring import Hlt2MonitorMinMax, Hlt2Monitor
    props = self.getProps()
    inputParticles = [inputTracks, inputPhotons]
    motherCuts = """(PT > %(B_PT_MIN)s) &
                    (((BPVCORRM > %(B_CORRM_MIN)s) & (BPVCORRM < %(B_CORRM_MAX)s))) &
                    (BPVVDSIGN > 0) &
                    (BPVVDCHI2 > %(B_FDCHI2_MIN)s) &
                    (SUMTREE(PT, (('K+'==ABSID)|('gamma'==ABSID)) , 0.0) > %(B_SUMPT_MIN)s*MeV)""" % props
    #motherMonitor = Hlt2MonitorMinMax("BPVDIRA", "B DIRA", 0.999, 1, nbins=100) + ' & ' +\
    #                Hlt2MonitorMinMax("BPVCORRM", "CorrM (2track + #gamma)", 3500, 10000, nbins=50) + ' & ' +\
    #                Hlt2MonitorMinMax("BPVVDCHI2", "Flight distance chi2", 0 , 250, nbins=100) + '&' +\
    #                Hlt2MonitorMinMax("BPVIPCHI2()", "VTX IP chi2", 0 , 100, nbins=100) + '&' +\
    #                Hlt2MonitorMinMax("M", "Mass (2track + #gamma)", 2000, 8000, nbins=60) + ' & ' +\
    #                Hlt2MonitorMinMax("SUMTREE(MIPCHI2DV(PRIMARY), M<4000, 0.0)", "#sum_{IP chi^{2}}", 0, 500, nbins=50) + ' & ' +\
    #                Hlt2MonitorMinMax("SUMTREE(PT, M<4000, 0.0)", "#sum p_{T}", 0, 10000, nbins=100)
    #gammaMonitor = Hlt2MonitorMinMax("PT", "#gamma p_{T}", 0, 5000, nbins=50)
    combination = Hlt2Member(CombineParticles                                         ,
                             "CombineTracksAndPhoton"                                 ,
                             DecayDescriptors=["[B0 -> K*(892)0  gamma]cc"]           ,
                             DaughtersCuts={"gamma": "(PT>%(photon_PT_MIN)s*MeV)" % props},
   #                          DaughtersMonitors={"gamma": gammaMonitor},
                             MotherCut=motherCuts                                     ,
   #                          MotherMonitor=motherMonitor                              ,
                             Preambulo=["from math import cos"]                       ,
                             Inputs=inputParticles)
    return bindMembers(name, inputParticles+[combination])    
  
  def __apply_configuration__(self):
    from HltLine.HltLine import Hlt2Line
    from HltTracking.HltPVs import PV3D
    from Configurables import HltANNSvc
    from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons  
    from HltLine.HltDecodeRaw import DecodeL0CALO
    
    l0filter = self.getProp("L0Filter")
    if l0filter == "":
      l0filter = None
    hltfilter = self.getProp("HLT1Filter")
    if hltfilter == "" :
      hltfilter = None

    algoList = []
    # GECs?
    if self.getProp('GEC_USE'):
      gec = self.__seqGEC()
      algoList.append(gec)
    # Build the pieces
    tracks  = self.__buildInputTracks()
    tracksTOS = self.__filterHlt1TOS('DiTrackTOS', tracks)
    photons = self.__buildPhotons()
    # Build combinations
    tracksAndPhoton = self.__combineTracksAndPhoton('PhotonL0', tracks, photons)
    tracksTOSAndPhoton = self.__combineTracksAndPhoton('TrackTOS', tracksTOS, photons)
    # Create the line
    algoListTOS = algoList[:]
    algoListL0  = algoList[:]
    algoListTOS.extend([PV3D(), BiKalmanFittedKaons, tracksTOS, DecodeL0CALO, photons, tracksTOSAndPhoton])
    algoListL0.extend([PV3D(), BiKalmanFittedKaons, tracks, DecodeL0CALO, photons, tracksAndPhoton])
    lineTOS = Hlt2Line('RadiativeTopoTrackTOS' ,
                       HLT=hltfilter           ,
                       prescale=1.0            ,
                       algos=algoListTOS       ,
                       postscale=1.0  )
    lineL0 = Hlt2Line('RadiativeTopoPhotonL0'  ,
                       L0DU=l0filter           ,
                       HLT=hltfilter           ,
                       prescale=1.0            ,
                       algos=algoListL0        ,
                       postscale=1.0  )
    HltANNSvc().Hlt2SelectionID.update( { "Hlt2RadiativeTopoTrackTOSDecision" : self.getProp('HltANNSvcID')['RadiativeTopoTrackTOS']} )
    HltANNSvc().Hlt2SelectionID.update( { "Hlt2RadiativeTopoPhotonL0Decision" : self.getProp('HltANNSvcID')['RadiativeTopoPhotonL0']} )
      
# EOF

