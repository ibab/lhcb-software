#!/usr/bin/env python
# $Id: runDaVinci.py,v 1.3 2004-08-21 14:33:42 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script to run DaVinci/Bender/... application 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2004-08-03 
# =============================================================================

# import the Bender itself
import sys
import gaudimodule   as gaudi 
import benderconfig  as bender


jobopts = bender.getDVoptions ( sys.argv[1:] )
if not jobopts :
    raise TypeError , "no DaVinci options fiel is specified! "

# =============================================================================
# globals 
# =============================================================================
SUCCESS = gaudi.gbl.StatusCode( gaudi.gbl.StatusCode.SUCCESS ) 
g       = gaudi.AppMgr()
# =============================================================================

# uncomment if some Bender-specific features need to be used 
# from   bendermodule  import *

# =============================================================================
# job configuration 
# =============================================================================
def configure() : 
    bender.config( files   = [ jobopts ] ,
                   options = [ 'EventSelector.PrintFreq   =  50  ' ] )
    
    g = gaudi.AppMgr()
    # histogram persistency from options file is ignored *NOW*
    g.HistogramPersistency = "HBOOK" ;

    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # analyse the options
    nEvents = bender.getNEvents( sys.argv[1:] )
    if not nEvents : nEvents = 1000
    # configure the job
    configure() 
    # run job
    #g.run  ( nEvents )
    g.run  ( 500  )
    # terminate the Application Manager 
    g.exit ()

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
