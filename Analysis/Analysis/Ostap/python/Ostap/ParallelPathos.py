#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file ParallelPathos.py 
#
#  Useful utilities for multiprocessing and parallel processing for Ostap
#  Actualy it is just a little bit upgraded version of original
#  GaudiMP.Parallel module developed by Pere MATO Pere.Mato@cern.ch
#
#  The original module relies on some obsolete python modules (e.g. pyssh)
#  and it has limitations related ti the pickling.
#
#  The upgraded module relies on <code>pathos</code> suite that
#  has very attratcive functionality and solve pickling issues
#  @see https://github.com/uqfoundation/pathos
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-02-23
#
#                    $Revision:$
#  Last modification $Date:$
#  by                $Author:$
# =============================================================================
"""   Useful utilities for multiprocessing and parallel processing for Ostap
Actualy it is just a little bit upgraded version of original
GaudiMP.Parallel module developed by Pere MATO Pere.Mato@cern.ch

The original module relies on some obsolete python modules (e.g. pyssh)
and it has limitations related ti the pickling.

The upgraded module relies on <code>pathos</code> suite that
has very attractive functionality and solve pickling issues
@see https://github.com/uqfoundation/pathos
"""
# =============================================================================
__version__ = '$Revision:$'
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2016-02-23'
__all__     = (
    'Task'        , ## the base class for task
    'TaskManager' , ## task manager 
    )
# =============================================================================
from Ostap.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'Ostap.ParalllelPathos' )
else                      : logger = getLogger ( __name__                ) 
# =============================================================================
import sys, os, time, copy
excluded_varnames =  ( 'HOSTNAME', 'SSH_CLIENT', 'SSH_CONNECTION', 'DISPLAY' ) 
# =============================================================================
try:
    
    ## try to use bare import 
    import pathos.multiprocessing as MP
    import pathos.core            as PC 
    import pp                     as PP
    
except ImportError :
    
    ## try to import with virtual env
    venv_file = '/afs/cern.ch/user/i/ibelyaev/cmtuser/venv/bin/activate_this.py'
    execfile( venv_file , dict( __file__ = venv_file ) )
    
    ## try import again 
    import pathos.multiprocessing as MP
    import pathos.core            as PC 
    import pp                     as PP



def _prefunction( f, task, item) :
    return f((task,item))

def _ppfunction( args ) :
    #--- Unpack arguments
    task, item = args
    stat = Statistics()
    #--- Initialize the remote side (at least once)
    if not task.__class__._initializeDone :
        for k,v in task.environ.items() :
            if k not in excluded_varnames : os.environ[k] = v
        task.initializeRemote()
        task.__class__._initializeDone = True
    #--- Reset the task output
    task._resetOutput()
    #--- Call processing
    task.process(item)
    #--- Collect statistics
    stat.stop()
    return (copy.deepcopy(task.output), stat)

# =============================================================================
## @class Statistics
#  helper class to collect statistics 
#  @author Pere MATO Pere.Meto@cern.ch
class Statistics(object):
    """Helper class to collect statistics
    """
    def __init__(self):
        import time, os 
        self.name  = os.getenv('HOSTNAME')
        self.start = time.time()
        self.time  = 0.0
        self.njob  = 0
    def stop ( self ) :
        import time
        self.time = time.time() - self.start
            

