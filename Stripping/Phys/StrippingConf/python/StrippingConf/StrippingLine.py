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
from Gaudi.Configuration import *
from GaudiConfUtils import isConfigurable
from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable
import Configurables
if hasattr(Configurables,"OfflineDeterministicPrescaler"):
    from Configurables import OfflineDeterministicPrescaler as Scaler
else:
    from Configurables import DeterministicPrescaler as Scaler

from Configurables import LoKi__L0Filter    as L0Filter
from Configurables import LoKi__HDRFilter   as HDRFilter
from Configurables import LoKi__ODINFilter  as ODINFilter
from Configurables import LoKi__VoidFilter  as VOIDFilter
from SelPy.selection import flatAlgorithmList
from GaudiConfUtils import isConfigurable
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

## Convention: the name of 'VOIDFilter' algorithm inside StrippingLine
def voidentryName    ( line, level = 'Stripping' ) :
    """ Convention: the name of 'VOIDFilter' algorithm inside StrippingLine """
    return '%s%sVOIDFilter'   % (level,line)

## Convention: the name of 'L0DUFilter' algorithm inside StrippingLine
def l0entryName    ( line, level = 'Stripping' ) :
    """ Convention: the name of 'L0DUFilter' algorithm inside StrippingLine """
    return '%s%sL0DUFilter'   % (level,line)

## Convention: the name of 'HLTFilter' algorithm inside StrippingLine
def hltentryName    ( line, level = 'Stripping' ) :
    """ Convention: the name of 'HLTFilter' algorithm inside StrippingLine """
    return '%s%sHltFilter'   % (level,line)

def decisionName   ( line, level = 'Stripping'  ) :
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
    __slots__ = ('_members', '_outputloc', '_selection')

    def outputLocation( self ) : 
        return self._outputloc

    def selection( self ) :
        return self._selection

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
                self._outputloc = "Phys/"+alg.OutputSelection
        elif hasattr ( type(alg) , 'OutputLocation' ) :
            if hasattr ( alg , 'OutputLocation' ) :
                self._outputloc = alg.OutputLocation 
        else :
#            self._outputsel = None
#            self._outputloc = None
            self._outputloc = "Phys/"+alg.name()

    def _default_handler_( self, line, alg ) :
        return self._handle_Selection(line, alg)

    def _handle_Configurable( self, line, alg ) :
        if isinstance(line, str) and line.find("Stream") != 0 :
            log.warning('line' + line + ' uses plain Gaudi configurable ' + alg.name() + '. Consider using Selection instead!')
        # if not known, blindly copy -- not much else we can do
        self._members += [ alg ]
        # try to guess where the output goes...
        self._getOutputLocation(alg)


    def _handle_SelectionSequence(self, line, alg) :
        raise TypeError('Line '+line+': Use of SelectionSequence forbidden. Use Selection types instead')

    def _handleSelectionType(self, line, sel) :
        members = flatAlgorithmList(sel)
        self._members += members
        self._selection = sel
        loc = sel.outputLocation()
        self._outputloc = loc

    def _handle_Selection(self, line, alg) :
        sel = alg.clone(name=line)
        self._handleSelectionType( line, sel )
        
    def _handle_PassThroughSelection(self, line, alg) :
        if alg.outputLocation() != '' :
            from PhysSelPython.Wrappers import MergedSelection
            alg = MergedSelection(line, RequiredSelections = [alg])       
            self._handleSelectionType( line, alg )

    def _handle_VoidEventSelection(self, line, alg) :
        if alg.outputLocation() != '' :
            from PhysSelPython.Wrappers import MergedSelection
            alg = MergedSelection(line, RequiredSelections = [alg])       
            self._handleSelectionType( line, alg )

    def _handle_AutomaticData(self, line, alg) :
        from PhysSelPython.Wrappers import MergedSelection
        sel = MergedSelection(line, RequiredSelections = [alg])
        self._handleSelectionType( line, sel )

    # allow chaining of previously bound members...
    def _handle_bindMembers( self, line, alg ) :
        self._members  += alg.members()
        # sometimes, we want to ignore this... 
        # add a flag to allow to skip this (when set to None?)
        if alg.outputLocation() : self._outputloc = alg.outputLocation()

    def _handle_StrippingMember( self, line, alg ) :
        if line == None: raise AttributeError, 'Must have a line name to bind to'
        alg = alg.createConfigurable( line, **alg.Args )
        return self._handle_Configurable( line,  alg )

    def __init__( self, line, algos ) :

        self._members = []
        self._outputloc = None
        self._selection = None
        for alg in algos:
            # dispatch according to the type of alg...
            if isConfigurable(alg) :
                self._handle_Configurable(line, alg)
            else :
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

