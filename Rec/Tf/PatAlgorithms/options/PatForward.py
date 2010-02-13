from TrackSys.Configuration import *
from Configurables import ( PatForward, PatForwardTool, PatFwdTool)
                            
PatForward("PatForward").addTool( PatForwardTool("PatForwardTool") )
PatForward("PatForward").PatForwardTool.AddTTClusterName = "PatAddTTCoord"
PatForward("PatForward").PatForwardTool.addTool( PatFwdTool("PatFwdTool"))

if TrackSys().fieldOff():
  PatForward("PatForward").PatForwardTool.PatFwdTool.withoutBField  = True
  PatForward("PatForward").PatForwardTool.WithoutBField = True

if TrackSys().earlyData():
  PatForward("PatForward").PatForwardTool.MinXPlanes = 4
  PatForward("PatForward").PatForwardTool.MinPlanes = 8
  PatForward("PatForward").PatForwardTool.MaxSpreadX = 1.5
  PatForward("PatForward").PatForwardTool.MaxSpreadY = 3.0
  PatForward("PatForward").PatForwardTool.MaxChi2 = 40
  PatForward("PatForward").PatForwardTool.MaxChi2Track = 40
  PatForward("PatForward").PatForwardTool.MinHits = 12
  PatForward("PatForward").PatForwardTool.MinOTHits = 14