# =============================================================================
## @class Task
#  Basic base class to encapsulate any processing that is going to be porcessed in parallel.
#  User class much inherit from it and implement the methods initializeLocal,
#  initializeRemote, process and finalize.
#  @author Pere MATO Pere.Meto@cern.ch
class Task(object) :
    """ Basic base class to encapsulate any processing that is going to be porcessed in parallel.
        User class much inherit from it and implement the methods initializeLocal,
        initializeRemote, process and finalize.   """
    _initializeDone = False
    def __new__ ( cls, *args, **kwargs ):
        task = object.__new__( cls )
        task.output = ()
        task.environ = {}
        for k,v in os.environ.items(): task.environ[k] = v  
        task.cwd = os.getcwd()
        return task
    def initializeLocal(self):
        pass
    def initializeRemote(self):
        pass
    def process(self, item):
        pass
    def finalize(self) :
        pass
    def _mergeResults(self, result) :
        
        if type(result) is not type(self.output) :
            raise TypeError("output type is not same as obtained result")
        
        #--No iteratable---
        if not   hasattr (      result , '__iter__' ) :
            if   hasattr ( self.output ,    'Add'   ) : self.output.Add( result )
            elif hasattr ( self.output , '__iadd__' ) : self.output += result
            elif hasattr ( self.output , '__add__'  ) : self.output = self.output + result
            else : raise TypeError('result cannot be added')
            
        #--Dictionary---
        elif type ( result ) is dict :

            for key in result.keys() :
                if key in self.output :
                    out = self.output[key]                    
                    if   hasattr ( out ,'Add'      ) : out.Add(result[key])
                    elif hasattr ( out ,'__iadd__' ) : out += result[key]
                    elif hasattr ( out ,'__add__'  ) : out  = out + result[key]
                    else : raise TypeError('result cannot be added')
                else :
                    self.output[key] = result[key]
        #--Anything else (list)
        else :
            for i in range( min( len(self.output) , len(result)) ):
                out = self.output[i] 
                if   hasattr ( out ,    'Add'   ) : out .Add(result[i])
                elif hasattr ( out , '__iadd__' ) : out += result[i]
                elif hasattr ( out , '__add__'  ) : out  = out + result[i]
                else : raise TypeError('result cannot be added')
    def _resetOutput(self):
        output =  (type(self.output) is dict) and self.output.values() or self.output
        for o in output :
            if hasattr( o , 'Reset'): o.Reset()

## import pp
## for i in range(1000):
##     job_server = pp.Server(restart=True)
##     for worker in job_server._Server__workers:
##         worker.is_free = False
##         worker.t.send('EXIT')
##         worker.t.close()
##     print 'Killed #%d'%(i)    
##     job_server.destroy()


# =============================================================================
## @class WorkManager
#  Class to in charge of managing the tasks and distributing them to
#  the workers. They can be local (using other cores) or remote
#  using other nodes in the local cluster """
#  @author Pere MATO Pere.Meto@cern.ch
class WorkManager(object) :
    """ Class to in charge of managing the tasks and distributing them to
        the workers. They can be local (using other cores) or remote
        using other nodes in the local cluster """
    def __init__( self, ncpus='autodetect', ppservers=None) :
        if ncpus == 'autodetect' : self.ncpus = MP.cpu_count()
        else :                     self.ncpus = ncpus
        if ppservers :
            self._ppservers = ppservers
            self.sessions   = [ ppServer(srv) for srv in ppservers ]
            self.ppservers  = tuple ( [ i.local_server for i in self.sessions ] )
            ##self.ppservers  = tuple ( [ i.remote_server for i in self.sessions ] )
            self.server     = PP.Server(ppservers=self.ppservers)
            self.mode       = 'cluster'
        else :
            self.pool = MP.Pool(self.ncpus)
            self.mode = 'multicore'
        self.stats = {}

    def __del__(self):
        if hasattr(self,'server') : self.server.destroy()

    def process(self, task, items, timeout=90000):
        if not isinstance(task,Task) :
            raise TypeError("task argument needs to be an 'Task' instance")
        # --- Call the Local initialialization
        task.initializeLocal()
        # --- Schedule all the jobs ....
        if self.mode == 'cluster' :
            jobs = [self.server.submit(_prefunction, (_ppfunction, task, item), (), ('Ostap.Parallel','time')) for item in items]
            ##jobs = [self.server.submit(_prefunction, (_ppfunction, task, item), (_ppfunction,), ('Ostap','time')) for item in items]
            for job in jobs :
                result, stat = job()
                task._mergeResults    ( result )
                self._mergeStatistics ( stat   )
            self._printStatistics()
            self.server.print_stats()
        elif self.mode == 'multicore' :
            start = time.time()
            jobs = self.pool.map_async(_ppfunction, zip([task for i in items] , items ))
            for result, stat in  jobs.get(timeout) :
                task._mergeResults(result)
                self._mergeStatistics(stat)
            end = time.time()
            self._printStatistics()
            logger.info ( 'Time elapsed since server creation %f' %(end-start) ) 
        # --- Call the Local Finalize
        task.finalize()
    def _printStatistics(self):
        njobs = 0
        for stat in self.stats.values():
            njobs += stat.njob
        logger.info ( 'Job execution statistics:' ) 
        logger.info ( 'job count | % of all jobs | job time sum | time per job | job server' ) 
        for name, stat  in self.stats.items():
            logger.info ( '       %d |        %6.2f |     %8.3f |    %8.3f | %s' % (stat.njob, 100.*stat.njob/njobs, stat.time, stat.time/stat.njob, name) ) 

    def _mergeStatistics(self, stat):
        if stat.name not in self.stats : self.stats[stat.name] = Statistics()
        s = self.stats[stat.name]
        s.time += stat.time
        s.njob += 1


