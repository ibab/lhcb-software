## @package AlignTrTools
# High level configuration tools for AlignTrTools
# @author Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>
# @date   03/02/2009 


from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables import (LHCbConfigurableUser, LHCbApp )


__version__= "$Id: Configuration.py"
__author__= "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"

class AlignTrTools( LHCbConfigurableUser ):

     __slots__ = {
          "Tools" : [] # list of various tools 
          }
     
     
     def __apply_configuration__( self ):
         print "******* calling ", self.name()
         for tool in self.getProp("Tools"):
             print "Configuring tool ", tool
             if tool == "TAConfig":
                importOptions("$ALIGNTRTOOLSROOT/options/TAConfig.py")
             if tool  == "ATrackSelector":
	        importOptions("$ALIGNTRTOOLSROOT/options/ATrackSelector.py")
             if tool == "WriteAlignmentConditions": 
	        importOptions("$ALIGNTRTOOLSROOT/options/WriteAlignmentConditions.py")
                
