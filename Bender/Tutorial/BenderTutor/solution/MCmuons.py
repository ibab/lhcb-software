#!/usr/bin/env python2.4
# =============================================================================
# $Id: MCmuons.py,v 1.8 2006-11-17 11:59:47 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.8 $
# =============================================================================
""" 'Solution'-file for 'MCmuons.py' example (Bender Tutorial) """
# =============================================================================
# @file
#
# "Solution"-file for 'MCmuons.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV  ibelyaev@physics.syr.edu
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

## import everything from BENDER
from bendermodule import *

# =============================================================================
## @class MCMuons  
#  the primitive algorithm
class MCMuons(AlgoMC):
    """  the primitive algorithm """

    ## the main "analysis" mehtod 
    def analyse( self ) :
        """ the main 'analysis' mehtod """
        
        ## get *ALL* Monte Carlo muons 
        mcmu   = self.mcselect  ( 'mcmu'   , 'mu+' == MCABSID )
        
        ## get all beauty hadrons 
        beauty = self.mcselect  ( 'beauty' , BEAUTY )
        
        # from selected muons select the muons from beauty
        muFromB = self.mcselect ( 'muFromCharm'         ,
                                  mcmu                  ,
                                  FROMMCTREE( beauty )  )
        
        # get muons produce in vicinity of  primary vertex
        muFromPV = self.mcselect ( 'MuFromPV'            ,
                                   muFromB               ,
                                   MCVXFUN( MCVZ ) < 200 )
        
        print '  MCMuons:         '                  , \
              '  \tTotal            ' , mcmu.size()    , \
              ', \tFrom B-tree:     ' , muFromB.size() , \
              ', \tProduced near PV ' , muFromPV.size()
        
        return SUCCESS
# =============================================================================

# =============================================================================
## The configuration of the job 
def configure() :
    """ The configuration of the job """
    
    gaudi.config ( files = ['$DAVINCIROOT/options/DaVinciCommon.opts' ] )
    
    # modify/update the configuration:
    
    ## 1) create the algorithm
    alg = MCMuons( 'McMu' )
    
    ## 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [ alg ] )
    
    ## redefine input files 
    evtSel = gaudi.evtSel()
    import data_tutorial as data 
    evtSel.open( data.FILES ) 
    evtSel.PrintFreq = 10

    
    return SUCCESS
# =============================================================================

# =============================================================================
## Job steering
if __name__ == '__main__' :
 

    ## job configuration
    configure()

    ## event loop 
    gaudi.run(100)

# =============================================================================
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END 
# =============================================================================
