#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file BenderExample/RealDstar_V0.py
#
#  The script to analyse the D*+ -> ( D0 -> K- pi+ ) pi+
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
#  with the smear campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2010-06-18
#
#                    $Revision$ 
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""

The script to analyse the D*+ -> ( D0 -> K- pi+ ) pi+

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

                  $Revision$ 
Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__   = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__     = " 2010-06-18 "
__version__  = " Version $Revision$ "
# =============================================================================
import ROOT                           ## needed to produce/visualize the histograms
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Bender.Main         import *   ## import all bender goodies 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm, cm 
from   GaudiKernel.PhysicalConstants import c_light
from   GaudiPython.GaudiAlgs         import Vector

from LoKiHlt.algorithms import ALG_PASSED, ALG_EXECUTED

# =============================================================================
## @class Dstar
#  The algorithm to analyse the D*+ -> ( D0 -> K- pi+ ) pi+
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-02-11
class Dstar(Algo) :
    """
    The algorithm to analyse the D*+ -> ( D0 -> K- pi+ ) pi+
    """
    
    ## the only one esential method: 
    def analyse  (self ) :
        """
        The only one essential method
        """

        ## get D*+ 
        dstars = self.select ( 'dstar' , '[ D*(2010)+ -> ( D0 -> K- pi+ ) pi+]CC' )        
        if dstars.empty()   :
            return self.Warning ( 'No D*+ decays are found' , SUCCESS )

        dm0 = ( M - M1  ) / GeV 
        dm1 = DTF_FUN ( dm0 , False )
        dm2 = DTF_FUN ( dm0 , True  )
        
        for d in dstars :
            print d.decay()
            
            v0 = dm0 ( d ) 
            self.plot ( v0 , "delta m0 (simple)   " ,  0.135 , 0.160 ) 
            v1 = dm1 ( d ) 
            self.plot ( v1 , "delta m1 (DTF) "      ,  0.135 , 0.160 )            
            v2 = dm2 ( d ) 
            self.plot ( v2 , "delta m2 (DTF+PV) "   ,  0.135 , 0.160 )
            
            self.plot ( v0 - v1 , "delta delta-1" , -0.0025 , 0.0025 , 200 )
            self.plot ( v1 - v2 , "delta delta-2" , -0.0025 , 0.0025 , 200 )


        self.setFilterPassed ( not dstars.empty()  ) 
        
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
## configure the job 
def configure ( datafiles , catalogs = [] , castor = False ) :
    """
    Job configuration 
    """
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters (
        VOID_Code = " 0.5 < CONTAINS('/Event/Charm/Phys/DstarForPromptCharm/Particles') " 
        )
    ##
    ## 1. Static configuration using "Configurables"
    ##
    
    from Configurables           import DaVinci    
    davinci = DaVinci (
        DataType        = '2010' ,
        InputType       = 'MDST' ,
        PrintFreq       = 1000   ,
        EventPreFilters = fltrs.filters('Filters') ,
        HistogramFile   = 'RealDstar_CHARM_Histos.root' ,
        TupleFile       = 'RealDstar_CHARM.root' , 
        ##
        Lumi            = True 
        )
    
    from Configurables import CondDB
    CondDB ( UseLatestTags = ['2010'] )
    
    from Configurables import NTupleSvc
    ntSvc = NTupleSvc() 
    
    ## define the input data:
    setData ( datafiles , catalogs , castor )
    
    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Dstar (
        #
        'Dstar'             ,   ## Algorithm name
        #
        ## RecoStripping-09 conventions! 
        RootInTES =  '/Event/Charm'  , 
        Inputs    = [ 'Phys/DstarForPromptCharm/Particles' ] ## input particles 
        )
    
    ## finally inform Application Manager about our algorithm
    userSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence',True)    
    userSeq.Members += [ alg.name() ]
    
    return SUCCESS 


# =============================================================================
# The actual job steering
if '__main__' == __name__ :
    
    ## make printout of the own documentation
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  

    files = [
        '/lhcb/data/2010/CHARM.MDST/00008383/0000/00008383_00000%03d_1.charm.mdst' % i for i in range(1,650) 
        ]
    
    configure ( files , castor = True )
    
    ## run the job
    run (2000)
    

# =============================================================================
# The END 
# =============================================================================
