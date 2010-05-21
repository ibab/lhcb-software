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
from GaudiConf.Configuration import *
from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable
from Configurables import DeterministicPrescaler as Scaler
from Configurables import LoKi__L0Filter    as L0Filter
from Configurables import LoKi__HDRFilter   as HDRFilter
from Configurables import LoKi__ODINFilter  as ODINFilter
from SelPy.selection import FlatSelectionListBuilder
#from Configurables import HltCopySelection_LHCb__Particle_ as HltCopyParticleSelection

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

## Convention: the name of 'ODINFilter' algorithm inside StrippingLine
def odinentryName    ( line, level = 'Stripping' ) :
    """ Convention: the name of 'ODINFilter' algorithm inside StrippingLine """
    return '%s%sODINFilter'   % (level,line)

## Convention: the name of 'L0DUFilter' algorithm inside StrippingLine
def l0entryName    ( line, level = 'StrippingLine' ) :
    """ Convention: the name of 'L0DUFilter' algorithm inside StrippingLine """
    return '%s%sL0DUFilter'   % (level,line)

## Convention: the name of 'HLTFilter' algorithm inside StrippingLine
def hltentryName    ( line, level = 'Stripping' ) :
    """ Convention: the name of 'HLTFilter' algorithm inside StrippingLine """
    return '%s%sHltFilter'   % (level,line)

def decisionName   ( line, level = 'Hlt1' ) :
    """Convention: the name of 'Decision' algorithm inside StrippingLine"""
    return level + '%sDecision'   % line if line != 'Global' else level+'Global'

_protected_ = ( 'IgnoreFilterPassed' , 'Members' , 'ModeOR', 'DecisionName', 'Prescale','Postscale','Filter1' )

# Own slots of StrippingLine
_myslots_   = ( 'name' , 'prescale'  , 'postscale' , 'ODIN', 'L0DU', 'HLT' , 'algos' ) 


_stripping_lines__ = []

# =============================================================================
## Add the created line into the local storage of created Hlt1Lines 
def _add_to_stripping_lines_( line ) :
    """
    Add the line into the local storage of created Hlt1Lines 
    """

    for i in _stripping_lines__ : 
	if i.name() == line.name() : 
	    raise ValueError,"Created StrippingLine with duplicate name %s" % line.name()
    
    _stripping_lines__.append ( line ) 

    
def strippingLines () :
    return tuple(_stripping_lines__)


class bindMembers (object) :
    """
    Simple class to represent a set of StrippingMembers which are bound to a line
    """
    __slots__ = ('_members','_outputsel', '_outputloc')

    def outputSelection( self ) : 
        return self._outputsel

    def outputLocation( self ) : 
        return self._outputloc


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
                self._outputloc = "Phys/"+alg.OutputSelection
        elif hasattr ( type(alg) , 'OutputLocation' ) :
            if hasattr ( alg , 'OutputLocation' ) :
                self._outputloc = alg.OutputLocation 
        else :
