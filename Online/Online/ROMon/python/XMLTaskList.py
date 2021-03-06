# XML Parser
# Chourouk Fanane

import xml.dom.minidom
class NodeList:
    def __init__(self,name):
        self.Name = name
        self.nodes = {}

    def addNode(self, node):
        self.nodes[node.Name] = node

    def __repr__(self):
        s = "NodeList %s contains:" % self.Name
        for x in self.nodes:
            s += "\n    -> %s\n"  %str(self.nodes[x])
        return s

class Node:
    def __init__(self,name):
        self.Name = name
        self.taskLists = {}
        self.connectionLists = {}

    def addTaskList(self, taskList):
        self.taskLists[taskList.Name] = taskList

    def addConnectionList(self, connectionList):
        self.connectionLists[connectionList.Name] = connectionList

    def __repr__(self):
        s = "\n---> %s contains:" % self.Name
        for x in self.taskLists:
            s += "\n\t\t- %s"  %str(self.taskLists[x])
        for x in self.connectionLists:
            s += "\n\t\t- %s"  %str(self.connectionLists[x])
        return s

class TaskList:
    def __init__(self,name):
        self.Name = name
        self.tasks = {}

    def addTask(self, task):
        self.tasks[task.Name] = task

    def __repr__(self):
        s = "TaskList %s contains:" % self.Name
        for x in self.tasks:
            s += "\n\t\t\t-%s"  %str(self.tasks[x])
        return s

class ConnectionList:
    def __init__(self,name):
        self.Name = name
        self.connections = {}

    def addConnection(self, connection):
        self.connections[connection] = connection

    def __repr__(self):
        s = "ConnectionList %s contains:" % self.Name
        for x in self.connections:
            s += "\n\t\t\t-Connection: '%s'"  %str(self.connections[x])
        return s


class Task:
    ApplicationName = None
    Path = None
    MachineName = None
    Responsible = None
    UserName = None
    DimServiceName = None
    DimDnsNode = None
    Name = None

    def __init__(self,name):
        self.Name = name

    def __str__(self):
        s = "Task %s: AppName=%s " %(self.Name, self.ApplicationName)
        return s

    def __repr__(self):
        return str(self)

