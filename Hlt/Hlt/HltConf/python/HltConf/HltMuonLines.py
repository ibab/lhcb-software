#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltMuonLines.py,v 1.3 2009-01-15 21:23:14 aperezca Exp $
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
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer
from Configurables import HltMuonRec
from Configurables import HltTrackUpgrade
from Configurables import PatMatchTool

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import bindMembers
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltL0Candidates import *


class HltMuonLinesConf(LHCbConfigurableUser) :
    # steering variables
    __slots__ = { }

    def __apply_configuration__(self) : 
        importOptions('$HLTCONFROOT/options/TsaTool.opts')
        importOptions('$HLTCONFROOT/options/HltRecoSequence.py')
        importOptions('$HLTCONFROOT/options/Hlt1HadFitTracks.opts') #For the fast fit a la hadron line

        RZVelo  = GaudiSequencer('Hlt1RecoRZVeloTracksSequence')
        RecoRZPV= GaudiSequencer('Hlt1RecoRZPVSequence')
        # It already Prepare the muon segments with proper error calling the tool MuonSeedTool
        RecoMuonSeg = HltMuonRec('HltRecoMuonSeg'
                                , OutputMuonTracksName='Hlt/Track/MuonSegmentForL0Single'
                                , DecodingFromCoord=True )


        TConfMatchVelo = [ Member ('TU', 'TConf' , RecoName = 'TConf' )
                         , Member ('TF', 'TConf' , FilterDescriptor = ['DeltaP,>,0.','IsMuon,>,0.'] )
                         , RZVelo
                         , Member ('TF', 'RZVelo'
                                  , InputSelection = 'RZVelo'
                                  , FilterDescriptor = ['RZVeloTMatch_%TFTConf,||<,80.'] )
                         , Member ('TU', 'Velo' , RecoName = 'Velo' )
                         , Member ('TM', 'VeloT'
                                  , InputSelection1 = '%TUVelo'
                                  , InputSelection2 = '%TFTConf'
                                  , MatchName = 'VeloT'
                                  , tools = [ Tool( PatMatchTool, maxMatchChi2 = 6 ) ]
                                  ) 
                         ]

        #### shared 'blocks' of members

        singleMuonPrep = bindMembers( 'SingleMuonPrep',
                           [ convertL0Candidates('AllMuon')
                           , Member ( 'TF', 'L0' , FilterDescriptor = [ 'PT0,||>,1300' ] )
                           ] + TConfMatchVelo )

        muonSegPrep = bindMembers ('MuonSegPrepare' ,
                           [ singleMuonPrep 
                           , RecoMuonSeg
                           , Member ('TF', 'PrepareMuonSeg'
                                    , RequirePositiveInputs = False
                                    , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuonSeg.getDefaults('OutputMuonTracksName')
                                    , FilterDescriptor = [ 'DoShareM3_'+singleMuonPrep.outputSelection()+',<,0.5' ] )
                           ] + TConfMatchVelo )

        ###DiMuons in Events with no Reconstructed Primary Vertices
        singleMuonPrepNoPV = bindMembers( 'SingleMuonPrepNoPV',
                             [ convertL0Candidates('AllMuon') 
                             , Member ( 'TF', 'L0' , FilterDescriptor = [ 'PT0,||>,400' ] )
                             ] + TConfMatchVelo )

        muonSegPrepNoPV = bindMembers ('MuonSegPrepareNoPV' ,
                           [ singleMuonPrepNoPV
                           , RecoMuonSeg
                           , Member ('TF', 'PrepareMuonSeg'
                                    , RequirePositiveInputs = False
                                    , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuonSeg.getDefaults('OutputMuonTracksName')
                                    , FilterDescriptor = [ 'DoShareM3_'+singleMuonPrepNoPV.outputSelection()+',<,0.5' ] )
                           ] + TConfMatchVelo )

        DiMuonFromL0DiMuonPrepare = bindMembers( 'DiMuonFromL0DiMuonPrepare',
                           [ convertL0Candidates('AllMuon') # WARNING: we require dimuon, but use L0AllMuons
                           , Member( 'VM1', 'L0DiMuon' , FilterDescriptor = [ 'SumPT,>,1500.' ])
                           , Member( 'VT', 'L0' )
                           , Member( 'TU', 'TConf' , RecoName = 'TConf')
                           , Member( 'TF', 'TConf' , FilterDescriptor = ['DeltaP,>,0.','IsMuon,>,0.'])
                           , RZVelo
                           , Member( 'TF', 'RZVelo'
                                   , InputSelection = 'RZVelo'
                                   , FilterDescriptor = ['RZVeloTMatch_%TFTConf,||<,80.']
                                   )
                           , Member( 'TU', 'Velo' , RecoName = 'Velo')
                           , Member( 'TM', 'VeloT'
                                   , InputSelection1 =  '%TUVelo'
                                   , InputSelection2 =  '%TFTConf'
                                   , MatchName = 'VeloT'
                                   , tools = [ Tool( PatMatchTool, maxMatchChi2 = 6 ) ]
                                   )
                           , Member( 'VM1', 'VeloT', FilterDescriptor = [ 'DOCA,<,0.5' ])
                           , Member( 'VF', 'VeloT',  FilterDescriptor = [ 'VertexMatchIDsFraction_%VM1L0DiMuon,>,1.9' ]) # TODO: a fraction which is larger than 1.9???
                           ] )

        DiMuonFromL0DiMuonPrepareHighIP = bindMembers( 'DiMuonFromL0DiMuonPrepareHighIP',
                           [ convertL0Candidates('AllMuon') # WARNING: we require dimuon, but use L0AllMuons
                           , Member( 'VM1', 'L0DiMuon' , FilterDescriptor = [ 'SumPT,>,500.' ])
                           , Member( 'VT', 'L0' )
                           , Member( 'TU', 'TConf' , RecoName = 'TConf')
                           , Member( 'TF', 'TConf' , FilterDescriptor = ['DeltaP,>,0.','IsMuon,>,0.'])
                           , RZVelo
                           , Member( 'TF', 'RZVelo'
                                   , InputSelection = 'RZVelo'
                                   , FilterDescriptor = ['RZVeloTMatch_%TFTConf,||<,80.']
                                   )
                           , Member( 'TU', 'Velo' , RecoName = 'Velo')
                           , Member( 'TM', 'VeloT'
                                   , InputSelection1 =  '%TUVelo'
                                   , InputSelection2 =  '%TFTConf'
                                   , MatchName = 'VeloT'
                                   , tools = [ Tool( PatMatchTool, maxMatchChi2 = 6 ) ]
                                   )
                           , Member( 'VM1', 'VeloT', FilterDescriptor = [ 'DOCA,<,0.5' ])
                           , Member( 'VF', 'VeloT',  FilterDescriptor = [ 'VertexMatchIDsFraction_%VM1L0DiMuon,>,1.9' ]) # TODO: a fraction which is larger than 1.9???
                           ] )

        DiMuonFromL0DiMuonPrepareNoPV = bindMembers( 'DiMuonFromL0DiMuonPrepareNoPV',
                           [ convertL0Candidates('AllMuon')
                           , Member( 'VM1', 'L0DiMuon' # this is the selection formery known as L0DiMuonDecision
                                   , FilterDescriptor = [ 'SumPT,>,400.' ]
                                   )
                           , Member( 'VT', 'L0' )
                           , Member( 'TU', 'TConf' , RecoName = 'TConf')
                           , Member( 'TF', 'TConf' , FilterDescriptor = ['DeltaP,>,0','IsMuon,>,0.'])
                           , RZVelo
                           , Member( 'TF', 'RZVelo'
                                   , InputSelection = 'RZVelo'
                                   , FilterDescriptor = ['RZVeloTMatch_%TFTConf,||<,80.']
                                   )
                           , Member( 'TU', 'Velo' , RecoName = 'Velo')
                           , Member( 'TM', 'VeloT'
                                   , InputSelection1 =  '%TUVelo'
                                   , InputSelection2 =  '%TFTConf'
                                   , MatchName = 'VeloT'
                                   , tools = [ Tool( PatMatchTool, maxMatchChi2 = 6 ) ]
                                   )
                           , Member( 'VM1', 'VeloT', FilterDescriptor = [ 'DOCA,<,0.5' ])
                           , Member( 'VF', 'VeloT',  FilterDescriptor = [ 'VertexMatchIDsFraction_%VM1L0DiMuon,>,1.9' ]) # TODO: a fraction which is larger than 1.9???
                           ] )

        #### these are the lines which actually make decisions...

        SingleMuonNoIP = Line( 'SingleMuonNoIP'
                           ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
                           ,  algos = 
                           [ singleMuonPrep 
                           , Member( 'TF', 'Decision' 
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['PT,>,6000']
                                   )
                           ] )

        ### TODO: clone and mutate from line above...
        ### FIXME: how to add RecoRZPV??
        SingleMuonIPandPT = Line ( 'SingleMuonIPandPT'
                           ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
                           ,  algos = 
                           [  singleMuonPrep
                           ,  RecoRZPV
                           ,  Member ( 'TF', 'Decision'
                                     , OutputSelection = '%Decision'
                                     , FilterDescriptor = [ 'IP_PV2D,||[],0.08,30.', 'PT,>,1300' ] 
                                     )
                           ] )


        DiMuonFromL0DiMuonNoIP = Line( 'DiMuonFromL0DiMuonNoIP'
                           , L0DU = "L0_CHANNEL('DiMuon')"
                           ,  algos = 
                           [ DiMuonFromL0DiMuonPrepare 
                           , Member( 'VF', 'Decision' 
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['VertexDimuonMass,>,2500.']
                                   )
                           ] )
        ### TODO: clone and mutate from line above...
        ### FIXME: how to remove RecoRZPV in that case??
        DiMuonFromL0DiMuonIP = Line ( 'DiMuonFromL0DiMuonIP'
                           , L0DU = "L0_CHANNEL('DiMuon')"
                           , algos = 
                           [ DiMuonFromL0DiMuonPrepare
                           , RecoRZPV
                           , Member( 'VF','Decision' 
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = [ 'VertexDimuonMass,>,500.', 'VertexMinIP_PV2D,||>,0.15' ]
                                   )
                           ] )

        DiMuonFromL0DiMuonNoPV = Line( 'DiMuonFromL0DiMuonNoPV'
                           , L0DU = "(L0_CHANNEL('DiMuon,lowMult'))"
                           ,  algos = 
                           [ DiMuonFromL0DiMuonPrepareNoPV
                           , Member( 'VF', 'Decision' 
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['VertexDimuonMass,>,1000.']
                                   )
                           ] )

        DiMuonFromL0DiMuonHighIP = Line ( 'DiMuonFromL0DiMuonHighIP'
                           , L0DU = "L0_CHANNEL('DiMuon')"
                           , algos = 
                           [ DiMuonFromL0DiMuonPrepareHighIP
                           , RecoRZPV
                           , Member( 'VF','Decision' 
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = [ 'VertexDimuonMass,>,1000.', 'VertexMinIP_PV2D,||>,1.' ]
                                   )
                           ] )

        #-----------------------------------------------------
        # DIMUON ALLEY : from L0Muon + L0Muon
        #-----------------------------------------------------

        DiMuonFrom2L0IP = Line ('DiMuonFrom2L0IP'
                           ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
                           , algos = 
                           [ singleMuonPrep
                           , Member( 'VM1' , 'VeloT' , FilterDescriptor = ['DOCA,<,0.5'])
                           , RecoRZPV
                           , Member( 'VF','Decision' 
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['VertexDimuonMass,>,500.', 'VertexMinIP_PV2D,||>,0.15']
                                   )
                           ] )

        DiMuonFrom2L0HighIP = Line ('DiMuonFrom2L0HighIP'
                           ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
                           , algos = 
                           [ singleMuonPrepNoPV
                           , Member( 'VM1' , 'VeloT' , FilterDescriptor = ['DOCA,<,0.5'])
                           , RecoRZPV
                           , Member( 'VF','Decision' 
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['VertexDimuonMass,>,1000.', 'VertexMinIP_PV2D,||>,1.']
                                   )
                           ] )

        ### TODO: clone and mutate from line above... 
        ### FIXME: how to remove RecoRZPV in that case??

        DiMuonFrom2L0NoIP = Line( 'DiMuonFrom2L0NoIP'
                           ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
                           , algos = 
                           [ singleMuonPrep
                           , Member( 'VM1', 'VeloT' ,   FilterDescriptor = ['DOCA,<,0.5'])
                           , Member( 'VF', 'Decision' 
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['VertexDimuonMass,>,2500.']
                                   )
                           ] )

        #-----------------------------------------------------
        # DIMUON ALLEY : from L0Muon + Muon Segment
        #-----------------------------------------------------

        DiMuonFromMuonSegIP  = Line( 'DiMuonFromMuonSegIP' 
                           ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
                           , algos = 
                           [ muonSegPrep
                           , Member ( 'VM2', 'VeloT'
                                   , InputSelection1 = singleMuonPrep
                                   , InputSelection2 = muonSegPrep
                                   , FilterDescriptor = ['DOCA,<,0.5']
                                   #If you do not set DoMergeInputs = False it will make vertices
                                   #with 2 tracks from  InputSelection and
                                   #one track from InputSelection and another track from InputSelection2 
                                   , DoMergeInputs = False)
                           , RecoRZPV
                           , Member ( 'VF' , 'Decision'
                                    , OutputSelection = '%Decision'
                                    , FilterDescriptor = [ 'VertexDimuonMass,>,500.'
                                                         , 'VertexMinIP_PV2D,||>,0.15'
                                                         ] )
                           ] )

        DiMuonFromMuonSegHighIP  = Line( 'DiMuonFromMuonSegHighIP' 
                           ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
                           , algos = 
                           [ muonSegPrepNoPV
                           , Member ( 'VM2', 'VeloT'
                                   , InputSelection1 = singleMuonPrepNoPV
                                   , InputSelection2 = muonSegPrepNoPV
                                   , FilterDescriptor = ['DOCA,<,0.5']
                                   #If you do not set DoMergeInputs = False it will make vertices
                                   #with 2 tracks from  InputSelection and
                                   #one track from InputSelection and another track from InputSelection2 
                                   , DoMergeInputs = False)
                           , RecoRZPV
                           , Member ( 'VF' , 'Decision'
                                    , OutputSelection = '%Decision'
                                    , FilterDescriptor = [ 'VertexDimuonMass,>,1000.'
                                                         , 'VertexMinIP_PV2D,||>,1.'
                                                         ] )
                           ] )

        ### TODO: clone and mutate from line above...
        ### FIXME: how to remove RecoRZPV??
        DiMuonFromMuonSegNoIP = Line( 'DiMuonFromMuonSegNoIP'
                           ,  L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
                           , algos = 
                           [ muonSegPrep
                           , Member( 'VM2', 'VeloT'
                                    , InputSelection1 = singleMuonPrep
                                    , InputSelection2   = muonSegPrep
                                    , FilterDescriptor = ['DOCA,<,0.5']
                                    #If you do not set DoMergeInputs = False it will make vertices
                                    #with 2 tracks from  InputSelection and
                                    #one track from InputSelection and another track from InputSelection2 
                                    , DoMergeInputs = False )
                           , Member(  'VF', 'Decision'
                                   , OutputSelection = '%Decision'
                                   , FilterDescriptor = ['VertexDimuonMass,>,2500.' ]
                                   ) 
                           ])

        DiMuonFromMuonSegNoPV = Line( 'DiMuonFromMuonSegNoPV'
                                  #,  L0DU = "(L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')) & ( L0_DATA('Spd(Mult)') < 20 )"
                                  ,  L0DU = "L0_CHANNEL('Muon,lowMult')"
                                  , algos = 
                                  [ muonSegPrepNoPV
                                  , Member( 'VM2', 'VeloT'
                                      , InputSelection1 = singleMuonPrepNoPV
                                      , InputSelection2   = muonSegPrepNoPV
                                      , FilterDescriptor = ['DOCA,<,0.5']
                                      #If you do not set DoMergeInputs = False it will make vertices
                                      #with 2 tracks from  InputSelection and
                                      #one track from InputSelection and another track from InputSelection2 
                                      , DoMergeInputs = False )
                                  , Member(  'VF', 'Decision'
                                     , OutputSelection = '%Decision'
                                     , FilterDescriptor = ['VertexDimuonMass,>,1000.' ]
                                     ) 
                                  ])


    
        #-----------------------------------------------------
        # MUON+TRACK ALLEY (Antonio Perez-Calero, aperez@ecm.ub.es):
        #-----------------------------------------------------


        MuonTrack= Line( 'MuonTrack'
                         , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('MuonNoGlob')"
                         , algos =
                         [ singleMuonPrep
                           , RecoRZPV
                           
                           , Member( 'TF','Muon' # // Select Muons with IP and pT
                                     , HistogramUpdatePeriod = 0
                                     , FilterDescriptor = ['PT,>,1000.', 'IP_PV2D,||>,0.025' ]
                                     , HistoDescriptor = { 'PT': ( 'PT',0.,6000.,400), 'PTBest': ( 'PTBest',0.,6000.,400),
                                                           'IP': ( 'IP',-1.,3.,400), 'IPBest': ( 'IPBest',-1.,3.,400)
                                                           }
                                     ) 

                           , HltTrackUpgrade('Hlt1RecoVelo') # // Velo Reco.
                           
                           , Member( 'TF', 'CompanionVelo' # // Select Velo tracks with an IP and good DOCA to Muon
                                     , InputSelection  = HltTrackUpgrade('Hlt1RecoVelo')
                                     , HistogramUpdatePeriod = 0
                                     , FilterDescriptor = ['IP_PV2D,||>,0.05', 'DOCA_%TFMuon,<,0.4' ]
                                     , HistoDescriptor = { 'IP': ( 'IP',-1.,3.,400), 'IPBest': ( 'IPBest',-1.,3.,400),
                                                           'DOCA': ( 'DOCA',0.,2.,400), 'DOCABest': ( 'DOCABest',0.,1.,400)
                                                           }
                                     )
                           , Member( 'VM2', 'VeloVertex' # // Make vertices with muon and VELO companion tracks, filtered by doca again
                                     , InputSelection1  = '%TFMuon'
                                     , InputSelection2  = '%TFCompanionVelo'
                                     , FilterDescriptor = ['DOCA,<,0.4' ]
                                     , HistogramUpdatePeriod = 0
                                     , HistoDescriptor = {  'DOCA': ('DOCA',0.,2.,400), 'DOCABest': ( 'DOCABest',0.,1.,400)
                                                           }
                                     )
                           
                           , Member( 'VF', 'VeloVertex' # // Filter velo vertices in DZ
                                     , InputSelection  = '%VM2VeloVertex'
                                     , FilterDescriptor = ['VertexDz_PV2D,>,2.' ]
                                     , HistogramUpdatePeriod = 0
                                     , HistoDescriptor = { 'VertexDz': ( 'VertexDz',-10.,50.,100), 'VertexDzBest': ( 'VertexDzBest',-10.,50.,100) }
                                     )
                           
                           , Member( 'VU', 'Vertex' # // Make forward the companion velo track
                                     , InputSelection  = '%VFVeloVertex'
                                     , RecoName = 'Forward'
                                     )

                           , Member( 'VF', 'Vertex' # // Select vertices if Pt, pointing, and distance
                                     , InputSelection  = '%VUVertex'
                                     , FilterDescriptor = ['VertexMinPT,>,800.','VertexPointing_PV2D,<,0.4','VertexDimuonMass,>,1000.']
                                     , HistogramUpdatePeriod = 0
                                     , HistoDescriptor = { 'VertexPointing': ( 'VertexPointing',0.,1.,100), 'VertexPointingBest': ( 'VertexPointingBest',0.,1.,100),
                                                           'VertexMinPT': ('PT',0.,6000.,100), 'VertexMinPTBest': ('PTBest',0.,6000.,100),
                                                           'VertexDimuonMass': ('DiMuonMass',0.,10000,200)
                                                           }
                                     )

                           , Member ( 'VU', 'FitVertex' # // Fast fit of tracks for selected vertices
                                      , InputSelection  = '%VFVertex'
                                      , RecoName = 'FitTrack'
                                      )

                           , Member ( 'VF', 'Decision' # // Final filter on track quality (Chi2 cut taken from hadron line)
                                      , InputSelection  = '%VUFitVertex'
                                      , OutputSelection = '%Decision'
                                      , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,10.',
                                                            'FitVertexMinIP_PV2D,||>,0.025'
                                                            ]
                                      )
                           ]
                         )


