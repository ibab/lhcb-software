#!/usr/bin/env gaudirun.py
# =============================================================================

from Gaudi.Configuration import * 

from Configurables import GaudiSequencer

from HltSys.HltLine import Hlt1Line   as Line
from HltSys.HltLine import Hlt1Member as Member
from HltSys.HltLine import hlt1Lines 


recoRZVelo = GaudiSequencer('Hlt1RecoRZVeloSequence') 
decodeT    = GaudiSequencer('HltDecodeT')
decodeTT   = GaudiSequencer('HltDecodeTT')

singleHadron = Line (
    'SingleHadron'     ,  ## the line name 
    L0 = [ "hadron" ]  ,  ## require L0-trigger
    algos = [
    recoRZVelo ,                              ## 1) start with RZ recontruction
    Member ( 'TF' , 'RZVelo' ,
             InputSelection   =  'RZVelo' ,
             FilterDescriptor = [ "Calo2DChi2_L0TriggerHadron,<,4" ] ) ,
    Member ( 'TU' , 'Velo'   , RecoName = 'Velo' ) ,
    Member ( 'TF' , 'Velo'      ,
             FilterDescriptor = [ "IP_PV2D,||>,0.1",
                                  "Calo3DChi2_L0TriggerHadron,<,4" ] ) ,
    Member ( 'TM' , 'VeloCalo' ,
             InputSelection1 = '%TFVelo'         ,
             InputSelection2 = 'L0TriggerHadron' ,
             MatchName       = 'VeloCalo'        ) ,
    decodeT  ,
    decodeTT , 
    Member ( 'TU' , 'Forward'  , RecoName = 'Forward' )
    ] )


singleHadron2 = singleHadron.clone (
    'SingleHadron2'
    , postscale = 0.1 ) 

singleHadron3 = singleHadron.clone (
    'SingleHadron3'
    , prescale  = 0.1 
    , postscale = 0.1
    , TMVeloCalo = { 'MatchName' : 'SomeOotherMath'}
    ) 

print singleHadron 
print singleHadron2
print singleHadron3

print hlt1Lines() 

    
