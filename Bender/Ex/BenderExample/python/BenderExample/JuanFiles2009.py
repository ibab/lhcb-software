#!/usr/bin/env python
# =============================================================================
# @file BEnderExamples/JuanFiles2009.oy
# The list of files for 2009 real data with "useful" events, stripped by Juan
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date 2010-01-12
# =============================================================================
"""

The list of files for 2009 real data with 'useful' events, stripped by Juan

Usage:


   ... 
   >>> gaudi = appMgr()
   >>> evtSel = gaudi.evtSel()                       ## get event selector 

   >>> from BenderExample.JuanFiles2009 import files ## import list of files 
   >>> evtSel.open( files )                         

   >>> run ( -1 )                                    ## run for all events
   ...
   
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================

data_first =  [
    '/castor/cern.ch/user/j/jpalac/72/0/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/1/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/2/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/3/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/4/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/5/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/6/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/7/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/8/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/9/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/10/outputdata/Data2009.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/72/11/outputdata/Data2009.PhysEvent.dst'
    ]

files_5730 = [
    '/castor/cern.ch/user/j/jpalac/74/0/outputdata/Prod5730.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/74/1/outputdata/Prod5730.PhysEvent.dst'
    ]

files_5731 = [
    '/castor/cern.ch/user/j/jpalac/76/0/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/1/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/2/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/3/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/4/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/5/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/6/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/7/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/8/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/9/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/10/outputdata/Prod5731.PhysEvent.dst',
    '/castor/cern.ch/user/j/jpalac/76/11/outputdata/Prod5731.PhysEvent.dst'
    ]


files = data_first + files_5730 + files_5731

if '__main__' == __name__ :

    print __doc__, __author__, __version__
    
    for f in files : print f 
    
# =============================================================================
# The END 
# =============================================================================


