import dim
import string
import time

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
    dns = Dns()
    print dns.get_servers()
        
    
