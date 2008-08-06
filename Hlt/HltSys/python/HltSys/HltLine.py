#!/usr/bin/env python
# =============================================================================
# $Id: HltLine.py,v 1.1 2008-08-06 14:38:11 ibelyaev Exp $ 
# =============================================================================
## @file
#  simple module which defines few configurables for HLT lines
#  @author Vanay BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-22
# =============================================================================
"""
The simple module which defines few configurables for HLT line

The module defines three public symbols :

 - class    Hlt1Line   :
      simple class to create the proper 'Configurable' for Hlt1 'lines'
 - class    Hlt1Member :
      helper class to represent the member of Hl1 'line'
 - function htl1Lines  :
      bookeeping routine which keeps the track of all created Hlt1 'lines'
 
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1 $ "
# =============================================================================

__all__ = ( 'Hlt1Line'   ,
            'Hlt1Member' ,
            'hlt1Lines'  )

from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable 

from Configurables import DeterministicPrescaler as Prescaler

from Configurables import HltL0Filter 
#from Configurables import L0Entry

from Configurables import HltTrackUpgrade
from Configurables import HltTrackMatch
from Configurables import HltTrackFilter
from Configurables import HltVertexMaker1
from Configurables import HltVertexMaker2
from Configurables import HltVertexFilter
# from Configurables import HltTrackPrepare
# from Configurables import HltVertexPrepare


def prescalerName  ( line ) : return 'Hlt1%sPrescaler'  % line
def postscalerName ( line ) : return 'Hlt1%sPostscaler' % line
def l0entryName    ( line ) : return 'Hlt1%sL0Filter'   % line
def memberName     ( member, line ) : return 'Hlt1%s%s'%(line,member.subname())


## the list of all created lines 
hlt_1_lines__ = []

# =============================================================================
## Simple function which returns the (tuple) of all currently created Hlt1Lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-08-06
def hlt1Lines () :
    """
    Simple function which returns the (tuple) of all currently created Hlt1Lines
    
    >>> lines = HltLines()
    >>> print lines
    >>> for line in lines : print line
    
    """
    return tuple(hlt_1_lines__)

# =============================================================================
## the list of valid members of Hlt1 sequencer 
_types_ = { HltTrackUpgrade    : 'TU'  
            , HltTrackMatch    : 'TM'  
            , HltTrackFilter   : 'TF'  
            , HltVertexMaker1  : 'VM1' 
            , HltVertexMaker2  : 'VM2'
            , HltVertexFilter  : 'VF'  
            #, HltTrackPrepare  : 'TP'  # not valid anymore 
            #, HltVertexPrepare : 'VP'  # not valid anymore 
            } 

## protected attributes 
_protected_ = ( 'IgnoreFilterPassed' , 'Members' , 'ModeOR' )
## own slots for HltLine 
_myslots_   = ( 'name' , 'prescale'  , 'postscale' , 'L0' , 'algos' ) 

# =============================================================================
## Get the full algorithm type from short nick
def typeFromNick ( nick ) :
    """
    Get the full algorithm type from short nick

    >>> t = typeFromNick ('TU')
    
    """
    for t in _types_ :
        if nick == _types_[t] : return t
    raise AttributeError, " No type is defined for nick '%s'"%nick 

# =============================================================================
## Construct the full selection name
#  The convention is
#     - if the selection name is defined as  'pattern', e.g. '%RZVelo',
#       the full name would include the Hlt line name: 'Hlt1SingleMuonRZVelo'
#     - if the selection name does not incldue 'pattern', it is unchanged
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-08-06
def _selectionName ( name ,      ## the selection name or pattern 
                     line ) :    ## the name of Hlt1 line 
    """
    Construct the full selection name.
    
    The convention is
    
    - if the selection name is defined as  'pattern', e.g. '%RZVelo',
      the full name would include the Hlt line name: 'Hlt1SingleMuonRZVelo'
      
    - if the selection name does not incldue 'pattern', it is unchanged
    
    @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    @date   2008-08-06


    >>> _selectionName ( '%RZVelo' , 'SingleMuon' )   #    pattern 
    'Hlt1SingleMuonRZVelo'
    >>> _selectionName ( 'RZVelo'  , 'SingleMuon' )   # no patetrn 
    'RZVelo'
    
    """
    if not name : return None
    if '%' != name[0] : return name 
    return 'Hlt1' + line + name[1:]


# =============================================================================
## Check the certain selection:
#  - The selection is required to be in the dictionary
#  - If selection has a 'pattern' form , it is expanded properly
#                                 and replaces in the dictionary
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-08-06
def _checkSelection ( sel   ,     ## the selection 
                      margs ,     ## arguments (dictionary)
                      name  ,     ## algorihtm(member) name
                      line  )  :  ## Hlt1 line name 
    """
    Check the certain selection:
    
    - The selection is required to be in the dictionary
    
    - If selection has a 'pattern' form (starts from %) ,
          it is expanded properly and replaced in the dictionary
    
    @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    @date   2008-08-06
    
    """
    val = margs.get( sel ,None)
    if not val  :
        raise AttributeError, "'%s' is not specified for %s" % ( sel , name ) 
    val = _selectionName ( val , line )
    # finally redefine the selection name 
    margs[sel] = val 

# =============================================================================
## Check the selections
#    - It require the presence of all mandatory attributes/properties
#    - The mandatory attributes/properties are expanded (if needed)
#    - It requires the absence of 'forbidden' attributes/properties  
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-08-06
def _checkSelections ( args      ,   # the dictionary with arguments 
                       check     ,   # entries to be checked
                       forbidden ,   # forbidden entries
                       name      ,   # the algorithm name
                       line      ) : # the line ID
    """
    Check the selections:
    
    - It require the presence of all mandatory attributes/properties
    
    - The mandatory attributes/properties are expanded (if needed)
    
    - It requires the absence of 'forbidden' attributes/properties
    
    @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    @date   2008-08-06
    """
    if list is not type(forbidden) and tuple is not type(forbidden) : forbidden =  (forbidden,) 
    for _f in forbidden :
        if _f in args :
            raise ArgumentError, 'The selection "%s" is not allowed for %s' % ( _f , name )
    if list is not type(check)     and tuple is not type(check)     : check     =  (check,) 
    for _c in check : _checkSelection ( _c , args , name , line )
        
    

# =============================================================================
## @class Hlt1Member
#  Simple class to represent the member of Hlt1 line
#  Only the specific algorithm types are allowed to be "members" of Hlt lines
#  Each such algorithm has a short "type"
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-08-06  
class Hlt1Member ( object ) :
    """
    Simple class to represent the member of Hlt1 line
    Only the specific algorithm types are allowed to be 'members' of Hlt lines
    Each such algorithm has a short 'type'
    
    @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    @date   2008-08-06
    
    >>> m1 = Member ( 'TU' , 'Velo'   , RecoName = 'Velo' ) # 'HltTrackUpgrade'
    
    """
    __slots__ = ( 'Type' , 'Name' , 'Args' )
    
    ### The standard constructor to create the  Hlt1Member instance:
    def __init__ ( self      , ## ...
                   Type      , ## short type of members, e.g. 'TF' HltTrackFilter
                   name = '' , ## the specific part of the algorithm name 
                   **Args ) :  ## arguments 
        """
        The standard constructor to create the  Hlt1Member instance:
        
        >>> m1 = Hlt1Member ( 'TU' , 'Velo'   , RecoName = 'Velo' ) ,
        >>> m2 = Hlt1Member ( 'TF' , 'Velo'      ,
                    FilterDescriptor = [ 'IP_PV2D,||>,0.1',
                                         'Calo3DChi2_L0TriggerHadron,<,4' ] ) 
        """
        Name = name
        if type(Type) is str : 
            Type = typeFromNick ( Type ) 
        if not Type in _types_ :
            raise AttributeError, "The type  %s is not known for Hlt1Member"%Type
        self.Type = Type 
        self.Name = Name 
        self.Args = Args
        for key in Args :
            if not key in self.Type.__slots__ :
                raise AttributeError, "The key %s is not allowed"%key

    def subtype( self )        :
        " Return the 'subtype' of the member "
        return _types_[ self.Type ]
    def name   ( self , line ) :
        " Return the full name of the member "
        return memberName ( self , line ) 
    def id     ( self )        :
        " Return the ID of the member "        
        return self.subtype() + self.Name
    def subname( self )        :
        " Return the specific part of the name "        
        return self.id()

# ============================================================================
## @class Hl1Line
#  The major class which represent the Hlt1 Line, the sequence.
#
#  The structure of each line is fixed to be
# 
#    - Prescaler
#    - L0Filter
#       - Member_1
#       - Member_2
#       - ...
#       - Member_N
#    - Decision
#    - Postscaler
# 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-08-06   
class Hlt1Line(object):
    """
    The major class which represent the Hlt1 Line, the sequence.
    @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    @date   2008-08-06   
    """
    ## The constructor, which defines the line
    #
    #  The major arguments
    #    - 'name'      : short name of the line, e.g, 'SingleMuon'
    #    - 'prescale'  : the prescaler factor
    #    - 'L0'        : the list of L0Channels names for HltL0Filter 
    #    - 'algos'     : the list of actual members 
    #    - 'postscale' : the postscale factor
    def __init__ ( self           ,
                   name           ,   # the base name for the Line
                   prescale  = 1  ,   # prescale factor
                   L0        = [] ,   # list of L0 cnannels  
                   postscale = 1  ,   # prescale factor
                   algos     = [] ,   # the list of algorithms/members
                   **args         ) : # other configuration parameters
        """
        The constructor, whcijh essentially defines the line
        
        The major arguments
        - 'name'      : short name of the line, e.g, 'SingleMuon'
        - 'prescale'  : the prescaler factor
        - 'L0'        : the list of L0Channels names for HltL0Filter 
        - 'algos'     : the list of actual members 
        - 'postscale' : the postscale factor
        
        """
        # 1) save all parameters (needed for the proper cloning)
        self._name      = name
        self._prescale  = prescale
        if str is type(L0) : L0 = [L0] 
        self._L0        = L0  
        self._postscale = postscale
        self._algos     = algos
        self._args      = args

        # check for forbidden attributes
        mdict = {} 
        for key in args :
            if key in _protected_ :
                raise AttributeError, "The attribute'%s' is protected for %s"%(key,self.type())
            mdict[key] = args[key] 
        #start to contruct the sequence        
        _members = []
        
        _members += [ Prescaler   ( prescalerName  ( self.subname () ) , AcceptFraction = self._prescale  ) ]
        _members += [ HltL0Filter ( l0entryName    ( self.subname () ) , L0Channels     = self._L0        ) ]
        
        _outputsel = None

        # algos_ = algos.copy()
        algos_ = algos 
        for alg in algos_ :

            if not Hlt1Member is type(alg) :
                _members += [ alg ]
                continue
            else :
                
                margs = alg.Args.copy() 

                ## need input selection?
                if   'InputSelection3' in alg.Type.__slots__ :
                    # check input selections
                    _checkSelections ( margs ,  # arguments
                                       # must be: 
                                       [ 'InputSelection3' , 'InputSelection2' , 'InputSelection1' ] ,
                                       # not allowed:
                                       [ 'InputSelection'  , 'InputSelections' ] ,
                                       alg.name ( self.subname() ) , self.subname() ) ;
                    #
                elif 'InputSelection2' in alg.Type.__slots__ :
                    # check input selections
                    _checkSelections ( margs ,  # arguments
                                       # must be: 
                                       [ 'InputSelection2' , 'InputSelection1' ] ,
                                       # not allowed:
                                       [ 'InputSelection'  , 'InputSelections' , 'InputSelection3' ] ,
                                       alg.name ( self.subname() ) , self.subname() ) ;
                    # 
                elif 'InputSelections' in alg.Type.__slots__ :
                    # check input selections
                    _checkSelections ( margs ,  # arguments
                                       # must be: 
                                       [ 'InputSelections' ] ,
                                       # not allowed:
                                       [ 'InputSelection'  , 'InputSelection1' , 'InputSelection2' , 'InputSelection3' ] ,
                                       alg.name ( self.subname() ) , self.subname() ) ;
                    #
                elif 'InputSelection' in alg.Type.__slots__ :
                    # come manual work 
                    _inputsel = _outputsel
                    if margs.has_key ( 'InputSelection' ) :
                        _checkSelection ( 'InputSelection' , margs , alg.name ( self.subname () ) , self.subname() )
                        _inputsel = margs['InputSelection']
                    if not _inputsel :
                        raise AttributeError, 'Cannot deduce InputSelection neither from argument nor from previous alg'
                    margs['InputSelection'] = _inputsel
                    # here start the machinery check input selections
                    _checkSelections ( margs ,  # arguments
                                       # must be: 
                                       [ 'InputSelection' ] ,
                                       # not allowed:
                                       [ 'InputSelections'  , 'InputSelection1' , 'InputSelection2' , 'InputSelection3' ] ,
                                       alg.name ( self.subname() ) , self.subname() ) ;
                    #
                else :
                    print "%s WARNING: HtlMember('%s') with strange configuration " % ( self.name() , alg.name() )

                
                ## output selection ( the algorithm name)
                if 'OutputSelection' in alg.Type.__slots__ : 
                    _outputsel = alg.name( self.subname() )
                    if margs.has_key ( 'OutputSelection' ) :
                        _checkSelection ( 'OutputSelection' , margs , alg.name() , self.subname() ) 
                        
                # create the algorithm:
                _members += [ alg.Type( memberName ( alg , self.subname() )  , **margs ) ]         
        
        _members += [ Prescaler ( postscalerName ( self.subname () ) , AcceptFraction = self._postscale ) ]
        
        # finally create the sequence
        self._sequencer = GaudiSequencer ( self.name()        ,
                                           Members = _members ,
                                           **mdict            ) ;
        
        # register into the local storage of all created Lines
        #global hlt_1_lines 
        hlt_1_lines__.append ( self ) 

        
    def subname   ( self ) : return            self._name
    def name      ( self ) : return 'Hlt1%s' % self._name 
    def type      ( self ) : return Hlt1Line
    def sequencer ( self ) : return self._sequencer
    
    def ids ( self ) :
        _ids = [] 
        for alg in self._algos :
            if Hlt1Member is type(alg) : _ids += [ alg.id() ] 
        return _ids 
            
    def clone ( self , name , **args ) :

        # classify arguments:
        _own   = {} # own arguments 
        _seq   = {} # arguments for sequencer
        _other = {} # the rest (probably reconfiguration of members)
        for key in args :
            if    key in GaudiSequencer.__slots__ : _seq[keq] = args[key]
            elif  key in  _myslots_               : _own[key] = args[key] 
            else : _other[key] = args[key]

        # Explictly copy all major structural parameters 
        __name       = name
        __prescale   = args.get ( 'prescale'  , self._prescale  ) 
        __postscale  = args.get ( 'postscale' , self._postscale )  
        __L0         = args.get ( 'L0'        , self._L0        )         
        __algos      = args.get ( 'algos'     , self._algos     )      
        __args       = self._args

        # Check the parameters, reponsible for reconfiguration:
        for alg in __algos :
            if not Hlt1Member is type(alg) : continue 
            id = alg.id()
            if id in _other :
                alg.Args.update( _other [id] ) 
                del _other [id]

        # unknonw parameters/arguments 
        if _other :
            raise AttributeError, 'Invalid attributes are detected: %s'%_other 

        # get all "OLD" arguments. and update them with all arguments, understandable by Sequencer 
        __args.update ( _seq   )

        return Hlt1Line ( name      = __name       ,
                          prescale  = __prescale   ,
                          L0        = __L0         ,
                          postscale = __postscale  ,
                          algos     = __algos      , **__args )
    
    
       


## Calculate the effective length of the string
#  which is the length from the last '\n'-symbol 
def len1 ( line ) :
    """
    Calculate the effective length of the line,
    which is thw length from the last '\n'-symbol 
    """
    _i = line.rfind('\n')
    if 0 > _i : return len(line)
    return len(line) - _i

# =============================================================================
## Print the major properties/attributes of the configurables
#  @param obj  the object to be inspected
#  @param lst  list of attributes/properties 
#  @param line the (output) line
#  @paral l1   the indentation parameter
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-08-06
def prnt ( obj , # the object 
           lst , line = '' , l1 = 65 ) :
    """
    Print the major properties/attributes of the configurables
    @param obj  the object to be inspected
    @param lst  list of attributes/properties 
    @param line the (output) line
    @paral l1   the indentation parameter
    @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    @date 2008-08-06

    >>> obj = ...  # get the object/configurable
    >>> print prnt ( obj , [ 'OutputLevel' , 'Members'] ) # print... 
    
    """
    if list is not type(lst) : lst = [ lst ]
    for item in lst :
        if hasattr ( obj , item ) :
            if l1 < len1( line ) : line += '\n' + l1*' '
            line = line + " %-15s : %s" % ( item , getattr ( obj , item ) )
    return line 

# =============================================================================
## The helper function for narcissic self-print of sequences  & algorithms 
#  @param self  the object to be inspected
#  @param level the recursion level
#  @param lst   the list of major properties/attributes 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-08-06
def __enroll__ ( self                             , ## the object
                 level = 0                        , ## the recursion level
                 lst   = [ 'AcceptFraction'    , 
                           'PercertPass'       ,
                           'L0Channels'        ,
                           'InputSelection'    ,
                           'InputSelection1'   ,
                           'InputSelection2'   ,
                           'InputSelection3'   ,
                           'InputSelections'   ,
                           'RecoName'          ,
                           'MatchName'         ,
                           'FilterDescription' ] ) : ## the major properties 
    """
    The helper function for narcissic self-print of sequences  & algorithms 
    @param self  the object to be inspected
    @param level the recursion level
    @param lst   the list of major properties/attributes 
    @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    @date 2008-08-06
    
    """
    if hasattr ( self , 'sequencer' ) :
        return __enroll__ ( self.sequencer() , level )
    
    _indent_ = ('%-3d'%level) + level * '   ' 
    line = _indent_ + self.name ()
    while len(line) < 40 : line = line + ' '
    line = line + '%-25.25s'%self.getType()

    line = prnt ( self , lst , line )
    
    line = line + '\n'

    # use the recursion 
    if hasattr ( self , 'Members' ) :
        _ms = self.Members
        for _m in _ms : line = line + __enroll__ ( _m , level + 1 ) 
    return line


Hlt1Line       . __str__ = __enroll__    
GaudiSequencer . __str__ = __enroll__ 
Sequencer      . __str__ = __enroll__ 

# =============================================================================
## The simple representation of Hlt1Line
def __line_repr__ ( self , *args ) :
    """
    The simple representation of Hlt1Line
    """
    return "Hlt1Line('%s')" % self.name()

Hlt1Line . __repr__ = __line_repr__ 

# =============================================================================
## some embedded action
if '__main__' == __name__ :
    print __doc__
    print __version__
    print __author__
    print "\n\tSymbols:\n", dir ()
    

# =============================================================================
# The END 
# =============================================================================
