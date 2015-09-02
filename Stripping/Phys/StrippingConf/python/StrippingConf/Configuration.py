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
                  TESPrefix = 'Strip', 
                  HDRLocation = 'Phys/DecReports', 
                  Streams = [],
                  DSTStreams = [],
                  MicroDSTStreams = [],
                  GlobalFlavourTagging = True,
                  BadEventSelection = None, 
                  AcceptBadEvents = True,
                  MaxCandidates = None, 
                  MaxCombinations = None,
                  ActiveMDSTStream = False,
                  Verbose = False ) :
        
        log.info("Initialising StrippingConf "+ name)
        if name == "" :
            self._name = "StrippingGlobal"
        else :
            self._name = name

        self._verbose = Verbose
        self._activeMDST = ActiveMDSTStream
        self._streams = []
        self._streamSequencers = []
        self._sequence = None
        self._tesPrefix = TESPrefix
        self._hdrLocation = HDRLocation
        self.DSTStreams = DSTStreams
        self.MicroDSTStreams = MicroDSTStreams
        self._GlobalFlavourTagging = GlobalFlavourTagging
        self._taggingLocations = []
        self._taggingSeq = None
        self.BadEventSelection = BadEventSelection
        self.AcceptBadEvents = AcceptBadEvents
        self.MaxCandidates = MaxCandidates
        self.MaxCombinations = MaxCombinations

        ## To be uncommented to limit combinatorics for StandardParticles
        if self.MaxCandidates != None or self.MaxCombinations != None:
            self.limitCombForStdParticles()

        ## Forces the limits on all configurables that implement the option...
        if self.MaxCandidates != None or self.MaxCombinations != None:
            self.checkAllForCombLimit()

        self.checkFlavourTagging(Streams)

        linesForFT = []
        if self._GlobalFlavourTagging:
          for stream in Streams:
            for line in stream.lines:
              if line._EnableFlavourTagging: 
                line._EnableFlavourTagging = False
                linesForFT += [ line ]

        for stream in Streams :
            self.appendStream(stream)

	# Global FT locations have to be filled after appending streams, 
	# because outputLocations of lines can be redefined
        if self._GlobalFlavourTagging:
          for line in linesForFT:
            self._taggingLocations += [ line.outputLocation().replace("/Particles","") ]

        if self._GlobalFlavourTagging and self._taggingLocations != []:
            self.appendFlavourTagging()

        self.checkAppendedLines()
        self.checkUniqueOutputLocations()

        if self._verbose:
          self.checkRawEventRequests()
          self.checkMDSTFlag()
       
        if self._activeMDST:
            mdstLines = [ line for line in self.activeLines(self.MicroDSTStreams) if line.MDSTFlag ]
            if self._verbose:
                mdstLinesNames = [ line.name() for line in mdstLines ]
                log.warning("The lines going to MDST.DST are")
                print mdstLinesNames
                
            if mdstLines != []:
              mdstStream = StrippingStream( "MDST", Lines = mdstLines )
              self.appendStream(mdstStream)
            else:
              log.warning("No line has been selected to go to the MDST stream, thus it will be skipped")
 
        from Gaudi.Configuration import appendPostConfigAction
        appendPostConfigAction ( defaultToolConfigCheck )

    def checkRawEventRequests(self) :
        for stream in self.activeStreams() : stream.checkRawEventRequests()

    def checkMDSTFlag(self) :
        for stream in self.activeStreams() : stream.checkMDSTFlag()

    def checkFlavourTagging(self,streams) :
        for stream in streams : 
          if stream.name() in self.DSTStreams : stream.checkFlavourTagging(disableFT=True,verbose=self._verbose)
          elif stream.name() in self.MicroDSTStreams : stream.checkFlavourTagging(disableFT=False,verbose=self._verbose)
          else : stream.checkFlavourTagging(disableFT=False,verbose=self._verbose)

    def checkAppendedLines (self) : 
        """
        Check if all declared lines are appended to streams
        """
        allAppended = True
        from StrippingLine import strippingLines
        for line in strippingLines() : 
            if not line.isAppended() : 
                log.warning("Line " + line.name() + " is declared but not appended to any stream")
                allAppended = False
	
        if allAppended : 
            log.info("All declared lines are appended to streams")

    def checkUniqueOutputLocations (self) : 
        """
        Check if the output locations of all active lines are unique
        """
        log.info("Checking uniqueness of output locations")
        import sys
        locations = {}
        for stream in self.activeStreams() :
            for line in stream.lines :
                location = line.outputLocation()
                if location and location != '' :
                    if location not in locations.keys() : 
                        locations[location] = [ (line.name(),stream.name()) ]
                    else : 
                        locations[location] += [ (line.name(),stream.name()) ]
        locationsUnique = True
        locationsUniqueSameStream = True
        message = ''
        messagesamestream = ''
        for loc, nameandstream in locations.iteritems() : 
            if len(nameandstream) > 1 :
                innermessage = ''
                innerlines = {}
                for ns in nameandstream:
                    if innermessage=='':
                        innermessage=str(ns[0])+"(stream "+str(ns[1])+")"
                    else:
                        innermessage+=" ,"+str(ns[0])+"(stream "+str(ns[1])+")"
                    if ns[1] not in innerlines.keys() :
                        innerlines[ns[1]] = [ns[0]]
                    else:
                        innerlines[ns[1]] += [ ns[0] ]
                message += "\tLines "+innermessage+" share the same output location '"+loc+"'\n"
                #sys.stderr.write(message)
                locationsUnique = False

                #print innerlines
                
                for stinner, linner in innerlines.iteritems() :
                    if len(linner) > 1 :
                        inmsg = ''
                        for inl in linner:
                            if inmsg=='':
                                inmsg=str(inl)+"(stream "+stinner+")"
                            else:
                                inmsg=" ,"+str(inl)+"(stream "+stinner+")"
                        messagesamestream+="\tLines "+inmsg+" share the same output location '"+loc+"' and the same stream '"+stinner+"! Please remove duplicates in configuration'\n"
                        locationsUniqueSameStream = False
                        
                    
                    
        if not locationsUnique : 
            #raise Exception('\n' + message)
            log.warning('\n' + message)

        if not locationsUniqueSameStream :
            #raise Exception('\n' + message)
            raise Exception('\n' + messagesamestream)


    def activeStreams (self) :
        """
        Return the list of all active StrippingStreams. 
        """
        return self._streams

    def activeLines (self,selStream = [] ) :
        """
        Return the list of all stripping lines 
        """
        streams = self.activeStreams()
        lines = []
        for stream in streams:
          if selStream != [] and stream.name() in selStream:
            for line in stream.lines:
              lines.append(line)
          elif selStream == []:
            for line in stream.lines:
              lines.append(line)
    
        return lines

    def sequence (self) :
        """
        Return GaudiSequencer containing stream sequencers
        """
        from Configurables import GaudiSequencer
        log.debug("Initialising GaudiSequencer/"+ self._name)
        if self._sequence == None :
            log.debug("Initialising GaudiSequencer/"+ self._name)
            selSeq = GaudiSequencer(self._name+"_Selection",
                                    ModeOR = True, 
                                    ShortCircuit = False,
                                    OutputLevel = WARNING, 
                                    Members = self._streamSequencers)

            self._sequence = GaudiSequencer(self._name,Members = [selSeq])

            if self._GlobalFlavourTagging and self._taggingLocations != []:
                self._sequence.Members += [self._taggingSeq]

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
        
        if stream.TESPrefix == None : 
            stream.TESPrefix = self._tesPrefix
        stream.HDRLocation = self._hdrLocation
        if stream.BadEventSelection == "Override" :
            if isinstance(self.BadEventSelection,dict):
              if stream.name() in self.BadEventSelection.keys():
                stream.BadEventSelection = self.BadEventSelection[stream.name()]
              else:
                stream.BadEventSelection = self.BadEventSelection['default']
              log.info(self._name+ " setting BadEventSelection for stream "+ stream.name())
            else:
              stream.BadEventSelection = self.BadEventSelection
        if stream.AcceptBadEvents == None :
            stream.AcceptBadEvents = self.AcceptBadEvents
 
        if stream.MaxCandidates == "Override" : 
    	    stream.MaxCandidates = self.MaxCandidates
        if stream.MaxCombinations == "Override" : 
    	    stream.MaxCombinations = self.MaxCombinations
        
        stream.createConfigurables()
        self._streams.append(stream)
        self._appendSequencer(stream)

    def appendFlavourTagging(self) :
        from Configurables import BTagging, GaudiSequencer
        btag = BTagging( "BTag_Global",Inputs = self._taggingLocations )
        self._taggingSeq = GaudiSequencer("BTaggingSequence", Members = [btag])
        
    def _appendSequencer(self, stream) :
        self._streamSequencers.append(stream.sequence())

    def decReportLocations(self) : 
        locs = []
        for stream in self._streams : 
            for line in stream.lines : 
                loc = line.decReportLocation()
                if loc != None and loc not in locs : locs.append(loc)

        return locs

    def limitCombForStdParticles(self,incidentName = 'ExceedsCombinatoricsLimit'):
        from StandardParticles import locations as stdLoc
        for loc,alg in stdLoc.iteritems():
            if hasattr(type(alg),'StopAtMaxCandidates') and \
               hasattr(type(alg),'MaxCandidates') and \
               hasattr(type(alg),'StopAtMaxCombinations') and \
               hasattr(type(alg),'MaxCombinations'):
                if self.MaxCandidates != None:
                    alg.StopAtMaxCandidates = True
                    alg.MaxCandidates = self.MaxCandidates
                if self.MaxCombinations != None:
                    alg.StopAtMaxCombinations = True
                    alg.MaxCombinations = self.MaxCombinations
                if self.MaxCandidates != None or self.MaxCombinations != None :
                    alg.StopIncidentType = incidentName
                ## comment to be removed once performed some more test
                #if self._verbose: log.warning('Changed parameters StopAtMaxCandidates = '+str(alg.StopAtMaxCandidates)+' MaxCandidates = '+str(alg.MaxCandidates)+' StopIncidentType = '+str(alg.StopIncidentType)+' for algorithm '+str(alg.name()))


    # Loops over all known configurables and applies MaxCandidate limits
    def checkAllForCombLimit(self) :

        from Gaudi.Configuration import allConfigurables
        
        for conf in allConfigurables.values() :
            
            if ( hasattr(type(conf),'StopAtMaxCandidates') and
                 hasattr(type(conf),'MaxCandidates') and 
                 hasattr(type(conf),'StopAtMaxCombinations') and
                 hasattr(type(conf),'MaxCombinations') ):

                if self.MaxCandidates != None and not conf.isPropertySet("MaxCandidates") :
                    log.debug( "Forcing MaxCandidate settings for " + conf.name() )
                    conf.StopAtMaxCandidates = True
                    conf.MaxCandidates = self.MaxCandidates
                    
                if self.MaxCombinations != None and not conf.isPropertySet("MaxCombinations") :
                    log.debug( "Forcing MaxCombinations settings for " + conf.name() )
                    conf.StopAtMaxCombinations = True
                    conf.MaxCombinations = self.MaxCombinations

                if self.MaxCandidates != None or self.MaxCombinations != None :
                    conf.StopIncidentType = 'ExceedsCombinatoricsLimit'
                    
            elif ( hasattr(type(conf),'MaxParticles') and
                   hasattr(type(conf),'StopIncidentType') ) :

                if self.MaxCandidates != None and not conf.isPropertySet("MaxParticles") :
                    log.debug( "Forcing MaxParticles settings for " + conf.name() )
                    conf.MaxParticles = self.MaxCandidates
                    conf.StopIncidentType = 'ExceedsCombinatoricsLimit'


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
        for i in [ 'Prescale','ODIN','L0DU','HLT','Filter1','Postscale' ] :
            if hasattr(self,i) : line += __enroll__( getattr(self,i), level + 1, lst )

    if type(self) is StrippingConf : 
        lines = 0
        for i in self.activeStreams() : 
            lines += len(i.sequence().Members)
        line += "Summary: " + len(self.sequence().Members) + " streams, " + lines + " lines\n"

    return line

def defaultToolConfigCheck () : 
    from Gaudi.Configuration import allConfigurables
    
    log.info('Checking if public tools have default configuration')

    for conf in allConfigurables.values() : 
        if conf.name().find('ToolSvc.')>=0 : 
            difference = []
            if isinstance(type(conf).__slots__,dict) : 
                for k,v in type(conf).__slots__.iteritems() : 
                    if hasattr(conf, k) : 
                        val = getattr(conf,k)
                        if val not in allConfigurables.values() : 
                            if v != val : 
                                difference.append( (k, v, val ) )
            if len(difference) > 0 : 
                log.warning( 'tool %s has non-default configuration! Attributes that differ:' % conf.name() )
                for d in difference : 
                    log.warning( '    %s = %s (default = %s)' % ( d[0], str(d[2]), str(d[1]) ) )


for conf in [ StrippingAlg, StrippingStream, StrippingConf] :
    if conf :
        conf.__str__ = __enroll__ 