# =============================================================================
## @class StrippingLine
class StrippingLine(object):

    def __init__ ( self             ,
                   name             ,   # the base name for the Line
                   prescale  = 1    ,   # prescale factor
                   ODIN      = None ,   # ODIN predicate
                   L0DU      = None ,   # L0DU predicate
                   HLT       = None ,   # HltDecReports predicate
                   FILTER    = None ,   # 'VOID'-predicate, e.g. Global Event Cut
                   checkPV   = True ,   # Check PV before running algos
                   algos     = None ,   # the list of stripping members
                   selection = None ,
                   postscale = 1    ,   # postscale factor
                   MaxCandidates = "Override",   # Maxumum number of candidates for CombineParticles
                   MaxCombinations = "Override", # Maxumum number of combinations for CombineParticles
                   HDRLocation = None,           # if None, defined by stream name
                   EnableFlavourTagging = False, # If True, run FlavourTaggingTool to store FT info
                   ExtraInfoTools = None,        # Configuration of ExtraInfo tools, as a list of dictionaries (or None)
                   **args           ) : # other configuration parameters

        if algos and selection :
            raise Exception('only algos or selection can be set. You have set both.')
        if selection :
            if isConfigurable(selection) :
                raise TypeError('StrippingLine selection cannot be Configurable type.')
            algos = [selection]

        if not algos :
            algos = []

        ## 1) clone all arguments
        name   = deepcopy ( name   )
        ODIN   = deepcopy ( ODIN   )
        L0DU   = deepcopy ( L0DU   )
        HLT    = deepcopy ( HLT    )
        FILTER = deepcopy ( FILTER )
        algos  = deepcopy ( algos  )
        args   = deepcopy ( args   )
        # 2) save all parameters (needed for the proper cloning)
        self._name      = name
        if callable(prescale) : prescale = prescale( self.name() )
        self._prescale  = prescale
        
        self._ODIN      = ODIN
        self._L0DU      = L0DU
        self._HLT       = HLT
        self._FILTER    = FILTER
        self._checkPV   = checkPV
        self._HDRLocation = HDRLocation
        self._EnableFlavourTagging = EnableFlavourTagging
        
        if callable(postscale) : postscale = postscale( self.name() )
        self._postscale = postscale
        self._algos     = algos
        self._args      = args
        self.MaxCandidates = MaxCandidates
        self.MaxCombinations = MaxCombinations
        self.ExtraInfoTools = ExtraInfoTools

        line = self.subname()

        self._appended  = False

        #start to contruct the sequence        

        self._members = []

        self._selection = None
        
        self.fullHDRLocation = None
        
        # if needed, check Primary Vertex before running all algos
        
        from Configurables import CheckPV
        if checkPV == True:
    	    check = CheckPV("checkPVmin1");
    	    check.MinPVs = 1;
    	    self._members.insert(0, check);
    	elif isinstance(checkPV, int) : 
    	    check = CheckPV("checkPVmin%d" % checkPV)
    	    check.MinPVs = checkPV
    	    self._members.insert(0, check);
    	elif isinstance(checkPV, tuple) : 
    	    if len(checkPV) == 2 : 
    		check = CheckPV("checkPVmin%dmax%d" % checkPV)
    		check.MinPVs = checkPV[0]
    		check.MaxPVs = checkPV[1]
    		self._members.insert(0, check);
    	    else :
    		raise TypeError, "Wrong checkPV tuple length %d, should be 2" % len(checkPV)
    	elif checkPV != False : 
    	    raise TypeError, "Wrong checkPV argument type '%s'" % type(checkPV).__name__

        # if needed, apply filter before running all algos
        if FILTER :
            if isinstance   ( FILTER , str   ) :
                fltr = VOIDFilter  ( voidentryName  ( line ) , Code = FILTER )
                self._members.insert ( 0 , fltr )
            elif isinstance ( FILTER , ( tuple , list ) ) and 2 == len ( FILTER ) :
                fltr = VOIDFilter  ( voidentryName  ( line ) , Code = FILTER[0] , Preambulo = FILTER[1] )
                self._members.insert ( 0 , fltr )
            elif isinstance ( FILTER , dict     ) :
                fltr = VOIDFilter  ( voidentryName  ( line ) , **FILTER ) 
                self._members.insert ( 0 , fltr )
            else :
    		raise TypeError, "Wrong FILTER attribute: %s " % FILTER
            
        # bind members to line
        _boundMembers    = bindMembers( line, algos )
        self._members   += _boundMembers.members()
        self._outputloc  = _boundMembers.outputLocation()
        self._selection = _boundMembers.selection()

        # register into the local storage of all created Lines
        _add_to_stripping_lines_( self ) 

    def selection(self) :
        return self._selection
        
    def declareAppended( self ) :
	self._appended = True 
	
    def isAppended( self ) : 
	return self._appended

    def createConfigurable( self, TESPrefix = "Strip", HDRLocation = 'Phys/DecReports' ) : 
        
        if self._HDRLocation == None : 
    	    self.fullHDRLocation = TESPrefix + "/" + HDRLocation
    	else : 
    	    self.fullHDRLocation = self._HDRLocation
        
        # check for forbidden attributes
        args    = self._args

        mdict = {} 
        for key in args :
            if key in _protected_ :
                raise AttributeError, "The attribute'%s' is protected for %s"%(key,self.type())
            mdict[key] = args[key] 

        line = self.subname()
        
        # create the line configurable
        # NOTE: even if pre/postscale = 1, we want the scaler, as we may want to clone configurations
        #       and change them -- and not having the scaler would be problem in that case...
        mdict.update( { 'Prescale'     : Scaler(     prescalerName ( line,'Stripping' ) , AcceptFraction = self._prescale  )
                      , 'Postscale'    : Scaler(    postscalerName ( line,'Stripping' ) , AcceptFraction = self._postscale ) 
                      } )

        if self._ODIN   : mdict.update( { 'ODIN'    : ODINFilter ( odinentryName ( line ) , Code = self._ODIN   )  } )
        if self._L0DU   : mdict.update( { 'L0DU'    : L0Filter   ( l0entryName   ( line ) , Code = self._L0DU   )  } )
        if self._HLT    : mdict.update( { 'HLT'     : HDRFilter  ( hltentryName  ( line ) , Code = self._HLT    ) } )

	# Add flavour tagging tool to the end of line sequence if needed
	if self._EnableFlavourTagging : 
	    if not self.outputLocation() or self.outputLocation == "" : 
                raise AttributeError, "Line %s does not have output, cannot do flavour tagging" % self.name()
	    from Configurables import BTagging
	    btag = BTagging("BTag_"+self.name(), Inputs = [ self.outputLocation() ] ) 
	    self._members.append(btag)

	# Add extra info tools if needed
	if self.ExtraInfoTools : 
	    from Configurables import AddExtraInfo
	    extraInfoAlg = AddExtraInfo('ExtraInfo' + self.name())
            extraInfoAlg.MaxLevel = 1
            extraInfoAlg.Inputs = [ self.outputLocation() ] 
            toolNames = []
            toolNum = 0
            for itool in self.ExtraInfoTools : 
        	toolNum += 1
        	toolType = itool["Type"]
        	toolName = "Tool%d" % toolNum
        	module = __import__("Configurables", globals(), locals(), [ toolType ] )
        	toolClass = getattr( module, toolType )
                localName = "ExtraInfo" + self.name() + "." + toolName
        	extraInfoAlg.addTool( toolClass, toolName )
        	toolInstance = getattr( extraInfoAlg, toolName )
        	for property,value in itool.iteritems() : 
        	    if property == "Type" : continue
        	    setattr( toolInstance, property, value)
        	toolNames += [ toolType + '/' + toolName ]
       	    extraInfoAlg.Tools = toolNames
	    self._members.append(extraInfoAlg)

        if self._members : 
            mdict.update( { 'Filter1' : GaudiSequencer( filterName ( line,'Stripping' ) , Members = self._members, OutputLevel = WARNING ) })
            
        mdict.update( { 'HltDecReportsLocation' : self.fullHDRLocation } )
        if (self.outputLocation()) : 
    	    mdict.update( { 'OutputLocation' : self.outputLocation() } )
        
        __mdict = deepcopy ( mdict ) 
        from Configurables import StrippingAlg
        self._configurable = StrippingAlg ( self.name() , **__mdict )

        # put upper limit on combinatorics
        if self.MaxCandidates == "Override" : self.MaxCandidates = None
        if self.MaxCombinations == "Override" : self.MaxCombinations = None
        limitCombinatorics( self._configurable, 
                            MaxCandidates = self.MaxCandidates, 
                            MaxCombinations = self.MaxCombinations ) 

        log.debug(' created StrippingAlg configurable for' +  self._name)
        log.debug( self._configurable )

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

	log.debug( "FilterMembers for line %s : " % self.name() )
	log.debug( _members )
	    
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

    def decReportLocation ( self ) :
	return self.fullHDRLocation

    def outputLocation ( self ) :
        """
        Get the name of output TES location of the line

        >>> line = ...
        >>> selection = line.outputLocation()
        
        """