#            self._outputsel = None
#            self._outputloc = None
            self._outputsel = alg.name()
            self._outputloc = "Phys/"+alg.name()

    def _default_handler_( self, line, alg ) :
        # if not known, blindly copy -- not much else we can do
        self._members += [ alg ]
        # try to guess where the output goes...
        self._getOutputLocation(alg)

    def _handle_SelectionSequence(self, line, alg) :
        gaudiSeq = alg.sequence()
        members = gaudiSeq.Members
        for a in members :
            self._members += [a]
        loc = alg.outputLocations()[0]
        self._outputsel = loc
        self._outputloc = loc

    def _handle_Selection(self, line, alg) :
        members = FlatSelectionListBuilder(alg).selectionList
        for a in members :
            self._members += [a]
        loc = alg.outputLocation()
        self._outputsel = loc
        self._outputloc = loc

    def _handle_SelSequence(self, line, alg) :
        gaudiSeq = alg.sequence()
        members = gaudiSeq.Members
        for a in members :
            self._members += [a]
        loc = alg.outputLocations()[0]
        self._outputsel = loc
        self._outputloc = loc

    # allow chaining of previously bound members...
    def _handle_bindMembers( self, line, alg ) :
        self._members  += alg.members()
        # sometimes, we want to ignore this... 
        # add a flag to allow to skip this (when set to None?)
        if alg.outputSelection() : self._outputsel = alg.outputSelection()
        if alg.outputLocation() : self._outputloc = alg.outputLocation()

    def _handle_StrippingMember( self, line, alg ) :
        if line == None: raise AttributeError, 'Must have a line name to bind to'
        alg = alg.createConfigurable( line, **alg.Args )
        return self._default_handler_( line,  alg )

    def __init__( self, line, algos ) :
        self._members = []
        self._outputsel = None
        self._outputloc = None
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
#                   InputLocations = None, 
                   tools = [] ,   
                   **Args     ) :

        from Configurables import FilterDesktop, CombineParticles, ConjugateNeutralPID 

        if Type not in [ FilterDesktop, CombineParticles, ConjugateNeutralPID ] :
            raise AttributeError, "The type  %s is not known for StrippingMember"%Type
        for key in Args :
            if  key not in Type.__slots__  :
                raise AttributeError, "The key %s is not allowed for type %s"%(key,Type.__name__)

        self.Type  = deepcopy ( Type  )
        self.Name  = deepcopy ( name  )
        self.Args  = deepcopy ( Args  )
        self.Tools = deepcopy ( tools )
#        self.InputLocations = deepcopy ( InputLocations )

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
        for i in  self.Args['InputLocations'] :
            if type(i) is bindMembers : i = i.outputLocation() 
            if i : 
        	if i[0] == '%' : i = 'Stripping' + line + i[1:]
        	_input += [ i ]
        return _input


    def createConfigurable( self, line, **args ) :

        ## clone the arguments
        line = deepcopy ( line )
        args = deepcopy ( args ) 
        _name = self.name( line )
        # see if alg has any special Tool requests...

        if 'InputLocations' in self.Args :
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
                   prescale  = 1    ,   # prescale factor
                   ODIN      = None ,   # ODIN predicate
                   L0DU      = None ,   # L0DU predicate
                   HLT       = None ,   # HltDecReports predicate
                   checkPV   = True ,   # Check PV before running algos 
                   algos     = []   ,   # the list of algorithms/members
                   postscale = 1    ,   # postscale factor
                   **args           ) : # other configuration parameters

        ## 1) clone all arguments
        name  = deepcopy ( name  )
        ODIN  = deepcopy ( ODIN  )
        L0DU  = deepcopy ( L0DU  )
        HLT   = deepcopy ( HLT   )
        algos = deepcopy ( algos )
        args  = deepcopy ( args  )
        # 2) save all parameters (needed for the proper cloning)
        self._name      = name
        if callable(prescale) : prescale = prescale( self.name() )
        self._prescale  = prescale
        
        self._ODIN      = ODIN
        self._L0DU      = L0DU
        self._HLT       = HLT
        self._checkPV   = checkPV
        
        if callable(postscale) : postscale = postscale( self.name() )
        self._postscale = postscale
        self._algos     = algos
        self._args      = args

        line = self.subname()

        # most recent output selection
        self._outputsel = None
        self._outputloc = None
        # bind members to line
        _boundMembers = bindMembers( line, algos )
        _members = _boundMembers.members()
        self._outputsel = _boundMembers.outputSelection()
        self._outputloc = _boundMembers.outputLocation()
        
        self._appended = False

        # register into the local storage of all created Lines
        _add_to_stripping_lines_( self ) 
        
    def declareAppended( self ) :
	self._appended = True 
	
    def isAppended( self ) : 
	return self._appended

    def createConfigurable( self, HDRLocation = 'Strip/Phys/DecReports' ) : 
        # check for forbidden attributes
        
        name    = self._name
        args    = self._args
        algos   = self._algos
        checkPV = self._checkPV
        ODIN    = self._ODIN
        L0DU    = self._L0DU
        HLT     = self._HLT

        mdict = {} 
        for key in args :
            if key in _protected_ :
                raise AttributeError, "The attribute'%s' is protected for %s"%(key,self.type())
            mdict[key] = args[key] 

        #start to contruct the sequence        
        line = self.subname()
        
        _boundMembers = bindMembers( line, algos )
        _members = _boundMembers.members()

        # if needed, check Primary Vertex before running all algos
        if checkPV :
            from Configurables import CheckPV
    	    check = CheckPV("checkPV");
    	    check.MinPVs = 1;
    	    _members.insert(0, check);

        # create the line configurable
        # NOTE: even if pre/postscale = 1, we want the scaler, as we may want to clone configurations
        #       and change them -- and not having the scaler would be problem in that case...
        mdict.update( { 'Prescale'     : Scaler(     prescalerName ( line,'Stripping' ) , AcceptFraction = self._prescale  )
                      , 'Postscale'    : Scaler(    postscalerName ( line,'Stripping' ) , AcceptFraction = self._postscale ) 
                      } )

        if ODIN : mdict.update( { 'ODIN' : ODINFilter ( odinentryName( line ) , Code = self._ODIN )  } )
        if L0DU : mdict.update( { 'L0DU' : L0Filter   ( l0entryName  ( line ) , Code = self._L0DU )  } )
        if HLT  : mdict.update( { 'HLT'  : HDRFilter  ( hltentryName ( line ) , Code = self._HLT  ) } )

        if _members : 
            last = _members[-1]
            while hasattr(last,'Members') : 
                last = getattr(last,'Members')[-1]

            members = _members

            ## TODO: check if 'last' is a FilterDesktop, CombineParticles, or something else...
