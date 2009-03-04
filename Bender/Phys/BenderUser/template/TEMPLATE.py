#!/usr/bin/env python
# =============================================================================
# $Id: TEMPLATE.py,v 1.3 2009-03-04 12:59:26 ibelyaev Exp $ 
# =============================================================================
## @file
#  This a template file for the Bender-based scriopt/module
#  @author ...
#  @date   ...
# =============================================================================
"""
This a template file for the Bender-based scriopt/module
"""
# =============================================================================
__author__  = " Do not forget your name here "
__verison__ = "CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.3 $"
# =============================================================================
## import all nesessary stuff from Bender
from Bender.MainMC import * 
# =============================================================================
## @class Template
class Tempate(AlgoMC) :
    """
    This is the template algorithm 
    """        
    ## standard constructor
    def __init__ ( self , name = 'Template' ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name )

    
   ## standard mehtod for analyses
    def analyse( self ) :
        """
        Standard method for analysis
        """

        return SUCCESS

# =============================================================================
## job configuration:
def configure ( **args ) :
    """
    Configure the job
    """
    
    from Configurables import DaVinci
    
    DaVinci (
        DataType   = 'DC06'     , # default  
        Simulation = True       ,
        HltType    = '' ) 
    
    from Gaudi.Configuration import NTupleSvc, HistogramPersistencySvc
    HistogramPersistencySvc ( OutputFile = 'TEMPLATE_histos.root' )
    NTupleSvc ( Output = [ "FILE1 DATAFILE='TEMPLATE.root' OPT='NEW' TYP='ROOT'" ] )
   
    
    ## get/create application manager
    gaudi = appMgr() 
    
    
    #
    # perform some specific configuration 
    #

    ## get the input data
    import BenderExample.data_Bs2Jpsiphi_mm as input 
    

    return SUCCESS

# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__
    
    ## configure the job:
    configure()

    ## run the job
    run(1000)

# =============================================================================
# The END 
# =============================================================================
