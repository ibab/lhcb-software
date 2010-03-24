"""
High level configuration tools for StrippingConf, to be used with Moore and DaVinci
"""

__author__  = "Anton Poluektov <A.O.Poluektov@warwick.ac.uk>"

#
# Configuration of stripping framework. 
#

from os import environ
from pprint import *
from Gaudi.Configuration import *
from Configurables import StrippingAlg
from StrippingStream import StrippingStream
from StrippingLine import StrippingLine

class StrippingConf ( object ) :

    def __init__( self,
                  name = "",
                  TES = False,
                  TESPrefix = 'Strip', 
                  HDRLocation = 'Phys/DecReports', 
                  Streams = [] ) :
        
        log.info("Initialising StrippingConf "+ name)
        if name == "" :
            self._name = "StrippingGlobal"
        else :
            self._name = name
        self.TES = TES
        self._streams = []
        self._streamSequencers = []
        self._sequence = None
        self._hdrLocation = HDRLocation
        self._tesPrefix = TESPrefix
        for stream in Streams :
            self.appendStream(stream)
	
	allAppended = True
	from StrippingLine import strippingLines
	for line in strippingLines() : 
	    if not line.isAppended() : 
		log.warning("Line " + line.name() + " is declared but not appended to any stream")
		allAppended = False
	
	if allAppended : 
	    log.info("All declared lines are appended to streams")


    def activeStreams (self) :
        """
        Return the list of all active StrippingStreams. 
        """
        return self._streams

    def sequence (self) :
        """
        Return GaudiSequencer containing stream sequencers
        """
        if self._sequence == None :
            from Configurables import GaudiSequencer
            log.info("Initialising GaudiSequencer/"+ self._name)
            self._sequence = GaudiSequencer(self._name,
                                            ModeOR = True, 
                                            ShortCircuit = False,
                                            Members = self._streamSequencers)
        return self._sequence

    def selections (self) :
        """
        Return list of selection names for TagCreator
        """
        _selections = [ self._name ]
        for stream in self._streams : 
            streamName = stream.sequence().name()
            _selections.append(streamName)
            for line in stream.lines :
                _selections.append(line.name())
        return _selections

    def appendStreams(self, streams) :
        """
        Append a list of streams to the configuration.
        """
        for stream in streams :
            self.appendStream(stream)
            
    def appendStream(self, stream) :
        """
        Append a StrippingStream to the configuration. Takes care of adding
        corresponding sequencer to _streamSequencers list.
        """
        log.info(self._name+ " appending stream "+ stream.name())
	stream.createConfigurables( TES = self.TES, 
				    TESPrefix = self._tesPrefix, 
				    HDRLocation = self._hdrLocation )
	self._streams.append(stream)
        self._appendSequencer(stream)
        
    def _appendSequencer(self, stream) :
        self._streamSequencers.append(stream.sequence())




# =============================================================================
# Some useful decorations 
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
_stripping_props_   =      [ 'AcceptFraction'    , 
                   'PercertPass'       ,
                   'L0Channels'        ,
                   'InputSelection'    ,
                   'InputSelection1'   ,
                   'InputSelection2'   ,
                   'InputSelection3'   ,
                   'InputSelections'   ,
                   'OutputSelection'   ,
                   'RecoName'          ,
                   'MatchName'         ,
                   'FilterDescription' ] 

## Get the tuple of major interesting properties for Hlt1  objects
def strippingProps () :
    """
    Get the tuple of major interesting properties for Hlt1  objects

    """
    _stripping_props_.sort() 
    return tuple(_stripping_props_)


# =============================================================================
## Try to promote a string to the corresponding configurable
def string2Configurable( name ) :
    # try to decode it into something reasonable
    (n,t) = (name,None) if name.find('/') == -1 else name.split('/')
    from Gaudi.Configuration import allConfigurables
    cfg = allConfigurables.get(n)
    if cfg :
        if ( t and cfg.getType() == t ) or cfg.getType() == 'ConfigurableGeneric' :
            return cfg
        else :
            print 'got something for %s, don\'t know what to do with %s'%(self,str(cfg))
            return None
    print 'cannot convert %s into a known configurable' % name
    return None

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
    if not lst : lst = strippingProps () 
    elif list  is type(lst) : pass
    elif tuple is type(lst) : pass
    else : lst = [ lst ]
    #
    for item in lst :
        if hasattr ( obj , item ) :
            if l1 < len1( line ) : line += '\n' + l1*' '
            line += "%-15s : %s" % ( item , getattr ( obj , item ) )
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

    if type(self) == str :
        cfg = string2Configurable(self)
        if cfg : self = cfg
    if hasattr ( self , 'sequence' ) :
        return __enroll__ ( self.sequence() , level )
    if hasattr ( self , 'sequencer' ) :
        return __enroll__ ( self.sequencer() , level )

    _tab = 50
    _indent_ = ('%-3d'%level) + level * '   ' 
    try:     line = _indent_ + self.name ()
    except:  line = _indent_ + ( self if type(self) == str else '<UNKNOWN>' )
        
    if len1(line)>( _tab-1): line +=  '\n'+ _tab*' '
    else :                   line +=  (_tab-len1(line))*' '
    try:                     line +=  '%-25.25s'%self.getType()
    except:                  line +=  '<UNKNOWN>'

    line = prnt ( self , lst , line, l1 = _tab+25 ) + '\n'

    # use the recursion 
    if hasattr ( self , 'Members' ) :
        for _m in getattr(self,'Members') : line += __enroll__ ( _m , level + 1 , lst ) 

    if type(self) is StrippingAlg :
        for i in [ 'Prescale','ODIN','L0DU','HLT','Filter','Postscale' ] :
            if hasattr(self,i) : line += __enroll__( getattr(self,i), level + 1, lst )

    if type(self) is StrippingConf : 
	lines = 0
	for i in self.activeStreams() : 
	    lines += len(i.sequence().Members)
	line += "Summary: " + len(self.sequence().Members) + " streams, " + lines + " lines\n"

    return line

for conf in [ StrippingAlg, StrippingStream, StrippingConf] :
    if conf :
        conf.__str__ = __enroll__ 
