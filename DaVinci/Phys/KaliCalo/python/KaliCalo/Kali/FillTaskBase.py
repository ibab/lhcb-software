#!/usr/bin/env python
# =============================================================================
# @file FillTaskBase.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @based on Vanya Belyaev's TreeTask.py
# @date 2010-11-11
# =============================================================================
"""
The helper class for parallel filling of historgams using GaudiMP.Parallel
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " $Date$"
__version__ = " $Revision$"

__all__     = [ "FillTask" , "FillHistos" ]

# =============================================================================
import os
import tempfile
import sets

import GaudiMP.Parallel          as     Parallel
from   GaudiKernel.SystemOfUnits import MeV

from KaliCalo.FakeCells           import SameCell
# =============================================================================

## @class FillTask 
#  The helper class for parallel filling of histograms using GaudiMP.Parallel
#  @see GaudiPyhon.Parallel
#  @see GaudiPyhon.Parallel.Task
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-27
class FillTask ( Parallel.Task ) :
    """
    The helper class for parallel filling of historgams
    using GaudiMP.Parallel
    
    @author Vanya BELYAEV Ivan.Belyaev@itep.ru
    @date 2010-03-28
    """
    
    ## constructor from lambda-map 
    def __init__ ( self         ,
                   fillDatabase ,
                   lambdas      ,
                   cellFunc     , 
                   Unit = MeV   ) : 
        
        ## initialize the base class 
        Parallel.Task.__init__ ( self )
        self._fillDatabase = fillDatabase
        self._lambdas      = lambdas
        self._Unit         = Unit  
        self._cellFunc     = cellFunc
            
    ## local initialization 
    def initializeLocal ( self ) :
        """
        Local initialization
        """
        print 'FillTask : Start parallel processing (local)'
        self.output = [ None , sets.Set() ]
                        
    ## local initialization 
    def initializeRemote ( self ) :
        """
        Remote initialization
        """
        host = os.environ['HOSTNAME']
        #
        print 'FillTask : Start parallel processing (%s) ' % host
        return Parallel.Task.initializeRemote ( self ) 
        
    ## process the list of histo sets 
    def process  ( self , file_pair ) :
        
        """
        Process the list of histo-sets 
        """
        host = os.environ['HOSTNAME']
        #
        print 'FillTask: Start processing at ', host
        
        files,dbase = file_pair 
        
        histos, lambdas, badfiles = self._fillDatabase (
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
            
        print 'FillTask: End   processing at ', host, histos.entries() 
            
    ## finalization of the task 
    def finalize ( self ) :
        """
        Finalization of the task
        """
        if hasattr ( self , 'histos' )  : 
            print 'FillTask.finalize TOTAL: ' , self.histos.entries()
        else :
            print 'FillTask.finalize'
    

    ## merge the result form the different processes 
    def _mergeResults ( self, result ) :
        """
        merge the results from the different processes
        """
        import os 
        if not hasattr ( self , 'histos' ) :
            self.histos = self._fillDatabase._HistoMapClass ()        
        if os.path.exists ( result[0] ) :
            ## self.histos.read( result[0] )
            self.histos.updateFromDB ( result[0] )
            os.remove ( result[0] )
            print 'MERGE : ', result[0], self.histos.entries()
        else :
            print 'NON_EXISTING PATH', result[0] 
        for f in result[1] : self.output[1].add ( f ) 
            
    def _resetOutput ( self ) :
        self.output[1].clear() 

# ==============================================================================
## Fill for the histograms     
class FillHistos( object ):
    def __init__ ( self , fillDatabase ):
        self._fillDatabase = fillDatabase

    def __call__( self ,
                  lambdas                           ,
                  file_names                        ,
                  manager    = None                 , 
                  cellFunc   = SameCell ()          , 
                  **args                          ) :
        if manager :
            return self.__fill_p_Histos (
                    lambdas                           ,
                    file_names                        ,
                    manager  = manager                ,
                    cellFunc = cellFunc               , 
                    **args                            )
        else         :
            return self.__fill_s_Histos   (
                    lambdas               ,
                    file_names            ,
                    cellFunc = cellFunc   ,
                    **args                )

    # ==============================================================================
    ## perform the regular ``sequential'' fill for the histograms 
    def __fill_s_Histos ( self                         ,
                          lambdas                      ,
                          file_names                   ,
                          cellFunc                     ,
                          dbase_name = 'kali_s_db.gz'  ,
                          Unit       = MeV             ,
                          **args                       ) :
        """
        Perform the regular ``sequential'' fill for the histograms 
        """

        histomap, lambdas, badfiles = self._fillDatabase (
                                lambdas                  ,
                                file_names               ,
                                dbase_name  = dbase_name ,
                                cellFunc    = cellFunc   ,
                                Unit        = Unit       , 
                                **args )

        return histomap,badfiles

    # ==============================================================================
    ## perform the parallel fill for the histograms 
    def __fill_p_Histos ( self                         ,
                          lambdas                      ,
                          file_names                   ,
                          manager                      ,
                          cellFunc                     ,   
                          dbase_name = 'kali_p_db.gz'  ,
                          Unit       = MeV             ) :
        """
        Perform the ``parallel'' fill for the histograms 
        """

        if issubclass ( type ( file_names ) , str ) :
            file_names = [ file_names ] 
            
        tmpdir = None
        if 'cluster' == manager.mode :  
            tmpdir = os.path.abspath('./')

        task = FillTask ( self._fillDatabase  ,
                          lambdas             ,
                          cellFunc = cellFunc ,
                          Unit     = Unit     ) 

        ## prepare the names for temporary files 
        tmps = set()
        while len ( tmps ) < len ( file_names ) :
            tmp = tempfile.mktemp ( dir    = tmpdir    ,
                                    prefix = 'tmp_'    ,
                                    suffix = '_zdb.gz' )
            if not os.path.exists ( tmp ) :
                tmps.add ( tmp ) 


        file_pairs = zip ( file_names , tmps )

        status = manager.process ( task , file_pairs ) 

        histomap  = task.histos
        badfiles  = task.output[1] 

        if dbase_name :
            print 'Save histos in ', dbase_name 
            histomap.save ( dbase_name )

        return histomap,badfiles
        
# EOF
