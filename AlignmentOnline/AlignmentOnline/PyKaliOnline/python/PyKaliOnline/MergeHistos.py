


# =============================================================================
import GaudiMP.Parallel as Parallel  
# =============================================================================

# =============================================================================
## The simplest object for more efficient firll fo RooDataSet from TChain 
#  @see GaudiMP.Parallel
#  @see GaudiMP.Parallel.Task
#  @see Ostap.SelectorWithVars
#  For 12-core machine, clear speed-up factor of about 10 is achieved 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-09-23 
class  MergeDB(Parallel.Task) :
    """
    The simplest object for more efficient firll fo RooDataSet from TChain 
    """
    ## 
    def __init__ ( self , total = -1 ) :

        self.total    = total 
        self.output   = ()  

    def initializeLocal   ( self ) : pass
    def initializeRemote  ( self ) : pass

    ## the actual processing 
    def process ( self , chunk ) :

        from   KaliCalo.MassDistribution import HistoMap

        histos = HistoMap()
        self.output  = histos

        
        for entry in chunk :

            efile = entry[0]
            enum  = entry[1]
            
            histos.read ( efile )            
            print 'Processed %s \t#%d/%d\tEntries:%d\tHistos:%d' % (
                efile            ,
                enum             , 
                self.total       ,
                histos.entries() ,
                len(histos)      ) 

    def finalize ( self ) : pass 

    ## methge results
    def _mergeResults(self, result) :
        #
        import ROOT 
        from   KaliCalo.MassDistribution import HistoMap
        if not isinstance ( self.output , HistoMap ) :  
            self.output = result
            return
        
        self.output += result

        histos = self.output
        
        print 'MERGED:',histos.entries(), len(histos)
        


# =============================================================================
## split the list into chunks 
def chunks(l, n):
    n = max(1, n)
    return [l[i:i + n] for i in range(0, len(l), n)]

# ==============================================================================
## Fill dataset from long TChain using per-file parallelisation
#  >>> chain =
#  >>> vars  = ...
#  >>> dset  = fillDataSet ( chain , vars , 'pt>10' )
#  @endcode
#  @see GaudiMP.Parallel
#  @see GaudiMP.Parallel.Task
#  @see Ostap.SelectorWithVars
#  For 12-core machine, clear speed-up factor of about 10 is achieved 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-09-23 
def  mergeDBs ( databases , chunk_size = 60 ) :
    """
    Fill dataset from long TChain using per-file parallelisation
    
    >>> chain =
    >>> vars  = ...
    >>> dset  = fillDataSet ( chain , vars , 'pt>10' )
    
    """
    
    
    ntasks = len ( databases ) 
    tasks1 = [ (databases[i],i) for i in range( 0 , ntasks ) ]
    ## split it into chunks 
    tasks2 = chunks ( tasks1 ,  chunk_size ) 

    task   = MergeDB (  ntasks  )
    wmgr   = Parallel.WorkManager()
 
    print 'Try to use %d cores for %d tasks in %d chunks ' % ( wmgr.ncpus , ntasks , len(tasks2) ) 
    import sys 
    sys.stdout.flush() 
    
    
    
    wmgr.process( task, tasks2  )
    return task.output
