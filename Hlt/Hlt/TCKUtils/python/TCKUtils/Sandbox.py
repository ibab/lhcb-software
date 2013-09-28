def execInSandbox(fun,*args,**kwargs) :
    from processing import Pipe, Process, Condition
    (r,w) = Pipe()

    # bind worker to the 'write' side of the pipe, and to 'fun'
    def worker( *args, **kwargs) :
            # TODO: catch errors, and signal parent about them...
            try :
                w.send(fun(*args,**kwargs))
            except Exception, e :
                w.send(e)
             
    p = Process(target=worker,args=args,kwargs=kwargs)
    p.start()
    result = r.recv()
    if type(result) is Exception : raise result
    # check whether child exited OK..    .
    return result

