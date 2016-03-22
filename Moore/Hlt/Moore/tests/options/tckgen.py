import collections
import select
import fcntl
import os
import time
import sys
import shlex

def make_async(fd):
    # http://stackoverflow.com/a/7730201/190597
    '''add the O_NONBLOCK flag to a file descriptor'''
    fcntl.fcntl(
        fd, fcntl.F_SETFL, fcntl.fcntl(fd, fcntl.F_GETFL) | os.O_NONBLOCK)

def read_async(fd):
    # http://stackoverflow.com/a/7730201/190597
    '''read some data from a file descriptor, ignoring EAGAIN errors'''
    # time.sleep(1)
    try:
        return fd.read()
    except IOError, e:
        if e.errno != errno.EAGAIN:
            raise e
        else:
            return ''

def write_output(fds, outmap):
    for fd in fds:
        line = read_async(fd)
        if line.strip():
            for l in line.strip().split('\n'):
                sys.stdout.write(l + '\n')
                outmap[fd.fileno()].append(l)

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

commandoptions  = "from Configurables import Moore; Moore().OutputLevel = 4; Moore().generateConfig=True; Moore().ForceSingleL0Configuration = True; Moore().EvtMax="+str(numEvents)+"; Moore().configLabel=\""+label+"\"; print \""+releaseString+"\", Moore().getRelease(); print \""+thresholdString+"\", Moore().ThresholdSettings; Moore().EnableTimer=False;"
commandoptions += "from Configurables import HltConf; HltConf().L0TCK = \"0x%s\"; print Moore();" % targetTCK[-4:]
commandoptions = "'%s'"%commandoptions

if mysplit=="Hlt1":
    commandoptions = commandoptions.replace("import Moore;",'import Moore; Moore().Split=\"Hlt1";')

if mysplit=="Hlt2":
    commandoptions = commandoptions.replace("import Moore;",'import Moore; Moore().Split="Hlt2";')


def expp(apath):
    return os.path.abspath(os.path.expandvars(os.path.expanduser(apath)))

import os
if not os.path.exists(expp(configsource)) or not os.path.exists(expp(configdest)):
    print configsource, configdest, expp(os.getcwd())
    raise IOError("Local filesystem not like install area, aborting")

if not os.path.exists(os.path.join(expp(configdest),"config.cdb")):
    print "copying config file"
    import shutil
    shutil.copy2(os.path.join(expp(configsource),"config.cdb"), os.path.join(expp(configdest),"config.cdb"))

print "modified config in: "+ os.path.join(expp(configdest),"config.cdb")

#run events
print "running",numEvents
import subprocess
cmd = shlex.split("gaudirun.py ../options/Moore_Hlt1And2.py ../options/default-threshold.py ../options/Moore-LocalTCKData.py ../options/Input_2015_0x0050.py --option=" + commandoptions)
print ' '.join(cmd)
process = subprocess.Popen(cmd, stdout = subprocess.PIPE, stderr = subprocess.PIPE, close_fds = True)

make_async(process.stdout)
make_async(process.stderr)

outmap = collections.defaultdict(list)
while True:
    rlist, wlist, xlist = select.select([process.stdout, process.stderr], [], [])
    write_output(rlist, outmap)
    if process.poll() is not None:
        write_output([process.stdout, process.stderr], outmap)
        break

fileno = {'stdout': process.stdout.fileno(),
          'stderr': process.stderr.fileno()}

output = outmap[fileno['stdout']]

#
#print status

#print output for the nightlies to parse
#print output

found = False
for line in output:
    if "FATAL" in line:
        print line
        raise ValueError("Something went wrong in the gaudirun step")
    if existsString in line:
        found = True

if not found:
        raise ValueError("Something went wrong in the gaudirun step")

def findme(searchstring,output):
    for line in output:
        if searchstring in line:
            start = line.find(searchstring) + len(searchstring)
            return line[start:].strip()

configuration=findme(existsString,output)
threshold=findme(thresholdString,output)
release=findme(releaseString,output)


print "===================================================="
print release, threshold, configuration
print "===================================================="

#
from TCKUtils.utils import *
from Configurables import ConfigCDBAccessSvc as ctas
ctas(File=os.sep.join([configdest,"config.cdb"]))
configs=getConfigurations()
search="/".join(["TOPLEVEL",release,threshold,configuration])

if search not in configs.keys():
    raise KeyError("Configuration I just created has not been found... looking for " + search)

createTCKEntries({targetTCK:configuration},ctas(Mode='ReadWrite',File=os.sep.join([configdest,"config.cdb"])))
ctas(Mode='ReadOnly',File=os.sep.join([configdest,"config.cdb"]))

configs=getConfigurations()
if search not in configs.keys():
    raise KeyError("Configuration I just created has not been found... looking for"+search)

if int(targetTCK,16) not in configs[search].info['TCK']:
    raise ValueError("TCK I asked to be created was not created for this threshold setting")

configs[search].printSimple()
print "===================================================="
print "PASS"
