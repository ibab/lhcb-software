#!/usr/bin/env python
#import sys
# never search for anything from current directory...
#if sys.path[0]=='' : sys.path.pop(0)
from GaudiKernel.ProcessJobOptions import PrintOff, InstallRootLoggingHandler
import logging
PrintOff(999)
InstallRootLoggingHandler(level = logging.CRITICAL)
#import sys
# never search for anything from current directory...
#if sys.path[0]=='' : sys.path.pop(0)
from GaudiKernel.ProcessJobOptions import PrintOff, InstallRootLoggingHandler
import logging
PrintOff(999)
InstallRootLoggingHandler(level = logging.CRITICAL)


def start(**kwargs) :
    from Moore.Configuration import Moore, MooreExpert
    from MooreOnlineConf.COnfiguration import MooreOnline
    import OnlineEnv 
    
    moore = Moore()
    mooreOnline = MooreOnline()

    mooreOnline.RunOnline = True

    ### default database setup -- require an explit tag when running in the LHCb or FEST partitions...
    #not needed, moore.Simulation = False
    moore.DDDBtag    = 'unknown-please-specify-in-PVSS-RunInfo' if OnlineEnv.PartitionName in [ 'LHCb', 'FEST' ] else 'hlt-20100906'
    moore.CondDBtag  = 'unknown-please-specify-in-PVSS-RunInfo' if OnlineEnv.PartitionName in [ 'LHCb', 'FEST' ] else 'hlt-20100906'

    #default is OK moore.UseDBSnapshot = True
    #default is OK moore.IgnoreDBHeartBeat = True
    mooreOnline.EnableRunChangeHandler = ( OnlineEnv.HLTType not in ['PA','PassThrough' ] )
    mooreOnline.PartitionName = OnlineEnv.PartitionName
    #not needed any longer? MooreOnline already does this...
    moore.InitialTCK = OnlineEnv.InitialTCK
    mooreOnline.REQ1 = "EvType=1;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"
    from Configurables import MooreExpert, HltConf
    for k,v in kwargs.iteritems() :
        #iterate through the available configurables to set required properties
        found=False
        for conf in [mooreOnline, moore, MooreExpert(), HltConf()]:
            if k in conf.__slots__ or hasattr(conf,k):
                setattr(conf,k,v)
                found=True
                #only set the top-level!
                break
        if not found:
            print "# WARNING: skipping setting '"+k+":"+v+"' because no configurable has that option"
    
    
    ## TODO: is this still needed???
    #if OnlineEnv.PartitionName == 'FEST' : Moore().EnableRunChangeHandler = False
    
    ### pick up requested DB tags
    if hasattr(OnlineEnv,'CondDBTag') and OnlineEnv.CondDBTag : moore.CondDBtag = OnlineEnv.CondDBTag
    if hasattr(OnlineEnv,'DDDBTag')   and OnlineEnv.DDDBTag   : moore.DDDBtag   = OnlineEnv.DDDBTag

    # Forward all attributes of 'OnlineEnv' python module to the job options service (using the OnlineEnv configurable)...
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    c = ConfigurableGeneric("OnlineEnv")
    #[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
    c.AcceptRate = OnlineEnv.AcceptRate

    OnlineEnv.end_config(False)
