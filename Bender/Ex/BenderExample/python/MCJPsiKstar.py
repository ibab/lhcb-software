#!/usr/bin/env python
# =============================================================================
# $Id: MCJPsiKstar.py,v 1.5 2005-01-24 17:33:00 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.5 $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script to check MC for B -> J/psi K* channel 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2004-03-16 
# =============================================================================

# import the Bender itself  
from   bendermodule import *

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

# create my own algorithm  
class MCJPsiKstar(Algo):
    " My own analysis algorithm for MC checks for B0 -> J/psi K* channel "
    def analyse ( self ) :
        
        mc  = self.mctruth()
        Bmc  = mc.find ( decay = "[ B0 ->  J/psi(1S)  K*(892)0]cc" )
        Psi  = mc.find ( decay = "[ B0 -> ^J/psi(1S)  K*(892)0]cc" )
        Kst  = mc.find ( decay = "[ B0 ->  J/psi(1S) ^K*(892)0]cc" )
        print ' #Decay tree found: '  ,  \
              Bmc.size()  , Psi.size()  , Kst.size() 
        
        return SUCCESS 
    
# =============================================================================
# job configuration 
# =============================================================================
def configure() :
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                    '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
                    '$BENDEREXAMPLEOPTS/Bd_JpsiKst.opts'    ] , # input data 
                  options =
                  [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                    'HcalPIDmu.OutputLevel     =   5  ' ,
                    'EcalPIDe.OutputLevel      =   5  ' ,
                    'HcalPIDe.OutputLevel      =   5  ' ,
                    'BremPIDe.OutputLevel      =   5  ' ,
                    'PrsPIDe.OutputLevel       =   5  ' ,
                    'NeutralPP2MC.OutputLevel  =   5  ' ,
                    'EventSelector.PrintFreq   =  50  ' ] )
    
    # specific job configuration 
    # create analysis algorithm and add it to the list of
    alg = MCJPsiKstar('MC')
    
    gaudi.setAlgorithms( [ alg ] ) 
    
    return SUCCESS
# =============================================================================

# =============================================================================
# job steering
# =============================================================================
if __name__ == '__main__' :
    import sys 
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 500 )
    # terminate the Application Manager 
    gaudi.exit ()
# =============================================================================

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
