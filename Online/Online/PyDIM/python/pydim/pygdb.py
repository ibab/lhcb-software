import os, sys, threading, traceback
import pydim

global _debug, _base_debug
_debug = 0
_base_debug = 0

MODE_LOCAL = 1
MODE_COMMON = 2

COMMAND_UNHANDLED = 0
COMMAND_HANDLED = 1
COMMAND_TERMINATE = 2
GDB_SHELL_COMMANDS = '/group/online/dataflow/cmtuser/utilities/gdb'

def sigint_handler(sig_num, frame):
    print 'Handling CTRL-C....'

def _host():
  import socket
  return socket.gethostname().upper()

def output(msg):
  import time
  #os.write(sys.stdout.fileno(),'%s> ++ %s'%(time.strftime('%H:%M:%S'),msg,))
  os.write(sys.stdout.fileno(),'%s> %s'%(time.strftime('%H:%M:%S'),str(msg),))

def makePrompt():
  output('(local) ')

def dbgCmd(status, msg=''):
  if _base_debug: print 'Command:', msg
  if msg.find('\r\n')<0:
    return (status, msg+'\r\n',)
  return (status, msg,)


def disable_buffering():
  # Appending to gc.garbage is a way to stop an object from being
  # destroyed.  If the old sys.stdout is ever collected, it will
  # close() stdout, which is not good.
  """
  import gc
  fd = sys.stdout
  gc.garbage.append(fd)
  sys.stdout = os.fdopen(fd.fileno(), 'w', 0)
  fd = sys.stderr
  gc.garbage.append(fd)
  sys.stderr = os.fdopen(fd.fileno(), 'w', 0)
  """
  import fcntl
  fd = sys.stderr
  fl = fcntl.fcntl(fd.fileno(), fcntl.F_GETFL)
  fcntl.fcntl(fd.fileno(), fcntl.F_SETFL, fl|os.O_SYNC)
  fd = sys.stdout
  fl = fcntl.fcntl(fd.fileno(), fcntl.F_GETFL)
  fcntl.fcntl(fd.fileno(), fcntl.F_SETFL, fl|os.O_SYNC)

#-------------------------------------------------------------------
class IOWorker(threading.Thread):
  """
  \version 1.0
  \author  M.Frank
  """
  #-----------------------------------------------------------------
  def __init__(self, inputs=None, output=None):
    threading.Thread.__init__(self)
    self.daemon  = True
    self.enabled = True
    self.output  = output
    self.epoll   = None
    self.setInputs(inputs)

  #-----------------------------------------------------------------
  def setInputs(self, inputs):
    import fcntl, select
    self.inputs  = {}
    if inputs:
      self.epoll = select.epoll(len(inputs))
      for i in inputs:
        fid = i.fileno()
        flags = fcntl.fcntl(fid, fcntl.F_GETFL)
        fcntl.fcntl(fid, fcntl.F_SETFL, flags | os.O_NONBLOCK )
        self.inputs[fid] = i
        self.epoll.register(i,select.EPOLLIN|select.EPOLLERR|select.EPOLLHUP)

  #-----------------------------------------------------------------
  def handleData(self, item):
    self.writeDevice( item )
    return self

  #-----------------------------------------------------------------
  def writeDevice(self, res ):
    # Simple file device
    if self.output:
      os.write(self.output.fileno(),res)
    return self

  #-----------------------------------------------------------------
  def readData(self, timeout):
    import time, select
    value = ''
    if self.epoll:
      evts = self.epoll.poll(timeout=timeout,maxevents=1)
      for fileno,event in evts:
        if event&select.EPOLLIN:
          value = value + os.read(fileno,4096)
        elif event&select.EPOLLHUP:
          pass
    else:
      time.sleep(timeout)
    return value

  #-----------------------------------------------------------------
  def run(self):
    ''' needs to be in a thread so we can read the stdout w/o blocking '''
    while self.enabled:
      value = ''
      try:
        value = self.readData(timeout=0.1)
      except Exception,X:
        value = value + '\n\nException:'+str(X)
        output('++ %s\n'%(str(value),))
        traceback.print_exc()
      if len(value):
        self.handleData( value )
    output('++ Thread exiting....\n')
    return self

