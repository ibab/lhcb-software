class Charm_320Vis_300L0_10Hlt1_Aug09 :
    """
    Threshold settings for Hlt1 and 2 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-26
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds' )
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.HltCommissioningLines  import HltCommissioningLinesConf
        from Hlt1Lines.HltMuonLines           import HltMuonLinesConf
        from Hlt1Lines.HltHadronLines         import HltHadronLinesConf
        from Hlt1Lines.HltElectronLines       import HltElectronLinesConf
        from Hlt1Lines.HltPhotonLines         import HltPhotonLinesConf
        from Hlt1Lines.HltExpressLines        import HltExpressLinesConf
        from Hlt1Lines.HltVeloLines           import HltVeloLinesConf

        return { HltHadronLinesConf : {  'SingleHadron_PTCut' : 4800
                                         , 'HadMain_IPCut' : 0.06 
                                         , 'HadMain_PTCut' : 250
                                         , 'HadCompanion_DZCut' : 1.83
                                         , 'HadCompanion_IPCut' : 0.05
                                         , 'HadCompanion_PTCut' : 750
                                         , 'HadCompanion_PointingCut' : 0.39
                                        }
                 , HltPhotonLinesConf : {  'Pho_EtCut' : 2800.0
                                           , 'Track_PtCut' : 650
                                           , 'Track_IPCut3D' : 0.15
                                           , 'Pho_IsPho' : -0.082
                                           }


                 , HltElectronLinesConf : {  'Ele_EtCut' : 2800.0
                                             , 'Compan_PtCut' : 2750
                                             , 'Ele_IPCut' : 0.13
                                            }
                 , HltMuonLinesConf : {  'DiMuon_SumPtCut' : 120.0
                                         , 'Muon_PtCut' : 1000
                                         , 'MuonIP_PtCut' : 1000
                                         , 'Muon_IPMinCut' : 0.05
                                         , 'DiMuon_DOCACut' : 1.0
                                         , 'DiMuon_MassCut' : 2500
                                         , 'DiMuonIP_MassCut' : 300
                                         , 'DiMuon_IPCut' : 0.08
                                         , 'Prescale' : { 'Hlt1MuTrack4JPsi' : 0. }
                                         , 'MuTrackMuPt' : 500
                                         , 'MuTrackMuIP' : 0.07
                                         , 'MuTrackTrPt' : 550
                                         , 'MuTrackTrIP' : 0.1
                                         , 'MuTrackDoca' : 0.2
                                         , 'MuTrackDZ' : 2.0
                                         , 'MuTrackDimuMass' : 1000
                                         , 'MuTrackPoint' : 0.4
                                         , 'MuTrackTrChi2' : 10
                                         }
                 }
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return [] 

        
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return [ 'Hlt1SingleHadron',
                 'Hlt1DiHadron',
                 'Hlt1SoftDiHadron',
                 'Hlt1SingleMuonNoIPL0',
                 'Hlt1SingleMuonIPCL0',                    
                 'Hlt1DiMuonNoIPL0Di',
                 'Hlt1DiMuonNoIP2L0',
                 'Hlt1DiMuonNoIPL0Seg',
                 'Hlt1DiMuonIPCL0Di',
                 'Hlt1DiMuonIPC2L0',
                 'Hlt1DiMuonIPCL0Seg',
                 'Hlt1DiMuonNoPVL0Di',
                 'Hlt1DiMuonNoPV2L0',
                 'Hlt1DiMuonNoPVL0Seg',
                 'Hlt1SingleMuonNoIPL0',
                 'Hlt1SingleMuonIPCL0',
                 'Hlt1DiMuonNoIPL0Di',
                 'Hlt1DiMuonNoIP2L0',
                 'Hlt1DiMuonNoIPL0Seg',
                 'Hlt1DiMuonIPCL0Di',
                 'Hlt1DiMuonIPC2L0',
                 'Hlt1DiMuonIPCL0Seg',
                 'Hlt1DiMuonNoPVL0Di',
                 'Hlt1DiMuonNoPV2L0',
                 'Hlt1DiMuonNoPVL0Seg',
                 'Hlt1MuTrack',
                 'Hlt1MuTrackFitMu',
                 'Hlt1MuTrack4JPsi',
                 'Hlt1LumiNoBeam',
                 'Hlt1LumiBeamCrossing',
                 'Hlt1LumiBeam1',
                 'Hlt1LumiBeam2',
                 'Hlt1L0Muon',
                 'Hlt1L0DiMuon',
                 'Hlt1L0Muon,lowMult',
                 'Hlt1L0DiMuon,lowMult',
                 'Hlt1L0Electron',
                 'Hlt1L0Photon',
                 'Hlt1L0Hadron',
                 'Hlt1L0LocalPi0',
                 'Hlt1L0GlobalPi0',
                 'Hlt1SingleElectron',
                 'Hlt1ElectronTrackWithIP',
                 'Hlt1ElectronTrackNoIP',
                 'Hlt1Photon',
                 'Hlt1PhotonFromEle',
                 'Hlt1VeloASide',
                 'Hlt1VeloCSide',
                 'Hlt1AlignVelo',
                 'Hlt1XPress',
                 'Hlt1NonRandomODIN',
                 'Hlt1RandomODIN',
                 'Hlt1Tell1Error' ,
                 'Hlt1Incident']
    
    
