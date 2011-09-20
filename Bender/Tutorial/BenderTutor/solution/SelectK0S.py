#!/usr/bin/env python
# =============================================================================
# $Id: SelectK0S.py,v 1.2 2010-09-13 13:24:05 ibelyaev Exp $ 
# =============================================================================
## @file solution/SelectK0S.py
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
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-12
#
#  Last modification $Date: 2010-09-13 13:24:05 $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Simple example that illustrates the selection of K0S from stripped DSTs

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
     ``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date: 2010-09-13 13:24:05 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2006-10-12 " 
__version__ = " Version $Revision: 1.2 $ "
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

            self.plot ( ctau ( k0 ) , 'c*tau for K0S' , 0 , 100 , 200 ) 
            

        return SUCCESS


    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        #
        self.dumpHistos ( 50 , 30 , True )
        #
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
        ## Reco05-Stripping09 convention 
        RootInTES      = '/Event/V0'       , ## Reco05-Stripping09 convention 
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
        ## dbpath: /LHCb/Collision10/Beam3500GeV-VeloClosed-MagDown/Reco05-Striping09-Merged/90000000/V0.DST
        '/castor/cern.ch/grid' + '/lhcb/data/2010/V0.DST/00007551/0000/00007551_00000%03d_1.v0.dst' % i for i in range ( 1 , 691 )
        ]
    
    configure( inputdata )
    
    ## event loop 
    run(500)

    
# =============================================================================
# The END
# =============================================================================
