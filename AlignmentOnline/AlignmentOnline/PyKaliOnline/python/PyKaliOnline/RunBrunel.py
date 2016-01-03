import os, socket
import PyKaliOnline
#from test import fmDSTprod, 

import subprocess, re
import shlex

# Path to Brunel user area
# Run the "run" script to generate the environment
partition = os.environ.get("PARTITIONNAME", "LHCbA")
lines = []
if partition == "TEST":
    brunel_path = "/home/dsavrina/cmtuser/BrunelDev_v47r8"
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
else:
    brunel_path = "/group/online/dataflow/cmtuser/OnlineBrunel_%s" %'v5r23p1'#os.environ['ONLINE_VERSION']
    ## When running on the farm, use the pre-existing environment cache
    env_file = os.path.join(brunel_path, 'setup.%s.vars' % os.environ['CMTCONFIG'])
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
brunel_env = os.environ.copy()
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
   
homedir = os.path.dirname(PyKaliOnline.__file__)
#builtdir = os.path.dirname(PyKaliOnline.__file__)
#spname   = builtdir.split('build.x86_64-slc6-gcc48-')
#homedir  = spname[0]+'AlignmentOnline/PyKaliOnline'+spname[1][3:]

brunel_env['RUNINFO'] = "/group/online/dataflow/options/LHCbA/HLT"
brunel_env['PYTHONPATH'] = "/group/online/dataflow/options/LHCbA/HLT:" + brunel_env['PYTHONPATH']
brunel_env['PYTHONPATH'] += (':%s' % homedir.rsplit('/', 1)[0])

def RunBrunel(list_of_files, index, first_run, last_run):
    ## Use our parsed environment to run a Brunel command. Output goes to terminal.
    run_prefix = '%s_%s'%(first_run, last_run)
    command = 'python ' + os.path.join(homedir,'MultiBrunelStep.py --run-prefix %s --index %i --input-files'%(run_prefix, index))

    ## add the input files to the command
    ## one by one
    for fl in list_of_files:
        command = command+' %s'%fl

    ## run!
    p = subprocess.call(shlex.split(command), env = brunel_env)
