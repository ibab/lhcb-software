"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.17 2008-11-14 17:27:45 tskwarni Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from LHCbKernel.Configuration import *
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables       import GaudiSequencer as Sequence
from Configurables       import L0Filter
from Configurables       import LoKi__HDRFilter   as HltFilter

class HltConf(LHCbConfigurableUser):
    __slots__ = {
          "hltType" :          'Hlt1+Hlt2'
        , "userAlgorithms":    [ ]  # put here user algorithms to add
        , "oldStyle" :         True # old style options configuration
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
                
    def applyConf(self):
        # GaudiKernel.ProcessJobOptions.PrintOff() #TODO: waiting for next release of Gaudi
        importOptions('$HLTCONFROOT/options/HltInit.opts')
        if self.getProp('replaceL0BanksWithEmulated') : importOptions('$L0DUROOT/options/ReplaceL0BanksWithEmulated.opts')
        hlttype = self.getProp('hltType')
        if self.getProp('oldStyle') :
            if hlttype not in self.validHltTypes() :  raise TypeError("Unknown hlttype '%s'"%hlttype)
            if hlttype.find('Hlt1') != -1 :   importOptions('$HLTCONFROOT/options/Hlt1.opts')
            if hlttype.find('Hlt2') != -1 :   
                importOptions('$HLTCONFROOT/options/Hlt2.py')
                # TODO: this next one should become a property of the Hlt2 configurable, and we
                #       just forward to it...
                if self.getProp('Hlt2IgnoreHlt1Decision') :  
                    Sequence("Hlt2CheckHlt1Passed").Members = [ L0Filter() ]
                else :
                    Sequence("Hlt2CheckHlt1Passed").Members = [ Sequence( "PassedAlleys" ) ]
            if hlttype == 'readBackLumi'  :   importOptions('$HLTCONFROOT/options/HltJob_readLumiPy.opts')
            if hlttype == 'writeLumi'     :   importOptions('$HLTCONFROOT/options/HltJob_onlyLumi.opts')
            if hlttype.find('Lumi') != -1 :   importOptions('$HLTCONFROOT/options/Lumi.opts')
            if hlttype.find('Velo') != -1 :   importOptions('$HLTCONFROOT/options/HltVeloAlleySequence.opts')
            importOptions( '$HLTCONFROOT/options/HltPersistentOutput.py' )
        else :
            trans = { 'Hlt1'   : 'PA+LU+VE+MU+HA+PH+EL'
                    , 'DEFAULT': 'PA+LU+VE'
                    , 'NONE'   : '' 
                    }
            for short,full in trans.iteritems() : hlttype = hlttype.replace(short,full)
            type2conf = { 'PA' : '$HLTCONFROOT/options/HltCommissioningLines.py' # PA for 'PAss-thru' (PT was considered bad)
                        , 'LU' : '$HLTCONFROOT/options/HltLumiLines.py'
                        , 'VE' : '$HLTCONFROOT/options/HltVeloLines.py'
                        , 'MU' : '$HLTCONFROOT/options/HltMuonLines.py' 
                        , 'HA' : '$HLTCONFROOT/options/HltHadronLines.py' 
                        , 'PH' : '$HLTCONFROOT/options/HltPhotonLines.py' 
                        , 'EL' : '$HLTCONFROOT/options/HltElectronLines.py' }
            for i in hlttype.split('+') :
                if i == 'Hlt2' : continue # we deal with this later...
                if i not in type2conf : raise AttributError, "unknown hlttype fragment '%s'"%i
                print '# requested ' + i + ', including ' + type2conf[i] 
                importOptions( type2conf[i] )
            importOptions('$HLTCONFROOT/options/HltMain.py')
            importOptions('$HLTCONFROOT/options/Hlt1.py')
            if hlttype.find('Hlt2') != -1 :   
                importOptions('$HLTCONFROOT/options/Hlt2.py')
                # TODO: this next one should become a property of the Hlt2 configurable, and we
                #       just forward to it...
                if self.getProp('Hlt2IgnoreHlt1Decision') :  
                    Sequence("Hlt2CheckHlt1Passed").Members = [ L0Filter() ]
                else : 
                    Sequence("Hlt2CheckHlt1Passed").Members = [ HltFilter('Hlt1GlobalPass' , Code = "HLT_PASS('Hlt1Global')" ) ]
            if self.getProp("verbose") : print Sequence('Hlt') 
        for userAlg in self.getProp("userAlgorithms"):
            ApplicationMgr().TopAlg += [ userAlg ]
