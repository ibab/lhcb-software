#!/usr/bin/env python
# $Id: Hybrids.py,v 1.3 2004-08-06 12:12:03 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script for LoKiHybrid demo  
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04 
# =============================================================================

# import the Bender itself  
from   bendermodule import *
import benderconfig  as bender 

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

# create my own algorithm  
class Hybrids(Algo):
    " Trivial algorithm to text LoKiHybrid approach "
    def analyse ( self ) :

        self.setFilterPassed ( TRUE )
        
        cut   = FILTER ( self.filterCriterion(0) )
        pions = self.select( tag = 'pions' , cuts = ( ID == 'pi+' ) & ( cut ) )

        return SUCCESS 

def configure() :    
    # Generic job configuration 
    
    bender.config( files   = [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ] ,
                   options = [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                               'HcalPIDmu.OutputLevel     =   5  ' ,
                               'EcalPIDe.OutputLevel      =   5  ' ,
                               'HcalPIDe.OutputLevel      =   5  ' ,
                               'BremPIDe.OutputLevel      =   5  ' ,
                               'PrsPIDe.OutputLevel       =   5  ' ,
                               'Hybrids.Hybrid.Code  =  " ( PT > 0.1 * MeV  ) & ( P < 30 * GeV ) "  ' ,
                               'Hybrids1.Hybrid.Code  =  " ( PT > 0.1 * MeV  ) & ( P < 30 * GeV ) "  ' ,
                               'EventSelector.PrintFreq   =  50  ' ] )
    
    # define input data channel B0 -> ( D*- -> D0bar(K+ pi-) pi- ) pi+  
    g.readOptions('/afs/cern.ch/lhcb/project/web/cards/415000.opts')
    
    # specific job configuration 
    
    # load LoKiHybrid library 
    g.DLLs += [ 'LoKiHybrid' ] 
    
    # create analysis algorithm and add it to the list of
    alg = Hybrids('Hybrids')
    g.topAlg += [ 'Hybrids' ]
    alg = gaudi.iProperty('Hybrids')
    alg.OutputLevel = 5
    alg.FilterCriteria = [ 'HybridFilterCriterion/Hybrid']
    
    # create analysis algorithm and add it to the list of
    alg = Hybrids('Hybrids1')
    g.topAlg += [ 'Hybrids1' ]
    alg = gaudi.iProperty('Hybrids1')
    alg.OutputLevel = 5
    alg.FilterCriteria = [ 'HybridFilterCriterion/Hybrid']
    
    # output histogram file 
    hsvc = g.property( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'Hybrids.hbook'
    
    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # analyse the options
    nEvents = bender.getNEvents( sys.argv[1:] )
    if not nEvents : nEvents = 1000 
    # configure the job
    configure() 
    # run job 
    g.run  ( nEvents )
    # terminate the Application Manager 
    g.exit ()
 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
