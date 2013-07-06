#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with some simple but useful utilities 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-02-10
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Module with some simple but useful utilities 

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2013-02-10"
# =============================================================================
__all__     = (
    #
    'virtualMemory'  , ## context manager to count virtual memory increase 
    'memory'         , ## ditto
    'clocks'         , ## context manager to count clocks 
    'timing'         , ## context manager to count time 
    'timer'          , ## ditto
    'mute_c'         , ## contex manager to suppress stdout/strerr printout 
    'silence_c'      , ## ditto 
    'mute_py'        , ## contex manager to suppress stdout/strerr printout 
    'silence_py'     , ## ditto 
    'mute'           , ## contex manager to suppress stdout/strerr printout 
    'silence'        , ## ditto 
    )    
# =============================================================================
import ROOT, time, os,sys  
from   GaudiPython.Bindings   import gbl as cpp
# =============================================================================
## @class Memory
#  Simple context manager to measure the virtual memory increase
#
#  @see System::virtualMemory
#  @code
#
#  with Memory() :
#     <whatever action is>
#     at the exit it prints the chaneg in virtual memory 
#  @endcode
#
# Or:
#
#  @code
#
#  with Memory() as Q :
#     <whatever action is>
#     at the exit it prints the chaneg in virtual memory
#
#  print Q.delta 
# 
#  @endcode
#
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10
class Memory(object):
    """
    Simple class to evaluate the change in virtual memory
    to be used as context manager:
    
    with Memory('here...') :
    #   <whatever action is>
    #    at the exit it prints the change in virtual memory
    
    with Memory('here...') as M :
    #   <whatever action is>
    #    at the exit it prints the change in virtual memory
    
    >>> delta = M.delta
    
    """    
    def __init__  ( self , name = '' ) :
        self.name   = name
    def __enter__ ( self ) :
        self.memory = cpp.System.virtualMemory()
    def __exit__  ( self, exc_type, exc_value, traceback) :
        if not exc_type :
            if not  exc_value :
                if not traceback :
                    self.delta = cpp.System.virtualMemory() - self.memory
                    print  '%s Memory %.1fMB ' % ( self.name , 0.001*self.delta ) 


# ============================================================================
## create the context manager to monitor the virtual memory increase  
def virtualMemory ( name = '' ) :
    """
    Create the context manager to monitor the virtual memory increase:

    with memory('here...') :
    #     <whatever action is>
    #      at the exit it prints the change in virtual memory
          
    with memory('here...') as m :
    #     <whatever action is>
    #      at the exit it prints the change in virtual memory
          
    >>> delta = m.delta
    
    """
    return Memory( name )

## ditto 
memory = virtualMemory  ## ditto

# =============================================================================
## @class Clock
#  Smple context manager to measure the clock counts
#
#  @code
#
#  with Clock() :
#     <whatever action is>
#     at the exit it prints the clock counts 
#  @endcode
#
# Or:
#
#  @code
#
#  with Clock() as c :
#     <whatever action is>
#     at the exit it prints the clock counts 
#
#  print c.delta 
# 
#  @endcode
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10                    
class Clock(object):
    """
    Smple context manager to measure the clock counts
    
    with Clock() :
    #    <whatever action is>
    #    at the exit it prints the clock counts 
    
    with Clock() as c :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    >>> print c.delta 
    """
    def __init__  ( self , name = '' ) :
        self.name   = name
    def __enter__ ( self ) :
            self.clock = time.clock() 
    def __exit__  ( self, exc_type, exc_value, traceback) :
        if not exc_type :
            if not  exc_value :
                if not traceback :
                    self.delta = time.clock() - self.clock
                    print  '%s Clocks %s ' % ( self.name , self.delta ) 

