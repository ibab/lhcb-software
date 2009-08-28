#!/usr/bin/env python

#
# StrippingLine
#

__all__ = ( 
	    'StrippingMember',
	    'StrippingLine', 
	    'strippingLines' 
	  )

import re
from copy import deepcopy 
from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable
from Configurables import DeterministicPrescaler as Scaler
from Configurables import StrippingAlg
from Configurables import LoKi__HDRFilter as HDRFilter
from Configurables import CheckPV

## Convention: the name of 'Filter' algorithm inside StrippingLine
def filterName   ( line , level = 'Stripping') :
    """Convention: the name of 'Filter' algorithm(s) inside StrippingLine"""
    return '%s%sFilterSequence'   % (level,line)

## Convention: the name of 'PreScaler' algorithm inside StrippingLine
def prescalerName  ( line, level = 'Stripping' ) :
    """ Convention: the name of 'PreScaler' algorithm inside StrippingLine """
    return '%s%sPreScaler'  % (level,line)

## Convention: the name of 'PostScaler' algorithm inside StrippingLine
def postscalerName ( line , level = 'Stripping') :
    """ Convention: the name of 'PostScaler' algorithm inside StrippingLine """
    return '%s%sPostScaler' % (level, line)

## Convention: the generic name of 'member' algorithm inside StrippingLine 
def memberName     ( member, line, level='Stripping' ) :
    """ Convention: the generic name of 'member' algorithm inside StrippingLine """
    return '%s%s%s'%(level,line,member.subname())

## Convention: the name of 'HLTFilter' algorithm inside StrippingLine
def hltentryName    ( line, level = 'Stripping' ) :
    """ Convention: the name of 'HLTFilter' algorithm inside StrippingLine """
    return '%s%sHltFilter'   % (level,line)

_stripping_lines__ = []

def strippingLines () :
    return tuple(_stripping_lines__)

def _add_to_stripping_lines_( line ) :
    for i in _stripping_lines__ : 
	if i.name() == line.name() : 
	    raise AttributeError, " StrippingLine  '%s' is already defined!" % line.name()
    _stripping_lines__.append ( line ) 

# Own slots of StrippingLine
_myslots_   = ( 'name' , 'prescale'  , 'postscale' , 'HLT' , 'algos', 'stream' ) 

class bindMembers (object) :
    """
    Simple class to represent a set of StrippingMembers which are bound to a line
    """
    __slots__ = ('_members','_outputsel')

    def outputSelection( self ) : 
        return self._outputsel

    def members( self ) :         
        # remove (downstream) duplicates
        members = []
        for m in self._members :
            if m not in members : members += [ m ]
        return members

    def _getOutputLocation (self, alg) :
        if type(alg) is GaudiSequencer : 
    	    for i in alg.Members : 
    		self._getOutputLocation( i )
        elif hasattr ( type(alg) , 'OutputSelection' ) :
            if hasattr ( alg , 'OutputSelection' ) :
                self._outputsel = alg.OutputSelection 
        elif hasattr ( type(alg) , 'OutputLocation' ) :
            if hasattr ( alg , 'OutputLocation' ) :
                self._outputsel = alg.OutputLocation 
        else :
            self._outputsel = alg.name()

    def _default_handler_( self, line, alg ) :
        # if not known, blindly copy -- not much else we can do
        self._members += [ alg ]
        # try to guess where the output goes...
        self._getOutputLocation(alg)

    # allow chaining of previously bound members...
    def _handle_bindMembers( self, line, alg ) :
        self._members  += alg.members()
        # sometimes, we want to ignore this... 
        # add a flag to allow to skip this (when set to None?)
        if alg.outputSelection() : self._outputsel = alg.outputSelection()
        # self._outputsel = alg.outputSelection()

    def _handle_StrippingMember( self, line, alg ) :
        if line == None: raise AttributeError, 'Must have a line name to bind to'
        alg = alg.createConfigurable( line, **alg.Args )
        return self._default_handler_( line,  alg )

    def __init__( self, line, algos ) :
        self._members = []
        self._outputsel = None
        for alg in algos:
            # dispatch according to the type of alg...
            x = '_handle_' + type(alg).__name__
            handle = getattr(self, x if hasattr(self, x) else '_default_handler_')
            handle(line,alg)




class StrippingTool (object ) :  

    __slots__ = ( 'Type' , 'Name' , 'Args', 'Tools' )

    ### The standard constructor to create an StrippingTool instance:
    def __init__ ( self        ,    ## ...
                   type = None ,    ## the type of the (configurable corresponding to the) tool
                   name = ''   ,    ## the instance name of the tool
                   tools = []  ,    ## tools used by this tool
                   **Args      ) :  ## other arguments 
        if type  == None : raise AttributeError, "Tool must have a type"
        self.Type = type
        self.Name = name if name else type().getType()
        self.Args = Args
        self.Tools = tools
    
    def createConfigurable( self, parent ) :
        parent.addTool( self.Type, name = self.Name  ) 
        instance = getattr( parent, self.Name )
        for k,v in self.Args.iteritems() : setattr(instance,k,v)
        for tool in self.Tools :
                if type(tool) is not StrippingTool : 
                    raise AttributeError, "The type %s is not an StrippingTool"%type(tool)
                tool.createConfigurable( instance )




