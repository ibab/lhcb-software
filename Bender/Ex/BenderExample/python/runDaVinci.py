#!/usr/bin/env python
# $Id: runDaVinci.py,v 1.1 2004-08-05 12:20:34 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script to run DaVinci/Bender/... application 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2004-08-03 
# =============================================================================

import sys
argv = sys.argv
if len(argv) < 1 :raise TypeError, "job-options fiel is not specified!"
jobopts = argv[1]
print " runDaVinci.py: Job-options file to be executed: '" + jobopts + "'"

# import the Bender itself
import gaudimodule   as gaudi 
import benderconfig  as bender
# uncomment if some Bender-specific features need to be used 
#from   bendermodule  import *

# =============================================================================
# Generic job configuration 
# =============================================================================

bender.config( files   = [ jobopts ] ,
               options = [ 'EventSelector.PrintFreq   =  50  ' ] )

g = gaudi.AppMgr()
# histogram persistency from options file is ignored *NOW*
g.HistogramPersistency = "HBOOK" ;

# =============================================================================
# job execution 
# =============================================================================


evtMax = 100   

g.run(evtMax) 

g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