#-------------------------------------------------------------------
WorkerOutput = IOWorker
WorkerInput = IOWorker
#-------------------------------------------------------------------
class DimService(WorkerOutput):
  """
  \version 1.0
  \author  M.Frank
  """
  #-----------------------------------------------------------------
  def __init__(self, service, inputs, format='C'):
    WorkerOutput.__init__(self, inputs=inputs)
    dns, svc = service
    pydim.dis_set_dns_node(dns)
    self.enable_stdout = False
    self.dim_output_buffer = ''
    self.service = svc
    self.serviceID = pydim.dis_add_service(svc, format, self.dim_output_handler, 1)

  #-----------------------------------------------------------------
  def __del__(self):
    self.release()

  #-----------------------------------------------------------------
  def release(self):
    if self.serviceID:
      output('++ Destructing DIM service[%d]: %s\n'%(self.serviceID,self.service,))
      pydim.dis_remove_service(self.serviceID)
    self.serviceID = None

  #-----------------------------------------------------------------
  def dim_output_handler(self, tag):
    return self.dim_output_buffer

  #-----------------------------------------------------------------
  def writeDevice(self, res ):
    self.dim_output_buffer = res
    if _debug: output('D: %s'%(res,))
    if self.enable_stdout: output(res)
    pydim.dis_update_service(self.serviceID,(self.dim_output_buffer,))

#-------------------------------------------------------------------
class DimCommand:
  #-----------------------------------------------------------------
  def __init__(self, service, output, handler=None, format='C'):
    self.output = output
    self.handler = handler
    dns,svc = service
    pydim.dis_set_dns_node(dns)
    self.service = svc
    self.commandID = pydim.dis_add_cmnd(svc, format, self.dim_input_handler, 1)

  #-----------------------------------------------------------------
  def __del__(self):
    self.release()

  #-----------------------------------------------------------------
  def release(self):
    if self.commandID:
      output('++ Destructing DIM command[%d]: %s\n'%(self.commandID,self.service,))
      pydim.dis_remove_service(self.commandID)
      self.commandID = None

  #-----------------------------------------------------------------
  def dim_input_handler(self, *args):
    try:
      
      msg = args[0][0]
      msg = msg[:msg.find('\0')]
      if _base_debug: output('DimCommand: <<%s>>\n'%(msg,))
      if self.handler:
        result,msg = self.handler(msg)
        if result == COMMAND_TERMINATE:
          self.enabled = False
          return self
        elif result == COMMAND_HANDLED:
          return self
      if self.output:
        nbytes = os.write(self.output.fileno(), msg)
      else:
        nbytes = os.write(sys.stdout.fileno(), 'UNHANDLED - No output device: '+msg)
    except Exception,X:
      output('++ Exception: input callback: %s\n'%(str(X),))
      traceback.print_exc()

#-------------------------------------------------------------------
class DimServiceClient:
  #-----------------------------------------------------------------
  def __init__(self, service, output, format='C', handler=None, first=True, raw_handler=None):
    dns,svc = service
    self.first = first
    self.service = svc
    self.output = output
    self.handler = handler
    pydim.dic_set_dns_node(dns)
    if not raw_handler: raw_handler = self.dim_input_handler
    self.svcID = pydim.dic_info_service(svc, format, raw_handler)

  #-----------------------------------------------------------------
  def release(self):
    output('++ Destructing DIC info service[%d]: %s\n'%(self.svcID,self.service,))
    pydim.dic_release_service(self.svcID)
    self.svcID = None

  #-----------------------------------------------------------------
  def dim_input_handler(self, *args):
    if _debug: output('Args: %s\n'%(str(args),))
    try:
      if not self.first and len(args)>1:
        r = args[1]
        r = r[:r.find('\0')]
        if self.handler:
          result,msg = self.handler(r)
          if result == COMMAND_TERMINATE:
            return self
          elif result == COMMAND_HANDLED:
            return self
        else:
          msg = r
        if self.output:
          nbytes = os.write(self.output.fileno(), '%s: %s'%(self.service,msg,))
        elif not first:
          nbytes = os.write(sys.stdout.fileno(), 'UNHANDLED - No output device: '+msg)

    except Exception,X:
      output('++ Exception: input callback: %s\n'%(str(X),))
      traceback.print_exc()
    self.first = False

