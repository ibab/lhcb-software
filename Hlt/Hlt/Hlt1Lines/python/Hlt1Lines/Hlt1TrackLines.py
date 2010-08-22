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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1 $"
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
    __slots__ = { 
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
        
        def trackprepare(ip,pt,p):
            #OutputOfConfirmation = confirmationguidedforward(type).outputSelection()
            from HltLine.HltDecodeRaw import DecodeIT
            from Hlt1Lines.Hlt1GECs import Hlt1GEC
            sh = [  Hlt1GEC(),
                    Velo,PV3D().ignoreOutputSelection(),
                    Member ( 'TF', 'OTIP'
                           , InputSelection = 'Velo' 
                           , FilterDescriptor = [ 'IP_PV3D,>,'+ip]
                         ),
                    DecodeIT,
                    Member ( 'TU', 'Forward'
                           , RecoName = 'Forward'
                           , tools = [ Tool( HltTrackUpgradeTool
                                             ,tools = [ConfiguredPR( "Forward" )] )]
                           ),
                    Member ( 'TF' , 'OTPT' ,
                            FilterDescriptor = ['PT,>,'+pt],
                            ),
                    Member ( 'TF' , 'OTMom' ,
                            FilterDescriptor = ['P,>,'+p],
                            )
                   ]
            return sh


        def afterburn(chi2,ipchi2):
            after = [ PV3D().ignoreOutputSelection()
                , Member ( 'TU' , 'FitTrack' , RecoName = "FitTrack", callback = setupHltFastTrackFit )
                , Member ( 'TF' , 'TrkChi2'
                           , FilterDescriptor = ["FitChi2OverNdf,<,"+chi2],
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
            return after

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
                           , FilterDescriptor = ["FitChi2OverNdf,<,"+chi2],
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
                          MinIPCHI2 = ipchi2
                         )   
                ]
            return after
 
        from Hlt1Lines.HltL0Candidates import L0Channels
        Line ( 'TrackAllL0'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Hadron','Muon','DiMuon','Electron','Photon'] ] ) 
             , algos =  trackprepare("0.125","1250","12500") + afterburn("3",50)
             )
        Line ( 'TrackMuon'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Muon','DiMuon'] ] ) 
             , algos =  trackprepare("0.08","800","8000") + muonAfterburn("10",25.)
             )
        Line ( 'TrackPhoton'
             , prescale = self.prescale
             , postscale = self.postscale
             , L0DU = "L0_CHANNEL('Photon')"
             , algos =  trackprepare("0.125","800","8000") + afterburn("5",50)
             )    

