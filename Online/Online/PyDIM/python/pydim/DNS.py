"""

 PyDIM interface to basic DIS_DNS functionality

  \version 1.0
  \author  M.Frank


"""
import time

global _debug
_debug = 0

#-------------------------------------------------------------------
class DNSInterface:
  """
  \version 1.0
  \author  M.Frank
  """
  #-----------------------------------------------------------------
  def __init__(self, service, dns=None, match=None):
    import threading
    self.dns = dns
    self.svcID = None
    self.match = None
    self.enabled = True
    self.service = service
    self.entries = {}
    self.handler = None
    self.lock = threading.RLock()
    self.connect = self.reconnect
    if match: self.match = match.upper()

  #-----------------------------------------------------------------
  def __del__(self):
    import pydim
    if self.svcID: pydim.dic_release_service( self.svcID )
    del self.lock

  #-----------------------------------------------------------------
  def default_handler(self, *args):
    import traceback
    if _debug: print 'Args:',args
    try:
      self.lock.acquire()
      if len(args)>1:
        result = args[1]
        if self.handler:
          self.handler(result)
    except Exception,X:
      print '++ Exception: ServerList callback: %s'%(str(X),)
      traceback.print_exc()
    self.lock.release()

  #-----------------------------------------------------------------
  def reconnect(self, handler=None, dns=None, sleep=2):
    import pydim
    if handler is not None:
      self.handler = handler
    if dns:        pydim.dic_set_dns_node( dns )
    elif self.dns: pydim.dic_set_dns_node( self.dns )
    if self.svcID:
      pydim.dic_release_service( self.svcID )
      time.sleep(sleep)
    self.svcID = pydim.dic_info_service(self.service, 'C', self.default_handler)

  #-----------------------------------------------------------------
  def get(self):
    import copy
    self.lock.acquire()
    entry = copy.deepcopy(self.entries)
    self.lock.release()
    return entry

  #-----------------------------------------------------------------
  def getServerEntries(self, dim_msg):
    v = dim_msg
    it = v.split('\0')
    servers = {}
    if len(it)>1:
      srvs = it[0].split('|')
      pids = it[1].split('|')
      for i in xrange(len(srvs)):
        s,n = srvs[i].split('@')
        p = int(pids[i])
        servers[s] = (n,p,)
    return servers

  #-----------------------------------------------------------------
  def getServiceEntries(self, dim_msg):
    v = dim_msg
    it = v.split('\0')
    services = {}
    if len(it)>1:
      srvs = it[0].split('\n')
      for i in xrange(len(srvs)):
        svc,fmt,typ = srvs[i].split('|')
        services[svc] = (typ,fmt,)
    return services

  #-----------------------------------------------------------------
  def remove(self, vals):
    for key in vals.keys():
      if self.entries.has_key(key):
        if _debug: print 'DIS_DNS: Removing entry:',key, self.entries[key]
        del self.entries[key]

  #-----------------------------------------------------------------
  def add(self, vals):
    import re
    for key in vals.keys():
      use_entry = True
      if self.match: 
        use_entry = re.match(self.match, key.upper(), flags=re.IGNORECASE)
        if not use_entry: 
          if _debug: print 'DIS_DNS: Reject entry: ',key,vals[key],self.match
      if use_entry: 
        if _debug: print 'DIS_DNS: Accept entry:',key, vals[key]
        self.entries[key] = vals[key]

  #-----------------------------------------------------------------
  def run(self):
    try:
      while self.enabled:
        time.sleep(1)
    except Exception,X:
      print str(X)

#-------------------------------------------------------------------
class ServerSelector(DNSInterface):
  """
  \version 1.0
  \author  M.Frank
  """
  #-----------------------------------------------------------------
  def __init__(self, match=None, dns=None):
    DNSInterface.__init__(self, service='DIS_DNS/SERVER_LIST', dns=dns, match=match)
    self.connect(handler=self.server_list_handler)

  #-----------------------------------------------------------------
  def server_list_handler(self, result):
    action = result[0]
    if action == '-':
      vals = self.getServerEntries(result[1:-1])
      self.remove(vals)
      return
    elif action == '+':
      vals = self.getServerEntries(result[1:-1])
    else:
      vals = self.getServerEntries(result[:-1])
    ##print vals
    self.add(vals)

#-------------------------------------------------------------------
class ServiceSelector(DNSInterface):
  """
  \version 1.0
  \author  M.Frank
  """
  #-----------------------------------------------------------------
  def __init__(self, match=None, dns=None):
    DNSInterface.__init__(self, service='DIS_DNS/SERVICE_LIST', dns=dns, match=match)
    self.connect(handler=self.service_list_handler)

  #-----------------------------------------------------------------
  def service_list_handler(self, result):
    action = result[0]
    if action == '-':
      vals = self.getServiceEntries(result[1:-1])
      self.remove(vals)
      return
    elif action == '+':
      vals = self.getServiceEntries(result[1:-1])
    else:
      vals = self.getServiceEntries(result[:-1])
    print vals
    self.add(vals)
