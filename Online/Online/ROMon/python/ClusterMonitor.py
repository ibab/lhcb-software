"""
    Cluster Monitor
    by: Raul Duran Bravo

    This script collects the information of the TaskSupervisors
    that are running in the clusters and then publishes the
    collective information creating a service called
    'ClusterCollector' in the cluster where it was invoked.

    Failing status for Clusters are DEAD and MIXED.
    
    A cluster is declared DEAD when its TaskSupervisor
    could not be interrogated for whatever reason.

    A cluster is declared MIXED when its TaskSupervisor
    exists but has error messages, then it proceeds to
    publish the information obtained from the corresponding
    TaskSupervisor.


    Functions that are similar to those in the TaskSupervisor
    script are:
      - error
      - info
      - debug
      - clearscreen

    Classes that are similar to those in the TaskSupervisor
    script are:
      - DimClient

    Classes that are exclusive of ClusterMonitor are:
    - TaskSupervisorClient
    - ClusterSupervisor
    - ClusterCollector

    Note: It uses an external script named XMLTaskList to
    decode the XML information

"""

import dimc, XMLTaskList
from Helpers import *

ADDING    = 1
DYING     = 2
LISTING   = 3

class TaskSupervisorClient(DimClient):
    """
        An instance of the TaskSupervisorClient class subscribes
        to the TaskSupervisor service in a node.
    """
    def __init__(self,name):
        DimClient.__init__(self,name)
        self.data = ''

    def callback(self,*args):
        try:
            if len(args)>0:
                self.data = args[0]
            else:
                info('TaskSupervisorClient received invalid data.')

        except Exception,X:
            import traceback
            traceback.print_stack()
            print 'Something went wrong:',str(X)
            print 'Exception handling finished.'

    def start(self):
        #info('TaskSupervisorClient for ',self.name,' started.....')
        return self.startClient('/'+self.name.upper()+'/TaskSupervisor/Status','C:20000',self.callback)

class ClusterCollector:
    """
        An instance of the ClusterCollector class suscribes
        to the DIS_DNS/SERVER_LIST service and extracts the
        list of processes that are running in each node of
        the cluster.

        It also looks for added and removed processes to put
        them in the ALIVE or DEAD lists.

        Then the information is formatted in XML and
        published by a process called ClusterCollector
    """
    def __init__(self,inventory,name='ClusterCollector'):
        self.name = name
        self.svcID = None
        self.dead = {}
        self.alive = {}
        self.errors = ''
        self.inventory = inventory
        self.monitors = {}
        self.node = name
        svc = self.name+'/status'
        self.serviceId = dimc.dis_add_service(svc,'C:20000',self.serviceCall,1)
        dimc.dis_start_serving(self.name)
        lst = self.inventory.getNodeLists()
        s = ''

        for i in lst.keys():
            l = lst[i]
            s = s + ' ' + l.Name+ ' ['+str(len(l.nodes))+'] '
            client = TaskSupervisorClient(l.Name)
            self.dead[l.Name.upper()] = client
            #print s

    def __del__(self):
        if self.svcID is not None:
            dimc.dic_release_service(self.svcID)
      
    def show(self):
        data = ''

        for n in self.dead.keys():
            data = data + '  <Cluster name="'+n+'" status="DEAD"/>\n'

        for n in self.alive.keys():
            data = data + self.alive[n].data + '\n'
        self.errors = data
        #info('State summary')
        return data    

    def startx(self):
        """Connect to the DIS_DNS/SERVER_LIST datapoint. Then:
        -- Select all availible TaskSupervisor status datapoints status datapoint?

        -- Collect the result
        -- Publish the summary information to be eventually
        put into PVSS.
        """
        self.svcID=dimc.dic_info_service('DIS_DNS/SERVER_LIST','C:200000',self.callback)
        return self

    def callback(self,*args):
        """
        """
        try:
            #print len(self.alive)
            if len(args) == 0:
                print 'There is nothing in DIS_DNS/SERVER_LIST '
                return
            items = args[0].split('|')

            for i in items:
                itm,node = i.split('@')
                type = 0
                if   itm[0] == '+':
                    type = ADDING
                    itm = itm[1:]
                elif itm[0] == '-':
                    type = DYING
                    itm = itm[1:]
                else:
                    type = LISTING
        
                sup = itm[1:].split('/')
                if len(sup)==2 and sup[1]=='TaskSupervisor':
                    nam = sup[0].upper()
                    if type == ADDING or type == LISTING:
                        client = TaskSupervisorClient(nam)
                        self.alive[nam] = client
                        client.start()
                        del self.dead[nam]
                        #print 'Added new client:',nam,' to ALIVE list'
                    elif type == DYING:
                        if self.alive.has_key(nam):
                            client = self.alive[nam]
                            client.stop()
                            self.dead[nam] = client
                            del self.alive[nam]
                            #print 'Moved client:',nam,' to DEAD list'

        except Exception,X:
            print str(X)

    def start(self):
        "Start controlling the associated nodes."
        for node in self.monitors.keys():
            status = self.monitors[node].start()
            if status != S_SUCCESS:
                # This should never fail, because nobody is monitoring me!!!!
                return error('Failed to start monitor for node:',node)
        info('All task monitors for ',self.node,' are now running.')
        return S_SUCCESS

    def serviceCall(self,*args):
        print 'serviceCall--------'
        print 'serviceCall'
        return ('whatever',)

    def publish(self):
        err = "<Network>\n"
        err = err + self.show()
        err = err + "</Network>\n"
        res = dimc.dis_update_service(self.serviceId,(err,))
        # all errors are published here
        info('dis_update_service:'+str(res)+'\n'+err)
    
    def run(self):
        "Here I can sleep while my monitoring slaves actually have to struggle."
        collect=0
        while(1):
            collect = collect + 1
            time.sleep(1)
            if (collect%1)==0:
                clearscreen()
                self.publish()


def clusterMonitor():
    """
        The clusterMonitor function loads the necessary lists,
        for now, the XML Nodelists are static. After that, the
        ClusterCollector is started
    """
    node = os.environ['DIM_DNS_NODE']
    xml = XMLTaskList.TransformXmlToObjects()
    xml.load('../xml/TaskInventory.xml')  # loads the Task Inventory
    xml.load('../xml/HLTD01.xml')   # loads the Node List
    xml.load('../xml/HLTD02.xml')   # loads the Node List
    xml.load('../xml/HLTD03.xml')   # loads the Node List
    xml.load('../xml/HLTD04.xml')   # loads the Node List
    xml.load('../xml/HLTD06.xml')   # loads the Node List
    xml.load('../xml/HLTD07.xml')   # loads the Node List
    xml.load('../xml/HLTD08.xml')   # loads the Node List
    xml.load('../xml/HLTD09.xml')   # loads the Node List
    xml.load('../xml/HLTD10.xml')   # loads the Node List
    xml.load('../xml/HLTD11.xml')   # loads the Node List
    xml.load('../xml/HLTE04.xml')   # loads the Node List
    xml.load('../xml/HLTE06.xml')   # loads the Node List
    xml.load('../xml/'+node.upper()+'.xml')   # loads the Node List
    collector = ClusterCollector(xml)
    collector.startx()
    collector.run()

if __name__=="__main__":
    clusterMonitor()
