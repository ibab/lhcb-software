#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltMuonLines.py,v 1.18 2009-03-16 09:56:34 depaula Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.18 $"
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer
from Configurables import HltMuonRec
from Configurables import HltTrackUpgrade

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import bindMembers
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltL0Candidates import *
from HltConf.HltFastTrackFit import setupHltFastTrackFit


class HltMuonLinesConf(LHCbConfigurableUser) :
    # steering variables
    __slots__ = { 
        #'L0MuonGEC_PtCut'          : 1300.
         'L0Muon_PtCut'             :  840.
        ,'L0MuonNoPV_PtCut'         :  800.
        ,'Muon_DeltaPCut'           :    0.
        ,'Muon_VeloTMatchCut'       :  200.
        ,'Muon_ShareCut'            :    0.5
        ,'Muon_FitChiCut'           :   50.
        ,'Muon_FitMuChi2Cut'        :   75.
        ,'Muon_PtCut'               : 6000.
        ,'MuonIP_PtCut'             : 1300. 
        ,'Muon_IPMinCut'            :    0.08
        ,'DiMuon_SumPtCut'          : 1000.
        ,'DiMuon_DOCACut'           :    0.5
        ,'DiMuon_IDCut'             :    1.9
        ,'DiMuon_MassCut'           : 2500.
        ,'DiMuonNoPV_SumPtCut'      :  200.
        ,'DiMuonNoPV_MassCut'       : 1000.
        ,'DiMuonIP_MassCut'         :  500.
        ,'DiMuon_IPCut'             :    0.15
        
        # For the Muon+Track lines
        ,'MuTrackL0DU'     :"L0_CHANNEL('MuonNoGlob')"
        #,'MuTrackL0DU'     :"L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
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
        ,'MuTrackAngle4JPsi'        : 0.3
        ,'MuTrackDimuMass4JPsiLow'  : 2900.
        ,'MuTrackDimuMass4JPsiHigh' : 3300.
        ,'MuTrackMuChi24JPsi'       : 4.
        ,'MuTrackTrChi24JPsi'       : 8.
        }


    def __apply_configuration__(self) : 
        importOptions('$HLTCONFROOT/options/TsaTool.opts')
        importOptions('$HLTCONFROOT/options/HltRecoSequence.py')

        RZVelo  = GaudiSequencer('Hlt1RecoRZVeloTracksSequence')
        RecoRZPV= GaudiSequencer('Hlt1RecoRZPVSequence')
        # It already Prepare the muon segments with proper error calling the tool MuonSeedTool
        RecoMuonSeg = HltMuonRec('HltRecoMuonSeg'
                                , OutputMuonTracksName='Hlt/Track/MuonSegmentForL0Single'
                                , DecodingFromCoord=True )

        ### Matching Confirmed T Tracks with VELO
        from Configurables import PatMatchTool
        TMatchV = [ Member ('TU', 'TConf' , RecoName = 'TConf' )
                  , Member ('TF', 'DeltaP' 
                           , FilterDescriptor = ['DeltaP,>,'+str(self.getProp('Muon_DeltaPCut')) ])
                  , RZVelo
                  , Member ('TF', 'RZVelo'
                           , InputSelection = 'RZVelo'
                           , FilterDescriptor = ['RZVeloTMatch_%TFDeltaP,||<,'+str(self.getProp('Muon_VeloTMatchCut'))] 
                           )
                  , Member ('TU', 'Velo' , RecoName = 'Velo' )
                  , Member ('TM', 'VeloT'
                           , InputSelection1 = '%TUVelo'
                           , InputSelection2 = '%TFDeltaP'
                           , MatchName = 'VeloT'
                           , tools = [ Tool( PatMatchTool, maxMatchChi2 = 6 ) ]
                           ) 
                  ]
        ### Check if 2 muons can come from a Vertex
        MakeVertex = [ Member( 'VM1', 'DOCA', FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ])
                     , Member( 'VF', 'VeloT'  
                             , FilterDescriptor = [ 'VertexMatchIDsFraction_%VM1L0DiMuon,>,'+str(self.getProp('DiMuon_IDCut')) ]) 
                     ]
        ### FastFit to improve track quality
        # single track without IP cut
        FastFitNoIP = [ Member ( 'TU' , 'FitTrack' ,      RecoName = 'FitTrack')
                      , Member ( 'TF' , 'Chi2Mu'
                               , FilterDescriptor = ["FitMuChi2,<,"+str(self.getProp('Muon_FitMuChi2Cut'))])
                      , Member ( 'TF' , 'Chi2OverN'
                               , OutputSelection = '%Decision'
                               , FilterDescriptor = ["FitChi2OverNdf,<,"+str(self.getProp('Muon_FitChiCut'))])
                      ]
        # single track with IP cut
        FastFitWithIP = [ Member ( 'TU' , 'FitTrack' ,      RecoName = 'FitTrack')
                        , Member ( 'TF' , 'IP' 
                                 ,  FilterDescriptor = ["FitIP_PV2D,||>,"+str(self.getProp('Muon_IPMinCut'))])
                        , Member ( 'TF' , 'Chi2Mu'
                                 , FilterDescriptor = ["FitMuChi2,<,"+str(self.getProp('Muon_FitMuChi2Cut'))])
                                 , Member ( 'TF' , 'Chi2OverN'
                                 , OutputSelection = '%Decision'
                                 , FilterDescriptor = ["FitChi2OverNdf,<,"+str(self.getProp('Muon_FitChiCut'))])
                        ]
        # vertex (two tracks) without IP cut
        FastFitVtxNoIP = [ Member ( 'VU' , 'FitTrack' ,      RecoName = 'FitTrack')
                         , Member ( 'VF' , 'Chi2Mu'
                                  , FilterDescriptor = ["FitVertexMaxMuChi2,<,"+str(self.getProp('Muon_FitMuChi2Cut'))])
                         , Member ( 'VF' , 'Chi2OverN'
                                  , OutputSelection = '%Decision'
                                  , FilterDescriptor = ["FitVertexMaxChi2OverNdf,<,"+str(self.getProp('Muon_FitChiCut'))])
                         ]
        # vertex (two tracks) with IP cut
        FastFitVtxWithIP = [ Member( 'VU' , 'FitTrack' ,      RecoName = 'FitTrack')
                           , Member( 'VF','IP' 
                                   , FilterDescriptor = [ 'FitVertexMinIP_PV2D,||>,'+str(self.getProp('DiMuon_IPCut')) ])
                           , Member( 'VF','Mass' 
                                   , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuonIP_MassCut')) ]
                                   )
                           , Member ( 'VF' , 'Chi2Mu'
                                    , FilterDescriptor = ["FitVertexMaxMuChi2,<,"+str(self.getProp('Muon_FitMuChi2Cut'))])
                           , Member ( 'VF' , 'Chi2OverN'
                                    , OutputSelection = '%Decision'
                                    , FilterDescriptor = ["FitVertexMaxChi2OverNdf,<,"+str(self.getProp('Muon_FitChiCut'))])
                           ]
        ###  Prepare 
        # Muons 
        MuonPrep = bindMembers( 'MuonPrep',
                              [ convertL0Candidates('MuonNoGlob')
                              , Member ( 'TF', 'L0', FilterDescriptor = [ 'PT0,||>,'+str(self.getProp('L0Muon_PtCut')) ]) 
                              ] + TMatchV )
        # Muons in Events without Reconstructed Primary Vertices
        MuonNoPVPrep = bindMembers( 'MuonNoPVPrep',
                                  [ convertL0Candidates('Muon,lowMult') 
                                  , Member ( 'TF', 'L0', FilterDescriptor = [ 'PT0,||>,'+str(self.getProp('L0MuonNoPV_PtCut')) ]) 
                                  ] + TMatchV )
        # Muons with GEC
        #MuonGECPrep = bindMembers( 'MuonGECPrep',
        #                         [ convertL0Candidates('Muon')
        #                         , Member ( 'TF', 'L0', FilterDescriptor = [ 'PT0,||>,'+str(self.getProp('L0MuonGEC_PtCut')) ]) 
        #                         ] + TMatchV )
        # Muon Segments
        MuonSegPrep = bindMembers ('MuonSegPrep' ,
                                  [ MuonPrep
                                  , RecoMuonSeg
                                  , Member ('TF', 'PrepMuSeg'
                                           , RequirePositiveInputs = False
                                           , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuSeg.getDefaults('OutputMuonTracksName')
                                           , FilterDescriptor = [ 'DoShareM3_'+MuonPrep.outputSelection()+',<,'+str(self.getProp('Muon_ShareCut')) ] )
                                  ] + TMatchV 
                                  )
        # Muon Segments in Eventes witout Reconstructed Primary Vertices
        MuonNoPVSegPrep = bindMembers ('MuonNoPVSegPrep' ,
                                      [ MuonNoPVPrep
                                      , RecoMuonSeg
                                      , Member ('TF', 'PrepMuSeg'
                                               , RequirePositiveInputs = False
                                               , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuSeg.getDefaults('OutputMuonTracksName')
                                               , FilterDescriptor = [ 'DoShareM3_'+MuonNoPVPrep.outputSelection()+',<,'+str(self.getProp('Muon_ShareCut')) ] )
                                      ] + TMatchV 
                                      )
        # Muon Segments with GEC
        #MuonGECSegPrep = bindMembers ('MuonGECSegPrep' ,
        #                             [ MuonGECPrep
        #                             , RecoMuonSeg
        #                             , Member ('TF', 'PrepMuSeg'
        #                                      , RequirePositiveInputs = False
        #                                      , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuSeg.getDefaults('OutputMuonTracksName')
        #                                      , FilterDescriptor = [ 'DoShareM3_'+MuonGECPrep.outputSelection()+',<,'+str(self.getProp('Muon_ShareCut')) ] )
        #                             ] + TMatchV 
        #                             )
        # Di Muons 
        DiMuonPrep = bindMembers('DiMuonPrep' ,
                                [ convertL0Candidates('DiMuon') 
                                , Member( 'VM1', 'L0DiMuon' , FilterDescriptor = [ 'SumPT,>,'+str(self.getProp('DiMuon_SumPtCut')) ])
                                , Member( 'VT', 'L0' )
                                ] + TMatchV + MakeVertex
                                )
        # Di Muons in Events without Reconstructed Primary Vertices
        DiMuonNoPVPrep = bindMembers('DiMuonNoPVPrep' ,
                                    [ convertL0Candidates('DiMuon,lowMult') 
                                    , Member( 'VM1', 'L0DiMuon' , FilterDescriptor = [ 'SumPT,>,'+str(self.getProp('DiMuonNoPV_SumPtCut')) ])
                                    , Member( 'VT', 'L0' )
                                    ] + TMatchV + MakeVertex
                                    )

        #--------------------------------------------------------------------
        #### these are the lines which actually make decisions...
        #--------------------------------------------------------------------
        #--------------------------------------------------------------------
        ####                Single Muon Lines                    ####
        #--------------------------------------------------------------------
        # Single Muon without IP cut from L0Muon (MuonNoGlob) 
        #--------------------------------------------------------------------
        SingleMuonNoIPL0 = Line( 'SingleMuonNoIPL0'
                               ,  L0DU = "L0_CHANNEL('MuonNoGlob')"
                               ,  algos = [ MuonPrep 
                                          , Member( 'TF', 'PT' 
                                                  , FilterDescriptor = ['PT,>,'+str(self.getProp('Muon_PtCut'))] 
                                                  ) 
                                          ] + FastFitNoIP 
                               )
        setupHltFastTrackFit('Hlt1SingleMuonNoIPL0TUFitTrack')
        #--------------------------------------------------------------------
        # Single Muon without IP cut from L0Muon with GEC (Muon)
        #--------------------------------------------------------------------
        #SingleMuonNoIPGEC = Line( 'SingleMuonNoIPGEC'
        #                        ,  L0DU = "L0_CHANNEL('Muon')"
        #                        ,  algos = [ MuonGECPrep 
        #                                   , Member( 'TF', 'PT' 
        #                                           , FilterDescriptor = ['PT,>,'+str(self.getProp('Muon_PtCut'))] 
        #                                           ) 
        #                                   ] + FastFitNoIP 
        #                        )
        #setupHltFastTrackFit('Hlt1SingleMuonNoIPGECTUFitTrack')
        #--------------------------------------------------------------------
        # Single Muon with IP cut from L0Muon (MuonNoGlob) 
        #--------------------------------------------------------------------
        SingleMuonIPCL0 = Line ( 'SingleMuonIPCL0'
                               ,  L0DU = "L0_CHANNEL('MuonNoGlob')"
                               ,  algos = [ MuonPrep
                                          , RecoRZPV
                                          ,  Member ( 'TF', 'PT'
                                                    , FilterDescriptor = ['PT,>,'+str(self.getProp('MuonIP_PtCut')) ])
                                          ] + FastFitWithIP 
                               ) 
        setupHltFastTrackFit('Hlt1SingleMuonIPCL0TUFitTrack')
        #--------------------------------------------------------------------
        # Single Muon with IP cut from L0Muon with GEC (Muon)
        #--------------------------------------------------------------------
        #SingleMuonIPCGEC = Line ( 'SingleMuonIPCGEC'
        #                        ,  L0DU = "L0_CHANNEL('Muon')"
        #                        ,  algos = [ MuonGECPrep
        #                                   , RecoRZPV
        #                                   ,  Member ( 'TF', 'PT'
        #                                             , FilterDescriptor = ['PT,>,'+str(self.getProp('MuonIP_PtCut')) ])
        #                                   ] + FastFitWithIP 
        #                        ) 
        #setupHltFastTrackFit('Hlt1SingleMuonIPCGECTUFitTrack')
        #--------------------------------------------------------------------
        ####         DiMuon Lines           ####
        #--------------------------------------------------------------------
        # DiMuon without IP cut from L0DiMuon (DiMuon) 
        #--------------------------------------------------------------------
        DiMuonNoIPL0Di = Line( 'DiMuonNoIPL0Di'
                             , L0DU = "L0_CHANNEL('DiMuon')"
                             , algos = [ DiMuonPrep
                                       , Member( 'VF', 'Mass' 
                                               , FilterDescriptor = ['VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut'))])
                                       ] + FastFitVtxNoIP
                             )
        setupHltFastTrackFit('Hlt1DiMuonNoIPL0DiVUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon without IP cut from 2 L0Muon (MuonNoGlob) 
        #--------------------------------------------------------------------
        DiMuonNoIP2L0 = Line ('DiMuonNoIP2L0'
                             ,  L0DU = "L0_CHANNEL('MuonNoGlob')"
                             , algos = 
                                     [ MuonPrep 
                                     , Member( 'VM1', 'VeloT', FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ])
                                     , Member( 'VF','Mass' 
                                             , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut')) ])
                                     ] + FastFitVtxNoIP
                             )
        setupHltFastTrackFit('Hlt1DiMuonNoIP2L0VUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon without IP cut from 1 L0Muon (MuonNoGlob) and 1 L0GEC (Muon) or from 2 L0GEC (Muon) 
        #--------------------------------------------------------------------
        #DiMuonNoIPL0GEC = Line ('DiMuonNoIPL0GEC'
        #                       ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
        #                       , algos = [ MuonGECPrep
        #                                 , MuonPrep
        #                                 , Member( 'VM2', 'VeloT'
        #                                         , InputSelection1 = MuonGECPrep
        #                                         , InputSelection2 = MuonPrep
        #                                         ,  FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
        #                                         )
        #                                 , Member( 'VF','Mass' 
        #                                         , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut')) ])
        #                                 ] + FastFitVtxNoIP
        #                       )
        #setupHltFastTrackFit('Hlt1DiMuonNoIPL0GECVUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon without IP cut using L0Muon (MuonNoGlob) and 1 Muon Segment
        #--------------------------------------------------------------------
        DiMuonNoIPL0Seg = Line( 'DiMuonNoIPL0Seg'
                          , L0DU = "L0_CHANNEL('MuonNoGlob')"
                          , algos = 
                                  [ MuonSegPrep
                                  , Member( 'VM2', 'VeloT'
                                          , InputSelection1 = MuonPrep
                                          , InputSelection2 = MuonSegPrep
                                          , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
                                          #If you do not set DoMergeInputs = False it will make vertices
                                          #with 2 tracks from  InputSelection and
                                          #one track from InputSelection and another track from InputSelection2 
                                          , DoMergeInputs = False 
                                          )
                                  , Member( 'VF','Mass' 
                                          , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut')) ]
                                          )
                                  ] + FastFitVtxNoIP
                          )
        setupHltFastTrackFit('Hlt1DiMuonNoIPL0SegVUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon without IP cut using L0Muon with GEC (Muon) and 1 Muon Segment
        #--------------------------------------------------------------------
        #DiMuonNoIPGECSeg = Line( 'DiMuonNoIPGECSeg'
        #                       ,  L0DU = "L0_CHANNEL('Muon')"
        #                       , algos = 
        #                               [  MuonGECSegPrep
        #                               , Member( 'VM2', 'VeloT'
        #                                       , InputSelection1 = MuonGECPrep
        #                                       , InputSelection2 = MuonGECSegPrep
        #                                       , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
        #                                       , DoMergeInputs = False )
        #                               , Member( 'VF','Mass' 
        #                                       , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut')) ]
        #                                       ) 
        #                               ] + FastFitVtxNoIP
        #                       )
        #setupHltFastTrackFit('Hlt1DiMuonNoIPGECSegVUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon with IP cut from L0DiMuon (DiMuon) 
        #--------------------------------------------------------------------
        DiMuonIPCL0Di = Line ( 'DiMuonIPCL0Di'
                             , L0DU = "L0_CHANNEL('DiMuon')"
                             , algos = [ DiMuonPrep
                                       , RecoRZPV
                                       ] + FastFitVtxWithIP
                             )
        setupHltFastTrackFit('Hlt1DiMuonIPCL0DiVUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon with IP cut from 2 L0Muon (MuonNoGlob) 
        #--------------------------------------------------------------------
        DiMuonIPC2L0 = Line ('DiMuonIPC2L0'
                            ,  L0DU = "L0_CHANNEL('MuonNoGlob')"
                            , algos = 
                                    [ MuonPrep 
                                    , Member( 'VM1', 'VeloT', FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ])
                                    , RecoRZPV
                                    ] + FastFitVtxWithIP
                            )
        setupHltFastTrackFit('Hlt1DiMuonIPC2L0VUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon with IP cut from 1 L0Muon (MuonNoGlob) and 1 L0GEC (Muon) or from 2 L0GEC (Muon) 
        #--------------------------------------------------------------------
        #DiMuonIPCL0GEC = Line ('DiMuonIPCL0GEC'
        #                      ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
        #                      , algos = 
        #                              [ MuonGECPrep
        #                              , MuonPrep
        #                              , Member( 'VM2', 'VeloT'
        #                                      , InputSelection1 = MuonGECPrep
        #                                      , InputSelection2 = MuonPrep
        #                                      , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ])
        #                              , RecoRZPV
        #                              ] + FastFitVtxWithIP
        #                      )
        #setupHltFastTrackFit('Hlt1DiMuonIPCL0GECVUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon with IP cut using L0Muon (MuonNoGlob) and 1 Muon Segment
        #--------------------------------------------------------------------
        DiMuonIPCL0Seg = Line( 'DiMuonIPCL0Seg'
                             ,  L0DU = "L0_CHANNEL('MuonNoGlob')"
                             , algos = 
                                     [ MuonSegPrep
                                     , Member( 'VM2', 'VeloT'
                                             , InputSelection1 = MuonPrep
                                             , InputSelection2   = MuonSegPrep
                                             , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
                                             , DoMergeInputs = False 
                                             )
                                     , RecoRZPV
                                     ] + FastFitVtxWithIP
                             )
        setupHltFastTrackFit('Hlt1DiMuonIPCL0SegVUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon with IP cut using L0Muon with GEC (Muon) and 1 Muon Segment
        #--------------------------------------------------------------------
        #DiMuonIPCGECSeg = Line( 'DiMuonIPCGECSeg'
        #                      ,  L0DU = "L0_CHANNEL('Muon')"
        #                      , algos = 
        #                              [ MuonGECSegPrep
        #                              , Member( 'VM2', 'VeloT'
        #                                      , InputSelection1 = MuonGECPrep
        #                                      , InputSelection2   = MuonGECSegPrep
        #                                      , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
        #                                      , DoMergeInputs = False 
        #                                      )
        #                              , RecoRZPV
        #                              ] + FastFitVtxWithIP
        #                      )
        #setupHltFastTrackFit('Hlt1DiMuonIPCGECSegVUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon without reconstructed Primary Vertex from L0DiMuon (DiMuon,lowMult) 
        #--------------------------------------------------------------------
        DiMuonNoPVL0Di = Line( 'DiMuonNoPVL0Di'
                             , L0DU = "L0_CHANNEL('DiMuon,lowMult')"
                             , algos = [ DiMuonNoPVPrep
                                       , Member( 'VF', 'Mass' 
                                               , FilterDescriptor = ['VertexDimuonMass,>,'+str(self.getProp('DiMuonNoPV_MassCut'))])
                                       ] + FastFitVtxNoIP
                             )
        setupHltFastTrackFit('Hlt1DiMuonNoPVL0DiVUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon without reconstructed Primary Vertex from L0Muon (Muon,lowMult) 
        #--------------------------------------------------------------------
        DiMuonNoPV2L0 = Line ('DiMuonNoPV2L0'
                             ,  L0DU = "L0_CHANNEL('Muon,lowMult')"
                             , algos = 
                                     [ MuonNoPVPrep 
                                     , Member( 'VM1', 'VeloT', FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ])
                                     , Member( 'VF','Mass' 
                                             , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuonNoPV_MassCut')) ])
                                     ] + FastFitVtxNoIP
                             )
        setupHltFastTrackFit('Hlt1DiMuonNoPV2L0VUFitTrack')
        #--------------------------------------------------------------------
        # DiMuon without reconstructed Primary Vertex using L0Muon (Muon.lowMult) and 1 Muon Segment
        #--------------------------------------------------------------------
        DiMuonNoPVL0Seg = Line( 'DiMuonNoPVL0Seg'
                              ,  L0DU = "L0_CHANNEL('Muon,lowMult')"
                              , algos = 
                                      [ MuonNoPVSegPrep
                                      , Member( 'VM2', 'VeloT'
                                              , InputSelection1 = MuonNoPVPrep
                                              , InputSelection2   = MuonNoPVSegPrep
                                              , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
                                              , DoMergeInputs = False )
                                      , Member( 'VF','Mass' 
                                              , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuonNoPV_MassCut')) ]
                                              ) 
                                      ] + FastFitVtxNoIP
                              )
        setupHltFastTrackFit('Hlt1DiMuonNoPVL0SegVUFitTrack')

        #-----------------------------------------------------
        # MUON+TRACK ALLEY Lines (Antonio Perez-Calero, aperez@ecm.ub.es):
        #-----------------------------------------------------
        
        MuTrack= Line( 'MuTrack'
                       , L0DU = str(self.getProp('MuTrackL0DU'))
                       , algos =
                       #[ SingleMuonPrep
                       [ MuonPrep
                         , Member( 'TF','MuonPt' # // Select Muons with pT
                                   , HistogramUpdatePeriod = 0
                                   , FilterDescriptor = ['PT,>,'+str(self.getProp('MuTrackMuPt'))]
                                   , HistoDescriptor = { 'PT': ( 'PT',0.,6000.,400), 'PTBest': ( 'PTBest',0.,6000.,400)}
                                   )
                         
                         , RecoRZPV #// 2D PV only if any muon passes pt filter
                         
                         , Member( 'TF','MuonIP' # // Select Muons with IP
                                   , HistogramUpdatePeriod = 0
                                   , FilterDescriptor = ['IP_PV2D,||>,'+str(self.getProp('MuTrackMuIP')) ]
                                   , HistoDescriptor = {'IP': ( 'IP',-1.,3.,400), 'IPBest': ( 'IPBest',-1.,3.,400)}
                                   ) 
                         
                         , HltTrackUpgrade('Hlt1RecoVelo') # // Velo Reco-> Can this be improved filtering 2d tracks before full 3D?
                         
                         , Member( 'TF', 'VeloCompDOCA' # // Select Velo tracks with good DOCA to Muon
                                   , HistogramUpdatePeriod = 0
                                   , FilterDescriptor = ['DOCA_%TFMuonIP,<,'+str(self.getProp('MuTrackDoca')) ]
                                   , HistoDescriptor = { 'DOCA': ( 'DOCA',0.,2.,400), 'DOCABest': ( 'DOCABest',0.,1.,400)}
                                   )

                         , Member( 'TF', 'VeloCompIP' # // Select Velo tracks with an IP 
                                   , HistogramUpdatePeriod = 0
                                   , FilterDescriptor = ['IP_PV2D,||>,'+str(self.getProp('MuTrackTrIP'))]
                                   , HistoDescriptor = { 'IP': ( 'IP',-1.,3.,400), 'IPBest': ( 'IPBest',-1.,3.,400)}
                                   )
                         
                         , Member( 'VM2', 'VeloVertex' # // Make vertices with muon and VELO companion tracks, filtered by doca again
                                   , InputSelection1  = '%TFMuonIP'
                                   , InputSelection2  = '%TFVeloCompIP'
                                   , FilterDescriptor = ['DOCA,<,'+str(self.getProp('MuTrackDoca')) ]
                                   , HistogramUpdatePeriod = 0
                                   , HistoDescriptor = {  'DOCA': ('DOCA',0.,2.,400), 'DOCABest': ( 'DOCABest',0.,1.,400)}
                                   )
                         
                         , Member( 'VF', 'VeloVertex' # // Filter velo vertices in DZ. Angle cut only here to fill extra info!
                                   , FilterDescriptor = ['VertexDz_PV2D,>,'+str(self.getProp('MuTrackDZ')),
                                                         'VertexAngle,>,0.']
                                   , HistogramUpdatePeriod = 0
                                   , HistoDescriptor = { 'VertexDz': ( 'VertexDz',-10.,50.,100), 'VertexDzBest': ( 'VertexDzBest',-10.,50.,100) }
                                   )
                         
                         , Member( 'VU', 'VForward' # // Make forward the companion velo track
                                   , RecoName = 'Forward'
                                   )
                         
                         , Member( 'VF', 'VertexPt' # // Select vertices if Pt
                                   , FilterDescriptor = ['VertexTrack1PT,>,'+str(self.getProp('MuTrackMuPt')),
                                                         'VertexTrack2PT,>,'+str(self.getProp('MuTrackTrPt'))]
                                   #, HistogramUpdatePeriod = 0
                                   #, HistoDescriptor = {'VertexTrack1PT': ('PT',0.,6000.,100), 'VertexTrack1PTBest': ('PTBest',0.,6000.,100)}
                                   )
                         
                         , Member( 'VF', 'VertexMass' # // Select vertices if Mass
                                   , FilterDescriptor = ['VertexDimuonMass,>,'+str(self.getProp('MuTrackDimuMass'))]
                                   , HistogramUpdatePeriod = 0
                                   , HistoDescriptor = {'VertexDimuonMass': ('DiMuonMass',0.,10000,200)}
                                   )
                         
                         , Member( 'VF', 'VertexPoint' # // Select vertices if Pt, pointing, and distance
                                   , FilterDescriptor = ['VertexPointing_PV2D,<,'+str(self.getProp('MuTrackPoint'))]
                                   , HistogramUpdatePeriod = 0
                                   , HistoDescriptor = { 'VertexPointing': ( 'VertexPointing',0.,1.,100), 'VertexPointingBest': ( 'VertexPointingBest',0.,1.,100)}
                                   )
                         
                         , Member ( 'VU', 'VTrackFit' # // Fast fit of tracks for selected vertices
                                    , RecoName = 'FitTrack'
                                    )
                         
                         , Member ( 'VF', 'TrackChi2' # // Filter on track quality (Chi2 cut taken from hadron line)
                                    , FilterDescriptor = ['FitVertexTrack1Chi2OverNdf,<,'+str(self.getProp('MuTrackMuChi2')),
                                                          'FitVertexTrack2Chi2OverNdf,<,'+str(self.getProp('MuTrackTrChi2'))]
                                    #, HistogramUpdatePeriod = 0
                                    #, HistoDescriptor = { 'FitVertexMaxChi2OverNdf': ( 'FitVertexMaxChi2OverNdf',0.,100.,100),
                                    #                      'FitVertexMaxChi2OverNdfBest': ( 'FitVertexMaxChi2OverNdfBest',0.,100.,100)}
                                    )
                         
                         , Member ( 'VF', 'Decision' # // Final filter, redo all cuts with improved tracking
                                    , OutputSelection = '%Decision'
                                    , FilterDescriptor = ['FitVertexTrack1IP_PV2D,||>,'+str(self.getProp('MuTrackMuIP')),
                                                          'FitVertexTrack2IP_PV2D,||>,'+str(self.getProp('MuTrackTrIP')),
                                                          'FitVertexDOCA,<,'+str(self.getProp('MuTrackDoca')),
                                                          'FitVertexAngle,>,0.',
                                                          'FitVertexDz_PV2D,>,'+str(self.getProp('MuTrackDZ')),
                                                          'FitVertexTrack1PT,>,'+str(self.getProp('MuTrackMuPt')),
                                                          'FitVertexTrack2PT,>,'+str(self.getProp('MuTrackTrPt')),
                                                          'FitVertexDimuonMass,>,'+str(self.getProp('MuTrackDimuMass')),
                                                          'FitVertexPointing_PV2D,<,'+str(self.getProp('MuTrackPoint'))
                                                          ]
                                    )
                         ]
                       )
        

        MuTrack4JPsi= Line( 'MuTrack4JPsi'
                            , L0DU = str(self.getProp('MuTrackL0DU'))
                            , algos =
                            #[ SingleMuonPrep
                            [ MuonPrep
                              , Member( 'TF','Muon' # // Select Muons with pT
                                        , HistogramUpdatePeriod = 0
                                        , FilterDescriptor = ['PT,>,'+str(self.getProp('MuTrackMuPt4JPsi'))]
                                        , HistoDescriptor = { 'PT': ( 'PT',0.,6000.,400), 'PTBest': ( 'PTBest',0.,6000.,400)}
                                        ) 
                              
                              , HltTrackUpgrade('Hlt1RecoVelo') # // Velo Reco.
                              
                              , Member( 'TF', 'CompanionVelo' # // Select Velo tracks with good DOCA to Muon
                                        , HistogramUpdatePeriod = 0
                                        , FilterDescriptor = ['DOCA_%TFMuon,<,'+str(self.getProp('MuTrackDoca4JPsi')) ]
                                        , HistoDescriptor = {'DOCA': ( 'DOCA',0.,2.,400), 'DOCABest': ( 'DOCABest',0.,1.,400)}
                                        )
                              , Member( 'VM2', 'VeloVertex' # // Make vertices with muon and VELO companion tracks, filtered by doca again
                                        , InputSelection1  = '%TFMuon'
                                        , InputSelection2  = '%TFCompanionVelo'
                                        , FilterDescriptor = ['DOCA,<,'+str(self.getProp('MuTrackDoca4JPsi')) ]
                                        , HistogramUpdatePeriod = 0
                                        , HistoDescriptor = {  'DOCA': ('DOCA',0.,2.,400), 'DOCABest': ( 'DOCABest',0.,1.,400)}
                                        )
                              
                              , Member( 'VF', 'VeloVertex' # // Filter velo vertices in DZ
                                        , FilterDescriptor = ['VertexAngle,<,'+str(self.getProp('MuTrackAngle4JPsi'))]
                                        , HistogramUpdatePeriod = 0
                                        )
                              
                              , Member( 'VU', 'Vertex' # // Make forward the companion velo track
                                        , RecoName = 'Forward'
                                        )
                              
                              , Member( 'VF', 'VertexPt' # // Select vertices if Pt
                                        , FilterDescriptor = ['VertexTrack1PT,>,'+str(self.getProp('MuTrackMuPt4JPsi')),
                                                              'VertexTrack2PT,>,'+str(self.getProp('MuTrackTrPt4JPsi'))]
                                        #, HistogramUpdatePeriod = 0
                                        #, HistoDescriptor = { 'VertexMinPT': ('PT',0.,6000.,100), 'VertexMinPTBest': ('PTBest',0.,6000.,100)}
                                        )
                              
                              , Member( 'VF', 'VertexMass' # // Select vertices if mass
                                        , FilterDescriptor = ['VertexDimuonMass,>,'+str(self.getProp('MuTrackDimuMass4JPsiLow')),
                                                              'VertexDimuonMass,<,'+str(self.getProp('MuTrackDimuMass4JPsiHigh'))
                                                              ]
                                        , HistogramUpdatePeriod = 0
                                        , HistoDescriptor = {'VertexDimuonMass': ('DiMuonMass',0.,10000,200)}
                                        )
                              
                              , Member ( 'VU', 'VTrackFit' # // Fast fit of tracks for selected vertices
                                         , RecoName = 'FitTrack'
                                         )
                              
                              , Member ( 'VF', 'TrackChi2' # // Filter on track quality (Chi2 cut taken from hadron line)
                                         , FilterDescriptor = ['FitVertexTrack1Chi2OverNdf,<,'+str(self.getProp('MuTrackMuChi24JPsi')),
                                                               'FitVertexTrack2Chi2OverNdf,<,'+str(self.getProp('MuTrackTrChi24JPsi'))]
                                         )
                              
                              , Member ( 'VF', 'Decision' # // Final filter redo cuts
                                         , OutputSelection = '%Decision'
                                         , FilterDescriptor = ['FitVertexDOCA,<,'+str(self.getProp('MuTrackDoca4JPsi')),
                                                               'FitVertexAngle,<,'+str(self.getProp('MuTrackAngle4JPsi')),
                                                               'FitVertexTrack1PT,>,'+str(self.getProp('MuTrackMuPt4JPsi')),
                                                               'FitVertexTrack2PT,>,'+str(self.getProp('MuTrackTrPt4JPsi')),
                                                               'FitVertexDimuonMass,>,'+str(self.getProp('MuTrackDimuMass4JPsiLow')),
                                                               'FitVertexDimuonMass,<,'+str(self.getProp('MuTrackDimuMass4JPsiHigh'))
                                                               ]
                                         )
                              ]
                            )
        
        setupHltFastTrackFit('Hlt1MuTrackVUVTrackFit')
        setupHltFastTrackFit('Hlt1MuTrack4JPsiVUVTrackFit')
        

        
        

