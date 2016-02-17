#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id::
# =============================================================================
## @file K0fromV0.py 
#
# Access for K0 from V0 
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
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-02-16
#
#                    $Revision: 197322 $
#  Last modification $Date: 2015-11-08 15:46:16 +0100 (Sun, 08 Nov 2015) $
#                 by $Author: ibelyaev $
# =============================================================================
"""Access for K0 from V0 

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

                  $Revision: 197322 $
Last modification $Date: 2015-11-08 15:46:16 +0100 (Sun, 08 Nov 2015) $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2016-02-16" 
__version__ = " $Revision:$"
# ============================================================================= 
## import everything from bender 
from   Bender.Main import *
# =============================================================================
## optional logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTutor.K0fromV0' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## @class V0
#  Demonstration how to use K0s & Lambda0 from V0 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-02-16
class V0(Algo):
    """
    Demonstration how to use K0s and Lambda0 from V0 
    """

    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """
        
        ## get particles from the input locations 
        kaons    = self.select ( 'k0s' , ' Meson  -> pi+ pi-   '  )
        
        tup1 = self.nTuple('MyK0S')
        for Ks in kaons :

            m = M ( Ks ) / GeV 
            tup1.column_float ( 'mass' , m ) 
            self.treatKine    ( tup1 , Ks , '_Ks'          ) 
            self.plot ( m , 'mass(pi+pi-)' , 0.460 , 0.540 )  
            tup1.write()

        lambdas  = self.select ( 'lams', '[Baryon -> p+  pi-]CC'  )            
        tup2 = self.nTuple('MyLambda0')
        for Lam in lambdas :

            m = M ( Lam ) / GeV 
            tup2.column_float ( 'mass' , m ) 
            self.treatKine    ( tup2 , Lam , '_Lam'       ) 
            self.plot ( m , 'mass(p+pi-)' , 1.100 , 1.140 )  
            tup2.write() 
            
            

        ## 
        return SUCCESS      ## IMPORTANT!!! 
# =============================================================================

# =============================================================================
## The configuration of the job
def configure ( inputdata        ,    ## the list of input files  
                catalogs = []    ,    ## xml-catalogs (filled by GRID)
                castor   = False ,    ## use the direct access to castor/EOS ? 
                params   = {}    ) :
    
    ## import DaVinci 
    from Configurables import DaVinci

    
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters (
        VOID_Code = """
        0 < CONTAINS('/Event/Rec/Vertex/V0')
        """
        )
    dv = DaVinci (
        DataType        = '2015'              ,
        InputType       = 'RDST'              ,
        EventPreFilters = fltrs.filters('V0') ,   
        TupleFile       = 'V0Tuples.root' , ## IMPORTANT 
        HistogramFile   = 'V0Histos.root' , ## IMPORTANT 
        )
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'V0'
    dv.UserAlgorithms += [ alg_name ]
    
    
    from StandardParticles import StdKs2PiPiLL    , StdKs2PiPiDD
    from StandardParticles import StdLambda2PPiLL , StdLambda2PPiDD
    
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg   = V0 (
        alg_name ,
        Inputs    = [
        StdKs2PiPiLL.outputLocation    () ,
        StdKs2PiPiDD.outputLocation    () ,
        StdLambda2PPiLL.outputLocation () ,
        StdLambda2PPiDD.outputLocation () ,
        ]
        )
             
    return SUCCESS 
# =============================================================================

# =============================================================================
## Job steering 
if __name__ == '__main__' :

    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( 80*'*'  ) 


    from Gaudi.Configuration import importOptions 
    importOptions('$STRIPPINGSELECTIONSROOT/tests/data/Reco15a_Run164668.py') 

    ## job configuration
    inputdata = []
    
    configure( inputdata , castor = True )
    
    ## event loop 
    run(5000)

    gaudi = appMgr()
    alg = gaudi.algorithm('V0')
    alg.dumpHistos( 50 , 20 ) 
    alg.NTuplePrint = True
    
# =============================================================================
# The END
# =============================================================================


