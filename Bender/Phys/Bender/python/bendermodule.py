#!/usr/bin/env python

import PyLCGDict 
import gaudimodule        as gaudi
import benderhelper       as helper

gbl = PyLCGDict.makeNamespace('')

MeV =          1.0
GeV = MeV * 1000.0
TeV = GeV * 1000.0 

mm     =         1.0
cm     = mm *  100.0 
um     = mm / 1000.0
micron = mm / 1000.0

FALSE  = (1>2)
TRUE   = (1<2)

# 'import' Vertex type enum 
class _VertexType(object) :
    """
    $OHYSEVENTROOT/xml/Vertex.xml:
    <enum
    desc   = "Describe how a vertex was constructed"
    name   = "VertexType"
    value  = "Unknown=0, Primary, Decay, DecayWithMass, Interaction, Brem, Pair, Kink"
    access = "PUBLIC"
    />
    """
    __slots__ = ('Primary')
    
VertexType = _VertexType()
VertexType.Primary = 1


        
g = gaudi.AppMgr()
# load the minimal set of dictionaries
g.loaddict('EventDict')
g.loaddict('PhysEventDict')
g.loaddict('BenderDict')

SUCCESS  = gbl.StatusCode( 1 )
FAILURE  = gbl.StatusCode( 0 )

BenderAlgo = gbl.Bender.BenderAlgoWrap 

Range      = helper.loadRange ( gbl.LoKi.Range_( 'std::vector<Particle*> '   ) ) 
VRange     = helper.loadRange ( gbl.LoKi.Range_( 'std::vector<Vertex*> '     ) ) 
MCRange    = helper.loadRange ( gbl.LoKi.Range_( 'std::vector<MCParticle*> ' ) )
MCVRange   = helper.loadRange ( gbl.LoKi.Range_( 'std::vector<MCVertex*> '   ) )

FitStrategy   = gbl.LoKi.Fits.FitStrategy

FitNone       = FitStrategy (   )
FitVertex     = FitStrategy ( 0 )
FitMass       = FitStrategy ( 1 )
FitMassVertex = FitStrategy ( 2 )
FitDirection  = FitStrategy ( 3 )
FitLifeTime   = FitStrategy ( 4 )

# interface functions, load & add the operators!!!
Func     = helper.loadFunP   ( gbl.LoKi.Function  ( 'const Particle*'   ) ) 
VFunc    = helper.loadFunV   ( gbl.LoKi.Function  ( 'const Vertex*'     ) ) 
MCFunc   = helper.loadFunMCP ( gbl.LoKi.Function  ( 'const MCParticle*' ) )
MCVFunc  = helper.loadFunMCV ( gbl.LoKi.Function  ( 'const MCVertex*'   ) ) 

# interface functions, load & add the operators!!!
Cuts     = helper.loadCutP   ( gbl.LoKi.Predicate ( 'const Particle*'   ) )
VCuts    = helper.loadCutV   ( gbl.LoKi.Predicate ( 'const Vertex*'     ) )
MCCuts   = helper.loadCutMCP ( gbl.LoKi.Predicate ( 'const MCParticle*' ) )
MCVCuts  = helper.loadCutMCV ( gbl.LoKi.Predicate ( 'const MCVertex*'   ) )

# load & add the operators 
Fun      = helper.loadFunP   ( gbl.LoKi.FunctionFromFunction   ( 'const Particle*'   ) )
VFun     = helper.loadFunV   ( gbl.LoKi.FunctionFromFunction   ( 'const Vertex*'     ) )
MCFun    = helper.loadFunMCP ( gbl.LoKi.FunctionFromFunction   ( 'const MCParticle*' ) ) 
MCVFun   = helper.loadFunMCV ( gbl.LoKi.FunctionFromFunction   ( 'const MCVertex*'   ) ) 

