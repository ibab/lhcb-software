"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.140 2010-01-11 14:48:15 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltANNSvc 
from Hlt1                import Hlt1Conf
from Hlt2                import Hlt2Conf

##################################################################################
class HltConf(LHCbConfigurableUser):
    """
    Hlt configuration
    """
    __used_configurables__ = [ Hlt1Conf
                             , Hlt2Conf ]
    __slots__ = { "L0TCK"                          : ''
                , "DataType"                       : '2009'
                , "Verbose"                        : False      # print the generated Hlt sequence
                , "HistogrammingLevel"             : 'None'     # or 'Line'
                , "ThresholdSettings"              : ''         #  select a predefined set of settings, eg. 'Effective_Nominal'
                , "EnableHltGlobalMonitor"         : True
                , "EnableHltDecReports"            : True
                , "EnableHltSelReports"            : True
                , "EnableHltVtxReports"            : True
                , "EnableHltRoutingBits"           : True
                , "EnableLumiEventWriting"         : False
                , "EnableAcceptIfSlow"             : False      # Switch on AcceptIfSlow switch of HltLine
                , 'RequireL0ForEndSequence'        : True
                , 'SkipHltRawBankOnRejectedEvents' : False
                , "LumiBankKillerAcceptFraction"   : 0.9999     # fraction of lumi-only events where raw event is stripped down
                                                                # (only matters if EnablelumiEventWriting = True)
                , "WithMC"                         : False
                , "AdditionalHlt1Lines"            : []         # must be configured
                , "AdditionalHlt2Lines"            : []         # must be configured
                }

    __settings__ = None 
    #import logging
    #_log = logging.getLogger( 'HltConf.Configuration')
    #handler = logging.StreamHandler()
    #formatter = logging.Formatter("# %(name)s - %(levelname)s : %(message)s")
    #handler.setFormatter( formatter )
    #_log.addHandler( handler )
    #_log.setLevel(logging.DEBUG)

