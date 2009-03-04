#!/usr/bin/env python
# =============================================================================
# $Id: MCmuons.py,v 1.13 2009-03-04 12:56:49 ibelyaev Exp $ 
# =============================================================================
"""
'Solution'-file for 'MC-muons' example (Bender Tutorial)
"""
# =============================================================================
# @file
#
# "Solution"-file for 'MC-muons' example (Bender Tutorial)
#
# @author Vanya BELYAEV  ibelyaev@physics.syr.edu
# @date   2004-10-12
# =============================================================================
__author__  = ' Vanya BELYAEV  Ivan.Belyaev@nikhef.nl '
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.13 $  '  
# =============================================================================
## import everything from BENDER
from Bender.MainMC import *
# =============================================================================
## @class MCMuons  
#  the primitive algorithm
class MCMuons(AlgoMC):
    """
    The primitive algorithm
    """
    ## the main "analysis" mehtod 
    def analyse( self ) :
        """
        The main 'analysis' mehtod
        """
        
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
    """
    The configuration of the job
    """
    
    from Configurables import DaVinci
    
    DaVinci (
        DataType   = 'DC06'     , # default  
        Simulation = True       ,
        HltType    = ''
        ) 
    
    ## get/create application manager
    gaudi = appMgr() 
   
    # modify/update the configuration:
    
    ## 1) create the algorithm
    alg = MCMuons( 'McMu' )
    
    ## 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [ alg ] )
    
    ## redefine input files 
    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 10
    import LoKiExample.Bs2Jpsiphi_mm_data as data 
    evtSel.open( data.Files ) 
    
    return SUCCESS
# =============================================================================

# =============================================================================
## Job steering
if __name__ == '__main__' : 

    ## job configuration
    configure()

    ## event loop 
    run(100)

# =============================================================================
# The END 
# =============================================================================