class StrippingMember ( object ) :

    __slots__ = ( 'Type' , 'Name' , 'Args', 'Tools', 'InputLocations' )
    
    def __init__ ( self       ,    
                   Type       ,   
                   name  = '' ,   
                   InputLocations = None, 
                   tools = [] ,   
                   **Args     ) :

        from Configurables import FilterDesktop, CombineParticles

        if Type not in [ FilterDesktop, CombineParticles ] :
            raise AttributeError, "The type  %s is not known for StrippingMember"%Type
        for key in Args :
            if  key not in Type.__slots__  :
                raise AttributeError, "The key %s is not allowed for type %s"%(key,Type.__name__)

        self.Type  = deepcopy ( Type  )
        self.Name  = deepcopy ( name  )
        self.Args  = deepcopy ( Args  )
        self.Tools = deepcopy ( tools )
        self.InputLocations = deepcopy ( InputLocations )

    def subtype( self )        :
        " Return the 'subtype' of the member "
        return self.Type.__name__

    def name   ( self , line ) :
        " Return the full name of the member "
        return memberName ( self , line, level = 'Stripping' ) 

    def id     ( self )        :
        " Return the ID of the member "        
        return self.subtype() + self.Name

    def subname( self )        :
        " Return the specific part of the name "
#        return self.id()
        return self.Name   # Unlike Hlt case, this will not include type name of the member. 

    def _InputLocations( self, line ) :
        _input = []
        for i in  self.InputLocations :
            if type(i) is bindMembers : i = i.outputSelection() 
            if i[0] == '%' : i = 'Stripping' + line + i[1:]
            _input += [ i ]
        return _input


    def createConfigurable( self, line, **args ) :

        ## clone the arguments
        line = deepcopy ( line )
        args = deepcopy ( args ) 
        _name = self.name( line )
        # see if alg has any special Tool requests...

        if self.InputLocations:
            args['InputLocations'] = self._InputLocations(line)

        instance =  self.Type( _name, **args)

        for tool in self.Tools : tool.createConfigurable( instance )

        return instance


#    def createConfigurable( self, line, **args ) :
#        """
#        Create the configurable, and, if needed, deal with tool configuration
#        """
#        ## clone the arguments
#        line = deepcopy ( line )
#        args = deepcopy ( args ) 
#        if 'InputLocations' in args : 
#            # adapt input...  and put back...  
#            inputLocations = args.pop('InputLocations')
#            # adapt bindMembers...
#            inputLocations = [ i.outputSelection() if type(i) is bindMembers else i 
#                               for i in inputLocations ]
#            # deal with nested lists (one level only), keep order invariant
#            _x = []
#            for i in inputLocations :
#                _x += i if type(i) is list else [ i ]
#            inputLocations = _x
#            # deal with concrete instances
#            from Configurables import FilterDesktop, CombineParticles
#            inputLocations = [ i.getName() if type(i) in [ CombineParticles, FilterDesktop ] else i
#                               for i in inputLocations ]
#            # and perform pattern substitution
#            from re import sub
#            inputLocations = [ sub('^%', 'Stripping' + line, i ) 
#                               for i in inputLocations ]
#            args['InputLocations'] = inputLocations
#        _name = self.name( line )
#        instance =  self.Type( _name, **args)
#        # see if alg has any special Tool requests...
#        for tool in self.Tools : tool.createConfigurable( instance )
#        return instance



