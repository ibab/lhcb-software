#!/usr/bin/env python
# =============================================================================
# $Id: MCJPsiKstar.py,v 1.4 2004-11-12 14:24:42 ibelyaev Exp $
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
        Bmc = mc.find ( decay = "[B0 -> J/psi(1S) K*(892)0]cc" )
        print ' #Decay tree dound: ' , Bmc.size()
        
        lst = Seq2List  ( Bmc )
        tup = Seq2Tuple ( Bmc )
        
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
    
    alg = gaudi.algorithm('MC')
    alg.OutputLevel = 5
    alg.NTupleLUN    = 'MC'
    
    desktop = gaudi.tool('Dstar.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Charged"]
    
    # output histogram file 
    hsvc = gaudi.service( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'mc.hbook'
    
    nsvc = gaudi.service( 'NTupleSvc' )
    nsvc.Output =[ "DSTAR DATAFILE='mc_tup.hbook' TYP='HBOOK' OPT='NEW'" ]

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
    gaudi.run  ( 100 )
    # terminate the Application Manager 
    gaudi.exit ()
# =============================================================================

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
