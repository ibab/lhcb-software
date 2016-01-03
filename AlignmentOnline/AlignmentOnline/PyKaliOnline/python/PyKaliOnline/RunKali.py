import os
import PyKaliOnline
#from test import fmDSTprod, 

import subprocess, re, socket
import shlex

# Path to DaVinic user area

# Run the "run" script to generate the environment

partition = os.environ.get("PARTITIONNAME", "LHCbA")
lines = []
if partition == "TEST":
    dv_path = "/home/dsavrina/cmtuser/DaVinciDev_v36r5"
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
else:
    dv_path = "/group/calo/cmtuser/CaloDaVinciDev_v36r5"
    ## When running on the farm, use the pre-existing environment cache
    env_file = os.path.join(dv_path, 'setup.%s.vars' % os.environ['CMTCONFIG'])
    p = subprocess.Popen("source %s; printenv" % env_file, stdout = subprocess.PIPE,
                         stderr = subprocess.PIPE, shell = True)
    o, e = p.communicate()
    lines = []
    for line in o.split('\n'):
        s = line.strip().split('=', 1)
        if not s or len(s) != 2:
            continue
        lines.append('export {0}="{1}";'.format(*s))
    
# Parse the output into a dictionary.
# Each contains one variable and looks like: "export FOO_bar='baz'"
exclude = set(['module'])
re_env = re.compile("""^export ([\w_]+)=["'](.*)["'];?$""")
dv_env = os.environ.copy()
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
   
homedir = os.path.dirname(PyKaliOnline.__file__)

#builtdir = os.path.dirname(PyKaliOnline.__file__)
#spname   = builtdir.split('build.x86_64-slc6-gcc48-')
#homedir  = spname[0]+'AlignmentOnline/PyKaliOnline'+spname[1][3:]

dv_env['RUNINFO']    = "/group/online/dataflow/options/LHCbA/HLT"    
dv_env['PYTHONPATH'] = "/group/online/dataflow/options/LHCbA/HLT:" + dv_env['PYTHONPATH']
dv_env['PYTHONPATH'] += (':%s' % homedir.rsplit('/', 1)[0])

def fmDSTprod(input_file, lambdas = None, index=0):
    # Use our parsed environment to run a DaVinci command. Output goes to terminal.
    command = 'python ' + os.path.join(homedir,'KaliPi0_producefmDST.py --index=%i --lambdas="%s" --input-file'%(index,lambdas))

    for fl in input_file:
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