#            needsCopy = [ 'CombineParticles', 'FilterDesktop', 'Hlt2DisplVertices' ]
#            knownLastMembers = needsCopy + [ 'HltCopySelection<LHCb::Track>' ]
#            if last.getType() not in knownLastMembers :
#              log.warning( 'last item in line ' + self.name() + ' is ' + last.getName() + ' with type ' + last.getType() )
#            if last.getType() in needsCopy :


#	    if self.outputLocation() : 
#        	members += [ HltCopyParticleSelection( decisionName( line, 'Stripping')
#                                                     , InputSelection = 'TES:/Event/Strip/%s/Particles'%self.outputLocation()
#                                                     , OutputSelection = decisionName(line, 'Stripping')) ]

            mdict.update( { 'Filter1' : GaudiSequencer( filterName ( line,'Stripping' ) , Members = members ) })
            
        mdict.update( { 'HltDecReportsLocation' : HDRLocation } )
        if (self.outputLocation()) : 
    	    mdict.update( { 'OutputLocation' : "/Event/" + self.outputLocation() + "/Particles" } )
#        mdict.update( { 'ANNSvc' : 'StripANNSvc' } )
        
        __mdict = deepcopy ( mdict ) 
        from Configurables import StrippingAlg
        self._configurable = StrippingAlg ( self.name() , **__mdict )

        # put upper limit on combinatorics
        limitCombinatorics( self._configurable, maxCandidates = 2000 ) 

        print '# created StrippingAlg configurable for', name, '\n'
        print self._configurable

        return self._configurable

    def filterMembers( self ) : 
	_members = GaudiSequencer( filterName ( self.subname(), 'Stripping' ) ).Members
	
	while True : 
	    _foundSequencer = False
	    _flattenedMembers = []
	    for i in _members : 
		if GaudiSequencer is type(i) : 
		    _flattenedMembers += i.Members
		    _foundSequencer = True
		else : 
		    _flattenedMembers += [ i ]
	    _members = _flattenedMembers
	    if not _foundSequencer : break
	    
	print "FilterMembers for line %s : " % self.name()
	print _members
	    
	return _members

    def subname   ( self ) :
        """ 'Sub-name' of the Stripping line  """ 
        return            self._name
    ## Full name of Stripping line 
    def name      ( self ) :
        """ Full name of Stripping Line """
        return 'Stripping%s' % self._name
    ## the actual type of Stripping Line 
    def type      ( self ) :
        """ The actual type of StrippingLine Line """
        return StrippingLine

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
#        if not self._outputsel :
#            raise AttributeError, "The line %s does not define valid outputSelection " % self.subname()
        return self._outputsel

    def outputLocation ( self ) :
        """
        Get the name of output TES location of the line

        >>> line = ...
        >>> selection = line.outputLocation()
        
        """