class StrippingLine(object):

    def __init__ ( self             ,
                   name             ,   # the base name for the Line
                   stream = None    ,   # stream name
                   prescale  = 1    ,   # prescale factor
                   HLT       = None ,   # HltDecReports predicate
                   checkPV   = True ,   # Check PV before running algos 
                   algos     = []   ,   # the list of algorithms/members
                   postscale = 1    ,   # postscale factor
                   **args           ) : # other configuration parameters

        ## 1) clone all arguments
        name  = deepcopy ( name  )
        HLT   = deepcopy ( HLT   )
        algos = deepcopy ( algos )
        args  = deepcopy ( args  )
        stream = deepcopy ( stream )
        
        # 2) save all parameters (needed for the proper cloning)
        self._name      = name
        if callable(prescale) : prescale = prescale( self.name() )
        self._prescale  = prescale
        
        self._HLT       = HLT
        self._checkPV   = checkPV
        
        if callable(postscale) : postscale = postscale( self.name() )
        self._postscale = postscale
        self._algos     = algos
        self._stream    = stream
        self._args      = args

        # check for forbidden attributes
        mdict = {} 
        for key in args :
            if key in _protected_ :
                raise AttributeError, "The attribute'%s' is protected for %s"%(key,self.type())
            mdict[key] = args[key] 

        #start to contruct the sequence        
        line = self.subname()
        
        # most recent output selection
        self._outputsel = None

        # bind members to line
        _boundMembers = bindMembers( line, algos )
        _members = _boundMembers.members()
        self._outputsel = _boundMembers.outputSelection()
        
        # if needed, check Primary Vertex before running all algos
        if checkPV : 
    	    check = CheckPV("checkPV");
    	    check.MinPVs = 1;
    	    _members.insert(0, check);

        # create the line configurable
        # NOTE: even if pre/postscale = 1, we want the scaler, as we may want to clone configurations
        #       and change them -- and not having the scaler would be problem in that case...
        mdict.update( { 'Prescale'     : Scaler(     prescalerName ( line,'Stripping' ) , AcceptFraction = self._prescale  )
                      , 'Postscale'    : Scaler(    postscalerName ( line,'Stripping' ) , AcceptFraction = self._postscale ) 
                      } )

        if HLT  : mdict.update( { 'HLT'  : HDRFilter  ( hltentryName ( line ) , Code = self._HLT  ) } )
        if _members : 
            mdict.update( { 'Filter' : GaudiSequencer( filterName ( line,'Stripping' ) , Members = _members ) } )
        # final cloning of all parameters:
        __mdict = deepcopy ( mdict ) 

        self._configurable = StrippingAlg ( self.name() , **__mdict )
        print '# created Stripping Line configurable for', name, '\n'
        print self._configurable

        # register into the local storage of all created Lines
        _add_to_stripping_lines_( self ) 

    def subname   ( self ) :
        """ 'Sub-name' of the Stripping line  """ 
        return            self._name
    ## Full name of Stripping line 
    def name      ( self ) :
        """ Full name of Stripping Line """
        return 'Stripping%s' % self._name
    ## the actual type of Stripping Line 
    def type      ( self ) :
        """ The actual type of Hlt Line """
        return StrippingLine

    def stream    ( self ) : 
	return self._stream

    ## Get the underlying 'Configurable'
    #  probably it is the most important method except the constructor
    #
    #  @code 
    #  >>> line = Hlt2Line ( .... )
    #  >>> conf = line.configurable() 
    #  @endcode    
    def configurable ( self ) :
        return self._configurable

    def outputSelection ( self ) :
        """
        Get the name of last outputSelection of the line

        >>> line = ...
        >>> selection = line.outputSelection()
        
        """
        if not self._outputsel :
            raise AttributeError, "The line %s does not define valid outputSelection " % self.subname()
        return self._outputsel

    def clone ( self , name , **args ) :
        """
        Clone the line
        
        A new StrippingLine is created with new name, all property/attrributes maps
        are updated accordingly.
        
        """
        # add some python magic to allow reasonable definition of the deepcopy 
        # of a member function bound to an object instance.
        # see http://bugs.python.org/issue1515 for more info...
        # This should be fixed in python 2.6, so at some point this hack
        # can be removed again
        import copy,types
        origMethod = copy._deepcopy_dispatch[types.MethodType] if types.MethodType in copy._deepcopy_dispatch else None
        def _deepcopy_method(x, memo):
            return type(x)(x.im_func, deepcopy(x.im_self, memo), x.im_class)
        copy._deepcopy_dispatch[types.MethodType] = _deepcopy_method


        ## 1) clone the arguyments
        args = deepcopy ( args )

        ## 2) classify arguments:
        _own   = {} # own arguments 
        _seq   = {} # arguments for sequencer
        _other = {} # the rest (probably reconfiguration of members)
        for key in args :
            if    key in GaudiSequencer.__slots__ : _seq   [keq] = args[key]
            elif  key in  _myslots_               : _own   [key] = args[key] 
            else                                  : _other [key] = args[key]

        # Explictly copy all major structural parameters 
        __name       = deepcopy ( name        )
        __prescale   = deepcopy ( args.get ( 'prescale'  , self._prescale  ) ) 
        __HLT        = deepcopy ( args.get ( 'HLT'       , self._HLT       ) )        
        __checkPV    = deepcopy ( args.get ( 'checkPV'   , self._checkPV   ) )        
        __postscale  = deepcopy ( args.get ( 'postscale' , self._postscale ) ) 
        __algos      = deepcopy ( args.get ( 'algos'     , self._algos     ) )
        __args       = deepcopy ( self._args  ) 
        __stream     = deepcopy ( args.get ( 'stream'    , self._stream    ) )

        # restore the original deepcopy behaviour...
        if origMethod :
            copy._deepcopy_dispatch[types.MethodType] = origMethod 
        else :
            del copy._deepcopy_dispatch[types.MethodType]

        # Check the parameters, reponsible for reconfiguration:
        for alg in [ i for i in __algos if type(i) is StrippingMember ] :
            id = alg.id()
            if id in _other :
                 alg.Args.update( _other [id] ) 
                 del _other [id]

        # unknown parameters/arguments 
        if _other :
            raise AttributeError, 'Invalid attributes are detected: %s'%_other 

        # get all "OLD" arguments
        # and update them with all arguments, understandable by Sequencer 
        __args.update ( _seq   )

        return StrippingLine ( name      = __name       ,
                               prescale  = __prescale   ,
                               HLT       = __HLT        ,
                               checkPV   = __checkPV    ,
                               postscale = __postscale  ,
                               algos     = __algos      , 
                               stream	 = __stream	, 
                               **__args )
