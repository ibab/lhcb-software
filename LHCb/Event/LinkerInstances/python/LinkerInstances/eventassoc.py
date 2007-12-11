#!/usr/bin/env python
# =============================================================================
# $Id: eventassoc.py,v 1.3 2007-12-11 08:34:25 truf Exp $ 
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
  
  >>> linktrack2part = linkedTo ( LHCb.MCParticle , LHCb.Track , 'Rec/Track/Best' )
  >>> track = ...        
  >>> for mcp in linktrack2part.range(track):  ## loop over all related MC-particles 
   ...       print mcp                          ## print it 
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
   e.g.
  
  >>> linkpart2track = linkedFrom ( LHCb.Track , LHCb.MCParticle , 'Rec/Track/Best' )
  >>> mcp = ...        
  >>> for track in linkpart2track.range(mcp):                 ## loop over all related tracks 
   ...       print 'track matched to MCParticle:',track.key()  ## print it 
  >>> ....
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
