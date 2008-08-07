#!/usr/bin/env gaudirun.py
# =============================================================================

from Gaudi.Configuration import * 

from Configurables import GaudiSequencer

from HltSys.HltLine import Hlt1Line   as Line
from HltSys.HltLine import Hlt1Member as Member
from HltSys.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop


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
    Member ( 'TU' , 'Forward'  , RecoName = 'Forward' , OutputLevel = 2 )
    ] )


singleHadron2 = singleHadron.clone (
    'SingleHadron2'
    , postscale = 0.1 ) 

singleHadron3 = singleHadron.clone (
    'SingleHadron3'
    , prescale  = 0.05 
    , postscale = 0.65
    , TMVeloCalo = { 'MatchName' : 'SomeOtherMatchAlgorihtm'}
    ) 



print singleHadron 

addHlt1Prop ( 'OutputLevel'     ) 
print singleHadron2
rmHlt1Prop  ( 'OutputLevel'     ) 
addHlt1Prop ( 'OutputSelection' ) 

print singleHadron3

print " The list of all created lines: ", hlt1Lines() 

    
