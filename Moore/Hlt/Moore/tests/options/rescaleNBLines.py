#When running from the real data for my tests, I need to scale down the prescale lines usign a transform, otherwise I simple select everything again.
#I need to do this when running, but not when generating a TCK!

#accessing this property is done via TCKSh:
# python
# from TCKUtils.utils import *
# ctas=ctas(File='../../../../InstallArea/x86_64-slc6-gcc48-opt/config.tar')
# getProperty(0x00ee0046,"Hlt1MBNoBiasPostScaler","AcceptFraction",ctas)
# 
from Gaudi.Configuration import *
def resetAction(mult=0.1):
    from Configurables import DeterministicPrescaler as dp
    dp("Hlt1MBNoBiasPreScaler").AcceptFraction=mult*mult
    dp("Hlt1CharmCalibrationNoBiasPreScaler").AcceptFraction=mult
    dp("Hlt2TransparentPreScaler").AcceptFraction=0.0
    

def resetTransform(mult=0.1):
    """
    Gah, can't guess what the original number is...
    """
    from Moore import Funcs
    trans={}
    trans[".*Hlt1CharmCalibrationNoBiasPreScaler"]={"AcceptFraction": {"^.*$":str(mult)}}
    trans[".*Hlt1MBNoBiasPreScaler"]={"AcceptFraction": {"^.*$":str(mult*mult)}}
    trans[".*Hlt2TransparentPreScaler"]={"AcceptFraction": {"^.*$":str(0.0)}}
    Funcs._mergeTransform(trans)
    
from Configurables import Moore
if Moore().getProp("generateConfig"):
    pass
else:
    if Moore().getProp("UseTCK"):
        resetTransform()
    else:
        appendPostConfigAction(resetAction)
