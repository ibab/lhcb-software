#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltMuonAlleyConfSequence.py,v 1.1 2008-08-23 22:59:57 graven Exp $
# =============================================================================
## @file
#  Simple script to test the class Hlt1Line
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-08-07
# =============================================================================
"""
 script to configure Muon trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1 $"
# =============================================================================

from Gaudi.Configuration import * 

from Configurables import GaudiSequencer
from Configurables import HltMuonRec
from Configurables import HltL0MuonPrepare

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop , hlt1Termini


importOptions('$HLTSYSROOT/options/TsaTool.opts')

decodeT = GaudiSequencer('HltDecodeT')
RZVelo  = GaudiSequencer('Hlt1RecoRZVeloTracksSequence')
RecoRZPV= GaudiSequencer('Hlt1RecoRZPVSequence')
# It already Prepare the muon segments with proper error calling the tool MuonSeedTool
RecoMuonSeg = HltMuonRec('HltRecoMuonSeg'
                        , OutputMuonTracksName='Hlt/Track/MuonSegmentForL0Single'
                        , DecodingFromCoord=True )

### save current known lines, so we know at the end what got added by 'us'
currentLines = hlt1Lines()

TConfMatchVelo = [ decodeT
                 , Member ('TU', 'TConf' , RecoName = 'TConf' )
                 , Member ('TF', 'TConf' , FilterDescriptor = ['IsMuon,>,0.'] )
                 , Member ('TF', 'RZVelo'
                          , InputSelection = 'RZVelo'
                          ### TODO/FIXME: make sure that we expand the % in FilterDescriptors for bound selections...
                          , FilterDescriptor = ['RZVeloTMatch_%TFTConf,||<,80.'] )
                 , Member ('TU', 'Velo' , RecoName = 'Velo' )
                 , Member ('TM', 'VeloT'
                          , InputSelection1 = '%TUVelo'
                          , InputSelection2 = '%TFTConf'
                          , MatchName = 'VeloT'
                          ) # TODO/FIXME:, PatMatchTool.maxMatchChi2 = 6 )
                 ]

muonSegPrep = Line ('muonSegPrepare'
                   , HLT = [ singleMuonPrep ]
                   , algos = 
                   [ RecoMuonSeg
                   , Member ('TF', 'PrepareMuonSeg'
                            , RequirePositiveInputs = False
                            , InputSelection = 'TES:' + RecoMuonSeg.OutputMuonTracksName # if has else RecoMuonSeg.getDi
                            # TODO/FIXME: needs the outputSelection name of singleMuonPrep..
                            , FilterDescriptor = [ 'DoShareM3_'+singleMuonPrep.outputSelection()+',<,0.5' ] )
                   ] + TConfMatchVelo 
                   )

singleMuonPrep = Line( 'singleMuonPrepare' 
                   , L0 = [ 'Muon','MuonNoGlob' ]
                   , algos = 
                   [ Member ( 'L0MuonPrepare' , 'MuonORMuonNoGlob'
                            , InputSelection = HltL0MuonPrepare().getDefaultProperties()['InputSelection']
                            # , InputSelection = 'TES:' + 'Trig/L0/MuonCtrl' # how to get LHCb::L0MuonCandidateLocation::Default instead?
                            , MinPt = 1300.0  
                            )
                   , Member ( 'TF', 'L0'
                            ,  InputSelection = 'L0AllMuons' # did we mean L0AllMuons and not the above???
                            ,  FilterDescriptor = [ 'PT0,||>,1300' ] )
                   ] + TConfMatchVelo 
                   )

SingleMuonNoIP = Line( 'SingleMuonNoIP'
                   ,  HLT = [ singleMuonPrep ]
                   ,  algos = [ Member( 'TF', 'Decision'
                            , InputSelection = singleMuonPrep
                            , FilterDescriptor = ['PT,>,6000']
                            )
                   ] )

SingleMuonIPandPT = Line ( 'SingleMuonIPandPT'
                   ,  HLT = [ singleMuonPrep ]
                   ,  algos = 
                   [  RecoRZPV
                   ,  Member ( 'TF', 'Decision'
                             ,  InputSelection = singleMuonPrep
                             ,  FilterDescriptor = [ 'IP_PV2D,||[],0.08,30.', 'PT,>,1300' ] 
                             )
                   ] )

DiMuonFromL0DiMuonPrepare = Line( 'DiMuonFromL0DiMuonPrepare'
                   , L0 = [ 'DiMuon' ]
                   , algos = 
                   [ HltL0MuonPrepare('L0AllMuons')
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
                           #TODO/FIXME, PatMatchTool.maxMatchChi2 = 6
                           )
                   , Member( 'VM1', 'VeloT' , FilterDescriptor   = ['DOCA,<,0.5' ])
                   , Member( 'VF', 'VeloT'
                           , FilterDescriptor = [ 'VertexMatchIDsFraction_L0DiMuonDecision,>,1.9']
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


############ and now the general bookkeeping ... 
addHlt1Prop ( 'FilterDescriptor'     ) 

for i in  hlt1Lines() : print i
#muonLines = ( hlt1Lines() - currentLines )

MuonAlleySequence = GaudiSequencer('HltMuonAlleySequence', ModeOR = True, ShortCircuit = False, MeasureTime = True )
for i in [ 'GaudiSequencer/Hlt1SingleMuonIPandPTSequence'
         , 'GaudiSequencer/Hlt1SingleMuonNoIPSequence'
         , 'GaudiSequencer/Hlt1DiMuonFrom2L0NoIPSequence'
         , 'GaudiSequencer/Hlt1DiMuonFrom2L0IPSequence'
         , 'GaudiSequencer/Hlt1DiMuonFromMuonSegNoIPSequence'
         , 'GaudiSequencer/Hlt1DiMuonFromMuonSegIPSequence'
         , 'GaudiSequencer/Hlt1DiMuonFromL0DiMuonNoIPSequence'
         , 'GaudiSequencer/Hlt1DiMuonFromL0DiMuonIPSequence'
         , 'HltSelectionFilter/Hlt1MuonSingleAlleysSF'
         , 'HltSelectionFilter/Hlt1MuonAlleysSF'
         , 'HltSelectionFilter/Hlt1MuonDiMuonAlleysSF'
         , 'HltSelectionFilter/Hlt1MuonDiMuonAlleysSFNoIP'
         , 'HltSelectionFilter/Hlt1MuonDiMuonAlleysSFWithIP' ] :
    MuonAlleySequence.Members.append( i )


#for i in [ p.terminus() for p in  muonLines ] :
#    HltSummaryWriter.Save.append(i)
#    if i Hlt1Muon*Decision: HltSelectionFilter('Hlt1Global').InputSelections.append( i )
#    if i Hlt1Muon*Decision: Hlt1MuonAlleysSF.InputSelections.append( i ) 
#    if i Hlt1MuonsSingle*Decision: Hlt1MuonSingleAlleysSF.InputSelections.append( i )
#    if i Hlt1MuonDiMuon*Decision: Hlt1MuonDiMuonAlleysSF.InputSelections.append(i)
#    if i Hlt1MuonDiMuon*NoIPDecision: Hlt1MuonDiMuonAlleysSFNoIP.InputSelections.append(i)
#    if i Hlt1MuonDiMuon*WithIPDecision: Hlt1MuonDiMuonAlleysSFWithIP.InputSelections.append(i)
