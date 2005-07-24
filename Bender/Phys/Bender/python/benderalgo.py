#!/usr/bin/env python
# =============================================================================
# $Id: benderalgo.py,v 1.16 2005-07-24 17:14:42 ibelyaev Exp $ 
# =============================================================================
# CVS version $Revision: 1.16 $ 
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.14  2005/05/20 10:55:19  ibelyaev
#  prepare for v4r8
#
# Revision 1.13  2005/03/04 19:30:43  ibelyaev
#  v4r7: add possibility to deal with AlgTools
#
# =============================================================================
"""
The definition and implemenation of basic Bender' algorithm:
It is the heart of Bender
"""
__author__ = "Vanya BELYAEV Ivan.Belyaev@itep.ru"

# =============================================================================
# @file
#
# defintion of basic Bender algorithm 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

# import global namespace 
import gaudimodule

from   bendertypes  import *
from   benderunits  import *
from   benderaux    import *
from   bendercuts   import *
from   benderloop   import Loop
from   bendertuple  import Tuple
from   bendermatch  import MCMatch

import bendertools

# load minimal set of obejcts 
gaudimodule.loaddict('BenderDict')

# load the 'intermediate' object
BenderAlgo = gaudimodule.gbl.Bender.BenderAlgoWrap 

# extend the intermeadiate object
class Algo(BenderAlgo):
    """
    The basic algorithm class for easy physics analysis. The core of Bender and LoKi.
    
    The underlying C++ classes are:
    
    - Bender::BenderAlgoWrap (Tools/BenderLibs)   $BENDERLIBSROOT/Bender/Algo.h
    - Bender::BenderAlgo     (Tools/BenderLibs)   $BENDERLIBSROOT/Bender/Algo.h
    - LoKi::Algo             (Tools/LoKi)         $LOKIROOT/LoKi/Algo.h
    - GaudiTupleAlg          (GaudiAlg)           $GAUDIALGROOT/GaudiAlg/GaudiAlgorithm.h
    - GaudiHistoAlg          (GaudiAlg)           $GAUDIALGROOT/GaudiAlg/GaudiAlgorithm.h
    - GaudiAlgorithm         (GaudiAlg)           $GAUDIALGROOT/GaudiAlg/GaudiAlgorithm.h
    - Algorithm              (GaudiKernel)        $GAUDIKERELROOT/GaudiKernel/Algorithm.h
    
    The basic functionality is provided by LoKi::Algo class and described in detail in
    'LoKi User Guide', always available at $LOKIDOC/doc directory
    
    """
    def __init__ ( self , name ) :
        """
        Constructor from the unique algorithm instance name
        """
        BenderAlgo.__init__( self , self , name )
        appMgr = gaudimodule.AppMgr()
        algMgr = appMgr._algmgr
        ia     = self.asAlgorithm()
        sc     = algMgr.addAlgorithm( ia )
        if sc.isFailure() :
            raise RuntimeError, 'Unable to add Algorithm "' + name + '"'         
        
    def initialize ( self ) :
        """
        The standard initialization includes 2 steps
        
        - read own properties
        
        - initialize the base class LoKi::Algo
        
        """
        status = BenderAlgo.setProperties( self )
        if status.isFailure () :
            raise RuntimeError , 'Can not set properties  for ' + self.name()
        # initialize the base class 
        status = BenderAlgo.initialize_( self )
        if status.isFailure () :
            raise RuntimeError , 'Can not initialize base for ' + self.name()

        _e = BenderAlgo.evtSvc( self )
        _s = gaudimodule.Interface(gaudimodule.gbl.IService).cast(_e)
        self._evtSvc_    = gaudimodule.iDataSvc      ( _s.name() , _e )
        
        _d = BenderAlgo.detSvc( self )
        _s = gaudimodule.Interface(gaudimodule.gbl.IService).cast(_d)
        self._detSvc_    = gaudimodule.iDataSvc      ( _s.name() , _d )
        
        _h = BenderAlgo.histoSvc( self )
        _s = gaudimodule.Interface(gaudimodule.gbl.IService).cast(_h)
        self._histoSvc_  = gaudimodule.iHistogramSvc ( _s.name() , _h )
        
        return status
    
    def analyse  ( self ) :
        """
        Default(empty) main 'analyse' method. It needs to be redefined for
        each concrete physics analysis algorithm.
        """
        return BenderAlgo.analyse_( self )
    
    def finalize   ( self ) :
        return BenderAlgo.finalize_( self )
    
    def execute    ( self ) : 
        """
        The default execute method.
        It performs per-event, per-instance LoKi actions with
        the actual call for 'analyse' method.
        """
        return Algo.execute_(self)
    
    def plot         ( self , value , title , **args ) :
        """
        Fill/book the histogram. The procedure is described in detail in
        'LoKi User Guide', always available in $LOKIDOC/doc directory
        
        Usage :
        
        self.plot( value = pt / GeV , ID = 1 , title = 'My Title' , high = 10 )
        
        -  'value'  is the value to be put into the histogram
        -  'title'  is historam title                
        -  'ID'     is integer histogram identifier  ('semi-optional')
        -  'low'    is low  histogram edge         (optional,default is   0 ) 
        -  'high'   is high histogeram edge        (optional,default is 100 )
        -  'bins'   is number of bins in histogram (optional,default is 100 )
        -  'weight' is weight for the given fill   (optional,default is 1.0 )
        
        Return value:  AIDA::IHistogram1D object
        
        """
        low    = args.get ( 'low'    ,   0.0  ) # low  limit 
        high   = args.get ( 'high'   , 100.0  ) # high limit 
        bins   = args.get ( 'bins'   , 100    ) # number of bins
        weight = args.get ( 'weight' ,   1.0  ) # weight
        if args.has_key   ( 'ID' ) :
            ID = args.get ( 'ID' )
            return BenderAlgo.plot( self , value , ID , title , low , high , bins , weight )
        return BenderAlgo.plot( self , value ,      title , low , high , bins , weight )
    
    def select       ( self , **args   ) :
        """
        Select the particles according to the selection cuts
        
        Usage :
        
        kaons = self.select( tag = 'K-'    , cuts = ID == 'K-' )
        fast  = self.select( tag = 'fastK- , cuts = P > 5 * GeV , source = kaons )
        
        where :
        
        -  'tag'    is the tag to be assigned to selected particles (optional,default is 'ALL' )
        -  'cuts'   is the selection function                       (optional,default is PALL  )
        -  'source' is the preselectd collection of particles       (optional)
        
        Return value:  the range of selected particle, associated with the tag
        
        """
        tag    = args.get ( 'tag'    , 'All'  )    # tag to be assigned
        cuts   = args.get ( 'cuts'   ,  PALL  )
        if args.has_key( 'source' ) :
            source = args.get ( 'source' )
            return BenderAlgo.select ( self , tag , source , cuts )
        return     BenderAlgo.select ( self , tag          , cuts )
    
    def vselect      ( self , **args   ) :
        """
        Select the vertices according to the selection cuts
        
        Usage :
        
        prims = self.vselect( tag = 'AllPrimaries' , cuts = VTYPE == 1 )
        goods = self.vselect( tag = 'GoodPVs' , cuts = VTRACKS > 6 , source = prims )
        
        where :
        
        -  'tag'    is the tag to be assigned to selected vertices (optional,default is 'ALL' )
        -  'cuts'   is the selection function                      (optional,default is VALL  )
        -  'source' is the preselected collection of vertices      (optional)
        
        Return value:  the range of selected vertices, associated with the tag
        
        """
        tag    = args.get ( 'tag'    , 'All'  )    # tag to be assigned 
        cuts   = args.get ( 'cuts'   ,  VALL  )
        if args.has_key ( 'source' ) :
            source = args.get ( 'source' )
            return BenderAlgo.vselect ( self , tag , source , cuts )
        return     BenderAlgo.vselect ( self , tag          , cuts )
    
    def mcselect       ( self , **args   ) :
        """
        Select the MC particles according to the selection cuts
        
        Usage :
        
        kaons = self.mcselect( tag = 'K-'    , cuts = MCID == 'K-' )
        fast  = self.mcselect( tag = 'fastK- , cuts = MCP > 5 * GeV , source = kaons )
        
        where :
        
        -  'tag'    is the tag to be assigned to selected MC particles (optional,default is 'MCAll' )
        -  'cuts'   is the selection function                       (optional,default is MCPALL  )
        -  'source' is the preselectd collection of particles       (optional)
        
        Return value:  the range of selected particle, associated with the tag
        
        """
        tag    = args.get ( 'tag'    , 'MCAll'  )    # tag to be assigned
        cuts   = args.get ( 'cuts'   ,  MCPALL  )
        if args.has_key( 'source' ) :
            source = args.get ( 'source' )
            return BenderAlgo.mcselect ( self , tag , source , cuts )
        return     BenderAlgo.mcselect ( self , tag          , cuts )
    
    def mcvselect      ( self , **args   ) :
        """
        Select the MC vertices according to the selection cuts
        
        Usage :
        
        prims = self.mcvselect( tag = 'MCV' , cuts = MCVZ < 1 * cm  )
        goods = self.mcvselect( tag = 'MCV1' , cuts = MCVZ > 6 * cm , source = prims )
        
        where :
        
        -  'tag'    is the tag to be assigned to selected vertices (optional,default is 'MCVAll' )
        -  'cuts'   is the selection function                      (optional,default is MCVALL  )
        -  'source' is the preselected collection of vertices      (optional)
        
        Return value:  the range of selected vertices, associated with the tag
        
        """
        tag    = args.get ( 'tag'    , 'MCVAll'  )    # tag to be assigned 
        cuts   = args.get ( 'cuts'   ,  MCVALL  )
        if args.has_key ( 'source' ) :
            source = args.get ( 'source' )
            return BenderAlgo.mcvselect ( self , tag , source , cuts )
        return     BenderAlgo.mcvselect ( self , tag          , cuts )

    def loop         ( self , **args ) :
        """
        
        Create the looping object from formula,pid and fit strategy
        
        Usage :
        
        D0 = self.loop ( formula = 'K- pi+' , pid = 'D0' , fit = FitVertex )
        
        where :
        
        -  'formula' is the selection formula 
        -  'pid'     is the effective particle ID to be assigned to the coombination
        -  'fit'     is the applied fit policy           (optional, default is FitVertex)
        
        Return value: the loop object
        
        """
        if not args.has_key ( 'formula') :                   # mandatory argument 
            raise TypeError, " Selection 'formula'    is not specified   "
        if not args.has_key ( 'pid'    ) :
            raise TypeError, " Selection 'ParticleID' is not specified   "        
        formula = args.get ( 'formula' )
        pid     = args.get ( 'pid'     )
        fit     = args.get ( 'fit' , FitVertex  )
        return Loop( BenderAlgo.loop( self , formula , pid , fit ) ) 
    
    def nTuple         ( self , **args ) :
        " Retrive/Book the Tuple oobject "
        if args.has_key ( 'ID'    ) :
            _t = BenderAlgo.ntuple ( self , args.get ( 'ID' ) , args.get ( 'title' ,'' ) )
            return Tuple ( _t ) ;
        if args.has_key ( 'title' ) :
            _t = BenderAlgo.ntuple ( self , args.get ( 'title' ) )
            return Tuple ( _t ) ;
        raise TypeError, "Tuple neither 'title' nor 'ID' is not specified"
    
    def evtCol         ( self , **args ) :
        " Retrive/Book the Event Tag Collection  object "
        if args.has_key ( 'ID'    ) :
            _t = BenderAlgo.evtcol ( self , args.get ( 'ID' ) , args.get ( 'title' ,'' ) )
            return Tuple ( _t ) ;
        if args.has_key ( 'title' ) :
            _t = BenderAlgo.evtcol ( self , args.get ( 'title' ) )
            return Tuple ( _t ) ;
        raise TypeError, "Tuple neither 'title' nor 'ID' is not specified"
    
    def evtCollection  ( self , **args ) : return self.evtCol( args )
    
    def mctruth        ( self , **args ) :
        " Retrive/Create MC Truth matching object "
        name = args.get('name','<empty>')
        return MCMatch( BenderAlgo.mctruth( self , name ) )
    
    def evtSvc         ( self ) : return self._evtSvc_
    def detSvc         ( self ) : return self._detSvc_
    def histoSvc       ( self ) : return self._histoSvc_
    
    def get            ( self , address = None , **args ) :
        """
        Helper function to locate objects in Gaudi Event Transient Store
        
        Usage :
        # 1 
        mcps = self.get (            'MC/Particles' )
        # 2 
        mcps = self.get ( address  = 'MC/Particles' )
        # 3 
        mcps = self.get ( location = 'MC/Particles' )
        # 4 
        mcps = self.get ( address  = 'MC/Particles' , vector = TRUE )
        # 5 
        mcps = self.get ( address  = 'MC/Particles' , list   = TRUE )
        # 6 
        mcps = self.get ( location = 'MC/Particles' , vector = TRUE )
        # 7 
        mcps = self.get ( location = 'MC/Particles' , list   = TRUE )
        
        where :
        
        -\t 'address'  \t is the objects's addres in Gaudi Event Transient Store
        -\t 'location' \t is the alias for 'address'
        -\t 'vector'  \t is an indicator to convert the container into std::vector form
        \t\t\t\t This operation could be expensive!
        -\t 'list'    \t is an indicator to convert the container into list        form
        \t\t\t\t This operation could be expensive!
        
        Return value: the located dobject in Gaudi Transient Store
        """
        if not address : address = args.get ( 'location' , None )
        if not address : raise TypeError, " 'address/location' is not specified "
        obj = self._evtSvc_[ address ]
        #
        # relation table? create the proxy and use it! 
        if hasattr( obj , '_rel_table_' ) : return obj._rel_table_()   # RETURN
        #
        if not hasattr( obj , 'containedObjects' ) : return obj        # RETURN 
        if   args.has_key( 'vector' ) :  return obj.containedObjects() # RETURN
        elif args.has_key( 'list'   ) :
            obj = obj.containedObjects()                               # RETURN 
            lst = []
            for o in obj : lst += [ o ]
            return lst                                                 # RETURN
        return obj                                                     # RETURN 
    
    def getDet         ( self , address = None , **args ) :
        """
        Helper function to locate objects in Gaudi Detector Transient Store
        """
        if not address : address = args.get( 'detector' , None )
        if not address : address = args.get( 'location' , None )
        if not address : raise TypeError, " 'address/location' is not specified "
        return self._detSvc_[ address ]                                # RETURN 
    
    def point          ( self , **args ) :
        """
        Helper function for geometry calculation
        (needed for contruction of some  important geometry-related funtions/cuts)
        """
        if args.has_key ( "vertex" ) :
            vertex = args['vertex']
            if not vertex : self.Warinig( message = 'point(): Vertex is None') 
            return BenderAlgo.point( self , vertex )
        if args.has_key ( "point"  ) :
            vertex = args['point']
            if not vertex : self.Warinig( message = 'point(): Vertex is None') 
            return BenderAlgo.point( self , vertex )
        return BenderAlgo.geo( self )

    def geo            ( self , **args ) :
        if   args : return self.point ( args )
        else      : return self.point (      ) 
        
    def Error          ( self                                ,
                         message = '<* Unspecified ERROR *>' ,
                         code    = FAILURE                   ,
                         prints  = 10                        ) :
        """
        Error: printout and statistics

        Usage:

        if a < 0 : self.Error( message = 'something wrong here!' ,prints = 5 )
        
        """
        return BenderAlgo.Error( self  , message  , code , prints )      #RETURN 
    
    def Warning        ( self                                  ,
                         message = '<* Unspecified Warning *>' ,
                         code    = FAILURE                     ,
                         prints  = 10                          ) :
        """
        Warnings: printout and statistics
        
        Usage:
        
        if a < 0 : self.Warning( message = 'something wrong here!' ,prints = 5 )
        
        """
        return BenderAlgo.Warning( self , message , code , prints )  # RETURN 
    
    def Print        ( self                           ,
                       message = '<* EMTY MESSAGE *>' ,
                       level   = MSG.ALWAYS           ,
                       code    = SUCCESS              ) :
        """
        Print the message with the given verbosity level 

        Usage:

        if a < 0 : self.Print( message = ' some information ' , level = MSG.ALWAYS  )
        
        """
        if level >= MSG.NUM_LEVELS : level = MSG.ALWAYS
        if level <= MSG.NIL        : level = MSG.VERBOSE  
        return BenderAlgo.Print( self  , message , code , level  )     # RETURN 

    def tool         ( self             ,
                       interface        ,
                       typename         ,
                       name     = None  ,
                       parent   = None  ,
                       create   = True  ) :
        """
        Useful method to locate the tool a certail tool

        Usage:

        # locate public tool
        t1 = self.tool(ITrExtrapolator,'TrParabolicExtrapolator')
        # locate private tool
        t2 = self.tool(ITrExtrapolator,'TrParabolicExtrapolator',parent=self)
        # locate public tool with defined name 
        t3 = self.tool(ITrExtrapolator,'TrParabolicExtrapolator/MyExt1')
        # locate private tool with defined name 
        t4 = self.tool(ITrExtrapolator,'TrParabolicExtrapolator/MyExt2',parent=self)
        # locate public tool with defined name 
        t5 = self.tool(ITrExtrapolator,'TrParabolicExtrapolator','MyExt3')
        # locate private tool with defined name 
        t6 = self.tool(ITrExtrapolator,'TrParabolicExtrapolator','MyExt4',parent=self)
        
        """
        if not interface : interface = bendertools.IAlgTool 
        if not parent    : parent    = self
        if name          : typename  += '/' + name 
        _tool = self.tool_( typename , parent , create )
        if not _tool : return None
        _tool = bendertools.Interface(interface).cast(_tool)
        if not _tool :
            self.Warning('Invalid cast to interface %s' % interface )
            return None 
        return _tool 
    
# =============================================================================
# The END 
# =============================================================================
