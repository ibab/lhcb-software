#!/usr/bin/env python
# =============================================================================
# $Id: DataAccess.py,v 1.3 2004-11-08 17:02:45 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

# =============================================================================
# @file
#
# "Solution"-file for 'DataAccess/GetData' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================

# import everything from BENDER
from bendermodule import *

# =============================================================================
# @class GetData
# define the primitive algorithm 
# =============================================================================
class GetData(Algo):
    def analyse( self ) :

        # get and print the event number 
        hdr  = self.get( 'Header' )
        print ' Event/Run #%d/%d ' %( hdr.evtNum() , hdr.runNum() )
        
        # get all MC particles as 'native' KeyedContainer
        mcps1 = self.get( 'MC/Particles' )
        print ' The type of "mcps1" is ' , type(mcps1)

        # get all MC particles as std::vector ( useful for explicit loops)
        mcps2 = self.get( address = 'MC/Particles' ,
                          vector  = TRUE           )
        print ' The type of "mcps2" is ' , type(mcps2)
        
        # get all MC particles as python's list  ( useful for explicit loops)
        mcps3 = self.get( address = 'MC/Particles' ,
                          list    = TRUE           )
        print ' The type of "mcps3" is ' , type(mcps3)

        # get all MC collisisons 
        colls = self.get( 'Gen/Collisions' , list = TRUE )
        for c in colls :
            print 'Collision' , `c`
            
        # for the first 5 particles print PID information
        for i in range( 0 , min( 5 , len(mcps3) ) ) :
            print ' MCParticle name is ', nameFromPID( mcps3[i].particleID() ) 
        
        return SUCCESS
# =============================================================================

# =============================================================================
# The configuration of the job
# =============================================================================
def configure() :
    
    gaudi.config ( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    
    # modify/update the configuration:
    
    # 1) create the algorithm
    alg = GetData( 'GetData' )
    
    # (2) replace the list of top level algorithm by
    #     new list, which contains only *THIS* algorithm
    gaudi.setAlgorithms( [ alg ] )
    
    # define input data files :
    #    1) get the Event Selector from Gaudi
    #    2) configure Event Selector 
    evtSel = gaudi.evtSel()
    evtSel.open( [ 'LFN:/lhcb/production/DC04/v1/DST/00000543_00000017_5.dst' , 
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000018_5.dst' ,
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000016_5.dst' ,
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000020_5.dst' ] )

    
    return SUCCESS 
# =============================================================================
    
# =============================================================================
# Job steering 
# =============================================================================
if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    gaudi.run(5)
    
    # for the interactive mode it is better to comment the last line
    gaudi.exit()
# =============================================================================


# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END
# =============================================================================