# =============================================================================
## @class Timer
#  Simple context manager to measure the time 
#  @code
#
#  with Timer() :
#     <whatever action is>
#     at the exit it prints the time 
#  @endcode
#
# Or:
#
#  @code
#
#  with Timer() as t :
#     <whatever action is>
#     at the exit it prints the clock counts 
#
#  print ct.delta 
# 
#  @endcode
#
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10
#
class Timer(object):
    """
    Simple context manager to measure the time
    
    with Timer() :
    #     <whatever action is>
    #     at the exit it prints the time 
    
    Or:
    
    with Timer() as t :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    >>> print ct.delta 
    """
    def __init__  ( self , name = '' ) :
        self.name   = name
    def __enter__ ( self ) :
        self.time = time.time() 
    def __exit__  ( self, exc_type, exc_value, traceback) :
        if not exc_type :
            if not  exc_value :
                if not traceback :
                    self.delta = time.time() - self.time
                    print  '%s Time   %s ' % ( self.name , self.delta ) 
                    

# =============================================================================
## Simple context manager to measure the clock counts
#
#  @code
#
#  with clocks () :
#     <whatever action is>
#     at the exit it prints the clock counts
#
#  @endcode
#
# Or:
#
#  @code
#
#  with clocks () as c :
#     <whatever action is>
#     at the exist it prints the clock counts 
#
#  print c.delta 
# 
#  @endcode
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10                    
def clocks ( name = '' ) :
    """
    Simple context manager to measure the clock counts 
    
    with clocks () :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    with clocks () as c :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    >>>print c.delta
    """
    return Clock ( name )


# =============================================================================
## Simple context manager to measure the time
#
#  @code
#
#  with timer () :
#     <whatever action is>
#     at the exit it prints the time
#
#  @endcode
#
# Or: 
#
#  @code
#
#  with timer () as t :
#     <whatever action is>
#     at the exit it prints the clock counts 
#
#  print t.delta 
# 
#  @endcode
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10                    
def timing ( name = '' ) :
    """
    Simple context manager to measure the clock counts 
    
    with timing () :
    #    <whatever action is>
    #    at the exit it prints the clock counts 
    
    with timing () as c :
    #    <whatever action is>
    #    at the exit it prints the clock counts 
    
    >>> print c.delta
    """
    return Timer ( name )

# =============================================================================
## ditto 
timer = timing   # ditto

# ============================================================================
## @class MutePy
#  context manager to suppress python printout 
class MutePy(object):
    """
    A context manager for doing a ``deep suppression'' of stdout and stderr in 
    Python, i.e. will suppress all print, even if the print originates in a 
    compiled C/Fortran sub-function.
    This will not suppress raised exceptions, since exceptions are printed
    to stderr just before a script exits, and after the context manager has
    exited (at least, I think that is why it lets exceptions through).      
    
    stallen from  
    http://stackoverflow.com/questions/11130156/suppress-stdout-stderr-print-from-python-functions
    """
    def __init__( self , out = True , err = False ):
        self._out = out
        self._err = err
        
    def __enter__(self):
        #
        ## helper class to define empty stream 
        class Silent(object):
            def write(self,*args,**kwards) : pass

        self.stdout = sys.stdout
        self.stderr = sys.stderr
        if self._out : sys.stdout = Silent() 
        if self._err : sys.stderr = Silent() 

    def __exit__(self, *_):
        
        sys.stdout = self.stdout
        sys.stderr = self.stderr
        
