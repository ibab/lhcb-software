#!/usr/bin/env python
# =============================================================================
# $Id: HandsOn1.py,v 1.4 2005-01-24 17:29:40 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.4 $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
""" 'Solution'-file for 'DataAccess/GetData' example (Bender Tutorial) """
# =============================================================================
# @file
#
# "Solution"-file for 'DataAccess/GetData' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  belyaev@lapp.in2p3.fr'

# import everything from BENDER
from bendermodule import *

# =============================================================================
# define the primitive algorithm 
# =============================================================================
class GetData(Algo):
    def analyse( self ) :
        
        # get all MC vertices  as python list 
        mcvs = self.get( address = 'MC/Vertices' ,
                          list    = TRUE           )
        for mcv in mcvs :            
            x = mcv.position().x() / cm  
            y = mcv.position().y() / cm  
            z = mcv.position().z() / cm
            if z > 30 : continue 
            print ' MCVertex x,y,z[cm] is ' , x,y,z   
        
        return SUCCESS
# =============================================================================

# =============================================================================
# The configurtaion of the job
# =============================================================================
def configure() :
    
    gaudi.config ( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    
    # modify/update the configuration:
    
    # 1) create the algorithm
    alg = GetData( 'GetData' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [alg] ) 

    return SUCCESS

# =============================================================================
# Job steering
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    gaudi.run(50)

    # for the interactive mode it is better to comment the last line
    gaudi.exit()
# =============================================================================
    

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END
# =============================================================================


