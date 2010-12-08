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
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# 
# =============================================================================
"""
Useful decorator for HLT Service and its interfaces


                    $Revision$
  Last modification $Date$
                 by $Author$
 

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "Version $Revision$"
__date__    = "2009-03-25"
# =============================================================================
__all__ = ( 'iHltInspector', )
# =============================================================================
import GaudiPython.Bindings
import GaudiPython.GaudiAlgs
import os.path,sys

cpp           = GaudiPython.Bindings.gbl
iService      = GaudiPython.Bindings.iService
iAlgorithm    = GaudiPython.Bindings.iAlgorithm
Helper        = GaudiPython.Bindings.Helper
InterfaceCast = GaudiPython.Bindings.InterfaceCast
AppMgr        = GaudiPython.Bindings.AppMgr 
Hlt           = cpp.Hlt

def name    ( s ) :
    for i in ( '/' , ' ' , ':' , '-' , '+' , '=' , '[' , ']') : s = s.replace( i ,'_')
    return s

def selNode ( s ) : return name ( 'SEL_%s' % s ) 
def tesNode ( l ) : return name ( 'TES_%s' % l )  
def algNode ( a ) : return name ( 'ALG_%s' % a ) 

def selLine ( s ) :
    return " \t\t %s [label=<%s>,shape=ellipse,fillcolor=yellow,color=yellow4,style=filled];    \n" % ( selNode ( s ) , s )  
def tesLine ( l ) :
    return " \t\t %s [label=<%s>,shape=ellipse,fillcolor=greenyellow,color=green4,style=filled];\n" % ( tesNode ( l ) , l ) 
def algLine ( a ) :
    return " \t\t %s [label=<%s>,shape=box3d,color=red4,fillcolor=red,style=filled];            \n" % ( algNode ( a ) , a )  

def arrow   ( s1 , s2 ) :
    return ' \t\t\t  %s  \t -> %s [shape=normal]; \n' % ( s1, s2 ) 

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

        lst = Hlt.IInspector.KeyList()
        if not self._ihltis : self.retrieveInterface() 
        num = self._ihltis.readTES ( algorithm , lst )
        if lst.size() != num :
            raise AttributeError, 'Unable to extract valid list of keys'
        result = []
        for i in range(0,num) :
            key = lst[i]
            result.append ( key )
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

    ## get the underlying IRegister service
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

        inps = self.inputs  ( alg )
        outs = self.outputs ( alg ) 
        tess = self.readTES ( alg )
        
        output  = "The algorithm: %s  \n" % alg
        output += "\tINPUT  HLT-Selections : %s \n" % len(inps)
        for i in inps : output += "\t\t%s\n" % i
        output += "\tOUTPUT HLT-Selections : %s \n" % len(outs) 
        for o in outs : output += "\t\t%s\n" % o
        output += "\tREAD   TES-Locations  : %s \n" % len(tess)
        for l in tess : output += "\t\t%s\n" % l


        if isinstance ( alg , str ) :alg = iAlgorithm ( alg )
        if type(alg) == iAlgorithm  :
            alg.retrieveInterface()
            
        cnts = alg.Counters()
        if '#accept' in cnts :
            c = cnts['#accept']
            m = c.mean    () * 100 
            e = c.meanErr () * 100           
            output += "\tACCEPT (%5.2f+-%-5.2f)%% " % ( m , e )
            
        return output 

    # =========================================================================
    ## Open/create DOT-file
    def _dotFile ( self , file = None ) :
        """
        Open/create DOT-file
        """
        tmp   = False        
        if not file :    
            import tempfile
            file = tempfile.NamedTemporaryFile ( prefix = 'tmp_' ,
                                                 suffix = ".dot" ,
                                                 mode   = 'w'    )
            tmp   = True 
        else : file  = open ( file , 'w')
        #
        if not file :
            raise AttributeError, "Unable open/create file"
        #
        return (file,tmp)
    
    # =========================================================================
    ## "DOT" the algorithm 
    #   Produce 'DOT'-graph for the algorithm
    #   @code
    #     >>> alg = ...
    #     >>> htl.dotAlg ( alg )
    #   @endcode 
    def dotAlg ( self , algs , file = None ) :
        """
        Produce 'DOT'-graph for the algorithm(s):

        >>> alg = ...
        >>> htl.dotAlg ( alg )
        
        """
        if not self._ihltis : self.retrieveInterface()

        if not algs : return self.dotAll ( file )

        ## open the file 
        file,tmp = self._dotFile ( file ) 
            
        # define all nodes:
        processed = set()
        
        if not isinstance ( algs , ( tuple , list ) ) : algs = [ algs ] 
        
        file.write ( 'digraph HLT_Algorithms {\n' )            ## OPENING LINE
        
        for alg in algs :
            
            if not self.hasAlgorithm ( alg ) :
                raise AttributeError, "Unknown algorithm, %s" % alg 
            
            inps = self.inputs  ( alg )
            outs = self.outputs ( alg )
            tess = self.readTES ( alg )
            
            if str != type(alg) : alg=alg.name() 

            a = alg 
            file.write ( 'subgraph HLT_Algorithm_%s {  \n' % name ( a )  )   ## start subgraph 
            
            #input selections:
            file.write ( '## INPUT  SELECTIONS:\n' )
            for i in inps :
                if not selNode ( i ) in processed :
                    file.write    ( selLine ( i ) )
                    processed.add ( selNode ( i ) )
                    
            # the algorithm
            if not algNode ( a ) in processed :
                file.write ( '## ALGORITHM:\n' )
                file.write    ( algLine ( a ) )
                processed.add ( algNode ( a ) )
                
            # the output selections 
            file.write ( '## OUTPUT SELECTIONS:\n' )
            for o in outs : 
                if not selNode ( o ) in processed :
                    file.write    ( selLine ( o ) )
                    processed.add ( selNode ( o ) )
                    
            # TES input locations  
            file.write ( '## INPUT TES-LOCATIONS:\n' )
            for l in tess :
                if not tesNode ( l ) in processed :                
                    file.write    ( tesLine ( l ) )
                    processed.add ( tesNode ( i ) )
                    
            ## links:
            for i in inps : file.write ( arrow ( selNode ( i ) , algNode ( a ) ) ) 
            for o in outs : file.write ( arrow ( algNode ( a ) , selNode ( o ) ) ) 
            for l in tess : file.write ( arrow ( tesNode ( l ) , algNode ( a ) ) ) 
            
            file.write ( '};\n' )   ## CLOSING LINE for SUBGRAPH

            
        file.write ( '};\n' )   ## CLOSING LINE 
        file.flush ()
        
        if 'linux2' == sys.platform :
            cmd = ' dot %s -Tfig | display & ' % file.name 
            print ' Try to visualize the file %s using command "%s"' %  ( file.name , cmd ) 
            os.system ( cmd )
            cmd = ' cat %s ' % file.name 
            os.system ( cmd )
            import time
            time.sleep( 10 ) 
            
        if not tmp :
            print 'The DOT-file %s is produced' % file.name
        file.close()

    # =========================================================================
    ## "DOT" the selection
    #   Produce 'DOT'-graph for the selection
    #   @code 
    #     >>> sel = ...
    #     >>> htl.dotSel ( sel )
    #   @endcode 
    def dotSel ( self , sel , file = None ) :
        """
        Produce 'DOT'-graph for the selection

        >>> sel = ...
        >>> htl.dotSel ( sel )
        
        """
        if not sel : return self.dotAll ( file )

        
        if not self._ihltis : self.retrieveInterface()
        
        file, tmp = self._dotFile ( file ) 

        
        file.write ( '## DOT-file for selection "%s" \n' % sels  )
        file.write ( 'digraph HLT_Selections {\n' )   ## OPENING LINE
        
        if not self.hasSelection ( sel ) :
            raise AttributeError, "Unknown selection, %s" % sel
        
        p   = self.producer  ( sel )
        con = self.consumers ( sel ) 
            
        # define all nodes:
        
        # producer
        file.write ( '## PRODUCER\n' )
        file.write ( algLine ( p ) )
                
        # the selectiion itself
        file.write ( '## THE SELECTIONS:\n' )
        file.write ( selLine ( sel ) )

        file.write ( '## CONSUMERS SELECTIONS:\n' )
        for c in con : file.write ( algLine ( c.name() ) )
            
        # links 
        file.write (     arrow ( algNode ( p.name() ) , selNode ( sel       ) ) ) 
        for c in con : 
            file.write ( arrow ( selNode ( sel )      , algNode ( c .name() ) ) ) 
            
        file.write ( '}; \n ' )   ## CLOSING LINE 
        file.flush ()
                
        if 'linux2' == sys.platform :
            cmd = ' dot %s -Tfig | display & ' % file.name
            print ' Try to visualize the file %s using command "%s"' %  ( file.name , cmd ) 
            os.system ( cmd )
            cmd = ' cat %s ' % file.name 
            os.system ( cmd )
            import time
            time.sleep( 10 ) 
            
        if not tmp :
            print 'The DOT-file %s is produced' % file.name()
            
        file.close()
        
        
    # =========================================================================
    ## "DOT" all
    #   Produce 'DOT'-graph for all algorithms and selections:
    #   @code
    #         >>> htl.dotAll ()
    #   @endcode 
    def dotAll ( self , file = None ) :
        """
        Produce 'DOT'-graph for all algorithms and selections:

        >>> htl.dotAll ()
        
        """
        if not self._ihltis : self.retrieveInterface()
        
        sels = self.selections ()
        algs = self.algorithms () 
        tess = self.allTES     () 

        file, tmp = self._dotFile ( file )  

        
        file.write ( 'digraph HLT_Service {\n' )   ## OPENING LINE

        # define all nodes:

        processed = set()

        # loop over all algorithms
        for alg in algs :

            # get inputs & outputs
            inps = self.inputs  ( alg )
            outs = self.outputs ( alg ) 
            tess = self.readTES ( alg ) 

            a = alg.name() 
            file.write ( 'subgraph HLT_Algorithm_%s {  \n' % name ( a )  )   ## start subgraph 
            
            # input selections
            for i in inps :
                if not selNode ( i ) in processed :
                    file.write    ( selLine ( i ) )
                    processed.add ( selNode ( i ) )
                    
            # the algorithm
            if not algNode ( a ) in processed : 
                file.write    ( algLine ( a ) )
                processed.add ( algNode ( a ) )
                
            # output selections
            for o in outs :
                if not selNode ( o ) in processed :
                    file.write    ( selLine ( o ) )
                    processed.add ( selNode ( o ) )
                    
            # TES inputs
            for l in tess :
                if not tesNode ( l ) in processed :
                    file.write    ( tesLine ( l ) )
                    processed.add ( tesNode ( l ) )
                    
            # lines:
            for i in inps : file.write ( arrow ( selNode ( i ) , algNode ( a ) ) ) 
            for o in outs : file.write ( arrow ( algNode ( a ) , selNode ( o ) ) ) 
            for l in tess : file.write ( arrow ( tesNode ( l ) , algNode ( a ) ) ) 

            file.write ( '}; \n' )   ## end of subgraph

        # 
        orphans = [ i for i in sels if not selNode ( i ) in processed ]
        if orphans : 
            file.write ( 'subgraph Orphan_Selections_%s {  \n' % name ( a )  )   ## start subgraph
            for i in orphans : file.write    ( selLine ( i ) )
            file.write ( '};\n' )   ## CLOSING LINE
            print 'Orphan seelctions ', orphans
        # 
        orphans = [ i for i in tess if not tesNode ( i ) in processed ]
        if orphans : 
            file.write ( 'subgraph Orphan_TES_Selections_%s {  \n' % name ( a )  )   ## start subgraph
            for i in orphans : file.write    ( tesLine ( i ) )
            file.write ( '};\n' )   ## CLOSING LINE 
            print 'Orphan TES-selections ', orphans
                        
        file.write ( '};\n' )   ## CLOSING LINE 
        file.flush ()

        if 'linux2' == sys.platform :
            cmd = ' dot %s -Tpng | display & ' % file.name  
            print ' Try to visualize the file %s using command "%s"' %  ( file.name , cmd ) 
            os.system ( cmd )
            cmd = ' cat %s ' % file.name
            os.system ( cmd )
            import time
            time.sleep( 10 ) 
            
        if not tmp :
            print 'The DOT-file %s is produced' % file.name
        file.close()
        



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


