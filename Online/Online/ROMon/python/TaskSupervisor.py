"""
    Task Supervisor
    by: Raul Duran Bravo

    It gathers the errors of each Node in a Cluster,
    then the summary is published by a service named
    /XXXXXX/TaskSupervisor,
    where XXXXXX is the name of the Cluster.

    The status for each Node can be DEAD and ALIVE.

    A Node is declared DEAD if there is no response from it.

    A Node is declared ALIVE if it responds, anyway if one
    or more Tasks are missing, their names will be specified
    and tagged one by one.


    Functions that are similar to those in the ClusterMonitor
    script are:
      - error
      - info
      - debug
      - clearscreen

    Classes that are similar to those in the ClusterMonitor
    script are:
      - DimClient

    Classes that are exclusive of TaskSupervisor are:
      - TaskList
      - TmTask
      - NodeMonitor
      - TaskSupervisor

    Note: It uses an external script named XMLTaskList to
    decode the XML information

"""

import os, sys, time, copy, XMLTaskList
try:
  import dimc
except Exception,X:
  print X
  print sys.path
  print os.environ['PATH']
  print os.environ['LD_LIBRARY_PATH']
  raise
  
from Helpers import *

s_fullInfo = True
PUBLISH_TIME = 15
PUBLISH_TIME = 15
NUM_TASK_ITEMS = 34

class TaskList(list):
    """
        An instance of the TaskList class represents a collection of
        task object as it results from reading the XML database.
    """
    def __init__(self):
        "Standard constructor"
        list.__init__(self)

class TmTask:               # initialization of the structure with NUM_TASK_ITEMS attributes per task
    """

        The TmTask class  initializes the structure of the 'Task'
        object with the NUM_TASK_ITEMS attributes of each task and when its
        fill function is invoked it assigns a number to each one of the
        attributes to be filled.

        HEADER and SHOW are samples of the information that can
        be displayed.
        
    """
    def __init__(self):
        self.utgid = None
        self.tid = 0
        self.tgid = 0
        self.ppid = 0
        self.pgid = 0
        self.nlwp = None
        self.user = None
        self.group = None
        self.tty = None
        self.cmd = None
        self.sch = None
        self.prio = None
        self.rtprio = None
        self.nice = None
        self.psr = None
        self.stat = None
        self.perc_cpu = 0
        self.perc_mem = 0
        self.vsize = 0
        self.lock = None
        self.rss = 0
        self.data = None
        self.stack = None
        self.exe = None
        self.lib = None
        self.share = None
        self.ignored = None
        self.pending = None
        self.catched = None
        self.blocked = None
        self.started = None
        self.elapsed = None
        self.cputime = None
        self.cmdline = None

    def header(self):  # header for printing some of the values (use with show()) currently unused
      print '%-32s %6s %6s %6s'%('UTGID','TID','TGID','VSIZE')

    def show(self):    # gathering some values to print (currently unused)
      print '%-32s %6d %6d %6d'%(self.utgid,self.tid,self.tgid,self.vsize)

    def fill(self,items):    # filling the attribute structure
      #print items
      try:
        self.utgid = items[0]
        self.tid = int(items[1])
        self.tgid = int(items[2])
        self.ppid = int(items[3])
        self.pgid = items[4]
        self.nlwp = items[5]
        self.user = items[6]
        self.group = items[7]
        self.tty = items[8]
        self.cmd = items[9]
        self.sch = items[10]
        self.prio = items[11]
        self.rtprio = items[12]
        self.nice = items[13]
        self.psr = items[14]
        self.stat = items[15]
        self.perc_cpu = float(items[16])
        self.perc_mem = float(items[17])
        self.vsize = int(items[18])
        self.lock = items[19]
        self.rss = int(items[20])
        self.data = int(items[21])
        self.stack = int(items[22])
        self.exe = items[23]
        self.lib = items[24]
        self.share = items[25]
        self.ignored = items[26]
        self.pending = items[27]
        self.catched = items[28]
        self.blocked = items[29]
        self.started = items[30]
        self.elapsed = items[31]
        self.cputime = items[32]
        self.cmdline = items[33]
      except Exception,X:
        #print items
        raise X


