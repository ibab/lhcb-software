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


def start(NbOfSlaves) :
    from Moore.Configuration import Moore
    import OnlineEnv 

    Moore().RunOnline = True
    Moore().NbOfSlaves = NbOfSlaves

    ### default database setup -- require an explit tag when running in the LHCb or FEST partitions...
    Moore().Simulation = False
    Moore().DDDBtag    = 'unknown-please-specify-in-PVSS-RunInfo' if OnlineEnv.PartitionName in [ 'LHCb', 'FEST' ] else 'hlt-20100906'
    Moore().CondDBtag  = 'unknown-please-specify-in-PVSS-RunInfo' if OnlineEnv.PartitionName in [ 'LHCb', 'FEST' ] else 'hlt-20100906'

    Moore().UseDBSnapshot = True
    Moore().IgnoreDBHeartBeat = True
    Moore().EnableRunChangeHandler = ( OnlineEnv.HLTType not in ['PA','PassThrough' ] )
    Moore().PartitionName = OnlineEnv.PartitionName
    Moore().InitialTCK = OnlineEnv.InitialTCK
    Moore().REQ1 = "EvType=1;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"


    ## TODO: is this still needed???
    if OnlineEnv.PartitionName == 'FEST' : Moore().EnableRunChangeHandler = False

    ### pick up requested DB tags
    if hasattr(OnlineEnv,'CondDBTag') and OnlineEnv.CondDBTag : Moore().CondDBtag = OnlineEnv.CondDBTag
    if hasattr(OnlineEnv,'DDDBTag')   and OnlineEnv.DDDBTag   : Moore().DDDBtag   = OnlineEnv.DDDBTag

    # Forward all attributes of 'OnlineEnv' to the job options service...
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    c = ConfigurableGeneric("OnlineEnv")
    #[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
    c.AcceptRate = OnlineEnv.AcceptRate

    OnlineEnv.end_config(False)
