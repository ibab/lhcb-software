#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Module with some multiprocessing functionality for Ostap 
#
#  Currently it is not loaded on default, and requires manual activation
#
#  @see GaudiMP.Parallel
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-09-23
#
#  
#                    $Revision:$
#  Last modification $Date:$
#  by                $Author:$
# =============================================================================
""" Multiprocessing functionality for Ostap
Currently it is not loaded on default, and requires manual activation
"""
# =============================================================================
__version__ = "$Revision: 177429 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = (
    'ProjectTask' ,
    'project'
    ) 
# =============================================================================
import GaudiMP.Parallel as Parallel  
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.Kisa' )
else                       : logger = getLogger ( __name__     )
# =============================================================================
logger.debug ( 'Multiprocessing functionality for Ostap')
# =============================================================================

# =============================================================================
## The simplest object for more efficient projection of long TChains
#  into histogarms
#  @see GaudiMP.Parallel
#  @see GaudiMP.Parallel.Task
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-09-23
class ProjectTask(Parallel.Task) :
    """ The simplest object for efficient projection of long TChains
    into histogarms  
    """
    ## constructor: chain name, historgam , variable , cuts 
    def __init__ ( self , tree , histo , what , cuts = '' ) :
        """
        Constructor: chain name, historgam , variable , cuts
        
        >>> histo = ...
        >>> task  = ProjectTask ( 'MyTuple' , histo , 'mass' , 'pt>10' ) 
        """
        
        self.histo = histo
        self.histo.Reset()
        
        if   isinstance ( tree , ROOT.TTree ) : self.tree = tree.GetName()
        elif isinstance ( tree , str        ) : self.tree = tree 
        
        self.what = what
        self.cuts = cuts

    ## local initialization (executed once in parent process)
    def initializeLocal   ( self ) :
        """
        Local initialization (executed once in parent process)
        """
        import ROOT,Ostap.PyRoUts
        self.output = self.histo.Clone() 
        
    ## remote initialization (executed for each sub-processs)
    def initializeRemote  ( self ) : pass 

    ## the actual processing 
    def process ( self , params ) :

        import ROOT
        import Ostap.PyRoUts 
        
        if   isinstance ( params , str                ) : pars = [ params            ]
        elif isinstance ( params , ROOT.TChainElement ) : pars = [ params.GetTitle() ] 
        
        chain = ROOT.TChain ( self.tree )
        for p in pars : chain.Add ( p )

        ## Create the output histogram   NB! (why here???) 
        self.output = self.histo.Clone()
        
        ## use the regular projection  
        from Ostap.TreeDeco import _tt_project_ 
        _tt_project_ ( chain ,  self.output  , self.what , self.cuts )

        del chain 

    ## finalization (execuetd at the end at parent process)
    def finalize ( self ) : pass 

    ## merge results 
    def _mergeResults(self, result) :
        #
        self.output.Add ( result )
        result.Delete () 

# =============================================================================  
## make a projection of the (long) chain into histogram using
#  multiprocessing functionality for per-file parallelisation
#  @code
#
#  >>> chain = ... ## large chain
#  >>> histo = ... ## histogram template 
#  >>> project ( chain , histo , 'mass' , 'pt>10' )
#
#  @endcode
#
#  For 12-core machine, clear speedup factor of about 8 is achieved 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-09-23
def  project ( chain , histo , what , cuts ) :
    """
    Make a projection of the (long) chain into histogram
    
    >>> chain = ... ## large chain
    >>> histo = ... ## histogram template 
    >>> project ( chain , histo , 'mass' , 'pt>10' )
    
    For 12-core machine, clear speedup factor of about 8 is achieved     
    """
    #
    histo.Reset()    
    files = chain.files() 

    task  = ProjectTask          ( chain , histo , what , cuts )
    wmgr  = Parallel.WorkManager ()
    wmgr.process( task, files )

    histo += task.output  

    return histo


import ROOT 
ROOT.TChain._project = project

# =============================================================================
# The END 
# =============================================================================
