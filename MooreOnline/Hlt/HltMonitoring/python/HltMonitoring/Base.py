# MultiProcessing
import os, sys
from multiprocessing import Process, Queue, Event, Condition, Lock

class Task( object ):
    def __init__( self, name ):
        self._name = name

    def configure( self ):
        pass

    def initialize( self ):
        pass

    def run( self ):
        pass

    def finalize( self ) :
        pass

class ProcessData( object ):

    def __init__( self ):
        self._condition = Condition( Lock() )
        self._inQueue = Queue(100)
        self._outQueue = Queue(100)

    def getData( self, block = True, timeout = None ):
        try:
            self._data = self._outQueue.get( block, timeout )
            return self._data
        except IOError:
            return None
        
    def data( self ):
        return self._data

    def putData( self, data ):
        self._inQueue.put( data )

class ProcessWrapper( object ):

    def __init__( self, key, task, name, config, debug = False ):
        self._name = name
        self._key = key
        self._debug = debug
        self._name = name
        self._data = ProcessData()
        self._task = task( name, ( self._data._inQueue, self._data._outQueue ),
                           self._data._condition )
        self._config = config
        self._process = Process( target = self.run )

    def run( self):
        if self._debug == False:
            filename = '%s.log' % self._name
            if 'Run' in self._config:
                run = self._config[ 'Run' ]
                cwd = os.path.realpath( os.path.curdir )
                dirname = os.path.join( cwd, '%d' % run )
                filename = os.path.join( dirname, filename )
            old_stdout = os.dup( sys.stdout.fileno() ) 
            fd = os.open( filename, os.O_CREAT | os.O_WRONLY ) 
            os.dup2( fd, sys.stdout.fileno() ) 

        self._task.configure( self._config )
        self._task.initialize()
        self._task.run()
        self._task.finalize()

        if self._debug == False:
            os.close( fd ) 
            os.dup2( old_stdout, sys.stdout.fileno() )

    def name( self ):
        return self._name

    def key( self ):
        return self._key

    def start( self ):
        self._process.start()

    def join( self ):
        self._process.join()

    def data( self ) :
        return self._data.data()

    def condition( self ):
        return self._data._condition

    def fileno( self ):
        return self._data._outQueue._reader.fileno()

    def getData( self, block = True, timeout = None ):
        return self._data.getData( block, timeout )

    def putData( self, data ):
        self._data.putData( data )
