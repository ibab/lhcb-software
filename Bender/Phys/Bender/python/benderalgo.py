#!/usr/bin/env python
# =============================================================================
# $Id: benderalgo.py,v 1.3 2004-08-06 12:07:06 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================


# =============================================================================
# @file
#
# defintion of basic Bender algorithm 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import PyLCGDict

# import global namespace 
import gaudimodule  as gaudi

from   bendertypes  import *
from   benderunits  import *
from   benderaux    import *
from   bendercuts   import *
from   benderloop   import Loop
from   bendertuple  import Tuple
from   bendermatch  import MCMatch

# load minimal set of obejcts 
PyLCGDict.loadDict('BenderDict')

# global namespace 
gbl = gaudi.gbl

# load the 'intermediate' obejct 
BenderAlgo = gbl.Bender.BenderAlgoWrap 

# extend the intermeadiate object
class Algo(BenderAlgo):
    """
    The basic algorithm class for easy physics analysis. The core of Bender and LoKi.
    The underlying C++ classes are:
    - Bender::BenderAlgoWrap (Tools/BenderLibs)   $BENDERLIBSROOT/Bender/Algo.h
    - Bender::BenderAlgo     (Tools/BenderLibs)   $BENDERLIBSROOT/Bender/Algo.h
    - LoKi::Algo             (Tools/LoKi)         $LOKIROOT/LoKi/Algo.h
    - GaudiAlgorithm         (GaudiAlg)           $GAUDIALGROOT/GaudiAlg/GaudiAlgorithm.h
    The basic functionality is provided by LoKi::Algo class and described in detail in
    'LoKi User Guide', always available at $LOKIDOC/doc directory  
    """
    def __init__ ( self , name ) :
        """
        Constructor from unique algorithm instance name
        """
        BenderAlgo.__init__( self , self , name )
        appMgr = gaudi.AppMgr()
        algMgr = appMgr._algmgr
        ia = self.asAlgorithm()
        sc     = algMgr.addAlgorithm( ia )
        if sc.isFailure() :
            raise RuntimeError, 'Unable to add Algorithm "'+name+'"'         
        
    def initialize ( self ) :
        """
        The standard initialization includes 2 steps
        - read own properties
        - initialize the base class LoKi::Algo
        """
        status = BenderAlgo.setProperties( self )
        if not status.isSuccess () :
            raise RuntimeError , 'Can not set properties  for ' + self.name()
        # initialize the base class 
        sc = BenderAlgo.initialize_( self )
        if not sc.isSuccess () :
            raise RuntimeError , 'Can not initialize base for ' + self.name()
        self._evtSvc_   = gaudi.iDataSvc      ( BenderAlgo.evtSvc   ( self ) )
        self._detSvc_   = gaudi.iDataSvc      ( BenderAlgo.detSvc   ( self ) )
        self._histoSvc_ = gaudi.iHistogramSvc ( BenderAlgo.histoSvc ( self ) )
        return sc
    
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
    
    def plot         ( self , **args ) :
        """
        Fill/book the histogram. The procedure is described in detail in
        'LoKi User Guide', always available in $LOKIDOC/doc directory
        Usage :
        self.plot( value = pt / GeV , ID = 1 , title = 'My Title' , high = 10 )
        -  'value'  is the value to be put into the histogram
        -  'ID'     is integer histogram identifier  ('semi-optional')
        -  'title'  is historam title                ('semi-optional')
        -  'low'    is low  histogram edge         (optional,default is   0 ) 
        -  'high'   is high histogeram edge        (optional,default is 100 )
        -  'bins'   is number of bins in histogram (optional,default is 100 )
        -  'weight' is weight for the given fill   (optional,default is 1.0 )
        Either 'ID' or 'title' *MUST* be supplied!
        Return value:  AIDA::IHistogram1D object 
        """
        if not args.has_key ( 'value' ) :                    # mandatory argument 
            raise TypeError, " 'value' for the histogram is not specified "
        value  = args.get ( 'value'                        ) # value 
        low    = args.get ( 'low'    ,   0.0               ) # low  limit 
        high   = args.get ( 'high'   , 100.0               ) # high limit 
        bins   = args.get ( 'bins'   , 100                 ) # number of bins
        weight = args.get ( 'weight' ,   1.0               ) # weight
        if args.has_key ( 'ID'    ) :
            ID    = args.get ( 'ID' )
            title = args.get ( 'title' , 'Plot #' + `ID` )
            return BenderAlgo.plot( self , value , ID , title , low , high , bins , weight )
        if args.has_key ( 'title' ) :
            title = args.get('title')
            return BenderAlgo.plot( self , value ,      title , low , high , bins , weight )
        raise TypeError, " Neither 'ID' no 'title' for the histogram are specified "
    
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
    
    def get            ( self , **args ) :
        """
        Helper function to locate objects in Gaudi Event Transient Store
        Usage :
        # 1 
        mcps = self.get( address = 'MC/Particles' )
        # 2 
        mcps = self.get( address = 'MC/Particles' , vector = TRUE )
        # 3 
        mcps = self.get( address = 'MC/Particles' , list   = TRUE )
        where :
        -\t 'address' \t is the objects's addres in Gaudi Event Transient Store
        -\t 'vector'  \t is an indicator to convert the container into std::vector form
        \t\t\t\t This operation could be expensive!
        -\t 'list'    \t is an indicator to convert the container into list        form
        \t\t\t\t This operation could be expensive!
        Return value: the locate dobject in gaudi Trnasient Store
        """
        if   args.has_key( 'address'  ) :
            obj = self._evtSvc_[args.get('address' ) ]
        else :
            raise TypeError, " 'address' is not specified "    
        if args.has_key( 'vector' ) and hasattr( obj , 'containedObjects') :
            return obj.containedObjects()
        if args.has_key( 'list'   ) and hasattr( obj , 'containedObjects') :
            obj = obj.containedObjects()
            lst = []
            for o in obj : lst += [ o ]
            return lst
        return obj
    
    def getDet         ( self , **args ) :
        " helper function to locate objects in Gaudi Detector Transient Store "
        if not args.has_key( "address" ) :
            raise TypeError, "'address' is not specified "
        return self._detSvc_[ args.get('address') ]
    
    def point          ( self , **args ) :
        " Helper function for geometry calculation "
        if args.has_key ( "vertex" ) :
            return BenderAlgo.point( self , args.get ( 'vertex' ) )
        if args.has_key ( "point"  ) :
            return BenderAlgo.point( self , args.get ( 'point' ) )
        return BenderAlgo.geo( self )
    def geo            ( self , **args ) :
        if   args : return self.point ( args )
        else      : return self.point (      ) 
        
    def Error          ( self , **args ) :
        """
        Error printout and statistics
        """
        message = args.get ( 'message' , 'Unspecified Error' )
        code    = args.get ( 'code'    ,  FAILURE            )
        prints  = args.get ( 'prints'  , 10                  ) 
        return BenderAlgo.Error( self  , msg , code , prints )
    
    def Warning        ( self , **args ) :
        """
        Warning printout and statistics
        """
        message = args.get ( 'message' , 'Unspecified Error' )
        code    = args.get ( 'code'    , FAILURE             )
        prints  = args.get ( 'prints'  , 10                  ) 
        return BenderAlgo.Warning( self , msg , code , prints )
    
    def Print        ( self , **args ) :
        """
        Print the message with the given vernosity level 
        """
        message = args.get ( 'message' , ' <* EMPTY MESSAGE *> ' )
        code    = args.get ( 'code'    , SUCCESS                 )
        level   = args.get ( 'level'   , MSG.ALWAYS              )
        if level >= MSG.NUM_LEVEL : level = MSG.ALWAYS
        if level <  MSG.NIL       : level = MSG.ALWAYS  
        return BenderAlgo.Print( self  , message , code , level  )
    
