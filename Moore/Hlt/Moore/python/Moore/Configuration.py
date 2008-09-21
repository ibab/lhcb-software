"""
High level configuration tools for Moore
"""
__version__ = "$Id: Configuration.py,v 1.28 2008-09-21 16:38:42 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Configurables import ConfigFileAccessSvc, ConfigDBAccessSvc, HltConfigSvc, HltGenConfig
from Configurables import EventClockSvc

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint

class Moore(ConfigurableUser):
    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "DAQStudies":        False # use DAQ study version of options
        , "DDDBtag" :          '2008-default'
        , "condDBtag" :        '2008-default'
        , "inputType":         'dst' # must either 'mdf' or 'dst'
        , "DC06" :             False # use DC06 setup
        , "hltType" :          'PA+LU+VE'
        , "runTiming"  :       False # include additional timing information
        , "useTCK"     :       False # use TCK instead of options...
        , "prefetchTCK" :      [ ] # which TCKs to prefetch. Initial TCK used is first one...
        , "generateConfig" :   False # whether or not to generate a configuration
        , "configLabel" :      ''    # label for generated configuration
        , "TCKData" :          '$TCKDATAROOT' # where do we read TCK data from?
        , "TCKpersistency" :   'file' # which method to use for TCK data? valid is 'file' and 'sqlite'
        , "enableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
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
                print "# %s().%s already defined, ignoring Moore().%s"%(other.name(),name,name)
            else:
                setattr(other,name,self.getProp(name))

    def validHltTypes(self):
        return [ 'PA'
                 'PA+LU', 
                 'PA+LU+VE', 
                 'PA+LU+VE+MU', 
                 'ALL_HLT1', 
                 'ALL_HLT1+HLT2', 
                 'DEFAULT' ] 

    def getRelease(self):
        import re,fileinput
        version = re.compile('^version (\w+)')
        for line in fileinput.input(os.environ.get('MOOREROOT')+'/cmt/requirements') :
            match = version.match(line)
            if not match: continue
            fileinput.close()
            return 'MOORE_'+match.group(1)
        raise TypeError('Could not deduce version from cmt/requirementes')

    def getConfigAccessSvc(self):
        method = self.getProp('TCKpersistency').lower()
        if method not in [ 'file', 'sqlite' ] : raise TypeError("invalid TCK persistency '%s'"%method)
        TCKData = self.getProp('TCKData')
        if method == 'file' :
            return ConfigFileAccessSvc( Directory = TCKData +'/config' )
        if method == 'sqlite' :
            return ConfigDBAccessSvc( Connection = 'sqlite_file:' + TCKData +'/db/config.db' )

    def addAuditor(self,x) :
        AuditorSvc().Auditors.append( x.name() )
        x.Enable = True

    def printAlgo( self, algName, appMgr, prefix = ' ') :
        #""" print algorithm name, and, if it is a sequencer, recursively those algorithms it calls"""
        print prefix + algName
        alg = appMgr.algorithm( algName.split( "/" )[ -1 ] )
        prop = alg.properties()
        if prop.has_key( "Members" ) :
            subs = prop[ "Members" ].value()
            for i in subs : self.printAlgo( i.strip( '"' ), appMgr, prefix + "     " )
        elif prop.has_key( "DetectorList" ) :
            subs = prop[ "DetectorList" ].value()
            for i in subs : self.printAlgo( algName.split( "/" )[ -1 ] + i.strip( '"' ) + "Seq", appMgr, prefix + "     ")

    def printFlow( self ) :
        mp = ApplicationMgr()
        print "\n ****************************** Application Flow ****************************** \n"
        for i in mp.TopAlg: self.printAlgo( i, mp )
        print "\n ****************************************************************************** \n"

        # Print all configurables
        if self.getProp( "OutputLevel" ) == DEBUG :
            from pprint import PrettyPrinter
            print "\n ************************** DEBUG: All Configurables ************************** \n"
            PrettyPrinter().pprint(allConfigurables)
            print "\n ****************************************************************************** \n"

    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1
        importOptions('$STDOPTS/DstDicts.opts')

        inputType = self.getProp('inputType').upper()
        if inputType not in [ 'MDF','DST','RAW' ] : raise TypeError("Invalid input type '%s'"%inputType)
        if inputType != 'DST' : 
            EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
        importOptions('$STDOPTS/DecodeRawEvent.opts')
        ApplicationMgr().ExtSvc.append(  "DataOnDemandSvc"   ); # needed for DecodeRawEvent...

        # forward some other settings... TODO: make a dictionary..
        self.setOtherProp( LHCbApp(), 'useOracleCondDB' )
        importOptions( "$DDDBROOT/options/DDDB.py" )
        if self.getProp('DC06') :
            importOptions( '$DDDBROOT/options/DC06.opts' )
        else : 
            # the next is for 2008 data & MC, i.e. NOT for DC'06
            importOptions( '$DDDBROOT/options/LHCb-2008.py' )
        self.setOtherProp( LHCbApp(), 'DDDBtag' )
        self.setOtherProp( LHCbApp(), 'condDBtag' )
        self.setOtherProp( LHCbApp(), 'skipEvents' )
        self.setOtherProp( ApplicationMgr(), 'EvtMax' )
        # Get the event time (for CondDb) from ODIN 
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        # make sure we don't pick up small variations of the read current
        MagneticFieldSvc().UseSetCurrent = True
        # output levels...
        ToolSvc().OutputLevel                     = INFO
        from Configurables import XmlParserSvc 
        XmlParserSvc().OutputLevel                = WARNING
        MessageSvc().OutputLevel                  = INFO
        ApplicationMgr().OutputLevel              = ERROR
        SequencerTimerTool().OutputLevel          = WARNING
        # Print algorithm name with 40 characters
        MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'
        # Profiling
        for i in [ 'ToolSvc' , 'AuditorSvc' ] : ApplicationMgr().ExtSvc.append( i ) 
        ApplicationMgr().AuditAlgorithms = 1
        AuditorSvc().Auditors.append( 'TimingAuditor/TIMER' )
        for i in self.getProp('enableAuditor') : self.addAuditor( i )
        # TODO: check for mutually exclusive options...
        if self.getProp('useTCK') :
            tcks = [ int(i) for i in self.getProp('prefetchTCK') ]
            cfg = HltConfigSvc( prefetchTCK = tcks
                              , initialTCK = tcks[0]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() ) 
            ApplicationMgr().ExtSvc.append(cfg.getFullName())
        else:
            if self.getProp("DAQStudies") :
                importOptions('$HLTCONFROOT/options/L0DAQ.opts')
                importOptions('$HLTCONFROOT/options/HltDAQ.opts')
            else :
                importOptions('$HLTCONFROOT/options/HltInit.opts')
                if inputType == 'DST' : # and hlttype in [ 'PHYSICS_Hlt1+Hlt2', 'PHYSICS_Hlt1' , 'PHYSICS_Lumi', 'Lumi'] : 
                    importOptions('$L0DUROOT/options/ReplaceL0BanksWithEmulated.opts')
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
                                , 'EL' : '$HLTCONFROOT/options/HltElectronLines.py' 
                                }
                    for i in hlttype.split('+') :
                        if i not in type2conf : raise AttributError, "unknown hlttype fragment '%s'"%i
                        print i + '->' + type2conf[i]
                        importOptions( type2conf[i] )
                    importOptions('$HLTCONFROOT/options/HltMain.py')
                    importOptions('$HLTCONFROOT/options/Hlt1.py')
            if self.getProp('runTiming') :
                importOptions('$HLTCONFROOT/options/HltAlleysTime.opts')
                importOptions('$HLTCONFROOT/options/HltAlleysHistos.opts')

        if self.getProp("userAlgorithms"):
            for userAlg in self.getProp("userAlgorithms"):
                ApplicationMgr().TopAlg += [ userAlg ]
            
        if self.getProp("generateConfig") :
            # TODO: add properties for ConfigTop and ConfigSvc...
            
            gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] if type(i) is str else i.getName() for i in ApplicationMgr().TopAlg ]
                              , ConfigSvc = [ 'ToolSvc','HltDataSvc','HltANNSvc' ]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getName()
                              , hltType = self.getProp('hltType')
                              , mooreRelease = self.getRelease()
                              , label = self.getProp('configLabel'))
            # make sure gen is the very first Top algorithm...
            ApplicationMgr().TopAlg = [ gen.getFullName() ] + ApplicationMgr().TopAlg

            
        LHCbApp().applyConf()
