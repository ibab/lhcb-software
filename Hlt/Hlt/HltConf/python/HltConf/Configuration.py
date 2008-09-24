"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.4 2008-09-24 07:51:47 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions

class HltConf(ConfigurableUser):
    __slots__ = {
          "hltType" :          'PA+LU+VE+MU'
        , "userAlgorithms":    [ ]  # put here user algorithms to add
        , "oldStyle" :         True # old style options configuration
        }   
                
    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def setProp(self,name,value):
        return setattr(self,name,value)

    def setOtherProp(self,other,name):
        # Function to propagate properties to other component, if not already set
        if hasattr(self,name):
            if hasattr(other,name) and len(other.getProp(name)) > 0 :
                print "# %s().%s already defined, ignoring HltConf().%s"%(other.name(),name,name)
            else:
                setattr(other,name,self.getProp(name))

    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1

        importOptions('$HLTCONFROOT/options/HltInit.opts')
        hlttype = self.getProp('hltType')
        if self.getProp('oldStyle') :
            if hlttype not in self.validHltTypes() :  raise TypeError("Unknown hlttype '%s'"%hlttype)
            if hlttype.find('Hlt1') != -1 :   importOptions('$HLTCONFROOT/options/Hlt1.opts')
            if hlttype.find('Hlt2') != -1 :   importOptions('$HLTCONFROOT/options/Hlt2.opts')
            if hlttype ==  'DEFAULT'      :   importOptions('$HLTCONFROOT/options/RandomPrescaling.opts')
            if hlttype == 'readBackLumi'  :   importOptions('$HLTCONFROOT/options/HltJob_readLumiPy.opts')
            if hlttype == 'writeLumi'     :   importOptions('$HLTCONFROOT/options/HltJob_onlyLumi.opts')
            if hlttype.find('Lumi') != -1 :   importOptions('$HLTCONFROOT/options/Lumi.opts')
            if hlttype.find('Velo') != -1 :   importOptions('$HLTCONFROOT/options/HltVeloAlleySequence.opts')
        else :
            if hlttype == 'DEFAULT'       : hlttype = 'PA+LU+VE'
            if hlttype == 'HLT1'          : hlttype = 'PA+LU+VE+MU+HA+PH+EL'
            type2conf = { 'PA' : '$HLTCONFROOT/options/HltCommissioningLines.py' # PA for 'Pass-Thru' (PT was considered bad)
                        , 'LU' : '$HLTCONFROOT/options/HltLumiLines.py'
                        , 'VE' : '$HLTCONFROOT/options/HltVeloLines.py'
                        , 'MU' : '$HLTCONFROOT/options/HltMuonLines.py' 
                        , 'HA' : '$HLTCONFROOT/options/HltHadronLines.py' 
                        , 'PH' : '$HLTCONFROOT/options/HltPhotonLines.py' 
                        , 'EL' : '$HLTCONFROOT/options/HltElectronLines.py' }
            for i in hlttype.split('+') :
                if i not in type2conf : raise AttributError, "unknown hlttype fragment '%s'"%i
                print '# requested ' + i + ', including ' + type2conf[i] 
                importOptions( type2conf[i] )
            importOptions('$HLTCONFROOT/options/HltMain.py')
            importOptions('$HLTCONFROOT/options/Hlt1.py')
        for userAlg in self.getProp("userAlgorithms"):
            ApplicationMgr().TopAlg += [ userAlg ]
