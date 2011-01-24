from GaudiPython.Bindings import iDataSvc, SUCCESS, FAILURE
 

def _dir_ ( self , node = None , forceload = False, level = -1 ) :

    """

    A simple tool to perform the inspection of Data Store 

    Usage:

    >>> evtSvc = gaudi.evtSvc()
    >>> evtSvc.dir('MC')
    >>> data = evtSvc['Rec/Calo']
    >>> evtSvc.dir( data)
    >>> evtSvc.dir('Rec', forceload=True)
   
    """

    if 0 == level          : return SUCCESS
    if   str is type(node) : node = self.retrieveObject( node )
    elif  not node         : return self.dir('', forceload, level )
    
    if not node            : return FAILURE
    if hasattr ( node , 'registry'   ) : node = node.registry()
    if hasattr ( node , 'identifier' ) :
        obj = node.object ()
        if not obj :  print  node.identifier()
        else :
            if hasattr( obj , 'size'    ) :
                size = obj.size()
                if 0 == size : print "%s (empty) %s" % (  node.identifier() , type( obj ) )
                elif hasattr ( obj , 'containedObjects' ) :
                    c = obj.containedObjects()[0]
                    print "%s container of %d %s objects " % ( node.identifier() , obj.size() , type(c))
                else :
                    print  "%s %s " % (  node.identifier() , type( node.object()) )
            else :
                print  "%s %s " % (  node.identifier() , type( node.object()) )

    else : print "The node has no 'identifier'"
    # start the recursion

    for l in self.leaves( node ) :
        if forceload :
            try :
                temp = self[l.identifier()]
            except :
                temp = self[l.identifier()]
        if l : self.dir( l , forceload, level - 1 ) 

    return SUCCESS

def _nodes_(self, forceload = False, node = '') :
    """

    A simple tool to obtain list of nodes in Data Store 

    Usage:

    >>> evtSvc = gaudi.evtSvc()
    >>> nodes = evtSvc.nodes('Rec')
    >>> for node in nodes :
    >>>  print node
    >>> nodes = evtSvc.nodes('Rec', forceload = True)
    >>> for node in nodes :
    >>>  print node
    """
    nodelist = []
    if type(node) is str:
        root = self.retrieveObject(node)
        if root : node = root.registry()
        else    : return nodelist
    if node.object() :
        if self.leaves(node).size() > 0 :
            for l in self.leaves(node) :
                if forceload :
                    try :
                        temp = self[l.identifier()]
                    except :
                        temp = self[l.identifier()]
                nodelist+=self.nodes(forceload,l)
        else :
            nodelist.append(node.identifier())
    else:
        nodelist.append(node.identifier())
    return nodelist

iDataSvc.nodes = _nodes_
iDataSvc.dir = _dir_
iDataSvc.ls  = _dir_

