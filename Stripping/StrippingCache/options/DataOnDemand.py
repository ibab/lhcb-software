#!/usr/bin/env gaudirunb.oy

from Configurables import DataOnDemandSvc


dod               = DataOnDemandSvc()
dod.PreInitialize = True

from Configurables import ApplicationMgr 
app  = ApplicationMgr()
from Configurables import LHCb__ParticlePropertySvc as PPSvc 
from Configurables import LoKiSvc 

ApplicationMgr().ExtSvc += [ PPSvc() , LoKiSvc() ] 

