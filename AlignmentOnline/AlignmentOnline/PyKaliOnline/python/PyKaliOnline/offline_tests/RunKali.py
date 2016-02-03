import os
import PyKaliOnline
#from test import fmDSTprod, 

import subprocess, re, socket
import shlex

# Path to DaVinic user area
dv_path = os.environ['User_release_area']+"/DaVinciDev_v36r5"

# Run the "run" script to generate the environment

lines = []
if os.path.exists("env_cache"):
    with open("env_cache") as cache:
        for line in cache:
            lines.append(line.strip())
else:
    p = subprocess.Popen([os.path.join(dv_path, 'run'), '--sh'],
                         stdout = subprocess.PIPE, stderr = subprocess.PIPE)
    o, e = p.communicate()
    with open("env_cache", 'w') as cache:
        cache.write(o)
        lines = [line.strip() for line in o.split('\n')]
                    
# Parse the output into a dictionary.
# Each contains one variable and looks like: "export FOO_bar='baz'"
exclude = set(['module'])
re_env = re.compile("^export ([\w_]+)='(.*)'$")
dv_env = {}
for line in lines:
    if not line:
        continue
    result = re_env.match(line)
    if not result:
        print "couldn't match", line
    elif result.group(1) in exclude:
        continue
    else:
        dv_env[result.group(1)] = result.group(2)

homedir = os.environ['PYKALIONLINEROOT']+'/python/PyKaliOnline/'

def fmDSTprod(list_of_files, lambdas = None, index=0):
    # Use our parsed environment to run a DaVinci command. Output goes to terminal.
    command = 'python ' + os.path.join(homedir,'KaliPi0_producefmDST.py --index %i --lambdas="%s" --input-file'%(index,lambdas))

    for fl in list_of_files:
        command = command+' %s'%fl

    p = subprocess.call(shlex.split(command), env = dv_env)

def RunKali(input_file, process = 'Fill', lambdas = None, index = 0):
    if lambdas:
        print 'Using coefficients from', lambdas
        command = 'python ' + os.path.join(homedir,'Run.py --input-file=%s --process=%s --lambdas=%s --index=%i'%(input_file, process, lambdas, index))
    else:
        command = 'python ' + os.path.join(homedir,'Run.py --input-file=%s --process=%s --index=%i'%(input_file, process, index))
    p = subprocess.call(shlex.split(command), env = dv_env)

def SplitHistos(directory):
    ## Use our parsed environment to run a DaVinci command for
    ## preparing histograms for fitting
    command = 'python ' + os.path.join(homedir,'Run.py --input-file=%s --process="Split"'%directory)
    p = subprocess.call(shlex.split(command), env = dv_env)

def CollectLambdas(directory):
    ## Use our parsed environment to run a DaVinci command for
    ## collecting coefficients from different nodes/workers
    command = 'python ' + os.path.join(homedir,'Run.py --input-file=%s --process="Collect"'%directory)
    p = subprocess.call(shlex.split(command), env = dv_env)

def GetOutput(directory):
   ## Use our parsed environment to run a DaVinci command for
   ## creating the resulting histograms
   command = 'python ' + os.path.join(homedir,'Run.py --input-file=%s --process="Statistics"'%directory)
   p = subprocess.call(shlex.split(command), env = dv_env)

