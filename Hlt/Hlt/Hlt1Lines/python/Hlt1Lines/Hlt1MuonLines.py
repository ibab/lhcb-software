#!/usr/bin/env gaudirun.py
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.3 $"
# =============================================================================


from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *


class Hlt1MuonLinesConf(HltLinesConfigurableUser) :
    # steering variables
    __slots__ = { 
    #  Muon Lines
         'L0SingleMuon'             :"Muon"
        ,'L0SingleMuonGEC'          :"Muon" ### is that correct?
        ,'L0SingleMuonNoPV'         :"Muon,lowMult"
        ,'L0DiMuonNoPV'             :"DiMuon,lowMult"
        ,'DiMuon_SumPtCut'          : 1000.
    # DC09 and DC06
        ,'L0DiMuon'                 :"DiMuon"
        ,'L0AllMuon_PtCut'          :   80.
        ,'L0MuonNoPV_PtCut'         :  800.
        ,'Muon_DeltaPCut'           :    0.
        ,'Muon_VeloTMatchCut'       :  200.
        ,'Muon_ShareCut'            :    0.5
        ,'Muon_FitChiCut'           :   50.
        ,'Muon_FitMuChi2Cut'        :   75.
        ,'Muon_PtCut'               : 6000.
        ,'MuonIP_PtCut'             : 1300. 
        ,'Muon_IPMinCut'            :    0.08
        ,'DiMuon_DOCACut'           :    0.5
        ,'DiMuon_IDCut'             :    1.9
        ,'DiMuon_MassCut'           : 2500.
        ,'DiMuonNoPV_SumPtCut'      :  200.
        ,'DiMuonNoPV_MassCut'       : 1000.
        ,'DiMuonIP_MassCut'         :  500.
        ,'DiMuon_IPCut'             :    0.15
        
        # For the Muon+Track lines
        #,'MuTrackL0DU'     :"L0_CHANNEL('Muon')"
        #,'MuTrackL0DU'     :"L0_CHANNEL('MuonNoGlob')"
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
        ,'MuTrackAngle4JPsiLow'     : 0.02
        ,'MuTrackAngle4JPsiHigh'    : 0.30
        ,'MuTrackDimuMass4JPsiLow'  : 2900.
        ,'MuTrackDimuMass4JPsiHigh' : 3300.
        ,'MuTrackMuChi24JPsi'       : 4.
        ,'MuTrackTrChi24JPsi'       : 8.
        }


    def __apply_configuration__(self) : 
        from Configurables import HltMuonRec
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import bindMembers
        from HltLine.HltLine import Hlt1Tool   as Tool
        from Hlt1Lines.HltL0Candidates import convertL0Candidates, L0Channels
        from Hlt1Lines.HltFastTrackFit import setupHltFastTrackFit
        from HltLine.HltReco import RZVelo, PV2D, Velo

        # It already Prepare the muon segments with proper error calling the tool MuonSeedTool
        RecoMuonSeg = HltMuonRec('HltRecoMuonSeg'
                                , OutputMuonTracksName='Hlt/Track/MuonSegmentForL0Single'
                                , DecodingFromCoord=True )

        ### Matching Confirmed T Tracks with VELO
        from Configurables import PatMatchTool
        from HltLine.HltDecodeRaw import DecodeIT, DecodeTT, DecodeVELO
        TMatchV = [ DecodeIT
                  , Member ('TU', 'TConf' , RecoName = 'TMuonConf')
                  , Member ('TF', 'DeltaP' 
                           , FilterDescriptor = ['DeltaP,>,%(Muon_DeltaPCut)s' %self.getProps()])
                  , RZVelo
                  , Member ('TF', 'RZVelo'
                             , FilterDescriptor = ['RZVeloTMatch_%%TFDeltaP,||<,%(Muon_VeloTMatchCut)s'%self.getProps()] )
                  , Member ('TU', 'Velo' , RecoName = 'Velo' )
                  , DecodeTT
                  , Member ('TM', 'VeloT'
                             , InputSelection1 = '%TUVelo'
                             , InputSelection2 = '%TFDeltaP'
                             , MatchName = 'VeloT'
                             , tools = [ Tool( PatMatchTool, maxMatchChi2 = 6 ) ]
                             ) 
                  ]
        ### Check if 2 muons can come from a Vertex
        MakeVertex = [ Member( 'VM1', 'DOCA', FilterDescriptor = [ 'DOCA,<,%(DiMuon_DOCACut)s'%self.getProps() ])
                     , Member( 'VF', 'VeloT'  
                             , FilterDescriptor = [ 'VertexMatchIDsFraction_%%VM1L0DiMuon,>,%(DiMuon_IDCut)s'%self.getProps() ]) 
                     ]
        ### FastFit to improve track quality
        # single track without IP cut
        FastFitNoIP = [ Member ( 'TU' , 'FitTrack' ,      RecoName = 'FitTrack', callback = setupHltFastTrackFit)
                      , Member ( 'TF' , 'Chi2Mu'
                               , FilterDescriptor = ['FitMuChi2,<,%(Muon_FitMuChi2Cut)s'%self.getProps()])
                      , Member ( 'TF' , 'Chi2OverN'
                               , OutputSelection = '%Decision'
                               , FilterDescriptor = ['FitChi2OverNdf,<,%(Muon_FitChiCut)s'%self.getProps()])
                      ]
        # single track with IP cut
        FastFitWithIP = [ PV2D.ignoreOutputSelection()
                        , Member ( 'TU' , 'FitTrack' , RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                        , Member ( 'TF' , 'IP' 
                                 ,  FilterDescriptor = ['FitIP_PV2D,||>,'+str(self.getProp('Muon_IPMinCut'))])
                        , Member ( 'TF' , 'Chi2Mu'
                                 , FilterDescriptor = ['FitMuChi2,<,'+str(self.getProp('Muon_FitMuChi2Cut'))])
                                 , Member ( 'TF' , 'Chi2OverN'
                                 , OutputSelection = '%Decision'
                                 , FilterDescriptor = ['FitChi2OverNdf,<,'+str(self.getProp('Muon_FitChiCut'))])
                        ]
        # vertex (two tracks) without IP cut
        FastFitVtxNoIP = [ Member ( 'VU' , 'FitTrack' ,      RecoName = 'FitTrack', callback = setupHltFastTrackFit)
                         , Member ( 'VF' , 'Chi2Mu'
                                  , FilterDescriptor = ['FitVertexMaxMuChi2,<,'+str(self.getProp('Muon_FitMuChi2Cut'))])
                         , Member ( 'VF' , 'Chi2OverN'
                                  , OutputSelection = '%Decision'
                                  , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,'+str(self.getProp('Muon_FitChiCut'))])
                         ]
        # vertex (two tracks) with IP cut
        FastFitVtxWithIP = [ PV2D.ignoreOutputSelection()
                           , Member( 'VU' , 'FitTrack' ,      RecoName = 'FitTrack', callback = setupHltFastTrackFit)
                           , Member( 'VF','IP'
                                   ,  FilterDescriptor = [ 'FitVertexMinIP_PV2D,||>,'+str(self.getProp('DiMuon_IPCut')) ])
                           , Member( 'VF','Mass'
                                   , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuonIP_MassCut')) ])
                           , Member( 'VF' , 'Chi2Mu'
                                   , FilterDescriptor = ['FitVertexMaxMuChi2,<,'+str(self.getProp('Muon_FitMuChi2Cut'))])
                           , Member( 'VF' , 'Chi2OverN'
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,'+str(self.getProp('Muon_FitChiCut'))])
                           ]
        ###  Prepare 
        AllL0MuPrep = bindMembers( 'AllL0MuPrep',
                                 [ convertL0Candidates('AllMuon')
                                 , Member ( 'TF', 'L0', FilterDescriptor = [ 'PT0,||>,'+str(self.getProp('L0AllMuon_PtCut')) ]) 
                                 ] + TMatchV )

        if self.getProp('L0SingleMuon') in L0Channels() :
            # Muons 
            MuonPrep = bindMembers( 'MuonPrep',
                                  [ convertL0Candidates( str(self.getProp('L0SingleMuon')) )
                                  ] + TMatchV )
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


        # Muons in Events without Reconstructed Primary Vertices
        if self.getProp('L0SingleMuonNoPV') in L0Channels() :
            MuonNoPVPrep = bindMembers( 'MuonNoPVPrep',
                                        [ convertL0Candidates( str(self.getProp('L0SingleMuonNoPV')) )
                                          , Member ( 'TF', 'L0', FilterDescriptor = [ 'PT0,||>,'+str(self.getProp('L0MuonNoPV_PtCut')) ]) 
                                          ] + TMatchV )
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
        # Muons with GEC
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            MuonGECPrep = bindMembers( 'MuonGECPrep',
                                       [ convertL0Candidates( str(self.getProp('L0SingleMuonGEC')) )
                                         ] + TMatchV )
            # Muon Segments with GEC
            MuonGECSegPrep = bindMembers ('MuonGECSegPrep' ,
                                          [ MuonGECPrep
                                            , RecoMuonSeg
                                            , Member ('TF', 'PrepMuSeg'
                                                      , RequirePositiveInputs = False
                                                      , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuSeg.getDefaults('OutputMuonTracksName')
                                                    , FilterDescriptor = [ 'DoShareM3_'+MuonGECPrep.outputSelection()+',<,'+str(self.getProp('Muon_ShareCut')) ] )
                                            ] + TMatchV 
                                          )
        # Di Muons 
        if self.getProp('L0DiMuon') in L0Channels() :
            DiMuonPrep = bindMembers('DiMuonPrep' ,
                                    [ convertL0Candidates( str(self.getProp('L0DiMuon')) )
                                    , Member( 'VM1', 'L0DiMuon' , FilterDescriptor = [ 'SumPT,>,'+str(self.getProp('DiMuon_SumPtCut')) ])
                                    , Member( 'VT', 'L0' )
                                    ] + TMatchV + MakeVertex
                                    )
        # Di Muons in Events without Reconstructed Primary Vertices
        if self.getProp('L0DiMuonNoPV') in L0Channels() :
            DiMuonNoPVPrep = bindMembers('DiMuonNoPVPrep' ,
                                         [ convertL0Candidates( str(self.getProp('L0DiMuonNoPV')) )
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
        if self.getProp('L0SingleMuon') in L0Channels() :
            SingleMuonNoIPL0 = Line( 'SingleMuonNoIPL0'
                                     , prescale = self.prescale
                                     , L0DU = "L0_CHANNEL('%(L0SingleMuon)s')"%self.getProps()
                                     , algos = [ MuonPrep 
                                                 , Member( 'TF', 'PT' 
                                                           , FilterDescriptor = ['PT,>,%(Muon_PtCut)s'%self.getProps()] 
                                                           ) 
                                                 ] + FastFitNoIP 
                                     , postscale = self.postscale
                                     )
        #--------------------------------------------------------------------
        # Single Muon without IP cut from L0Muon with GEC (Muon)
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            SingleMuonNoIPGEC = Line( 'SingleMuonNoIPGEC'
                                      , prescale = self.prescale
                                      , L0DU = "L0_CHANNEL('%(L0SingleMuonGEC)s')"%self.getProps()
                                      , algos = [ MuonGECPrep 
                                                  , Member( 'TF', 'PT' 
                                                            , FilterDescriptor = ['PT,>,%(Muon_PtCut)s'%self.getProps()] 
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
                                    , algos = [ MuonPrep
                                                , PV2D.ignoreOutputSelection()
                                                , Member ( 'TF', 'PT'
                                                           , FilterDescriptor = ['PT,>,'+str(self.getProp('MuonIP_PtCut')) ])
                                                ] + FastFitWithIP 
                                    , postscale = self.postscale
                                    ) 
        #--------------------------------------------------------------------
        # Single Muon with IP cut from L0Muon with GEC (Muon)
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            SingleMuonIPCGEC = Line( 'SingleMuonIPCGEC'
                                     , prescale = self.prescale
                                     , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuonGEC'))+"')"
                                     , algos = [ MuonGECPrep  
                                                 , Member ( 'TF', 'PT'
                                                            , FilterDescriptor = ['PT,>,'+str(self.getProp('MuonIP_PtCut')) ])
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
                                   , L0DU = "L0_CHANNEL('"+str(self.getProp('L0DiMuon'))+"')"
                                   , algos = [ DiMuonPrep
                                               , Member( 'VF', 'Mass' 
                                                         , FilterDescriptor = ['VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut'))])
                                               ] + FastFitVtxNoIP
                                   , postscale = self.postscale
                                   )
        #--------------------------------------------------------------------
        # DiMuon without IP cut from 2 L0Muon (MuonNoGlob) 
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuon') in L0Channels() :
            DiMuonNoIP2L0 = Line( 'DiMuonNoIP2L0'
                                  , prescale = self.prescale
                                  , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuon'))+"')"
                                  , algos = 
                                  [ MuonPrep 
                                    , AllL0MuPrep
                                    , Member( 'VM2', 'VeloT'
                                              , InputSelection1 = MuonPrep
                                              , InputSelection2 = AllL0MuPrep
                                              ,  FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
                                              )
                                    , Member( 'VF','Mass' 
                                              , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut')) ])
                                    ] + FastFitVtxNoIP
                                  , postscale = self.postscale
                                  )
        #--------------------------------------------------------------------
        # DiMuon without IP cut from 1 L0Muon (MuonNoGlob) and 1 L0GEC (Muon) or from 2 L0GEC (Muon) 
        #--------------------------------------------------------------------
        if self.getProp('L0SingleMuonGEC') in L0Channels() :
            DiMuonNoIPL0GEC = Line( 'DiMuonNoIPL0GEC'
                                    , prescale = self.prescale
                                    , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuonGEC'))+"')"
                                    , algos = [ MuonGECPrep
                                                , MuonPrep
                                                , Member( 'VM2', 'VeloT'
                                                          , InputSelection1 = MuonGECPrep
                                                          , InputSelection2 = MuonPrep
                                                          ,  FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
                                                          )
                                                , Member( 'VF','Mass' 
                                                          , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut')) ])
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
                                     [  MuonGECSegPrep
                                        , Member( 'VM2', 'VeloT'
                                                  , InputSelection1 = MuonGECPrep
                                                  , InputSelection2 = MuonGECSegPrep
                                                  , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
                                                  , DoMergeInputs = False )
                                        , Member( 'VF','Mass' 
                                                  , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuon_MassCut')) ]
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
                                  , algos = [ DiMuonPrep ] + FastFitVtxWithIP
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
                                 [ MuonPrep 
                                   , AllL0MuPrep
                                   , Member( 'VM2', 'VeloT'
                                                 , InputSelection1 = MuonPrep
                                             , InputSelection2 = AllL0MuPrep
                                             ,  FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
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
                                   [ MuonGECPrep
                                     , MuonPrep
                                     , Member( 'VM2', 'VeloT'
                                               , InputSelection1 = MuonGECPrep
                                               , InputSelection2 = MuonPrep
                                               , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ])
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
                                   [ MuonSegPrep
                                     , Member( 'VM2', 'VeloT'
                                               , InputSelection1 = MuonPrep
                                               , InputSelection2   = MuonSegPrep
                                               , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
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
                                    [ MuonGECSegPrep
                                      , Member( 'VM2', 'VeloT'
                                                , InputSelection1 = MuonGECPrep
                                                , InputSelection2   = MuonGECSegPrep
                                                , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ]
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
                                   , algos = [ DiMuonNoPVPrep
                                               , Member( 'VF', 'Mass' 
                                                         , FilterDescriptor = ['VertexDimuonMass,>,'+str(self.getProp('DiMuonNoPV_MassCut'))])
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
                                 [ MuonNoPVPrep 
                                   , Member( 'VM1', 'VeloT', FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('DiMuon_DOCACut')) ])
                                   , Member( 'VF','Mass' 
                                             , FilterDescriptor = [ 'VertexDimuonMass,>,'+str(self.getProp('DiMuonNoPV_MassCut')) ])
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
                                    , postscale = self.postscale
                                    )
        
        #-----------------------------------------------------
        # MUON+TRACK Lines (Antonio Perez-Calero, aperez@ecm.ub.es):
        #-----------------------------------------------------
        
        if self.getProp('L0SingleMuon') in L0Channels() :
            MuTrack= Line( 'MuTrack'
                           , prescale = self.prescale
                           , L0DU = "L0_CHANNEL('%s')" % self.getProp('L0SingleMuon')
                           , algos =
                           [ MuonPrep
                             , Member( 'TF','MuonPt' # // Select Muons with pT
                                       , HistogramUpdatePeriod = 0
                                       , FilterDescriptor = ['PT,>,'+str(self.getProp('MuTrackMuPt'))]
                                       , HistoDescriptor = { 'PT': ( 'PT',0.,6000.,400), 'PTBest': ( 'PTBest',0.,6000.,400)}
                                       )
                             , PV2D.ignoreOutputSelection()  #// 2D PV only if any muon passes pt filter 
                             , Member( 'TF','MuonIP' # // Select Muons with IP
                                       , HistogramUpdatePeriod = 0
                                       , FilterDescriptor = ['IP_PV2D,||>,'+str(self.getProp('MuTrackMuIP')) ]
                                       , HistoDescriptor = {'IP': ( 'IP',-1.,3.,400), 'IPBest': ( 'IPBest',-1.,3.,400)}
                                       )
                             , Velo  # Velo Reco-> Can this be improved filtering 2d tracks before full 3D?
                             , Member( 'TF', 'VeloCompDOCA' #  Select Velo tracks with good DOCA to Muon
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
                                      , RecoName = 'FitTrack' , callback = setupHltFastTrackFit
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
                           , postscale = self.postscale
                           )

        
        #------------------------- TESTING FOR IMPROVED TIMING------------------------------------------
        
            MuTrackFitMu= Line( 'MuTrackFitMu'
                                , prescale = self.prescale
                                , L0DU = "L0_CHANNEL('%(L0SingleMuon)s')" % self.getProps()
                                , algos =
                                [ MuonPrep
                                  , Member( 'TF','MuonPt' # // Select Muons with pT
                                            , HistogramUpdatePeriod = 0
                                            , FilterDescriptor = ['PT,>,%(MuTrackMuPt)s'%self.getProps()]
                                            , HistoDescriptor = { 'PT': ( 'PT',0.,6000.,400), 'PTBest': ( 'PTBest',0.,6000.,400)}
                                            )
                                  , PV2D.ignoreOutputSelection()  #// 2D PV only if any muon passes pt filter 
                                  , Member( 'TF','MuonIP' # // Select Muons with IP
                                            , HistogramUpdatePeriod = 0
                                            , FilterDescriptor = ['IP_PV2D,||>,%(MuTrackMuIP)s'%self.getProps() ]
                                            , HistoDescriptor = {'IP': ( 'IP',-1.,3.,400), 'IPBest': ( 'IPBest',-1.,3.,400)}
                                            )
                             
                                  , Member ( 'TU' , 'MuonFit' ,  RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                                  , Member ( 'TF', 'MuonChi2' , FilterDescriptor = ['FitChi2OverNdf,<,'+str(self.getProp('MuTrackMuChi2'))])
                                  , Member ( 'TF', 'MuonFitIPpT' , FilterDescriptor = ['IP_PV2D,||>,'+str(self.getProp('MuTrackMuIP')),
                                                                                       'PT,>,'+str(self.getProp('MuTrackMuPt'))
                                                                                       ]
                                             )
                                  
                                  , Velo  # Velo Reco-> Can this be improved filtering 2d tracks before full 3D?
                                  , Member( 'TF', 'VeloCompDOCA' #  Select Velo tracks with good DOCA to Muon
                                            , HistogramUpdatePeriod = 0
                                            , FilterDescriptor = ['DOCA_%TFMuonFitIPpT,<,'+str(self.getProp('MuTrackDoca')) ]
                                            , HistoDescriptor = { 'DOCA': ( 'DOCA',0.,2.,400), 'DOCABest': ( 'DOCABest',0.,1.,400)}
                                            )
                                  , Member( 'TF', 'VeloCompIP' # // Select Velo tracks with an IP 
                                            , HistogramUpdatePeriod = 0
                                            , FilterDescriptor = ['IP_PV2D,||>,'+str(self.getProp('MuTrackTrIP'))]
                                            , HistoDescriptor = { 'IP': ( 'IP',-1.,3.,400), 'IPBest': ( 'IPBest',-1.,3.,400)}
                                            )
                                  , Member( 'VM2', 'VeloVertex' # // Make vertices with muon and VELO companion tracks, filtered by doca again
                                            , InputSelection1  = '%TFMuonFitIPpT'
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
                                             , RecoName = 'FitTrack', callback = setupHltFastTrackFit
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
                                , postscale = self.postscale
                                )

        #------------------------- Muon + Track for JPsi lifetime unbiased ---------------------------------------

            MuTrack4JPsi= Line( 'MuTrack4JPsi'
                                , prescale = self.prescale
                                #, L0DU = str(self.getProp('MuTrackL0DU'))
                                , L0DU = "L0_CHANNEL('%s')" % self.getProp('L0SingleMuon')
                                , algos =
                                #[ SingleMuonPrep
                                [ MuonPrep
                                  , Member( 'TF','Muon' # // Select Muons with pT
                                            , HistogramUpdatePeriod = 0
                                            , FilterDescriptor = ['PT,>,'+str(self.getProp('MuTrackMuPt4JPsi'))]
                                            , HistoDescriptor = { 'PT': ( 'PT',0.,6000.,400), 'PTBest': ( 'PTBest',0.,6000.,400)}
                                            ) 
                                  , Velo
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
                                  
                                  , Member( 'VF', 'VeloVertex' # // Filter velo vertices in angle
                                            , FilterDescriptor = ['VertexAngle,>,'+str(self.getProp('MuTrackAngle4JPsiLow')),
                                                                  'VertexAngle,<,'+str(self.getProp('MuTrackAngle4JPsiHigh'))]
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
                                             , RecoName = 'FitTrack', callback = setupHltFastTrackFit
                                             )
                                  , Member ( 'VF', 'TrackChi2' # // Filter on track quality (Chi2 cut taken from hadron line)
                                             , FilterDescriptor = ['FitVertexTrack1Chi2OverNdf,<,'+str(self.getProp('MuTrackMuChi24JPsi')),
                                                                   'FitVertexTrack2Chi2OverNdf,<,'+str(self.getProp('MuTrackTrChi24JPsi'))]
                                             )
                                  , Member ( 'VF', 'Decision' # // Final filter redo cuts
                                             , OutputSelection = '%Decision'
                                             , FilterDescriptor = ['FitVertexDOCA,<,'+str(self.getProp('MuTrackDoca4JPsi')),
                                                                   'FitVertexAngle,>,'+str(self.getProp('MuTrackAngle4JPsiLow')),
                                                                   'FitVertexAngle,<,'+str(self.getProp('MuTrackAngle4JPsiHigh')),
                                                                   'FitVertexTrack1PT,>,'+str(self.getProp('MuTrackMuPt4JPsi')),
                                                                   'FitVertexTrack2PT,>,'+str(self.getProp('MuTrackTrPt4JPsi')),
                                                                   'FitVertexDimuonMass,>,'+str(self.getProp('MuTrackDimuMass4JPsiLow')),
                                                                   'FitVertexDimuonMass,<,'+str(self.getProp('MuTrackDimuMass4JPsiHigh'))
                                                                   ]
                                             )
                                  ]
                                , postscale = self.postscale
                                )
        

        
        #------------------------- TESTING FOR IMPROVED TIMING------------------------------------------
        
##         MuTrack4JPsi= Line( 'MuTrackFitMu4JPsi'
##                             , prescale = self.prescale
##                             , L0DU = "L0_CHANNEL('"+str(self.getProp('L0SingleMuon'))+"')"
##                             , algos =
##                             [ MuonPrep
##                               , Member( 'TF','Muon' # // Select Muons with pT
##                                         , HistogramUpdatePeriod = 0
##                                         , FilterDescriptor = ['PT,>,'+str(self.getProp('MuTrackMuPt4JPsi'))]
##                                         , HistoDescriptor = { 'PT': ( 'PT',0.,6000.,400), 'PTBest': ( 'PTBest',0.,6000.,400)}
##                                         )

##                               , Member ( 'TU' , 'MuonFit' ,  RecoName = 'FitTrack', callback = setupHltFastTrackFit)
##                               , Member ( 'TF', 'MuonChi2' , FilterDescriptor = ['FitChi2OverNdf,<,'+str(self.getProp('MuTrackMuChi24JPsi'))])

##                               , Velo
##                               , Member( 'TF', 'CompanionVelo' # // Select Velo tracks with good DOCA to Muon
##                                         , HistogramUpdatePeriod = 0
##                                         , FilterDescriptor = ['DOCA_%TFMuonChi2,<,'+str(self.getProp('MuTrackDoca4JPsi')) ]
##                                         , HistoDescriptor = {'DOCA': ( 'DOCA',0.,2.,400), 'DOCABest': ( 'DOCABest',0.,1.,400)}
##                                         )
##                               , Member( 'VM2', 'VeloVertex' # // Make vertices with muon and VELO companion tracks, filtered by doca again
##                                         , InputSelection1  = '%TFMuonChi2'
##                                         , InputSelection2  = '%TFCompanionVelo'
##                                         , FilterDescriptor = ['DOCA,<,'+str(self.getProp('MuTrackDoca4JPsi')) ]
##                                         , HistogramUpdatePeriod = 0
##                                         , HistoDescriptor = {  'DOCA': ('DOCA',0.,2.,400), 'DOCABest': ( 'DOCABest',0.,1.,400)}
##                                         )
                              
##                               , Member( 'VF', 'VeloVertex' # // Filter velo vertices in angle
##                                         , FilterDescriptor = ['VertexAngle,>,'+str(self.getProp('MuTrackAngle4JPsiLow')),
##                                                               'VertexAngle,<,'+str(self.getProp('MuTrackAngle4JPsiHigh'))]
##                                         , HistogramUpdatePeriod = 0
##                                         )
##                               , Member( 'VU', 'Vertex' # // Make forward the companion velo track
##                                         , RecoName = 'Forward'
##                                         )
##                               , Member( 'VF', 'VertexPt' # // Select vertices if Pt
##                                         , FilterDescriptor = ['VertexTrack1PT,>,'+str(self.getProp('MuTrackMuPt4JPsi')),
##                                                               'VertexTrack2PT,>,'+str(self.getProp('MuTrackTrPt4JPsi'))]
##                                         #, HistogramUpdatePeriod = 0
##                                         #, HistoDescriptor = { 'VertexMinPT': ('PT',0.,6000.,100), 'VertexMinPTBest': ('PTBest',0.,6000.,100)}
##                                         )
##                               , Member( 'VF', 'VertexMass' # // Select vertices if mass
##                                         , FilterDescriptor = ['VertexDimuonMass,>,'+str(self.getProp('MuTrackDimuMass4JPsiLow')),
##                                                               'VertexDimuonMass,<,'+str(self.getProp('MuTrackDimuMass4JPsiHigh'))
##                                                               ]
##                                         , HistogramUpdatePeriod = 0
##                                         , HistoDescriptor = {'VertexDimuonMass': ('DiMuonMass',0.,10000,200)}
##                                         )
##                               , Member ( 'VU', 'VTrackFit' # // Fast fit of tracks for selected vertices
##                                          , RecoName = 'FitTrack', callback = setupHltFastTrackFit
##                                          )
##                               , Member ( 'VF', 'TrackChi2' # // Filter on track quality (Chi2 cut taken from hadron line)
##                                          , FilterDescriptor = ['FitVertexTrack1Chi2OverNdf,<,'+str(self.getProp('MuTrackMuChi24JPsi')),
##                                                                'FitVertexTrack2Chi2OverNdf,<,'+str(self.getProp('MuTrackTrChi24JPsi'))]
##                                          )
##                               , Member ( 'VF', 'Decision' # // Final filter redo cuts
##                                          , OutputSelection = '%Decision'
##                                          , FilterDescriptor = ['FitVertexDOCA,<,'+str(self.getProp('MuTrackDoca4JPsi')),
##                                                                'FitVertexAngle,>,'+str(self.getProp('MuTrackAngle4JPsiLow')),
##                                                                'FitVertexAngle,<,'+str(self.getProp('MuTrackAngle4JPsiHigh')),
##                                                                'FitVertexTrack1PT,>,'+str(self.getProp('MuTrackMuPt4JPsi')),
##                                                                'FitVertexTrack2PT,>,'+str(self.getProp('MuTrackTrPt4JPsi')),
##                                                                'FitVertexDimuonMass,>,'+str(self.getProp('MuTrackDimuMass4JPsiLow')),
##                                                                'FitVertexDimuonMass,<,'+str(self.getProp('MuTrackDimuMass4JPsiHigh'))
##                                                                ]
##                                          )
##                               ]
##                             , postscale = self.postscale
##                             )        
        
