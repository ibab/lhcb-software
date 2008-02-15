from dimc import *
from dimcpp import *
from debug import *
import string, time

# ###########################################################################
# The DIM RPC proxy class.
# ###########################################################################
class PyDimRpcProxy (DimRpc):
    """
Accepts a string in the format:
  'function_name/par1_name=par1_value1,par1_value2/par2_name=par2_value/.../'
or
  'function_name/par1_value,par2_value/.../' for possitional arguments
The appropiate function is called with the appropiate parameters and the 
result is returned in the same format.
The special characters (',', '=' and '/') must be excaped.
Identifiers must contain alfanumeric characters plus the '_' and '-' chars.
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
        ret += '\0' 
        # setting result
        DEBUG('Sending result: %s' %ret)
        self.setData(ret)
        


# ###########################################################################
# The DIM RPC proxy class.
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

if __name__ == "__main__":
    #dns = Dns()
    #print dns.get_servers()
    pass    
    
