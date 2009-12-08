## Utility functions useful across the various modules of the browser

from datetime import datetime, timedelta
from PyQt4.QtCore import Qt, QDateTime
from PyQt4.QtGui import QApplication

__all__ = ["report", "DEBUG",
           "valKeyToDateTime", "dateTimeToValKey",
           "basename", "parentpath",
           "BusyCursor"]

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

## Small function useful to temporarily add debug printouts
def DEBUG(*args):
    print "DEBUG:", ", ".join(args)

_epoch = datetime(1970, 1, 1, 0, 0)
## Helper function to convert a cool::ValidityKey to a QDateTime (UTC).
def valKeyToDateTime(valkey):
    dt = _epoch + timedelta(seconds = valkey / 1e9)
    dt = QDateTime(dt)
    dt.setTimeSpec(Qt.UTC)
    return dt

## Helper function to convert a QDateTime to a cool::ValidityKey.
def dateTimeToValKey(dt):
    dt = dt.toUTC()
    dt = datetime(dt.date().year(), dt.date().month(), dt.date().day(),
                  dt.time().hour(), dt.time().minute(), dt.time().second(),
                  dt.time().msec() * 1000)
    delta = dt - _epoch
    valkey = (delta.days * 24 * 3600 + delta.seconds) * 1000000000 + delta.microseconds * 1000
    return valkey

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

