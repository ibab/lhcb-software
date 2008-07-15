from Brunel.Configuration import *
from Configurables import ( PatForward, PatForwardTool, PatFwdTool)
                            
PatForward("PatForward").addTool( PatForwardTool("PatForwardTool") )
PatForwardTool("PatForwardTool").AddTTClusterName = "PatAddTTCoord"
PatForwardTool("PatForwardTool").addTool( PatFwdTool("PatFwdTool"))

PatFwdTool("PatFwdTool").ZMagnetParams = [ 5199.31, 334.725, -1283.86, 9.59486e-06, -413.281 ]
PatFwdTool("PatFwdTool").xParams = [ 16.8238, -6.35556 ]
PatFwdTool("PatFwdTool").yParams = [ -970.89,  -0.686028 ]
PatFwdTool("PatFwdTool").momentumParams = [ 1.21909, 0.627841, -0.235216, 0.433811, 2.92798, -21.3909 ]

if Brunel().getProp("fieldOff"):
  PatFwdTool("PatFwdTool").withoutBField  = True;
  PatForwardTool("PatForwardTool").WithoutBField = True;
