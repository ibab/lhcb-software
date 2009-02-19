
## @package RichHLTLocalPID
#  High level configuration tools for RICH HLT Local PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: RichHLTLocalPID_.py,v 1.2 2009-02-19 12:20:33 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__Rec__TrackPIDAlg,
                            Rich__Rec__HLTLocalPID__PIDTool )

# ----------------------------------------------------------------------------------

## @class RichHLTLocalPIDConfig
#  High level configuration tools for RICH HLT Local PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichHLTLocalPIDConfig(RichConfigurableUser):

    ## The default options
    __slots__ = {
        "Context" : "Offline"
       ,"PidSequencer" : None     # The sequencer to add the RICH PID algorithms to
        }
    
    ## @brief Configure the algorithms, adding them to the supplied sequencer
    #  @param sequence The sequencer to add the PID algorithms to
    def applyConf(self):

        # Are we properly configured
        recoSequencer = self.getProp("PidSequencer")
        if recoSequencer == None : raise RuntimeError("ERROR : Reconstruction Sequencer not set")
        
        # context
        context = self.getProp("Context")

        # Create the PID algorithm
        pidAlg = Rich__Rec__TrackPIDAlg( "Rich"+context+"LocalPIDAlg" )
        pidAlg.PIDToolName = "RichLPID"

        # Local PID tool
        pidTool = Rich__Rec__HLTLocalPID__PIDTool( "ToolSvc."+context+"_"+pidAlg.PIDToolName )

        # Add to tool registry
        self.toolRegistry().Tools += [pidTool.getType()+"/"+pidAlg.PIDToolName]

        # Add alg to the sequence
        recoSequencer.Members += [pidAlg]
