import gaudimodule as gaudi

g = gaudi.AppMgr()
g.loaddict('BenderDict')

def _OpsL ( fun , ops  ) :
    " define le & lt operations "
    fun.  __lt__  = ops.   __lt__
    fun. __rlt__  = ops.  __rlt__
    fun.  __le__  = ops.   __le__
    fun. __rle__  = ops.  __rle__
    
def _OpsG ( fun , ops  ) :
    " define ge & gt operations "
    fun.  __gt__  = ops.   __gt__
    fun. __rgt__  = ops.  __rgt__
    fun.  __ge__  = ops.   __ge__
    fun. __rge__  = ops.  __rge__
    
def _OpsE ( fun , ops  ) :
    " define eq & ne operations "
    fun.  __eq__  = ops.   __eq__
    fun. __req__  = ops.  __req__
    fun.  __ne__  = ops.   __ne__
    fun. __rne__  = ops.  __rne__
    
def _OpsS ( fun , ops ) :
    " define +*-/ operators  "
    fun. __add__  = ops.  __add__
    fun.__radd__  = ops. __radd__
    fun. __sub__  = ops.  __sub__
    fun.__rsub__  = ops. __rsub__
    fun. __mul__  = ops.  __mul__
    fun.__rmul__  = ops. __rmul__
    fun. __div__  = ops.  __div__
    fun.__rdiv__  = ops. __rdiv__
    
def _OpsFun ( fun , ops ) :
    " define operators for functions "
    _OpsL  ( fun , ops )
    _OpsG  ( fun , ops )
    _OpsE  ( fun , ops )
    _OpsS  ( fun , ops )
    
def _OpsCut ( cut , ops ) :
    " define operators for cuts "
    cut.  __and__  = ops.  __and__
    cut. __rand__  = ops. __rand__
    cut.   __or__  = ops.   __or__
    cut.  __ror__  = ops.  __ror__

def _CALL_ ( self , obj ) :
    if not obj : return self.__CALL__(     )
    return              self.__CALL__( obj ) 

def _CallsFun ( fun , calls ) :
    " define call operators "
    fun. __CALL__ =  calls. __fcall__
    fun. __call__ = _CALL_
    if hasattr( fun , 'printOut' ) : fun.__repr__ = fun.printOut 
    
def _CallsCut ( cut , calls ) :
    " define call operators "
    cut. __CALL__ =  calls. __ccall__ 
    cut. __call__ = _CALL_
    if hasattr( cut , 'printOut' ) : cut.__repr__ = cut.printOut 
    
# import operators 
_OpsP      = gaudi.gbl.Bender.Operators    ( 'const Particle*'   )
_OpsV      = gaudi.gbl.Bender.Operators    ( 'const Vertex*'     )
_OpsMCP    = gaudi.gbl.Bender.Operators    ( 'const MCParticle*' )
_OpsMCV    = gaudi.gbl.Bender.Operators    ( 'const MCVertex*'   )
_OpsPID1   = gaudi.gbl.Bender.OperatorsPID ( 'LoKi::Particles::Identifier'      )
_OpsPID2   = gaudi.gbl.Bender.OperatorsPID ( 'LoKi::Particles::AbsIdentifier'   )
_OpsPID3   = gaudi.gbl.Bender.OperatorsPID ( 'LoKi::MCParticles::Identifier'    )
_OpsPID4   = gaudi.gbl.Bender.OperatorsPID ( 'LoKi::MCParticles::AbsIdentifier' )

_CallsP    = gaudi.gbl.Bender.Calls        ( 'const Particle*'   )
_CallsV    = gaudi.gbl.Bender.Calls        ( 'const Vertex*'     )
_CallsMCP  = gaudi.gbl.Bender.CallsMC      ( 'const MCParticle*' )

def _OpsFunP   ( fun ) :
    _OpsFun    ( fun ,   _OpsP   )
    _CallsFun  ( fun ,   _CallsP )
    
def _OpsFunV   ( fun ) :
    _OpsFun    ( fun ,   _OpsV    )
    _CallsFun  ( fun ,   _CallsV  )
    
def _OpsFunMCP ( fun ) :
    _OpsFun    ( fun , _OpsMCP   )
    _CallsFun  ( fun , _CallsMCP )
    
def _OpsFunMCV ( fun ) : _OpsFun ( fun , _OpsMCV )

