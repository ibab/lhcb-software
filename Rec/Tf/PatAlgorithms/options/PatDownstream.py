from TrackSys.Configuration import *
from Configurables import  PatDownstream


if "headTuning" in TrackSys().getProp("ExpertTracking"):       
    PatDownstream().xPredTol2 = 100.0
    PatDownstream().TolMatch = 5.5
    PatDownstream().TolUV = 6.0
    PatDownstream().maxWindowSize = 100.0
    PatDownstream().MaxChisq  = 20.0
    PatDownstream().MaxDistance = 0.15
    PatDownstream().deltaP = 3.0
    PatDownstream().errorZMagnet = 70.0
