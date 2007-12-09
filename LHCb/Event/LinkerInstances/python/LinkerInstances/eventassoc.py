#!/usr/bin/env python
# =============================================================================
# $Id: eventassoc.py,v 1.2 2007-12-09 20:29:30 ibelyaev Exp $ 
# =============================================================================
"""
The helper module for easy manipulations with Linker objects

The module exports two funcions:

   - linkedTo 
   - linkedFrom
   
"""
# =============================================================================
__author__ = "Thomas RUF Thomas.Ruf@cern.ch"
# =============================================================================

from GaudiPython.Bindings import AppMgr as __AppMgr
from GaudiPython.Bindings import gbl    as __gbl 


## helper function for manipulation with linker objects 
def linkedTo( t1, t2, location ):
  """
  Helper function for easy manipulation with linker objects

  e.g.
  [
  the example is stollen from LHCb/GaudiPython TWiki page:
  
    http://cern.ch/twiki/bin/view/GaudiPython#How_to_access_Linker_tables
    
  ]
  
  >>> linked = linkedTo ( LHCb.MCParticle , LHCb.Track , 'Rec/Track/Velo' )
  >>> track = ...        
  >>> links = linked.range ( track )   ## get all related MC-particles 
  >>> for i in range(0,links.size())   ## loop over all related MC-particles 
  ...       mcpi = links[i]                ## get the related MC-particle  
  ...       print mcpi                     ## print it 
  >>> ....
    
  """
  appMgr = __AppMgr()
  cl = __gbl.LinkedTo( t1, t2)
  lt = cl( appMgr.evtsvc()._idp, None, location )
  return lt

## helper function for manipulation with linker objects 
def linkedFrom( t1, t2, location ):
  """
  Helper function for manipulation with linker objects
  """
  appMgr = __AppMgr()
  cl =     __gbl.LinkedFrom( t1, t2, int)
  lt = cl( appMgr.evtsvc()._idp, None, location )
  return lt

# =============================================================================
if '__main__' == __name__ :
  print __doc__, " by ", __author__
  o = dir() 
  for i in o : print i   
  
# =============================================================================
# The END 
# =============================================================================
