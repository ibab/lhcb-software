#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltPhotonLines.py,v 1.2 2008-09-23 08:49:43 graven Exp $
# =============================================================================
## @file
#  Configuration of Photon Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure Photon trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================

from Gaudi.Configuration import * 

from Configurables import GaudiSequencer
from Configurables import PatMatchTool

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop 

#//---------------------------
#// HLT Reconstruction
#//--------------------------
#
#// in Had Alley #include "$HLTCONFROOT/options/HltRecoSequence.opts"
#
#//---------------------------
#// HLT Pho Alley
#//--------------------------

Line ('Photon' ,
      L0 = [ 'Photon' ]
      algos = 
      [ Member ( 'CaloPrep' 
               , CaloType = "Photon"
               , MinEt = 2300.0 
               )
      , Member ('TF', 'Photon', , FilterDescriptor = ["IsPhoton,>,0.5"])
      , GaudiSequencer('Hlt1RecoRZVeloSequence')
      , Member ('TF', 'RZVelo'
               , InputSelection     = "RZVelo"
               , FilterDescriptor = ["rIP_PV2D,||[],0.10,3.0"]
               , HistogramUpdatePeriod = 0
               , HistoDescriptor = { "rIP" : ("rIP",-1.,3.,400), "rIPBest" : ("rIPBest",-1.,3.,400)}
               )
      , Member ('TU', 'Velo' , RecoName = "Velo") #  Velo Reco
      , Member ('TF', 'Velo' #  3D IP selection
               , FilterDescriptor = ["IP_PV2D,||[],0.15,3.0"]
               , HistogramUpdatePeriod = 0
               , HistoDescriptor = { "IP" : ("IP",-1.,3.,400), "IPBest" : ("IPBest",-1.,3.,400) }
               )
      , GaudiSequencer('HltDecodeT')
      , GaudiSequencer('HltDecodeTT')
      , Member ('TU' ,'Forward' , RecoName = "Forward") #  upgrade to Forward 
      , Member ('TF', 'Forward' #  Pt cut (call it pretrigger sice could not implement veloTT)
               , FilterDescriptor = ["PT,>,650."]
               , HistogramUpdatePeriod = 0
               , HistoDescriptor =  { "PT"     : ("PT",0.,8000.,100), "PTBest" : ("PTBest",0.,8000.,100) }
               )
      , Member ('VM1', 'DiTrack' # two track vertex: DOCA
               , FilterDescriptor = [ "DOCA,<,0.2"]
               , HistoDescriptor = { "DOCA":("DOCA",0.,3.,100), "DOCABest":("DOCABest",0.,3.,100) }
               )
      , Member ('VF', 'DiTrack' #two track vertex: DZ
               , FilterDescriptor = [ "VertexDz_PV2D,>,0."]
               , HistoDescriptor = { "VertexDz_PV2D":("VertexDz_PV2D",-3.,3.,100), "VertexDz_PV2DBest":("VertexDz_PV2D",-3.,3.,100) }
               )
      , Member ( 'HltAddPhotonToVertex', 'DiTrackDecision' # add photon track to ditrack vertex to save all objects into summary
               , InputSelection1 = "%VFDiTrack"
               , InputSelection2 = "%TFPhoton"
               )
      ] )
