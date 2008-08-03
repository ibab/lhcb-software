import sys, os, pickle

def _child(p,fun,*args,**keyw) :
    sys.stdin.close() 
    os.write(p,pickle.dumps(fun(*args,**keyw),-1))
    os.close(p)
    os._exit(0)

def _parent(pid,p):
    s = ''
    while True:
        r = os.read(p,1024)
        s+=r
        if len(r) < 1024 : break
    os.close(p)
    x = os.waitpid(pid,0)
    return pickle.loads(s)

def execInSandbox(fun,*args,**keyw) :
    (r,w) = os.pipe()
    pid = os.fork()
    if (pid==0) : _child(w,fun,*args,**keyw)
    else :        return _parent(pid,r)