#-------------------------------------------------------------------
class DimCommandClient(WorkerInput):
  """
  \version 1.0
  \author  M.Frank
  """
  #-----------------------------------------------------------------
  def __init__(self, service, input, handler=None, format='C'):
    self.format = format
    self.handler = handler
    self.dns,self.service = service
    WorkerInput.__init__(self, inputs=[input], output=None)

  #-----------------------------------------------------------------
  def release(self):
    pass

  #-----------------------------------------------------------------
  def writeDevice(self, msg ):
    # Simple file device
    if self.handler:
      result,msg = self.handler(msg)
      if result == COMMAND_TERMINATE:
        self.enabled = False
        return self
      elif result == COMMAND_HANDLED:
        return self
    if self.dns:
      pydim.dic_set_dns_node( self.dns )
    if self.service:
      ret = pydim.dic_cmnd_service( self.service, (msg,), self.format )
    return self

#-------------------------------------------------------------------
class Helper:
  """
  \version 1.0
  \author  M.Frank
  """
  #-----------------------------------------------------------------
  def __init__(self, client, node, dns):
    self.decode(client, node, dns)

  #-----------------------------------------------------------------
  def decode(self, client, node, dns):
    import socket
    self.node = node
    self.client = client
    if _debug: output('Helper: client="%s"\n'%(str(client),))
    if self.client:
      idx = self.client.find('@')
      if idx > 0:
        self.dns        = self.client[idx+1:].upper()
        self.process    = node+'_'+self.client[:idx]
      elif dns is not None:
        self.dns        = dns
        self.process    = node+'_'+self.client
      else:
        raise ValueError('Helper arguments are invalid/inconsistent: '+\
                           'client=%s dns=%s'%(str(client),str(dns),))
      self.proc_out   = self.process+'/OUT'
      self.proc_in    = self.process+'/IN'
      self.proc_all   = self.dns+'_'+self.client+'/IN'
    else:
      self.dns = None
      self.process  = None
      self.proc_in  = None
      self.proc_out = None
      self.proc_all = None
      output('++ Configuration without DIM connections....\n')
    return self