##################################################################################
    def defineL0Channels(self, L0TCK = None) :
        """
        Define L0 channels
        """
        if L0TCK :
            log.warning( '###############################################################')
            log.warning( '## WARNING HLT will assume input data contains L0 TCK %s ##' % L0TCK )
            log.warning( '###############################################################')
            importOptions('$L0TCK/L0DUConfig.opts')
            from Configurables import L0DUMultiConfigProvider
            if L0TCK not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
                raise KeyError('requested L0 TCK %s is not known'%L0TCK)
            from Hlt1Lines.HltL0Candidates import decodeL0Channels
            channels = decodeL0Channels( L0TCK )
        else :
            log.warning( '##################################################################################################')
            log.warning( '## WARNING You did not inform the HLT configuration what L0 Configuration is used for the input ##')
            log.warning( '## WARNING Will assume some canonical list of L0 Channels                                       ##')
            log.warning( '##################################################################################################')
            channels = [ 'Muon','DiMuon','Muon,lowMult','DiMuon,lowMult','Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]
        from Hlt1Lines.HltL0Candidates import setupL0Channels
        setupL0Channels( channels ) 

##################################################################################
    def settings(self) :
        """
        Get the class that contains the thresholds, etc
        """
        thresName  = self.getProp('ThresholdSettings')   # the name
        if not thresName : thresName = self.settingsForDataType( self.getProp('DataType') )
        if not thresName : return None
        from HltConf.ThresholdUtils import Name2Threshold
        return Name2Threshold(thresName)

##################################################################################
    def confType(self) :
        """
        Decoding of configuration. This is where Hlt1 and 2 configurations are called.
        """
        #
        # set thresholds if you can
        #
        ThresholdSettings = {}
        thresClass = self.settings()
        if ( thresClass != None ):
            ThresholdSettings = thresClass.Thresholds()
        else :
            log.warning( '##################################################################' )
            log.warning( '## WARNING You are running the HLT with no defined thresholds   ##' )
            log.warning( '## WARNING You will get the default cuts and all lines          ##' )
            log.warning( '## WARNING Set a ThresholdSetting to get something well defined ##' )
            log.warning( '## ###############################################################' )


        ## what L0 configuration are we running on top of?
        L0TCK = None
        if thresClass : L0TCK = thresClass.L0TCK()
        if self.getProp('L0TCK') :
            if L0TCK != self.getProp('L0TCK') :
                log.warning( '##############################################################################' )
                log.warning( '## WARNING You are configuring the HLT to run on top of an L0 configuration ##' )
                log.warning( '## WARNING which is different then the one it was generated for             ##' )
                log.warning( '##############################################################################' )
            L0TCK = self.getProp('L0TCK')
        self.defineL0Channels( L0TCK )
        #
        #  main HLT sequencer
        # 
        Hlt = Sequence('Hlt', ModeOR= True, ShortCircuit = False
                       , Members = 
                       [ Sequence('Hlt1') 
                       , Sequence('Hlt2')
                       , Sequence('HltEndSequence') 
                       ] )

        #
        # dispatch Hlt1 configuration
        #
        Hlt1Conf()
        Hlt1Conf().ThresholdSettings = ThresholdSettings
        #
        # dispatch Hlt2 configuration
        #
        if not thresClass or thresClass.ActiveHlt2Lines() :
            Hlt2Conf()
            self.setOtherProps(Hlt2Conf(),[ "DataType" ])
            Hlt2Conf().ThresholdSettings = ThresholdSettings
            Hlt2Conf().WithMC = self.getProp("WithMC")

#########################################################################################
# Utility function for setting thresholds both in Hlt1 and 2
#
    def settingsForDataType( self, x ) :
        """
        Defaults per datatype
        """
        _dataType2Settings = { 'DC06' : None ,  # development is default
                               'MC09' : None }  # development is default
        return _dataType2Settings[x] if x in _dataType2Settings else None
    
##################################################################################
    def configureRoutingBits(self) :
        """
        Routing bits
        """
        ## set triggerbits
        #  0-15: reserved for ODIN  // need to add ODIN support to routing bit writer
        # 16-31: reserved for L0    // need to add L0DU support to routing bit writer
        # 32-63: reserved for Hlt1
        # 64-91: reserved for Hlt2

        ### NOTE: any change in the _meaning_ of any of the following needs to be 
        ###       communicated (at least!) with online, to insure the events are 
        ###       still properly routed!!! (this goes esp. for [32,36]!!!)
        ### NOTE: current usage:
        ###       bit 46 -> 'physics triggers'
        ###       bit 37 -> subscrided to be Hlt monitoring
        ###       bit 36 -> express stream
        ###       bit 35 -> subscribed to by Velo closing monitoring
        ###       bit 34 -> count number of 'non-lumi-exlusive events'
        ###       bit 33 -> lumi stream
        ###       bit 32 -> full stream (actually, not used for that, but it could be ;-)
        routingBits = { 32 : "HLT_PASS('Hlt1Global')"
                      , 33 : "HLT_PASS_SUBSTR('Hlt1Lumi')" 
                      , 34 : "HLT_PASS_RE('Hlt1(?!Lumi).*Decision')"  # note: we need the 'Decision' at the end to _exclude_ Hlt1Global
                      , 35 : "HLT_PASS_SUBSTR('Hlt1Velo')"  
                      , 36 : "scale(HLT_PASS_RE('Hlt1(?!Lumi).*Decision'), RATE(1)) | HLT_PASS('Hlt2UnbiasedJPsiDecision') " # for now, flag 1 Hz of 'physics events' for express stream, and all J/psi
                      , 37 : "HLT_PASS('Hlt1ODINPhysicsDecision')"
                      , 38 : "HLT_PASS('Hlt1ODINTechnicalDecision')"
                      , 39 : "HLT_PASS_SUBSTR('Hlt1L0')"
                      , 40 : "HLT_PASS_RE('Hlt1.*Hadron.*Decision')"
                      , 41 : "HLT_PASS_RE('Hlt1.*SingleMuon.*Decision')"
                      , 42 : "HLT_PASS_RE('Hlt1.*DiMuon.*Decision')"
                      , 43 : "HLT_PASS_RE('Hlt1.*MuTrack.*Decision')"
                      , 44 : "HLT_PASS_RE('Hlt1.*Electron.*Decision')"
                      , 45 : "HLT_PASS_RE('Hlt1.*Pho.*Decision')"
                      , 46 : "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi).*Decision')"    # I don't like these
                      # 64--96: Hlt2
                      , 64 : "HLT_PASS('Hlt2Global')"
                      , 65 : "HLT_PASS('Hlt2DebugEventDecision')"
                      , 66 : "HLT_PASS_RE('Hlt2(?!Transparent).*Decision')"
                      }

        ## and record the settings in the ANN service
        HltANNSvc().RoutingBits = dict( [ (v,k) for k,v in routingBits.iteritems() ] )
        from Configurables import HltReferenceRateSvc
        rsvc = HltReferenceRateSvc( ReferenceRate = 80
                                  , ODINPredicate = '( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger )'
                                  )
        # add explicitly as ExtSvc to avoid being triggered from within HltRoutingBitsWriter's decode,
        # which implies we start to decode the above ODINPredicate at the time one of HltRoutingBitsWriter's 
        # predicates is being decoded -- the hybrid factory doesn't like going recursive!
        ApplicationMgr().ExtSvc.append( rsvc ) 
        # LoKi::Hybrid::HltFactory is what RoutingBitsWriter uses as predicate factory..
        # make sure 'strings' is known... 
        # make sure 'RATE,SCALE and SKIP' are known...
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        HltFactory('ToolSvc.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions', 'LoKiNumbers.sources' ]
        #  forward compatibility: HltFactory will become private for HltRoutingBitsWriter...
        HltFactory('HltRoutingBitsWriter.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions', 'LoKiNumbers.sources' ]
        # and, last but not least, tell the writer what it should write..
        from Configurables import HltRoutingBitsWriter
        HltRoutingBitsWriter().RoutingBits = routingBits

##################################################################################
    def configureVertexPersistence(self,lines) :
        """
        persistify vertices
        """
        ## and persist some vertices...
        selections = []
        from HltLine.HltLine     import hlt1Lines
        for i in hlt1Lines() :
               if i.name() in lines : selections.extend( [ j for j in i.outputSelections() if j not in selections ] )
        vertices = [ i for i in selections if i is 'PV2D' or   ( i.startswith('Hlt1Velo') and i.endswith('Decision') ) ]
        from Configurables import HltVertexReportsMaker        
        HltVertexReportsMaker().VertexSelections = vertices
        HltVertexReportsMaker().Context = "HLT"
        ## do not write out the candidates for the vertices we store 
        from Configurables import HltSelReportsMaker
        HltSelReportsMaker().SelectionMaxCandidates.update( dict( [ (i,0) for i in vertices if i.endswith('Decision') ] ) )
        veto = [ 'TES:Trig/L0/FullCalo' ,   'TES:Trig/L0/MuonCtrl'
               , 'TES:Hlt/Vertex/ASidePV3D','TES:Hlt/Vertex/CSidePV3D' , 'TES:Hlt/Track/Long', 'TES:Hlt/Track/Forward',   'TES:Hlt/Track/RZVelo',    'TES:Hlt/Track/Velo'
               , 'TES:Hlt/Vertex/PV2D' 
               , 'TES:Hlt/Track/MuonSegmentForL0Single'
               , 'RZVeloBW'
               ]
        HltSelReportsMaker().SelectionMaxCandidatesDebug = dict( [ (i,0) for i in veto ] )

##################################################################################
    def configureANNSelections(self) :
        """
        Assigned numbers configuration
        """

        def updateDict( d, id, entries ) :
             # pick up first unused entry after 'id'
             for i in entries :
                 while id in d.values() : id = id + 1
                 d.update({ i : id } )

        ### TODO: use the computed indices available from the lines...
        ### TODO: what about shared selections??? (which appear with multiple indices!)
        ###       but which have names not prefixed by the line name
        ### Make sure that the ANN Svc has everything it will need
        from HltLine.HltLine     import hlt1Selections
        missing = [ i for i in sorted(set(hlt1Selections()['All']) - set(HltANNSvc().Hlt1SelectionID.keys())) if not i.startswith('TES:') ]
        missingDecisions  = [ i for i in missing if i.endswith('Decision') ]
        updateDict( HltANNSvc().Hlt1SelectionID, 1000, missingDecisions )
        missingSelections = [ i for i in missing if not i.endswith('Decision') ]
        updateDict( HltANNSvc().Hlt1SelectionID, 11000, missingSelections )
        log.warning( '# added ' + str(len(missingSelections)) + ' selections to HltANNSvc' )
        log.warning( '# added ' + str(len(missingDecisions))  +  ' decisions to HltANNSvc' )

        if False :
            from HltLine.HltLine     import hlt1Lines
            for i in hlt1Lines() :
                print ( "checking " + i.name() )
                decisionName = i.name() + 'Decision'
                if decisionName in HltANNSvc().Hlt1SelectionID :
                    id = HltANNSvc().Hlt1SelectionID[ decisionName ] 
                else :
                    id = None

                if id :
                    print ( i.index() )
                    for (key,value ) in zip(i.index(),range(0,len(i.index()))) :
                        from HltLine.HltLine     import hlt1Selections
                        if key in hlt1Selections()['All'] :
                            # TODO: see if the selection is unique to this line...
                            unique = key.startswith(i.name())
                            cur = HltANNSvc().Hlt1SelectionID[ key ] if key in HltANNSvc().Hlt1SelectionID else  None
                            print( ' selection %s in line %s (unique:%s) should have ID %d:%d -- has %d' % ( key,  i.name(), unique, id, value, cur)  )
                        #else :
                        #    print ' line %s, algo %s does not have a selection? ' % (i.name(),key)
                else :
                    log.warning( 'Hlt1Line %s not known to ANNSvc??' % i.name() )



##################################################################################
    def configureHltMonitoring(self, lines) :
        """
        Hlt Monitoring
        @todo That's all Hlt1. Shouldn't it go there ?
        """
        import re
        ## and tell the monitoring what it should expect..
        from Configurables import HltGlobalMonitor
        HltGlobalMonitor().Hlt1Decisions = [ i.decision() for i in lines ]
       
        # the keys are the Labels for the Histograms in the GUI
        # the values are the Pattern Rules to for the Decisions contributing 
        alley_string_patterns = [ ("L0"         , "Hlt1L0.*Decision"),
                                  ("Lumi"       , "Hlt1Lumi.*Decision"),
                                  ("Velo"       , "Hlt1Velo.*Decision"),
                                  ("XPress"     , "Hlt1.*XPress.*Decision"),
                                  ("Hadron"     , "Hlt1.*Hadron.*Decision"),
                                  ("SingleMuon" , "Hlt1.*SingleMuon.*Decision"),
                                  ("DiMuon"     , "Hlt1.*DiMuon.*Decision"),
                                  ("MuTrack"    , "Hlt1.*MuTrack.*Decision"),
                                  ("Electron"   , "Hlt1.*Electron.*Decision"),
                                  ("Photon"     , "Hlt1.*Pho.*Decision"),
                                  ("PA"         , "Hlt1(ODIN.*|Tell1Error)Decision"),
                                  ("Global"     , ".*Global.*"),
                                  ("Other"      , ".*") # add a 'catch all' term to pick up all remaining decisions...
                                ]

        # prepare compiled regex patterns         
        # and a list of names for the Labels  
        patterns = []
        group_labels = [] 
        for pos in range(len(alley_string_patterns)):
          (name, pattern_string) = alley_string_patterns[pos]
          patterns.append(re.compile(pattern_string))
          group_labels.append(name) 

        # prepare the group map dictionary
        decision_group_map = {}
        for decision_name in HltGlobalMonitor().Hlt1Decisions: 
          for pos in range(len(patterns)):
            m = patterns[pos].match(decision_name)
            if m and not decision_name in decision_group_map:
                decision_group_map[decision_name]=pos

        HltGlobalMonitor().DecToGroup  = decision_group_map
        HltGlobalMonitor().GroupLabels = group_labels 


        def _disableHistograms(c,filter = lambda x : True) :
            if 'HistoProduce' in c.getDefaultProperties() and filter(c):
                c.HistoProduce = False
            for p in [ 'Members','Filter0','Filter1' ] :
                if not hasattr(c,p) : continue
                x = getattr(c,p)
                if list is not type(x) : x = [ x ]
                for i in x : _disableHistograms(i,filter) 
        from HltLine.HltLine     import hlt1Lines, hlt2Lines
        if   self.getProp('HistogrammingLevel') == 'None' : 
            for i in hlt1Lines()+hlt2Lines() : _disableHistograms( i.configurable() )
        elif self.getProp('HistogrammingLevel') == 'Line' : 
            for i in hlt1Lines()+hlt2Lines() : _disableHistograms( i.configurable(), lambda x: x.getType()!='HltLine' ) 
        elif self.getProp('HistogrammingLevel') == 'NotLine' : 
            for i in hlt1Lines()+hlt2Lines() : _disableHistograms( i.configurable(), lambda x: x.getType()=='HltLine' )
            

##################################################################################
    def postConfigAction(self) : 
        """
        Add the configured lines into the Hlt1 and Hlt2 sequencers,
        provided there is no reason not to do so...

        @todo remove this method
        """
        from HltLine.HltLine     import hlt1Lines
        from HltLine.HltLine     import hlt2Lines

        activeHlt1Lines = []
        activeHlt2Lines = []
        sets = self.settings()
        if ( sets ):
            activeHlt1Lines = sets.ActiveHlt1Lines()
            activeHlt2Lines = sets.ActiveHlt2Lines()
        else :
            activeHlt1Lines = [ i.name() for i in hlt1Lines() ]
            activeHlt2Lines = [ i.name() for i in hlt2Lines() ]

        # append additional lines # WARNING do that in DaVinci, not Moore!
        # @todo : Need to protect against that when making a TCK. Gerhard know how
        activeHlt1Lines.extend( self.getProp('AdditionalHlt1Lines')  )
        activeHlt2Lines.extend( self.getProp('AdditionalHlt2Lines')  )

        # make sure Hlt.Global is included as soon as there is at least one Hlt. line...
        if activeHlt1Lines : activeHlt1Lines += [ 'Hlt1Global' ]
        if activeHlt2Lines : activeHlt2Lines += [ 'Hlt2Global' ]

        print '# List of requested Hlt1Lines : %s ' % activeHlt1Lines 
        print '# List of available Hlt1Lines : %s ' % [ i.name() for i in hlt1Lines() ] 
        awol1 = set( activeHlt1Lines ) - set( [ i.name() for i in hlt1Lines() ] )
        if awol1 : 
            log.fatal(' # some requested Hlt1 lines are absent : %s ' % awol1 )

        print '# List of requested Hlt2Lines : %s ' % activeHlt2Lines 
        print '# List of available Hlt2Lines : %s ' % [ i.name() for i in hlt2Lines() ] 
        awol2 = set( activeHlt2Lines ) - set( [ i.name() for i in hlt2Lines() ] )
        if awol2 : 
            log.fatal(' # some requested Hlt2 lines are absent : %s ' % awol2 )

        
        lines1 = [ i for i in hlt1Lines() if  i.name() in activeHlt1Lines ]
        log.info( '# List of configured Hlt1Lines : ' + str(hlt1Lines()) )
        log.info( '# List of Hlt1Lines added to Hlt1 : ' + str(lines1) )
        log.info( '# List of configured Hlt1Lines not added to Hlt1 : ' + str(set(hlt1Lines())-set(lines1)) )
        Sequence('Hlt1').Members = [ i.configurable() for i in lines1 ]

        # for i in hlt2Lines() : print '# active line :', i.name(), ' found :', i.name() in activeHlt2Lines
        
        lines2 = [ i for i in hlt2Lines() if i.name() in activeHlt2Lines  ]
        log.info( '# List of configured Hlt2Lines : ' + str(hlt2Lines())  )
        log.info( '# List of Hlt2Lines added to Hlt2 : ' + str( lines2 )  )
        log.info( '# List of configured Hlt2Lines not added to Hlt2 : ' + str(set(hlt2Lines())-set(lines2)) )
        Sequence('Hlt2').Members += [ i.configurable() for i in lines2 ] 

        # switch on timing limit
        if  self.getProp('EnableAcceptIfSlow') : 
            for i in hlt1Lines()+hlt2Lines() :
                i.configurable().AcceptIfSlow = True 
        
        self.configureHltMonitoring(lines1)
        self.configureVertexPersistence( [ i.name() for i in lines1 ] ) # TODO: add lines2...
        self.configureANNSelections()

        if self.getProp("Verbose") : print Sequence('Hlt') 
         

##################################################################################
#
# end sequence
#
    def endSequence(self):
        """
        define end sequence (mostly for persistence + monitoring)
        """
        from Configurables       import GaudiSequencer as Sequence
        from Configurables       import HltGlobalMonitor
        from Configurables       import bankKiller
        from Configurables       import LoKi__HDRFilter   as HltFilter
        from Configurables       import HltSelReportsMaker, HltSelReportsWriter
        from Configurables       import HltDecReportsWriter
        from Configurables       import HltVertexReportsMaker, HltVertexReportsWriter
        from Configurables       import HltRoutingBitsWriter
        from Configurables       import HltLumiWriter
        from Configurables       import DeterministicPrescaler as Prescale
        
        sets = self.settings()
        if sets and hasattr(sets,'StripEndSequence') and getattr(sets,'StripEndSequence') :
            log.warning('### Setting requests stripped down HltEndSequence ###')
            self.EnableHltGlobalMonitor = False
            self.EnableHltSelReports    = False
            self.EnableHltVtxReports    = False
            self.EnableLumiEventWriting = False
            
        # note: the following is a list and not a dict, as we depend on the order of iterating through it!!!
        from Configurables import HltLine
        _list = ( ( "EnableHltRoutingBits"   , [ HltRoutingBitsWriter ] )
                , ( "EnableHltGlobalMonitor" , [ HltGlobalMonitor ] )
                , ( "SkipHltRawBankOnRejectedEvents", [ lambda : HltLine('Hlt1Global') ] )
                # , ( "SkipHltRawBankOnRejectedEvents", [ lambda : 'Hlt1Global' ] ) # TODO: fwd Moore.WriterRequires (which is a list...)
                , ( "EnableHltDecReports"    , [ HltDecReportsWriter ] )
                , ( "EnableHltSelReports"    , [ HltSelReportsMaker, HltSelReportsWriter ] )
                , ( "EnableHltVtxReports"    , [ HltVertexReportsMaker, HltVertexReportsWriter ] )
                )
        
        End = Sequence( 'HltEndSequence' )
        EndMembers = End.Members
        # make sure we only instantiate if we actually use it...
        for i in [ v for (k,v) in _list if self.getProp(k) ] :
            EndMembers += [ c() for c in i ]
        if (self.getProp("EnableLumiEventWriting")) :
            EndMembers += [ HltLumiWriter()
                          , Sequence( 'LumiStripper' , Members = 
                                      [ HltFilter('LumiStripperFilter' , Code = "HLT_PASS_SUBSTR('Hlt1Lumi') & ~HLT_PASS_RE('Hlt1(?!Lumi).*Decision') " ) 
                                      , Prescale('LumiStripperPrescaler',AcceptFraction=self.getProp('LumiBankKillerAcceptFraction')) 
                                      , bankKiller( BankTypes=[ 'ODIN','HltLumiSummary','HltRoutingBits','DAQ' ],  DefaultIsKill=True )
                                      ])
                          ] 
        if self.getProp( 'RequireL0ForEndSequence') :
            from Configurables import LoKi__L0Filter    as L0Filter
            from HltLine.HltDecodeRaw import DecodeL0DU
            L0accept = Sequence(name='HltEndSequenceFilter', Members = DecodeL0DU.members() + [ L0Filter( 'L0Pass', Code = "L0_DECISION" )])
            EndMembers.insert(1,  L0accept )  # after routing bits
##################################################################################
    def __apply_configuration__(self):
        """
        Apply Hlt configuration
        """
        log.debug("Hlt configuration")
        import GaudiKernel.ProcessJobOptions
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions('$HLTCONFROOT/options/HltInit.py')
        
        self.confType()
        self.endSequence()
        self.configureRoutingBits()

        #appendPostConfigAction( self.postConfigAction )
        GaudiKernel.Configurable.postConfigActions.insert( 0,  self.postConfigAction )
