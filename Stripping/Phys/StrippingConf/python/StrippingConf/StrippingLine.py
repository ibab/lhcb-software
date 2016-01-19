#!/usr/bin/env python

#
# StrippingLine
#

__all__ = (
	    'StrippingLine',
	    'strippingLines'
	  )

# cached related info information
relatedInfoCache = { }

def makePropKey(props):
    return hash(repr(sorted(props.items())))

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

## Convention: the name of 'HLT1Filter' algorithm inside StrippingLine
def hlt1entryName    ( line, level = 'Stripping' ) :
    """ Convention: the name of 'HLT1Filter' algorithm inside StrippingLine """
    return '%s%sHlt1Filter'   % (level,line)

## Convention: the name of 'HLT2Filter' algorithm inside StrippingLine
def hlt2entryName    ( line, level = 'Stripping' ) :
    """ Convention: the name of 'HLT2Filter' algorithm inside StrippingLine """
    return '%s%sHlt2Filter'   % (level,line)

def decisionName   ( line, level = 'Stripping'  ) :
    """Convention: the name of 'Decision' algorithm inside StrippingLine"""
    return level + '%sDecision'   % line if line != 'Global' else level+'Global'

_protected_ = ( 'IgnoreFilterPassed' , 'Members' , 'ModeOR', 'DecisionName', 'Prescale','Postscale','Filter1' )

# Own slots of StrippingLine
_myslots_   = ( 'name' , 'prescale'  , 'postscale' , 'ODIN', 'L0DU', 'HLT' , 'HLT1', 'HLT2', 'algos' )


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
    Simple class to represent a set of StrippingLine members which are bound to a line
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




