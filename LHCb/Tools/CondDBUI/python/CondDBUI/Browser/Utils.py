## Utility functions useful across the various modules of the browser

import time
from PyQt4.QtCore import Qt, QDateTime, QDate, QTime
from PyQt4.QtGui import QApplication

# Number of times to indent output
# A list is used to force access by reference
__report_indent = [0]
def report(fn):
    """Decorator to print information about a function
    call for use while debugging.
    Prints function name, arguments, and call number
    when the function is called. Prints this information
    again along with the return value when the function
    returns.
    """
    def wrap(*params,**kwargs):
        call = wrap.callcount = wrap.callcount + 1
        indent = ' ' * __report_indent[0]
        fc = "%s(%s)" % (fn.__name__, ', '.join(
            [a.__repr__() for a in params] +
            ["%s = %s" % (a, repr(b)) for a,b in kwargs.items()]
        ))
        print "%s%s called [#%s]" % (indent, fc, call)
        __report_indent[0] += 1
        ret = fn(*params,**kwargs)
        __report_indent[0] -= 1
        print "%s%s returned %s [#%s]" % (indent, fc, repr(ret), call)
        return ret
    wrap.callcount = 0
    return wrap

def report_(fn):
    """Decorator to print information about a function
    call for use while debugging.
    Prints function name, arguments, and call number
    when the function is called. Prints this information
    again along with the return value when the function
    returns.
    """
    def wrap(*params,**kwargs):
        #call = wrap.callcount = wrap.callcount + 1
        #indent = ' ' * __report_indent[0]
        #fc = "%s(%s)" % (fn.__name__, ', '.join(
        #    [a.__repr__() for a in params] +
        #    ["%s = %s" % (a, repr(b)) for a,b in kwargs.items()]
        #))
        #print "%s%s called [#%s]" % (indent, fc, call)
        #__report_indent[0] += 1
        ret = fn(*params,**kwargs)
        #__report_indent[0] -= 1
        #print "%s%s returned %s [#%s]" % (indent, fc, repr(ret), call)
        return ret
    #wrap.callcount = 0
    return wrap


## Small function useful to temporarily add debug printouts
def DEBUG(*args):
    print "DEBUG:", ", ".join(args)

## Helper function to convert a cool::ValidityKey to a QDateTime (UTC).
def valKeyToDateTime(valkey):
    # Cannot use setTime_t because of the limited range.
    timeTuple = time.gmtime(valkey / 1e9)
    d = apply(QDate, timeTuple[0:3])
    t = apply(QTime, timeTuple[3:6])
    return QDateTime(d, t, Qt.UTC)

## Utility function to extract the basename from a path
def basename(path):
    if len(path) > 1:
        path = path.split("/")[-1]
    return path

## Utility function to extract the name of the parent node from a path
def parentpath(path):
    if len(path) == 1: # root node
        return None
    parent = path.rsplit("/",1)[0]
    if not parent:
        parent = "/"
    return parent

## Guard-like class to change the cursor icon during operations that may take a
#  long time.
class BusyCursor(object):
    ## Constructor, sets the application cursor to Qt.WaitCursor.
    def __init__(self):
        QApplication.setOverrideCursor(Qt.WaitCursor)
    ## Destructor, restore the application cursor.
    def __del__(self):
        QApplication.restoreOverrideCursor()