#-------------------------------------------------------------------
class Server:
  """
  \version 1.0
  \author  M.Frank
  """
  def __init__(self, application, client, cont, dns):
    global _debug
    #_debug = 1
    output('++ Starting server: %s application: %s\n'%(client,application,))
    self.enabled     = True
    self.dns         = dns
    self.cont        = cont
    self.client      = client
    self.application = application
    self.app         = None
    helper           = Helper(self.client, _host(), self.dns)
    self.process     = helper.process
    self.cmd_common  = None
    self.cmd_in      = DimCommand((helper.dns, helper.proc_in), 
                                  output=None, handler=self.handler)
    self.worker_out  = DimService((helper.dns, helper.proc_out), inputs=None)
    self.output('++ DIM common  service: %s->%s\n'%(helper.dns, helper.proc_all,))
    self.output('++ DIM command service: %s->%s\n'%(helper.dns, helper.proc_in,))
    self.output('++ DIM output  service: %s->%s\n'%(helper.dns, helper.proc_out,))
    self.worker_out.enable_stdout = True
    self.worker_out.start()

  #-----------------------------------------------------------------
  def common_handler(self, *args):
    if _base_debug: print args
    if len(args) > 1:
      result,msg = self.handler(args[1][:-1].strip())
      if result == COMMAND_TERMINATE:
        return self
      elif result == COMMAND_HANDLED:
        return self
      if self.app and self.app.stdin:
        nbytes = os.write(self.app.stdin.fileno(),msg)
      elif _debug:
        nbytes = os.write(sys.stdout.fileno(), 'UNHANDLED - No output device: '+msg)

  #-----------------------------------------------------------------
  def connectToUTGID(self,utgid):
    the_utgid = utgid\
        .replace('<node>',_host().lower())\
        .replace('<NODE>',_host().upper())
    
    cmd = '/opt/FMC/bin/tmLl -m '+_host()+' | grep '+the_utgid
    lines = os.popen(cmd).readlines()
    self.output('\n++Processes matching the utgid pattern: %s\n'%(the_utgid,))
    for l in lines:
      self.output('%s\n'%(l,))
    for l in lines:
      items = l.split()
      if len(items)>4:
        pid = items[1]
        return 'attach '+pid
    return 'print "++ No process with a UTGID matching the '+\
        'pattern %s found on host:%s"\n'%(the_utgid,_host(),)

  #-----------------------------------------------------------------
  def sendInterrupt(self):
    import signal
    self.app.send_signal(signal.SIGINT)

  #-----------------------------------------------------------------
  def handler(self, msg):
    if _base_debug: output('MSG: "%s" [len:%s]\n'%(msg[:-1],len(msg),))
    cmd = msg.strip()
    m = cmd
    idx = cmd.find(' ')
    if idx >= 0: idx = idx + 1
    if idx > 0: m = cmd[idx:].strip()
    else:       m = ''

    if cmd[:1] == 'q':
      return dbgCmd(COMMAND_UNHANDLED,'q')
    elif cmd[:2] == '.q':
      self.enabled = False
      self.worker_out.enabled = False
      return dbgCmd(COMMAND_UNHANDLED,'q')
    elif cmd[:4] == '.app':
      if len(m):
        self.application = m
        output('++ Use new application: "%s"'%(m,))
      return dbgCmd(COMMAND_UNHANDLED,'q')
    elif cmd[:3] == '.ps':
      return dbgCmd(COMMAND_UNHANDLED,'shell ps '+cmd[3:])
    elif cmd == '.utgid':
      return dbgCmd(COMMAND_UNHANDLED,
                    'shell /opt/FMC/bin/tmLl -m '+_host()+' | grep -v term')
    elif cmd[:6] == '.utgid':
      return dbgCmd(COMMAND_UNHANDLED,self.connectToUTGID(m))
    elif cmd[:4] == '.she':
      return dbgCmd(COMMAND_UNHANDLED,cmd[1:])
    elif cmd[:4] == '.pid':
      return dbgCmd(COMMAND_UNHANDLED,'attach '+m)
    elif cmd[:4] == '.cmd':
      cmd = 'shell source '+GDB_SHELL_COMMANDS+os.sep+'macros.sh; '+m
    elif cmd[:4] == '.inv': # .invoke ....
      return dbgCmd(COMMAND_HANDLED,m)
    elif cmd[:4] == '.sou': # .source ....
      return dbgCmd(COMMAND_UNHANDLED,
                    'source '+GDB_SHELL_COMMANDS+os.sep+m+'.gdb')
    elif cmd[:5] == '.stop':
      self.sendInterrupt()
      return dbgCmd(COMMAND_HANDLED)
    elif cmd[:5] == '.star':   # .start <application> --> Not implemented
      return dbgCmd(COMMAND_HANDLED)
    return dbgCmd(COMMAND_UNHANDLED,cmd)

  def output(self, msg):
    if self.worker_out:
      self.worker_out.writeDevice(msg)
    output(msg)

  def startMe(self):
    import subprocess
    #_debug = 1
    pipe             = subprocess.PIPE
    self.app         = subprocess.Popen([self.application],stdin=pipe,stdout=pipe,stderr=pipe)
    self.cmd_in.output = self.app.stdin
    self.cmd_common.output = self.app.stdin
    self.worker_out.setInputs([self.app.stdout,self.app.stderr])
    self.output('++ Remote debugger [%s] started: PID:%d\n'%(self.application,self.app.pid,))

  #-----------------------------------------------------------------
  def run(self):
    helper       = Helper(self.client, _host(), self.dns)
    self.process = helper.process
    self.output('++ Start serving: %s->%s\n'%(helper.dns,helper.process,))
    pydim.dis_set_dns_node(helper.dns)
    pydim.dic_set_dns_node(helper.dns)
    pydim.dis_start_serving(self.process)
    self.cmd_common = DimServiceClient((helper.dns, helper.proc_all), 
                                       output=sys.stdout, 
                                       raw_handler=self.common_handler)
    while self.enabled:
      try:
        self.startMe()
        self.app.wait()
      except Exception,X:
        self.output('Server Exception: %s'%(str(X),))
        self.enabled = False

    self.output('++ Wait for output thread being stopped.....\n')
    self.worker_out.enabled = False
    self.worker_out.join(timeout=1)
    self.cmd_common.release()
    self.cmd_in.release()
    self.worker_out.release()
      