# load & add the operators 
Cut      = helper.loadCutP   ( gbl.LoKi.PredicateFromPredicate ( 'const Particle*'   ) ) 
VCut     = helper.loadCutV   ( gbl.LoKi.PredicateFromPredicate ( 'const Vertex*'     ) ) 
MCCut    = helper.loadCutMCP ( gbl.LoKi.PredicateFromPredicate ( 'const MCParticle*' ) ) 
MCVCut   = helper.loadCutMCV ( gbl.LoKi.PredicateFromPredicate ( 'const MCVertex*'   ) )

# load & add the operators 
PCONST   = helper.loadFunP   ( gbl.LoKi.Constant ( 'const Particle*'   ) )
VCONST   = helper.loadFunV   ( gbl.LoKi.Constant ( 'const Vertex*'     ) )
MCPCONST = helper.loadFunMCP ( gbl.LoKi.Constant ( 'const MCParticle*' ) ) 
MCVCONST = helper.loadFunMCV ( gbl.LoKi.Constant ( 'const MCVertex*'   ) ) 

# load & add the operators 
PBCONST   = helper.loadCutP   ( gbl.LoKi.BooleanConstant ( 'const Particle*'   ) )
VBCONST   = helper.loadCutP   ( gbl.LoKi.BooleanConstant ( 'const Vertex*'     ) )
MCPBCONST = helper.loadCutMCP ( gbl.LoKi.BooleanConstant ( 'const MCParticle*' ) ) 
MCVBCONST = helper.loadCutMCV ( gbl.LoKi.BooleanConstant ( 'const MCVertex*'   ) )

PALL       = PBCONST   ( 1 )
PTRUE      = PALL
PNONE      = PBCONST   ( 0 )
PFALSE     = PNONE

VALL       = VBCONST   ( 1 )
VTRUE      = VALL
VNONE      = VBCONST   ( 0 )
VFALSE     = VNONE

MCPALL     = MCPBCONST ( 1 )
MCPTRUE    = MCPALL
MCPNONE    = MCPBCONST ( 0 )
MCPFALSE   = MCPNONE

MCVALL     = MCVBCONST ( 1 )
MCVTRUE    = MCVALL
MCVNONE    = MCVBCONST ( 0 )
MCVFALSE   = MCVNONE

# functions 
CL    =  helper.loadFunP  ( gbl.LoKi.Particles.ConfidenceLevel    ) () 
ID    =  helper.loadFunID ( gbl.LoKi.Particles.Identifier         ) () 
ABSID =  helper.loadFunP  ( gbl.LoKi.Particles.AbsIdentifier      ) () 
Q     =  helper.loadFunP  ( gbl.LoKi.Particles.Charge             ) ()      
SUMQ  =  helper.loadFunP  ( gbl.LoKi.Particles.SumCharge          ) ()      
P     =  helper.loadFunP  ( gbl.LoKi.Particles.Momentum           ) ()  
P2    =  helper.loadFunP  ( gbl.LoKi.Particles.Momentum2          ) ()  
PT    =  helper.loadFunP  ( gbl.LoKi.Particles.TransverseMomentum ) ()  
PX    =  helper.loadFunP  ( gbl.LoKi.Particles.MomentumX          ) () 
PY    =  helper.loadFunP  ( gbl.LoKi.Particles.MomentumY          ) ()  
PZ    =  helper.loadFunP  ( gbl.LoKi.Particles.MomentumZ          ) () 
E     =  helper.loadFunP  ( gbl.LoKi.Particles.Energy             ) () 
M     =  helper.loadFunP  ( gbl.LoKi.Particles.Mass               ) () 
MM    =  helper.loadFunP  ( gbl.LoKi.Particles.MeasuredMass       ) ()


# functions 
DMASS  =  helper.loadFunP ( gbl.LoKi.Particles.DeltaMass             ) 
ADMASS =  helper.loadFunP ( gbl.LoKi.Particles.AbsDeltaMass          ) 
DMMASS =  helper.loadFunP ( gbl.LoKi.Particles.DeltaMeasuredMass     )
CHI2M  =  helper.loadFunP ( gbl.LoKi.Particles.DeltaMeasuredMassChi2 ) 

