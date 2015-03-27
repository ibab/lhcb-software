#! /usr/bin/env python

"""
PyDIM is a Python interface to DIM.

PyDIM could be used to create DIM clients and servers, using an API very similar
to the one that is used for C.
"""

import random
import string
import time
import types
import threading
import logging

from dimc import *
from dimcpp import *
from debug import *

_version = '1.3.4'

# ###########################################################################
# The DIM RPC proxy class.
# ###########################################################################
class PyDimRpcProxy (DimRpc):
    """
Class that facilitates publishing of random python functions using a single 
DIM RPC. At creation it receives a lost of callable objects wo which the calls 
will be passed. It creates a single string DIM RPC and accepts a string with 
the format:
  'function_name/par1_name=par1_value1,par1_value2/par2_name=par2_value/.../'
or
  'function_name/par1_value,par2_value/.../' for possitional arguments
The appropiate function is called with the appropiate parameters and the 
result is returned in the same format.
The special characters (',', '=' and '/') must be excaped.
Identifiers must contain alfanumeric characters plus the '_' and '-' chars.
It makes the assumption that the python functions will return always a tuple
in the format (STATUSCODE, RESULTS). All the return parameters are converted 
to a string and are returned to the client. 
    """
    def __init__(self, funcs, rpcName='testRPC'):
        DimRpc.__init__(self, rpcName, 'C', 'C')
        self.lastValue = None
        self.funcs = {}
        self.name = rpcName
        if not hasattr(funcs, '__iter__'):
            funcs = [funcs]
        for f in funcs:
            # creating a dictionary of function names and callable objects
            if hasattr(f, '__call__'):
                self.funcs[f.func_name] = f
            else:
                ERROR ('Object %s is not callable' %f)
                DEBUG(dir(f))

    def convert(self, args):
        ret = ""
        if not hasattr(args, '__iter__'):
            args = (args,)
        for x in args:
            add = str(x)
            add = add.replace('/', '\/')
            add = add.replace('=', '\=')
            add = add.replace(',', '\,')
            ret += add + ','
        return ret[:-1]

    def split(self, s, sep, exc='\\'):
        args = []
        poz = 0
        while s:
            poz = s.find(sep, poz)
            if poz == 0:
                s = s[1:]
            elif poz == -1:
                args.append(s)
                s = ""
            elif s[poz-1] != exc:
                args.append(s[:poz])
                s = s[poz+1:]
                poz = 0
            else:
                poz += 1
        if len(args) > 1 or len(args) == 0:
            return args
        else:
            return args[0]
 
    def parse(self, s):
        pozArgs = []
        keyArgs = {}
        args = self.split(s, '/')
        if type(args) is str: args = (args,)
        DEBUG(args)
        for arg in args:
            #DEBUG(arg)
            poz = arg.find('=')
            if poz > 0 and arg[poz-1] != '\\':
                # we have a named argument
                keyArgs[arg[:poz]] = self.split(arg[poz+1:], ',')
            else:
                pozArgs.append(self.split(arg, ','))
        return pozArgs, keyArgs

    def rpcHandler(self):
        s = self.getString()
        DEBUG("Received: ", s)
        # figuring out which method to call and parsing arguments
        if s.find('/') > -1:
            funcName = s[:s.find('/')]
            s = s[s.find('/'):]
        elif s:
            # we have 0 arguments
            funcName = s
            s = ""
        else:
            self.setData('ERROR: function %s is not registered' %s)
            return
        try:
            funcObj = self.funcs[funcName]
        except KeyError, e:
            ERROR(e)
            self.setData('status=FAIL/error=Could not find function '+str(e))
            return
        #DEBUG(funcName, s)
        pozArgs, keyArgs = self.parse(s)
        #DEBUG(pozArgs, keyArgs)
        ret = funcName + '/'

        try:
           res = funcObj(*pozArgs, **keyArgs)
        except Exception, e:
            # catch all convert it to string and return
            print type(funcObj)
            ret += 'status=FAIL/error='+str(e)
            DEBUG('Returning exception message: %s' %ret)
        else:
            '''
            We have an result. Converting to a string and returning it.
            There can be two types of returned results: iterables and 
            dictionaries. Both results can contain basic types or other 
            iterables.
            Iterables: They will be  converted in the equivalent of positional
            arguments for calling a function.
            Dictionaries: Will be converted in the equivalent of named 
            arguments.
            '''
            if res[0] == 0:
                ret += 'status=ERROR/'
                try:
                    ret = '%serror=%s/' %(ret, res[1])
                except:
                    pass
            else:
                ret += 'status=SUCCESS/'
                named = False
                try:
                    res = res[1]
                except:
                    pass
                else:
                    if isinstance(res, dict):
                        for x in res:
                            ret = "%s%s=%s/" %(ret, 
                                               self.convert(x), 
                                               self.convert(res[x])
                                               )
                    elif hasattr(res, '__iter__'):
                        for x in res:
                            ret = "%s%s/" %(ret, self.convert(x))
                    else:
                        ret = "%s%s/" %(ret, self.convert(res))
	    # this is particularly usefull if on the other side are C/C++ clients
        ret += '\0'
        # setting result
        DEBUG('Sending result: %s' %ret)
        self.setData(ret)
        


