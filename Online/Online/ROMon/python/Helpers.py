import os,sys,time,dimc

S_ERROR   = 0
S_SUCCESS = 1

def error(*args):
    """
        The error function labels the information as
        [ERROR] to be properly identified when showed
    """
    s = ''
    for i in args:
        s = s + str(i)
    print '%s %-8s %s'%(time.ctime(),'[ERROR]',s)
    return S_ERROR

def info(*args):
    """
        The info function labels the information as
        [INFO] to be properly identified when showed
    """
    s = ''
    for i in args:
        s = s + str(i)
    print '%s %-8s %s'%(time.ctime(),'[INFO]',s)

def debug(*args):
    """
        The debug function labels the information as an
        [DEBUG] to be properly identified when showed
    """
    s = ''
    for i in args:
        s = s + str(i)
    print '%s %-8s %s'%(time.ctime(),'[DEBUG]',s)

def clearscreen(numlines=100):
    """
        The clearscreen function is invoked each second
        to sweep the old information from the screen and
        permits the updates to be posted without displacing
        the info from its regular place, it also takes into
        account the type of operating system to do it.
    """
    if os.name == "posix":
        # Unix/Linux/MacOS/BSD/etc
        os.system('clear')
    elif os.name in ("nt", "dos", "ce"):
        # DOS/Windows
        os.system('CLS')
    else:
        # Fallback for other operating systems.
        print '\n' * numlines

class DimClient:
    """
        The DimClient class sets the constructor and the
        destructor for a future DIM Service as well as
        the cleanup after stopping a service. Also, the
        base parameters for starting the service are
        declared here.
    
        TaskSupervisor and ClusterSupervisor inherit from
        this class
    """
    def __init__(self,name):
        "Standard constructor"
        self.name = name
        self.id = None
    def __del__(self):
        if self.id is not None:
            print 'Release service:',self.id
            dimc.dic_release_service(self.id)
    def stop(self):
        if self.id is not None:
            dimc.dic_release_service(self.id)
        self.id = None
        return self

    def startClient(self,datapoint,format,callback):
        if self.id is not None:
            dimc.dic_release_service(self.id)
        self.id = dimc.dic_info_service(datapoint,format,callback)
        return S_SUCCESS