# aliases 
MCHI2    =  CHI2M
MASSCHI2 =  CHI2M
CHI2MASS =  CHI2M

# function
MASS =  helper.loadFunP ( gbl.LoKi.Particles.InvariantMass )
# objects 
M12  =  MASS ( 1 , 2 )
M13  =  MASS ( 1 , 3 )
M14  =  MASS ( 1 , 4 )
M23  =  MASS ( 2 , 3 )
M24  =  MASS ( 2 , 4 )
M34  =  MASS ( 3 , 4 )

#functions 
LV0  = helper.loadFunP( gbl.LoKi.Particles.RestFrameAngle )
# objects 
LV01 =  LV0  ( 1 )
LV02 =  LV0  ( 2 )
LV03 =  LV0  ( 3 )
LV04 =  LV0  ( 4 )

#function
TCHI2NDF = helper.loadFunP ( gbl.LoKi.Particles.TrackChi2PerDoF ) ()

# funstions 
IP       = helper.loadFunP ( gbl.LoKi.Vertices.ImpPar     ) 
IPCHI2   = helper.loadFunP ( gbl.LoKi.Vertices.ImpParChi2 )
# alias 
CHI2IP   = IPCHI2

# functions 
MIP      = helper.loadFunP ( gbl.LoKi.Vertices.MinImpPar     ) 
MIPCHI2  = helper.loadFunP ( gbl.LoKi.Vertices.MinImpParChi2 )
# alias 
CHI2MIP  = MIPCHI2

# funstions 
VD       = helper.loadFunP ( gbl.LoKi.Vertices.VertexDistance     ) 
VDDOT    = helper.loadFunP ( gbl.LoKi.Vertices.VertexDistanceDot  ) 
VDCHI2   = helper.loadFunP ( gbl.LoKi.Vertices.VertexDistanceChi2 )
# alias 
CHI2VD   = VDCHI2

#function 
MVDCHI2  = helper.loadFunP ( gbl.LoKi.Vertices.MinChi2Distance )
#alias 
CHI2MVD  = MVDCHI2

# functions 
VDSIGN   = helper.loadFunP ( gbl.LoKi.Vertices.SignedVertexDistance ) 
VDTIME   = helper.loadFunP ( gbl.LoKi.Vertices.SignedTimeDistance   ) 
VDDTIME  = helper.loadFunP ( gbl.LoKi.Vertices.DotTimeDistance      ) 
DTR      = helper.loadFunP ( gbl.LoKi.Vertices.ClosestApproach      ) 
DTRCHI2  = helper.loadFunP ( gbl.LoKi.Vertices.ClosestApproachChi2  )

#aliases
CHI2DTR  = DTRCHI2

#function 
DIRA     = helper.loadFunP ( gbl.LoKi.Vertices.DirectionAngle )
#alias 
DDANG    = DIRA

# function
PID      = helper.loadFunP ( gbl.LoKi.Particles.ParticleIdEstimator )
# objects 
#   PIDe
#   PIDmu
#   PIDpi
#   PIDK
#   PIDp

# functions 
VIP     = helper.loadFunV ( gbl.LoKi.Vertices.VertexImpPar     ) 
VIPCHI2 = helper.loadFunV ( gbl.LoKi.Vertices.VertexImpParChi2 )
# alias 
CHI2IP  = VIPCHI2

#functions 
VPSD    = helper.loadFunV ( gbl.LoKi.Vertices.VertexParticleSignedDistance )

#object 
VCHI2   = helper.loadFunV ( gbl.LoKi.Vertices.VertexChi2   )  ()
#alias 
CHI2V   = VCHI2
#object 
VTYPE   = helper.loadFunV ( gbl.LoKi.Vertices.VertexType    ) () 
VDOF    = helper.loadFunV ( gbl.LoKi.Vertices.VertexDoF     ) () 
VX      = helper.loadFunV ( gbl.LoKi.Vertices.VertexX       ) () 
VY      = helper.loadFunV ( gbl.LoKi.Vertices.VertexY       ) () 
VZ      = helper.loadFunV ( gbl.LoKi.Vertices.VertexZ       ) () 
VPRONGS = helper.loadFunV ( gbl.LoKi.Vertices.VertexProngs  ) () 
VTRACKS = helper.loadFunV ( gbl.LoKi.Vertices.VertexTracks  ) () 

