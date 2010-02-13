#!/usr/bin/env python

#
# StrippingConfigurableUser
#

__all__ = ( 
	    'StrippingConfigurableUser'
	  )

from GaudiConf.Configuration import *
from Configurables import LHCbConfigurableUser

class StrippingConfigurableUser ( LHCbConfigurableUser ) :

    def getProps(self) :
        """
	From HltLinesConfigurableUser
	@todo Should be shared between Hlt and stripping
	"""
	d = dict()
	for (k,v) in self.getDefaultProperties().iteritems() :
    	    d[k] = getattr(self,k) if hasattr(self,k) else v
	return d
