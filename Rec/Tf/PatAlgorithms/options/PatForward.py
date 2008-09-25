from TrackSys.Configuration import *
from Configurables import ( PatForward, PatForwardTool, PatFwdTool)
                            
PatForward("PatForward").addTool( PatForwardTool("PatForwardTool") )
PatForward("PatForward").PatForwardTool.AddTTClusterName = "PatAddTTCoord"
PatForward("PatForward").PatForwardTool.addTool( PatFwdTool("PatFwdTool"))


if TrackSys().getProp("fieldOff"):
  PatForward("PatForward").PatForwardTool.PatFwdTool.withoutBField  = True;
  PatForward("PatForward").PatForwardTool.WithoutBField = True;
