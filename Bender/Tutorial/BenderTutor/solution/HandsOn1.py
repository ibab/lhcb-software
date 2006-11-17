#!/usr/bin/env python2.4
# =============================================================================
# $Id: HandsOn1.py,v 1.7 2006-11-17 11:59:47 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.7 $
# =============================================================================
""" 'Solution'-file for 'DataAccess/GetData' example (Bender Tutorial) """
# =============================================================================
# @file
#
# "Solution"-file for 'DataAccess/GetData' example (Bender Tutorial)
#
# @author Vanya BELYAEV  ibelyaev@physics.syr.edu
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  ibelyaev@physics.syr.edu'
# =============================================================================

## import everything from BENDER
from bendermodule import *

# =============================================================================
## @class GetData
#  solution for the first excersize 
class GetData(Algo):
    """ solution for the first excersize  """

    ## the main method for analysis 
    def analyse( self ) :
        """ the main method for analysis """
        
        ## get all MC vertices 
        mcvs = self.get( 'MC/Vertices' )
        
        for mcv in mcvs :
            pos = mcv.position() 
            x = pos.x()  
            y = pos.y()  
            z = pos.z() 
            if not 100 < z < 200 : continue
            print 'MCVertex x/y/z:  %s/%s/%s [mm] ' %(x,y,z)
        
        return SUCCESS
# =============================================================================

# =============================================================================
## The configuration of the job
def configure() :
    """ The configurtaion of the job """ 
    
    ## general configuration :
    gaudi.config ( files = [ '$DAVINCIROOT/options/DaVinciCommon.opts' ] )
     
    ## modify/update the configuration:
    
    ## 1) create the algorithm
    alg = GetData( 'GetData' )
    
    ## 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [alg] ) 

    ## define input data files :
    #    1) get the Event Selector from Gaudi
    evtSel = gaudi.evtSel()
    #    2) configure Event Selector
    import data_tutorial as data 
    evtSel.open( data.FILES ) 
    
    return SUCCESS
# =============================================================================

# =============================================================================
## Job steering
if __name__ == '__main__' :

    ## job configuration
    configure()
    
    ## event loop 
    gaudi.run(10)
# =============================================================================



# =============================================================================
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END
# =============================================================================


