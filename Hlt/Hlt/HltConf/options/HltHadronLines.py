#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltHadronLines.py,v 1.16 2008-12-18 14:00:22 graven Exp $
# =============================================================================
## @file
#  Configuration of Hadron Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure Hadron trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.16 $"
# =============================================================================

from Gaudi.Configuration import * 

from Configurables import GaudiSequencer
from Configurables import HltL0CaloPrepare
from Configurables import HadronSeedTool
from Configurables import HltTrackUpgrade

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import bindMembers
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop 
from HltConf.HltL0Candidates import *

importOptions('$HLTCONFROOT/options/HltRecoSequence.py')
importOptions('$HLTCONFROOT/options/Hlt1HadFitTracks.opts')

#L0CandidateConverter = convertL0Candidates('Hadron')
#L0Candidates = L0CandidateConverter.OutputSelection
# TODO 
#--------------------------------
#
# (J.A. Hernando 17/12/08)
# External selection to be provided
#    1) L0Hadrons (L0 Candidates above a given threshold i.e 2000 MeV)
#    2) L0HadronsDecision (L0 Candidates that fired the L0-Hadron channel)
#           This is necesary for the TOS tool
#    3) RZVelo, PV2D, Velo tracks
# Replace opts-file to set the opts for the fast fit by configurables
# Declare the knows of the alleys to the optimization program

# stearing variables
#----------------------------------
#L0HADRON = "HadronNoGlob"
L0HADRON = "Hadron"

HADL0_ETCUT = 3500.
HADL0_MINETCUT = 2500.

SINGLEHADRON_PTCUT = 5000.

HADMAIN_IPCUT = 0.1
HADMAIN_PTCUT = 2500.
HADMAIN_TRACKFITCHI2CUT = 10.

HADVERTEX_DOCACUT = 0.2
HADVERTEX_DZCUT = 0.
HADVERTEX_MINIPCUT = 0.1
HADVERTEX_MINPTCUT = 1000.
HADVERTEX_POINTINGCUT = 0.4

SOFTDIHADRON = False
HADL0_SOFTETCUT = 2500. 
HADMAIN_SOFTPTCUT = 1500. 

# depending variables
#------------------------------------
if (L0HADRON == "HadronNoGlob"): HADL0_ETCUT = 4000.

#L0ALL = "L0_CHANNEL('"+L0HADRON+"') "
#for l0channel in L0Channels():
#    L0ALL = L0ALL + " | L0_CHANNEL('"+l0channel+"') "

L0ALL = "L0_CHANNEL('"+",".join([L0HADRON]+L0Channels())+"') "

# Single Hadron Alley
#-----------------------------------
HltL0CaloPrepare('L0Hadrons').addTool( HadronSeedTool(decodeCalos = False) )


Line ( 'SingleHadron' 
       , L0DU  = "L0_CHANNEL('"+L0HADRON+"')"
       , algos = [
    HltL0CaloPrepare('L0Hadrons', CaloMakerTool="HadronSeedTool",
                     CaloType = 'Hadron',MinEt = HADL0_MINETCUT)
    , Member("TF","L0Hadrons",
             InputSelection = "L0Hadrons",
             FilterDescriptor = ["L0ET,>,"+str(HADL0_ETCUT)])
    , Member("TF","2L0Hadrons",
             InputSelection = "L0Hadrons",
             OutputSelection = "L0HadronsDecision",
             FilterDescriptor = ["L0ET,>,"+str(HADL0_ETCUT)])
    , GaudiSequencer('Hlt1RecoRZVeloSequence')
    , Member ( 'TF' ,'RZVelo'
               , InputSelection = 'RZVelo'
               , FilterDescriptor = ['Calo2DChi2_%TFL0Hadrons,<,4'])
    , Member ( 'TU', 'Velo', RecoName = 'Velo')
    , Member ( 'TF', '1Velo'
               , FilterDescriptor = [ 'IP_PV2D,||>,'+str(HADMAIN_IPCUT)])
    , Member ( 'TF', '2Velo'
               , FilterDescriptor = [ 'Calo3DChi2_%TFL0Hadrons,<,4' ])
    , Member ( 'TM' , 'VeloCalo'
               , InputSelection1 = '%TF2Velo'
               , InputSelection2 = '%TFL0Hadrons'
               , MatchName = 'VeloCalo' , MaxQuality = 4.
               )
    , Member ( 'TU', 'GuidedForward'
               , InputSelection  = '%TMVeloCalo'
               , RecoName = 'GuidedForward'
               , HistogramUpdatePeriod = 1
               )
    , Member ( 'TF' , 'GuidedForward'
               , FilterDescriptor = ['PT,>,'+str(SINGLEHADRON_PTCUT)])
    , Member ( 'TU' , 'FitTrack'
               , RecoName = "FitTrack")
    , Member ( 'TF' , '1FitTrack'
               , FilterDescriptor = ["FitIP_PV2D,||>,"+str(HADMAIN_IPCUT)])
    , Member ( 'TF' , '2FitTrack'
               , OutputSelection = '%Decision'
               , FilterDescriptor = ["FitChi2OverNdf,<,"+str(HADMAIN_TRACKFITCHI2CUT)])
    ])
       

