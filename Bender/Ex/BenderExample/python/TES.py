#!/usr/bin/env python
# =============================================================================
# $Id: TES.py,v 1.1 2004-07-24 14:06:38 ibelyaev Exp $
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

# create my own algorithm  
class TES(Algo):
    " My own algorithm to demonstaret the direct manipulation with TES  "
    def analyse ( self ) :

        # get all mc-particles from Transient event store
        # (make on-flight convertsion to std::vector<> ) 
        mcps = self.get ( address = 'MC/Particles'  , vector = TRUE  )
        
        # print the first 2 particles 
        i = 0 
        for mcp in mcps :
            if i < 2 : print ' *** *** *** MC-particle *** *** *** \n' , `mcp` 
            i+=1
            
        # get all "selection results" from the store 
        #  (make on-flight conversion to list) 
        sels = self.get ( address = 'Phys/Selections' , list = TRUE )

        # print all available selection results 
        if sels :
            for sel in sels :
                print ' *** *** *** SelResult   *** *** *** \n' , `sel`
            
        return SUCCESS 

# =============================================================================
# Generic job configuration 
# =============================================================================

bender.config( files   = [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ] )

# define input data channel B0 -> ( D*- -> D0bar(K+ pi-) pi- ) pi+  
g.readOptions('/afs/cern.ch/lhcb/project/web/cards/415000.opts')

g.HistogramPersistency = "HBOOK" ;

# =============================================================================
# specific job configuration 
# =============================================================================


# create analysis algorithm and add it to the list of
alg = TES('TES')
alg = TES('TES1')
g.topAlg = [ 'TES' , 'TES1' ]


# =============================================================================
# job execution 
# =============================================================================

g.run(10) 

g.exit()

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
