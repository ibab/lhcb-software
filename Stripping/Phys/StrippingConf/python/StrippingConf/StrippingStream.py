#!/usr/bin/env python

#
# StrippingStream
#

__all__ = ( 
	    'StrippingStream'
	  )

from Gaudi.Configuration import *
from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable
from Configurables import LHCbConfigurableUser
from copy import copy

class StrippingStream ( object ) :

    def __init__ ( self,
                   name = 'StrippingStream',
                   Lines =  [],                                    # List of stream lines
                   BadEventSelection = "Override",                 # Bad event selection algo
                                                                   # By default, it will be 
                                                                   # overridden in StrippingConf, 
                                                                   # but can be set to None here
                                                                   # to cancel bad event check

                   AcceptBadEvents = None,                         # If None, will be overridden 
                                                                   # in StrippingConf
		   MaxCandidates = "Override", 
		   MaxCombinations = "Override", 
		   TESPrefix = None
                 ) :

        self._name = name

        self.lines = []
        for line in Lines:
          if line.prescale() > 0:
            self.lines += [line]
          else:
            log.warning('Line '+line.name()+' has prescale <= 0, it will be removed from stream '+self.name())

        for line in Lines : 
    	    line.declareAppended()

        self.algs = []
        self.seq = None
        self.streamLine = None                   # Line with OR of all stream lines, created in createConfigurables()
        self.eventSelectionLine = None           # Line to mark bad events, created in createConfigurables()
        self.goodEventFilter    = None           # filter to reject bad events
        self.BadEventSelection = BadEventSelection
        self.AcceptBadEvents = AcceptBadEvents
        self.MaxCandidates = MaxCandidates
        self.MaxCombinations = MaxCombinations
        self.TESPrefix = TESPrefix               # Prefix for DecReports location, configured when appending stream
        self.HDRLocation = 'Phys/DecReports'     # DecReports location, configured when appending to StrippingConf

    def name(self) :
        return self._name

    def clone(self, name) : 
        return StrippingStream(name, Lines = copy(self.lines), 
	                       BadEventSelection = self.BadEventSelection, 
	                       AcceptBadEvents = self.AcceptBadEvents, 
	                       MaxCandidates = self.MaxCandidates, 
	                       MaxCombinations = self.MaxCombinations)

    def appendLines (self, lines) : 
        for line in lines : 
            self.lines.append(line)
            line.declareAppended()

    def createConfigurables(self) :
        from Configurables import StrippingCheck

        # Create configurables

        if self.TESPrefix == None : self.TESPrefix = self._name

        from StrippingLine import StrippingLine
        # If the BadEventsSelection was not given neither in StrippingConf nor in StrippingStream

        if self.BadEventSelection == "Override" :
            self.BadEventSelection = None

        # Make the line to mark bad events (those satisfying BadEventSelection)

        if self.BadEventSelection != None :

            # create the line that select bad events
            self.eventSelectionLine = StrippingLine("Stream"+self.name()+"BadEvent",
                                                    checkPV = False, 
                                                    algos = [ self.BadEventSelection ] )
            self.eventSelectionLine.createConfigurable( self.TESPrefix, self.HDRLocation )
            self.eventSelectionLine.declareAppended()

            if self.AcceptBadEvents != False :

                # Need to think a bit more on this
                log.error("NOT POSSIBLE TO ACCEPT BAD EVENTS WITH THE CURRENT CONFIGURATION OF STRIPPINGCONF!!!")
                return

            else:

                # Create a filter that negates the bad events (select good events). 
                # This filter will be inserted at the beginning of the filters run by all the lines.
                from Configurables import LoKi__VoidFilter as Filter
                self.goodEventFilter = Filter( "StrippingGoodEventCondition"+self.name() ,
                                               Code      = " ALG_EXECUTED('%s') & ~ALG_PASSED('%s') "
                                               % (self.eventSelectionLine.name(), self.eventSelectionLine.name()) ,
                                               Preambulo = [ "from LoKiHlt.algorithms import *" ] )



        for line in self.lines : 
            if line.MaxCandidates == "Override" : 
                line.MaxCandidates = self.MaxCandidates
            if line.MaxCombinations == "Override" : 
                line.MaxCombinations = self.MaxCombinations

            if self.BadEventSelection != None :
                if not self.AcceptBadEvents != False :
                    # inserting filter for good events inside the lines
                    if not "StrippingGoodEventCondition" in line._members[0].name() :
                      line._members.insert(0,self.goodEventFilter)

            if line.prescale() > 0. : 
                line.createConfigurable( self.TESPrefix, self.HDRLocation )
                log.debug("ADDING configurable " + line.configurable().name() + "to stream " + self.name() )
                self.algs.append(line.configurable())

            else : 
                line.RequiredRawEvents = None #forcing None rawevents to avoid (harmless) errors from the DSTWriter
                log.warning("Line " + line.name() + " has zero prescale, skipping" )

        # Make the line for stream decision (OR of all stream lines)

        linesSeq = GaudiSequencer("StrippingStreamSeq"+self.name(),
                                  ModeOR = True,
                                  #ShortCircuit = False,
                                  OutputLevel = WARNING, 
                                  Members = self.algs)

        self.streamLine = StrippingLine("Stream"+self.name(), checkPV = False, algos = [ linesSeq ] )
        ## I think it is redundant, otherwise if it comes out it is needed
        ## uncomment the following lines
        #if self.BadEventSelection != None :
        #    if not self.AcceptBadEvents != False :
        #        if not "StrippingGoodEventCondition" in self.streamLine._members[0].name() :
        #            self.streamLine._members.insert(0,self.goodEventFilter)
        self.streamLine.createConfigurable( self.TESPrefix, self.HDRLocation )
        self.streamLine.declareAppended()

    def sequence ( self ) :
        if self.seq == None :

            if self.eventSelectionLine == None:
                # We don't care of BadEvents, thus not insert bad event line in the stream sequence
                self.seq = GaudiSequencer("StrippingSequenceStream"+self.name(),
                                          ModeOR = True,
                                          ShortCircuit = False,
                                          OutputLevel = WARNING,
                                          Members =  self.algs + [ self.streamLine.configurable() ] )

            else:

                if self.AcceptBadEvents != False :

                    # Need to think a bit more on this
                    log.error("NOT POSSIBLE TO ACCEPT BAD EVENTS WITH THE CURRENT CONFIGURATION OF STRIPPINGCONF!!!")
                    return None

                else:
                    # since we care of bad events we have to protect the execution of lines and stream-line
                    # inserting them in a sequence with ModeOR = True and ShortCircuit = False.
                    lineSeq = GaudiSequencer("StrippingProtectedSequence"+self.name(),
                                             ModeOR = True,
                                             ShortCircuit = False,
                                             OutputLevel = WARNING,
                                             Members = self.algs + [ self.streamLine.configurable() ] )

                    # We need a badEventSeq with IgnoreFilterPassed = True in order to always run the protected sequence with
                    # lines and stream-lines
                    badEventSeq = GaudiSequencer("StrippingBadEventSequence"+self.name(),
                                                 Members = [ self.eventSelectionLine.configurable() ],
                                                 OutputLevel = WARNING,
                                                 IgnoreFilterPassed = True)

                    # The final sequence is an AND sequence of the badEventSeq and the lines + stream-lines
                    #  algorithms (protected sequence)
                    self.seq = GaudiSequencer("StrippingSequenceStream"+self.name(),
                                              OutputLevel = WARNING,
                                              Members = [ badEventSeq, lineSeq ] )

        return self.seq


    def outputLocations (self) : 
        outputList = []
        for i in self.lines :
            output = i.outputLocation()
            if output : 
                outputList.append(output)
        return outputList


    def filterMembers( self ) : 
        _members = []
        for line in self.lines : 
            lineMembers = line.filterMembers()
            for i in lineMembers : 
                if i not in _members : _members.append(i) 
        return _members


    def getRequiredRawEvents(self) :
        rawEvents = { }
        for line in self.lines : 
            if ( line.RequiredRawEvents != None ) :
                rawEvents[line.name()] = [ "/Event/"+r+"/RawEvent#1" for r in line.RequiredRawEvents ]
        return rawEvents


    def checkRawEventRequests(self) :
        for line in self.lines : 
            if line.RequiredRawEvents != None :
                log.debug("Stream='"+self.name()+"' Line='"+line.name()+
                          "' Requests RawEvents "+str(line.RequiredRawEvents))

    def checkMDSTFlag(self) :
        for line in self.lines :
            if line.MDSTFlag :
                log.debug("Stream='"+self.name()+"' Line='"+line.name()+
                          "' Requests to go to MDST.DST")

    def checkFlavourTagging(self,disableFT=False,verbose=False) :
        for line in self.lines :
            if line._EnableFlavourTagging :
                if verbose : log.debug("Stream='"+self.name()+"' Line='"+line.name()+
                                         "' Requests to run FlavourTagging")
                if disableFT:
                    line._EnableFlavourTagging = False
                    log.warning("Stream='"+self.name()+"' Line='"+line.name()+"' Requests Flavor Tagging but is not assigned to a micro-dst stream, Flavour Tagging has been disabled")

    def checkPVRefit(self) :
        for line in self.lines :
            sel = line.selection()
            #for algo in line._algos:
            if sel != None:
                props = sel.algorithm().getProperties()
                if props.has_key('ReFitPVs'):
                    if props['ReFitPVs'] == True:
                        log.warning("Stream='"+self.name()+"' Line='"+line.name()+
                                    "' Requests to refit the PV")

    def getRelatedInfoLocations(self) : 
        locations = []
        for line in self.lines : 
            if line.RelatedInfoTools != None : 
                for tool in line.RelatedInfoTools : 
                    if 'Locations' in tool.keys() : 
                        locs = tool['Locations']
                        for part,loc in locs.iteritems() : 
                            if loc not in locations : locations += [ loc ]
                    if 'Location' in tool.keys() : 
                        loc = tool['Location']
                        if loc not in locations : locations += [ loc ]
        log.info("RelatedInfoLocations for stream %s: %s" % (self.name(), str(locations)) )
        return locations
