"""
Decorator to execute a method call in its own thread,
and wait for the result (passed back through a Pipe).
"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "August 2012"
__all__    = ( "RunInSeparateThread" )

import multiprocessing as mp

import logging
import os

class RunInSeparateThread(object):
    """
    Decorator to run a method in a separate thread and return its value
    """
    def __init__( self, f ):
        self._f = f
    def wrappedMethod( self, p, fargs, fkwargs ):
        """
        The method running in the separate thread, sends the output of f through the connection p
        """
        logging.info( "Running %s( %s ) @ %i" % ( self._f.__name__, ", ".join( [ str(a) for a in fargs ] + [ "%s=%s" % (k, v) for k, v in fkwargs.iteritems() ]), os.getpid() ) )
        ret = self._f(*fargs, **fkwargs)
        p.send(ret)
        p.close()
    def __call__( self, *args, **kwargs ):
        """
        Decorated method: calls a wrapped version of f with args and kwargs, waits for the output (through a pipe) and returns it
        """
        pipeRecv, pipeSend = mp.Pipe()
        proc = mp.Process(name="Isolated process", target=self.wrappedMethod, args=(pipeSend, args, kwargs))
        proc.start()
        retVal = pipeRecv.recv()
        proc.join()
        return retVal
