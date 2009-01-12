"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.33 2009-01-12 11:02:19 graven Exp $"
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
        , "oldStyle" :         False # old style options configuration
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
                
    def oldConfig(self,hlttype) :
            if hlttype not in self.validHltTypes() :  raise TypeError("Unknown hlttype '%s'"%hlttype)
            importOptions('$HLTCONFROOT/options/HltMain.py')
            if hlttype.find('Hlt1') != -1 :   
                importOptions('$HLTCONFROOT/options/Hlt1.opts')
            if hlttype.find('Hlt2') != -1 :   
                importOptions('$HLTCONFROOT/options/Hlt2.py')
                # TODO: this next one should become a property of the Hlt2 configurable, and we
                #       just forward to it...
                if self.getProp('Hlt2IgnoreHlt1Decision') :  
                    Sequence("Hlt2CheckHlt1Passed").Members = [ L0Filter('L0Pass', Code = "L0_DECISION") ]
                else :
                    Sequence("Hlt2CheckHlt1Passed").Members = [ Sequence( "PassedAlleys" ) ]
            importOptions( '$HLTCONFROOT/options/HltPersistentOutput.py' )
    def newConfig(self,hlttype) :
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
        log.info( 'List of known Hlt1Lines : ' + str(hlt1Lines()) )
        lines = hlt1Lines() if not self.getProp('ActiveHlt1Lines') else self.getProp('ActiveHlt1Lines')
        log.info( 'List of Hlt1Lines to be added to Hlt1 : ' + str(lines) )
        Sequence('Hlt1').Members = [ i.sequencer() for i in  lines ] # note : should verify order (?) -- global should be last hlt1line! 
        ## and tell the monitoring what it should expect..
        HltGlobalMonitor().Hlt1Decisions = list( hlt1Decisions() )
        ## and persist some vertices...
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
        if self.getProp('oldStyle') :
            self.oldConfig(hlttype)
        else :
            self.newConfig(hlttype)
        for userAlg in self.getProp("userAlgorithms"):
            ApplicationMgr().TopAlg += [ userAlg ]
        appendPostConfigAction( self.postConfigAction )