def clientHelp():
  print """

  Cluster debugger local help:
  ============================
  Local commands:
  .q                    Quit cluster debugger
  .dis(connect)         Disconnect from current client
  .client               Disconnect from current client
  .client <utgid>@<dns> Connect to remote debugger client identified by <utgid> 
                        in namespace <dns>

  ** Local commands acting on a connected remote client:
  q                     Quit remote debugger instance and recreate a new one
                        (Standard gdb command)
  .qq                   Quit remote connected instance
  .ps <args>            Issue remote ps command if connected
  .stop                 Send SIGINT to remote
  .start                Start remote debugger
  .shell <file>         Source remote shell script
  .cmd <command>        Invoke remote command from file %s/macros.sh
  .source <file-name>   Source remote %s/<file-name>.gdb file

  .utgid                Show all remote processes with a utgid known to the task manager.
  .utgid <utgid>        Connect remote debugger to process identified by it's UTGID.
  .pid                  Connect remote debugger to process identified by it's process id.

  ** Global commands acting on ALL connected instances:
  .common <command>
    Command: - All standard gdb commands are valid
             - All commands, which can be sent to a connected remote client.
    Substitutions: 
    The node name in UTGID string of the form  aaaa_<node>_bbbb and aaa_<NODE>_bbb
    are case dependent substituted.

                                                        M.Frank CERN/LHCb

  """%(GDB_SHELL_COMMANDS,GDB_SHELL_COMMANDS,)