Line ('DiHadron'
      , L0DU  = "L0_CHANNEL('"+L0HADRON+"')"
      , algos =  [
    HltL0CaloPrepare('L0Hadrons', CaloMakerTool="HadronSeedTool",
                     CaloType = 'Hadron',MinEt = HADL0_MINETCUT)
    , Member("TF","L0Hadrons",
             InputSelection = "L0Hadrons",
             FilterDescriptor = ["L0ET,>,"+str(HADL0_ETCUT)])
    , GaudiSequencer('Hlt1RecoRZVeloSequence')
    , Member ( 'TF' ,'RZVelo'
               , InputSelection = 'RZVelo'
               , FilterDescriptor = ['Calo2DChi2_%TFL0Hadrons,<,4'])
    , Member ( 'TU', 'Velo', RecoName = 'Velo')
    , Member ( 'TF', '1Velo'
               , FilterDescriptor = [ 'IP_PV2D,||>,'+str(HADMAIN_IPCUT)] )
    , Member ( 'TF', '2Velo'
               , FilterDescriptor = [
                                      'Calo3DChi2_%TFL0Hadrons,<,4' ])
    , Member ( 'TM' , 'VeloCalo'
               , InputSelection1 = '%TF2Velo'
               , InputSelection2 = '%TFL0Hadrons'
               , MatchName = 'VeloCalo' , MaxQuality = 4.
               )
    , Member ( 'TU', 'GuidedForward'
               , InputSelection  = '%TMVeloCalo'
               , RecoName = 'GuidedForward'
               , HistogramUpdatePeriod = 1
               )        
    , Member ( 'TF', 'GuidedForward'
               , FilterDescriptor = ['PT,>,'+str(HADMAIN_PTCUT)])
    , HltTrackUpgrade( 'Hlt1RecoVelo' )
    , Member ( 'TF', '1Velo2'
               , InputSelection = HltTrackUpgrade('Hlt1RecoVelo')
               , FilterDescriptor = [ 'IP_PV2D,||>,'+str(HADMAIN_IPCUT)])
    , Member ( 'TF', '2Velo2'
               , InputSelection = HltTrackUpgrade('Hlt1RecoVelo')
               , FilterDescriptor =
               ['MatchIDsFraction_%TFGuidedForward,<,0.9' ])
    , Member ( 'VM2', 'Velo'
               , InputSelection1 = '%TFGuidedForward'
               , InputSelection2 = '%TF2Velo2'
               , FilterDescriptor = [ 'DOCA,<,'+str(HADVERTEX_DOCACUT)])
    , Member ( 'VF' ,'Velo2'
               , FilterDescriptor = [ 'VertexDz_PV2D,>,'+str(HADVERTEX_DZCUT)])
    , Member ( 'VU', 'Forward' , RecoName = 'Forward')
    , Member ( 'VF', '1Forward'
               , FilterDescriptor = [ 'VertexMinPT,>,'+str(HADVERTEX_MINPTCUT)])
    , Member ( 'VF', '2Forward'
               , FilterDescriptor =
               [ 'VertexPointing_PV2D,<,'+str(HADVERTEX_POINTINGCUT)])
    , Member('VU', 'FitTrack',RecoName = 'FitTrack')
    , Member('VF', '1FitTrack'
             , FilterDescriptor =
             [ 'FitVertexMinIP_PV2D,||>,'+str(HADVERTEX_MINIPCUT)])
    , Member('VF', '2FitTrack'
             , OutputSelection = "%Decision"
             , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,'+str(HADMAIN_TRACKFITCHI2CUT) ])
    ])


# here get a Hlt1 line 
def getLine(name):
    for line in hlt1Lines():
        if (line._name == name): return line
    print "ERROR:  no line with name", name


if (SOFTDIHADRON):
    getLine("DiHadron").clone("SoftDiHadron"
                              , L0DU = L0ALL
                              , TFL0Hadrons = {"FilterDescriptor":
                                               ["L0ET,>,"+str(HADL0_SOFTETCUT)]}
                              , TFGuidedForward =
                              {"FilterDescriptor":["PT,>,"+str(HADMAIN_SOFTPTCUT)]}
                              )