class NodeMonitor(DimClient):
    """
        An instance of the NodeMonitor class polls the /ps/data
        service in the current Node to see what tasks are being
        executed in that moment.

        The callback function defined in the class splits the
        arguments read from /ps/data into NUM_TASK_ITEMS pieces which is
        the number of attributes each task has. A TmTask
        structure is then filled with all the NUM_TASK_ITEMS attributes for
        each existing task.

        The utgid names are then put together in a list that is
        compared against the Master Tasklist. The Master
        Tasklist is  provided with anticipation, right now they
        are static XML files.

        If at the end of the comparison, there is a difference
        between the MasterList and the created utgid list, the
        tasks from the Masterlist that were not found in the
        utgid list will be formatted as XML information for
        later publishing

        The NodeMonitor class inherits from the DimClient. 
    """
    def __init__(self,node,node_info):
        "Standard constructor"
        DimClient.__init__(self,node)
        self.tasks = {}
        for l in node_info.taskLists.keys():
          for t in node_info.taskLists[l].tasks.keys():
            self.tasks[t] = node_info.taskLists[l].tasks[t]
        self.connections = {}
        for l in node_info.connectionLists.keys():
          for t in node_info.connectionLists[l].connections.keys():
            self.connections[t] = node_info.connectionLists[l].connections[t]
        self.errors = ''
        self.connerrors = ''
        self.time = 0
        self.connTime = 0
        print 'Created node monitor:',self.name

    def testConnection(self,name,size,count):
      import xmlrpclib
      try:
        s = xmlrpclib.Server('http://'+self.name+':8088')
        #print self.name,name,count,size
        lines = s.ping(name,count,size)
        tag = '%d packets transmitted, %d received'%(count,count,)
        for l in lines:
          if l.find(tag)==0:
            #print self.name,name,l[:-1]
            return 1
      except Exception,X:
        print 'Failed to ping ',name,X
      return None
        
    def checkConnections(self):
        good = []
        bad = []
        self.connTime = time.time()
        for t in self.connections.keys(): 
          target = t.replace('<DIM_DNS_NODE>',os.environ['DIM_DNS_NODE'])
          siz = 0
          if target[:-3] == '-d1': siz=9000
          if self.testConnection(target,siz,1) is None:
            bad.append(target)
          else:
            good.append(target)
        errs = '\t\t<Connections count="%d" ok="%d" missing="%d">\n'%(len(good)+len(bad),len(good),len(bad))
        if s_fullInfo:
          for t in good: errs = errs + '\t\t\t<Connection name="%s" status="OK"/>\n'%(t,)
        for t in bad: errs = errs + '\t\t\t<Connection name="%s" status="Not OK"/>\n'%(t,)
        errs = errs + '\t\t</Connections>\n'
        self.connerrors = errs
        return self.connerrors
        
    def callback(self,*args): # separates each item from the chain
        try:
            if len(args)==0:
                return
            self.time = time.time()
            items = args[0].split('\0')[:-1] # the long chain of arguments is splitted here
            tasks = TaskList()
            for i in xrange(len(items)/NUM_TASK_ITEMS):  
              t = TmTask()
              # calling of the fill routine that will put the arguments into the object
              if len(items[i*NUM_TASK_ITEMS])>0:
                try:
                  #print items[i*NUM_TASK_ITEMS:(i+1)*NUM_TASK_ITEMS]
                  t.fill(items[i*NUM_TASK_ITEMS:(i+1)*NUM_TASK_ITEMS]) 
                  tasks.append(t)
                except:
                  print 'Failed to decode task information......'
            all_tasks = copy.deepcopy(self.tasks)
            good_tasks = {}
            num_req_tasks = len(all_tasks.keys())
            err = ''
            mem = 0.0
            cpu = 0.0
            rss = 0
            vsize = 0
            data = 0
            stack = 0
            for t in tasks:
              utgid = t.utgid.replace(self.name,'<Node>')
              if all_tasks.has_key(utgid):
                good_tasks[utgid] = all_tasks[utgid]
                del all_tasks[utgid]
              if t.cmd == 'init':
                err = '\t\t<Boot time="%s"/>\n'%(t.started,)
              mem = mem + t.perc_mem
              cpu = cpu + t.perc_cpu
              rss = rss + t.rss
              data = data + t.data
              vsize = vsize + t.vsize
              stack = stack + t.stack

            err = err + '\t\t<System perc_cpu="%f" perc_memory="%f" vsize="%d" rss="%d" data="%d" stack="%d"/>\n'%(cpu,mem,vsize,rss,data,stack,)
            keys = all_tasks.keys()
            err = err + '\t\t<Tasks count="%d" ok="%d" missing="%d">\n'%(len(self.tasks.keys()),len(good_tasks.keys()),len(keys),)
            for i in keys:
              err = err + '\t\t\t<Task name="'+i.replace('<Node>',self.name)+'" status="Not OK"/>\n'
            if s_fullInfo:
              for i in good_tasks:
                err = err + '\t\t\t<Task name="'+i.replace('<Node>',self.name)+'" status="OK"/>\n'
            err = err + '\t\t</Tasks>\n'
            self.errors = err

        except Exception,X:
          import traceback
          traceback.print_exc()
          print 'Something went wrong:',str(X)
          #print 'Exception handling finished.'

    def start(self):
        #return self.startClient('/'+self.name.upper()+'/ps/data','C:200000',self.callback)
        return self.startClient('/'+self.name.upper()+'/ps/data','C',self.callback)

    def statusInfo(self):
        if (time.time() - self.time) > 35:
          self.errors = '    <Node name="'+self.name+'" status="DEAD" last="+self.time+"/>"\n'
          print self.errors
          return self.errors
        else:
          if (time.time() - self.connTime) > 30: self.checkConnections()
          err = '\t<Node name="'+self.name+'" status="ALIVE" time="'+time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(self.time))+'">\n'
          err = err + self.errors
          err = err + self.connerrors
          err = err + '\t</Node>\n'
          return err


