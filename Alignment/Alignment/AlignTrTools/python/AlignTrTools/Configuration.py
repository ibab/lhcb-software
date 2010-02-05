## @package AlignTrTools
# High level configuration tools for AlignTrTools
# @author Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>
# @date   03/02/2009 


from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables import (LHCbConfigurableUser, LHCbApp, TAConfig )


__version__= "$Id: Configuration.py"
__author__= "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"

class AlignTrTools( LHCbConfigurableUser ):

     __slots__ = {
            "Tools" 		 : [] # list of various tools 
	  , "Constraints" 	 : [] # list of constraints
          , "Degrees_of_Freedom" : [] # degrees of freedom to align
          }
     
     
     def __apply_configuration__( self ):
         print "******* calling ", self.name()
	 print "Tools used: ", self.getProp("Tools")
         for tool in self.getProp("Tools"):
             print "Configuring tool ", tool
             if tool == "TAConfig":
                importOptions("$ALIGNTRTOOLSROOT/options/TAConfig.py")
 		TAConfig().Constrain_OT = self.getProp("Constraints")
                TAConfig().Degrees_of_Freedom = self.getProp("Degrees_of_Freedom")
                print "TAConfig degrees of freedom = ", TAConfig().getProp("Degrees_of_Freedom")
             if tool  == "ATrackSelector":
	        importOptions("$ALIGNTRTOOLSROOT/options/ATrackSelector.py")
             if tool == "WriteAlignmentConditions": 
	        importOptions("$ALIGNTRTOOLSROOT/options/WriteAlignmentConditions.py")
                
