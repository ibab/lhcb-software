####################################################################################
#
# Pass-through lines with configurable prescale: 
#    PassLine:  selects all events 
#    PhysLine:  selects all non-lumi events
#
# @author Anton Poluektov
# @date 2009-12-18
#
####################################################################################       

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingPassConf(LHCbConfigurableUser):

    __slots__ = { 
                   'PassPrescale'       : 1.       # adimentional 
                ,  'PhysPrescale'       : 1.       # adimentional
                   }

    def passLine( self ) : 
    
	props = self.getProps()

	from StrippingConf.StrippingLine import StrippingLine
	return StrippingLine( "PassLine" , checkPV = False 
                                     , prescale = props['PassPrescale'] )


    def physLine( self ) : 
	from Configurables import HltRoutingBitsFilter

	# Filter lumi events
	physFilter = HltRoutingBitsFilter( "PhysFilter", RequireMask = [ 0x0, 0x4, 0x0 ] ) 

	props = self.getProps()
	
	from StrippingConf.StrippingLine import StrippingLine
	return StrippingLine( "PhysLine" , checkPV = False
				     , prescale = props['PhysPrescale']
				     , algos = [ physFilter ]  )

    def getProps(self) :
        """
	From HltLinesConfigurableUser
	@todo Should be shared between Hlt and stripping
	"""
	d = dict()
	for (k,v) in self.getDefaultProperties().iteritems() :
    	    d[k] = getattr(self,k) if hasattr(self,k) else v
	return d