class TransformXmlToObjects:
    __currentNode__ = None
    __taskLists = None
    __tasks = None
    __nodes = None
    __nodeLists = None

    def __init__(self):
        self.__tasks = {}
        self.__taskLists = {}
        self.__nodes = {}
        self.__nodeTypes = {}
        self.__nodeLists = {}
        self.__connectionLists = {}

    def load(self,name):
        from xml.dom.minidom import parse
        self.doc = parse(name)
        root = self.doc.documentElement
        for task in root.getElementsByTagName("Task"):
            if task.parentNode != root:
                continue
            if task.nodeType == task.ELEMENT_NODE:
                tname = task.getAttribute('name')
                if not self.__tasks.has_key(tname):
                    self.__tasks[tname] = Task(tname)
                t = self.__tasks[tname]
                type = task.getAttribute('type')
                if len(type)>0: t.ApplicationName = type
                else: t.ApplicationName = tname
                
                elt = task.getElementsByTagName("Path")
                if len(elt)>0: t.Path = self.getText(elt[0])
                elt = task.getElementsByTagName("MachineName")
                if len(elt)>0: t.MachineName =self.getText(elt[0])
                elt = task.getElementsByTagName("UserName")
                if len(elt)>0: t.UserName =self.getText(elt[0])
                elt = task.getElementsByTagName("DimServiceName")
                if len(elt)>0: t.DimServiceName =self.getText(elt[0])
                elt = task.getElementsByTagName("DimDnsNode")
                if len(elt)>0: t.DimDnsNode =self.getText(elt[0])

        for tl in root.getElementsByTagName("TaskList"):
            if tl.parentNode != root:
                continue
            if tl.nodeType == tl.ELEMENT_NODE:
                name = tl.getAttribute('name')
                if not self.__taskLists.has_key(name):
                    self.__taskLists[name] = TaskList(name)
                for x in tl.getElementsByTagName("Task"):
                    tName = x.getAttribute('name')
                    if not self.__tasks.has_key(tName):
                        self.__tasks[tName] = Task(tName)
                    self.__taskLists[name].addTask(self.__tasks[tName])

        for cl in root.getElementsByTagName("ConnectionList"):
            if cl.parentNode != root: continue
            if cl.nodeType == cl.ELEMENT_NODE:
                name = cl.getAttribute('name')
                if not self.__connectionLists.has_key(name):
                    self.__connectionLists[name] = ConnectionList(name)
                for x in cl.getElementsByTagName('Connection'):
                    print name,x.getAttribute('name')
                    self.__connectionLists[name].addConnection(x.getAttribute('name'))

        for n in root.getElementsByTagName("NodeType"):
            if n.parentNode != root: continue
            if n.nodeType == n.ELEMENT_NODE:
                name = n.getAttribute('name')
                if not self.__nodeTypes.has_key(name):
                    self.__nodeTypes[name] = Node(name)
                #
                # Add tasklist definitions
                #
                for x in n.getElementsByTagName("TaskList"):
                    nName = x.getAttribute('name')
                    if not self.__taskLists.has_key(nName):
                        self.__taskLists[nName] = TaskList(nName)
                    self.__nodeTypes[name].addTaskList(self.__taskLists[nName])
                # Add intrinsic tasks to local node type....
                tskList = TaskList('Intrinsic')
                self.__nodeTypes[name].addTaskList(tskList)
                for x in n.getElementsByTagName("Task"):
                    tName = x.getAttribute('name')
                    if not self.__tasks.has_key(tName):
                        self.__tasks[tName] = Task(tName)
                    tskList.addTask(self.__tasks[tName])
                #
                # Add connections to other computers
                #
                for x in n.getElementsByTagName('ConnectionList'):
                    nName = x.getAttribute('name')
                    if not self.__connectionLists.has_key(nName):
                        self.__connectionLists[nName] = ConnectionList(nName)
                    print name,nName
                    self.__nodeTypes[name].addConnectionList(self.__connectionLists[nName])
                # Add intrinsic connections to local node type....
                cList = ConnectionList('Intrinsic')
                for x in n.getElementsByTagName('Connection'):
                    cList.addConnection(x.getAttribute('name'))
                self.__nodeTypes[name].addConnectionList(cList)

        for nl in root.getElementsByTagName("NodeList"):
            if nl.parentNode != root:
                continue
            if nl.nodeType == nl.ELEMENT_NODE:
                name = nl.getAttribute('name')
                if not self.__nodeLists.has_key(name):
                    self.__nodeLists[name] = NodeList(name)
                for n in nl.getElementsByTagName('Node'):
                    node = n.getAttribute('name')
                    type = n.getAttribute('type')
                    if not self.__nodes.has_key(node):
                        self.__nodes[node] = Node(node)
                    if not self.__taskLists.has_key(type):
                        self.__taskLists[type] = TaskList(type)
                    self.__nodes[node].taskLists = self.__nodeTypes[type].taskLists
                    if not self.__connectionLists.has_key(type):
                        self.__connectionLists[type] = ConnectionList(type)
                    self.__nodes[node].connectionLists = self.__nodeTypes[type].connectionLists
                    self.__nodeLists[name].addNode(self.__nodes[node])

        print 'Load done.'
    def getTasks(self):
        return self.__tasks

    def getText(self, node):
        return node.childNodes[0].nodeValue

    def getTaskLists(self):
        return self.__taskLists

    def getNodes(self):
        return self.__nodes

    def getNodeTypes(self):
        return self.__nodeTypes

    def getNodeLists(self):
        return self.__nodeLists


if __name__ == "__main__":
    #x = TransformXmlToObjects('tasks.xml')
    x = TransformXmlToObjects()
    x.load('TaskList.xml')
    x.load('TaskInventory.xml')
    x.load('mona08.xml')

    for t in x.getTasks().values(): print t
    for tl in x.getTaskLists().values(): print tl
    for n in x.getNodeTypes().values() : print n
    for n in x.getNodes().values() : print n
    for nl in x.getNodeLists().values() : print nl
