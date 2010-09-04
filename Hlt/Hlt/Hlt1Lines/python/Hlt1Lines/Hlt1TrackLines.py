# =============================================================================
## @@file
#  Configuration of One Track Lines
#  @@author Vladimir Gligorov vladimir.gligorov@@cern.ch
#  @@date 2010-08-16
# =============================================================================
"""
 script to configure One Track trigger lines
"""
# =============================================================================
__author__  = "Vladimir Gligorov vladimir.gligorov@@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.6 $"
# =============================================================================

import Gaudi.Configuration 

def histosfilter(name,xlower=0.,xup=100.,nbins=100):
    """ return the dictonary with the booking of the histograms associated to a filter
    @@param filter name i.e 'PT
    """
    histosfs = { name : ( name,xlower,xup,nbins),
                 name+"Best" : (name+"Best",xlower,xup,nbins) }
    return histosfs


from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt1TrackLinesConf(HltLinesConfigurableUser) :
    #--------------------------------
    #
    # V. Gligorov
    __slots__ = {       'RunInParallel' : False
                    ,   'AllL0_PT'      : 1250.
                    ,   'AllL0_P'       : 12500.
                    ,   'AllL0_IP'      : 0.125
                    ,   'AllL0_IPChi2'  : 50.
                    ,   'AllL0_TrChi2'  : 3
                    ,   'Muon_PT'       : 1000.
                    ,   'Muon_P'        : 10000.
                    ,   'Muon_IP'       : 0.08
                    ,   'Muon_IPChi2'   : 25.
                    ,   'Muon_TrChi2'   : 10 
                    ,   'Photon_PT'     : 500
                    ,   'Photon_P'      : 5000
                    ,   'Photon_IP'     : 0.125
                    ,   'Photon_IPChi2' : 50
                    ,   'Photon_TrChi2' : 5
                    ,   'Velo_NHits'    : 9 # Minimum number of hits on a Velo track - 1
                    ,   'Velo_Qcut'     : 3 # This - 1 is the maximum allowed difference between
                                            # the number of hits on a velo track and the 
                                            # expected number of hits from the track's angle
                                            # and first measured point on the track        
                }
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import bindMembers
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import Hlt1Tool   as Tool
        from HltLine.HltLine import hlt1Lines  
        from Hlt1Lines.HltFastTrackFit import setupHltFastTrackFit
        from HltTracking.HltReco import Velo
        from HltTracking.HltPVs  import PV3D
        from Configurables import HltTrackUpgradeTool, PatForwardTool
        from Hlt1Lines.HltConfigurePR import ConfiguredPR
        from Configurables import HltFilterFittedVertices
        
        def trackprepare(ip,pt,p,pt_reco,p_reco):
            from HltLine.HltDecodeRaw import DecodeIT
            from Hlt1Lines.Hlt1GECs import Hlt1GEC
            seq =  [Hlt1GEC(),
                    Velo,PV3D().ignoreOutputSelection(),
                    Member ( 'TF', 'OTIP'
                           , InputSelection = 'Velo' 
                           , FilterDescriptor = [ 'IP_PV3D,>,%s'%ip]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('IP',0.,1.,100) 
                         ),
                    Member ( 'TF', 'OTNH'
                           , FilterDescriptor = [ 'NumberOfTrackHits,>,%s'%self.getProp('Velo_NHits')]
                         ),
                    Member ( 'TF', 'OTEXH'
                           , FilterDescriptor = [ 'MissedVeloHits,||<,%s'%self.getProp('Velo_Qcut')]
                         ),
                    DecodeIT]
            if (self.getProp('RunInParallel')): #Running in parallel so run with defaults
                seq += [Member ( 'TU', 'Forward'
                           , RecoName = 'Forward'
                           , tools = [ Tool( HltTrackUpgradeTool
                                             ,tools = [ConfiguredPR( "Forward") ] )]
                           )]
            else :
                seq += [Member ( 'TU', 'Forward'
                           , RecoName = 'Forward'
                           , tools = [ Tool( HltTrackUpgradeTool
                                             ,tools = [ConfiguredPR( "Forward",pt_reco,p_reco) ] )]
                           )]
            seq += [Member ( 'TF' , 'OTPT' ,
                            FilterDescriptor = ['PT,>,%s'%pt]
                            , HistogramUpdatePeriod = 1 
                            , HistoDescriptor  = histosfilter('PT',0.,8000.,200)
                            ),
                    Member ( 'TF' , 'OTMom' ,
                            FilterDescriptor = ['P,>,%s'%p]
                            , HistogramUpdatePeriod = 1 
                            , HistoDescriptor  = histosfilter('P',0.,80000.,200)
                            )
                   ]
            return seq

        def afterburn(chi2,ipchi2):
            return [ PV3D().ignoreOutputSelection()
                , Member ( 'TU' , 'FitTrack' , RecoName = "FitTrack", callback = setupHltFastTrackFit )
                , Member ( 'TF' , 'TrkChi2'
                           , FilterDescriptor = ["FitChi2OverNdf,<,%s"%chi2],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitChi2OverNdf',0.,100.,100)
                           )
                , Member ('HltFilterFittedTracks', 'FFT',
                          OutputSelection = "%Decision",
                          InputSelection1 = '%TFTrkChi2', 
                          InputSelection2 = 'PV3D',
                          MinIPCHI2 = ipchi2
                         )
                ]

        def muonAfterburn(chi2,ipchi2) :
            from Configurables import MuonRec, MuonIDAlg
            from MuonID import ConfiguredMuonIDs
            cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data="2010")
            HltMuonIDAlg        = cm.configureMuonIDAlg("Hlt1TrackMuonIDAlg")
            HltMuonIDAlg.TrackLocation          = "Hlt1/Track/FitTrack"
            HltMuonIDAlg.MuonIDLocation         = "Hlt1/Track/TrackMuons/MuonID"
            HltMuonIDAlg.MuonTrackLocation      = "Hlt1/Track/TrackMuons/Muon"
            HltMuonIDAlg.MuonTrackLocationAll   = "Hlt1/Track/TrackMuons/AllMuon"
            HltMuonIDAlg.FindQuality            = False
            after = [ PV3D().ignoreOutputSelection()
                , Member ( 'TU' , 'FitTrack' , RecoName = "FitTrack", callback = setupHltFastTrackFit )
                , Member ( 'TF' , 'TrkChi2'
                           , FilterDescriptor = ["FitChi2OverNdf,<,%s"%chi2],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitChi2OverNdf',0.,100.,100)
                           )
                , MuonRec() 
                , HltMuonIDAlg]

            from Configurables import ChargedProtoParticleAddMuonInfo
            from Configurables import ChargedProtoCombineDLLsAlg
            from Configurables import ChargedProtoParticleMaker
            from Configurables import CombinedParticleMaker
            from Configurables import ProtoParticleMUONFilter

            charged      = ChargedProtoParticleMaker("Hlt1TrackMuonProtoPMaker")
            charged.InputTrackLocation  = ["Hlt1/Track/FitTrack"]
            charged.OutputProtoParticleLocation = "Hlt1/ProtoP/TrackMuon"
            
            muon                = ChargedProtoParticleAddMuonInfo("Hlt1TrackChargedProtoPAddMuon")
            muon.ProtoParticleLocation  = "Hlt1/ProtoP/TrackMuon" 
            muon.InputMuonPIDLocation   = "Hlt1/Track/TrackMuons/MuonID"
            
            Hlt1Muons = CombinedParticleMaker("Hlt1TrackMuonCPM")
            Hlt1Muons.Particle = "muon" 
            Hlt1Muons.addTool(ProtoParticleMUONFilter('Muon'))
            Hlt1Muons.Muon.Selection = ["RequiresDet='MUON' IsMuon=True" ]
            Hlt1Muons.Input =  "Hlt1/ProtoP/TrackMuon"
            Hlt1Muons.WriteP2PVRelations = False 

            after += [charged,muon,Hlt1Muons]

            from Configurables import LoKi__VoidFilter as VoidFilter
            Hlt1MuonFilter = VoidFilter('Hlt1_MuonFilter'
                                 , Code = " CONTAINS('Phys/Hlt1TrackMuonCPM/Particles') > 0 "
                                 )  

            after += [    Hlt1MuonFilter,
                          Member ('HltFilterFittedParticles', 'FFT',
                          OutputSelection = "%Decision",
                          InputSelection1 = 'TES:/Event/Phys/Hlt1TrackMuonCPM/Particles', 
                          InputSelection2 = 'PV3D',
                          MinIPCHI2 = '%s'%ipchi2
                         )   
                ]
            return after

        props = self.getProps()

        minptcut_forreco = -9999. 
        minpcut_forreco  = -9999. #Turn off and use default in ConfigredPR
        if not props["RunInParallel"] : #If we don't run in parallel can be more agressive
            minptcut_forreco = min(props["AllL0_PT"],props["Muon_PT"])
            minpcut_forreco  = min(props["AllL0_P"],props["Muon_P"])

        from Hlt1Lines.HltL0Candidates import L0Channels
        #Do not run on photon triggers to allow soft track XGamma recovery
        Line ( 'TrackAllL0'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Hadron','Electron','Muon','DiMuon'] ] ) #"L0_DECISION_PHYSICS" 
             , algos = trackprepare(ip=props["AllL0_IP"],pt=props["AllL0_PT"],p=props["AllL0_P"],pt_reco = minptcut_forreco, p_reco = minpcut_forreco) \
                     + afterburn(chi2=props["AllL0_TrChi2"],ipchi2=props["AllL0_IPChi2"])
             )
        Line ( 'TrackMuon'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Muon','DiMuon'] ] ) 
             , algos = (trackprepare(ip=props["Muon_IP"],pt=props["Muon_PT"],p=props["Muon_P"],pt_reco = minptcut_forreco, p_reco = minpcut_forreco) \
                     + muonAfterburn(chi2=props["Muon_TrChi2"],ipchi2=props["Muon_IPChi2"]))
             )
        #Note: for the photons, nothing runs on these events except this line so can use much softer thresholds to recover as much XGamma as possible.     
        Line ( 'TrackPhoton'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_CHANNEL('Photon')"
             , algos = (trackprepare(ip=props["Photon_IP"],pt=props["Photon_PT"],p=props["Photon_P"],pt_reco = props["Photon_PT"],p_reco = props["Photon_P"]) \
                     + afterburn(chi2=props["Photon_TrChi2"],ipchi2=props["Photon_IPChi2"]))  
             )    

