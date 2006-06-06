#!/usr/bin/env python
# =============================================================================
# $Id: MCmuons.py,v 1.7 2006-06-06 20:03:27 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.7 $
# =============================================================================
""" 'Solution'-file for 'MCmuons.py' example (Bender Tutorial) """
# =============================================================================
# @file
#
# "Solution"-file for 'MCmuons.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  belyaev@lapp.in2p3.fr'
# =============================================================================

# import everything from BENDER
from bendermodule import *

# =============================================================================
# define the primitive algorithm 
# =============================================================================
class MCMuons(Algo):
    def analyse( self ) :
        
        # get *ALL* Monte Carlo muons 
        mcmu   = self.mcselect ( tag = 'mcmu'   , cuts = 'mu+' == MCABSID )

        # get all beauty hadrons 
        beauty  = self.mcselect( tag = 'beauty' , cuts = BEAUTY )
        
        # from selected muons select the muons from beauty
        muFromB = self.mcselect ( tag    = 'muFromCharm'         ,
                                  source = mcmu                  ,
                                  cuts   = FROMMCTREE( beauty )  )

        # get muons produce in vicinity of  primary vertex
        muFromPV = self.mcselect ( tag    = 'MuFromPV'                    ,
                                   source = muFromB                       ,
                                   cuts   = MCVXFUN( MCVZ ) < ( 20 * cm ) )
        
        print '  MCMuons:         '                  , \
              '  \tTotal            ' , mcmu.size()    , \
              ', \tFrom B-tree:     ' , muFromB.size() , \
              ', \tProduced near PV ' , muFromPV.size()
        
        return SUCCESS
# =============================================================================

# =============================================================================
# The configuration of the job 
# =============================================================================
def configure() :

    gaudi.config ( files = ['$DAVINCIROOT/options/DaVinciCommon.opts' ] )
    
    # modify/update the configuration:
    
    # 1) create the algorithm
    alg = MCMuons( 'McMu' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [ alg ] )
    
    # define input data files :
    #    1) get the Event Selector from Gaudi
    #    2) configure Event Selector 
    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 10 
    # files from $DAVINCIROOT/options/DaVinciTestData.opts 
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
# The control flow 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    gaudi.run(100)

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
