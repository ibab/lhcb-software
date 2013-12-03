# -*- coding: utf-8 -*-
from GaudiPython.Bindings import iDataSvc, SUCCESS, FAILURE

 

def _dir_ ( self , node = None , forceload = False, level = -1 , ondemand = False ) :

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
    elif  not node         : return self.dir('', forceload, level , ondemand )

    if not node            : return FAILURE
    if hasattr ( node , 'registry'   ) : node = node.registry()

    if hasattr ( node , 'identifier' ) :
        id  = node.identifier () 
        obj = node.object     ()
        if not obj or not type( obj ) : 
            print  " %-60s " % id 
        else :
            if hasattr ( obj , 'containedObjects' ) and not obj.empty()  :
                c = obj.containedObjects()[0]
                print " %-60s container of %-5d %s objects " % ( id , obj.size() , type(c).__name__ )
            elif hasattr( obj , 'size'    ) :
                size = obj.size()
                if 0 == size : print " %-60s (empty)  \t%s " % ( id , type ( obj ) )
                else         : print " %-60s #=%-5d   \t%s " % ( id , obj.size() , type ( obj ).__name__ )  
            else :
                print " %-60s \t\t%s " % ( id , type( obj ).__name__ )


    else : print " The node has no 'identifier'"
    # start the recursion

        
    for l in self.leaves( node ) :
        if forceload :
            try :
                temp = self[l.identifier()]
            except :
                temp = self[l.identifier()]
        if l : self.dir( l , forceload, level - 1 , ondemand ) 

    if ondemand and hasattr ( node , 'identifier' ) :

        s   = set()
        id  = node.identifier()
        id1 = id.split('/')
        
        from Configurables import DataOnDemandSvc as DoD
        dod = DoD () 

        dmap = dod.NodeMap
        for key_ in dmap :
            key = key_ if 0 == key_.find('/Event/') else '/Event/' + key_
            if 0 == key.find ( id ) :
                nkey = '/'.join(key.split('/')[:-1])
                nkey = key.split('/')
                if len ( nkey ) > len( id1 ) :
                    nkey = nkey[:len(id1)+1]                
                nkey = '/'.join(nkey)
                if id == '/'.join(key.split('/')[:-1]) :
                    if not nkey in s : 
                        print "*%-60s \t\t%s"  % ( nkey , 'Directory')
                        s.add( nkey )
                        
        amap = dod.AlgMap
        for key_ in amap :
            key = key_ if 0 == key_.find('/Event/') else '/Event/' + key_
            if 0 == key.find ( id ) :
                nkey  = key.split('/')
                if len ( nkey ) > len( id1 ) :
                    nkey = nkey[:len(id1)+1]
                nkey  = '/'.join(nkey)
                alg = amap[key_]
                if hasattr ( alg , 'getFullName' ) : alg = alg.getFullName()
                if not nkey in s : 
                    print "*%-60s \t\t%s"  % ( nkey , alg )
                    s.add ( nkey )
                        
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

