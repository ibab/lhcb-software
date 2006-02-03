#!/usr/bin/env python
# =============================================================================
# $Id: VolumeCheck.py,v 1.1 2006-02-03 11:13:13 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
# =============================================================================
import os 
import gaudimodule as gaudi


def init() :

    #gaudi.loaddict('DetDict')
    g=gaudi.AppMgr()
    g.DLLs   += [ "XmlTools"   , "DetDescCnv"    ,
                  "DetDescSvc" , "DetDescChecks" , "RootHistCnv" ]
    g.ExtSvc += [ "XmlCnvSvc" , "XmlParserSvc" ] 
    g.EvtSel = "NONE"
    
    det = g.service('DetectorPersistencySvc')
    det.CnvServices = ["XmlCnvSvc"]

    det = g.service('XmlCnvSvc')
    det.AllowGenericConversion = True
    
    det = g.service('DetectorDataSvc')
    det.UsePersistency = True
    det.DetDbRootName  = "dd"
    det.DetStorageType = 7
    det.DetDbLocation  = os.environ['XMLDDDBROOT']+'/DDDB/lhcb-200601.xml'
    

def run( volume , event = 1 ) :
    g = gaudi.AppMgr() 
    g.TopAlg = ["VolumeCheckAlg/Check"]
    
    alg = g.algorithm('Check')
    alg.Shots3D = 10000
    alg.ShotsXY = 10000
    alg.ShotsYZ = 10000
    alg.ShotsZX = 10000
    alg.Volume  = volume
    g.run( event )
    
def _dir_ ( self , node = None , level = -1 ) :
    """
    The simple tool to perform the inspection fo Data Store
    
    Usage:
    
    evtSvc = gaudi.evtSvc()
    evtSvc.dir('MC')
    data = evtSvc['Rec/Calo']
    evtSvc.dir( data)
    
    """
    if 0 == level          : return gaudi.SUCCESS ;
    if   str is type(node) : node = self.retrieveObject( node )
    elif  not node         : return self.dir('', level )
    if not node            : return gaudi.FAILURE
    if     hasattr ( node , 'registry'   ) : node = node.registry()
    if     hasattr ( node , 'identifier' ) :
        obj = node.object ()
        if not obj :
            obj = self.retrieveObject( node.identifier() ) 
            print  node.identifier()
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
        if l : self.dir( l , level - 1 )  
    return gaudi.SUCCESS 

def _get_( self , node = None , cut = lambda x : True ) :
    if   str is type(node) : node = self.retrieveObject( node )
    elif  not node         : return _get_( self , '' , cut  )
    if not node            : return []
    if     hasattr ( node , 'registry'   ) : node = node.registry()
    result = []
    if     hasattr ( node , 'identifier' ) :
        obj = node.object()
        if cut( obj ) : result += [ obj ]
    for l in self.leaves ( node ) :
        if l : result += _get_( self , l , cut )
    return result 
    
gaudi.iDataSvc.dir = _dir_ 
gaudi.iDataSvc.DIR = _get_ 


if "__main__" == __name__  :
    init()
    run ("/dd/Geometry/MagnetRegion/Magnet/Magnet")