#-------------------------------------------------------------------
class Client:
  """
  \version 1.0
  \author  M.Frank
  """
  #-----------------------------------------------------------------
  def __init__(self, client, node, dns):
    self.dns = dns
    self.node = node
    self.mode = MODE_LOCAL
    self.client = client
    self.terminate   = False
    self.worker_log = None
    self.worker_cmd = None
    self.dim_output_buffer = ''
    helper = Helper(self.client, self.node, self.dns)
    pydim.dis_set_dns_node(helper.dns)
    pydim.dic_set_dns_node(helper.dns)
    self.serviceID = pydim.dis_add_service(helper.proc_all,'C', self.dim_output_handler, 1)
    self.publish_common('')
    pydim.dis_start_serving('MASTER/'+helper.proc_all)
    output('++ DIM common  service: %s->%s\n'%(helper.dns, helper.proc_all,))
    
  #-----------------------------------------------------------------
  def dim_output_handler(self, tag):
    return self.dim_output_buffer

  #-----------------------------------------------------------------
  def publish_common(self, cmd):
    self.dim_output_buffer = cmd
    if _debug: output('Publish-ALL: %s\n'%(cmd,))
    pydim.dis_update_service(self.serviceID,(self.dim_output_buffer,))

  #-----------------------------------------------------------------
  def connect(self, new_node, new_client):
    self.node   = new_node.upper()
    self.client = new_client
    self.worker_cmd.enabled = False
    return self

  #-----------------------------------------------------------------
  def startDebugger(self):
    #/usr/bin/ssh hlta0101 echo \$DIM_DNS_NODE
    pass

  #-----------------------------------------------------------------
  def handler(self, msg):
    #print 'Client: MSG: "%s" [len:%s]'%(msg[:-1],len(msg),)
    msg = msg.strip()
    idx = msg.find(' ')
    m = msg
    if idx >=0: m = msg[idx+1:].strip()

    if msg[:1] == "q":
      makePrompt()
      return dbgCmd(COMMAND_UNHANDLED,'q')
    elif msg[:3] == ".qq":
      makePrompt()
      return dbgCmd(COMMAND_UNHANDLED,'.q')
    elif msg[:2] == ".q": 
      self.terminate = True
      return dbgCmd(COMMAND_TERMINATE,m)
    elif msg[:2] == ".h":
      clientHelp()
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,m)
    elif msg[:4] == ".dis":
      self.connect(None)
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,m)
    elif msg[:6] == ".start":
      self.startDebugger()
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,m)
    elif msg[:4] == ".loc":
      output('++ Setting cluster debugger client to LOCAL mode.\n')
      self.mode = MODE_LOCAL
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,m)
    elif msg[:5] == ".glob":
      output('++ Setting cluster debugger client to GLOBAL/COMMON mode.\n')
      self.mode = MODE_COMMON
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,m)
    elif msg[:7] == ".client":
      if len(msg) > 8: self.connect(self.node,m)
      else:            self.connect(None)
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,m)
    elif msg[:5] == ".node":
      self.node = m.upper()
      if len(msg) > 7: self.connect(m,self.client)
      else:            self.connect(None)
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,m)
    elif msg[:7] == ".common":
      if len(m)> 0: self.publish_common(m+'\n\r')
      else:  output('** Unhandled statement: %s'%(msg,))
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,m)
    elif self.worker_log is None:
      output('++ You must be connected to a remote client to '+\
          'issue the command: "%s"\n'%(msg[:-1],))
      output('type .h for local help commands.\n')
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,msg)

    if self.mode == MODE_COMMON and msg.find('.common')<0:
      self.publish_common(msg)
      makePrompt()
      return dbgCmd(COMMAND_HANDLED,m)

    return dbgCmd(COMMAND_UNHANDLED,msg)

  #-----------------------------------------------------------------
  def startMe(self):
    if self.client:
      helper = Helper(self.client, self.node, self.dns)
      output('++ Starting Client: DNS:%s Target:%s->%s Receiving end:%s->%s\n'%(\
        helper.dns, helper.dns, helper.proc_in, helper.dns, helper.proc_out, ))
      self.process = helper.process
      self.worker_log = DimServiceClient((helper.dns, helper.proc_out), sys.stdout)
      self.worker_cmd = DimCommandClient((helper.dns, helper.proc_in), sys.stdin, self.handler)
    else:
      output('++ Starting Client in disconnected mode: type .client <utgid>@<dns> to connect.\n')
      self.process = None
      self.worker_log = None
      self.worker_cmd = DimCommandClient((None,None,), sys.stdin, self.handler)
      makePrompt()
    self.worker_cmd.start()

  #-----------------------------------------------------------------
  def waitForMe(self):
    try:
      if self.worker_cmd:
        self.worker_cmd.join()
        output('++ Client finihed....\n')
    except Exception,X:
      print str(X)

    if self.worker_log:
      self.worker_log.release()
    if self.worker_cmd: 
      self.worker_cmd.release()
    self.worker_log = None
    self.worker_cmd = None

  #-----------------------------------------------------------------
  def run(self):
    while not self.terminate:
      self.startMe()
      self.waitForMe()

    output('++ Commands disabled. Exiting....\n')
    if self.serviceID:
      output('++ Destructing COMMON DIM service %d\n'%(self.serviceID,))
      pydim.dis_remove_service(self.serviceID)

