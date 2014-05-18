#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file Generator.py
#
#  Very simple manipulations with Generator/HepMC information
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
#  @date 2006-10-12
#
#                    $Revision:$
#  Last modification $Date: 2010-09-13 13:24:04 $
#                 by $Author: ibelyaev $
# =============================================================================
"""

Very simple manipulations with Generator/HepMC information

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

                  $Revision:$
Last modification $Date: 2010-09-13 13:24:04 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2006-10-12 " 
__version__ = " Version $Revision: 1.15 $ "
# =============================================================================
import ROOT 
from   GaudiKernel.SystemOfUnits import GeV 
## import everything from bender 
from   Bender.MainMC import * 
# =============================================================================
## @class Generator
#  Very simple manipulations with Generator/HepMC information
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class Generator(AlgoMC):                        ## <--- Note the base class here
    """
    Very simple manipulations with Generator/HepMC information
    """
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """

        
        ## get "all" decays 
        mybc = self.gselect ( 'mybc' , 'B_c+' == GABSID )
        if mybc.empty () : 
            return self.Warning('No true Bc are found!' ,SUCCESS )
        
        for b in mybc :
            self.plot( GPT( b ) / GeV , 'pt(Bc)' , 0 , 50 , 50 )
        
        ## get all other beauty long-lived particles
        from PartProp.Nodes import LongLived
        LongLived.validate ( self.ppSvc() ) 
        otherb = self.gselect ( 'otherb' ,
                                GBEAUTY & ( 'B_c+' != GABSID ) & GDECNODE ( LongLived ) )

        for b in otherb :
            print 'Other B:' , b.decay()

        return SUCCESS
    
# =============================================================================
## The configuration of the job
def configure ( inputdata        ,    ## the list of input files  
                catalogs = []    ,    ## xml-catalogs (filled by GRID)
                castor   = False ,    ## use the direct access to castor/EOS ? 
                params   = {}    ) :
    
    ## import DaVinci    
    from Configurables import DaVinci
    ## delegate the actual configuration to DaVinci
    dv = DaVinci ( Simulation      = True    , ## IMPORTANT 
                   DataType        = '2012'  ,
                   InputType       = 'DST'   ) 
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'GenTruth'
    dv.UserAlgorithms += [ alg_name ]

    #
    ## specific acion for HepMC-files
    #
    from BenderTools.GenFiles import genAction
    genAction()
    
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg   = Generator( alg_name  , PPMCs =  [] )
    
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
        'Gauss-14433050-2000ev-20140415.gen'
        ]
    
    configure( inputdata , castor = True )
    
    ## event loop 
    run(100)


# =============================================================================
# The END 
# =============================================================================
