#test to check if I can make a TCK
#needs numEvents and targetTCK to be defined already, which is done in your python shell or a different file

creationString="created Aliases/TOPLEVEL/"
existsString="trying to read Aliases/TOPLEVEL/"

configsource="$HLTTCKROOT"
f=open("../options/Moore-LocalTCKData.py")
configdest=f.read().split("'")[1]
f.close()

commandoptions="'from Configurables import Moore; Moore().ForceSingleL0Configuration=False; Moore.generateConfig=True; Moore().EvtMax="+str(numEvents)+"; Moore().configLabel=\"Test TCK from nightly builds, auto-generated in nightlies based on some physics threshold setting.\"; '"

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
status,output = commands.getstatusoutput("gaudirun.py ../options/Moore-Testing-Default.py ../options/Moore-LocalTCKData.py --option="+commandoptions)

#
#print status

#print output for the nightlies to parse
#print output

if "FATAL" in output or (creationString not in output and existsString not in output):
    print output
    raise ValueError("Something went wrong in the gaudirun step")

searchstring=creationString
if creationString not in output and existsString in output:
    searchstring=existsString

loc=output.find(searchstring)+len(searchstring.replace("TOPLEVEL/",""))
eol=output.find('\n',loc)
configuration=output[loc:eol].strip()


print "===================================================="
print configuration
print "===================================================="

#
from TCKUtils.utils import *
from Configurables import ConfigTarFileAccessSvc as ctas
ctas(File=os.sep.join([configdest,"config.tar"]))
configs=getConfigurations()
if configuration not in configs.keys():
    raise KeyError("Configuration I just created has not been found... looking for"+configuration)

createTCKEntries({targetTCK:configuration.split('/')[-1]},ctas(Mode='ReadWrite',File=os.sep.join([configdest,"config.tar"])))
ctas(Mode='ReadOnly',File=os.sep.join([configdest,"config.tar"]))

configs=getConfigurations()
if configuration not in configs.keys():
    raise KeyError("Configuration I just created has not been found... looking for"+configuration)

if int(targetTCK,16) not in configs[configuration].info['TCK']:
    raise ValueError("TCK I asked to be created was not created for this threshold setting")

configs[configuration].printSimple()
print "===================================================="
print "PASS"
