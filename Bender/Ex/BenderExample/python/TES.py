#!/usr/bin/env python
# =============================================================================
# $Id: TES.py,v 1.3 2004-08-06 12:12:03 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script to demonstaret the data manipulation in Bender
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2004-07-23 
# =============================================================================

# import the Bender itself  
from   bendermodule import *
import benderconfig as bender 
import sys

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class TES(Algo):
    " My own algorithm to demonstaret the direct manipulation with TES  "
    def analyse ( self ) :

##         # get all mc-particles from Transient event store
##         # (make on-flight convertsion to std::vector<> ) 
##         mcps = self.get ( address = 'MC/Particles'  , vector = TRUE  )
        
##         # print the first 2 particles 
##         i = 0 
##         for mcp in mcps :
##             if i < 2 :
##                 print ' *** *** *** MC-particle *** *** *** \n' , `mcp` , \
##                       mcp.particleID()
##             i+=1
            
##         # get all "selection results" from the store 
##         #  (make on-flight conversion to list) 
##         sels = self.get ( address = 'Phys/Selections' , list = TRUE )

##         # print all available selection results 
##         if sels :
##             for sel in sels :
##                 print ' *** *** *** SelResult   *** *** *** \n' , \
##                       `sel` , type(sel)          , \
##                       hasattr( sel , 'decay' )  
##                       #sel.decay() 

        l0 = self.get( address = '/Event/Trig/L0/Decision')
        if l0 : self.Print( message = ' L0 desision ' + `l0.decision()` )  
        l1 = self.get( address = '/Event/Trig/L1/Decision')
        if l1 : self.Print( message = ' L1 desision ' + `l1.decision()` )  
            
        return SUCCESS 

# =============================================================================
# job configuration 
# =============================================================================
def configure() :
    
    bender.config( files   = [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,
                               '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,
                               '$BENDEREXAMPLEOPTS/Bd_DstA1.opts'      ] )
    
    # specific job configuration    
    # create analysis algorithm and add it to the list of
    alg = TES('TES')
    alg = TES('TES1')
    g.topAlg = [ 'TES' , 'TES1' ]

    return SUCCESS 

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # analyse the options
    nEvents = bender.getNEvents( sys.argv[1:] )
    if not nEvents : nEvents = 10 
    # configure the job
    configure() 
    # run job
    g.run  ( nEvents )
    # terminate the Application Manager 
    g.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
