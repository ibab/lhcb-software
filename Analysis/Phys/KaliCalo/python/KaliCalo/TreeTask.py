#!/usr/bin/env python
# =============================================================================
# $Id: TreeTask.py,v 1.1 2010-03-30 15:40:59 ibelyaev Exp $
# =============================================================================
## @file KaliKalo/TreeTask.py
#  The helper class for parallel filling of histograms  using GaudiPython.Parallel
#  @see GaudiPyhon.Parallel
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-28
# =============================================================================
"""
The helper class for parallel filling of historgams using GaudiPython.Parallel
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-03-28 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $ "
__all__     = (
    "TreeTask"   ,
    )
# =============================================================================
import GaudiPython.Parallel   as Parallel
import KaliCalo.Pi0FillHistos as Fill
import KaliCalo.Kali          as Kali 
from GaudiKernel.SystemOfUnits import MeV
# =============================================================================

## @class TreeTask 
#  The helper class for parallel filling of historgams using GaudiPython.Parallel
#  @see GaudiPyhon.Parallel
#  @see GaudiPyhon.Parallel.Task
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-27
class TreeTask ( Parallel.Task ) :
    """
    The helper class for parallel filling of historgams
    using GaudiPython.Parallel
    
    @author Vanya BELYAEV Ivan.Belyaev@itep.ru
    @date 2010-03-28
    """
    
    ## constructor from lambda-map 
    def __init__ ( self                    ,
                   lambdas                 ,
                   Unit     = MeV          ,
                   tmpdir   = None         ) :
        
        ## initialize the base class 
        Parallel.Task.__init__ ( self )
        self._lambdas  = lambdas
        self._Unit     = Unit  
        self._tmpdir   = tmpdir
        
    ## local initialization 
    def initializeLocal ( self ) :
        """
        Local initialization
        """
        print 'FillTask : Start parallel processing (local)'
        import sets 
        self.output = ( sets.Set() , sets.Set() ) 
                        
    ## local initialization 
    def initializeRemote ( self ) :
        """
        Local initialization
        """
        print 'FillTask : Start parallel processing (remote) '
        return Parallel.Task.initializeRemote ( self ) 
        
    ## process the list of histo sets 
    def process  ( self , files ) :
        
        """
        Process the list of histo-sets 
        """

        import tempfile
        
        if self._tmpdir : 
            dbase_name = tempfile.mktemp ( dir = self._tmpdir , suffix = '_zdb.gz' )
        else :
            dbase_name = tempfile.mktemp (                      suffix = '_zdb.gz' )
            
        histos, lambdas, badfiles = Fill.fillDataBase (
            self._lambdas               ,
            files                       ,
            dbase_name = dbase_name     ,
            Unit       = self._Unit     )
        
        self.output[0].add ( dbase_name )
        for b in badfiles : self.output[1].add (  b )

        print 'End of processing ', files  

    ## finalization of the task 
    def finalize ( self ) :
        """
        Finalization of the task
        """
        print 'FillTask.finalize TOTAL: '

    ## merge the result form the different processes 
    def _mergeResults ( self, result ) :
        """
        merge the results form the different processes
        """
        print " Merge results ", result 

        for f in result[0] : self.output[0].add ( f ) 
        for f in result[1] : self.output[1].add ( f ) 
        
    def _resetOutput ( self ) :

        print 'I am reset output '
        self.output[0].clear() 
        self.output[1].clear() 

# ==============================================================================
## perform the parallel fill for the histograms 
def __fill_p_Histos ( lambdas                   ,
                      file_names                ,
                      ppservers  = []           ,
                      dbase_name = 'kali_db.gz' ,
                      Unit       = MeV          ) :
    """
    Perform the ``parallel'' fill for the histograms 
    """

    tmpdir = None
    if ppservers :
        import os 
        tmpdir = os.path.abspath('./')
        
    task = TreeTask ( lambdas , Unit , tmpdir = tmpdir ) 
    
    from GaudiPython.Parallel import WorkManager
    
    if ppservers : wm = WorkManager ( ppservers = ppservers )
    else         : wm = WorkManager ()

    print 'WorkManager: #cpus: ', wm.ncpus
        
    status = wm.process ( task , file_names ) 

    tmpdbases, badfiles = task.output 

    histomap = Kali.HistoMap()
    histomap.read ( tmpdbases )
    
    for f in tmpdbases :
        import os
        if os.path.exists ( f ) : os.remove ( f ) 

    if dbase_name :
        histomap.save ( dbase_name )
    
    return histomap,badfiles 

# ==============================================================================
## perform the regular ``sequential'' fill for the histograms 
def __fill_s_Histos ( lambdas                   ,
                      file_names                ,
                      dbase_name = 'kali_db.gz' ,
                      cellFunc   = lambda s : s ,
                      Unit       = MeV          ,
                      **args                    ) :
    """
    Perform the regular ``sequential'' fill for the histograms 
    """
    
    histomap, lambdas, badfiles = Fill.fillDataBase (
        self._lambdas           ,
        file_names              ,
        cellFunc     = cellFunc ,
        Unit         = Unit     , 
        **args )
    
    return histomap,badfiles

# ==============================================================================
## Fill for the histograms 
def fillHistos ( lambdas           ,
                 file_names        ,
                 parallel   = True ,
                 **args            ) :
    """
    Fill for the histograms
    """
    if parallel : return __fill_p_Histos ( lambdas , file_names , **args )
    else        : return __fill_s_Histos ( lambdas , file_names , **args )

    
# =============================================================================
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print ' __all__ : '    %  __all__ 
    print '*'*120  

# =============================================================================
# The END 
# =============================================================================


