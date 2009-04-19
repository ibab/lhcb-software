"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.67 2009-04-19 14:37:02 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from pprint import *
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltANNSvc 
from HltConf.HltLine     import hlt1Lines
from HltConf.HltLine     import hlt2Lines
from HltConf.HltLine     import hlt1Selections
from HltConf.HltLine     import hlt1Decisions
from HltConf.HltCommissioningLines  import HltCommissioningLinesConf
from HltConf.HltVeloLines     import HltVeloLinesConf
from HltConf.HltL0Lines       import HltL0LinesConf
from HltConf.HltLumiLines     import HltLumiLinesConf
from HltConf.HltMuonLines     import HltMuonLinesConf
from HltConf.HltHadronLines   import HltHadronLinesConf
from HltConf.HltElectronLines import HltElectronLinesConf
from HltConf.HltPhotonLines   import HltPhotonLinesConf
from HltConf.HltExpressLines  import HltExpressLinesConf
from HltConf.HltBeamGasLines  import HltBeamGasLinesConf
from HltConf.Hlt1             import Hlt1Conf
from RichRecSys.Configuration import *

class HltConf(LHCbConfigurableUser):
    __used_configurables__ = [ HltCommissioningLinesConf
                             , HltVeloLinesConf
                             , HltLumiLinesConf
                             , HltBeamGasLinesConf
                             , HltL0LinesConf
                             , HltMuonLinesConf
                             , HltHadronLinesConf
                             , HltElectronLinesConf
                             , HltPhotonLinesConf
                             , HltExpressLinesConf
                             , Hlt1Conf
                             , RichRecSysConf ]
    __slots__ = { "L0TCK"                      : ''
                , "hltType"                    : 'Hlt1+Hlt2'
                , "Hlt2Requires"               : 'L0+Hlt1'  # require L0 and Hlt1 pass before running Hlt2
                , "Verbose"                    : False # print the generated Hlt sequence
                , "LumiBankKillerAcceptFraction" : 0 # fraction of lumi-only events where raw event is stripped down
                , "ActiveHlt1Lines"            : [] # list of lines to be added
                , "ActiveHlt2Lines"            : [] # list of lines to be added
                , "HistogrammingLevel"         : 'None' # or 'Line'
                }   
                
    def defineL0Channels(self, L0TCK = None) :
            if L0TCK :
                importOptions('$L0TCK/L0DUConfig.opts')
                from Configurables import L0DUMultiConfigProvider
                if L0TCK not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
                    raise KeyError('requested L0 TCK %s is not known'%L0TCK)
                from HltConf.HltL0Candidates import decodeL0Channels
                channels = decodeL0Channels( L0TCK )
            else :
                channels = [ 'Muon','MuonNoGlob','DiMuon','Muon,lowMult','DiMuon,lowMult','Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]
            from HltConf.HltL0Candidates import setupL0Channels
            setupL0Channels( channels ) 


    def confType(self,hlttype) :
            self.defineL0Channels( self.getProp('L0TCK') )
            #
            #  main HLT sequencer
            # 
            from Configurables import L0DUFromRawAlg
            Hlt = Sequence('Hlt', ModeOR= True, ShortCircuit = False
                                , Members = [ L0DUFromRawAlg()
                                            , Sequence('Hlt1') 
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
            for i in hlttype.split('+') :
                if i == 'NONE' : continue # no operation...
                if i == 'Hlt2' : continue # we deal with this later...
                if i not in type2conf : raise AttributeError, "unknown hlttype fragment '%s'"%i
                if type2conf[i] not in self.__used_configurables__ : raise AttributeError, "configurable for '%s' not in list of used configurables"%i
                print '# requested ' + i + ', importing ' + str(type2conf[i]) 
                # TODO: propagate relevant attributes to lines
                # FIXME: warning: the next is 'brittle': if someone outside 
                #        does eg. HltMuonLinesConf(), it will get activated
                #        regardless of whether we do it over here...
                #        So anyone configuring some part explictly will _always_ get
                #        that part of the Hlt run, even if it does not appear in HltType...
                type2conf[i]()
            Hlt1Conf()
            if hlttype.find('Hlt2') != -1 :   
                importOptions('$HLTCONFROOT/options/Hlt2.py')
                # TODO: this next one should become a property of the Hlt2 configurable, and we
                #       just forward to it...
                Sequence("Hlt2Requirements").Members = [ ]
                reqs = self.getProp('Hlt2Requires')
                if reqs.upper != "NONE" :
                    from Configurables import LoKi__HDRFilter   as HltFilter
                    from Configurables import LoKi__L0Filter    as L0Filter
                    hlt2requires = { 'L0'   : L0Filter( 'L0Pass',          Code = "L0_DECISION"            )
                                   , 'Hlt1' : HltFilter('Hlt1GlobalPass' , Code = "HLT_PASS('Hlt1Global')" )
                                   }
                    for i in reqs.split('+') :
                        if i : Sequence("Hlt2Requirements").Members.append( hlt2requires[i] )

    def configureRoutingBits(self) :
        ## set triggerbits
        #  0-31: reserved for L0  // need to add L0DU support to routing bit writer
        # 32-63: reserved for Hlt1
        # 64-91: reserved for Hlt2

        ### NOTE: any change in the _meaning_ of any of the following needs to be 
        ###       communicated with online, to insure the events are still properly
        ###       routed!!!
        routingBits = { 32 : "HLT_PASS('Hlt1Global')"
                      , 33 : "HLT_PASS('Hlt1LumiDecision')"         ## TODO: make it accept wildcards.. then use HLT_PASS('Hlt1.*Lumi.*Decision')
                      , 34 : "HLT_PASS('Hlt1IgnoringLumiDecision')" ## TODO: make it accept wildcards.. then use HLT_PASSIGNORING('Hlt1.*Lumi.*Decision')
                      , 35 : "HLT_PASS('Hlt1VeloClosingDecision')"  ## TODO: make it accept wildcards.. then use HLT_PASS('Hlt1.*Velo.*Decision')
                      , 36 : "HLT_PASS('Hlt1XPressDecision')"
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
        ### Make sure that the ANN Svc has everything it will need
        missing = [ i for i in sorted(set(hlt1Selections()['All']) - set(HltANNSvc().Hlt1SelectionID.keys())) if not i.startswith('TES:') ]
        missingSelections = [ i for i in missing if not i.endswith('Decision') ]
        extraSelections = dict(zip( missingSelections , range(11000, 11000 + len(missingSelections) ) ))
        HltANNSvc().Hlt1SelectionID.update( extraSelections )
        missingDecisions  = [ i for i in missing if i.endswith('Decision') ]
        extraDecisions = dict(zip( missingDecisions , range( 1000,  1000 + len(missingDecisions) ) ))
        HltANNSvc().Hlt1SelectionID.update( extraDecisions )
        print '# added ' + str(len(missingSelections)) + ' selections to HltANNSvc'
        print '# added ' + str(len(missingDecisions)) + ' decisions to HltANNSvc' 


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
                                  ("MounTrack"  , "Hlt1.*MuonTrack.*Decision"),
                                  ("Velo"       , "Hlt1.*Velo.*Decision"),
                                  ("Electron"   , "Hlt1.*Electron.*Decision"),
                                  ("Photon"     , "Hlt1.*Pho.*Decision"),
                                  ("IgnoreLumi" , ".*IgnoreLumi.*"),
                                  ("Lumi"       , "Hlt1.*Lumi.*Decision"),
                                  ("Global"     , ".*Global.*")]

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


        def disableHistograms(c,filter = lambda x : True) :
            if 'HistoProduce' in c.getDefaultProperties() and filter(c):
                c.HistoProduce = False
            for p in [ 'Members','Filter0','Filter1' ] :
                if not hasattr(c,p) : continue
                x = getattr(c,p)
                if list is not type(x) : x = [ x ]
                for i in x : disableHistograms(i,filter) 
        if   self.getProp('HistogrammingLevel') == 'None' : 
            for i in hlt1Lines() : disableHistograms( i.sequencer() )
        elif self.getProp('HistogrammingLevel') == 'Line' : 
            for i in hlt1Lines() : disableHistograms( i.sequencer(), lambda x: x.getType()!='HltLine' ) 
            

    def postConfigAction(self) : 
        from HltConf.HltLine     import Hlt1Line
        from HltConf.HltLine     import Hlt2Line
        ## Should find a more elegant way of doing this...
        ## there are too many implicit assumptions in this action...
        ##
        ## add a line for 'not lumi only' aka not lumi exclusive
        ## -- note: before the 'global' otherwise lumi set global, and we have lumi AND global set...
        lumi = [ "'" + i +"'"  for i in hlt1Decisions() if i.find('Lumi') != -1 ]
        if lumi: 
            lumi = ','.join(lumi) # Note: at max 4 entries... then should switch to a list..
            Hlt1Line('IgnoringLumi', HLT = "HLT_PASSIGNORING(" + lumi + ")" )
            Hlt1Line('Lumi',         HLT = "HLT_PASS("         + lumi + ")" )
        ## finally, add the Hlt1Global line...
        Hlt1Line('Global', HLT = 'HLT_DECISION' )

        activeLines = self.getProp('ActiveHlt1Lines') 
        lines1 = [ i for i in hlt1Lines() if ( not activeLines or i.name() in activeLines ) ]
        print '# List of configured Hlt1Lines : ' + str(hlt1Lines()) 
        print '# List of Hlt1Lines added to Hlt1 : ' + str(lines1) 
        Sequence('Hlt1').Members = [ i.configurable() for i in lines1 ] # note : should verify order (?) -- global should be last hlt1line! 

        ### TEMPORARY HACK until HltSelectionsDecision completely obsolete...
        print '# List of configured Hlt2Lines : ' + str(hlt2Lines()) 
        activeLines = self.getProp('ActiveHlt2Lines') 
        lines2 = [ i for i in hlt2Lines() if ( not activeLines or i.name() in activeLines ) ]
        Hlt2Global = Hlt2Line( "Global", HLT= "HLT_PASS( strings(" + str([ i.name()+'Decision' for i in lines2 ]) + "))")
        Sequence('Hlt2Lines').Members += [ i.configurable() for i in lines2+[ Hlt2Global] ] 
        print Sequence('Hlt2Lines').Members


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
        self.confType(self.getProp('hltType'))
        appendPostConfigAction( self.postConfigAction )
