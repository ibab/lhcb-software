#!/usr/bin/env python
# $Id: MCJPsiKstar.py,v 1.3 2004-08-06 12:12:03 ibelyaev Exp $
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
import benderconfig as bender 

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
    bender.config( files   =
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
    
    g.topAlg = [ 'MC' ]
    
    alg = gaudi.iProperty('MC')
    alg.OutputLevel = 5
    alg.NTupleLUN    = 'MC'
    
    desktop                 = g.property('Dstar.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Charged"]
    
    # output histogram file 
    hsvc = g.property( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'mc.hbook'
    
    nsvc = gaudi.iProperty( 'NTupleSvc' )
    nsvc.Output =[ "DSTAR DATAFILE='mc_tup.hbook' TYP='HBOOK' OPT='NEW'" ]

    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # analyse the options
    nEvents = bender.getNEvents( sys.argv[1:] )
    if not nEvents : nEvents = 100 
    # configure the job
    configure() 
    # run job 
    g.run  ( nEvents )
    # terminate the Application Manager 
    g.exit ()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
