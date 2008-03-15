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
    
    ## get the input data
    import BenderExample.data_Bs2Jpsiphi_mm as input 
    
    ## read external configruation files
    gaudi.config (
        files = [
        '$DAVINCIROOT/options/DaVinciCommon.opts'         ,
        '$COMMONPARTICLESROOT/options/StandardKaons.opts' ] )

    #
    # perform some specific configuration 
    #

    return SUCCESS

# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__
    
    ## configure the job:
    configure()

    ## run the job
    gaudi.run(1000)

# =============================================================================
# The END 
# =============================================================================
