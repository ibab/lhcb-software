#test to check if I can make a TCK

commandoptions="'from Configurables import Moore; Moore().ForceSingleL0Configuration=False; Moore.generateConfig=True; Moore().EvtMax=1; Moore().configLabel=\"Test TCK from nightly builds, auto-generated in nightlies based on some physics threshold setting.\";'"

creationString="created Aliases/TOPLEVEL/"
existsString="trying to read Aliases/TOPLEVEL/"

targetTCK='0x00ee0046'

import os
if not os.path.exists("../../../../TCK/HltTCK"):
    raise IOError("No local TCK package, test cannot be run")

#run with one event
import commands
status,output = commands.getstatusoutput("source ../../../../TCK/HltTCK/cmt/setup.sh; gaudirun.py ../options/Moore-Testing-Default.py --option="+commandoptions)

#
#print status

#print output for the nightlies to parse
#print output

if "FATAL" in output or (creationString not in output and existsString not in output):
    raise ValueError("Something went wrong in the gaudirun step")
    print output

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
configs=getConfigurations()
if configuration not in configs.keys():
    raise KeyError("Configuration I just created has not been found... looking for"+configuration)

from Configurables import ConfigTarFileAccessSvc as ctas
createTCKEntries({targetTCK:configuration.split('/')[-1]},ctas(Mode='ReadWrite'))
ctas(Mode='ReadOnly')

configs=getConfigurations()
if configuration not in configs.keys():
    raise KeyError("Configuration I just created has not been found... looking for"+configuration)

if int(targetTCK,16) not in configs[configuration].info['TCK']:
    raise ValueError("TCK I asked to be created was not created for this threshold setting")

configs[configuration].printSimple()
print "===================================================="
