"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.96 2009-07-18 21:08:56 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from pprint import *
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltANNSvc 
from HltLine.HltLine     import hlt1Lines
from HltLine.HltLine     import hlt2Lines
from HltLine.HltLine     import hlt1Selections
from HltLine.HltLine     import hlt1Decisions
from Hlt1Lines.HltCommissioningLines  import HltCommissioningLinesConf
from Hlt1Lines.HltVeloLines     import HltVeloLinesConf
from Hlt1Lines.HltL0Lines       import HltL0LinesConf
from Hlt1Lines.HltLumiLines     import HltLumiLinesConf
from Hlt1Lines.HltMuonLines     import HltMuonLinesConf
from Hlt1Lines.HltHadronLines   import HltHadronLinesConf
from Hlt1Lines.HltElectronLines import HltElectronLinesConf
from Hlt1Lines.HltPhotonLines   import HltPhotonLinesConf
from Hlt1Lines.HltExpressLines  import HltExpressLinesConf
from Hlt1Lines.HltBeamGasLines  import HltBeamGasLinesConf

from HltConf.Hlt1             import Hlt1Conf
from HltConf.Hlt2             import Hlt2Conf
from RichRecSys.Configuration import *

class HltConf(LHCbConfigurableUser):
    __used_configurables__ = [ Hlt1Conf
                             # Hlt1 Lines
                             , HltCommissioningLinesConf
                             , HltVeloLinesConf
                             , HltLumiLinesConf
                             , HltBeamGasLinesConf
                             , HltL0LinesConf
                             , HltMuonLinesConf
                             , HltHadronLinesConf
                             , HltElectronLinesConf
                             , HltPhotonLinesConf
                             , HltExpressLinesConf
                             , Hlt2Conf ]
    __slots__ = { "L0TCK"                      : ''
                , "HltType"                    : 'Hlt1+Hlt2'
                , "DataType"                   : '2009'
                , "Hlt2Requires"               : 'L0+Hlt1'  # require L0 and Hlt1 pass before running Hlt2
                , "Verbose"                    : False # print the generated Hlt sequence
                , "LumiBankKillerAcceptFraction" : 0. # fraction of lumi-only events where raw event is stripped down
                , "ActiveHlt1Lines"            : [] # list of lines to be added
                , "ActiveHlt2Lines"            : [] # list of lines to be added
                , "HistogrammingLevel"         : 'None' # or 'Line'
                , "ThresholdSettings"           : '' #  select a predefined set of settings, eg. 'Miriam_20090430' or 'FEST'
                }   

    def defineL0Channels(self, L0TCK = None) :
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


    def confType(self,hlttype) :
            self.defineL0Channels( self.getProp('L0TCK') )
            #
            #  main HLT sequencer
            # 
            from Configurables import L0DUFromRawAlg
            Hlt = Sequence('Hlt', ModeOR= True, ShortCircuit = False
                                , Members = #[ L0DUFromRawAlg()
                                            [ Sequence('Hlt1') 
                                            , Sequence('Hlt2') # NOTE: Hlt2 checks itself whether Hlt1 passed or not
                                            , Sequence('HltEndSequence') 
                                            ] )
            trans = { 'Hlt1'   : 'LU+L0+VE+XP+MU+HA+PH+EL'
                    , 'DEFAULT': 'PA+LU+L0+VE+XP'
                    }
            for short,full in trans.iteritems() : hlttype = hlttype.replace(short,full)
            type2conf = { 'PA' : HltCommissioningLinesConf # PA for 'PAss-thru' (PT was considered bad)
                        , 'LU' : HltLumiLinesConf
                        , 'BG' : HltBeamGasLinesConf
                        , 'L0' : HltL0LinesConf
                        , 'VE' : HltVeloLinesConf
                        , 'XP' : HltExpressLinesConf
                        , 'MU' : HltMuonLinesConf
                        , 'HA' : HltHadronLinesConf
                        , 'PH' : HltPhotonLinesConf
                        , 'EL' : HltElectronLinesConf }

            ThresholdSettings = None
            if self.getProp('ThresholdSettings'): 
                exec "from HltThresholdSettings import %s as ThresholdSettings" % self.getProp('ThresholdSettings')
            else :
                from HltThresholdSettings import SettingsForDataType
                ThresholdSettings = SettingsForDataType( self.getProp('DataType') )
            log.info('# ThresholdSettings ' + str(ThresholdSettings) )

            for i in hlttype.split('+') :
                if i == 'NONE' : continue # no operation...
                if i == 'Hlt2' : continue # we deal with this later...
                if i not in type2conf : raise AttributeError, "unknown HltType fragment '%s'"%i
                if type2conf[i] not in self.__used_configurables__ : raise AttributeError, "configurable for '%s' not in list of used configurables"%i
                log.info( '# requested ' + i + ', importing ' + str(type2conf[i])  )
                # FIXME: warning: the next is 'brittle': if someone outside 
                #        does eg. HltMuonLinesConf(), it will get activated
                #        regardless of whether we do it over here...
                #        So anyone configuring some part explictly will _always_ get
                #        that part of the Hlt run, even if it does not appear in HltType...
                conf = type2conf[i]()
                if ThresholdSettings and i in ThresholdSettings : 
                    for (k,v) in ThresholdSettings[i].iteritems() :
                        # configurables have an exception for list and dict: 
                        #   even if not explicitly set, if you ask for them, you get one...
                        #   this is done to make foo().somelist += ... work.
                        # hence we _assume_ that, even if we have an attr, but it matches the
                        # default, it wasn't set explicitly, and we overrule it...
                        if hasattr(conf,k) and conf.getProp(k) != conf.getDefaultProperty(k) :
                            log.warning('# WARNING: %s.%s has explictly been set, NOT using requested predefined threshold %s, but keeping explicit value: %s '%(conf.name(),k,str(v),getattr(conf,k)))
                        else :
                            setattr(conf,k,v)
            Hlt1Conf()
            if hlttype is 'PA' :  # if _only_ PA, we strip down even more...
                Hlt1Conf().EnableHltGlobalMonitor = False
                Hlt1Conf().EnableHltSelReports = False
                Hlt1Conf().EnableHltVtxReports = False
                Hlt1Conf().EnableLumiEventWriting = False
                self.setProp('ActiveHlt1Lines',[ 'Hlt1Physics','Hlt1Random'] )
            #
            # Hlt2
            #
            if hlttype.find('Hlt2') != -1 :                
                self.setOtherProps(Hlt2Conf(),["DataType","Hlt2Requires", "ActiveHlt2Lines",
                                               "HistogrammingLevel", "ThresholdSettings"])
                


    def configureRoutingBits(self) :
        ## set triggerbits
        #  0-31: reserved for L0  // need to add L0DU support to routing bit writer
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
                      }

        ## and record the settings in the ANN service
        HltANNSvc().RoutingBits = dict( [ (v,k) for k,v in routingBits.iteritems() ] )
        from Configurables import HltRoutingBitsWriter
        HltRoutingBitsWriter().RoutingBits = routingBits

    def configureVertexPersistence(self) :
        ## and persist some vertices...
        ## uhhh... need to pick only those in ActiveHlt1Lines...
        vertices = [ i for i in hlt1Selections()['All'] if i is 'PV2D' or   ( i.startswith('Hlt1Velo') and i.endswith('Decision') ) ]
        from Configurables import HltVertexReportsMaker
        HltVertexReportsMaker().VertexSelections = vertices
        ## do not write out the candidates for the vertices we store 
        from Configurables import HltSelReportsMaker
        HltSelReportsMaker().SelectionMaxCandidates.update( dict( [ (i,0) for i in vertices if i.endswith('Decision') ] ) )

    def configureANNSelections(self) :
        ### TODO: use the computed indices available from the lines...
        ### TODO: what about shared selections??? (which appear with multiple indices!)
        ###       but which have names not prefixed by the line name
        ### Make sure that the ANN Svc has everything it will need
        missing = [ i for i in sorted(set(hlt1Selections()['All']) - set(HltANNSvc().Hlt1SelectionID.keys())) if not i.startswith('TES:') ]
        missingDecisions  = [ i for i in missing if i.endswith('Decision') ]
        extraDecisions = dict(zip( missingDecisions , range( 1000,  1000 + len(missingDecisions) ) ))
        HltANNSvc().Hlt1SelectionID.update( extraDecisions )

        missingSelections = [ i for i in missing if not i.endswith('Decision') ]

        if False :
            for i in hlt1Lines() :
                decisionName = i.name() + 'Decision'
                if decisionName in HltANNSvc().Hlt1SelectionID :
                    id = HltANNSvc().Hlt1SelectionID[ decisionName ] 
                else :
                    id = None

                if id :
                    for (key,value ) in zip(i.index(),range(0,len(i.index()))) :
                        if key in hlt1Selections()['All'] :
                            log.warning( ' selection %s in line %s should have ID %d:%d' % ( key,  i.name(), id, value) )
                        #else :
                        #    print ' line %s, algo %s does not have a selection? ' % (i.name(),key)
                else :
                    log.warning( 'Hlt1Line %s not known to ANNSvc??' % i.name() )

        extraSelections = dict(zip( missingSelections , range(11000, 11000 + len(missingSelections) ) ))
        HltANNSvc().Hlt1SelectionID.update( extraSelections )
        log.info( '# added ' + str(len(missingSelections)) + ' selections to HltANNSvc' )
        log.info( '# added ' + str(len(missingDecisions)) + ' decisions to HltANNSvc'  )


    def configureHltMonitoring(self, lines) :
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
                                  ("MuonTrack"  , "Hlt1.*MuonTrack.*Decision"),
                                  ("Velo"       , "Hlt1Velo.*Decision"),
                                  ("Electron"   , "Hlt1.*Electron.*Decision"),
                                  ("Photon"     , "Hlt1.*Pho.*Decision"),
                                  ("IgnoreLumi" , ".*IgnoreLumi.*"),
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
            

    def postConfigAction(self) : 
        from HltLine.HltLine     import Hlt1Line
        from HltLine.HltLine     import Hlt2Line

        # make sure 'strings' is known...
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        HltFactory('ToolSvc.HltFactory').Modules += [ 'LoKiCore.functions' ]
        HltFactory('ToolSvc.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions' ]

        ## Should find a more elegant way of doing this...
        ## there are too many implicit assumptions in this action...
        ##
        ## finally, add the Hlt1Global line...
        Hlt1Global = Hlt1Line('Global', HLT = 'HLT_DECISION' )

        activeLines = self.getProp('ActiveHlt1Lines') 
        lines1 = [ i for i in hlt1Lines() if ( not activeLines or i.name() in activeLines ) ]
        if Hlt1Global not in lines1 : lines1 += [ Hlt1Global ]
        log.info( '# List of configured Hlt1Lines : ' + str(hlt1Lines())  )
        log.info( '# List of Hlt1Lines added to Hlt1 : ' + str(lines1)  )
        Sequence('Hlt1').Members = [ i.configurable() for i in lines1 ] # note : should verify order (?) -- global should be last hlt1line! 

        log.info( '# List of configured Hlt2Lines : ' + str(hlt2Lines())  )
        activeLines = self.getProp('ActiveHlt2Lines') 
        Hlt2Global = Hlt2Line( "Global", HLT= "HLT_PASS_SUBSTR('Hlt2') " )

        lines2 = [ i for i in hlt2Lines() if ( not activeLines or i.name() in activeLines ) ]
        lines2 += [ Hlt2Global ]
        Sequence('Hlt2Lines').Members += [ i.configurable() for i in lines2 ] 
        log.info( '# List of Hlt2Lines added to Hlt2 : ' + str( lines2 ) )


        self.configureHltMonitoring(lines1)
        self.configureRoutingBits()
        self.configureVertexPersistence()
        self.configureANNSelections()

        if self.getProp("Verbose") : print Sequence('Hlt') 
         

    def __apply_configuration__(self):
        """
        Apply Hlt configuration
        """
        log.info("Hlt configuration")
        import GaudiKernel.ProcessJobOptions
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions('$HLTCONFROOT/options/HltInit.py')
        log.info("Loaded HltInit")
        self.setOtherProp( Hlt1Conf(), 'LumiBankKillerAcceptFraction' )
        self.confType(self.getProp('HltType'))
        appendPostConfigAction( self.postConfigAction )