# ###########################################################################
# The DIM DNS abstraction class
# ###########################################################################
class Dns:
    def __init__(self):
        self.dns_srv_sub = dim.Subscription(name = "DIS_DNS/SERVER_LIST",
                                            tag = self, schema = "S",
                                            handler = self.servers_update)
        self.servers = None
    def services_update(self, svcstr):
        self.services = []
        for s in string.split(svcstr[0], '\n'):
            if s == '': continue
            i = string.rfind(s, '|')
            j = string.rfind(s, '|', 0, i)
            print s, i, j
            if (i == len(s) - 1):
                type = "SVC"
            else:
                type = s[i + 1:(len(s))]
            self.services.append([ s[0:j], s[j+1:i], type]) 
        self.service_update = True
 
    def servers_update(self, srv):
        self.servers = []; self.server_host = []
        for s in string.split(srv[0], '|'):
            [server, host ] = string.split(s, '@')
            self.servers.append(server)
            self.server_host.append([server, host])

    def get_services(self, server):
        if (not server in self.servers): return None;
        self.service_update = False
        srv = dim.Subscription(name=server+"/SERVICE_LIST", schema = "S",
                               handler = self.services_update)
        i = 0
        while (self.service_update == False and i < 50):
            i += 1
            time.sleep(0.1)
        return self.services
    def get_servers(self):
        while (self.servers == None):
            time.sleep(0.1)
        return self.servers

def dim_service(fn):
    """
    A decorator function that makes easier to use normal functions as callbacks
    for DIM services.
    
    'fn' is a function (in general, a callable object) that doesn't 
    receive arguments, and returns a function that is suited for DIM callbacks.
    """
    def svc(tag):
        # Tag argument is declared but not used
        rtn = fn()
        # The returned value must be tuple, so we try to convert simple values
        if not rtn in (types.ListType, types.TupleType):
            rtn = (rtn,)
        return rtn

    return svc

def dim_service_tag(fn):
    """
    A decorator function that makes easier to use function with an argument as
    callbacks for DIM services.
    
    'fn' is a function (in general, a callable object) that receives one 
    argument, and returns a function that is suited for DIM callbacks. The 
    argument used is the tag of the service tag registerd with DIM.
    """
    def svc(tag):
        rtn = fn(tag)
        if not rtn in (types.ListType, types.TupleType):
            rtn = (rtn,)
        return rtn
    
    return svc


def dic_sync_info_service(name, description, timeout=None, default_value=None):
    """
    A synchronous call for getting the value of a service.
    
    This function is equivalent to `dic_info_service`, but it waits until the
    value is retrieved and returns the value of the service. For this reason a
    callback function is not needed.
    
    Arguments are:

       *name* Service name. same name used by server when declaring the service.
    
       *description* The description string of the service.
    
       *timeout* The number of seconds after which the service is considered to
        have failed. Optional, by default there is no timeout.  
    
       *default_value* The value that will be returned in case the service 
        doesn't succeed. Optional, default is `None`.
    """
    executed = threading.Event()
    state = dict(value=None)

    def create_callback(st):
        def _callback(*args):
            st['value'] = args
            executed.set()
    
        return _callback
    
    callback = create_callback(state)
    dim_timeout = 0 
    tag = random.randint(0,100000)
    sid = dic_info_service(name, description, callback, ONCE_ONLY, dim_timeout, tag, default_value)
    
    executed.wait(timeout)

    dic_release_service(sid)

    return state['value']

def dic_sync_cmnd_service(name, arguments, description, timeout=None):
    """
    A synchronous call for executing a command.
    
    This function works in the same way as `dic_cmnd_service`, but it waits
    until the command is executed.
    
    Arguments are:
        
       *name* Command name, same name used by server when declaring the command
       service. 
    
       *arguments* A tuple with the values that are sent to the command as 
       arguments.
    
       *description* The description string of the command.
       
       *timeout* The number of seconds after which the command is considered 
       to have failed. Optional, by default there is no timeout.  

    It returns an integer which indicates if the command was executed 
    successfully; 1 if it was correctly sent to the server, 0 otherwise.   

    """
    executed = threading.Event()
    state = dict(retcode=None)

    def create_callback(st):
        def _callback(tag, retcode):
            state['retcode'] = retcode
            executed.set()
    
        return _callback
    
    callback = create_callback(state)
    tag = 0
    dic_cmnd_callback(name, arguments, description, callback, tag)
    
    executed.wait(timeout)

    return state['retcode']


if __name__ == "__main__":
    # in case the file is executed directly
    dns = Dns()
    print dns.get_servers()
    pass    
    
