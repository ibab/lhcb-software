#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1MuonLines.py,v 1.34 2010-09-08 14:55:24 graven Exp $
# =============================================================================
## @file
#  Configuration of Muon Lines
#  Include New Line for Dimuon without PV: Leandro de Paula
#  2008-12-17 
#  @Translator from the original options: Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure Muon trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.34 $"
# =============================================================================


from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *


class Hlt1MuonLinesConf(HltLinesConfigurableUser) :
    # steering variables
    __slots__ = { 
        #  Muon Lines
        'Prescale'                  : {  'Hlt1SingleMuonNoVeloL0'      :  0.00001
                                      ,  'Hlt1DiMuonNoVeloL0'          :  0.00001
                                      }
        ,'L0SingleMuon'             :"Muon"
        ,'L0SingleMuonGEC'          :"Muon"
        ,'L0SingleMuonNoPV'         :"Muon,lowMult"
        ,'L0SingleMuon4BsMuMu'      :"MuonHigh"
        ,'L0SingleMuonHigh'         :"MuonHigh"
        ,'L0DiMuonNoPV'             :"DiMuon,lowMult"
        ,'L0DiMuon'                 :"DiMuon"
        ,'L0MuonNoVelo'             :"MUON,minbias"
        ,'DiMuon_SumPtCut'          : 1000.
        ,'L0AllMuon_PtCut'          :   80.
        ,'L0MuonNoPV_PtCut'         :  800.
        ,'Muon_DeltaPCut'           :    0.
        ,'Muon_3DVeloTMatchCut'     :    6.
        ,'Muon_VeloTMatchCut'       :  200.
        ,'Muon_ShareCut'            :    0.5
        ,'Muon_FitChiCut'           :   50.
        ,'Muon_FitMuChi2Cut'        :   75.
        ,'Muon_PtCut'               : 6000.
        ,'Muon_highPtCut'           : 5000.
        ,'MuonTS_PtCut'             :  600.
        ,'MuonIP_PtCut'             : 1300. 
        ,'Muon_IPMinCut'            :    0.08
        ,'DiMuon_DOCACut'           :    0.5
        ,'DiMuonTS_DOCACut'         : 1000.0
        ,'DiMuon_IPCut'             :    1.9
        ,'DiMuon_MassCut'           : 2500.
        ,'DiMuonNoPV_SumPtCut'      :  200.
        ,'DiMuonNoPV_MassCut'       : 1000.
        ,'DiMuonIP_MassCut'         :  500.
        ,'DiMuon_IPCut'             :    0.15
        
        # For the Muon+Track lines
        ,'MuTrackMuPt'    : 1000.
        ,'MuTrackMuIP'    : 0.025
        ,'MuTrackTrPt'    : 800.
        ,'MuTrackTrIP'    : 0.050
        ,'MuTrackDoca'    : 0.4
        ,'MuTrackDZ'      : 1.5
        #,'MuTrackAngle'   : 0.
        ,'MuTrackDimuMass': 1000.
        ,'MuTrackPoint'   : 0.4
        ,'MuTrackMuChi2'  : 10.
        ,'MuTrackTrChi2'  : 10.
        
        ,'MuTrackMuPt4JPsi'         : 1600.
        ,'MuTrackTrPt4JPsi'         : 400.
        ,'MuTrackDoca4JPsi'         : 0.1
        ,'MuTrackAngle4JPsiLow'     : 0.02
        ,'MuTrackAngle4JPsiHigh'    : 0.30
        ,'MuTrackDimuMass4JPsiLow'  : 2900.
        ,'MuTrackDimuMass4JPsiHigh' : 0.
        ,'MuTrackMuChi24JPsi'       : 4.
        ,'MuTrackTrChi24JPsi'       : 8.
        ,'bmm_IP'                   : 0.05
        ,'bmm_MuChi2'               : 16
        ,'bmm_fitChi2'              : 10
        ,'bmm_IPS'                  : 49
        ,'bmm_pt'                   : 1500
        ,'bmm_dimuonMass'           : 4700
        }
    

    def __apply_configuration__(self) : 
        from Configurables import HltMuonRec
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import bindMembers
        from HltLine.HltLine import Hlt1Tool   as Tool
        from Hlt1Lines.HltL0Candidates import convertL0Candidates, L0Channels
        #from HltTracking.HltReco import RZVelo
        from HltTracking.HltReco import Velo
        from HltTracking.HltPVs  import PV3D

        # First the pass-through NoPV line
        Hlt1NoPVMuonPassThrough = Line( 'NoPVPassThrough'
                                        , prescale = self.prescale
                                        , L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Muon,lowMult','DiMuon,lowMult'] ] )
                                        , postscale = self.postscale)  

        # It already Prepare the muon segments with proper error calling the tool MuonSeedTool
        RecoMuonSeg = HltMuonRec('HltRecoMuonSeg'
                                , OutputMuonTracksName='Hlt/Track/MuonSegmentForL0Single'
                                , DecodingFromCoord=True )

        ### Matching Confirmed T Tracks with VELO
        from Configurables import PatMatchTool, PatSeedingTool
        from Configurables import HltTrackUpgradeTool, L0ConfirmWithT, PatConfirmTool
        from HltLine.HltDecodeRaw import DecodeIT, DecodeTT, DecodeVELO
        from HltTracking.Hlt1TrackUpgradeConf import TMuonConf, Forward, FitTrack
        from HltTracking.Hlt1TrackMatchConf import MatchCallback

        MatchT =[ DecodeIT
                   , Member ('TU', 'TConf' , RecoName = TMuonConf.splitName()[-1]
                            , HistoDescriptor = { 'TMuonConfQuality': ( 'Seed track chi2',0.,20.,100) ,
                                                  'TMuonConfQualityBest': ( 'Seed track chi2 Best',0.,20.,100)} )
                   , Member ('TF', 'DeltaP' 
                             , FilterDescriptor = ['DeltaP,>,%(Muon_DeltaPCut)s' %self.getProps()]
                             , HistoDescriptor = { 'DeltaP': ( 'DeltaP',0.,2.,100), 'DeltaPBest': ( 'DeltaPBest',0.,2.,100)}
                             )
                   ]
        TMatchV = [ Velo
                    , DecodeTT
                    , Member ('TM', 'VeloT'
                              , InputSelection1 = "Velo"
                              , InputSelection2 = '%TFDeltaP'
                              , callback = MatchCallback('VeloT')
                              , tools = [ Tool( PatMatchTool, maxMatchChi2 = self.getProp( 'Muon_3DVeloTMatchCut') ) ]
                              #,OutputLevel=2
                              , HistoDescriptor = { 'chi2_PatMatch': ( 'Match 3dVelo-T (PatMatch)',0.,9.,100),
                                                    'chi2_PatMatchBest': ( 'Match 3dVelo-T (PatMatch) Best',0.,9.,100) }           
                              ) 
                    ]
        ### Check if 2 muons can come from a Vertex
        MakeVertex = [ Member( 'VM1', 'DOCA'
                             , FilterDescriptor = [ 'DOCA,<,%(DiMuon_DOCACut)s'%self.getProps() ]
                             , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                   'DOCABest': ('DOCA Best',0.,3.,100)}
                             )
                     , Member( 'VF', 'VeloT'  
                             , FilterDescriptor = [ 'VertexMatchIDsFraction_%%VM1L0DiMuon,>,%(DiMuon_IPCut)s'%self.getProps() ]
                             , HistoDescriptor = { 'VertexMatchIDsFraction_Hlt1DiMuonPrepVM1L0DiMuon': ('IP',0.,2,100),
				                  'VertexMatchIDsFraction_Hlt1DiMuonPrepVM1L0DiMuonBest': ('IP Best',0.,2,100) }
                             ) 
                     ]
        ### FastFit to improve track quality
        # single track without IP cut
        FastFitNoIP = [ Member ( 'TU' , 'FitTrack' ,      RecoName = FitTrack.splitName()[-1]
                                 , HistoDescriptor = { 'FitTrackQuality': ( 'Track Fit Chi2 / ndf',0.,50.,100),
                                                       'FitTrackQualityBest': ( 'lowest track Fit Chi2 / ndf',0.,50.,100) }        
                                 )
                        , Member ( 'TF' , 'Chi2Mu'
                                   , FilterDescriptor = ['FitMuChi2,<,%(Muon_FitMuChi2Cut)s'%self.getProps()]
                                   , HistoDescriptor = { 'FitMuChi2': ( 'muon contribution to track fit chi2 / ndf',0.,100.,100),
                                                         'FitMuChi2Best': ( 'lowest muon contribution to track fit chi2 / ndf',0.,100.,100) }  
                                   )
                        , Member ( 'TF' , 'Chi2OverN'
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['FitChi2OverNdf,<,%(Muon_FitChiCut)s'%self.getProps()]
                                   , HistoDescriptor = { 'FitChi2OverNdf': ( 'track fit chi2 / ndf',0.,75.,100),
                                                         'FitChi2OverNdfBest': ( 'lowest track fit chi2 / ndf',0.,75.,100) }  
                                   )
                        ]
        # single track with IP cut
        FastFitWithIP = [ PV3D().ignoreOutputSelection()
                          , Member ( 'TU' , 'FitTrack' , RecoName = FitTrack.splitName()[-1]
                                     , HistoDescriptor = { 'FitTrackQuality': ( 'Track Fit Chi2 / ndf',0.,50.,100),
                                                           'FitTrackQualityBest': ( 'lowest track Fit Chi2 / ndf',0.,50.,100) }  
                                     )
                          , Member ( 'TF' , 'IP' 
                                     ,  FilterDescriptor = ['FitIP_PV3D,||>,'+str(self.getProp('Muon_IPMinCut'))]
                                       , HistoDescriptor = {'FitIP_PV3D': ( 'muon IP to PV3D',0.,0.3,50),
                                                            'FitIP_PV3DBest': ( 'muon highest IP to PV3D',0.,0.3,50)}
                                     )
                          , Member ( 'TF' , 'Chi2Mu'
                                     , FilterDescriptor = ['FitMuChi2,<,'+str(self.getProp('Muon_FitMuChi2Cut'))]
                                     , HistoDescriptor = { 'FitMuChi2': ( 'muon contribution to track fit chi2 / ndf',0.,100.,100),
                                                           'FitMuChi2Best': ( 'lowest muon contribution to track fit chi2 / ndf',0.,100.,100) } 
                                     )
                          , Member ( 'TF' , 'Chi2OverN'
                                     , OutputSelection = '%Decision'
                                     , FilterDescriptor = ['FitChi2OverNdf,<,'+str(self.getProp('Muon_FitChiCut'))]
                                     , HistoDescriptor = { 'FitChi2OverNdf': ( 'track fit chi2 / ndf',0.,75.,100),
                                                         'FitChi2OverNdfBest': ( 'lowest track fit chi2 / ndf',0.,75.,100) } 
                                     )
                          ]
        # vertex (two tracks) without IP cut
        FastFitVtxNoIP = [ Member ( 'VU' , 'FitTrack' ,      RecoName = FitTrack.splitName()[-1])
                         , Member ( 'VF' , 'Chi2Mu'
                                  , FilterDescriptor = ['FitVertexMaxMuChi2,<,'+str(self.getProp('Muon_FitMuChi2Cut'))]
                                  , HistoDescriptor = { 'FitVertexMaxMuChi2': ( 'muon contribution to track fit chi2 / ndf',0.,100.,100),
                                                        'FitVertexMaxMuChi2Best': ( 'lowest muon contribution to track fit chi2 / ndf',0.,100.,100) } 
                                  )
                         , Member ( 'VF' , 'Chi2OverN'
                                  , OutputSelection = '%Decision'
                                  , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,'+str(self.getProp('Muon_FitChiCut'))]
                                  , HistoDescriptor = { 'FitVertexMaxChi2OverNdf': ( 'track fit chi2 / ndf',0.,75.,100),
                                                        'FitVertexMaxChi2OverNdfBest': ( 'lowest track fit chi2 / ndf',0.,75.,100) } 
                                  )   
                         ]
        # vertex (two tracks) with IP cut
        FastFitVtxWithIP = [ PV3D().ignoreOutputSelection()
                           , Member( 'VU' , 'FitTrack' ,      RecoName = FitTrack.splitName()[-1])
                           , Member( 'VF','IP'
                                   ,  FilterDescriptor = [ 'FitVertexMinIP_PV3D,||>,'+str(self.getProp('DiMuon_IPCut')) ]
                                   , HistoDescriptor = { 'FitVertexMinIP_PV3D': ('IP',0.,2,100),
                                                         'FitVertexMinIP_PV3DBest': ('IP Best',0.,2,100) }
                                   )
                           , Member( 'VF','Mass'
                                   , FilterDescriptor = [ 'FitVertexDimuonMass,>,'+str(self.getProp('DiMuonIP_MassCut')) ]
                                   , HistoDescriptor = { 'FitVertexDimuonMass': ('Di Muon Invariant Mass',0.,5000,100),
                                                         'FitVertexDimuonMassBest': ('Di Muon Invariant Mass - Best',0.,5000,100) }
                                   )
                           , Member( 'VF' , 'Chi2Mu'
                                   , FilterDescriptor = ['FitVertexMaxMuChi2,<,'+str(self.getProp('Muon_FitMuChi2Cut'))]
                                   , HistoDescriptor = { 'FitVertexMaxMuChi2': ( 'muon contribution to track fit chi2 / ndf',0.,100.,100),
                                                        'FitVertexMaxMuChi2Best': ( 'lowest muon contribution to track fit chi2 / ndf',0.,100.,100) } 
                                   )
                           , Member( 'VF' , 'Chi2OverN'
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,'+str(self.getProp('Muon_FitChiCut'))]
                                   , HistoDescriptor = { 'FitVertexMaxChi2OverNdf': ( 'track fit chi2 / ndf',0.,75.,100),
                                                         'FitVertexMaxChi2OverNdfBest': ( 'lowest track fit chi2 / ndf',0.,75.,100) } 
                                   )
                           ]
        ###  Prepare 
        AllL0MuPrep = bindMembers( 'AllL0MuPrep',
                                   [ convertL0Candidates('AllMuon')
                                     , Member ( 'TF', 'L0'
				              , FilterDescriptor = [ 'PT0,||>,'+str(self.getProp('L0AllMuon_PtCut')) ]
				              , HistoDescriptor = { 'PT0': ('Pt(L0) ',0.,5000.,100),
						               'PT0Best': ('Pt(L0) Best',0.,5000.,100)}
				              ) 
                                   ] + MatchT + TMatchV )

        if self.getProp('L0MuonNoVelo') in L0Channels() :
            MuonPrepT = bindMembers( 'MuonPrepT',
                                 #[ convertL0Candidates( str(self.getProp('L0SingleMuon')) )
                                 [ convertL0Candidates('AllMuon')
                                   ] + MatchT )

        if self.getProp('L0SingleMuon') in L0Channels() :
            # Muons 
            MuonPrep = bindMembers( 'MuonPrep',
                                    [ convertL0Candidates( str(self.getProp('L0SingleMuon')) )
                                      ] + MatchT + TMatchV )
            # Muon Segments
            MuonSegPrep = bindMembers ('MuonSegPrep' ,
                                       [ MuonPrep
                                         , RecoMuonSeg
                                         , Member ('TF', 'PrepMuSeg'
                                                   , RequirePositiveInputs = False
                                                   , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuSeg.getDefaults('OutputMuonTracksName')
                                                   , FilterDescriptor = [ 'DoShareM3_'+MuonPrep.outputSelection()+',<,'+str(self.getProp('Muon_ShareCut')) ] 
						   , HistoDescriptor = { 'DoShareM3_Hlt1MuonPrepTMVeloT': ('Shared Hits',0,5,6),
							   'DoShareM3_Hlt1MuonPrepTMVeloTBest': ('Shared Hits Best',0,5,6)}
						   )
                                         ] + MatchT + TMatchV
                                       )
            
                        
        if self.getProp('L0SingleMuonNoPV') in L0Channels() :
            # Muons in Events without Reconstructed Primary Vertices
            MuonNoPVPrep = bindMembers( 'MuonNoPVPrep',
                                        [ convertL0Candidates( str(self.getProp('L0SingleMuonNoPV')) )
                                          , Member ( 'TF', 'L0'
						   , FilterDescriptor = [ 'PT0,||>,'+str(self.getProp('L0MuonNoPV_PtCut')) ]
				                   , HistoDescriptor = { 'PT0': ('Pt(L0) ',0.,5000.,100),
						               'PT0Best': ('Pt(L0) Best',0.,5000.,100)}
						   ) 
                                          ] + MatchT  + TMatchV)
            # Muon Segments in Eventes witout Reconstructed Primary Vertices
            MuonNoPVSegPrep = bindMembers ('MuonNoPVSegPrep' ,
                                           [ MuonNoPVPrep
                                             , RecoMuonSeg
                                             , Member ('TF', 'PrepMuSeg'
                                                      , RequirePositiveInputs = False
                                                      , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuSeg.getDefaults('OutputMuonTracksName')
                                                      , FilterDescriptor = [ 'DoShareM3_'+MuonNoPVPrep.outputSelection()+',<,'+str(self.getProp('Muon_ShareCut')) ] 
						      , HistoDescriptor = { 'DoShareM3_Hlt1MuonNoPVPrepTMVeloT': ('Shared Hits',0,5,6),
							   'DoShareM3_Hlt1MuonNoPVPrepTMVeloTBest': ('Shared Hits Best',0,5,6)}
						      )
                                             ] + MatchT + TMatchV 
                                           )
        
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            # Muons with GEC
            MuonGECPrep = bindMembers( 'MuonGECPrep',
                                       [ convertL0Candidates( str(self.getProp('L0SingleMuonGEC')) )
                                         ] + MatchT + TMatchV )
            # Muon Segments with GEC
            MuonGECSegPrep = bindMembers ('MuonGECSegPrep' ,
                                          [ MuonGECPrep
                                            , RecoMuonSeg
                                            , Member ('TF', 'PrepMuSeg'
                                                      , RequirePositiveInputs = False
                                                      , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuSeg.getDefaults('OutputMuonTracksName')
                                                      , FilterDescriptor = [ 'DoShareM3_'+MuonGECPrep.outputSelection()+',<,'+str(self.getProp('Muon_ShareCut')) ] )
                                            ] + MatchT + TMatchV
                                          )
        # Di Muons 
        if self.getProp('L0DiMuon') in L0Channels() :
            DiMuonPrep = bindMembers('DiMuonPrep' ,
                                     [ convertL0Candidates( str(self.getProp('L0DiMuon')) )
                                       , Member( 'VM1', 'L0DiMuon' , FilterDescriptor = [ 'SumPT,>,'+str(self.getProp('DiMuon_SumPtCut')) ])
                                       , Member( 'VT', 'L0' )
                                       ] + MatchT + TMatchV + MakeVertex
                                     )
        # Di Muons in Events without Reconstructed Primary Vertices
        if self.getProp('L0DiMuonNoPV') in L0Channels() :
            DiMuonNoPVPrep = bindMembers('DiMuonNoPVPrep' ,
                                         [ convertL0Candidates( str(self.getProp('L0DiMuonNoPV')) )
                                           , Member( 'VM1', 'L0DiMuon' , FilterDescriptor = [ 'SumPT,>,'+str(self.getProp('DiMuonNoPV_SumPtCut')) ])
                                           , Member( 'VT', 'L0' )
                                           ] + MatchT + TMatchV + MakeVertex
                                         )
            
        #--------------------------------------------------------------------
        #### these are the lines which actually make decisions...
        #--------------------------------------------------------------------
        #--------------------------------------------------------------------
        ####                Single Muon Lines                    ####
        #--------------------------------------------------------------------
        # Single Muon without IP cut from L0Muon (MuonNoGlob)
        #--------------------------------------------------------------------
        from Hlt1Lines.Hlt1GECs import Hlt1GEC
        if self.getProp('L0SingleMuon') in L0Channels() :
            SingleMuonNoIPL0 = Line( 'SingleMuonNoIPL0'
                                     , prescale = self.prescale
                                     , L0DU = "L0_CHANNEL('%(L0SingleMuon)s')"%self.getProps()
                                     , algos = [ Hlt1GEC(),MuonPrep 
                                                 , Member( 'TF', 'PT' 
                                                           , FilterDescriptor = ['PT,>,%(Muon_PtCut)s'%self.getProps()] 
                                                           , HistoDescriptor = { 'PT': ( 'Pt',0.,7000.,100), 'PTBest': ( 'Pt Best',0.,7000.,100)}
                                                         ) 
                                                 ] + FastFitNoIP 
                                     , postscale = self.postscale
                                     )

        #--------------------------------------------------------------------
        # Single Muon high PT without IP cut from L0Muon (MuonNoGlob)
        #--------------------------------------------------------------------
        from Hlt1Lines.Hlt1GECs import Hlt1GEC
        if self.getProp('L0SingleMuonHigh') in L0Channels() :
            SingleMuonNoIPL0HighPT = Line( 'SingleMuonNoIPL0HighPT'
                                     , prescale = self.prescale
                                     , L0DU = "L0_CHANNEL('%(L0SingleMuonHigh)s')"%self.getProps()
                                     , algos = [ Hlt1GEC(),MuonPrep 
                                                 , Member( 'TF', 'PT' 
                                                           , FilterDescriptor = ['PT,>,%(Muon_highPtCut)s'%self.getProps()] 
                                                           , HistoDescriptor = { 'PT': ( 'Pt',0.,7000.,100), 'PTBest': ( 'Pt Best',0.,7000.,100)}
                                                         ) 
                                                 ] + FastFitNoIP 
                                     , postscale = self.postscale
                                     )
        #--------------------------------------------------------------------
        # Single Muon without  Velo
        #--------------------------------------------------------------------
        if self.getProp('L0MuonNoVelo') in L0Channels() :
            SingleMuonNoVeloL0 = Line( 'SingleMuonNoVeloL0'
                                     , prescale = self.prescale
                                     , L0DU = "L0_CHANNEL('%(L0MuonNoVelo)s')"%self.getProps()
                                     , algos = [ Hlt1GEC(),MuonPrepT 
                                               , Member( 'TF', 'PTNV' 
                                                         , OutputSelection = '%Decision'
                                                         , FilterDescriptor = ['PT,>,%(MuonTS_PtCut)s'%self.getProps()] 
                                                         , HistoDescriptor = { 'PT': ( 'Pt',0.,7000.,100), 'PTBest': ( 'Pt Best',0.,7000.,100)}
                                                         ) 
                                               ] 
                                     , postscale = self.postscale
                                     )
        #--------------------------------------------------------------------
        # Single Muon without IP cut from L0Muon with GEC (Muon)
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            SingleMuonNoIPGEC = Line( 'SingleMuonNoIPGEC'
                                      , prescale = self.prescale
                                      , L0DU = "L0_CHANNEL('%(L0SingleMuonGEC)s')"%self.getProps()
                                      , algos = [ Hlt1GEC(),MuonGECPrep 
                                                  , Member( 'TF', 'PT' 
                                                            , FilterDescriptor = ['PT,>,%(Muon_PtCut)s'%self.getProps()] 
                                                            , HistoDescriptor = { 'PT': ( 'Pt',0.,7000.,100), 'PTBest': ( 'Pt Best',0.,7000.,100)}
                                                            ) 
                                                  ] + FastFitNoIP 
                                      , postscale = self.postscale
                                      )
        #--------------------------------------------------------------------
        # Single Muon with IP cut from L0Muon (MuonNoGlob) 
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuon') in L0Channels() :
            SingleMuonIPCL0 = Line( 'SingleMuonIPCL0'
                                    , prescale = self.prescale
                                    , L0DU = "L0_CHANNEL('%(L0SingleMuon)s')"%self.getProps()
                                    , algos = [ Hlt1GEC(),MuonPrep
                                                , PV3D().ignoreOutputSelection()
                                                , Member ( 'TF', 'PT'
                                                           , FilterDescriptor = ['PT,>,%(MuonIP_PtCut)s' %self.getProps() ]
                                                           , HistoDescriptor = { 'PT': ( 'Pt',0.,7000.,100), 'PTBest': ( 'Pt Best',0.,7000.,100)}
							   )
                                                ] + FastFitWithIP 
                                    , postscale = self.postscale
                                    ) 
        #--------------------------------------------------------------------
        # Single Muon with IP cut from L0Muon with GEC (Muon)
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            SingleMuonIPCGEC = Line( 'SingleMuonIPCGEC'
                                     , prescale = self.prescale
                                     , L0DU = "L0_CHANNEL('%(L0SingleMuonGEC)s')" %self.getProps()
                                     , algos = [ Hlt1GEC(),MuonGECPrep  
                                                 , Member ( 'TF', 'PT'
                                                            , FilterDescriptor = ['PT,>,%(MuonIP_PtCut)s' % self.getProps() ]
                                                            , HistoDescriptor = { 'PT': ( 'Pt',0.,7000.,100), 'PTBest': ( 'Pt Best',0.,7000.,100)}
							    )
                                                 ] + FastFitWithIP 
                                     , postscale = self.postscale
                                     ) 
        #--------------------------------------------------------------------
        ####         DiMuon Lines           ####
        #--------------------------------------------------------------------
        # DiMuon without IP cut from L0DiMuon (DiMuon) 
        #--------------------------------------------------------------------
        if self.getProp('L0DiMuon') in L0Channels() :
            DiMuonNoIPL0Di = Line( 'DiMuonNoIPL0Di'
                                   , prescale = self.prescale
                                   , L0DU = "L0_CHANNEL('%(L0DiMuon)s')" % self.getProps()
                                   , algos = [ Hlt1GEC(),DiMuonPrep
                                               , Member( 'VF', 'Mass' 
                                                         , FilterDescriptor = ['VertexDimuonMass,>,%(DiMuon_MassCut)s'%self.getProps()]
                                                         , HistoDescriptor = { 'VertexDimuonMass': ('Di Muon Invariant Mass',0.,5000,100),
				                                             'VertexDimuonMassBest': ('Di Muon Invariant Mass - Best',0.,5000,100) }
							 )
                                               ] + FastFitVtxNoIP
                                   , postscale = self.postscale
                                   )

        #--------------------------------------------------------------------
        # DiMuon without IP cut from L0DiMuon for B2MuMu - cut at < 4000 Velo tracks
        #--------------------------------------------------------------------
        FastFitBmm = [ PV3D().ignoreOutputSelection()
                       , Member ( 'TU' , 'FitTrack' , RecoName = FitTrack.splitName()[-1] 
                                  , HistoDescriptor = { 'FitTrackQuality': ( 'Track Fit Chi2 / ndf',0.,50.,100),
                                                        'FitTrackQualityBest': ( 'lowest track Fit Chi2 / ndf',0.,50.,100) }  
                                  )
                          , Member ( 'TF' , 'IP'
                                     ,  FilterDescriptor = ['FitIP_PV3D,||>,%(bmm_IP)s'%self.getProps()]
                                     , HistoDescriptor = {'FitIP_PV3D': ( 'muon IP to PV3D',0.,0.3,50),
                                                          'FitIP_PV3DBest': ( 'muon highest IP to PV3D',0.,0.3,50)}
                                     )
                       , Member ( 'TF' , 'Chi2Mu'
                                  , FilterDescriptor = ['FitMuChi2,<,%(bmm_MuChi2)s'%self.getProps()]
                                  , HistoDescriptor = { 'FitMuChi2': ( 'muon contribution to track fit chi2 / ndf',0.,100.,100),
                                                        'FitMuChi2Best': ( 'lowest muon contribution to track fit chi2 / ndf',0.,100.,100) } 
                                  )
                       , Member ( 'TF' , 'Chi2OverN'
                                  , FilterDescriptor = ['FitChi2OverNdf,<,%(bmm_fitChi2)s'%self.getProps()]
                                     , HistoDescriptor = { 'FitChi2OverNdf': ( 'track fit chi2 / ndf',0.,75.,100),
                                                           'FitChi2OverNdfBest': ( 'lowest track fit chi2 / ndf',0.,75.,100) } 
                                  )
                       , Member ('HltFilterFittedTracks', 'FFT',
                          OutputSelection = "%Decision",
                          InputSelection1 = '%TFChi2OverN', 
                          InputSelection2 = 'PV3D',
                          MinIPCHI2 = self.getProp("bmm_IPS")
                         )


                       
                       ]
        
        if self.getProp('L0SingleMuon4BsMuMu') in L0Channels() :
            SingleMuon4BsMuMu = Line( 'SingleMuon4BsMuMu'
                                    , prescale = self.prescale
                                    , L0DU = "L0_CHANNEL('%(L0SingleMuon4BsMuMu)s')"%self.getProps()
                                    , algos = [ MuonPrep
                                                , PV3D().ignoreOutputSelection()
                                                , Member ( 'TF', 'PT'
                                                           , FilterDescriptor = ['PT,>,%(bmm_pt)s'%self.getProps()]
                                                           , HistoDescriptor = { 'PT': ( 'Pt',0.,7000.,100), 'PTBest': ( 'Pt Best',0.,7000.,100)}
							   )
                                                ] + FastFitBmm 
                                    , postscale = self.postscale
                                    )
            

        if self.getProp('L0DiMuon') in L0Channels() :
            DiMuon4BsMuMu = Line( 'DiMuon4BsMuMu'
                                  , prescale = self.prescale
                                  , L0DU = "L0_CHANNEL('%(L0DiMuon)s')"%self.getProps()
                                  , algos = [ DiMuonPrep ]
                                  + [ Member( 'VF','Mass'
                                              , FilterDescriptor = [ 'VertexDimuonMass,>,%(bmm_dimuonMass)s'%self.getProps() ]
                                              , HistoDescriptor = { 'VertexDimuonMass': ('Di Muon Invariant Mass',4000.,10000,200),
				                         'VertexDimuonMassBest': ('Di Muon Invariant Mass - Best',4000.,10000,200) }
			           ) ]
                                  + FastFitVtxNoIP#FastFitVtxWithIP
                                  , postscale = self.postscale
                                  )
        #--------------------------------------------------------------------
        # DiMuon from L0 without Velo
        #--------------------------------------------------------------------
        if self.getProp('L0MuonNoVelo') in L0Channels() :
            DiMuonNoVeloL0 = Line( 'DiMuonNoVeloL0'
                                     , prescale = self.prescale
                                     , L0DU = "L0_CHANNEL('%(L0MuonNoVelo)s')"%self.getProps()
                                     , algos = [ Hlt1GEC(),MuonPrepT 
					         , Member( 'VM1', 'MuonT'
                                                           , OutputSelection = '%Decision'
							   , FilterDescriptor = [ 'DOCA,<,%(DiMuonTS_DOCACut)s'%self.getProps() ]
			                                   , HistoDescriptor = { 'DOCA':('DOCA',0.,1200.,100),
				                                                 'DOCABest': ('DOCA Best',0.,1200.,100)}
							   )
                                               ] 
                                     , postscale = self.postscale
                                     )
        #--------------------------------------------------------------------
        # DiMuon without IP cut from 2 L0Muon (MuonNoGlob) 
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuon') in L0Channels() :
            DiMuonNoIP2L0 = Line( 'DiMuonNoIP2L0'
                                  , prescale = self.prescale
                                  , L0DU = "L0_CHANNEL('%(L0SingleMuon)s')"%self.getProps()
                                  , algos = 
                                  [ Hlt1GEC(),MuonPrep 
                                    , AllL0MuPrep
                                    , Member( 'VM2', 'VeloT'
                                              , InputSelection1 = MuonPrep
                                              , InputSelection2 = AllL0MuPrep
                                              ,  FilterDescriptor = [ 'DOCA,<,%(DiMuon_DOCACut)s'%self.getProps() ]
			                      , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                    'DOCABest': ('DOCA Best',0.,3.,100)}
                                              )
                                    , Member( 'VF','Mass' 
                                              , FilterDescriptor = [ 'VertexDimuonMass,>,%(DiMuon_MassCut)s'%self.getProps() ]
                                              , HistoDescriptor = { 'VertexDimuonMass': ('Di Muon Invariant Mass',0.,5000,100),
				                                    'VertexDimuonMassBest': ('Di Muon Invariant Mass - Best',0.,5000,100) }
					      )
                                    ] + FastFitVtxNoIP
                                  , postscale = self.postscale
                                  )
        #--------------------------------------------------------------------
        # DiMuon without IP cut from 1 L0Muon (MuonNoGlob) and 1 L0GEC (Muon) or from 2 L0GEC (Muon) 
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            DiMuonNoIPL0GEC = Line( 'DiMuonNoIPL0GEC'
                                    , prescale = self.prescale
                                    , L0DU = "L0_CHANNEL('%(L0SingleMuonGEC)s')"%self.getProps()
                                    , algos = [Hlt1GEC(), MuonGECPrep
                                                , MuonPrep
                                                , Member( 'VM2', 'VeloT'
                                                          , InputSelection1 = MuonGECPrep
                                                          , InputSelection2 = MuonPrep
                                                          ,  FilterDescriptor = [ 'DOCA,<,%(DiMuon_DOCACut)s'%self.getProps() ]
			                                  , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                       'DOCABest': ('DOCA Best',0.,3.,100)}
                                                          )
                                                , Member( 'VF','Mass' 
                                                          , FilterDescriptor = [ 'VertexDimuonMass,>,%(DiMuon_MassCut)s'%self.getProps() ]
                                                          , HistoDescriptor = { 'VertexDimuonMass': ('Di Muon Invariant Mass',0.,5000,100),
				                                    'VertexDimuonMassBest': ('Di Muon Invariant Mass - Best',0.,5000,100) }
							  )
                                                ] + FastFitVtxNoIP
                                    , postscale = self.postscale
                                    )
        #--------------------------------------------------------------------
        # DiMuon without IP cut using L0Muon (MuonNoGlob) and 1 Muon Segment
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuon') in L0Channels() :
            DiMuonNoIPL0Seg = Line( 'DiMuonNoIPL0Seg'
                                    , prescale = self.prescale
                                    , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuon'))+"')"
                                    , algos = 
                                    [ Hlt1GEC(),MuonSegPrep
                                      , Member( 'VM2', 'VeloT'
                                                , InputSelection1 = MuonPrep
                                                , InputSelection2 = MuonSegPrep
                                                , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
			                        , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                      'DOCABest': ('DOCA Best',0.,3.,100)}
                                                #If you do not set DoMergeInputs = False it will make vertices
                                                #with 2 tracks from  InputSelection and
                                                #one track from InputSelection and another track from InputSelection2 
                                                , DoMergeInputs = False 
                                                )
                                      , Member( 'VF','Mass' 
                                                , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut')) ]
                                                , HistoDescriptor = { 'VertexDimuonMass': ('Di Muon Invariant Mass',0.,5000,100),
				                                     'VertexDimuonMassBest': ('Di Muon Invariant Mass - Best',0.,5000,100) }
                                                )
                                      ] + FastFitVtxNoIP
                                    , postscale = self.postscale
                                    )
        #--------------------------------------------------------------------
        # DiMuon without IP cut using L0Muon with GEC (Muon) and 1 Muon Segment
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            DiMuonNoIPGECSeg = Line( 'DiMuonNoIPGECSeg'
                                     , prescale = self.prescale
                                     , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuonGEC'))+"')"
                                     , algos = 
                                     [  Hlt1GEC(),MuonGECSegPrep
                                        , Member( 'VM2', 'VeloT'
                                                  , InputSelection1 = MuonGECPrep
                                                  , InputSelection2 = MuonGECSegPrep
                                                  , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
			                          , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                        'DOCABest': ('DOCA Best',0.,3.,100)}
                                                  , DoMergeInputs = False )
                                        , Member( 'VF','Mass' 
                                                  , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut')) ]
                                                  , HistoDescriptor = { 'VertexDimuonMass': ('Di Muon Invariant Mass',0.,5000,100),
				                                        'VertexDimuonMassBest': ('Di Muon Invariant Mass - Best',0.,5000,100) }
                                                  ) 
                                        ] + FastFitVtxNoIP
                                     , postscale = self.postscale
                                     )
        #--------------------------------------------------------------------
        # DiMuon with IP cut from L0DiMuon (DiMuon) 
        #--------------------------------------------------------------------
        if self.getProp('L0DiMuon') in L0Channels() :
            DiMuonIPCL0Di = Line( 'DiMuonIPCL0Di'
                                  , prescale = self.prescale
                                  , L0DU = "L0_CHANNEL('"+str(self.getProp('L0DiMuon'))+"')"
                                  , algos = [ Hlt1GEC(),DiMuonPrep ] + FastFitVtxWithIP
                                  , postscale = self.postscale
                                  )

      
        #--------------------------------------------------------------------
        # DiMuon with IP cut from 2 L0Muon (MuonNoGlob) 
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuon') in L0Channels() :
            DiMuonIPC2L0 = Line( 'DiMuonIPC2L0'
                                 , prescale = self.prescale
                                 , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuon'))+"')"
                                 , algos = 
                                 [ Hlt1GEC(),MuonPrep 
                                   , AllL0MuPrep
                                   , Member( 'VM2', 'VeloT'
                                             , InputSelection1 = MuonPrep
                                             , InputSelection2 = AllL0MuPrep
                                             ,  FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
			                     , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                   'DOCABest': ('DOCA Best',0.,3.,100)}
                                             )
                                   ] + FastFitVtxWithIP
                                 , postscale = self.postscale
                                 )
        #--------------------------------------------------------------------
        # DiMuon with IP cut from 1 L0Muon (MuonNoGlob) and 1 L0GEC (Muon) or from 2 L0GEC (Muon) 
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            DiMuonIPCL0GEC = Line( 'DiMuonIPCL0GEC'
                                   , prescale = self.prescale
                                   , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuonGEC'))+"')"
                                   , algos = 
                                   [ Hlt1GEC(),MuonGECPrep
                                     , MuonPrep
                                     , Member( 'VM2', 'VeloT'
                                               , InputSelection1 = MuonGECPrep
                                               , InputSelection2 = MuonPrep
                                               , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
			                       , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                     'DOCABest': ('DOCA Best',0.,3.,100)}
					       )
                                     ] + FastFitVtxWithIP
                                   , postscale = self.postscale
                                   )
        #--------------------------------------------------------------------
        # DiMuon with IP cut using L0Muon (MuonNoGlob) and 1 Muon Segment
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuon') in L0Channels() :
            DiMuonIPCL0Seg = Line( 'DiMuonIPCL0Seg'
                                   , prescale = self.prescale
                                   , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuon'))+"')"
                                   , algos = 
                                   [ Hlt1GEC(),MuonSegPrep
                                     , Member( 'VM2', 'VeloT'
                                               , InputSelection1 = MuonPrep
                                               , InputSelection2   = MuonSegPrep
                                               , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
			                       , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                     'DOCABest': ('DOCA Best',0.,3.,100)}
                                               , DoMergeInputs = False 
                                               )
                                     ] + FastFitVtxWithIP
                                   , postscale = self.postscale
                                   )
        #--------------------------------------------------------------------
        # DiMuon with IP cut using L0Muon with GEC (Muon) and 1 Muon Segment
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            DiMuonIPCGECSeg = Line( 'DiMuonIPCGECSeg'
                                    , prescale = self.prescale
                                    , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuonGEC'))+"')"
                                    , algos = 
                                    [ Hlt1GEC(),MuonGECSegPrep
                                      , Member( 'VM2', 'VeloT'
                                                , InputSelection1 = MuonGECPrep
                                                , InputSelection2   = MuonGECSegPrep
                                                , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
			                        , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                      'DOCABest': ('DOCA Best',0.,3.,100)}
                                                , DoMergeInputs = False 
                                                )
                                      ] + FastFitVtxWithIP
                                    , postscale = self.postscale
                                    )
        #--------------------------------------------------------------------
        # DiMuon without reconstructed Primary Vertex from L0DiMuon (DiMuon,lowMult) 
        #--------------------------------------------------------------------
        if self.getProp('L0DiMuonNoPV') in L0Channels() :
            DiMuonNoPVL0Di = Line( 'DiMuonNoPVL0Di'
                                   , prescale = self.prescale
                                   , L0DU = "L0_CHANNEL('"+str(self.getProp('L0DiMuonNoPV'))+"')"
                                   , algos = [ Hlt1GEC(),DiMuonNoPVPrep
                                               , Member( 'VF', 'Mass' 
                                                         , FilterDescriptor = ['VertexDimuonMass,>,'+str(self.getProp('DiMuonNoPV_MassCut'))]
                                                         , HistoDescriptor = { 'VertexDimuonMass': ('Di Muon Invariant Mass',0.,5000,100),
				                                               'VertexDimuonMassBest': ('Di Muon Invariant Mass - Best',0.,5000,100) }
							 )
                                               ] + FastFitVtxNoIP
                                   , postscale = self.postscale
                                   )
        #--------------------------------------------------------------------
        # DiMuon without reconstructed Primary Vertex from L0Muon (Muon,lowMult) 
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonNoPV') in L0Channels() :
            DiMuonNoPV2L0 = Line('DiMuonNoPV2L0'
                                 , prescale = self.prescale
                                 , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuonNoPV'))+"')"
                                 , algos = 
                                 [ Hlt1GEC(),MuonNoPVPrep 
                                   , Member( 'VM1', 'VeloT'
					     , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
			                     , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                   'DOCABest': ('DOCA Best',0.,3.,100)}
					   )
                                   , Member( 'VF','Mass' 
                                             , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuonNoPV_MassCut')) ]
                                             , HistoDescriptor = { 'VertexDimuonMass': ('Di Muon Invariant Mass',0.,5000,100),
				                                   'VertexDimuonMassBest': ('Di Muon Invariant Mass - Best',0.,5000,100) }
					     )
                                   ] + FastFitVtxNoIP
                                 , postscale = self.postscale
                                 )
        #--------------------------------------------------------------------
        # DiMuon without reconstructed Primary Vertex using L0Muon (Muon.lowMult) and 1 Muon Segment
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonNoPV') in L0Channels() :
            DiMuonNoPVL0Seg = Line( 'DiMuonNoPVL0Seg'
                                    , prescale = self.prescale
                                    , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuonNoPV'))+"')"
                                    , algos = 
                                    [ Hlt1GEC(),MuonNoPVSegPrep
                                      , Member( 'VM2', 'VeloT'
                                                , InputSelection1 = MuonNoPVPrep
                                                , InputSelection2   = MuonNoPVSegPrep
                                                , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
			                        , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100),
				                                      'DOCABest': ('DOCA Best',0.,3.,100)}
                                                , DoMergeInputs = False )
                                      , Member( 'VF','Mass' 
                                                , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuonNoPV_MassCut')) ]
                                                , HistoDescriptor = { 'VertexDimuonMass': ('Di Muon Invariant Mass',0.,5000,100),
				                                      'VertexDimuonMassBest': ('Di Muon Invariant Mass - Best',0.,5000,100) }
                                                ) 
                                      ] + FastFitVtxNoIP
                                    , postscale = self.postscale
                                    )
        
        #-----------------------------------------------------
        # MUON+TRACK ALLEY Lines (Antonio Perez-Calero, aperez@ecm.ub.es):
        #-----------------------------------------------------
        if self.getProp('L0SingleMuon') in L0Channels() :
            from HltTracking.Hlt1TrackUpgradeConf import Forward
            MuTrack= Line( 'MuTrack'
                           , prescale = self.prescale
                           , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuon'))+"')"
                           , algos =
                           [ Hlt1GEC(),MuonPrep
                             , Member( 'TF','MuonPt' # // Select Muons with pT
                                       , HistogramUpdatePeriod = 0
                                       , FilterDescriptor = ['PT,>,'+str(self.getProp('MuTrackMuPt'))]
                                       , HistoDescriptor = { 'PT': ( 'L0+T+Velo confirmed muon PT',0.,15000.,400),
                                                             'PTBest': ( 'L0+T+Velo confirmed muon highest PT',0.,15000.,400)}
                                       #, OutputLevel=2
                                       )
                             , PV3D().ignoreOutputSelection()  #// 2D PV only if any muon passes pt filter
                             , Member( 'TF','MuonIP' # // Select Muons with IP
                                       , HistogramUpdatePeriod = 0
                                       , FilterDescriptor = ['IP_PV3D,||>,'+str(self.getProp('MuTrackMuIP')) ]
                                       , HistoDescriptor = {'IP_PV3D': ( 'L0+T+Velo confirmed muon IP to PV3D',0.,10.,400),
                                                            'IP_PV3DBest': ( 'L0+T+Velo confirmed muon highest IP to PV3D',0.,10.,400)}
                                       #, OutputLevel=2
                                       )
                             , Velo  # Full Velo Reco 3D (already done for conf and PV3D)
                             , Member( 'TF', 'VeloCompDOCA' #  Select Velo tracks with good DOCA to Muon
                                       , HistogramUpdatePeriod = 0
                                       , FilterDescriptor = ['DOCA_%TFMuonIP,<,'+str(self.getProp('MuTrackDoca')) ]
                                       , HistoDescriptor = { 'DOCA_Hlt1MuTrackTFMuonIP': ( 'DOCA of muon and companion velo track',0.,4.,400),
                                                             'DOCA_Hlt1MuTrackTFMuonIPBest': ( 'Lowest DOCA of muon and companion velo track',0.,1.,200)}
                                       #, OutputLevel=2
                                       )
                             , Member( 'TF', 'VeloCompIP' # // Select Velo tracks with an IP 
                                       , HistogramUpdatePeriod = 0
                                       , FilterDescriptor = ['IP_PV3D,||>,'+str(self.getProp('MuTrackTrIP'))]
                                       , HistoDescriptor = { 'IP_PV3D': ( 'Velo companion track IP to PV3D',0.,10.,400),
                                                             'IP_PV3DBest': ( 'Velo companion track highest IP to PV3D',0.,10.,400)}
                                       )
                             , Member( 'VM2', 'VeloVertex' # // Make vertices with muon and VELO companion tracks, filtered by doca again
                                       , InputSelection1  = '%TFMuonIP'
                                       , InputSelection2  = '%TFVeloCompIP'
                                       , FilterDescriptor = ['DOCA,<,'+str(self.getProp('MuTrackDoca')) ]
                                       , HistogramUpdatePeriod = 0
                                       , HistoDescriptor = {  'DOCA': ('DOCA of muon and companion velo track by VM',0.,4.,400),
                                                              'DOCABest': ( 'Lowest DOCA of muon and companion velo track by VM',0.,1.,200)}
                                       )
                             , Member( 'VF', 'VeloVertex' # // Filter velo vertices in DZ. Angle cut only here to fill extra info!
                                       , FilterDescriptor = ['VertexDz_PV3D,>,'+str(self.getProp('MuTrackDZ')),
                                                             'VertexAngle,>,0.']
                                       , HistogramUpdatePeriod = 0
                                       , HistoDescriptor = { 'VertexDz_PV3D': ( 'Muon+companion velo track vertex Dz to PV3D',-20.,80.,100),
                                                             'VertexDz_PV3DBest': ( 'Highest muon+companion velo track vertex Dz to PV3D',-20.,80.,100) }
                                       )
                             , Member( 'VU', 'VForward' # // Make forward the companion velo track
                                       , RecoName = Forward.splitName()[-1]
                                       )
                             , Member( 'VF', 'VertexPt' # // Select vertices if Pt
                                       , FilterDescriptor = [ 'VertexTrack2PT,>,'+str(self.getProp('MuTrackTrPt'))]
                                       , HistogramUpdatePeriod = 0
                                       , HistoDescriptor = {'VertexTrack2PT': ('Forward companion track PT',0.,15000.,400),
                                                            'VertexTrack2PTBest': ('Forward companion track highest PT',0.,15000.,400)}
                                       )
                             , Member( 'VF', 'VertexMass' # // Select vertices if Mass
                                       , FilterDescriptor = ['VertexDimuonMass,>,'+str(self.getProp('MuTrackDimuMass'))]
                                       , HistogramUpdatePeriod = 0
                                       , HistoDescriptor = {'VertexDimuonMass': ('Muon+track pair inv. mass as dimuon',0.,10000.,200),
                                                            'VertexDimuonMassBest': ('Highest muon+track pair inv. mass as dimuon',0.,10000.,200)
                                                            }
                                       )
                             , Member( 'VF', 'VertexPoint' # // Select vertices if Pointing
                                       , FilterDescriptor = ['VertexPointing_PV3D,<,'+str(self.getProp('MuTrackPoint'))]
                                       , HistogramUpdatePeriod = 0
                                       , HistoDescriptor = { 'VertexPointing_PV3D': ( 'Muon+track pair pointing to PV3D',0.,1.,100),
                                                             'VertexPointing_PV3DBest': ( 'Lowest muon+track pair pointing to PV3D',0.,1.,100)}
                                       )
                             , Member ( 'VU', 'VTrackFit' # // Fast fit of tracks for selected vertices
                                        , RecoName = FitTrack.splitName()[-1] 
                                        )
                             , Member ( 'VF', 'TrackChi2' # // Filter on track quality (Chi2 cut taken from hadron line)
                                        , FilterDescriptor = ['FitVertexTrack1Chi2OverNdf,<,'+str(self.getProp('MuTrackMuChi2')),
                                                              'FitVertexTrack2Chi2OverNdf,<,'+str(self.getProp('MuTrackTrChi2'))]
                                        , HistogramUpdatePeriod = 0
                                        , HistoDescriptor = { 'FitVertexTrack1Chi2OverNdf': ( 'Fitted muon track Chi2OverNdf',0.,100.,100),
                                                              'FitVertexTrack1Chi2OverNdfBest': ( 'Lowest fitted muon track Chi2OverNdf',0.,100.,100),
                                                              'FitVertexTrack2Chi2OverNdf': ( 'Fitted companion track Chi2OverNdf',0.,100.,100),
                                                              'FitVertexTrack2Chi2OverNdfBest': ( 'Lowest fitted companion track Chi2OverNdf',0.,100.,100)
                                                              }
                                        )
                             , Member ( 'VF', 'Decision' # // Final filter, redo cuts with improved velo part
                                        , OutputSelection = '%Decision'
                                        , HistogramUpdatePeriod = 0
                                        , FilterDescriptor = [ 'FitVertexTrack1IP_PV3D,||>,'+str(self.getProp('MuTrackMuIP'))
                                                               ,'FitVertexTrack2IP_PV3D,||>,'+str(self.getProp('MuTrackTrIP'))
                                                               ,'FitVertexDOCA,<,%s'%self.getProp('MuTrackDoca'),
                                                               ]
                                        , HistoDescriptor = {'FitVertexTrack1IP_PV3D': ( 'Fitted muon track IP to PV3D',0.,10.,400),
                                                             'FitVertexTrack1IP_PV3DBest': ( 'Highest fitted muon track IP to PV3D',0.,10.,400),
                                                             'FitVertexTrack2IP_PV3D': ( 'Fitted companion track IP to PV3D',0.,10.,400),
                                                             'FitVertexTrack2IP_PV3DBest': ( 'Highest fitted companion track IP to PV3D',0.,10.,400),
                                                             'FitVertexDOCA': ('Fitted muon and companion track doca',0.,4.,400),
                                                             'FitVertexDOCABest': ( 'Lowest DOCA of fitted muon and companion track',0.,1.,200)}
                                        
                                        )
                             ]
                           , postscale = self.postscale
                           )
            
            
        #------------------------- Muon + Track lifetime unbiased (originally only for Jpsi->mumu )-----------------------------------
        if self.getProp('L0SingleMuon') in L0Channels() :
            MuTrack4JPsi= Line( 'MuTrack4JPsi'
                                , prescale = self.prescale
                                , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuon'))+"')"
                                , algos =
                                [ Hlt1GEC(),MuonPrep
                                  , Member( 'TF','MuonPt' # // Select Muons with pT
                                            , HistogramUpdatePeriod = 0
                                            , FilterDescriptor = ['PT,>,'+str(self.getProp('MuTrackMuPt4JPsi'))]
                                            , HistoDescriptor = { 'PT': ( 'L0+T+Velo confirmed muon PT',0.,15000.,400),
                                                                  'PTBest': ( 'L0+T+Velo confirmed muon highest PT',0.,15000.,400)}
                                            ) 
                                  , Velo
                                  , Member( 'TF', 'CompanionVelo' # // Select Velo tracks with good DOCA to Muon
                                            , HistogramUpdatePeriod = 0
                                            , FilterDescriptor = ['DOCA_%TFMuonPt,<,'+str(self.getProp('MuTrackDoca4JPsi')) ]
                                            , HistoDescriptor = { 'DOCA_Hlt1MuTrack4JPsiTFMuonPt': ( 'DOCA of muon and companion velo track',0.,4.,400),
                                                                  'DOCA_Hlt1MuTrack4JPsiTFMuonPtBest': ( 'Lowest DOCA of muon and companion velo track',0.,1.,200)}
                                            )
                                  , Member( 'VM2', 'VeloVertex' # // Make vertices with muon and VELO companion tracks, filtered by doca again
                                            , InputSelection1  = '%TFMuonPt'
                                            , InputSelection2  = '%TFCompanionVelo'
                                            , FilterDescriptor = ['DOCA,<,'+str(self.getProp('MuTrackDoca4JPsi')) ]
                                            , HistogramUpdatePeriod = 0
                                            , HistoDescriptor = {  'DOCA': ('DOCA of muon and companion velo track by VM',0.,4.,400),
                                                                   'DOCABest': ( 'Lowest DOCA of muon and companion velo track by VM',0.,1.,200)}
                                            )
                                  
                                  , Member( 'VF', 'VeloVertex' # // Filter velo vertices in angle
                                            , FilterDescriptor = ['VertexAngle,>,'+str(self.getProp('MuTrackAngle4JPsiLow')),
                                                                  'VertexAngle,<,'+str(self.getProp('MuTrackAngle4JPsiHigh'))]
                                            , HistoDescriptor = { 'VertexAngle': ( 'Angle at Velo between muon and companion velo tracks',0.,1.5,100),
                                                                  'VertexAngleBest': ( 'Best angle at Velo between muon and companion velo tracks',0.,1.5,100) }
                                            , HistogramUpdatePeriod = 0
                                            )
                                  , Member( 'VU', 'Vertex' # // Make forward the companion velo track
                                            , RecoName = Forward.splitName()[-1]
                                            )
                                  , Member( 'VF', 'VertexPt' # // Select vertices if Pt
                                            , FilterDescriptor = ['VertexTrack2PT,>,'+str(self.getProp('MuTrackTrPt4JPsi'))]
                                            , HistogramUpdatePeriod = 0
                                            , HistoDescriptor = {'VertexTrack2PT': ('Forward companion track PT',0.,15000.,400),
                                                                 'VertexTrack2PTBest': ('Forward companion track highest PT',0.,15000.,400)}
                                            )
                                  , Member( 'VF', 'VertexMass' # // Select vertices if mass
                                            , FilterDescriptor =[ 'VertexDimuonMass,>,%s' %self.getProp('MuTrackDimuMass4JPsiLow') ] + ([ 'VertexDimuonMass,<,%s' %self.getProp('MuTrackDimuMass4JPsiHigh') ] if self.getProp('MuTrackDimuMass4JPsiHigh') > 0 else [ ] 
                                                                                                                                        )
                                            , HistogramUpdatePeriod = 0
                                            , HistoDescriptor = {'VertexDimuonMass': ('Muon+track pair inv. mass as dimuon',0.,10000.,200),
                                                                 'VertexDimuonMassBest': ('Highest muon+track pair inv. mass as dimuon',0.,10000.,200)
                                                                 }
                                            )
                                  , Member ( 'VU', 'VTrackFit' # // Fast fit of tracks for selected vertices
                                             , RecoName = FitTrack.splitName()[-1]
                                             )
                                  , Member ( 'VF', 'TrackChi2' # // Filter on track quality (Chi2 cut taken from hadron line)
                                             , FilterDescriptor = ['FitVertexTrack1Chi2OverNdf,<,'+str(self.getProp('MuTrackMuChi24JPsi')),
                                                                   'FitVertexTrack2Chi2OverNdf,<,'+str(self.getProp('MuTrackTrChi24JPsi'))]
                                             , HistoDescriptor = { 'FitVertexTrack1Chi2OverNdf': ( 'Fitted muon track Chi2OverNdf',0.,100.,100),
                                                                   'FitVertexTrack1Chi2OverNdfBest': ( 'Lowest fitted muon track Chi2OverNdf',0.,100.,100),
                                                                   'FitVertexTrack2Chi2OverNdf': ( 'Fitted companion track Chi2OverNdf',0.,100.,100),
                                                                   'FitVertexTrack2Chi2OverNdfBest': ( 'Lowest fitted companion track Chi2OverNdf',0.,100.,100)
                                                                   }
                                             )
                                  , Member ( 'VF', 'Decision' # // Final filter redo cuts
                                             , OutputSelection = '%Decision'
                                             , FilterDescriptor = ['FitVertexDOCA,<,%s'%self.getProp('MuTrackDoca4JPsi')]
                                             , HistoDescriptor = {'FitVertexDOCA': ('DOCA of fitted muon and companion tracks',0.,4.,400),
                                                                  'FitVertexDOCABest': ( 'Lowest DOCA of fitted muon and companion tracks',0.,1.,200)
                                                                  }
                                             )
                                  ]
                                , postscale = self.postscale
                                )
            
            
        
