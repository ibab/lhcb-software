"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.40 2009-02-12 12:25:45 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from pprint import *
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables       import GaudiSequencer as Sequence
from Configurables       import LoKi__L0Filter    as L0Filter
from Configurables       import LoKi__HDRFilter   as HltFilter
from Configurables       import HltGlobalMonitor
from Configurables       import HltVertexReportsMaker
from Configurables       import HltSelReportsMaker
from Configurables       import L0DUMultiConfigProvider
from Configurables       import HltANNSvc 
from HltConf.HltLine     import Hlt1Line   as Line
from HltConf.HltLine     import hlt1Lines
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
from HltConf.Hlt1             import Hlt1Conf
import HltConf.HltL0Candidates
from HltConf.HltL0Candidates  import setupL0Channels, decodeL0Channels

class HltConf(LHCbConfigurableUser):
    __used_configurables__ = [ HltCommissioningLinesConf
                             , HltVeloLinesConf
                             , HltLumiLinesConf
                             , HltL0LinesConf
                             , HltMuonLinesConf
                             , HltHadronLinesConf
                             , HltElectronLinesConf
                             , HltPhotonLinesConf
                             , HltExpressLinesConf
                             , Hlt1Conf ]
    __slots__ = { "L0TCK"                      : ''
                , "hltType"                    : 'Hlt1+Hlt2'
                , "replaceL0BanksWithEmulated" : False
                , "Hlt2IgnoreHlt1Decision"     : False # run Hlt2 even if Hlt1 failed
                , "Verbose"                    : False # print the generated Hlt sequence
                , "LumiBankKillerAcceptFraction" : 0 # fraction of lumi-only events where raw event is stripped down
                , "ActiveHlt1Lines"            : [] # list of lines to be added
                }   
                
    def defineL0Channels(self, L0TCK = None) :
            if L0TCK :
                importOptions('$L0TCK/L0DUConfig.opts')
                if L0TCK not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
                    raise KeyError('requested L0 TCK %s is not known'%L0TCK)
                channels = decodeL0Channels( L0TCK )
            else :
                channels = [ 'Muon','DiMuon','MuonNoGlob','Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]
            setupL0Channels( channels ) 


    def confType(self,hlttype) :
            self.defineL0Channels( self.getProp('L0TCK') )
            #/**
            # * main HLT sequencer
            # */
            Hlt = Sequence('Hlt', ModeOR= True, ShortCircuit = False
                                , Members = [ Sequence('Hlt1') 
                                            , Sequence('Hlt2') # NOTE: Hlt2 checks itself whether Hlt1 passed or not
                                            , Sequence('HltEndSequence') 
                                            ] )
            trans = { 'Hlt1'   : 'LU+L0+VE+XP+MU+HA+PH+EL'
                    , 'DEFAULT': 'PA+LU+L0+VE+XP'
                    }
            importOptions('$HLTCONFROOT/options/HltRecoSequence.py')
            for short,full in trans.iteritems() : hlttype = hlttype.replace(short,full)
            type2conf = { 'PA' : HltCommissioningLinesConf # PA for 'PAss-thru' (PT was considered bad)
                        , 'LU' : HltLumiLinesConf
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
                if self.getProp('Hlt2IgnoreHlt1Decision') :  
                    Sequence("Hlt2CheckHlt1Passed").Members = [ L0Filter('L0Pass', Code = "L0_DECISION" ) ]
                else : 
                    Sequence("Hlt2CheckHlt1Passed").Members = [ HltFilter('Hlt1GlobalPass' , Code = "HLT_PASS('Hlt1Global')" ) ]

    def postConfigAction(self) : 
        ## Should find a more elegant way of doing this...
        ## there are too many implicit assumptions in this action...
        #if self.getProp('replaceL0BanksWithEmulated') : 
        #    topalg = ApplicationMgr().TopAlg
        #    topalg.remove('GaudiSequencer/L0DUBankSwap')
        #    Sequence('Hlt').Members.insert(0,'GaudiSequencer/L0DUBankSwap')
        ##
        ## add a line for 'not lumi only' 
        ## -- note: before the 'global' otherwise lumi set global, and we have lumi AND global set...
        Line('IgnoringLumi', HLT = "HLT_PASSIGNORING('Hlt1LumiDecision')" )
        ## finally, add the Hlt1Global line...
        Line('Global', HLT = 'HLT_DECISION' )
        activeLines = self.getProp('ActiveHlt1Lines') 
        lines = [ i for i in hlt1Lines() if ( not activeLines or i.name() in activeLines ) ]
        log.info( 'List of configured Hlt1Lines : ' + str(hlt1Lines()) )
        log.info( 'List of Hlt1Lines to be added to Hlt1 : ' + str(lines) )
        Sequence('Hlt1').Members = [ i.sequencer() for i in lines ] # note : should verify order (?) -- global should be last hlt1line! 
        ## and tell the monitoring what it should expect..
        HltGlobalMonitor().Hlt1Decisions = [ i.decision() for i in lines ]
        ## and persist some vertices...
        ## uhhh... need to pick only those in ActiveHlt1Lines...
        vertices = [ i for i in hlt1Selections()['All'] if i is 'PV2D' or   ( i.startswith('Hlt1Velo') and i.endswith('Decision') ) ]
        HltVertexReportsMaker().VertexSelections = vertices
        ## do not write out the candidates for the vertices we store 
        HltSelReportsMaker().SelectionMaxCandidates.update( dict( [ (i,0) for i in vertices if i.endswith('Decision') ] ) )
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
        if self.getProp("Verbose") : print Sequence('Hlt') 
         

    def __apply_configuration__(self):
        """
        Apply Hlt configuration
        """
        log.info("Hlt configuration")
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions('$HLTCONFROOT/options/HltInit.py')
        log.info("Loaded HltInit")
        if self.getProp('replaceL0BanksWithEmulated') : 
            importOptions('$L0DUROOT/options/ReplaceL0BanksWithEmulated.opts')
        self.setOtherProp( Hlt1Conf(), 'LumiBankKillerAcceptFraction' )
        self.confType(self.getProp('hltType'))
        appendPostConfigAction( self.postConfigAction )
