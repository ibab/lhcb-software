"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.35 2009-01-15 20:25:47 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables       import GaudiSequencer as Sequence
from Configurables       import LoKi__L0Filter    as L0Filter
from Configurables       import LoKi__HDRFilter   as HltFilter
from Configurables       import HltGlobalMonitor
from Configurables       import HltVertexReportsMaker
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
    __slots__ = {
          "hltType" :          'Hlt1+Hlt2'
        , "userAlgorithms":    [ ]  # put here user algorithms to add
        , "replaceL0BanksWithEmulated" : False
        , "Hlt2IgnoreHlt1Decision" : False # run Hlt2 even if Hlt1 failed
        , "verbose" :          False # print the generated Hlt sequence
        , "ActiveHlt1Lines" :      [] # list of lines to be added
        }   
    def validHltTypes(self):
        return [ 'NONE',
                 'Hlt1',
                 'Hlt2',
                 'Hlt1+Hlt2',
                 'DEFAULT' ]
                
    def confType(self,hlttype) :
            trans = { 'Hlt1'   : 'LU+L0+VE+XP+MU+HA+PH+EL'
                    , 'DEFAULT': 'PA+LU+L0+VE+XP'
                    }
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
            importOptions('$HLTCONFROOT/options/HltMain.py')
            Hlt1Conf()
            if hlttype.find('Hlt2') != -1 :   
                importOptions('$HLTCONFROOT/options/Hlt2.py')
                # TODO: this next one should become a property of the Hlt2 configurable, and we
                #       just forward to it...
                if self.getProp('Hlt2IgnoreHlt1Decision') :  
                    Sequence("Hlt2CheckHlt1Passed").Members = [ L0Filter('L0Pass', Code = "L0_DECISION" ) ]
                else : 
                    Sequence("Hlt2CheckHlt1Passed").Members = [ HltFilter('Hlt1GlobalPass' , Code = "HLT_PASS('Hlt1Global')" ) ]
            if self.getProp("verbose") : log.info( Sequence('Hlt')  )

    def postConfigAction(self) : 
        ## Should find a more elegant way of doing this...
        ## there are too many implicit assumptions in this action...
        ##
        ## add a line for 'not lumi only' 
        ## -- note: before the 'global' otherwise lumi set global, and we have lumi AND global set...
        Line('IgnoringLumi', HLT = "HLT_PASSIGNORING('Hlt1LumiDecision')" )
        ## finally, add the Hlt1Global line...
        Line('Global', HLT = 'HLT_DECISION' )
        activeLines = self.getProp('ActiveHlt1Lines') 
        lines = [ i for i in hlt1Lines() if ( not activeLines or i.name() in activeLines ) ]
        log.info( 'List of Hlt1Lines to be added to Hlt1 : ' + str(lines) )
        Sequence('Hlt1').Members = [ i.sequencer() for i in  lines ] # note : should verify order (?) -- global should be last hlt1line! 
        ## and tell the monitoring what it should expect..
        ## uhhh... need to pick only those in ActiveHlt1Lines...
        HltGlobalMonitor().Hlt1Decisions = list( hlt1Decisions() )
        ## and persist some vertices...
        ## uhhh... need to pick only those in ActiveHlt1Lines...
        HltVertexReportsMaker().VertexSelections = [ i for i in hlt1Selections()['All'] if i is 'PV2D' or ( i.startswith('Hlt1Velo') and i.endswith('Decision') ) ]

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
        hlttype = self.getProp('hltType')
        self.confType(hlttype)
        for userAlg in self.getProp("userAlgorithms"):
            ApplicationMgr().TopAlg += [ userAlg ]
        appendPostConfigAction( self.postConfigAction )