# =============================================================================
## @class StrippingLine
class StrippingLine(object):

    def __init__ ( self             ,
                   name             ,   # the base name for the Line
                   prescale  = 1    ,   # prescale factor
                   ODIN      = None ,   # ODIN predicate
                   L0DU      = None ,   # L0DU predicate
                   HLT       = None ,   # HltDecReports predicate  -> Deprecated since 2015
                   HLT1      = None ,   # Hlt1DecReports predicate
                   HLT2      = None ,   # Hlt2DecReports predicate
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
                   ExtraInfoSelections = None,   # Input selections for ExtraInfo tools. If None, use the top-level selection of the line
                   ExtraInfoDaughters = None,    # Daughter selections for which store ExtraInfo. If None, use only the top selection.
                   ExtraInfoRecursionLevel = 1,  # Maximum depth in the decay tree to calculate ExtraInfo
                                                 # Only used is ExtraInfoDaughters are given, otherwise is 0

                   RelatedInfoTools = None,      # Configuration of Related Info tools, as a list of dictionaries (or None)
                   RelatedInfoFilter = None,     # Optional filter which can use RelatedInfo, added to the line sequence
                                                 # after RelatedInfoTools

                   RequiredRawEvents = None,     # Possible list of RawEvent banks required by this line
                   MDSTFlag          = False,     # Flag to ask the line to be written to MDST.DST stream
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
        HLT1   = deepcopy ( HLT1    )
        HLT2   = deepcopy ( HLT2    )
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
        self._HLT1      = HLT1
        self._HLT2      = HLT2
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
        self.ExtraInfoSelections = ExtraInfoSelections
        self.ExtraInfoDaughters = ExtraInfoDaughters
        self.ExtraInfoRecursionLevel = ExtraInfoRecursionLevel

        self.RelatedInfoTools = RelatedInfoTools
        self.RelatedInfoFilter = RelatedInfoFilter

        self._initialSelection = selection

        validRawBanks = ["Trigger","Muon","Calo","Rich","Velo","Tracker","HC"] # hard coded list, should really come from elsewhere....
        if RequiredRawEvents != None :
            for bank in RequiredRawEvents :
                if bank not in validRawBanks :
                    raise Exception("RawBank "+bank+" is not a known type")
        self.RequiredRawEvents = RequiredRawEvents

        self.MDSTFlag = MDSTFlag

        line = self.subname()

        self._appended  = False

        # Configurable is not yet created
        self._configurable = None

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

    def selectionsToLocations(self, selList) :
        locList = []
        for sel in selList :
            if type(sel).__name__ == 'Selection' or type(sel).__name__ == 'MergedSelection' :
                # Need to check if the selection is the top selection
                # In that case use line's output location because the
                # name of the top algoritm is redefined by the framework
                if sel == self._initialSelection :
                    fullPath = "/Event/" + self.outputLocation()
                else :
                    fullPath = "/Event/" + sel.outputLocation()
                locList += [ fullPath ]
                    #print "Added outputlocation %s to ExtraInfo in line %s" % (fullPath, self.name() )
            else :
                raise AttributeError, "Storing ExtraInfo is not supported for selection of type '%s' (in line %s)" % \
                      (type(sel).__name__, self.name() )
        return locList

    def createConfigurable( self, TESPrefix = "Strip", HDRLocation = 'Phys/DecReports' ) :

        if self._configurable != None : 
            log.info("Configurables already created for line %s, skipping" % self.subname() )
            return self._configurable

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

        if self._ODIN :
            if isinstance   ( self._ODIN , str   ) :
                mdict.update( { 'ODIN'    : ODINFilter ( odinentryName ( line ) , Code = self._ODIN   )  } )
            elif isinstance ( self._ODIN , ( tuple , list ) ) and 2 == len ( self._ODIN ) :
                mdict.update( { 'ODIN'    : ODINFilter ( odinentryName ( line ) , Code = self._ODIN[0],  Preambulo = self._ODIN[1] )  } )
            elif isinstance ( self._ODIN , dict     ) :
                mdict.update( { 'ODIN'    : ODINFilter ( odinentryName ( line ) , **self._ODIN  )  } )
            else :
                raise TypeError, "Wrong ODIN attribute: %s " % self._ODIN

        if self._L0DU   :
            if isinstance   ( self._L0DU , str   ) :
                mdict.update( { 'L0DU'    : L0Filter   ( l0entryName   ( line ) , Code = self._L0DU   )  } )
            if isinstance   ( self._L0DU , ( tuple, list) ) and 2 == len ( self._L0DU ) :
                mdict.update( { 'L0DU'    : L0Filter   ( l0entryName   ( line ) , Code = self._L0DU[0],  Preambulo = self._L0DU[1]  )  } )
            if isinstance   ( self._L0DU , dict )  :
                mdict.update( { 'L0DU'    : L0Filter   ( l0entryName   ( line ) , **self._L0DU)  } )

        from DAQSys.Decoders import DecoderDB
        Hlt1DecReportsDecoder=DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"].setup()
        Hlt2DecReportsDecoder=DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"].setup()
        if self._HLT :

            log.warning( self.name()+" : The usage of unique HLT is deprecated and will not work on data taken from 2015 onward. Please move to use HLT1 and HLT2.")

            if isinstance( self._HLT , str   ) :
                mdict.update( { 'HLT' : HDRFilter( hltentryName( line ), Code = self._HLT ) } )
            if isinstance   ( self._HLT , ( tuple, list) ) and 2 == len ( self._HLT ) :
                mdict.update( { 'HLT' : HDRFilter( hltentryName( line ), Code = self._HLT[0],  Preambulo = self._HLT[1] ) } )
            if isinstance   ( self._HLT , dict )  :
                mdict.update( { 'HLT' : HDRFilter( hltentryName( line ) , **self._HLT) } )

        if self._HLT1 :
            if isinstance( self._HLT1, str ) :
                mdict.update( { 'HLT1' : HDRFilter( hlt1entryName( line ),
                                                    Code = self._HLT1,
                                                    Location = Hlt1DecReportsDecoder.OutputHltDecReportsLocation ) } )
            if isinstance( self._HLT1, ( tuple, list) ) and 2 == len ( self._HLT1 ) :
                mdict.update( { 'HLT1' : HDRFilter( hlt1entryName( line ), 
                                                    Code = self._HLT1[0],
                                                    Location = Hlt1DecReportsDecoder.OutputHltDecReportsLocation,
                                                    Preambulo = self._HLT1[1] ) } )
            if isinstance( self._HLT1 , dict )  :
                mdict.update( { 'HLT1' : HDRFilter( hlt1entryName( line ), **self._HLT1) } )

        if self._HLT2 :
            if isinstance( self._HLT2 , str ) :
                mdict.update( { 'HLT2' : HDRFilter( hlt2entryName( line ), 
                                                    Code = self._HLT2,
                                                    Location = Hlt2DecReportsDecoder.OutputHltDecReportsLocation ) } )
            if isinstance( self._HLT2, ( tuple, list) ) and 2 == len ( self._HLT2 ) :
                mdict.update( { 'HLT2' : HDRFilter( hlt2entryName( line ), 
                                                    Code = self._HLT2[0],  
                                                    Location = Hlt2DecReportsDecoder.OutputHltDecReportsLocation,
                                                    Preambulo = self._HLT2[1] ) } )
            if isinstance( self._HLT2 , dict )  :
                mdict.update( { 'HLT2' : HDRFilter( hlt2entryName( line ), **self._HLT2) } )


	# Add extra info tools if needed
        if self.ExtraInfoTools :
            from Configurables import AddExtraInfo
            extraInfoAlg = AddExtraInfo('ExtraInfo_' + self.name())
            if self.ExtraInfoSelections :
                extraInfoAlg.Inputs = self.selectionsToLocations( self.ExtraInfoSelections )
            else :
                extraInfoAlg.Inputs = [ self.outputLocation() ]
            if self.ExtraInfoDaughters :
                extraInfoAlg.MaxLevel = self.ExtraInfoRecursionLevel
                extraInfoAlg.DaughterLocations = self.selectionsToLocations( self.ExtraInfoDaughters )
            else :
                extraInfoAlg.MaxLevel = 0

            toolNames = []
            toolNum = 0

            for itool in self.ExtraInfoTools :
                toolNum += 1
                toolType = itool["Type"]
                toolName = "Tool%d" % toolNum
                module = __import__("Configurables", globals(), locals(), [ toolType ] )
                toolClass = getattr( module, toolType )
                extraInfoAlg.addTool( toolClass, toolName )
                toolInstance = getattr( extraInfoAlg, toolName )
                for property,value in itool.iteritems() :
                    if property == "Type" : continue
                    setattr( toolInstance, property, value)
                toolNames += [ toolType + '/' + toolName ]
            extraInfoAlg.Tools = toolNames
            self._members.append(extraInfoAlg)

        if self.RelatedInfoTools != None :
            self.addRelatedInfo()
            if self.RelatedInfoFilter :
                if hasattr(type(self.RelatedInfoFilter), "CloneFilteredParticles") : 
                    log.debug("Setting CloneFilteredParticles = True for RelatedInfoFilter %s " % self.RelatedInfoFilter.name() )
                    self.RelatedInfoFilter.CloneFilteredParticles = True
                self._members.append( self.RelatedInfoFilter )
                oldOutput = self.outputLocation()
                self._outputloc = "Phys/" + self.RelatedInfoFilter.name() + "/Particles"
                log.debug( 'Redefined OutputLocation for line '+ self.name()+ ' from '+ oldOutput+ ' to '+ self._outputloc )
                self.addRelatedInfo()

        # Add flavour tagging tool to the end of line sequence if needed
        if self._EnableFlavourTagging :
            if not self.outputLocation() or self.outputLocation() == "" :
                raise AttributeError, "Line %s does not have output, cannot do flavour tagging" % self.name()
            from Configurables import BTagging
            btag = BTagging("BTag_"+self.name(), Inputs = [ self.outputLocation() ] )
            self._members.append(btag)

        if self._members :

            filterSeq = GaudiSequencer( filterName( line,'Stripping' ), 
                                        Members = self._members, 
                                        OutputLevel = WARNING )

            mdict.update( { 'Filter1' : filterSeq })

        #print self._members

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

    # Add related info tools if needed
    def addRelatedInfo( self ) :

        if self.RelatedInfoTools :

            log.debug( "Add RelatedInfo tools for output location "+ self.outputLocation() )

            toolNum = 0
            for itool in self.RelatedInfoTools :

                toolNum += 1

                if 'Locations' in itool.keys() or 'RecursionLevel' in itool.keys() : 
                    raise Exception("\n"+self.name()+": Old-style syntax for RelatedInfoTools is specified. Use \'DaughterLocations\' dictionary instead!\n")

                from Configurables import AddRelatedInfo
                output_basename = self.outputLocation().split("/")[-2]
                relatedInfoAlg = AddRelatedInfo('RelatedInfo%d_%s' % ( toolNum, output_basename ) )
                if 'TopSelection' in itool.keys() : # and 'Locations' in itool.keys() :
                    relatedInfoAlg.Inputs = self.selectionsToLocations( [ itool['TopSelection'] ] )
                else :
                    relatedInfoAlg.Inputs = [ "/Event/" + self.outputLocation() ]

                if 'DaughterLocations' in itool.keys() :
                    relatedInfoAlg.DaughterLocations = itool['DaughterLocations']
                if 'Location' in itool.keys() :
                    relatedInfoAlg.Location = itool['Location']
                if 'DaughterLocations' not in itool.keys() and 'Location' not in itool.keys() :
                    raise Exception('\n'+self.name()+': Neither "Location" nor "DaughterLocations" are defined in RelatedInfo dictionary')
                if 'IgnoreUnmatchedDescriptors' in itool.keys() :
                    relatedInfoAlg.IgnoreUnmatchedDescriptors = itool['IgnoreUnmatchedDescriptors']

                toolType = itool["Type"]

                # Extract the remaining properties specific for the tool from the overall list
                toolprops = { }
                for property,value in itool.iteritems() :
                    if property not in ["Type", "Location", "DaughterLocations", "TopSelection", "IgnoreUnmatchedDescriptors" ] : 
                        toolprops[property] = value

                # make a hashable key from the properties dict
                propkey = makePropKey(toolprops)

                # for debugging
                #relatedInfoAlg.OutputLevel = VERBOSE

                # Get the entry in the cache map for thios type of tool
                toolcache = globals()["relatedInfoCache"]
                if toolType not in toolcache.keys() : toolcache[toolType] = { "ToolNum" : 1, "Tools" : { } }
                configmap = toolcache[toolType]

                # Does an instance with the exact configuration requested already exist ?
                toolName = "UNDEFINED"
                if propkey in configmap["Tools"].keys() :
                    
                    # Just reuse the existing tool
                    toolName = configmap["Tools"][propkey]
                    
                else:
                    # need to configure a new tool

                    # Get instance class type
                    module = __import__("Configurables", globals(), locals(), [ toolType ] )
                    ToolClass = getattr( module, toolType )

                    # Construct global name
                    toolName = toolType+"_"+str(configmap["ToolNum"])
                    
                    # Increment count for next time
                    configmap["ToolNum"] += 1

                    # make an instance owned by tool svc, i.e. public
                    toolInstance = ToolClass( "ToolSvc." + toolName )

                    # set options
                    for property,value in toolprops.iteritems() :
                        setattr( toolInstance, property, value )

                    # Save toolname in the cache map
                    configmap["Tools"][propkey] = toolName

                    # for debugging ...
                    #toolInstance.OutputLevel = 1

                # Add tool to algorithm, as a public tool
                relatedInfoAlg.Tool = toolType + '/' + toolName + ":PUBLIC"

                #print relatedInfoAlg.name(), relatedInfoAlg.Tool

                # add algorithm to the members to run
                self._members.append(relatedInfoAlg)

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
        __HLT1       = deepcopy ( args.get ( 'HLT1'      , self._HLT1      ) )
        __HLT2       = deepcopy ( args.get ( 'HLT2'      , self._HLT2      ) )
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
                               HLT1      = __HLT1       ,
                               HLT2      = __HLT2       ,
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
    elif type(configurable) != CombineParticles and \
         hasattr(type(configurable),'StopAtMaxCandidates') and hasattr(type(configurable),'MaxCandidates') and \
         hasattr(type(configurable),'StopAtMaxCombinations') and hasattr(type(configurable),'MaxCombinations'):
        ### Comments will be removed before the release.
        if MaxCandidates != None:
            configurable.StopAtMaxCandidates = True
            configurable.MaxCandidates = MaxCandidates
            #log.warning('Set StopAtMaxCandidates = '+str(configurable.StopAtMaxCandidates)+
            #           ' and MaxCandidates = '+str(configurable.MaxCandidates)+
            #           ' for algorithm '+configurable.name()+'/'+str(type(configurable)))
        if MaxCombinations != None:
            configurable.StopAtMaxCombinations = True
            configurable.MaxCombinations = MaxCombinations
            #log.warning('Set StopAtMaxCombinations = '+str(configurable.StopAtMaxCombinations)+
            #           ' and MaxCombinations = '+str(configurable.MaxCombinations)+
            #           ' for algorithm '+configurable.name()+'/'+str(type(configurable)))
        if MaxCandidates != None or MaxCombinations != None :
            configurable.StopIncidentType    = incidentName
            #log.warning('Set StopIncidentType = '+str(configurable.StopIncidentType)+
            #           ' for algorithm '+configurable.name()+'/'+str(type(configurable)))
            return True
        else :
            return False
    else:
        #log.warning('Not able to set MaxCandidates and MaxCombinations to algorithm '+str(type(configurable))+'/'+configurable.name())
        return False



