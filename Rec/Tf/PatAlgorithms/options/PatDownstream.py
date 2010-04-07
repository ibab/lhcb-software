from TrackSys.Configuration import *
from Configurables import  PatDownstream


if TrackSys().earlyData():       
    PatDownstream().xPredTol2 = 20.0
    PatDownstream().TolMatch = 1.5
    PatDownstream().TolUV = 2.0
    PatDownstream().maxWindowSize = 10.0
    PatDownstream().MaxChisq  = 20.0
    PatDownstream().MaxDistance = 0.3
    PatDownstream().deltaP = 2.0
    PatDownstream().errorZMagnet = 30.0