def _OpsCutP   ( cut ) :
    _OpsCut    ( cut ,   _OpsP )
    _CallsCut  ( cut , _CallsP )
    
def _OpsCutV   ( cut ) :
    _OpsCut    ( cut ,   _OpsV )
    _CallsCut  ( cut , _CallsV )
    
def _OpsCutMCP ( cut ) : _OpsCut ( cut , _OpsMCP )
def _OpsCutMCV ( cut ) : _OpsCut ( cut , _OpsMCV )

def _OpsFunID      ( fun ) : _OpsFunP  ( fun ) ; _OpsE   ( fun , _OpsPID1 )
def _OpsFunABSID   ( fun ) : _OpsFunP  ( fun ) ; _OpsE   ( fun , _OpsPID2 )
def _OpsFunMCID    ( fun ) : _OpsFunMCP( fun ) ; _OpsE   ( fun , _OpsPID3 )
def _OpsFunMCABSID ( fun ) : _OpsFunMCP( fun ) ; _OpsE   ( fun , _OpsPID4 )

def  loadFunP     ( val ) : _OpsFunP     ( val ) ; return val
def  loadFunV     ( val ) : _OpsFunV     ( val ) ; return val
def  loadFunMCP   ( val ) : _OpsFunMCP   ( val ) ; return val
def  loadFunMCV   ( val ) : _OpsFunMCV   ( val ) ; return val
def  loadFunID    ( val ) : _OpsFunID    ( val ) ; return val
def  loadFunABSID ( val ) : _OpsFunABSID ( val ) ; return val

def  loadCutP   ( val ) : _OpsCutP    ( val ) ; return val
def  loadCutV   ( val ) : _OpsCutV    ( val ) ; return val
def  loadCutMCP ( val ) : _OpsCutMCP  ( val ) ; return val
def  loadCutMCV ( val ) : _OpsCutMCV  ( val ) ; return val

def  loadFunMCID    ( val ) : _OpsFunMCID    ( val ) ; return val
def  loadFunMCABSID ( val ) : _OpsFunMCABSID ( val ) ; return val

def loadRange  ( val ) :
    val.at    = None
    val.begin = None
    val.end   = None
    val.front = None
    val.back  = None
    return      val 

def dump( val ) :
    import sys , string 
    mod = sys.modules[ val ].__dict__
    keys = mod.keys()
    keys.sort()
    for i in keys:
        if i[:1] == '_'      : continue 
        s = i
        n = `mod[i]`
        t = '   object'
        if -1 != string.find( n , 'module'   ) : continue 
        if -1 != string.find( n , 'class'    ) : t = '    class'
        if -1 != string.find( n , 'function' ) : t = ' function'
        if '<' == n[:1]  : n = n[1:]
        if '>' == n[-1:] : n = n[:-1]
        pos = string.find( n , 'at 0x')
        if -1 != pos : n = n[:pos]
        n = string.replace( n , "'" , '' )
        n = string.replace( n , 'class ' , '' )
        n = string.replace( n , 'object' , '' )
        while len(s) < 25 : s += ' '
        print ( "  %s  %s  %s " ) % ( s , t , n ) 

def funcTest( fun1 , fun2 ) :
    print ' f1     ' , fun1
    print ' f2     ' , fun2
    print ' f1+f2  ' , fun1 +  fun2 
    print ' f1-f2  ' , fun1 -  fun2 
    print ' f1*f2  ' , fun1 *  fun2 
    print ' f1/f2  ' , fun1 /  fun2 
    print ' f1< f2 ' , fun1 <  fun2 
    print ' f1<=f2 ' , fun1 <= fun2 
    print ' f1> f2 ' , fun1 >  fun2 
    print ' f1>=f2 ' , fun1 >= fun2 
    print ' f1==f2 ' , fun1 == fun2 
    print ' f1!=f2 ' , fun1 != fun2 


def range2list( self ) :
    " conver <XX>Range object to the list"
    res = []
    for obj in self : res += [ obj ]
    print ' sizes ' , self.size() , len(res) 
    return res 
def range2tuple( self ) :
    " conver <XX>Range object to the tuple"
    return tuple( self.range2list() )

def Seq2List( range ) :
    " conver <XX>Range object to the list"
    res = []
    for obj in range : res += [ obj ]
    print ' sizes ' , len(range) , len(res) 
    return res 

def Seq2Tuple( range ) :
    " conver <XX>Range object to the tuple "
    return tuple( Seq2List ( range ) )

