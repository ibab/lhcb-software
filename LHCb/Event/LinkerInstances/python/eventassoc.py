#!/usr/bin/env python
# =============================================================================
# $Id: eventassoc.py,v 1.8 2007-12-09 20:13:20 ibelyaev Exp $ 
# =============================================================================
"""
The helper module for manipulations with Linker objects 

WARNING: The module is deprecated, please use  LinkerIntances.eventassoc instead

 ---------------------------------------------------------------------------------------------------
 |  OLD semantics  (deprecated)           |          NEW SEMANTICS                                 |
 ---------------------------------------------------------------------------------------------------
 | import eventassoc                      | import LinkerInstances.eventassoc                      |
 | import eventassoc as XXX               | import LinkerInstances.eventassoc as XXX               |
 | from eventassoc import *               | from LinkerInstances.eventassoc import *               |
 | from eventassoc import linkedTo        | from LinkerInstances.eventassoc import linkedTo        |
 | from eventassoc import linkedTo   ax Y | from LinkerInstances.eventassoc import linkedTo   as Y |
 | from eventassoc import linkedFrom      | from LinkerInstances.eventassoc import linkedFrom      |  
 | from eventassoc import linkedFrom as X | from LinkerInstances.eventassoc import linkedFrom as X |
 ---------------------------------------------------------------------------------------------------
"""
# =============================================================================
__author__ = "Thomas RUF Thomas.Ruf@cern.ch"
# =============================================================================

print """
eventassoc.py WARNING  Deprecated module to be removed soon
                       Please use LinkerIntances.eventassoc instead

 ---------------------------------------------------------------------------------------------------
 |  OLD semantics  (deprecated)           |          NEW SEMANTICS                                 |
 ---------------------------------------------------------------------------------------------------
 | import eventassoc                      | import LinkerInstances.eventassoc                      |
 | import eventassoc as XXX               | import LinkerInstances.eventassoc as XXX               |
 | from eventassoc import *               | from LinkerInstances.eventassoc import *               |
 | from eventassoc import linkedTo        | from LinkerInstances.eventassoc import linkedTo        |
 | from eventassoc import linkedTo   ax Y | from LinkerInstances.eventassoc import linkedTo   as Y |
 | from eventassoc import linkedFrom      | from LinkerInstances.eventassoc import linkedFrom      |  
 | from eventassoc import linkedFrom as X | from LinkerInstances.eventassoc import linkedFrom as X |
 ---------------------------------------------------------------------------------------------------
"""

from LinkerInstances.eventassoc import *
      
# =============================================================================
if '__main__' == __name__ :
  print __doc__ 
  o = dir() 
  for i in o : print i
  
# =============================================================================
# The END 
# =============================================================================
