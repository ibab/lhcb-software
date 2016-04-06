
"""
High level configuration tool(s) for MooreOnline
"""


from os import environ, path
from LHCbKernel.Configuration import *
from Gaudi.Configuration import *
from Configurables import Moore, CondDB

import GaudiKernel.ProcessJobOptions

class MooreOnline(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ Moore, CondDB ]
    __slots__ = {
          '__buffers' : []
        ############################################
        # Options specific to the Online environment
        ############################################
        , "NbOfSlaves":        0
        , 'EnableMonitoring' : False
        , 'REQ1' : ''
        , 'ForceMDFInput' : False
        ################################################################
        # Options to Setup the online database, propagated to CondDB
        ################################################################
        , "UseDBSnapshot"     : True
        , 'EnableRunChangeHandler' : True
        , "DBSnapshotDirectory" : "/group/online/hlt/conditions"
        , "RunChangeHandlerConditions" : {  'online_%d.xml' :  [ "Conditions/Online/LHCb/Magnet/Set"
                                         , "Conditions/Online/Velo/MotionSystem"
                                         , "Conditions/Online/LHCb/Lumi/LumiSettings"
                                         , "Conditions/Online/LHCb/LHCFillingScheme"
                                         , "Conditions/Online/LHCb/RunParameters"
                                         , "Conditions/Online/Rich1/R1HltGasParameters"
                                         , "Conditions/Online/Rich2/R2HltGasParameters" ] }
        , 'IgnoreDBHeartBeat'  : True
        ################################################################
        # Options to Moore which have a different default in online mode
        # Or that need to be forced in online mode
        ################################################################
        , "RunOnline"   :       True
        , "UseTCK"     :       True
        , 'EnableTimer' :    False
        , 'Simulation': False
        , 'DataType' : '2016'
        , "CheckOdin" : True
        , "HltLevel" : 'Hlt1'
        , 'EnableUpdateAndReset' : True
    }

    def _configureDBSnapshot(self):
        from Configurables import CondDB
        conddb = CondDB()
        conddb.EnableRunStampCheck = False
        conddb.Tags["ONLINE"]='fake'
        conddb.setProp('IgnoreHeartBeat', self.getProp('IgnoreDBHeartBeat'))
        self.setOtherProps( conddb, [ 'UseDBSnapshot',
                                      'DBSnapshotDirectory',
                                      'EnableRunChangeHandler',
                                      'RunChangeHandlerConditions' ])

        # https://savannah.cern.ch/bugs/?94454#comment12
        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True

    def _configureOnline(self) :
        from Configurables import LoKiSvc
        LoKiSvc().Welcome = False

        import OnlineEnv
        EventLoopMgr().Warnings = False

        app = ApplicationMgr()

        # setup the histograms and the monitoring service
        from Configurables import MonitorSvc
        MonitorSvc().disableDimPropServer  = 1
        MonitorSvc().disableDimCmdServer   = 1
        MonitorSvc().disableMonRate        = 0
        MonitorSvc().CounterUpdateInterval = 15
        app.ExtSvc.append( 'MonitorSvc' )

        if self.getProp('EnableUpdateAndReset'):
            from Configurables import UpdateAndReset
            if not self.getProp('Simulation'):
                app.TopAlg = [ UpdateAndReset() ] + app.TopAlg

        HistogramPersistencySvc().OutputFile = ''
        HistogramPersistencySvc().Warnings = False
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc().OutputEnabled = False

        # set up the event selector
        if 'EventSelector' in allConfigurables :
            del allConfigurables['EventSelector']

        # Enable Hlt2 ZeroMQ based monitoring
        if self.getProp("HltLevel") == "Hlt2":
            app.ExtSvc += [ 'HltMonitorSvc/Hlt2MonSvc', 'HltInfoSvc' ]

        ## Setup Checkpoint & forking: Do this EXACTLY here. Just befor the MEPManager & event selector.
        ## It will not work if these are created before.
        if OnlineEnv.MooreStartupMode == 1:
            self._configureOnlineForking()
        elif OnlineEnv.MooreStartupMode == 2:
            self._configureOnlineCheckpointing()

        importOptions('$MBM_SETUP_OPTIONS')
        mbm_setup = allConfigurables['OnlineEnv']
        task_type = os.environ['TASK_TYPE']

        input_buffer  = mbm_setup.__getattribute__(task_type + '_Input')   #'Events'
        output_buffer = mbm_setup.__getattribute__(task_type + '_Output')  #'Send'
        self.__buffers = [input_buffer, output_buffer]
        TAE = (OnlineEnv.TAE != 0)
        mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID, OnlineEnv.PartitionName,
                                      self.__buffers, False)
        mepMgr.PartitionBuffers = True
        mepMgr.PartitionName    = OnlineEnv.PartitionName
        mepMgr.PartitionID      = OnlineEnv.PartitionID
        mepMgr.ConnectWhen      = 'start'

        app.Runable = OnlineEnv.evtRunable(mepMgr)
        app.ExtSvc.append(mepMgr)

        if TAE:
             eventSelector = OnlineEnv.mbmSelector(input = input_buffer, TAE = TAE, decode = False)
        elif self.getProp('HltLevel') == "Hlt2" or self.getProp('ForceMDFInput'):
             eventSelector = OnlineEnv.mbmSelector(input = input_buffer, TAE = TAE, decode = False)  # decode=False for HLT2 ONLY!!!!!
        else:
             eventSelector = OnlineEnv.mbmSelector(input = input_buffer, TAE = TAE, decode = True)
        app.ExtSvc.append(eventSelector)

        OnlineEnv.evtDataSvc()
        if self.getProp('REQ1') : eventSelector.REQ1 = self.getProp('REQ1')

        from Configurables import AuditorSvc
        AuditorSvc().Auditors = []
        # Now setup the message service
        self._configureOnlineMessageSvc()

    def rawEventMergerProperties(self):
        import OnlineEnv
        iProps = {'Buffer' : self.__buffers[1],
                  "Compress" : "0",
                  "DataType" : str(OnlineEnv.MDF_BANKS),
                  "InputDataType" : str(OnlineEnv.MDF_NONE),
                  "RoutingBits" : "1",
                  "Silent"      : "false",
                  "OutputLevel" : str(OnlineEnv.OutputLevel)}
        gProps = {"BankLocation" : "DAQ/%sRawEvent"}

        return "LHCb::RawEvent2MBMMergerAlg", iProps, gProps, {}

    def _configureOnlineForking(self):
        import os, socket, OnlineEnv
        from Configurables import LHCb__CheckpointSvc
        numChildren = os.sysconf('SC_NPROCESSORS_ONLN')
        host = socket.gethostname().split('.')[0].upper()
        #if host[:3]=='HLT':
        #  if host[3]=='F': numChildren=23
        #  if host[3]!='F':
        #    if len(host)==8:
        #      id = int(host[6:])
        #      if id < 12: numChildren=9
        #      if id > 11: numChildren=23
        if self.getProp("NbOfSlaves") > 0 : numChildren = self.getProp("NbOfSlaves")

        forker = LHCb__CheckpointSvc("CheckpointSvc")
        forker.NumberOfInstances   = numChildren
        forker.Partition           = OnlineEnv.PartitionName
        forker.TaskType            = os.environ['TASK_TYPE'] ###'GauchoJob'
        forker.UseCores            = False
        forker.ChildSessions       = False
        forker.FirstChild          = 0
        # Sleep in [ms] for each child in batches of 10:
        forker.ChildSleep          = 500;
        forker.UtgidPattern        = "%UTGID%02d" #"%P_%NN_%T_%02d";
        forker.PrintLevel          = 3  # 1=MTCP_DEBUG 2=MTCP_INFO 3=MTCP_WARNING 4=MTCP_ERROR
        forker.OutputLevel         = 4  # 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
        ApplicationMgr().ExtSvc.insert(0,forker)

    def _configureOnlineCheckpointing(self):
        pass

    def _configureOnlineMessageSvc(self):
        # setup the message service
        from Configurables import LHCb__FmcMessageSvc as MessageSvc
        if 'MessageSvc' in allConfigurables :
            del allConfigurables['MessageSvc']
        msg=MessageSvc('MessageSvc')
        app=ApplicationMgr()
        app.MessageSvcType = msg.getType()
        app.SvcOptMapping.append( msg.getFullName() )
        msg.LoggerOnly = True
        if 'LOGFIFO' not in os.environ :
            os.environ['LOGFIFO'] = '/tmp/logGaudi.fifo'
            log.warning( '# WARNING: LOGFIFO was not set -- now set to ' + os.environ['LOGFIFO'] )
        msg.fifoPath = os.environ['LOGFIFO']
        import OnlineEnv
        msg.OutputLevel = OnlineEnv.OutputLevel
        #msg.OutputLevel = '2'
        msg.doPrintAlways = False

    def _checkSetDefault(self,prop):
        """
        check a given slot, and reset to default. Print a warning if necessary
        """
        default=self.getDefaultProperty(prop)
        if not ( self.getProp(prop)==self.getDefaultProperty(prop) ):
            log.warning("# WARNING: resetting "+conf+" to default: "+str(default))
            self.setProp(conf, default)

    def _onlineMode(self):
        """
        Check and set various things if runOnline is True
        """
        import OnlineEnv
        #don't allow these to be overwritten
        self._checkSetDefault('EnableTimer')
        self._checkSetDefault('UseTCK')
        self._checkSetDefault('Simulation')
        self._checkSetDefault('DataType')
        self._checkSetDefault('UseDBSnapshot')

        ### TODO: see if 'OnlineEnv' has InitialTCK attibute. If so, set it
        ## in case of LHCb or FEST, _REQUIRE_ it exists...
        if hasattr(OnlineEnv,'InitialTCK'):
            Moore().setProp('InitialTCK', OnlineEnv.InitialTCK)
            if not self._setIfNotSet('CheckOdin', True) and self.getProp('HltLevel') != "Hlt2":
                print 'WARNING '*10
                print ('WARNING: you are running HLT1 or HLT1HLT2 online, using a TCK, but ignoring ODIN. '
                       'Are you sure this is really what you want???')
                print 'WARNING '*10

    def _setIfNotSet(self,prop,value) :
        if not self.isPropertySet(prop) : self.setProp(prop,value)
        return self.getProp(prop) == value

    def forceOtherProps(self, other, props, force):
        """
        Force overwriting properties in online mode
        """
        for prop in props:
            if other.isPropertySet(prop) and other.getProp(prop) != self.getProp(prop) and force:
                log.warning("Resetting "+prop+" even though you set it elsewhere!")
            if not other.isPropertySet(prop) or force:
                other.setProp(prop,self.getProp(prop))


    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        from functools import partial

        propagator = None
        if self.getProp("RunOnline") :
            #check and reconfigure own properties
            self._onlineMode()
            #in online mode, force certain Moore properties!
            propagator = partial(self.forceOtherProps, force = True)
        else:
            #in offline mode don't force, but set them if not set
            propagator = partial(self.forceOtherProps, force = False)

        #pass certain properties to Moore
        propagator(Moore(), ["RunOnline","UseTCK",'EnableTimer','Simulation','DataType','CheckOdin'] )

        # Use the online output algorithm
        from Configurables import HltOutputConf
        HltOutputConf().setProp("OutputAlgProperties", self.rawEventMergerProperties)

        from Configurables import LoKiSvc, LoKi__Reporter
        LoKiSvc().addTool(LoKi__Reporter, 'REPORT')
        LoKiSvc().REPORT.OutputLevel = 5

        if self.getProp("UseDBSnapshot"): self._configureDBSnapshot()

        self._configureOnline()
