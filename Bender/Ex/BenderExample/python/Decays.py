#!/usr/bin/env python
# $Id: Decays.py,v 1.3 2004-08-06 12:12:03 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script to run D*+ -> ( D0 -> K- pi+ ) pi+ selection with Bender
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04 
# =============================================================================

# import the Bender itself  
from   bendermodule import *
import benderconfig as bender 

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

# create my own algorithm  
class Decays(Algo):
    """
    Simple algorithm to test variouse symbold in mc.find statements
    """
    def analyse ( self ) :
        
        mc  = self.mctruth()
        trees = mc.find ( decay = "       [D0 -> K- pi+]cc   " )
        all   = mc.find ( decay = "       [D0 -> ^K- ^pi+]cc " )
        p1    = mc.find ( decay = "       [D0 -> ^K- pi+]cc  " )
        p2    = mc.find ( decay = "       [D0 -> K- ^pi+]cc  " )
        k1    = mc.find ( decay = " K-  : [D0 -> K- pi+]cc   " )
        k2    = mc.find ( decay = " K+  : [D0 -> K- pi+]cc   " )
        pi1   = mc.find ( decay = " pi- : [D0 -> K- pi+]cc   " )
        pi2   = mc.find ( decay = " pi+ : [D0 -> K- pi+]cc   " )
        
        self.plot ( value = trees.size() , title = " number of trees " ,
                    high = 10 , bins = 20 )
        self.plot ( value =   all.size() , title = " number of all   " ,
                    high = 10 , bins = 20 )
        self.plot ( value =    p1.size() , title = " number of p1    " ,
                    high = 10 , bins = 20 )
        self.plot ( value =    p2.size() , title = " number of p2    " ,
                    high = 10 , bins = 20 )
        self.plot ( value =    k1.size() , title = " number of k1    " ,
                    high = 10 , bins = 20 )
        self.plot ( value =    k2.size() , title = " number of k2    " ,
                    high = 10 , bins = 20 )
        self.plot ( value =   pi1.size() , title = " number of pi1   " ,
                    high = 10 , bins = 20 )
        self.plot ( value =   pi2.size() , title = " number of pi2   " ,
                    high = 10 , bins = 20 )
        
        msg  = " MCDecays found : "
        msg += "  trees = " + `trees.size()`
        msg += " ,all   = " + `all.size()`
        msg += " ,p1    = " + `p1.size()`
        msg += " ,p2    = " + `p2.size()`
        msg += " ,k1    = " + `k1.size()`
        msg += " ,k2    = " + `k2.size()`
        msg += " ,pi1   = " + `k1.size()`
        msg += " ,pi2   = " + `k2.size()`
        
        self.Print( message = msg , level = MSG.ALWAYS )
        
        return SUCCESS 

# =============================================================================
# job configuration 
# =============================================================================
def configure () :    
    # Generic job configuration     
    bender.config( files   =
                   [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                     '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
                     '$BENDEREXAMPLEOPTS/Bd_DstA1.opts'      ] , # input data 
                   options =                                     # extra options 
                   [ 'EcalPIDmu.OutputLevel     =   5  ' , 
                     'HcalPIDmu.OutputLevel     =   5  ' ,
                     'EcalPIDe.OutputLevel      =   5  ' ,
                     'HcalPIDe.OutputLevel      =   5  ' ,
                     'BremPIDe.OutputLevel      =   5  ' ,
                     'PrsPIDe.OutputLevel       =   5  ' ,
                     'EventSelector.PrintFreq   =  50  ' ] )
    
    # specific job configuration 
    # create analysis algorithm and add it to the list of
    alg       = Decays('Decays')
    g.topAlg  = [ 'Decays' ]
    alg = gaudi.iProperty('Decays')
    alg.OutputLevel = 5
    alg.FilterCriteria = [ 'HybridFilterCriterion/Hybrid']
    
    # add 'similar' C++ algorithm from LoKiExample package
    g.DLLs   += [ "LoKiExample"            ]
    g.topAlg += [ 'LoKi_MCDecays/MCDecays' ]
    
    # output histogram file 
    hsvc = g.property( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'Decays.hbook'
    
    return SUCCESS 

# =============================================================================
# job execution 
# =============================================================================
if __name__ == '__main__' :
    import sys 
    # analyse the options
    nEvents = bender.getNEvents( sys.argv[1:] )
    if not nEvents : nEvents = 20 
    # configure the job 
    configure()
    # execute 
    g.run( nEvents )
    # terminate 
    g.exit()

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
