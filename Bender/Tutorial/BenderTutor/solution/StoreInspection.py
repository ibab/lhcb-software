#!/usr/bin/env python
# =============================================================================
# $Id: StoreInspection.py,v 1.2 2005-01-24 17:29:40 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.2 $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
"""
'Solution'-file for 'StoreInspection' example (Bender Tutorial)
"""
# =============================================================================
# @file
#
# "Solution"-file for 'StoreInspection' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  belyaev@lapp.in2p3.fr'

# import everything from BENDER
from bendermodule import *

# =============================================================================
# @class GetData
# define the primitive algorithm 
# =============================================================================
class GetData(Algo):
    def analyse( self ) :

        evtSvc = self.evtSvc()

        data = evtSvc['/Event/MC']
        
        print ' perform data inspection INSIDE the algorithm scope ' 
        evtSvc.dir('Rec')
                
        return SUCCESS
# =============================================================================

# =============================================================================
# The configuration fo the job
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
    evtSel = gaudi.evtSel()
    #    2) configure Event Selector 
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

    # inspect the data outside the algorithm
    print ' perform data inspection OUTSIDE the algorithm scope ' 
    evtSvc  = gaudi.evtSvc()
    data    = evtSvc['Rec'] 
    evtSvc.dir( data )
    data = evtSvc['Phys/GetData/Particles']
    evtSvc.ls(data)
    
    # for the interactive mode it is better to comment the last line
    gaudi.exit()
# =============================================================================
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END
# =============================================================================
