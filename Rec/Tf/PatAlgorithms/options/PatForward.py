from TrackSys.Configuration import *
from Configurables import ( PatForward, PatForwardTool, PatFwdTool)
                            
PatForward("PatForward").addTool( PatForwardTool("PatForwardTool") )
PatForwardTool("PatForwardTool").AddTTClusterName = "PatAddTTCoord"
PatForwardTool("PatForwardTool").addTool( PatFwdTool("PatFwdTool"))


if TrackSys().getProp("fieldOff"):
  PatFwdTool("PatFwdTool").withoutBField  = True;
  PatForwardTool("PatForwardTool").WithoutBField = True;