#-------------------------------------------------------------------
def start_gdb():
  import subprocess
  disable_buffering()
  pipe = subprocess.PIPE
  gdb_process = subprocess.Popen(['/usr/bin/gdb'],stdin=pipe,stdout=pipe,stderr=pipe)
  cmd_in   = WorkerInput([sys.stdin],gdb_process.stdin)
  worker_out  = WorkerOutput ([gdb_process.stdout,gdb_process.stderr],sys.stdout)
  cmd_in.start()
  worker_out.start()
  # Now wait until gdb ended .....
  gdb_process.wait()
  worker_out.enabled = False
  cmd_in.join(timeout=1)
  worker_out.join(timeout=1)


def dispatch():
  import optparse, traceback

  parser = optparse.OptionParser()
  parser.formatter.width=132
  parser.description = "Remote DIM based debugger implementation."

  parser.add_option("-c", "--continue", dest="cont", default=False,
                    help="Continue processing after attaching to target.",
                    action="store_true",
                    metavar="<boolean>")
  parser.add_option("-C", "--client", dest="client", default=False,
                    help="Set client mode to connect to processes.",
                    action="store_true",
                    metavar="<boolean>")
  parser.add_option("-S", "--server", dest="server", default=False,
                    help="Set server mode to connect to processes.",
                    action="store_true",
                    metavar="<string>")
  parser.add_option("-t", "--target", dest="target", default=None,
                    help="Server/Client variable: Set mode to connect to process: "+
                    "<utgid>@node or <pid@node>",
                    metavar="<string>")
  parser.add_option("-a","--application",
                    dest="app", default='/usr/bin/gdb',
                    help="Application name of the server process",
                    metavar="<string>")
  parser.add_option("-n","--node",
                    dest="node", default=_host(),
                    help="Server/Client variable: Set node-name of debuggee",
                    metavar="<string>")
  parser.add_option("-d","--dns",
                    dest="dns", default='HLT01',
                    help="Server/Client variable: Set DIM DNS node-name of debuggee",
                    metavar="<string>")
  parser.add_option("-p","--pid",
                    dest="pid", default=-1,
                    help="Server variable: Set PID selector of process to be debugged",
                    metavar="<integer>")

  if 1:
    (opts, args) = parser.parse_args()

    if opts.node:  opts.node= opts.node.upper()
    if opts.dns:   opts.dns= opts.dns.upper()

    if not opts.server and opts.dns:
      opts.server = opts.dns
    if opts.server and opts.app is None:
      output('Undefined application name! This may not be.\n')
      parser.format_help()
      return

    sensor = None
    if opts.client:
      disable_buffering()
      sensor = Client(opts.target, node=opts.node, dns=opts.dns)
    elif opts.server:
      output('Server: %s [%s] \n'%(opts.target, opts.app, ))
      disable_buffering()
      sensor = Server(opts.app, opts.target, cont=opts.cont, dns=opts.dns)
    else:
      parser.format_help()

    if sensor:
      sensor.run()
    else:
      parser.format_help()

  #except Exception,X:
  #  traceback.print_exc()


def find_server():  
  import pydim.DNS, time
  #pydim.DNS._debug = 1
  #svc = pydim.DNS.ServerSelector(match='LHCb_HLTA010._PassThrough_10[1-9]',dns='HLTA01')
  svc = pydim.DNS.ServerSelector(match='LHCb_MONA080[4-5]_.*',dns='MONA08')
  while svc.enabled:    
    time.sleep(2)
    data = svc.get()
    for k in data.keys():
      v = data[k]
      output('Server: [%6d]   %s     @     %s\n'%(v[1],k,v[1],))

if __name__ == '__main__':
  #start_gdb()
  dispatch()
  #find_server()

