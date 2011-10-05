"""
Module for handy threading classes
"""
import threading, Queue

class LockOnPrintThread(threading.Thread):
    """
    A threading class which locks before printing a result,
    method should be replaced with your own method returning a
    string for printing
    """
    def __init__(self, pool, lock):
        """
        pool should be a Queue.Queue object
        lock, a thread.lock object
        """
        threading.Thread.__init__(self)
        self.lock=lock
        self.pool=pool
        self.done=False

    def run(self):
        """
        The main sequence, method, lock, print, unlock
        """
        while not self.done:
            try:
                item=self.pool.get_nowait()
                if item is not None:
                    result=self.method(self,item)
                    self.lock.acquire()
                    print result.strip()
                    self.lock.release()
            except Queue.Empty:
                self.done=True

    def method(self,item):
        """
        A const method on any class variables, must return a string
        or an exception
        """
        raise ImportError, "do not call the baseclass method!"