# =============================================================================
## @class ppServer
#  helper class that starts <code>ppserver.py</code> on remote site
#  and makes SSH tunnel
#  @attention Password-less SSH-connection is required!
#  @code
#  pps = ppServer('lxplus009.cern.ch')
#  print pps.local_server, pps.remote_server
#  del pps
#  @endcode
#  ... or as context manager :
#  @code
#  with ppServer('lxplus009.cern.ch') as pps : 
#      print pps.local_server, pps.remote_server
#      ... do something here 
#  @endcode
#  @author Vanya BELYAEV Ivan.B
class ppServer(object) : 
    
    def __init__(self, hostname , **kwargs ) :

        # establish ssh tunnel 

        import sys
        sys.stdout.flush()
        sys.stdin .flush()

        self.tunnel  = PC.connect ( hostname )        
        remport = self.tunnel._rport

        ## needed ???
        self.input = open('/dev/null','r')
        
        command    = 'ppserver.py -p%d ' % remport
        secret     = kwargs.pop( 'secret' , '' )
        if secret : command += " -s%" % secret
        self.session = PC.execute (
            command              ,
            hostname             ,
            bg = True            , 
            options = '-q -T'    , 
            stdin   = self.input
            )
        
        r = self.session.response()
        if self.session._stdout :
            self.session._stdout.flush() 

            
        import time
        time.sleep(0.2) ## sleep... 

        self.hostname      = hostname 
        self.local_server  = 'localhost:%d' %              self.tunnel._lport 
        self.remote_server = '%s:%d'        % ( hostname , self.tunnel._rport  )
        
        target     = '[P,p]ython[^#]*'+'ppserver'
        self.pid   = PC.getpid( target , hostname )

        import sys
        sys.stdout.flush()
        sys.stdin .flush()        
        
        logger.info ('Make a tunnel %s <--> %s PID=#%d' % ( self.local_server  , self.remote_server , self.pid ) )   
        
    # Context manager:  ENTER
    def __enter__ ( self ) : return self

    ## Context manager:  EXIT
    #   - kill server and SSH session
    #   - disconnect SSH tunnel
    def __exit__  ( self ) :
        """Context manager:  EXIT
        - kill server and SSH session
        - disconnect SSH tunnel
        """
        
        if self.pid :
            import pathos.core as PC
            PC.kill ( self.pid , self.hostname )
            self.pid = None 
        if self.session : self.session.kill       ()
        if self.tunnel  : self.tunnel .disconnect () 
        if self.input   : self.input.close        ()
        self.session  = None
        self.tunnel   = None
        self.input    = None
            
    ## delete
    def __del__ ( self ) :

        self.__exit__()
        
        del self.session  
        del self.tunnel   
        del self.input
        

# =============================================================================
if '__main__' == __name__ :
    
    from Ostap.Line import line 
    logger.info ( __file__ + '\n' + line  )
    logger.info ( 80*'*' )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 


# =============================================================================
# The END 
# =============================================================================
