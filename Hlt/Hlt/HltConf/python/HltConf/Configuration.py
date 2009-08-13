"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.108 2009-08-13 20:57:07 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltANNSvc 
from HltLine.HltLine     import hlt1Lines
from HltLine.HltLine     import hlt2Lines
from HltLine.HltLine     import hlt1Selections
from HltLine.HltLine     import hlt1Decisions
from Hlt1                import Hlt1Conf, hlt1TypeDecoder
from Hlt2                import Hlt2Conf, hlt2TypeDecoder
from Effective_Nominal   import Effective_Nominal 

##################################################################################
class HltConf(LHCbConfigurableUser):
    """
    Hlt configuration
    """
    __used_configurables__ = [ Hlt1Conf
                             , Hlt2Conf ]
    __slots__ = { "L0TCK"                      : ''
                , "HltType"                    : 'Hlt1+Hlt2'
                , "DataType"                   : '2009'
                , "Hlt2Requires"               : 'L0+Hlt1'  # require L0 and Hlt1 pass before running Hlt2
                , "Verbose"                    : False # print the generated Hlt sequence
                , "HistogrammingLevel"         : 'None' # or 'Line'
                , "ThresholdSettings"           : '' #  select a predefined set of settings, eg. 'Effective_Nominal'
                , "EnableHltGlobalMonitor"       : True
                , "EnableHltDecReports"          : True
                , "EnableHltSelReports"          : True
                , "EnableHltVtxReports"          : True
                , "EnableHltRoutingBits"         : True
                , "EnableLumiEventWriting"       : False
                , "LumiBankKillerAcceptFraction" : 0.9999 # fraction of lumi-only events where raw event is stripped down
                                                          # (only matters if EnablelumiEventWriting = True)
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
            importOptions('$L0TCK/L0DUConfig.opts')
            from Configurables import L0DUMultiConfigProvider
            if L0TCK not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
                raise KeyError('requested L0 TCK %s is not known'%L0TCK)
            from Hlt1Lines.HltL0Candidates import decodeL0Channels
            channels = decodeL0Channels( L0TCK )
        else :
            channels = [ 'Muon','DiMuon','Muon,lowMult','DiMuon,lowMult','Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]
        from Hlt1Lines.HltL0Candidates import setupL0Channels
        setupL0Channels( channels ) 

##################################################################################
    def settings(self) :
        """
        Get the class that contains the thresholds, etc
        """
        thresName  = self.getProp('ThresholdSettings')   # the name
        thresClass = None
        if ( thresName == '' ):
            # return a class or None
            thresClass = self.settingsForDataType( self.getProp('DataType') )
        else :
            # bootstrap (it does not like string replacement...)
            exec "from HltConf."+self.getProp('ThresholdSettings')+" import "+self.getProp('ThresholdSettings') 
            thresClass = eval(thresName+"()")                # the class
        return thresClass
        
##################################################################################
    def confType(self,hlttype) :
        """
        Decoding fo configuration. That's where Hlt1 and 2 configurations are called.
        """
        self.defineL0Channels( self.getProp('L0TCK') )
        #
        #  main HLT sequencer
        # 
        Hlt = Sequence('Hlt', ModeOR= True, ShortCircuit = False
                       , Members = 
                       [ Sequence('Hlt1') 
                       , Sequence('Hlt2') # NOTE: Hlt2 checks itself whether Hlt1 passed or not
                       , Sequence('HltEndSequence') 
                       ] )
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

        #
        # decode Hlt types
        #
        hlttype = self.getProp("HltType")
        #
        # decode Hlt1 types
        #
        ( hlt1type, hlttype ) = hlt1TypeDecoder( hlttype )
        if hlt1type != '' :
            Hlt1Conf()
            Hlt1Conf().ThresholdSettings = ThresholdSettings
            Hlt1Conf().Hlt1Type = hlt1type
        #
        # decode Hlt2 types
        #
        ( hlt2type, hlttype ) = hlt2TypeDecoder( hlttype )
        if hlt2type != '':
            self.setOtherProps(Hlt2Conf(),[ "DataType","Hlt2Requires" ])
            Hlt2Conf()
            Hlt2Conf().ThresholdSettings = ThresholdSettings
            Hlt2Conf().Hlt2Type = hlt2type

        if hlttype and hlttype not in [ '', 'NONE' ]:
            raise AttributeError, "unknown HltType fragment '%s'"%hlttype

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
        ###       communicated with online, to insure the events are still properly
        ###       routed!!!
        routingBits = { 32 : "HLT_PASS('Hlt1Global')"
                      , 33 : "HLT_PASS_SUBSTR('Hlt1Lumi')" 
                      , 34 : "HLT_PASS_RE('Hlt1(?!Lumi).*Decision')"  # note: we need the 'Decision' at the end to _exclude_ Hlt1Global
                      , 35 : "HLT_PASS_SUBSTR('Hlt1Velo')"  
                      , 36 : "HLT_PASS('Hlt1XPressDecision','Hlt2UnbiasedJPsiDecision')"
                      # 64--96: Hlt2
                      , 64 : "HLT_PASS('Hlt2Global')"
                      }

        ## and record the settings in the ANN service
        HltANNSvc().RoutingBits = dict( [ (v,k) for k,v in routingBits.iteritems() ] )
        from Configurables import HltRoutingBitsWriter
        HltRoutingBitsWriter().RoutingBits = routingBits

##################################################################################
    def configureVertexPersistence(self) :
        """
        persistify vertices
        """
        ## and persist some vertices...
        ## uhhh... need to pick only those in ActiveHlt1Lines...
        vertices = [ i for i in hlt1Selections()['All'] if i is 'PV2D' or   ( i.startswith('Hlt1Velo') and i.endswith('Decision') ) ]
        from Configurables import HltVertexReportsMaker
        HltVertexReportsMaker().VertexSelections = vertices
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
        HltSelReportsMaker().DebugEventPeriod = 2000

##################################################################################
    def configureANNSelections(self) :
        """
        Assigned numbers configuration
        """
        ### TODO: use the computed indices available from the lines...
        ### TODO: what about shared selections??? (which appear with multiple indices!)
        ###       but which have names not prefixed by the line name
        ### Make sure that the ANN Svc has everything it will need
        missing = [ i for i in sorted(set(hlt1Selections()['All']) - set(HltANNSvc().Hlt1SelectionID.keys())) if not i.startswith('TES:') ]
        missingDecisions  = [ i for i in missing if i.endswith('Decision') ]
        extraDecisions = dict(zip( missingDecisions , range( 1000,  1000 + len(missingDecisions) ) ))
        HltANNSvc().Hlt1SelectionID.update( extraDecisions )

        missingSelections = [ i for i in missing if not i.endswith('Decision') ]
        extraSelections = dict(zip( missingSelections , range(11000, 11000 + len(missingSelections) ) ))
        HltANNSvc().Hlt1SelectionID.update( extraSelections )
        log.info( '# added ' + str(len(missingSelections)) + ' selections to HltANNSvc' )
        log.info( '# added ' + str(len(missingDecisions)) + ' decisions to HltANNSvc'  )

        if False :
            for i in hlt1Lines() :
                log.debug( "checking " + i.name() )
                decisionName = i.name() + 'Decision'
                if decisionName in HltANNSvc().Hlt1SelectionID :
                    id = HltANNSvc().Hlt1SelectionID[ decisionName ] 
                else :
                    id = None

                if id :
                    log.debug( i.index() )
                    for (key,value ) in zip(i.index(),range(0,len(i.index()))) :
                        if key in hlt1Selections()['All'] :
                            # TODO: see if the selection is unique to this line...
                            cur = HltANNSvc().Hlt1SelectionID[ key ] if key in HltANNSvc().Hlt1SelectionID else  None
                            log.debug( ' selection %s in line %s should have ID %d:%d -- has %d' % ( key,  i.name(), id, value, cur)  )
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
                                  ("XPress"     , "Hlt1.*XPress.*Decision"),
                                  ("Hadron"     , "Hlt1.*Hadron.*Decision"),
                                  ("SingleMuon" , "Hlt1.*SingleMuon.*Decision"),
                                  ("DiMuon"     , "Hlt1.*DiMuon.*Decision"),
                                  ("MuTrack"    , "Hlt1.*MuTrack.*Decision"),
                                  ("Velo"       , "Hlt1Velo.*Decision"),
                                  ("Electron"   , "Hlt1.*Electron.*Decision"),
                                  ("Photon"     , "Hlt1.*Pho.*Decision"),
                                  ("Lumi"       , "Hlt1Lumi.*Decision"),
                                  ("Global"     , ".*Global.*"),
                                  ("PA"         , "Hlt1(Random|Physics|Tell1Error)Decision"),
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


        # set property (in C++: std::map< std::string, std::vector<std::string> > ) 
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
        """

        activeLines = []
        sets = self.settings()
        if ( sets != None ):
            activeLines = sets.ActiveLines()

#        for i in hlt1Lines() : print '# active line :', i.name(), ' found :', i.name() in activeLines
        
        lines1 = [ i for i in hlt1Lines() if ( not activeLines or i.name() in activeLines + [ 'Hlt1Global' ] ) ]
        log.info( '# List of configured Hlt1Lines : ' + str(hlt1Lines()) )
        log.info( '# List of Hlt1Lines added to Hlt1 : ' + str(lines1) )
        log.info( '# List of configured Hlt1Lines not added to Hlt1 : ' + str(set(hlt1Lines())-set(lines1)) )
        Sequence('Hlt1').Members = [ i.configurable() for i in lines1 ]

#        for i in hlt2Lines() : print '# active line :', i.name(), ' found :', i.name() in activeLines
        
        lines2 = [ i for i in hlt2Lines() if ( not activeLines or i.name() in activeLines + [ 'Hlt2Global' ]) ]
        log.info( '# List of configured Hlt2Lines : ' + str(hlt2Lines())  )
        log.info( '# List of Hlt2Lines added to Hlt2 : ' + str( lines2 )  )
        log.info( '# List of configured Hlt2Lines not added to Hlt2 : ' + str(set(hlt2Lines())-set(lines2)) )
        Sequence('Hlt2Lines').Members += [ i.configurable() for i in lines2 ] 

        self.configureHltMonitoring(lines1)
        self.configureVertexPersistence()
        self.configureANNSelections()

        if self.getProp("Verbose") : print Sequence('Hlt') 
         

##################################################################################
#
# end sequence
#
    def endSequence(self,hlttype):
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
        
        if hlttype is 'PA' :  # if _only_ PA, we strip down even more...
            self.EnableHltGlobalMonitor = False
            self.EnableHltSelReports = False
            self.EnableHltVtxReports = False
            self.EnableLumiEventWriting = False
            self.ActiveHlt1Lines = [ 'Hlt1NonRandomODIN','Hlt1RandomODIN','Hlt1Tell1Error'] 
            
            # note: the following is a list and not a dict, as we depend on the order of iterating through it!!!
        _list = ( ( "EnableHltGlobalMonitor" , [ HltGlobalMonitor ] )
                , ( "EnableHltDecReports"    , [ HltDecReportsWriter ] )
                , ( "EnableHltSelReports"    , [ HltSelReportsMaker, HltSelReportsWriter ] )
                , ( "EnableHltVtxReports"    , [ HltVertexReportsMaker, HltVertexReportsWriter ] )
                , ( "EnableHltRoutingBits"   , [ HltRoutingBitsWriter ] )
                )
        
        End = Sequence( 'HltEndSequence', IgnoreFilterPassed = True )
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
##################################################################################
    def __apply_configuration__(self):
        """
        Apply Hlt configuration
        """
        log.debug("Hlt configuration")
        import GaudiKernel.ProcessJobOptions
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions('$HLTCONFROOT/options/HltInit.py')
        
        self.confType(self.getProp('HltType'))      
        self.endSequence(self.getProp('HltType'))
        # make sure 'strings' is known...
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        HltFactory('ToolSvc.HltFactory').Modules += [ 'LoKiCore.functions' ]
        HltFactory('ToolSvc.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions' ]
        self.configureRoutingBits()

        appendPostConfigAction( self.postConfigAction )
