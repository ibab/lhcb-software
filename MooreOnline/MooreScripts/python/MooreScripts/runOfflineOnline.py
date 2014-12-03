#!/usr/bin/env python
def configure(**kwargs) :
    # Add some expected stuff to OnlineEnv
    import OnlineEnv
    OnlineEnv.MooreStartupMode = 1 # 1 is forking
    OnlineEnv.TAE = 0
    OnlineEnv.PartitionID = 0
    OnlineEnv.PartitionIDName = "0000"
    OnlineEnv.PartitionName = kwargs.pop('PartitionName')
    OnlineEnv.OutputLevel = kwargs.pop('OutputLevel', 4)

    moore_tests = __import__("MooreTests" , globals(), locals(), [kwargs.get('UserPackage')])
    user_package = getattr(moore_tests, kwargs.pop('UserPackage'))

    from Moore.Configuration import Moore, MooreExpert
    from MooreOnlineConf.Configuration import MooreOnline

    moore = Moore()
    moore.OutputLevel = OnlineEnv.OutputLevel
    if OnlineEnv.OutputLevel < 4:
        moore.EnableTimer = True
    # We need MooreOnline to setup the buffer manager infrastructure etc, but we
    # don't want to use things like the RunChangeHandler and database snapshots.
    mooreOnline = MooreOnline()
    mooreOnline.RunOnline = False
    mooreOnline.EnableRunChangeHandler = None
    mooreOnline.UseDBSnapshot = False
    mooreOnline.EnableTimer = False
    mooreOnline.CheckOdin = False

    # This is the stuff that should come from the PRConfig module
    for conf, d in { moore: {'DDDBtag' : str, 'CondDBtag' : str},
                     mooreOnline : {'UseTCK' : bool, 'Simulation' : bool,
                                    'DataType' : ('2011', '2012', '2013', '2014'),
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
                        
    if mooreOnline.HltLevel in ["Hlt1", "Hlt1Hlt2"]:
        mooreOnline.REQ1 = "EvType=1;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"
    else:
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

    import OnlineEnv 
    OnlineEnv.end_config(False)
