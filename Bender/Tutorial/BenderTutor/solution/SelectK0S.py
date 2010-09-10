#!/usr/bin/env python
# =============================================================================
# $Id: SelectK0S.py,v 1.1 2010-09-10 09:33:33 ibelyaev Exp $ 
# =============================================================================
# $URL$ 
# =============================================================================
## @file solutions/SelectK0S.py
#
#  Simple example that illustrates the selection of K0S from stripped DSTs
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-12
#
#  Last modification $Date: 2010-09-10 09:33:33 $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Simple example that illustrates the selection of K0S from stripped DSTs

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date: 2010-09-10 09:33:33 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2006-10-12 " 
__version__ = " Version $Revision: 1.1 $ "
# =============================================================================
## import everything from BENDER
from Bender.Main                   import *
from GaudiKernel.PhysicalConstants import c_light 
# =============================================================================
## @class SelectKs
#  Simple algoritm to select K0S from stripped DSTs 
class SelectKs(Algo):

    # =========================================================================
    ## the main method: analyse the event
    def analyse( self ) :
        """
        the main method: analyse the event
        """
        
        # select from input particles according to the decay pattern:
        k0s = self.select ( 'k0' , 'KS0 -> pi+ pi-' )

        # loop over selected particles:
        for k0 in k0s :

            # fill the histogram
            self.plot ( M ( k0 ) , 'mass K0S' , 450 , 550 )

            if PT ( k0 ) > 500 * MeV :
                self.plot ( M ( k0 ) , 'mass K0S, pt > 500 MeV ' , 450 , 550 )

            pv   = self.bestVertex ( k0 )
            
            ctau = LTIME( self.lifetimeFitter() , pv , 9 ) * c_light

            self.plot ( ctau ( k0 ) , 'c*tau for K0S' , 0 , 200 , 200 ) 
            

        return SUCCESS


    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return Algo.finalize ( self )
    
# =============================================================================

## The configuration of the job
def configure( inputdata , catalogs = [] ) :
    """
    The configuration of the job
    """
    
    from Configurables import DaVinci 
    DaVinci ( DataType        = '2010' ,
              Lumi            = False  )

    from Configurables import CondDB
    CondDB  ( IgnoreHeartBeat = True )

    ## treat the input data 
    setData ( inputdata , catalogs )
    
    ## get/create Application Manager
    gaudi = appMgr()

    #
    ## modify/update the configuration:
    #
    
    ## create the algorithm
    alg = SelectKs (
        'SelectKs'                         ,
        RootInTES      = '/Event/V0'       ,
        InputLocations = [ 'StrippingK0S'  ] 
        )

    ## add the algorithm into the main DaVinci sequence
    dvMain = gaudi.algorithm('GaudiSequencer/DaVinciMainSequence' , True )
    dvMain.Members += [ alg.name() ]
    
    return SUCCESS 
# =============================================================================


# =============================================================================
## Job steering 
if __name__ == '__main__' :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
    ## job configuration
    inputdata = [
        '/castor/cern.ch/grid' + '/lhcb/data/2010/V0.DST/00007577/0000/00007577_000000%02d_1.v0.dst' % n for n in range ( 4 , 15 ) 
        ]
    
    configure( inputdata )
    
    ## event loop 
    run(500)

    
# =============================================================================
# The END
# =============================================================================
