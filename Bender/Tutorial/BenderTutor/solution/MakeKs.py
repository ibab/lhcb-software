#!/usr/bin/env python
# =============================================================================
# $Id: MakeKs.py,v 1.2 2010-09-13 13:24:04 ibelyaev Exp $ 
# =============================================================================
## @file solution/MakeKs.py
#
#  Simple example that illustrates Ks-making
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. And it is based on the 
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
#  Last modification $Date: 2010-09-13 13:24:04 $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Simple example that illustrates Ks-making

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. And it is based on the LoKi project:
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
   ``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date: 2010-09-13 13:24:04 $
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
## @class MakeKs
#  Simple algoritm to make K0s from pi+ and pi-
#
class MakeKs(Algo):

    # =========================================================================
    ## the main method: analyse the event
    def analyse( self ) :
        """
        the main method: analyse the event
        """
        # get all primary vertices
        primary = self.vselect ( 'pv' , PRIMARY )
        if primary.empty() : return SUCCESS
        
        chi2ip  = MIPCHI2 ( primary , self.geo() )
        
        # select from input particles according to the criteria: 
        pions   = self.select ( 'pions' ,
                                ( 'pi+' == ABSID ) &
                                ( 100 * MeV < PT ) &
                                ( TRCHI2DOF < 10 ) )
        
        pi_plus  = self.select ( 'MyGoodPi+' , pions , Q > 0 )
        pi_minus = self.select ( 'MyGoodPi-' , pions , Q < 0 )
        
        k0s = self.loop ( 'MyGoodPi+ MyGoodPi-' , 'KS0' )        
        for k in k0s :

            ## rought estimate for the mass 
            m12 = k.mass(1,2) / MeV 
            if not 300 < m12   < 800  : continue  ## CONTINUE 

            ## vertex fit & chi2(vertex) cut 
            chi2vx = VCHI2 ( k )
            if not 0 <= chi2vx < 100  : continue  ## CONTINUE 
            
            ## get good mass-estimate:
            m     = M ( k ) / MeV 
            if not 450 < m   < 550    : continue  ## CONTINUE 
            
            # fill the histogram
            self.plot ( m , 'mass pi+ pi-' , 450 , 550 )
            
            pi1 = k(1)
            pi2 = k(2)

            ## pointing cut for daughter pions 
            if chi2ip ( pi1 ) < 9 : continue    ## CONTINUE
            if chi2ip ( pi2 ) < 9 : continue    ## CONTINUE
            
            # fill the histogram
            self.plot ( m , 'mass pi+ pi-, 2' , 450 , 550 )

            pv = self.bestVertex ( k.particle() )
            if not pv : continue
            
            ## Ks pointing cut 
            chi2ip_k = IPCHI2 ( pv , self.geo() )            
            if chi2ip_k( k ) > 9 : continue      ## CONTINUE 
            
            # fill the histogram
            self.plot ( m , 'mass pi+ pi-, 3' , 450 , 550 )

            ## lifetime cut : 
            ctau = LTIME( self.lifetimeFitter() , pv ) * c_light
            if ctau ( k ) < 5 * mm : continue    ## CONTINUE 
            
            # fill the histogram
            self.plot ( m , 'mass pi+ pi-, 4' , 450 , 550 )
            
            k.save('Ks')
            
            
        ks = self.selected('Ks')
            
        self.setFilterPassed ( not ks.empty() )
        
        return SUCCESS


    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        ##
        self.dumpHistos( 50 , 30 , True )
        ##
        return Algo.finalize ( self )
    
# =============================================================================

# =============================================================================
## The configuration of the job
def configure( inputdata , catalogs = [] ) :
    """
    The configuration of the job
    """
    
    from Configurables import DaVinci , GaudiSequencer 
    DaVinci ( DataType        = '2011' ,
              Lumi            = False  )
    
    from StandardParticles import StdLoosePions
    InputParticles = [ StdLoosePions.outputLocation () ]
    
    setData ( inputdata , catalogs )

    ## get/create Application Manager
    gaudi = appMgr()
    
    # modify/update the configuration:
    
    ## create the algorithm
    alg = MakeKs (
        'MakeKs'  ,
        Inputs =  InputParticles 
        )
    
    ## add algorithm into the main DaVinci sequence
    dvMain = gaudi.algorithm('GaudiSequencer/DaVinciUserSequence' , True )
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
        '/castor/cern.ch/grid/lhcb/LHCb/Collision11/MINIBIAS.DST/00011900/0000/00011900_000000%02d_1.minibias.dst' % i for i in range(1,25)
        ]

    configure( inputdata )
    
    ## event loop 
    run(500)
    
# =============================================================================
# The END
# =============================================================================
