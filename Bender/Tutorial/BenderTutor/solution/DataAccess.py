#!/usr/bin/env python
# =============================================================================
# $Id: DataAccess.py,v 1.6 2006-06-06 20:03:27 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.6 $
# =============================================================================
"""
'Solution'-file for 'DataAccess/GetData' example (Bender Tutorial)
"""
# =============================================================================
# @file
#
# "Solution"-file for 'DataAccess/GetData' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  belyaev@lapp.in2p3.fr'
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
        
        # get all MC particles 
        mcps = self.get( 'MC/Particles' )
        print ' # of MC-particles : ' , mcps.size() 
        
        # get all MC collisisons 
        colls = self.get( 'Gen/Collisions' , list = TRUE )
        for c in colls :
            print 'Collision:' , `c` 
            
        # for the first 5 particles print PID information
        i = 0
        for mcp in mcps :
            if not i < 5 : break
            print ' MCParticle name is ', nameFromPID( mcp.particleID() ) 
            print ' MCParticle:', `mcp`
            i +=1 
            
        return SUCCESS
# =============================================================================

# =============================================================================
# The configuration of the job
# =============================================================================
def configure() :

    # general configuration :
    gaudi.config ( files = [ '$DAVINCIROOT/options/DaVinciCommon.opts' ] )
    
    # modify/update the configuration:
    
    # 1) create the algorithm
    alg = GetData( 'GetData' )
    
    # (2) replace the list of top level algorithms by
    #     new list, which contains only *THIS* algorithm
    gaudi.setAlgorithms( [ alg ] )
    
    # define input data files :
    #    1) get the Event Selector from Gaudi
    evtSel = gaudi.evtSel()
    #    2) configure Event Selector 
    #       files from $DAVINCIROOT/options/DaVinciTestData.opts 
    evtSel.open( [
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000665_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000645_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000648_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000652_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000656_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000658_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000659_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000667_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000670_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000672_9.dst' ] ) 
    

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
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END
# =============================================================================
