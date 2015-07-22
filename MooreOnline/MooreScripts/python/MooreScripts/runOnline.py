#!/usr/bin/env python
#import sys
# never search for anything from current directory...
#if sys.path[0]=='' : sys.path.pop(0)
import os, logging
from GaudiKernel.ProcessJobOptions import PrintOff, InstallRootLoggingHandler
PrintOff(999)
InstallRootLoggingHandler(level = logging.CRITICAL)
#import sys
# never search for anything from current directory...
#if sys.path[0]=='' : sys.path.pop(0)
#from GaudiKernel.ProcessJobOptions import PrintOff, InstallRootLoggingHandler
#import logging
#PrintOff(999)
#InstallRootLoggingHandler(level = logging.CRITICAL)


def start(**kwargs) :
    from Moore.Configuration import Moore, MooreExpert
    from MooreOnlineConf.Configuration import MooreOnline
    import OnlineEnv

    if kwargs.get('Split','')  == 'Hlt1' :
       HltLevel = 'Hlt1'
    elif kwargs.get('Split','')  == 'Hlt2' :
       HltLevel = 'Hlt2'
    else :
       HltLevel = 'Hlt1Hlt2'

    moore = Moore()

    mooreOnline = MooreOnline()
    mooreOnline.HltLevel = HltLevel
    mooreOnline.RunOnline = True

    ### default database setup -- require an explit tag when running in the LHCb or FEST partitions...
    #not needed, moore.Simulation = False
    moore.DDDBtag    = 'unknown-please-specify-in-PVSS-RunInfo'
    moore.CondDBtag  = 'unknown-please-specify-in-PVSS-RunInfo'

    ### pick up requested DB tags
    import ConditionsMap
    def fwdOnlineEnv(attr, cfg, attr2 = None) :
        setattr(cfg, attr2 if attr2 else attr, getattr(ConditionsMap, attr))

    fwdOnlineEnv( 'CondDBTag', moore, 'CondDBtag' )
    fwdOnlineEnv( 'DDDBTag',   moore, 'DDDBtag' )
    fwdOnlineEnv( 'RunChangeHandlerConditions', mooreOnline )

    ## Enable RunChangeHandlerSvc
    mooreOnline.EnableRunChangeHandler = ( OnlineEnv.HLTType not in ['PA','PassThrough', 'Commissioning_OTCosmics'] )

    if OnlineEnv.PartitionName == 'TEST' : mooreOnline.CheckOdin = False
    if OnlineEnv.PartitionName == 'LHCb1' : mooreOnline.CheckOdin = False # TODO: push into Hack, but that doesn't seem to pick up its properties???

    #not needed any longer? MooreOnline already does this...
    moore.InitialTCK = OnlineEnv.InitialTCK
    mooreOnline.REQ1 = "EvType=1;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"
    from Configurables import MooreExpert
    for k,v in kwargs.iteritems() :
        #iterate through the available configurables to set required properties
        found=False
        for conf in [mooreOnline, moore, MooreExpert()]:
            if k in conf.__slots__ or hasattr(conf,k):
                setattr(conf,k,v)
                found=True
                #only set the top-level!
                break
        if not found:
            print "# WARNING: skipping setting '"+k+":"+v+"' because no configurable has that option"

    # Forward all attributes of 'OnlineEnv' python module to the job options service (using the OnlineEnv configurable)...
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    c = ConfigurableGeneric("OnlineEnv")
    #[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
    c.AcceptRate = OnlineEnv.AcceptRate

    if OnlineEnv.PartitionName in ('TEST', 'LHCb2') and HltLevel == 'Hlt2' :
        mooreOnline.REQ1 = "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"

    from Moore import Funcs
    comps = (".*Hlt2.*Fitter$", ".*Hlt2SelReportsWriter", ".*TrackEff.*[01]N3Body.*",
             ".*Hlt2.*TrackMasterExtrapolator", ".*DBAccessor", r".*\.PhotonMatch",
              r".*ToolSvc\.ParticleTransporter", ".*LoKiSvc.REPORT")
    outputTrans = {c : {"OutputLevel" : {"^.*$": "5"}} for c in comps}
    Funcs._mergeTransform(outputTrans)

    OnlineEnv.end_config(False)
