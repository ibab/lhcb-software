
def execInSandbox(fun,*args,**kwargs) :
    from processing import Pipe, Process
    (r,w) = Pipe()

    # bind worker to the 'write' side of the pipe, and to 'fun'
    def worker( *args, **kwargs) :
        w.send(fun(*args,**kwargs))

    p = Process(target=worker,args=args,kwargs=kwargs)
    p.start()
    result = r.recv()
    p.join()
    return result
    


