#!/usr/bin/env python
# =============================================================================
# $Id: HltLine.py,v 1.25 2008-10-28 13:53:19 graven Exp $ 
# =============================================================================
## @file
#
#  simple module which defines few helper classes & utilities
#  for configuration of Hlt(1) lines
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-22
# =============================================================================
"""
The simple module which defines few configurables for HLT line

The module defines three major public symbols :

 - class    Hlt1Line    :
      simple class to create the proper 'Configurable' for Hlt1 'lines'
 - class    Hlt1Member  :
      helper class to represent the member of Hl1 'line'
 - function htl1Lines   :
      bookeeping routine which keeps the track of all created Hlt1 'lines'
 - function hlt1Decisions:
      simle function which returns decisions for all created Hlt lines
      
Also few helper symbols are defined:

 - function hlt1Props  :
      simple function which shows the current list of 'major'
      properties/attributes of algorithms to be inspected
 - function addHlt1Prop :
      the function which allows to add more properties/attributes
      into the list of of 'major' properties to be inspected 
 - function rmHlt1Prop :
      the function which allows to remove the properties/attributes
      from the list of of 'major' properties to be inspected 

 - function hlt1InputSelections :
      get tuple of all known input selections
      
 - function hlt1OutputSelections :
      get tuple of all known output selections

 - function hlt1Selections :
      get dictionaty for all known selections:
         'Input'        : all input  selections 
         'Output'       : all output selections 
         'All'          : all input&output selections 
         'Input-Output' : all unmatched input selections 
         'Output-Input' : all unmatched output selections 
         'Input&Output' : all matched selections
         
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.25 $ "
# =============================================================================

__all__ = ( 'Hlt1Line'     ,  ## the Hlt line itself 
            'Hlt1Member'   ,  ## the representation of the line member 
            'hlt1Lines'    ,  ## list of all created lines 
            'hlt1Decisions',  ## all decisions for created alleys 
            'hlt1Props'    ,  ## list of all major properties for inspection
            'addHlt1Prop'  ,  ## add attribute/property into the list 
            'rmHlt1Prop'   ,  ## remove attribute/property form the list
            'hlt1InputSelections'  , ## the list of all known input selections
            'hlt1OutputSelections' , ## the list of all known output selections
            'hlt1Selections' ) ## the list of all matched selections
            
            

import re
from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable 

from Configurables import DeterministicPrescaler as Scaler

from Configurables import LoKi__L0Filter    as L0Filter
from Configurables import LoKi__HDRFilter   as HDRFilter
from Configurables import LoKi__ODINFilter  as ODINFilter

from Configurables import HltTrackUpgrade        as TrackUpgrade 
from Configurables import HltTrackMatch          as TrackMatch   
from Configurables import HltTrackFilter         as TrackFilter 
from Configurables import HltVertexMaker1        as VertexMaker1 
from Configurables import HltVertexMaker2        as VertexMaker2 
from Configurables import HltVertexFilter        as VertexFilter 
from Configurables import HltVertexUpgrade       as VertexUpgrade 
from Configurables import HltL0MuonPrepare       as L0MuonPrepare 
from Configurables import HltL0CaloPrepare       as L0CaloPrepare 
from Configurables import HltVertexToTracks      as VertexToTracks 
from Configurables import HltAddPhotonToVertex   as AddPhotonToVertex
from Configurables import HltDummySelection      as Dummy 

## @todo introduce the proper decision 
from Configurables import HltLine                as Line


## Convention: the name of 'Filter' algorithm inside HltLine
def filterName   ( line ) :
    """Convention: the name of 'Filter' algorithm(s) inside HltLine"""
    return 'Hlt1%sFilterSequence'   % line
## Convention: the name of 'PreScaler' algorithm inside HltLine
def prescalerName  ( line ) :
    """ Convention: the name of 'PreScaler' algorithm inside HltLine """
    return 'Hlt1%sPreScaler'  % line
## Convention: the name of 'PostScaler' algorithm inside HltLine
def postscalerName ( line ) :
    """ Convention: the name of 'PostScaler' algorithm inside HltLine """
    return 'Hlt1%sPostScaler' % line
## Convention: the name of 'ODINFilter' algorithm inside HltLine
def odinentryName    ( line ) :
    """ Convention: the name of 'ODINFilter' algorithm inside HltLine """
    return 'Hlt1%sODINFilter'   % line
## Convention: the name of 'L0DUFilter' algorithm inside HltLine
def l0entryName    ( line ) :
    """ Convention: the name of 'L0DUFilter' algorithm inside HltLine """
    return 'Hlt1%sL0DUFilter'   % line
## Convention: the name of 'HLTFilter' algorithm inside HltLine
def hltentryName    ( line ) :
    """ Convention: the name of 'HLTFilter' algorithm inside HltLine """
    return 'Hlt1%sHltFilter'   % line
## Convention: the generic name of 'member' algorithm inside HltLine 
def memberName     ( member, line ) :
    """ Convention: the generic name of 'member' algorithm inside HltLine """
    return 'Hlt1%s%s'%(line,member.subname())
## Convention: the name of 'Decision' algorithm inside HltLine
def decisionName   ( line ) :
    """Convention: the name of 'Decision' algorithm inside HltLine"""
    return 'Hlt1%sDecision'   % line if line != 'Global' else 'Hlt1Global'

## the list of all created lines 
_hlt_1_lines__ = []

# =============================================================================
## Simple function which returns the (tuple) of all currently created Hlt1Lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-08-06
def hlt1Lines () :
    """
    Simple function which returns the (tuple) of all currently created Hlt1Lines
    
    >>> lines = hlt1Lines()
    >>> print lines
    >>> for line in lines : print line

    it is also a good way to get all 'decisions' from the registered Hlt lines:

    >>> decisions = [ p.decision() for p in hlt1Lines() ]
    
    """
    return tuple(_hlt_1_lines__)

# =============================================================================
## Simple function whcih returns the decisions for all created Hlt1 lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-08-06
def hlt1Decisions () :
    """
    Simple function which returns the decisions for all created Hlt1 lines:
    
    >>> decisions = hlt1Decisions ()
    
    """
    t = [ l.decision() for l in hlt1Lines() if l.decision() ]
    return tuple(t)

## the list of all input selections
_hlt_1_input_selections__  = []
## the list of all output selections
_hlt_1_output_selections__ = []

# =============================================================================
## get the list of all known input selections
def hlt1InputSelections ( ) :
    """
    Get the full list/tuple of known input selections

    >>> inputs = hlt1InputSelections()
    
    """
    return tuple(_hlt_1_input_selections__)


# =============================================================================
## get the list of all known output selections
def hlt1OutputSelections ( ) :
    """
    Get the full list/tuple of known output selections

    >>> outputs = hlt1OutputSelections()
    
    """
    return tuple(_hlt_1_output_selections__)

# =============================================================================
## add the selection to the list of input selections
def _add_to_hlt1_input_selections_ ( sel ) :
    """
    Add the selection to the list of input selections
    """
    if list is not type(sel) and tuple is not type(sel) : sel = [sel]
    for s in sel :
        if s not in hlt1InputSelections() :
            _hlt_1_input_selections__.append ( s )
    _hlt_1_input_selections__.sort()
    return hlt1InputSelections ()

# =============================================================================
## add the selection to the list of output selections
def _add_to_hlt1_output_selections_ ( sel ) :
    """
    Add the selection to the list of output selections
    """
    if list is not type(sel) and tuple is not type(sel) : sel = [sel]
    for s in sel :
        if s not in hlt1OutputSelections() :
            _hlt_1_output_selections__.append ( s)
    _hlt_1_output_selections__.sort()
    return hlt1OutputSelections ()


# =============================================================================
## get the dictionary of all selections 
def hlt1Selections() :
    """
    Get the dictionary of all selections

    >>> sels = hlt1Selections()

    >>> print sels['All']    # get all selections 
    >>> print sels['Input']  # get input  selections 
    >>> print sels['Output'] # get output selections 
    >>> print sels['Input-Output'] # get unmatched input selections 
    >>> print sels['Output-Input'] # get unmatched output selections 
    >>> print sels['Input&Output'] # get matched input/output selections 
    
    """
    dct = {}
    dct [ 'Input'  ] = hlt1InputSelections ()
    dct [ 'Output' ] = hlt1OutputSelections ()
    import sets
    s1 = sets.Set( hlt1InputSelections  () )
    s2 = sets.Set( hlt1OutputSelections () )
    dct [ 'All' ] = tuple(s1.union(s2) )
    dct [ 'Input&Output' ] = tuple(s1.intersection(s2) )
    dct [ 'Input-Output' ] = tuple(s1.difference(s2))
    dct [ 'Output-Input' ] = tuple(s2.difference(s1))
    return dct
    
# =============================================================================
## Add the created line into the local storage of created Hlt1Lines 
def _add_to_hlt1_lines_( line ) :
    """
    Add the line into the local storage of created Hlt1Lines 
    """
    _hlt_1_lines__.append ( line ) 
        
# =============================================================================
## the list of valid members of Hlt1 sequencer 
_types_ = { TrackUpgrade  : 'TU'  
          , TrackMatch    : 'TM'  
          , TrackFilter   : 'TF'  
          , VertexMaker1  : 'VM1' 
          , VertexMaker2  : 'VM2'
          , VertexFilter  : 'VF'
          , VertexToTracks: 'VT'
          , VertexUpgrade : 'VU'
          , L0CaloPrepare : 'L0CaloPrepare'
          , L0MuonPrepare : 'L0MuonPrepare'
          , AddPhotonToVertex : 'AddPhotonToVertex'
          , Dummy         : 'Dummy'
          } 

## protected attributes 
_protected_ = ( 'IgnoreFilterPassed' , 'Members' , 'ModeOR', 'DecisionName', 'Prescale','Postscale','Filter1' )
## own slots for HltLine 
_myslots_   = ( 'name' , 'prescale'  , 'postscale' , 'ODIN' , 'L0DU' , 'HLT' , 'algos' ) 

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

    - if the selection is of type Hlt1Line, replace it by its output selection
    
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
    if type(name) in [ Hlt1Line, bindMembers ] : name = name.outputSelection()
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
    Check specified selection:

    - If selection is an Hlt1Line or bindMembers, substitute its output selection

    - If selection has a 'pattern' form (starts from %) ,
          it is expanded properly and the dictionary is updated accordingly

    - TODO: The selection is required to be in the dictionary, 
            and a warning should be given if an input is used which 
            is not contained in the current line (i.e. when the line
            has 'external' dependencies)
    
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

#def _last_algo_with_output( algos ) :
#    print algos
#    for i in  algos : print i
#    for i in range(len(algos)-1,-1,-1) : 
#        if 'OutputSelection' in algos[i].Type.__slots__ : return i
#    return None
# =============================================================================
## Bind members to an HLT line
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date   2008-09-16
class bindMembers (object) :
    """
    Simple class to represent a set of Hlt1Members which are bound to a line
    """
    __slots__ = ('_members','_outputsel')

    def members( self ) : return self._members
    def outputSelection( self ) : return self._outputsel


    def __init__( self, line, algos ) :
        if line == None: raise AttributeError, 'Must have a line name to bind to'

        self._members = []
        self._outputsel = None


        # last_output_index = _last_algo_with_output(algos)
        # for algi in range(len(algos)) :
        for alg in algos:
            # print 'iterator: at ' + str(algi)
            # print '  type: ' + str(type(algos[algi]))
            # alg  = algos[algi]
            # last = ( last_output_index and algi == last_output_index ) 
            # print 'last? ' + str(last)

            # allow chaining of previously bound members...
            if type(alg) is bindMembers:
                self._members  += alg.members()
                self._outputsel = alg.outputSelection()
                continue

            # if not Hlt1Member, blindly copy -- not much else we can do
            if type(alg) is not Hlt1Member:
                self._members += [ alg ]
                continue
                
            # if Hlt1Member, verify, expand, and chain
            margs = alg.Args.copy() 
            #### TODO: use _checkSelection to make sure the result is valid!!!
            #    expand '%' in FilterDescriptor, InputSelection{,1,2} to allow bound selections
            _subs_cand_ =  ['FilterDescriptor', 'OutputSelection', 'InputSelections'
                           , 'InputSelection', 'InputSelection1','InputSelection2','InputSelecion3' ]
            for key in set(margs).intersection(set(_subs_cand_)) :
                if type(margs[key]) is str  : margs[key] =   re.sub('%','Hlt1%s'%line,margs[key])
                if type(margs[key]) is list : margs[key] = [ re.sub('%','Hlt1%s'%line,i) for i in margs[key] ]
            algName = alg.name ( line )
            
            ## need input selection?
            if   'InputSelection3' in alg.Type.__slots__ :
                # check input selections
                _checkSelections ( margs ,  # arguments
                                   # must be: 
                                   [ 'InputSelection3' , 'InputSelection2' , 'InputSelection1' ] ,
                                   # not allowed:
                                   [ 'InputSelection'  , 'InputSelections' ] ,
                                   algName , line ) ;
                #
            elif 'InputSelection2' in alg.Type.__slots__ :
                # check input selections
                _checkSelections ( margs ,  # arguments
                                   # must be: 
                                   [ 'InputSelection2' , 'InputSelection1' ] ,
                                   # not allowed:
                                   [ 'InputSelection'  , 'InputSelections' , 'InputSelection3' ] ,
                                   algName , line ) ;
                # 
            elif 'InputSelections' in alg.Type.__slots__ :
                # check input selections
                _checkSelections ( margs ,  # arguments
                                   # must be: 
                                   [ 'InputSelections' ] ,
                                   # not allowed:
                                   [ 'InputSelection'  , 'InputSelection1' , 'InputSelection2' , 'InputSelection3' ] ,
                                   algName , line ) ;
                #
            elif 'InputSelection' in alg.Type.__slots__ :
                # come manual work 
                _inputsel = self._outputsel
                if margs.has_key ( 'InputSelection' ) :
                    _checkSelection ( 'InputSelection' , margs , algName , line )
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
                                   algName , line ) ;
                #
            else :
                print "# WARNING: Line '%s', Member '%s'  with strange configuration " % ( line , algName )

            
            ## output selection (default: the algorithm instance name)
            ## TODO: make sure the 'last' algorithm in the line (which has 'OutputSelection' in 
            ## its __slots__) has an OutputSelection which matches the decision name...
            if 'OutputSelection' in alg.Type.__slots__ : 
                # self._outputsel = algName if not last else 'Hlt1%sDecision'%line
                self._outputsel = algName
                if margs.has_key ( 'OutputSelection' ) :
                    _checkSelection ( 'OutputSelection' , margs , algName , line ) 
                    self._outputsel = margs['OutputSelection']
            
            # create (the configurable for) the algorithm and add it to the sequencer:
            self._members += [ alg.createConfigurable( line , **margs ) ]

        


# =============================================================================
## @class Hlt1Tool
#  Simple class to represent the settings for a tool used by an Hlt1Member
#  Note: in case of a tool used by a tool, we support recursion...
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date   2008-08-23  
class Hlt1Tool (object ) :  
    """
    Simple class to represent the settings for a tool used by an Hlt1Member
    @author Gerhard Raven Gerhard.Raven@nikhef.nl
    @date   2008-08-23  
    """
    __slots__ = ( 'Type' , 'Name' , 'Args' )

    ### The standard constructor to create an Hlt1Tool instance:
    def __init__ ( self      , ## ...
                   type = None , ## the type of the (configurable corresponding to the) tool
                   name = '' , ## the instance name of the tool
                   **Args ) :  ## other arguments 
        if type  == None : raise AttributeError, "Tool must have a type"
        self.Type = type
        self.Name = name if len(name) > 0 else type().getType()
        self.Args = Args
        ### need to deal with recursion on 2nd (and higher) level tools
        ### eg. when Args contains anything of type Hlt1Tool...
        ### TODO/FIXME: what if this is a private tool???
    
    def createConfigurable( self, parent ) :
        # TODO: check that we don't try to generate two (different) configs for the same instance?
        #       (which wouldn't work, and would only result in the 2nd instance overruling
        #        the settings of the first)
        # TODO: do we need addTool??? (only if recursive? only if private tool?)
        conf = self.Type( self.Name, self.Args )
        for k,v in self.Args.iteritems() : setattr(conf,k,v)
        return conf
        
   

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
    __slots__ = ( 'Type' , 'Name' , 'Args', 'Tools' )
    
    ### The standard constructor to create the  Hlt1Member instance:
    def __init__ ( self      , ## ...
                   Type      , ## short type of members, e.g. 'TF' HltTrackFilter
                   name = '' , ## the specific part of the algorithm name 
                   tools = [] , ## list of tool options for this algorithm
                   **Args ) :  ## arguments 
        """
        The standard constructor to create the  Hlt1Member instance:
        
        >>> m1 = Hlt1Member ( 'TU' , 'Velo'   , RecoName = 'Velo' ) ,
        >>> m2 = Hlt1Member ( 'TF' , 'Velo'      ,
                    FilterDescriptor = [ 'IP_PV2D,||>,0.1',
                                         'Calo3DChi2_L0TriggerHadron,<,4' ] ) 
        """
        Name = name
        if type(Type) is str : Type = typeFromNick ( Type ) 
        if not Type in _types_ :
            raise AttributeError, "The type  %s is not known for Hlt1Member"%Type
        self.Type = Type 
        self.Name = Name 
        self.Args = Args
        self.Tools = tools
        for key in Args :
            if  key not in self.Type.__slots__  :
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
    def createConfigurable( self, line, **args ) :
        " Create the configurable, and, if needed, deal with tool configuration "        
        # see if alg has any special Tool requests...
        name = memberName( self, line )
        for tool in self.Tools : tool.createConfigurable( parent = name )
        return self.Type( name, **args)

# ============================================================================
## @class Hl1Line
#  The major class which represent the Hlt1 Line, the sequence.
#
#  The structure of each line is fixed to be
# 
#    - Prescaler
#    - ODINFilter | L0DUFilter | HLTFilter
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
    #    - 'ODIN'      : the list of ODINtype names for ODINFilter 
    #    - 'L0DU'      : the list of L0Channels names for L0Filter 
    #    - 'HLT'       : the list of HLT selections for HLTFilter
    #    - 'algos'     : the list of actual members 
    #    - 'postscale' : the postscale factor
    def __init__ ( self           ,
                   name           ,   # the base name for the Line
                   prescale  = 1  ,   # prescale factor
                   ODIN      = None ,   # ODIN predicate  
                   L0DU      = None ,   # L0DU predicate  
                   HLT       = None ,   # HltDecReports predicate
                   algos     = [] ,   # the list of algorithms/members
                   postscale = 1  ,   # prescale factor
                   **args         ) : # other configuration parameters
        """
        The constructor, which essentially defines the line
        
        The major arguments
        - 'name'      : short name of the line, e.g, 'SingleMuon'
        - 'prescale'  : the prescaler factor
        - 'ODIN'      : the list of ODIN types for ODINFilter (mutally exclusive with L0DU and HLT)
        - 'L0DU'      : the list of L0Channels names for L0Filter (mutally exclusive with ODIN and HLT)
        - 'HLT'       : the list of HLT selections for HLTFilter (mutally exclusive with ODIN and L0)
        - 'algos'     : the list of actual members 
        - 'postscale' : the postscale factor
        
        """
        # verify exclusivity...
        if L0DU and HLT :
            raise AttributeError, "The attribute L0DU and HLT should be exclusive -- line %s" % name 
        if L0DU and ODIN :
            raise AttributeError, "The attribute L0DU and ODIN should be exclusive -- line %s" % name 
        if HLT and ODIN :
            raise AttributeError, "The attribute HLT and ODIN should be exclusive -- line %s" % name 
        # 1) save all parameters (needed for the proper cloning)
        self._name      = name
        self._prescale  = prescale
        
        self._ODIN      = ODIN
        self._L0DU      = L0DU
        self._HLT       = HLT
        
        self._postscale = postscale
        self._algos     = algos
        self._args      = args

        
        # decision: (pre)set to None, and assign once we're successfully completed ourselfs...
        self._decision  = None 
        
        # check for forbidden attributes
        mdict = {} 
        for key in args :
            if key in _protected_ :
                raise AttributeError, "The attribute'%s' is protected for %s"%(key,self.type())
            mdict[key] = args[key] 

        #start to contruct the sequence        
        line = self.subname()
        
        #TODO: shouldn't we just allow all three of them? 
        #      and shouldn't each correspond to a stage in the DecReport?
        _seed = None
        if  ODIN  : _seed = ODINFilter ( odinentryName( line ) , Code = self._ODIN ) 
        elif L0DU : _seed = L0Filter   ( l0entryName  ( line ) , Code = self._L0DU ) 
        elif HLT  : _seed = HDRFilter  ( hltentryName ( line ) , Code = self._HLT  )
        
        # most recent output selection
        self._outputsel = None

        # bind members to line
        _boundMembers = bindMembers( line, algos )
        _members = _boundMembers.members()
        self._outputsel = _boundMembers.outputSelection()
        
        # register selections:
        _input_selection_properties_ = [ 'InputSelection' , 'InputSelection1'
                                       , 'InputSelection2', 'InputSelection3' 
                                       , 'InputSelections'
                                       ] 
        for _m in _members :
            for attr in _input_selection_properties_ :
                if hasattr ( _m , attr  ) :
                    _add_to_hlt1_input_selections_ ( getattr( _m, attr ) )
            if hasattr ( type(_m) , 'OutputSelection' ) :
                if hasattr ( _m , 'OutputSelection' ) :
                    _add_to_hlt1_output_selections_ ( _m.OutputSelection )
                else :
                    _add_to_hlt1_output_selections_ ( _m.name         () )

        if self._outputsel is not None and self._outputsel!= decisionName( line ) :
            print "# WARNING: Line '%s' has a final output selection named '%s'"%(line,self._outputsel)

        # create the line configurable
        mdict.update( { 'DecisionName' : decisionName ( line ) 
                      , 'Prescale'     : Scaler(     prescalerName ( line ) , AcceptFraction = self._prescale  )
                      , 'Postscale'    : Scaler(    postscalerName ( line ) , AcceptFraction = self._postscale ) 
                      , 'Seed'         : _seed } )
        if _members : mdict.update( { 'Filter1' : GaudiSequencer( filterName ( line ) , Members = _members ) })
        self._configurable = Line ( self.name() , **mdict )

        ## finally assign the decision name!
        self._decision = decisionName ( line )

        # register into the local storage of all created Lines
        _add_to_hlt1_lines_( self ) 

    ## 'sub-name' of Hlt Line 
    def subname   ( self ) :
        """ 'Sub-name' of the Hlt line  """ 
        return            self._name
    ## Full name of Hlt line 
    def name      ( self ) :
        """ Full name of Hlt Line """
        return 'Hlt1%s' % self._name
    ## the actual type of Hlt Line 
    def type      ( self ) :
        """ The actual type of Hlt Line """
        return Hlt1Line
    ## Get the underlying 'Configurable'
    #  probably it is the most important method except the constructor
    #
    #  @code 
    #  >>> line = Hlt1Line ( .... )
    #  >>> conf = line.sequencer()
    #  @endcode
    def sequencer ( self ) :
        """
        Get the underlying 'Configurable' instance 
        probably it is the most important method except the constructor
        
        >>> line = Hlt1Line ( .... )
        >>> conf = line.sequencer()
        
        """
        return self._configurable
    ## Get the underlying 'Configurable'
    #  probably it is the most important method except the constructor
    #
    #  @code 
    #  >>> line = Hlt1Line ( .... )
    #  >>> conf = line.configurable() 
    #  @endcode    
    def configurable ( self ) :
        """
        Get the underlying 'Configurable' instance 
        probably it is the most important method except the constructor
        
        >>> line = Hlt1Line ( .... )
        >>> conf = line.configurable()
        """
        return self.sequencer()
    ## get the lits of all 'IDs' for all members 
    def ids ( self ) :
        """ Get the list of all IDs for all members """
        _ids = [] 
        for alg in self._algos :
            if Hlt1Member is type(alg) : _ids += [ alg.id() ] 
        return _ids
    ## get the decision of the line
    def decision ( self ) :
        """
        Get the actual decision of the line

        >>> line = ...
        >>> decision = line.decision()
        
        """
        if not self._decision :
            raise AttributeError, "The line %s does not define valid decision " % self.subname()
        return self._decision
    ## get the last output selection of the line
    def outputSelection ( self ) :
        """
        Get the name of last outputSelection of the line

        >>> line = ...
        >>> selection = line.outputSelection()
        
        """
        if not self._outputsel :
            raise AttributeError, "The line %s does not define valid outputSelection " % self.subname()
        return self._outputsel
    
    
    ## Clone the line  
    def clone ( self , name , **args ) :
        """
        Clone the line
        
        A new Hlt Line is created with new name, all property/attrributes maps
        are updated accordingly.
        
        """
        # classify arguments:
        _own   = {} # own arguments 
        _seq   = {} # arguments for sequencer
        _other = {} # the rest (probably reconfiguration of members)
        for key in args :
            if    key in GaudiSequencer.__slots__ : _seq[keq]   = args[key]
            elif  key in  _myslots_               : _own[key]   = args[key] 
            else                                  : _other[key] = args[key]

        # Explictly copy all major structural parameters 
        __name       = name
        __prescale   = args.get ( 'prescale'  , self._prescale  ) 
        __ODIN       = args.get ( 'ODIN'      , self._ODIN      )         
        __L0DU       = args.get ( 'L0DU'      , self._L0DU      )         
        __HLT        = args.get ( 'HLT'       , self._HLT       )         
        __postscale  = args.get ( 'postscale' , self._postscale )  
        __algos      = args.get ( 'algos'     , self._algos     )      
        __args       = self._args

        # Check the parameters, reponsible for reconfiguration:
        for alg in [ i for i in __algos if type(i) is Hlt1Member ] :
            id = alg.id()
            if id in _other :
                alg.Args.update( _other [id] ) 
                del _other [id]

        # unknonw parameters/arguments 
        if _other :
            raise AttributeError, 'Invalid attributes are detected: %s'%_other 

        # get all "OLD" arguments
        # and update them with all arguments, understandable by Sequencer 
        __args.update ( _seq   )

        return Hlt1Line ( name      = __name       ,
                          prescale  = __prescale   ,
                          ODIN      = __ODIN       ,
                          L0DU      = __L0DU         ,
                          HLT       = __HLT        ,
                          postscale = __postscale  ,
                          algos     = __algos      , **__args )
    
    

# =============================================================================
# Soem useful decorations 
# =============================================================================
## Calculate the effective length of the string
#  which is the length from the last '\n'-symbol 
def len1 ( line ) :
    """
    Calculate the effective length of the line,
    which is thw length from the last '\n'-symbol 
    """
    _i = line.rfind('\n')
    return len(line) if _i < 0 else len(line) - _i

## the major properties/attributes  
_hlt1_props_   = [ 'AcceptFraction'    , 
                   'PercertPass'       ,
                   'L0Channels'        ,
                   'InputSelection'    ,
                   'InputSelection1'   ,
                   'InputSelection2'   ,
                   'InputSelection3'   ,
                   'InputSelections'   ,
                   #'OutputSelection'   ,
                   'RecoName'          ,
                   'MatchName'         ,
                   'FilterDescription' ] 

## Get the tuple of major interesting properties for Hlt1  objects
def hlt1Props () :
    """
    Get the tuple of major interesting properties for Hlt1  objects

    >>> print hlt1Props()
    
    """
    _hlt1_props_.sort() 
    return tuple(_hlt1_props_)

## Add the item into the list of Hlt major proeprties 
def addHlt1Prop ( prop ) :
    """
    Add the item into the list of Hlt major proeprties
    
    >>> print hlt1Props ()
    >>> print addHlt1Prop ( 'OutputLevel' )
    
    """
    if  not list is type(prop) and not  tuple is type(prop) : prop = [ prop ] 
    # loop over all items:
    for item in prop :
        if not item in _hlt1_props_ : _hlt1_props_.append ( item )
    # 
    return hlt1Props()

## remove the item from the list of Hlt1 major properties
def rmHlt1Prop ( prop ) :
    """
    Remove the item into the list of Hlt major proeprties
    
    >>> print hlt1Props ()
    >>> rmHlt1Prop ( 'OutputLevel' )
    
    """
    if  not list is type(prop) and not  tuple is type(prop) : prop = [ prop ]
    for item in prop :
        if not item in _hlt1_props_ : return hlt1Props() 
        pos = _hlt1_props_.index ( item )
        del _hlt1_props_[ pos ]
    return hlt1Props() 

# =============================================================================
## Print the major properties/attributes of the configurables
#  @param obj  the object to be inspected
#  @param lst  list of attributes/properties 
#  @param line the (output) line
#  @paral l1   the indentation parameter
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-08-06
def prnt ( obj        ,   # the object 
           lst  = []  ,   # list of 'major' properties
           line = ''  ,   # the line to eb updated
           l1   = 65  ) : # the indentation/format parameter 
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
    if not lst : lst = hlt1Props () 
    elif list  is type(lst) : pass
    elif tuple is type(lst) : pass
    else : lst = [ lst ]
    #
    for item in lst :
        if hasattr ( obj , item ) :
            if l1 < len1( line ) : line += '\n#' + (l1-1)*' '
            line = line + " %-15s : %s" % ( item , getattr ( obj , item ) )
    return line 

# =============================================================================
## The helper function for narcissic self-print of sequences  & algorithms 
#  @param self  the object to be inspected
#  @param level the recursion level
#  @param lst   the list of major properties/attributes 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-08-06
def __enroll__ ( self       ,   ## the object
                 level = 0  ,   ## the recursion level
                 lst   = [] ) : ## the major properties  
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

    _tab = 50
    _indent_ = ('#%-3d'%level) + level * '   ' 
    try:     line = _indent_ + self.name ()
    except:  line = _indent_ + '<UNKNOWN>'
        
    if len1(line)>( _tab-1): line = line + '\n#'+ (_tab-1)*' '
    else :                   line = line + (_tab-len1(line))*' '
    try:    line = line + '%-25.25s'%self.getType()
    except: line = line + '<UNKNOWN>'


    line = prnt ( self , lst , line, l1 = _tab+25 )
    
    line = line + '\n'

    # use the recursion 
    if hasattr ( self , 'Members' ) :
        _ms = self.Members
        for _m in _ms : line = line + __enroll__ ( _m , level + 1 , lst ) 

    if type(self) is Line :
        line = line + __enroll__( self.Prescale,  level + 1, lst )
        line = line + __enroll__( self.Seed,      level + 1, lst )
        if hasattr(self,'Filter1') : line = line + __enroll__( self.Filter1,   level + 1, lst )
        line = line + __enroll__( self.Postscale, level + 1, lst )

    return line


Hlt1Line       . __str__ = __enroll__    
Line           . __str__ = __enroll__ 
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