#        if not self._outputloc :
#            raise AttributeError, "The line %s does not define valid output " % self.subname()
        return self._outputloc

    def prescale( self ) :
        return self._prescale

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
        __FILTER     = deepcopy ( args.get ( 'FILTER'    , self._FILTER    ) )        
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
                               FILTER    = __FILTER     ,
                               checkPV   = __checkPV    ,
                               postscale = __postscale  ,
                               algos     = __algos      , 
                               **__args )

def limitCombinatorics( configurable, 
                        MaxCandidates, 
                        MaxCombinations, 
                        incidentName = 'ExceedsCombinatoricsLimit' ) :
    val = False
    from Configurables import CombineParticles, StrippingAlg
    from Configurables import SubPIDMMFilter, SubstitutePID
    if hasattr( configurable, 'Members' ) :
        for i in getattr( configurable, 'Members' ) : 
            # order is important to avoid shortcircuit from skipping call to limitCombinatorics!
            val = limitCombinatorics( i, MaxCandidates, MaxCombinations, incidentName ) or val
        return val
    elif type(configurable) == StrippingAlg :
        stages = [ 'Filter1' ]
        for i in [ getattr( configurable, j ) for j in stages if hasattr(configurable,j) ] :
            # order is important to avoid shortcircuit from skipping call to limitCombinatorics!
            val = limitCombinatorics( i, MaxCandidates, MaxCombinations, incidentName ) or val
        if val : configurable.IncidentsToBeFlagged += [ incidentName ]
        return val
    elif type(configurable) == CombineParticles :
	if MaxCandidates != None : 
    	    configurable.StopAtMaxCandidates = True
    	    configurable.MaxCandidates       = MaxCandidates
    	if MaxCombinations != None : 
    	    configurable.StopAtMaxCombinations = True
    	    configurable.MaxCombinations       = MaxCombinations
    	if MaxCandidates != None or MaxCombinations != None : 
    	    configurable.StopIncidentType    = incidentName
    	    return True
	else :
	    return False
    elif type(configurable) == SubPIDMMFilter or \
             type(configurable) == SubstitutePID:
	if MaxCandidates != None: 
    	    configurable.MaxParticles        = MaxCandidates
    	    configurable.StopIncidentType    = incidentName
    	    return True
	else :
	    return False
        
