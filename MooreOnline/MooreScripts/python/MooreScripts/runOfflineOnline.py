#!/usr/bin/env python
import os

def configure(**kwargs) :
    # Add some expected stuff to OnlineEnv
    import OnlineEnv
    OnlineEnv.MooreStartupMode = 1 # 1 is forking
    OnlineEnv.TAE = 0
    OnlineEnv.PartitionID = 0
    OnlineEnv.PartitionIDName = "0000"
    OnlineEnv.PartitionName = kwargs.pop('PartitionName')
    from Gaudi.Configuration import INFO, WARNING
    output_level = kwargs.pop('OutputLevel', WARNING)
    OnlineEnv.OutputLevel = output_level

    moore_tests = __import__("MooreTests" , globals(), locals(), [kwargs.get('UserPackage')])
    user_package = getattr(moore_tests, kwargs.pop('UserPackage'))
    input_type = kwargs.pop('InputType', 'MEP')

    # Only a single setting directly for Moore
    from Moore.Configuration import Moore, MooreExpert
    moore = Moore()
    moore.OutputLevel = output_level

    # We need MooreOnline to setup the buffer manager infrastructure etc, but we
    # don't want to use things like the RunChangeHandler and database snapshots.
    from MooreOnlineConf.Configuration import MooreOnline
    mooreOnline = MooreOnline()
    mooreOnline.RunOnline = False
    mooreOnline.EnableTimer = False
    mooreOnline.EnableRunChangeHandler = None
    mooreOnline.UseDBSnapshot = False
    mooreOnline.CheckOdin = False

    ### Process priority
    nice = mooreOnline.getProp("Priority")[HltLevel]
    os.nice(nice)

    # Add the timing auditor by hand with output level INFO, as Moore is never
    # going to do it for us if the rest is at WARNING
    from Gaudi.Configuration import ApplicationMgr, AuditorSvc
    from Configurables import LHCbTimingAuditor, LHCbSequencerTimerTool
    ApplicationMgr().AuditAlgorithms = 1
    ta = LHCbTimingAuditor('TIMER')
    ta.OutputLevel = INFO
    ta.addTool(LHCbSequencerTimerTool, name = 'TIMER')
    ta.TIMER.NameSize = 90
    ta.TIMER.OutputLevel = INFO
    if 'AuditorSvc' not in ApplicationMgr().ExtSvc:
        ApplicationMgr().ExtSvc.append('AuditorSvc')
    AuditorSvc().Auditors.append(ta)
    ta.Enable = True

    # Hack the shit out of the CondDB services to stop them from spawning a
    # thread that will segfault on finalize with forking
    def no_timeout():
        from Gaudi.Configuration import allConfigurables
        from Configurables import CondDBAccessSvc
        for conf in allConfigurables.itervalues():
            if type(conf) == CondDBAccessSvc:
                conf.ConnectionTimeOut = 0

    from Gaudi.Configuration import appendPostConfigAction
    appendPostConfigAction(no_timeout)

    ## from Gaudi.Configuration import appendPostConfigAction
    ## def info_dammit():
    ##     from Configurables import LHCbTimingAuditor, LHCbSequencerTimerTool
    ##     ta = LHCbTimingAuditor('TIMER')
    ##     ta.OutputLevel = OnlineEnv.OutputLevel
    ##     ta.addTool(LHCbSequencerTimerTool, name = 'TIMER')
    ##     ta.TIMER.OutputLevel = OnlineEnv.OutputLevel
    ## appendPostConfigAction(info_dammit)

    # This is the stuff that should come from the PRConfig user module
    for conf, d in { moore: {'DDDBtag' : str, 'CondDBtag' : str},
                     mooreOnline : {'UseTCK' : bool, 'Simulation' : bool,
                                    'DataType' : ('2011', '2012', '2013', '2014', '2015'),
                                    'HltLevel' : ('Hlt1', 'Hlt2', 'Hlt1Hlt2')}}.iteritems():
        for a, t in d.iteritems():
            ua = user_package.MooreOptions.pop(a)
            if hasattr(t, '__iter__'):
                assert(ua in t)
            else:
                assert(type(ua) == t)
            conf.setProp(a, ua)
    if 'InitialTCK' in user_package.MooreOptions:
        moore.setProp('InitialTCK', user_package.MooreOptions['InitialTCK'])

    if input_type == 'MEP' and 'Hlt1' in mooreOnline.HltLevel:
        mooreOnline.REQ1 = "EvType=1;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"
    elif input_type == 'MDF' and 'Hlt1' in mooreOnline.HltLevel:
        mooreOnline.ForceMDFInput = True
        mooreOnline.REQ1 = "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"
    elif mooreOnline.HltLevel == 'Hlt2':
        mooreOnline.REQ1 = "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"

    # Apparently we need to set this, otherwise something goes wrong with
    # default properties being retrieved that have the wrong type.
    from Gaudi.Configuration import EventSelector
    moore.inputFiles = [ ]
    EventSelector().Input = []

    # Extra options
    from Configurables import MooreExpert
    args = user_package.MooreOptions
    args.update(kwargs)
    for k,v in args.iteritems():
        #iterate through the available configurables to set required properties
        found=False
        for conf in [mooreOnline, moore, MooreExpert()]:
            if k in conf.__slots__ or hasattr(conf,k):
                conf.setProp(k,v)
                found = True
                break
        if not found:
            print "# WARNING: skipping setting '"+k+":"+v+"' because no configurable has that option"

    user_package.configure()

    OnlineEnv.end_config(False)