# ============================================================================
## @class MuteC
#  context manager to suppress pythion prinout
#  the actual code is stallen from
#  http://stackoverflow.com/questions/11130156/suppress-stdout-stderr-print-from-python-functions
#  A fix is added for "IOError: [Errno 24] Too many open files" :
#  original code leaks the file descriptors
class MuteC(object):
    """
    A context manager for doing a ``deep suppression'' of stdout and stderr in 
    Python, i.e. will suppress all print, even if the print originates in a 
    compiled C/Fortran sub-function.
    This will not suppress raised exceptions, since exceptions are printed
    to stderr just before a script exits, and after the context manager has
    exited (at least, I think that is why it lets exceptions through).      
    
    stallen from  
    http://stackoverflow.com/questions/11130156/suppress-stdout-stderr-print-from-python-functions
    """
    #
    ## class variables: dev-null device & instance counter 
    _devnull = 0
    _cnt     = 0
    
    def __init__( self , out = True , err = False ):
        
        self._out = out
        self._err = err

        # increment instance counter 
        self.__class__._cnt += 1

        # create dev-null if not done yet 
        if not self.__class__._devnull :
            self.__class__._devnull = os.open ( os.devnull , os.O_WRONLY )            

    def __del__  ( self ) :

        
        # decrement instance counter 
        self.__class__._cnt -= 1
        
        # close dev-null if not done yet 
        if self.__class__._cnt <= 0 and self.__class__._devnull : 
            os.close ( self.__class__._devnull  )
            self.__class__._devnull = 0
            

    ## context-manager 
    def __enter__(self):
        
        #
        ## Save the actual stdout (1) and stderr (2) file descriptors.
        #
        self.save_fds =  os.dup(1), os.dup(2)  # leak was here !!!
        
        #
        ## mute it!
        #
        if self._out : os.dup2 ( self.__class__._devnull , 1 )  ## C/C++
        if self._err : os.dup2 ( self.__class__._devnull , 2 )  ## C/C++

    ## context-manager 
    def __exit__(self, *_):
        
        #
        # Re-assign the real stdout/stderr back to (1) and (2)  (C/C++)
        #
        
        if self._err : os.dup2 ( self.save_fds[1] , 2 )
        if self._out : os.dup2 ( self.save_fds[0] , 1 )
        
        # fix the  file descriptor leak
        # (there were no such line in example, and it causes
        #      the sad:  "IOError: [Errno 24] Too many open files"
        
        os.close ( self.save_fds[1] ) 
        os.close ( self.save_fds[0] )
                
# ============================================================================
## @class MuteAll
#  context manager to suppress All (C/C++/Python) printout
class MuteAll(object):
    """
    A context manager to suppress All  (C/C++/Python) printout
    """    
    def __init__( self , out = True , err = False ):
        self._c    = MuteC  ( out , err ) 
        self._py   = MutePy ( out , err ) 
        
    def __enter__(self):

        self._c .__enter__ ( )
        self._py.__enter__ ( )
        
    def __exit__(self, *_):
        
        self._py.__exit__  ( *_ )
        self._c .__exit__  ( *_ )
        
# =============================================================================
## simple context manager to suppress all(C/C++/Python) printout
#
#  @code
#  with mute_all () :
#          <some code here>
#  @endcode 
def mute_all ( cout = True , cerr = False )   :
    """
    Simple context manager to suppress All printout
    
    with mute_all () :
    # <some code here>
    #
    """
    return MuteAll ( cout , cerr )

# =============================================================================
## simple context manager to suppressC/C++-printout
#
#  @code
#  with mute_c () :
#          <some code here>
#  @endcode 
def mute_c ( cout = True , cerr = False )   :
    """
    Simple context manager to suppress All printout
    
    with mute_c () :
    # <some code here>
    #
    """
    return MuteC ( cout , cerr )

# =============================================================================
## simple context manager to suppress Python-printout
#
#  @code
#  with mute_py () :
#          <some code here>
#  @endcode 
def mute_py ( cout = True , cerr = False )   :
    """
    Simple context manager to suppress All printout
    
    with mute_Py () :
    # <some code here>
    #
    """
    return MutePy ( cout , cerr )

# ==============================================================================
## ditto 
silence_c   = mute_c   # ditto
silence_py  = mute_py  # ditto
silence_all = mute_all # ditto

mute        = mute_all # ditto  
silence     = mute     # ditto

# =============================================================================
## get all open file descriptors
#  The actual code is copied from http://stackoverflow.com/a/13624412
def get_open_fds():
    """
    Get all open file descriptors
    
    The actual code is copied from http://stackoverflow.com/a/13624412
    """
    #
    import resource
    import fcntl
    #
    fds = []
    soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
    for fd in range(0, soft):
        try:
            flags = fcntl.fcntl(fd, fcntl.F_GETFD)
        except IOError:
            continue
        fds.append(fd)
    return fds

# =============================================================================
## get the actual fiel name form file descriptor 
#  The actual code is copied from http://stackoverflow.com/a/13624412
#  @warning: it is likely to be "Linux-only" function
def get_file_names_from_file_number(fds):
    """
    Get the actual file name from file descriptor 
    The actual code is copied from http://stackoverflow.com/a/13624412 
    """
    names = []
    for fd in fds:
        names.append(os.readlink('/proc/self/fd/%d' % fd))
    return names


# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
