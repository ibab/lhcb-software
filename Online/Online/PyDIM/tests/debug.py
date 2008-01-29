import sys, string

#global _DEBUG, _VERBOSE, _ERROR
_DEBUG   = 1
_VERBOSE = 1
_ERROR   = 1

__private = 'test'

def setDebug(value):
    global _DEBUG
    _DEBUG = value

def getCallerID(level):
    """
    Returns a string containing informations about the function that called it
    at a desired level.
    level = how many level up should the tracing of this function start.
    """
    try:
        raise FakeException("")
    except:
        func = sys.exc_info()[2].tb_frame
    while level >= 0:
        func = func.f_back
        level -= 1
    obj = func.f_locals.get("self", None)
    functionName = func.f_code.co_name
    if obj:
        callStr = obj.__class__.__name__+"::"+func.f_code.co_name+" (line "+\
                                                     str(func.f_lineno)+")"
    else:
        callStr = func.f_code.co_name+" (line "+str(func.f_lineno)+")"
    return callStr


def LAST_EXCEPTION():
    return str(sys.exc_info()[0]) + " :" + str (sys.exc_info()[1])

def SAY(*args):
    """Prints messages adding the caller class name and the line of where the call was invoked.
    Behaviour can be changed (on/off) by modifying the _VERBOSE global var.
    """
    if _VERBOSE:
        msg = getCallerID(1) + ' ' +string.join(map(str, args))
        print msg

def ERROR(*args):
    """Prints messages adding the caller class name and the line of where the call was invoked.
    Behaviour can be changed (on/off) by modifying the _ERROR global var.
    """
    if _ERROR:
        msg="ERROR: in " + getCallerID(1) + ": " + string.join(map(str, args))
        print msg

def DEBUG(*args):
    """Prints messages adding the caller class name and the line of where the call was invoked.
    Behaviour can be changed (on/off) by modifying the _DEBUG global var.
    """
    if _DEBUG:
        msg="DEBUG: in " + getCallerID(1) + ": " + string.join(map(str, args))
        print msg


if __name__=='__main__':
    SAY("This is a NORMAL message")
    ERROR("This is an ERROR message")
    DEBUG("This is a DEBUG message")