# functions
CHILD   = helper.loadFunP ( gbl.LoKi.Particles.ChildFunction ) 
MCTRUTH = helper.loadCutP ( gbl.LoKi.Particles.MCTruth       ) 

# mc functions
MCP    = helper.loadFunMCP ( gbl.LoKi.MCParticles.Momentum           ) ()
MCE    = helper.loadFunMCP ( gbl.LoKi.MCParticles.Energy             ) ()
MCPT   = helper.loadFunMCP ( gbl.LoKi.MCParticles.TransverseMomentum ) ()
MCPX   = helper.loadFunMCP ( gbl.LoKi.MCParticles.MomentumX          ) ()
MCPY   = helper.loadFunMCP ( gbl.LoKi.MCParticles.MomentumY          ) ()
MCPZ   = helper.loadFunMCP ( gbl.LoKi.MCParticles.MomentumZ          ) ()
MCID   = helper.loadFunMCP ( gbl.LoKi.MCParticles.Identifier         ) ()
MC3Q   = helper.loadFunMCP ( gbl.LoKi.MCParticles.ThreeCharge        ) ()
MCCTAU = helper.loadFunMCP ( gbl.LoKi.MCParticles.ProperLifeTime     ) ()


# "intermediate" objects 
BenderLoop    = gbl.Bender.Loop
BenderTuple   = gbl.Bender.BenderTuple
BenderMCMatch = gbl.Bender.MCMatch
BenderAlgo    = gbl.Bender.BenderAlgoWrap


class Tuple(BenderTuple):
    """
    The basic object for easy manipulation with N-Tuples
    The underlying C++ classes are:
    - Bender::BenderTuple   (Tools/BenderLibs)   $BENDERLIBSROOT/Bender/Tuple.h
    - LoKi::Tuple           (Tools/LoKi)         $LOKIROOT/LoKi/Tuple.h
    - LoKi::TupleObj        (Tools/LoKi)         $LOKIROOT/LoKi/TupleObj.h
    - NTuple::Tuple         (GaudiKernel)        $GAUDIKERNELROOT/GaudiKernel/NTuple.h
    - INTuple               (GaudiKernel)        $GAUDIKERNELROOT/GaudiKernel/INTuple.h
    """
    def __init__( self ,  obj ) :
        """
        Constructor from BenderTuple object
        """
        BenderTuple.__init__ ( self , obj )
        
    def column  ( self , **args ) : 
        """
        Fill single N-Tuple column. The column can be primitive or specialized.
        Usage :
        tuple.column( name = 'pt' , value = pt / GeV )
        where :
        -  tuple    is the instance of Tuple object
        - 'name'    is column name 
        - 'value'   is column value
        - 'min'     is minimal value for integer column  (optional)
        - 'max'     is maximal value for integer columnt (optional)
        Currently supported single column types are
        - float (32bits)
        - long  (32bits)
        - bool    
        - Hep3Vector        (CLHEP)       CLHEP/Vector/ThreeVector.h 
        - HepPoint3D        (CLHEP)       CLHEP/Geometry/Point3D.h 
        - HepVector3D       (CLHEP)       CLHEP/Geometry/Vector3D.h 
        - HepLorentzVector  (CLHEP)       CLHEP/Vector/LorentzVector.h
        - EventHeader       (Event/Event) $EVENTROOT/Event/EventHeader.h
        """
        if not args.has_key('name') :
            raise TypeError, " Column 'name'  is not specified "
        if not args.has_key('value') :
            raise TypeError, " Column 'value' is not specified "
        name  = args.get ( 'name'  )
        value = args.get ( 'value' )
        if args.has_key('min') | args.has_key( 'max' ) :
            # "size-like" column 
            minv   = args.get( 'min' ,   0 )
            maxv   = args.get( 'max' , 500 ) 
            return BenderTuple.column( self , name , value , minv , maxv )
        # ordinary-type column
        return BenderTuple.column( self , name , value )
    def write  ( self ) :
        """
        Commit the filled row into N-Tuple
        """
        return BenderTuple.write( self ) ;
    def commit ( self ) :
        """
        Commit the filled row into N-Tuple
        """
        return BenderTuple.write( self ) ;
    
