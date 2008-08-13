import sys, os, pickle, traceback


def _exceptHook(type, value, tb):
   global _tb
   sys.__excepthook__(type, value, tb)
   _tb = ''.join(traceback.format_exception(type, value, tb))

# we don't use try except, but instead set the exceptHook so
# we can catch the traceback and route it to the parent...
def _child(p,fun,*args,**keyw) :
    sys.stdin.close() 
    sys.excepthook = _exceptHook
    global _tb
    _tb = ''
    os.write(p,pickle.dumps(fun(*args,**keyw),-1))
    if (len(_tb)==0) :
        os.close(p)
        os._exit(0)
    print 'Unexpected error:', sys.exc_info()[0]
    os.write(p,_tb)
    os.close(p)
    os._exit(1)

def _readpipe(p,blocksize=1024) :
    s = ''
    while True:
        r = os.read(p,blocksize)
        s+=r
        if len(r) < blocksize : break
    os.close(p)
    return s

def _parent(pid,p):
    s = _readpipe(p)
    (rpid,stat) = os.waitpid(pid,0)
    if stat != 0 : 
        print 'child exit status: ' + str(stat>>8)
        print s
        return
    return pickle.loads(s)

def execInSandbox(fun,*args,**keyw) :
    (r,w) = os.pipe()
    pid = os.fork()
    if (pid==0) : _child(w,fun,*args,**keyw)
    else :        return _parent(pid,r)
