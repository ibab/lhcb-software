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

import os, sys, time, dimc, XMLTaskList
from Helpers import *

class TaskList(list):
    """
        An instance of the TaskList class represents a collection of
        task object as it results from reading the XML database.
    """
    def __init__(self):
        "Standard constructor"
        list.__init__(self)

class TmTask:               # initialization of the structure with 34 attributes per task
    """

        The TmTask class  initializes the structure of the 'Task'
        object with the 34 attributes of each task and when its
        fill function is invoked it assigns a number to each one of the
        attributes to be filled.

        HEADER and SHOW are samples of the information that can
        be displayed.
        
    """
    def __init__(self):
        self.utgid = None
        self.tid = None
        self.tgid = None
        self.ppid = None
        self.pgid = None
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
        self.perc_cpu = None
        self.perc_mem = None
        self.vsize = None
        self.lock = None
        self.rss = None
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
        self.perc_cpu = items[16]
        self.perc_mem = items[17]
        self.vsize = int(items[18])
        self.lock = items[19]
        self.rss = items[20]
        self.data = items[21]
        self.stack = items[22]
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


class NodeMonitor(DimClient):
    """
        An instance of the NodeMonitor class polls the /ps/data
        service in the current Node to see what tasks are being
        executed in that moment.

        The callback function defined in the class splits the
        arguments read from /ps/data into 34 pieces which is
        the number of attributes each task has. A TmTask
        structure is then filled with all the 34 attributes for
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
    def __init__(self,node,tasks):
        "Standard constructor"
        DimClient.__init__(self,node)
        self.tasks = tasks
        self.errors = None
        self.time = 0
        print 'Created node monitor:',self.name
        
    def callback(self,*args): # separates each item from the chain
        try:
            if len(args)==0:\
                return
            self.time = time.time()
            items = args[0].split('\0')[:-1] # the long chain of arguments is splitted here
            tasks = TaskList()
            for i in xrange(len(items)/34):  
                t = TmTask()
                # calling of the fill routine that will put the arguments into the object
                if len(items[i*34])>0:
                  t.fill(items[i*34:(i+1)*34]) 
                  tasks.append(t)
            node_tasks = self.tasks
            all_tasks = {}

            for l in node_tasks.taskLists.keys():
                for t in node_tasks.taskLists[l].tasks.keys():
                    all_tasks[t] = node_tasks.taskLists[l].tasks[t]
            num_req_tasks = len(all_tasks.keys())

            for t in tasks:
                utgid = t.utgid.replace(self.name,'<Node>')
                if all_tasks.has_key(utgid):
                    del all_tasks[utgid]
            err = '    <Node name="'+self.name+'" status="ALIVE" time="'+time.strftime('%Y-%m-%d %H:%M:%S')+'">\n'
            if len(all_tasks)>0:
                keys = all_tasks.keys()
                err = err + '      <Missing_Tasks  count="%d">\n'%(len(keys),)
                for i in keys:
                    utgid = i.replace('<Node>',self.name)
                    err = err + '         <Task name="'+utgid+'"/>\n'
                err = err + '      </Missing_Tasks>\n'
                err = err + '      <Found_Tasks count="%d"/>\n'%(num_req_tasks,)
                err = err + '    </Node>\n'
                self.errors = err
            else:
                self.errors = None
                self.errors = err + '      <Found_Tasks count="%d"/></Node>\n'%(num_req_tasks,)

        except Exception,X:
            import traceback
            traceback.print_exc()
            print 'Something went wrong:',str(X)
            print 'Exception handling finished.'

    def start(self):
        #info('Node monitor for ',self.name,' started.....')
        return self.startClient('/'+self.name.upper()+'/ps/data','C:200000',self.callback)

    def statusInfo(self):
        if (time.time() - self.time) > 35:
            #print 'No answer since ', self.time, time.time() - self.time
            self.errors = '    <Node name="'+self.name+'" status="DEAD"/>"\n'
        return self.errors




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
        "Iniitalize the object"
        self.node = name
        self.name = '/'+name.upper()+'/TaskSupervisor'
        self.inventory = inventory
        self.nodes = None
        self.monitors = {}        
        svc = self.name+'/Status'
        dimc.dis_set_dns_node('ecs03')  # a call can be individually made with this command
        #dimc.dis_set_dns_node(os.environ['DIM_DNS_NODE'])
        
        self.serviceId = dimc.dis_add_service(svc,'C:20000',self.serviceCall,1)
        dimc.dis_start_serving(self.name)
        dimc.dic_set_dns_node(name)
        self.list = {}

    def initialize(self):
        "Load the database and check for consistency"
        nodes = self.inventory.getNodes()
        print 'Nodes:',nodes
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
        nodelist = []
        for node in self.monitors.keys():
            nodelist.append(node)
            status = self.monitors[node].start()
            if status != S_SUCCESS:
                return error('Failed to start monitor for node:',node)
        info('All task monitors for ',self.node,' are now running.')
        #print nodelist
        return S_SUCCESS
    
    def serviceCall(self,*args):
        print 'serviceCall--------'
        print 'serviceCall',args
        return ('whatever',)

    def publish(self):
        er = ''
        sc1 = "DEAD"
        sc2 = "ALIVE"
        #clearscreen()
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
        err = '  <Cluster name="'+self.node.upper()+'" status="'
        if sc1=="DEAD":     err=err+sc1
        elif sc2=="ALIVE":  err=err+sc2
        else:               err=err+sc1
        err = err + '" time="'+time.strftime('%Y-%m-%d %H:%M:%S')+'">\n' + er + "  </Cluster>"
        res = dimc.dis_update_service(self.serviceId,(err,))
        #info('dis_update_service:'+str(res)+'\n'+err)
    
    def run(self):
        "Here I can sleep while my monitoring slaves actually have to struggle."
        collect=0
        while(1):
            collect = collect + 1
            time.sleep(2)
            if (collect%1)==0:
                self.publish()



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


if __name__=="__main__":
    callTaskSupervisor()
