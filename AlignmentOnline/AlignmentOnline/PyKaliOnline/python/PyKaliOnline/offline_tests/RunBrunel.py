import os, socket
import PyKaliOnline

#from test import fmDSTprod, 

import subprocess, re
import shlex

# Path to Brunel user area
brunel_path = os.environ['User_release_area']+"/BrunelDev_v47r8"

# Run the "run" script to generate the environment

lines = []
if os.path.exists("env_cache_brunel"):
    with open("env_cache_brunel") as cache:
        for line in cache:
            lines.append(line.strip())
else:
    p = subprocess.Popen([os.path.join(brunel_path, 'run'), '--sh'],
                         stdout = subprocess.PIPE, stderr = subprocess.PIPE)
    o, e = p.communicate()
    with open("env_cache_brunel", 'w') as cache:
        cache.write(o)
        lines = [line.strip() for line in o.split('\n')]
                    
# Parse the output into a dictionary.
# Each contains one variable and looks like: "export FOO_bar='baz'"
exclude = set(['module'])
re_env = re.compile("^export ([\w_]+)='(.*)'$")
brunel_env = {}
for line in lines:
    if not line:
        continue
    result = re_env.match(line)
    if not result:
        print "couldn't match", line
    elif result.group(1) in exclude:
        continue
    else:
        brunel_env[result.group(1)] = result.group(2)
   
homedir = os.environ['PYKALIONLINEROOT']+'/python/PyKaliOnline/'

def RunBrunel(list_of_files, index):
    # def RunBrunel(input_file, index):
    # Use our parsed environment to run a DaVinci command. Output goes to terminal.
    #command = 'python ' + os.path.join(homedir,'BrunelStep.py --input-file=%s --index=%i'%(input_file, index))
    #p = subprocess.call(shlex.split(command), env = brunel_env)

    #directory     = '/localdisk/Alignment/BWDivision/'
    #list_of_files = [os.path.join(directory,f) for f in os.listdir(directory) if re.match(r'.*\.(mdf)$', f)]
    
    command = 'python ' + os.path.join(homedir,'BrunelStep.py --index %i --input-files'%(index))
    
    for fl in list_of_files:
        command = command+' %s'%fl

    p = subprocess.call(shlex.split(command), env = brunel_env)
