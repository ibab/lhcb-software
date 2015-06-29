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
        ############################################
        # Options specific to the Online environment
        ############################################
        "NbOfSlaves":        0
        , 'EnableMonitoring' : False
        , "RunMonitoringFarm" : False
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
        , 'DataType' : '2012'
        , "CheckOdin" : True
        , "HltLevel" : 'Hlt1'
        , 'Priority' : {'Hlt1' : -10, 'Hlt2' : 10, 'Hlt1Hlt2' : 0}
        }

    def _configureDBSnapshot(self):
        from Configurables import CondDB
        conddb = CondDB()
        conddb.Tags["ONLINE"]='fake'
        conddb.setProp('IgnoreHeartBeat',self.getProp('IgnoreDBHeartBeat')  )
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

        #from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc
        #done in LHCbApp
        #EventPersistencySvc().CnvServices.append( RawDataCnvSvc('RawDataCnvSvc') )
        EventLoopMgr().Warnings = False

        from Configurables import MonitorSvc
        MonitorSvc().disableDimPropServer      = 1
        MonitorSvc().disableDimCmdServer       = 1
        MonitorSvc().disableMonRate            = 0
        MonitorSvc().CounterUpdateInterval     = 15

        app=ApplicationMgr()

        # setup the histograms and the monitoring service
        from Configurables import UpdateAndReset
        if not self.getProp('Simulation'):
            app.TopAlg = [ UpdateAndReset() ] + app.TopAlg
        app.ExtSvc.append( 'MonitorSvc' )
        HistogramPersistencySvc().OutputFile = ''
        HistogramPersistencySvc().Warnings = False
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc().OutputEnabled = False

        # set up the event selector
        if 'EventSelector' in allConfigurables :
            del allConfigurables['EventSelector']

        if not self.getProp('RunMonitoringFarm') :
            ## Setup Checkpoint & forking: Do this EXACTLY here. Just befor the MEPManager & event selector.
            ## It will not work if these are created before.

            if OnlineEnv.MooreStartupMode == 1:
                self._configureOnlineForking()
            elif OnlineEnv.MooreStartupMode == 2:
                self._configureOnlineCheckpointing()


            importOptions('$MBM_SETUP_OPTIONS')
            mbm_setup = allConfigurables['OnlineEnv']
            task_type = os.environ['TASK_TYPE']

            ## The next four lines are a temporary hack for the timing tests. It
            ## will be removed once the permanent solution for the
            ## CheckPointingSvc is in place.
            import re
            m = re.match("(.*)_(?:\\d+)$", task_type)
            if m:
                task_type = m.group(1)

            input_buffer  = mbm_setup.__getattribute__(task_type + '_Input')   #'Events'
            output_buffer = mbm_setup.__getattribute__(task_type + '_Output')  #'Send'

            TAE = (OnlineEnv.TAE != 0)
            mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID, OnlineEnv.PartitionName,
                                          [input_buffer, output_buffer], False)
            mepMgr.PartitionBuffers = True
            mepMgr.PartitionName    = OnlineEnv.PartitionName
            mepMgr.PartitionID      = OnlineEnv.PartitionID
            mepMgr.ConnectWhen      = 'start'

            app.Runable = OnlineEnv.evtRunable(mepMgr)
            app.ExtSvc.append(mepMgr)
            evtMerger = OnlineEnv.evtMerger(name = 'Output', buffer = output_buffer,
                                            location = 'DAQ/RawEvent', datatype = OnlineEnv.MDF_NONE,
                                            routing = 1)
            evtMerger.DataType = OnlineEnv.MDF_BANKS

            if TAE:
                 eventSelector = OnlineEnv.mbmSelector(input = input_buffer, TAE = TAE, decode = False)
            elif self.getProp('HltLevel') == "Hlt2" or self.getProp('ForceMDFInput'):
                 eventSelector = OnlineEnv.mbmSelector(input = input_buffer, TAE = TAE, decode = False)  # decode=False for HLT2 ONLY!!!!!
            else:
                 eventSelector = OnlineEnv.mbmSelector(input = input_buffer, TAE = TAE, decode = True)
            app.ExtSvc.append(eventSelector)

            OnlineEnv.evtDataSvc()
            if self.getProp('REQ1') : eventSelector.REQ1 = self.getProp('REQ1')

            # define the send sequence
            writer =  GaudiSequencer('SendSequence')
            writer.OutputLevel = OnlineEnv.OutputLevel
            if len(Moore().getProp('WriterRequires')):
                from Configurables import LoKi__VoidFilter as Filter
                writer.Members.append( Filter( "SendSequenceFilter"
                                             , Preambulo = [ 'from LoKiHlt.algorithms import ALG_EXECUTED, ALG_PASSED' ]
                                             , Code = ' & '.join( [ "ALG_EXECUTED('{0}') & ALG_PASSED('{0}')".format(i) for i in Moore().getProp('WriterRequires') ] )
                                             )
                                     )
            writer.Members.append(evtMerger)
            app.OutStream.append( writer )
        else :
            input = 'Events'
            mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,[input],True)
            app.Runable = OnlineEnv.evtRunable(mepMgr)
            app.ExtSvc.append(mepMgr)
            eventSelector = OnlineEnv.mbmSelector(input=input,decode=False)
            app.ExtSvc.append(eventSelector)
            OnlineEnv.evtDataSvc()
            if self.getProp('REQ1') : eventSelector.REQ1 = self.getProp('REQ1')

        #ToolSvc.SequencerTimerTool.OutputLevel = @OnlineEnv.OutputLevel;
        from Configurables import AuditorSvc
        AuditorSvc().Auditors = []
        # Now setup the message service
        self._configureOnlineMessageSvc()

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
        if hasattr(OnlineEnv,'InitialTCK') and self.getProp('HltLevel') != "Hlt2":
            Moore().setProp('InitialTCK',OnlineEnv.InitialTCK)
            if not self._setIfNotSet('CheckOdin',True) :
                print 'WARNING '*10
                print 'WARNING: you are running online, using a TCK, but ignoring ODIN. Are you sure this is really what you want???'
                print 'WARNING '*10

    def _setIfNotSet(self,prop,value) :
        if not self.isPropertySet(prop) : self.setProp(prop,value)
        return self.getProp(prop) == value

    def forceOtherProps(self, other, props):
        """
        Force overwriting properties in online mode
        """
        for prop in props:
            if other.isPropertySet(prop) and other.getProp(prop)!=self.getProp(prop):
                log.warning("Resetting "+prop+" even though you set is elsewhere!")
            other.setProp(prop,self.getProp(prop))

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()

        propagator=None
        if self.getProp("RunOnline") :
            #check and reconfigure own properties
            self._onlineMode()
            #in online mode, force certain Moore properties!
            propagator=self.forceOtherProps
        else:
            #in offline mode don't force, but set them if not set
            propagator=self.setOtherProps

        #pass certain properties to Moore
        propagator(Moore(), ["RunOnline","UseTCK",'EnableTimer','Simulation','DataType','CheckOdin'] )

        if self.getProp("UseDBSnapshot"): self._configureDBSnapshot()

        self._configureOnline()