class MCMatch(BenderMCMatch):
    """
    The basic object for easy manipulation with Monte Carlo Truth
    For detailes see 'LoKi User Guide' available at $LOKIDOC/doc directory
    The underlying C++ classes are:
    - Bender::MCMatch    (Tools/BenderLibs)   $BENDERLIBSROOT/Bender/MCMatch.h
    - LoKi::MCMatch      (Tools/LoKi)         $LOKIROOT/LoKi/MCMatch.h
    - LoKi::MCMatchObj   (Tools/LoKi)         $LOKIROOT/LoKi/MCMatchObj.h
    see also
    - IMCDecayFinder  (Phys/DaVinciMCTools)
    and especially $DAVINCIMCTOOLSROOT/src/MCDecayFinder.h
    """
    def __init__ ( self , obj ) :
        """
        Constructor from BenderMCMatch object
        """
        BenderMCMatch.__init__( self , obj ) 

    def __call__ ( self  , reco , mc  ) :
        """
        Match the reconstructed and Monte Carlo objects. ('Callable' interface)
        Usage:
        matched = matcher( reco , mc ) 
        where :
        - 'matcher' is the instance of MCMatch object 
        - 'reco'    is either reconstructed particle or range of reconstructed particle
        - 'mc'      is either Monte Carlo particle or range of Monte Carlo particles
        The Monte Calo matching procedure is described in 'LoKi User Guide'
        """
        return BenderMCMatch.match( self , reco , mc )
    
    def match    ( self , **args      ) :
        """
        Match the reconstructed and Monte Carlo objects . ('Python'-like interface)
        Usage :
        matched = matcher.match( reco = particle  , mc = mcparticle ) 
        where :
        -  matcher  is the instance of MCMatch object 
        - 'reco'    is either reconstructed particle or range of reconstructed particle
        - 'mc'      is either Monte Carlo particle or range of Monte Carlo particles
        The Monte Calo matching procedure is described in 'LoKi User Guide'
        """
        if not args.has_key ( 'reco' ) :
            raise TypeError, "  'reco'  is not specified "
        if not args.has_key ( 'mc'   ) :
            raise TypeError, "  'mc'    is not specified "
        reco = args.get ( 'reco' )
        mc   = args.get ( 'mc'   )
        return BenderMCMatch.match( self , reco , mc )
    
    def find     ( self , **args ) :
        """
        Find Monte Carlo decays using teh descriptor
        Usage :
        decays = mc.find( decay = 'D0 -> K- pi+' , address = address )
        where :
        -  mc       is the instance of MCMatch object 
        - 'decay'   is the decay descriptor 
        - 'address' is address of Monte Carlo Particles in Gaudi TES  (optional)
        Internally the nice tool MCDecayFinder by Olivier Dormond is used. 
        The decay descriptor semantics is described in detail in
        $DAVINCIMCTOOLSROOT/src/MCDecayFinder.h
        """
        if not args.has_key ( 'decay'   ) :
            raise TypeError, "  'decay'  is not specified "
        decay   = args.get ( 'decay')
        if not args.has_key ( 'address' ) :
            return BenderMCMatch.find( self , decay )
        address = args.get ( 'address'  )
        return BenderMCMatch.find( self , decay , address )
    
