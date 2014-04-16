#test to check if I can make a TCK
#needs numEvents and targetTCK to be defined already, which is done in your python shell or a different file

#creationString="created Aliases/TOPLEVEL/"
#existsString="trying to read Aliases/TOPLEVEL/"
existsString="top id for this config:"

mysplit=''
try:
    mysplit=split
except:
    pass

label="Test TCK from nightly builds, auto-generated in nightlies based on some physics threshold setting."
releaseString="----MOORERELEASE----:"
thresholdString="----MOORETHRESHOLD----:"

configsource="$HLTTCKROOT"
f=open("../options/Moore-LocalTCKData.py")
configdest=f.read().split("'")[1]
f.close()

commandoptions="'from Configurables import Moore; Moore().generateConfig=True; Moore().EvtMax="+str(numEvents)+"; Moore().configLabel=\""+label+"\"; print \""+releaseString+"\", Moore().getRelease(); print \""+thresholdString+"\", Moore().ThresholdSettings; Moore().EnableTimer=False;'"

if mysplit=="Hlt1":
    commandoptions.replace("import Moore;","import Moore; Moore().ThresholdSettings=\"Hlt1_\"+Moore().ThresholdSettings;")

if mysplit=="Hlt2":
    commandoptions.replace("import Moore;","import Moore; Moore().ThresholdSettings=\"Hlt2_\"+Moore().ThresholdSettings;")


def expp(apath):
    return os.path.abspath(os.path.expandvars(os.path.expanduser(apath)))

import os
if not os.path.exists(expp(configsource)) or not os.path.exists(expp(configdest)):
    print configsource, configdest, expp(os.getcwd())
    raise IOError("Local filesystem not like install area, aborting")

if not os.path.exists(os.path.join(expp(configdest),"config.tar")):
    print "copying config file"
    import shutil
    shutil.copy2(os.path.join(expp(configsource),"config.tar"), os.path.join(expp(configdest),"config.tar"))

print "modified config in: "+ os.path.join(expp(configdest),"config.tar")

#run events
print "running",numEvents
import commands
status,output = commands.getstatusoutput("gaudirun.py ../options/Moore_Hlt1And2.py ../options/default-threshold.py ../options/Moore-LocalTCKData.py --option="+commandoptions)

#
#print status

#print output for the nightlies to parse
#print output

if "FATAL" in output or existsString not in output:
    print output
    raise ValueError("Something went wrong in the gaudirun step")

def findme(searchstring,output):
    loc=output.find(searchstring)+len(searchstring)
    eol=output.find('\n',loc)
    return output[loc:eol].strip()

configuration=findme(existsString,output)
threshold=findme(thresholdString,output)
release=findme(releaseString,output)


print "===================================================="
print release, threshold, configuration
print "===================================================="

#
from TCKUtils.utils import *
from Configurables import ConfigTarFileAccessSvc as ctas
ctas(File=os.sep.join([configdest,"config.tar"]))
configs=getConfigurations()
search="/".join(["TOPLEVEL",release,threshold,configuration])
if search not in configs.keys():
    raise KeyError("Configuration I just created has not been found... looking for"+search)

createTCKEntries({targetTCK:configuration},ctas(Mode='ReadWrite',File=os.sep.join([configdest,"config.tar"])))
ctas(Mode='ReadOnly',File=os.sep.join([configdest,"config.tar"]))

configs=getConfigurations()
if search not in configs.keys():
    raise KeyError("Configuration I just created has not been found... looking for"+search)

if int(targetTCK,16) not in configs[search].info['TCK']:
    raise ValueError("TCK I asked to be created was not created for this threshold setting")

configs[search].printSimple()
print "===================================================="
print "PASS"
