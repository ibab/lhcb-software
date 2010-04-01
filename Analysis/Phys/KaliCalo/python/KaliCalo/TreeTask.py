#!/usr/bin/env python
# =============================================================================
# $Id: TreeTask.py,v 1.3 2010-04-01 16:04:10 ibelyaev Exp $
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
__version__ = " CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $ "
__all__     = (
    "TreeTask"   ,
    "fillHistos"
    )
# =============================================================================
import GaudiPython.Parallel      as     Parallel
import KaliCalo.Pi0FillHistos    as     Fill
import KaliCalo.Kali             as     Kali 
from   GaudiKernel.SystemOfUnits import MeV
# =============================================================================

## @class TreeTask 
#  The helper class for parallel filling of histograms using GaudiPython.Parallel
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
    def __init__ ( self                       ,
                   lambdas                    ,
                   Unit     = MeV             ,
                   cellFunc = Kali.SameCell() ) :
        
        ## initialize the base class 
        Parallel.Task.__init__ ( self )
        self._lambdas  = lambdas
        self._Unit     = Unit  
        self._cellFunc = cellFunc
            
    ## local initialization 
    def initializeLocal ( self ) :
        """
        Local initialization
        """
        print 'FillTask : Start parallel processing (local)'
        import sets 
        self.output = [ None , sets.Set() ]
                        
    ## local initialization 
    def initializeRemote ( self ) :
        """
        Local initialization
        """
        print 'FillTask : Start parallel processing (remote) '
        return Parallel.Task.initializeRemote ( self ) 
        
    ## process the list of histo sets 
    def process  ( self , file_pair ) :
        
        """
        Process the list of histo-sets 
        """

        files,dbase = file_pair 
        
        histos, lambdas, badfiles = Fill.fillDataBase (
            self._lambdas               ,
            file_names = files          ,
            dbase_name = None           ,
            cellFunc   = self._cellFunc ,
            Unit       = self._Unit     )

        histos.save ( dbase )

        self.output[0] = dbase
        for b in badfiles :
            self.output[1].add (  b )

        if hasattr ( self , 'histos' ) :
            print 'I have HISTOS attribute!!!' , len ( self.histos ) , self.histos.entries() 
            
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
        import os 
        if not hasattr ( self , 'histos' ) :
            self.histos = Kali.HistoMap()        
        if os.path.exists ( result[0] ) :
            self.histos.read( result[0] )
            os.remove ( result[0] )
        else :
            print 'NON_EXISTING PATH', result[0] 
        for f in result[1] : self.output[1].add ( f ) 
            
    def _resetOutput ( self ) :
        self.output[1].clear() 

__managers = [] 
# ==============================================================================
## perform the parallel fill for the histograms 
def __fill_p_Histos ( lambdas                      ,
                      file_names                   ,
                      manager                      ,
                      dbase_name = 'kali_p_db.gz'  ,
                      cellFunc   = Kali.SameCell() , 
                      Unit       = MeV             ) :
    """
    Perform the ``parallel'' fill for the histograms 
    """

    if issubclass ( type ( file_names ) , str ) : file_names = [ file_names ] 

    import os
    import tempfile
    
    tmpdir = None
    if 'cluster' == manager.mode :  
        tmpdir = os.path.abspath('./')
        
    task = TreeTask ( lambdas             ,
                      Unit     = Unit     ,
                      cellFunc = cellFunc )

    ## prepare the names for temporary files 
    import sets
    tmps = sets.Set()
    while len ( tmps ) < len ( file_names ) :
        tmp = tempfile.mktemp ( dir    = tmpdir    ,
                                prefix = 'tmp_'    ,
                                suffix = '_zdb.gz' )
        if not os.path.exists ( tmp ) : tmps.add ( tmp ) 


    file_pairs = zip ( file_names , tmps )
            
    status = manager.process ( task , file_pairs ) 

    histomap  = task.histos
    badfiles  = task.output[1] 

    if dbase_name :
        print 'Save histos in ', dbase_name 
        ##histomap.save ( dbase_name )
        
    return histomap,badfiles 

# ==============================================================================
## perform the regular ``sequential'' fill for the histograms 
def __fill_s_Histos ( lambdas                      ,
                      file_names                   ,
                      dbase_name = 'kali_s_db.gz'  ,
                      cellFunc   = Kali.SameCell() , 
                      Unit       = MeV             ,
                      **args                       ) :
    """
    Perform the regular ``sequential'' fill for the histograms 
    """
    
    histomap, lambdas, badfiles = Fill.fillDataBase (
        lambdas                 ,
        file_names              ,
        cellFunc     = cellFunc ,
        Unit         = Unit     , 
        **args )
    
    return histomap,badfiles

# ==============================================================================
## Fill for the histograms 
def fillHistos ( lambdas           ,
                 file_names        ,
                 manager    = None , 
                 **args            ) :
    """
    Fill for the histograms
    """
    ##
    if manager :
        return __fill_p_Histos (
            lambdas           ,
            file_names        ,
            manager = manager , 
            **args            )
    else         :
        return __fill_s_Histos ( lambdas , file_names , **args )

    
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