class Loop(BenderLoop):
    """
    The basic object for looping over particle combinatios
    Usage:
    for D0 in loop( formula = 'K- pi+' , pid = 'D0' ) :
    \tp=D0.particle()
    \tprint ' D0 mass ' , M( p ) 
    """
    def __init__ ( self , loop ) :
        """
        Constructor from C++ Bender::Loop class
        """
        BenderLoop.__init__( self , loop )
        
    def save     ( self , **args ) :
        """
        Save the particle ans assosiate it with a certain tag
        Usage:
        for D0 in loop( formula = 'K- pi+' , pid = 'D0' ) :
        \t...
        \tD0.save( tag = 'D0' )
        """
        if not args.has_key('tag') :
            raise TypeError, "  'tag'  is not specified "
        return BenderLoop.save( self , args.get('tag') )

    def fit       ( self , **args ) :
        """
        Perforn kinematical/topological constraint according the fitting strategy
        Usage:
        for D0 in loop( formula = 'K- pi+' , pid = 'D0' ) :
        \t...
        \tD0.fit( strategy = FitMassVertex ) 
        """
        if args.has_key ( 'stragegy' ) :
            return BenderLoop.fit( self , args.get ( 'strategy' ) )
        if args.has_key ( 'policy'   ) :
            return BenderLoop.fit( self , args.get ( 'policy'   ) )
        if args.has_key ( 'fit'      ) :
            return BenderLoop.fit( self , args.get ( 'fit'      ) )
        raise TypeError, "  Neither 'strategy' nor 'policy' are specified "

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
         sc     = algMgr.addAlgorithm( self.asAlgorithm() )
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
             return BenderAlgo.plot( self , ID , title , value , low , high , bins , weight )
         if args.has_key ( 'title' ) :
             title = args.get('title')
             return BenderAlgo.plot( self ,      title , value , low , high , bins , weight )
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
         if not args.has_key( 'title' ) :
             raise TypeError, "Tuple 'title' is not specified"
         return Tuple( BenderAlgo.ntuple( self , args.get('title') ) )
     
     def ntuple         ( self , **args ) :
         " Retrive/Book the Tuple oobject "
         if not args.has_key( 'title' ) :
             raise TypeError, "Tuple 'title' is not specified"
         return Tuple( BenderAlgo.ntuple( self , args.get('title') ) )
          
     def evtCol         ( self , **args ) :
         " Retrive/Book the Event Tag Collection  object "
         if not args.has_key( 'title' ) :
             raise TypeError, "Tuple 'title' is not specified"
         return Tuple( BenderAlgo.evtCol( self , args.get('title') ) ) 
     
     def evtCollection  ( self , **args ) : return self.evtCol( args )
     
     def mctruth        ( self , **args ) :
         " Retrive/Create MC Truth matching object "
         name = args.get('name','<empty>')
         return MCMatch( BenderAlgo.mctruth( self , name ) )
     
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
         " Error printout and statistics "
         message = args.get ( 'msg'    , 'Unspecified Error' )
         code    = args.get ( 'code'   ,  FAILURE            )
         prints  = args.get ( 'prints' , 10                  ) 
         return BenderAlgo.Error( self , msg , code , prints )
     
     def Warning        ( self , **args ) :
         " Warning printout and statistics "
         message = args.get ( 'msg'    , 'Unspecified Error' )
         code    = args.get ( 'code'   , FAILURE             )
         prints  = args.get ( 'prints' , 10                  ) 
         return BenderAlgo.Warning( self , msg , code , prints )

# import useful functions
vertex                 = gbl.Bender.Accessors.vertex
particle               = gbl.Bender.Accessors.particle
child                  = gbl.Bender.Child.child
select_min             = gbl.Bender.Select.select_min
select_max             = gbl.Bender.Select.select_max
SelectPrimaryVertexMin = gbl.Bender.Select.SelectPrimaryVertexMin
SelectPrimaryVertexMax = gbl.Bender.Select.SelectPrimaryVertexMax

# if __name__ == '__main__': helper.dump( __name__ )
