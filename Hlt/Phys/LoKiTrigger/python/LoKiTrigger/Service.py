#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file LoKiTrigger/Service.py
#  Useful decorator for Hlt::Service 
#  @see Hlt::IRegister 
#  @see Hlt::IData
#  @see Hlt::IInspector 
#  @see Hlt::Serice 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-03-25
# =============================================================================
"""
Useful decorator for HLT Service and its interfaces
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision$"
# =============================================================================
__all__ = ( 'iHltInspector', )
# =============================================================================
import GaudiPython.Bindings
import os.path,sys

cpp           = GaudiPython.Bindings.gbl
iService      = GaudiPython.Bindings.iService
iAlgorithm    = GaudiPython.Bindings.iAlgorithm
Helper        = GaudiPython.Bindings.Helper
InterfaceCast = GaudiPython.Bindings.InterfaceCast
AppMgr        = GaudiPython.Bindings.AppMgr 
Hlt           = cpp.Hlt


def selNode ( s ) : return ' %s '    % s 
def tesNode ( l ) : return ' %s '    % l
def algNode ( a ) : return ' Alg%s ' % a

def selLine ( s ) :
    return " \t\t %s [shape=ellipse,color=yellow,style=filled];" % selNode ( s ) 

def tesLine ( l ) :
    return " \t\t %s [shape=ellipse,color=green,style=filled];"  % tesNode ( l ) 

def algLine ( a ) :
    return " \t\t %s [label=%s,shape=box,color=red,style=filled];" % ( algNode ( a ) , a )  

def arrow   ( s1 , s2 ) :
    return ' \t\t\t  %s  -> %s [shape=vee];' % ( s1, s2 ) 

# ============================================================================
## @class iHltInspector
#  python image of class Hlt::Inspector, which allows interactive inspection
#  of HLT Service
#  @see Hlt::IInspector
#  @see Hlt::Service 
class iHltInspector(iService) :
    """
    Python image of Hlt::IInspector service for inspection of HLT Service 
    """
    def __init__( self , name , isvc ) :
        """
        Constructor from the name and the service 
        """
        iService.__init__( self , name, isvc )
        self.__dict__['_ihltis'] = InterfaceCast(Hlt.IInspector) (isvc)
        self.__dict__['_ihltds'] = InterfaceCast(Hlt.IData)      (isvc)
        self.__dict__['_ihltrs'] = InterfaceCast(Hlt.IRegister)  (isvc)
        
    ## retrieve the interface (needef by framework)
    def retrieveInterface(self) :
        """
        Retrieve the interface (needef by framework)
        """
        iService.retrieveInterface ( self )
        iHltInspector.__init__ ( self , self._name , self._isvc ) 
        
    ## find a producer for the given selection
    #  @see Hlt::IInspector::producer 
    def producer ( self , selection ) :
        """
        Find the producer for the given selection

        >>> svc = ...                                 # get the service
        >>> pp1 = svc.producer ( 'SelectionByName' )  # find by selecltion name

        >>> sel = ...    
        >>> pp2 = svc.producer () # find by Hlt::Selection 
        
        """
        if not self._ihltis : self.retrieveInterface()
        alg = self._ihltis.producer ( selection )
        if alg : alg = iAlgorithm ( alg.name() , alg ) 
        return alg

    
    ## find consumers for the given selection
    #  @see Hlt::IInspector::producer 
    def consumers ( self , selection ) :
        """
        Find the consumers for the given selection

        >>> selection = ...   # get selection (pointer or name)
        >>> consumers = svc.consumers ( selection )
        
        """
        lst = Hlt.IInspector.AlgList()
        if not self._ihltis : self.retrieveInterface() 
        num = self._ihltis.consumers ( selection , lst )
        if lst.size() != num :
            raise AttributeError, 'Unable to extract valid list of algorithms'
        result = []
        for i in range(0,num) :
            alg = lst[i]
            result.append ( iAlgorithm( alg.name() , alg ) )
        return result

    ## find all inputs for the given algorihtm
    #  @see Hlt::IInspector::inputs
    def inputs ( self , algorithm ) :
        """
        Get all input selections for the given algorithm
        
        >>> algorithm = ... # get the algorithm (name or C++/Python stuff)
        >>> inputs = svc.inputs ( algorithm )

        """
        if type(algorithm) == iAlgorithm :
            algorithm.retrieveInterface()
            algorithm = algorithm._ialg
        
        lst = Hlt.IInspector.KeyList()
        if not self._ihltis : self.retrieveInterface() 
        num = self._ihltis.inputs ( algorithm , lst )
        if lst.size() != num :
            raise AttributeError, 'Unable to extract valid list of keys'
        result = []
        for i in range(0,num) :
            key = lst[i]
            result.append ( key )
        return result
        
    ## find all output for the given algorihtm
    #  @see Hlt::IInspector::outputs
    def outputs ( self , algorithm ) :
        """
        Get all Output selections for the given algorithm
        
        >>> algorithm = ... # get the algorithm (name or C++/Python stuff)
        >>> outputs = svc.outputs ( algorithm )
        
        """
        if type(algorithm) == iAlgorithm :
            algorithm.retrieveInterface()
            algorithm = algorithm._ialg
            
        lst = Hlt.IInspector.KeyList()
        if not self._ihltis : self.retrieveInterface() 
        num = self._ihltis.outputs( algorithm , lst )
        if lst.size() != num :
            raise AttributeError, 'Unable to extract valid list of keys'
        result = []
        for i in range(0,num) :
            key = lst[i]
            result.append ( key )
        return result

    ## get the list of readers for the given location
    #  @see Hlt::IInspector::allReaders 
    def allReaders ( self ) :
        """
        Get all registered TES-readers
        
        >>> svc = ...                  ## get the inspection service
        >>> readers = scv.allReaders() ## get the list of all registered readers
        
        
        """
        lst = Hlt.IInspector.AlgList()
        if not self._ihltis : self.retrieveInterface() 
        num = self._ihltis.allReaders ( lst )
        if lst.size() != num :
            raise AttributeError, 'Unable to extract valid list of readers'
        result = []
        for i in range(0,num) :
            alg = lst[i]
            result.append ( iAlgorithm( alg.name() , alg ) )
        return result

    ## get the list of registeres TES input locations 
    #  @see Hlt::IInspector::allTES
    def allTES ( self ) :
        """
        Get all registered TES input locations

        >>> svc = ...           ## get the inspection service
        >>> tes = scv.allTES()  ## get all registered TES input locations
        
        """
        lst = Hlt.IInspector.KeyList()
        if not self._ihltis : self.retrieveInterface() 
        num = self._ihltis.allTES( lst )
        if lst.size() != num :
            raise AttributeError, 'Unable to extract valid list of keys'
        result = []
        for i in range(0,num) :
            key = lst[i]
            result.append ( key )
        return result
    
    ## get the list of input TES location for the given reader
    #  @see Hlt::IInspector::readTES
    def readTES ( self , algorithm ) :
        """
        Get the list of registered input locations for the given reader

        >>> svc = ...                  ## get the inspection service
        >>> alg = ...                  ## get the reader 
        >>> tes = scv.readTES ( alg )  ## get the registered TES input locations
        
        """
        if type(algorithm) == iAlgorithm :
            algorithm.retrieveInterface()
            algorithm = algorithm._ialg

        print 'I am here 1-- '
        lst = Hlt.IInspector.KeyList()
        print 'I am here 1-1 '
        if not self._ihltis : self.retrieveInterface() 
        print 'I am here 1-2 '
        num = self._ihltis.readTES ( algorithm , lst )
        print 'I am here 1-3 '
        print 'I am here 2-- '
        if lst.size() != num :
            raise AttributeError, 'Unable to extract valid list of keys'
        result = []
        print 'I am here 3-- '
        for i in range(0,num) :
            key = lst[i]
            result.append ( key )
        print 'I am here 4-- '
        return result

    ## get the list of readers for the given registred TES input location
    #  @see Hlt:IInspector::readers 
    def readers ( self , location ) :
        """
        Get the list of readers for the given registred TES input location

        >>> svc = ...                   ## get the inspection service
        >>> tes = ...                   ## get the TES input location 
        >>> algs = scv.readers ( tes )  ## get the registered readers
        
        """
        
        lst = Hlt.IInspector.AlgList()
        if not self._ihltis : self.retrieveInterface() 
        num = self._ihltis.readers ( location  , lst )
        if lst.size() != num :
            raise AttributeError, 'Unable to extract valid list of readers'
        result = []
        for i in range(0,num) :
            alg = lst[i]
            result.append ( iAlgorithm( alg.name() , alg ) )
        return result
        
    ## check the existence of the algorithm
    #  @see Hlt::IInspector::hasAlgorithm
    def hasAlgorithm ( self , algorithm ) :
        """
        check the existence of the algorithm
        
        >>> algorithm = ... # get the algorithm (name or C++/Python stuff)
        >>> ok = svc.hasAlgorithm( algorithm )

        """
        if type(algorithm) == iAlgorithm :
            algorithm.retrieveInterface()
            algorithm = algorithm._ialg

        if not self._ihltis : self.retrieveInterface()
        result = self._ihltis.hasAlgorithm ( algorithm )

        return True if 0 != result else False 
 
    
    ## check the existence of producer 
    #  @see Hlt::IInspector::hasProducer
    def hasProducer ( self , algorithm ) :
        """
        check the existence of the producer
        
        >>> algorithm = ... # get the algorithm (name or C++/Python stuff)
        >>> ok = svc.hasProducer( algorithm )

        """
        if type(algorithm) == iAlgorithm :
            algorithm.retrieveInterface()
            algorithm = algorithm._ialg

        if not self._ihltis : self.retrieveInterface()
        result = self._ihltis.hasProducer ( algorithm )

        return True if 0 != result else False 
 

    ## check the existence of consumer 
    #  @see Hlt::IInspector::hasConsumer
    def hasConsumer ( self , algorithm ) :
        """
        check the existence of the consumer 
        
        >>> algorithm = ... # get the algorithm (name or C++/Python stuff)
        >>> ok = svc.hasConsumer( algorithm )

        """
        if type(algorithm) == iAlgorithm :
            algorithm.retrieveInterface()
            algorithm = algorithm._ialg

        if not self._ihltis : self.retrieveInterface()
        result = self._ihltis.hasConsumer ( algorithm )

        return True if 0 != result else False 
 

    ## check the existence of selection
    #  @see Hlt::IInspector::hasSelection
    def hasSelection ( self , selection ) :
        """
        check the existence of the selection
        
        >>> selection = ... # get the selection ( pointer or name)
        >>> ok = svc.hasSelection ( selection )

        """
        if not self._ihltis : self.retrieveInterface()
        result = self._ihltis.hasSelection ( selection )

        return True if 0 != result else False 

    ## get all algorithms
    #  @see Hlt::IInspector::algorithms
    def algorithms ( self ) :
        """
        Get all algorithms form the service:

        >>> algs = svc.algoritms()
        >>> for a in algs :
        ...         print a
        
        """
        if not self._ihltis : self.retrieveInterface()
        lst = Hlt.IInspector.AlgList()
        num = self._ihltis.algorithms ( lst )
        if num != lst.size () :
            raise AttributeError, 'Unable to extrack valid list of algorithms'
        result = []
        for i in range(0,num) :
            alg = lst[i]
            result.append ( iAlgorithm( alg.name() , alg ) )
        return result 

    ## get all selections 
    #  @see Hlt::IInspector::selections 
    def selections ( self ) :
        """
        Get all selections from the service:
        
        >>> keys = svc.selections()
        >>> for k in keys :
        ...         print k
        
        """
        if not self._ihltis : self.retrieveInterface()
        lst = Hlt.IInspector.KeyList()
        num = self._ihltis.selections ( lst )
        if num != lst.size () :
            raise AttributeError, 'Unable to extrack valid list of selections'
        result = []
        for i in range(0,num) :
            key = lst[i]
            result.append ( key )
        return result 
        
        
    ## get the underlying IInspector service
    #  @see Hlt::IInspector 
    def hltInspector ( self ) :
        """
        Get the raw C++ pointer to the service
        
        >>> isvc = svc.htlInspector()
        
        """
        if not self._ihltis : self.retrieveInterface()
        return self._ihltis
    
    ## get the underlying IInspector service
    #  @see Hlt::IData
    def hltData ( self ) :
        """
        Get the raw C++ pointer to the service
        
        >>> isvc = svc.htlData()
        
        """
        if not self._ihltds : self.retrieveInterface()
        return self._ihltds

    ## get the underlying IRegicter service
    #  @see Hlt::IRegister
    def hltRegister ( self ) :
        """
        Get the raw C++ pointer to the service
        
        >>> isvc = svc.htlRegister()
        
        """
        if not self._ihltrs : self.retrieveInterface()
        return self._ihltrs

    ## dump the algorithm 
    def dumpAlg ( self , alg ) :
        """
        """
        if not self._ihltis : self.retrieveInterface()
        
        if not self.hasAlgorithm ( alg ) :
            raise AttributeError, "Unknown algorithm, %s" % alg 

        print 'before inps'
        inps = self.inputs  ( alg )
        print 'before outs'
        outs = self.outputs ( alg ) 
        print 'before tess'
        tess = self.readTES ( alg )
        
        output  = "The algorithm: %s  \n" % alg
        output += "\tINPUT  HLT-Selections : %s \n" % len(inps)
        for i in inps : output += "\t\t%s\n" % i
        output += "\tOUTPUT HLT-Selections : %s \n" % len(outs) 
        for o in outs : output += "\t\t%s\n" % o
        output += "\tREAD   TES-Locations  : %s \n" % len(tess)
        for l in tess : output += "\t\t%s\n" % l

        return output 

    ## "DOT" the algorithm 
    def dotAlg ( self , alg , file_ = None ) :
        """
        """
        if not self._ihltis : self.retrieveInterface()
        
        if not self.hasAlgorithm ( alg ) :
            raise AttributeError, "Unknown algorithm, %s" % alg 

        inps = self.inputs  ( alg )
        outs = self.outputs ( alg )
        tess = self.readTES ( alg )

        if str != type(alg) : alg=alg.name() 

        fname = None
        tmp   = False 
        if not file_ :
            import tempfile
            file_ = tempfile.NamedTemporaryFile( prefix = 'tmp_' , suffix = ".dot" ,
                                                 dir    = '.'    , mode   = 'w'    )
            if not file_ :
                raise AttributeError, "Unable to create temporary file"
            fname = file_.name 
            tmp = True 
        elif str == type(file_) :
            fname = file_ 
            file_ = file( file_ , 'w')
            if not file :
                raise AttributeError, "Unable to open the file %s" % fname
            
        ## else treat the file as already opened file object 

        file_.write ( '## DOT-file for algorithm "%s" \n' % alg  )
        file_.write ( 'digraph HLT_ALgorithm {          \n' )   ## OPENING LINE

        # define all nodes:

        #input selections:
        file_.write ( '## INPUT  SELECTIONS:          \n' )
        for i in inps : 
            file_.write ( selLine ( i )  +           '\n' )
        # the algorithm
        file_.write ( '## ALGORITHM:                  \n' )
        file_.write ( algLine ( alg )  +             '\n' )
        # the output selections 
        file_.write ( '## OUTPUT SELECTIONS:          \n' )
        for o in outs : 
            file_.write ( selLine ( o )  +           '\n' )
        # TES input locations  
        file_.write ( '## INPUT TES-LOCATIONS:        \n' )
        for l in tess : 
            file_.write ( tesLine ( l )  +           '\n' )

        ## links:
        for i in inps :
            file_.write ( arrow ( selNode ( i ) , algNode ( alg ) ) +  "\n" ) 

        ## links:
        for o in outs :
            file_.write ( arrow ( algNode ( alg ) , selNode ( o ) ) +  "\n" ) 

        ## links:
        for l in tess :
            file_.write ( arrow ( tesNode ( l ) , algNode ( alg ) ) +  "\n" ) 
            
        
        file_.write ( '}                              \n' )   ## CLOSING LINE 
        file_.flush ()

        if fname and 'linux2' == sys.platform :
            cmd = ' dot %s -Tfig | display & ' % fname 
            print ' Try to visualize the file %s using command "%s"' %  ( fname , cmd ) 
            os.system ( cmd )
            cmd = ' cat %s ' % fname 
            os.system ( cmd )
            import time
            time.sleep( 10 ) 
            
        if fname and not tmp : return file_ 

    ## "DOT" the selection
    def dotSel ( self , sel , file_ = None ) :
        """
        """
        if not self._ihltis : self.retrieveInterface()
        
        if not self.hasSelection ( sel ) :
            raise AttributeError, "Unknown selection, %s" % sel

        p   = self.producer  ( sel )
        con = self.consumers ( sel ) 

        fname = None
        tmp   = False 
        if not file_ :
            import tempfile
            file_ = tempfile.NamedTemporaryFile( prefix = 'tmp_' , suffix = ".dot" ,
                                                 dir    = '.'    , mode   = 'w'    )
            if not file_ :
                raise AttributeError, "Unable to create temporary file"
            fname = file_.name 
            tmp = True 
        elif str == type(file_) :
            fname = file_ 
            file_ = file( file_ , 'w')
            if not file :
                raise AttributeError, "Unable to open the file %s" % fname
            
        ## else treat the file as already opened file object 

        file_.write ( '## DOT-file for selection "%s" \n' % sel  )
        file_.write ( 'digraph HLT_Selection {          \n' )   ## OPENING LINE

        # define all nodes:

        # producer 
        file_.write ( '## PRODUCER                    \n' )
        file_.write ( algLine ( p.name() )    +      '\n' )

        # the selectiion 
        file_.write ( '## THE SELECTIONS:             \n' )
        file_.write ( selLine ( sel )  +             '\n' )

        file_.write ( '## CONSUMERS SELECTIONS:       \n' )
        for c in con :
            file_.write ( algLine ( c.name() )  +    '\n' )
            

        file_.write ( arrow ( algNode ( p.name() ) , selNode ( sel ) ) +  "\n" ) 

        for c in con : 
            file_.write ( arrow ( selNode ( sel ) , algNode  ( c .name() ) ) +  "\n" ) 
            
        file_.write ( '}                              \n' )   ## CLOSING LINE 
        file_.flush ()

        if fname and 'linux2' == sys.platform :
            cmd = ' dot %s -Tfig | display & ' % fname 
            print ' Try to visualize the file %s using command "%s"' %  ( fname , cmd ) 
            os.system ( cmd )
            cmd = ' cat %s ' % fname 
            os.system ( cmd )
            import time
            time.sleep( 10 ) 
            
        if fname and not tmp : return file_ 


    ## "DOT" all
    def dotAll ( self , file_ = None ) :
        """
        """
        if not self._ihltis : self.retrieveInterface()
        
        sels = self.selections ()
        algs = self.algorithms () 
        tess = self.allTES     () 

        fname = None
        tmp   = False 
        if not file_ :
            import tempfile
            file_ = tempfile.NamedTemporaryFile( prefix = 'tmp_' , suffix = ".dot" ,
                                                 dir    = '.'    , mode   = 'w'    )
            if not file_ :
                raise AttributeError, "Unable to create temporary file"
            fname = file_.name 
            tmp = True 
        elif str == type(file_) :
            fname = file_ 
            file_ = file( file_ , 'w')
            if not file :
                raise AttributeError, "Unable to open the file %s" % fname
            
        ## else treat the file as already opened file object 

        file_.write ( 'digraph HLT_Service {          \n' )   ## OPENING LINE

        # define all nodes:

        import sets
        processed = sets.Set()

        # loop over all algorithms
        for alg in algs :

            # get inputs & outputs
            inps = self.inputs  ( alg )
            outs = self.outputs ( alg ) 
            tess = self.readTES ( alg ) 

            file_.write ( 'subgraph HLT_Algorithm_%s {  \n' % alg.name() )   ## start subgraph 
            
            # input selections
            for i in inps :
                if not i in processed :
                    file_.write ( selLine ( i )  + '\n' )
                    processed.add ( i )
            # the algorithm 
            file_.write ( algLine ( alg.name() )   +      '\n' )

            # output selections
            for o in outs :
                if not o in processed :
                    file_.write ( selLine ( o )  + '\n' )
                    processed.add ( o )
                    
            # TES inputs
            for l in tess :
                if not l in processed :
                    file_.write ( tesLine ( l )  + '\n' )
                    processed.add ( l )
                    
            # lines:
            for i in inps :
                file_.write ( arrow ( selNode ( i ) , algNode ( alg.name() ) ) +  "\n" ) 
            # lines:
            for o in outs :
                file_.write ( arrow ( algNode ( alg.name() ) , selNode ( o ) ) +  "\n" ) 
            # lines:
            for l in tess :
                file_.write ( arrow ( tesNode ( l ) , algNode ( alg.name() ) ) +  "\n" ) 

            file_.write ( '}; \n' )   ## end of subgraph
            
                        
        file_.write ( '}                              \n' )   ## CLOSING LINE 
        file_.flush ()

        if fname and 'linux2' == sys.platform :
            cmd = ' dot %s -Tfig | display & ' % fname 
            print ' Try to visualize the file %s using command "%s"' %  ( fname , cmd ) 
            os.system ( cmd )
            cmd = ' cat %s ' % fname 
            os.system ( cmd )
            import time
            time.sleep( 10 ) 
            
        if fname and not tmp : return file_ 




# =============================================================================
## accessor to the service 
def _iHltInspector_ ( self, name='Hlt::Service' ) :
    """
    Get iHltInspector from Application manager 
    
    >>> gaudi = ...
    >>> svc = gaudi.hltInspector()
    
    """
    svc = Helper.service( self._svcloc, name )
    return iHltInspector ( name , svc )


AppMgr.hltInspector   = _iHltInspector_


# =============================================================================
# The END 
# =============================================================================


