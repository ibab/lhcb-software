#!/usr/bin/env python
# =============================================================================
"""
This a template file for the Bender-based scriopt/module
"""
# =============================================================================
## @file
#  This a template file for the Bender-based scriopt/module
#  @author ...
#  @date   ...
# =============================================================================
__author__ = " Do not forget your name here "
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
    
    ## read external configruation files
    importOptions ( '$DAVINCIROOT/options/DaVinciCommon.opts' )
    importOptions ( '$COMMONPARTICLESROOT/options/StandardKaons.opts' )

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