class TaskSupervisor:
    """
        An instance of the TaskSupervisor class is able to
        start the NodeMonitors in each node and then retrieve
        their informations to publish it in each node in a
        service called after it: TaskSupervisor.

        First, the constructor initializes the necessary values,
        one of them is the name of the service (with the node
        name included).

        The initialize function using the XMLTaskList script
        checks the existance of nodes. Then the start function
        makes each NodeMonitor begin their work.

        The run function invokes the publish function each
        second. Then the publish function polls for the status
        of the NodeMonitors, based on their response, it
        assigns DEAD, MIXED or ALIVE  status and publishes.

        The results are framed in XML tags to match the
        information received from the NodeMonitors

        ------------------

    
    An instance of thie class is able to supervise a collection of
    processing nodes. To do so it will:
        - Query the task inventory to retrieve the list of tasks
        executing on the controls PC and the worker nodes.
        - Delegate the checking of the task existence to the
        node class and process the return code indicating
        if a node works properly or not.
        - Publish the result indicating if ALL nodes work properly
        or show errors.
    @author R.Bravo
    @version 1.0
    """
    def __init__(self,name,inventory):
        "Initialize the object"
        self.node = name
        self.name = '/'+name.upper()+'/TaskSupervisor'
        self.inventory = inventory
        self.nodes = None
        self.monitors = {}        
        svc = self.name+'/Status'
        dimc.dis_set_dns_node('ecs03')  # a call can be individually made with this command
        #dimc.dis_set_dns_node(os.environ['DIM_DNS_NODE'])
        
        #self.serviceId = dimc.dis_add_service(svc,'C:50000',self.serviceCall,1)
        self.serviceId = dimc.dis_add_service(svc,'C',self.serviceCall,1)
        dimc.dis_start_serving(self.name)
        dimc.dic_set_dns_node(name)
        self.list = {}

    def initialize(self):
        "Load the database and check for consistency"
        nodes = self.inventory.getNodes()
        print 'Nodes:',nodes.keys()
        if nodes is None:
          return error('Failed to retrieve the nodelist from the inventory.')
        elif len(nodes) == 0:
          return error('Node list for ',self.node,' is of ZERO length. This cannot be.')
        else:
          debug('Yes: I got a node list with ',len(nodes),' entries. All fine so far.')
        self.nodes = nodes
        for node in self.nodes.keys():
          self.monitors[node] = NodeMonitor(node,self.nodes[node])
        return S_SUCCESS

    def start(self):
        "Start controlling the associated nodes."
        for node in self.monitors.keys():
          status = self.monitors[node].start()
          if status != S_SUCCESS:
            return error('Failed to start monitor for node:',node)
        info('All task monitors for ',self.node,' are now running.')
        return S_SUCCESS
    
    def serviceCall(self,*args):
        print 'serviceCall--------'
        print 'serviceCall',args
        return ('whatever',)

    def publish(self):
        er = ''
        sc1 = "DEAD"
        sc2 = "ALIVE"
        for node,mon in self.monitors.items():
          e = mon.statusInfo()
          if e is not None:
            dead  = e.find('status="DEAD"')>0
            alive = e.find('status="ALIVE"')>0
            if alive and sc1=="DEAD":
              sc1 = "MIXED"
            if dead and sc2=="ALIVE":
              sc2 = "MIXED"
            er = er + e
        err = '<Cluster name="'+self.node.upper()+'" status="'
        if sc1=="DEAD":     err=err+sc1
        elif sc2=="ALIVE":  err=err+sc2
        else:               err=err+sc1
        err = err + '" time="'+time.strftime('%Y-%m-%d %H:%M:%S')+'">\n' + er + "  </Cluster>"
        #print err
        e = str(err)
        #print '================================\n',type(e),e
        res = dimc.dis_update_service(self.serviceId,(e,))
    
    def run(self):
        "Here I can sleep while my monitoring slaves actually have to struggle."
        while(1):
            try:
              time.sleep(PUBLISH_TIME)
              self.publish()
            except:
              pass



def callTaskSupervisor():
    """
        The callTaskSupervisor function loads the Master Tasklist,
        the TaskInventory and the Nodelist. It also starts the
        TaskSupervisor
    """
    node = os.environ['DIM_DNS_NODE']
    xml = XMLTaskList.TransformXmlToObjects()
    xml.load('../xml/TaskInventory.xml')      # loads the Task Inventory
    xml.load('../xml/'+node.upper()+'.xml')   # loads the Node List
    sup = TaskSupervisor(node,xml)
    sup.initialize()
    sup.start()
    sup.run()
    print '.....Task supervisor stopped executing.....'

if __name__=="__main__":
    callTaskSupervisor()