#        if not self._outputloc :
#            raise AttributeError, "The line %s does not define valid output " % self.subname()
        return self._outputloc

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
            if    key in GaudiSequencer.__slots__ : _seq   [key] = args[key]
            elif  key in  _myslots_               : _own   [key] = args[key] 
            else                                  : _other [key] = args[key]

        # Explictly copy all major structural parameters 
        __name       = deepcopy ( name        )
        __prescale   = deepcopy ( args.get ( 'prescale'  , self._prescale  ) ) 
        __ODIN       = deepcopy ( args.get ( 'ODIN'      , self._ODIN      ) )        
        __L0DU       = deepcopy ( args.get ( 'L0DU'      , self._L0DU      ) )        
        __HLT        = deepcopy ( args.get ( 'HLT'       , self._HLT       ) )        
        __checkPV    = deepcopy ( args.get ( 'checkPV'   , self._checkPV   ) )        
        __postscale  = deepcopy ( args.get ( 'postscale' , self._postscale ) ) 
        __algos      = deepcopy ( args.get ( 'algos'     , self._algos     ) )
        __args       = deepcopy ( self._args  ) 

        # restore the original deepcopy behaviour...
        if origMethod :
            copy._deepcopy_dispatch[types.MethodType] = origMethod 
        else :
            del copy._deepcopy_dispatch[types.MethodType]

        # Check the parameters, reponsible for reconfiguration:
        for alg in [ i for i in __algos if type(i) is StrippingMember ] :
            id = alg.id()
            if id in _other :
        	 print "Updating StrippingMember arg: %s to %s" % (id, _other[id])
        	 print "Before update: "
        	 print alg.Args
                 alg.Args.update( _other [id] ) 
        	 print "After update: "
        	 print alg.Args
        	 print "\n"
                 del _other [id]

        # unknown parameters/arguments 
        if _other :
            raise AttributeError, 'Invalid attributes are detected: %s'%_other 

        # get all "OLD" arguments
        # and update them with all arguments, understandable by Sequencer 
        __args.update ( _seq   )

        return StrippingLine ( name      = __name       ,
                               prescale  = __prescale   ,
                               ODIN      = __ODIN       ,
                               L0DU      = __L0DU       ,
                               HLT       = __HLT        ,
                               checkPV   = __checkPV    ,
                               postscale = __postscale  ,
                               algos     = __algos      , 
                               **__args )

def limitCombinatorics( configurable, maxCandidates, incidentName = 'ExceedsCombinatoricsLimit' ) :
    val = False
    from Configurables import CombineParticles, StrippingAlg
    if hasattr( configurable, 'Members' ) :
        for i in getattr( configurable, 'Members' ) : 
            # order is important to avoid shortcircuit from skipping call to limitCombinatorics!
            val = limitCombinatorics( i, maxCandidates, incidentName ) or val
        return val
    elif type(configurable) == StrippingAlg :
        stages = [ 'Filter1' ]
        for i in [ getattr( configurable, j ) for j in stages if hasattr(configurable,j) ] :
            # order is important to avoid shortcircuit from skipping call to limitCombinatorics!
            val = limitCombinatorics( i, maxCandidates, incidentName ) or val
        if val : configurable.IncidentsToBeFlagged += [ incidentName ]
        return val
    elif type(configurable) == CombineParticles :
        configurable.StopAtMaxCandidates = True
        configurable.MaxCandidates       = maxCandidates
        configurable.StopIncidentType    = incidentName
        return True

