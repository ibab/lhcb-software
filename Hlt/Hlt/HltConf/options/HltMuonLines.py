#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltMuonLines.py,v 1.2 2008-09-17 19:44:40 graven Exp $
# =============================================================================
## @file
#  Configuration of Muon Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure Muon trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================

from Gaudi.Configuration import * 

from Configurables import GaudiSequencer
from Configurables import HltMuonRec
from Configurables import HltL0MuonPrepare
from Configurables import HltTrackUpgrade
from Configurables import PatMatchTool

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop 


importOptions('$HLTCONFROOT/options/TsaTool.opts')
importOptions('$HLTCONFROOT/options/HltRecoSequence.py')

decodeT = GaudiSequencer('HltDecodeT')
RZVelo  = GaudiSequencer('Hlt1RecoRZVeloTracksSequence')
RecoRZPV= GaudiSequencer('Hlt1RecoRZPVSequence')
# It already Prepare the muon segments with proper error calling the tool MuonSeedTool
RecoMuonSeg = HltMuonRec('HltRecoMuonSeg'
                        , OutputMuonTracksName='Hlt/Track/MuonSegmentForL0Single'
                        , DecodingFromCoord=True )


TConfMatchVelo = [ decodeT
                 , Member ('TU', 'TConf' , RecoName = 'TConf' )
                 , Member ('TF', 'TConf' , FilterDescriptor = ['IsMuon,>,0.'] )
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

#### lines which are shared inputs, and which do NOT make decisions

singleMuonPrep = Line( 'singleMuonPrepare' 
                   , makesDecision = False
                   , L0 = [ 'Muon','MuonNoGlob' ]
                   , algos = 
                   [ Member ( 'L0MuonPrepare' , 'MuonORMuonNoGlob'
                            , InputSelection = HltL0MuonPrepare().getDefaultProperties()['InputSelection']
                            # , InputSelection = 'TES:' + 'Trig/L0/MuonCtrl' # how to get LHCb::L0MuonCandidateLocation::Default instead?
                            , MinPt = 1300.0  
                            )
                   , Member ( 'TF', 'L0'
                            , InputSelection = 'L0AllMuons' # did we mean L0AllMuons and not the above???
                            , FilterDescriptor = [ 'PT0,||>,1300' ] )
                   ] + TConfMatchVelo 
                   )

muonSegPrep = Line ('muonSegPrepare'
                   , makesDecision = False
                   , HLT = [ singleMuonPrep ]
                   , algos = 
                   [ RecoMuonSeg
                   , Member ('TF', 'PrepareMuonSeg'
                            , RequirePositiveInputs = False
                            , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuonSeg.getDefaults('OutputMuonTracksName')
                            , FilterDescriptor = [ 'DoShareM3_'+singleMuonPrep.outputSelection()+',<,0.5' ] )
                   ] + TConfMatchVelo 
                   )

DiMuonFromL0DiMuonPrepare = Line( 'DiMuonFromL0DiMuonPrepare'
                   , makesDecision = False
                   , L0 = [ 'DiMuon' ]
                   , algos = 
                   [ HltL0MuonPrepare('L0AllMuons') # WARNING: we require dimuon, but use L0AllMuons
                   , Member( 'VM1', 'L0DiMuon'
                           , InputSelection = 'L0AllMuons'
                           , FilterDescriptor = [ 'SumPT,>,1500.' ]
                           )
                   , Member( 'VT', 'L0' )
                   , decodeT
                   , Member( 'TU', 'TConf' , RecoName = 'TConf')
                   , Member( 'TF', 'TConf' , FilterDescriptor = ['IsMuon,>,0.'])
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
                   , Member( 'VM1', 'VeloT' , FilterDescriptor   = ['DOCA,<,0.5' ])
                   , Member( 'VF', 'VeloT'
                           , FilterDescriptor = [ 'VertexMatchIDsFraction_L0DiMuonDecision,>,1.9']
                           )
                   ] )

#### these are the lines which actually make decisions...

SingleMuonNoIP = Line( 'SingleMuonNoIP'
                   ,  HLT = [ singleMuonPrep ]
                   ,  algos = 
                   [ Member( 'TF', 'Decision'
                           , InputSelection = singleMuonPrep
                           , FilterDescriptor = ['PT,>,6000']
                           )
                   ] )

### TODO: clone and mutate from line above...
### FIXME: how to add RecoRZPV??
SingleMuonIPandPT = Line ( 'SingleMuonIPandPT'
                   ,  HLT = [ singleMuonPrep ]
                   ,  algos = 
                   [  RecoRZPV
                   ,  Member ( 'TF', 'Decision'
                             ,  InputSelection = singleMuonPrep
                             ,  FilterDescriptor = [ 'IP_PV2D,||[],0.08,30.', 'PT,>,1300' ] 
                             )
                   ] )


DiMuonFromL0DiMuonNoIP = Line( 'DiMuonFromL0DiMuoNoIP'
                   ,  HLT = [ DiMuonFromL0DiMuonPrepare ]
                   ,  algos = 
                   [  Member( 'VF', 'Decision'
                            , InputSelection = DiMuonFromL0DiMuonPrepare
                            , FilterDescriptor = ['VertexDimuonMass,>,2500.']
                            )
                   ] )

### TODO: clone and mutate from line above...
### FIXME: how to remove RecoRZPV in that case??
DiMuonFromL0DiMuonIP = Line ( 'DiMuonFromL0DiMuonIP'
                   , HLT = [ DiMuonFromL0DiMuonPrepare ]
                   , algos = 
                   [ RecoRZPV
                   , Member( 'VF','Decision'
                           , InputSelection = DiMuonFromL0DiMuonPrepare
                           , FilterDescriptor = [ 'VertexDimuonMass,>,500.', 'VertexMinIP_PV2D,||>,0.15' ]
                           )
                   ] )

#-----------------------------------------------------
# DIMUON ALLEY : from L0Muon + L0Muon
#-----------------------------------------------------

DiMuonFrom2L0IP = Line ('DiMuonFrom2L0IP'
                   , HLT = [ singleMuonPrep ] 
                   , algos = 
                   [ Member( 'VM1' , 'VeloT'
                           , InputSelection = singleMuonPrep
                           , FilterDescriptor = ['DOCA,<,0.5']
                           )
                   , RecoRZPV
                   , Member( 'VF','Decision'
                           , FilterDescriptor = ['VertexDimuonMass,>,500.', 'VertexMinIP_PV2D,||>,0.15']
                           )
                   ] )

### TODO: clone and mutate from line above... 
### FIXME: how to remove RecoRZPV in that case??

DiMuonFrom2L0NoIP = Line( 'DiMuonFrom2L0NoIP'
                   , HLT = [ singleMuonPrep ] 
                   , algos = 
                   [ Member( 'VM1' , 'VeloT'
                          , InputSelection = singleMuonPrep
                          , FilterDescriptor = ['DOCA,<,0.5']
                          )
                   , Member( 'VF', 'Decision'
                           , FilterDescriptor = ['VertexDimuonMass,>,2500.']
                           )
                   ] )

#-----------------------------------------------------
# DIMUON ALLEY : from L0Muon + Muon Segment
#-----------------------------------------------------

DiMuonFromMuonSegIP  = Line( 'DiMuonFromMuonSegIP' 
                   ,  HLT = [ singleMuonPrep
                            , muonSegPrep ] # TODO: this is OR, should be AND
                                            # but OK for now: muonSegPrep requires singleMuonPrep
                   , algos =
                   [  Member ( 'VM2', 'VeloT'
                           , InputSelection1 = singleMuonPrep
                           , InputSelection2   = muonSegPrep
                           , FilterDescriptor = ['DOCA,<,0.5']
                           #If you do not set DoMergeInputs = False it will make vertices
                           #with 2 tracks from  InputSelection and
                           #one track from InputSelection and another track from InputSelection2 
                           , DoMergeInputs = False)
                   , RecoRZPV
                   , Member ( 'VF' , 'Decision'
                            , FilterDescriptor = [ 'VertexDimuonMass,>,500.'
                                                 , 'VertexMinIP_PV2D,||>,0.15'
                                                 ] )
                   ] )


### TODO: clone and mutate from line above...
### FIXME: how to remove RecoRZPV??
DiMuonFromMuonSegNoIP = Line( 'DiMuonFromMuonSegNoIP'
                   ,  HLT = [ singleMuonPrep
                            , muonSegPrep ] # TODO: this is OR, should be AND
                   , algos = 
                   [  Member( 'VM2', 'VeloT'
                            , InputSelection1 = singleMuonPrep
                            , InputSelection2   = muonSegPrep
                            , FilterDescriptor = ['DOCA,<,0.5']
                            #If you do not set DoMergeInputs = False it will make vertices
                            #with 2 tracks from  InputSelection and
                            #one track from InputSelection and another track from InputSelection2 
                            , DoMergeInputs = False )
                   , Member(  'VF', 'Decision'
                           , FilterDescriptor = ['VertexDimuonMass,>,2500.' ]
                           ) 
                   ])


#### and now the mu+had line(s)

Line( 'MuonHadron'
       , HLT = [ singleMuonPrep ]
       , algos =
       [ RecoRZPV
       , Member( 'TF','Muon' # // Select Muons with IP and pT
               , InputSelection  = singleMuonPrep
               , HistogramUpdatePeriod = 0
               , FilterDescriptor = ["PT,>,1000.", "IP_PV2D,||[],0.1,3." ]
               , HistoDescriptor = { "PT"     : ( "PT",0.,6000.,400), "PTBest" : ( "PTBest",0.,6000.,400), "IP"     : ( "IP",-1.,3.,400), "IPBest" : ( "IPBest",-1.,3.,400) }
               ) # // NEED TO CHANGE TO MAKEVERTICES TO HAVE A GOOD CONFIRMATION LOGIC
       ,  HltTrackUpgrade('Hlt1RecoVelo')
       ,  Member( 'TF', 'CompanionVelo' # // Select Velo tracks with an IP and good DOCA to Muon
                , InputSelection  = HltTrackUpgrade('Hlt1RecoVelo').OutputSelection
                , HistogramUpdatePeriod = 0
                , FilterDescriptor = ["IP_PV2D,||[],0.1,3.", "DOCA_%TFMuon,<,0.2" ]
                , HistoDescriptor = { "IP"         : ( "IP",-1.,3.,400), "IPBest"     : ( "IPBest",-1.,3.,400), "DOCA"       : ( "DOCA",0.,1.,400), "DOCABest"   : ( "DOCABest",0.,1.,400) }
                )
       ,  GaudiSequencer('HltDecodeT')
       ,  GaudiSequencer('HltDecodeTT')
       ,  Member ( 'TU', 'CompanionForward' # // Make forward the selected velo tracks
                 , RecoName = "Forward"
                 )
       ,  Member ( 'TF', 'CompanionForward' # // Select forward track with a given Pt
                 , FilterDescriptor = ["PT,>,500."]
                 , HistogramUpdatePeriod = 0
                 , HistoDescriptor = {  "PT" : ("PT",0.,6000.,100), "PTBest" : ("PTBest",0.,6000.,100) }
                 )
       ,  Member( 'VM2', 'Vertex' # // Make vertices with the forward companion tracks 
                ,InputSelection1  = "%TFMuon"
                ,InputSelection2  = "%TFCompanionForward"
                ,FilterDescriptor = ["DOCA,<,0.2" ]
                ,HistogramUpdatePeriod = 0
                ,HistoDescriptor = {  "DOCA" : ("DOCA",100,0.,1.), "DOCABest" : ( "DOCABest",100,0.,0.5) }
                )
       ,  Member( 'VF','Decision' # // select vertices if Pt, pointing, and distance
                , FilterDescriptor = ["VertexPointing_PV2D,<,0.4", "VertexDz_PV2D,>,2." ]
                , HistogramUpdatePeriod = 0
                , HistoDescriptor = { "VertexPointing"     : ( "VertexPointing",0.,1.,100), "VertexPointingBest" : ( "VertexPointingBest",0.,1.,100), "VertexDz"           : ( "VertexDz",-10.,50.,100), "VertexDzBest"       : ( "VertexDzBest",-10.,50.,100) }
                )
       ])
