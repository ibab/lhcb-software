#!/usr/bin/env python
# $Id: Hybrids.py,v 1.2 2004-07-24 14:06:38 ibelyaev Exp $
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

# =============================================================================
# Generic job configuration 
# =============================================================================

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

g.HistogramPersistency = "HBOOK" ;

# =============================================================================
# specific job configuration 
# =============================================================================

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


# =============================================================================
# job execution 
# =============================================================================

g.run(200) 

g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2004/07/11 15:54:54  ibelyaev
#  regular increment
#
# Revision 1.1  2004/06/29 06:41:52  ibelyaev
#  add new algorithm
#
# =============================================================================
