import OnlineEnv
print dir(OnlineEnv)
OnlineEnv.MooreStartupMode = 1
OnlineEnv.AcceptRate=1
OnlineEnv.TAE=0
import os
#dummy task type
os.environ['TASK_TYPE']='GauchoJob'
OnlineEnv.GauchoJob_Input='Events'
OnlineEnv.GauchoJob_Output='Send'
OnlineEnv.OutputLevel=4

#harmless options file
os.environ['MBM_SETUP_OPTIONS']=os.environ['STDOPTS']+'/units.opts'
# Forward all attributes of 'OnlineEnv' to the job options service...

from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
c = ConfigurableGeneric("OnlineEnv")
#[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
c.AcceptRate = OnlineEnv.AcceptRate
c.Monitor = True
c.GauchoJob_Input=OnlineEnv.GauchoJob_Input
c.GauchoJob_Output=OnlineEnv.GauchoJob_Output
#OnlineEnv.end_config(False)
