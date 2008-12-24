"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.28 2008-12-24 09:39:05 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from LHCbKernel.Configuration import *
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables       import GaudiSequencer as Sequence
from Configurables       import LoKi__L0Filter    as L0Filter
from Configurables       import LoKi__HDRFilter   as HltFilter

class HltConf(LHCbConfigurableUser):
    __slots__ = {
          "hltType" :          'Hlt1+Hlt2'
        , "userAlgorithms":    [ ]  # put here user algorithms to add
        , "oldStyle" :         False # old style options configuration
        , "replaceL0BanksWithEmulated" : False
        , "Hlt2IgnoreHlt1Decision" : False # run Hlt2 even if Hlt1 failed
        , "verbose" :          False # print the generated Hlt sequence
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
            type2conf = { 'PA' : '$HLTCONFROOT/options/HltCommissioningLines.py' # PA for 'PAss-thru' (PT was considered bad)
                        , 'LU' : '$HLTCONFROOT/options/HltLumiLines.py'
                        , 'L0' : '$HLTCONFROOT/options/HltL0Lines.py'
                        , 'VE' : '$HLTCONFROOT/options/HltVeloLines.py'
                        , 'XP' : '$HLTCONFROOT/options/HltExpressLines.py'
                        , 'MU' : '$HLTCONFROOT/options/HltMuonLines.py' 
                        , 'HA' : '$HLTCONFROOT/options/HltHadronLines.py' 
                        , 'PH' : '$HLTCONFROOT/options/HltPhotonLines.py' 
                        , 'EL' : '$HLTCONFROOT/options/HltElectronLines.py' }
            for i in hlttype.split('+') :
                if i == 'NONE' : continue # no operation...
                if i == 'Hlt2' : continue # we deal with this later...
                if i not in type2conf : raise AttributError, "unknown hlttype fragment '%s'"%i
                log.info( '# requested ' + i + ', including ' + type2conf[i]  )
                importOptions( type2conf[i] )
            importOptions('$HLTCONFROOT/options/HltMain.py')
            importOptions('$HLTCONFROOT/options/Hlt1.py')
            if hlttype.find('Hlt2') != -1 :   
                importOptions('$HLTCONFROOT/options/Hlt2.py')
                # TODO: this next one should become a property of the Hlt2 configurable, and we
                #       just forward to it...
                if self.getProp('Hlt2IgnoreHlt1Decision') :  
                    Sequence("Hlt2CheckHlt1Passed").Members = [ L0Filter('L0Pass', Code = "L0_DECISION" ) ]
                else : 
                    Sequence("Hlt2CheckHlt1Passed").Members = [ HltFilter('Hlt1GlobalPass' , Code = "HLT_PASS('Hlt1Global')" ) ]
            if self.getProp("verbose") : log.info( Sequence('Hlt')  )

    def __apply_configuration__(self):
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
