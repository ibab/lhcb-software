##
## Options to run pp collisions in the center of the LHCb cavern
## 

from Gauss.Configuration import *

## Sigle interactions
myGen = Generation("Generation")
myGen.PileUpTool = "FixedNInteractions"
 
## Beam momentum and vertex smearing as in IP

## Interaction in center of the cavern, i.e. z = 11.22 m
myGen.addTool(BeamSpotSmearVertex, name="BeamSpotSmearVertex")
myGen.BeamSpotSmearVertex.MeanZ = 11.22*SystemOfUnits.m

## Magnetic Field OFF and zero beam crossing angle and Velo close
## $APPCONFIGOPTS/Gauss/XX-bYY-moff and Conditions tags XX-vc-moff
