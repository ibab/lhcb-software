import os, os.path
import re
import sys
import string
from os import pathsep, listdir, makedirs
from os.path import exists, isdir, realpath, islink, dirname
from zipfile import is_zipfile
import fnmatch


def StripPath(path,predicate=None):
    collected = []
    for p in path.split(pathsep):
        rp = realpath(p)
        if exists(rp) :
            if ( isdir(rp) and len(listdir(rp))!=0 ) or is_zipfile(rp) :
                if p not in collected and ( not predicate or predicate(p) ): collected.append(p)
    return pathsep.join(collected)
#
def ContainsFNmatch(dir,matches) :
    for root,dirs,files in os.walk( dir ) :
        for m in matches :
            if fnmatch.filter(files,m)  : return True
        dirs.extend([ f for f in files if islink(f) and isdir(f)])
    return False
#

def StripSetupMoore( output ):
    """
    Edit the output of SetupProject to remove certain rubbish
    """
    if exists(output) :

        print 'removing use of StripPath.sh'
# remove call to StripPath.sh from generated SetupProject.sh
        with open(output, 'r') as f : txt = f.read()
        with open(output, 'w') as f :
            for input in txt.splitlines():
                line = input.replace('echo ','#echo ')
                line = re.sub('test -f [^ ]*StripPath.sh','false', line)
                m = re.match('export ([^=]+)="([^"]+)"',line)
                if m :
                    (name,value) = m.groups()
                    if name == 'LD_LIBRARY_PATH':
                        # value = StripPath(value,lambda x: not re.search('lcg/external/Grid',x))
                        value = StripPath(value,lambda x: ContainsFNmatch(x,['*.so']))
                    if name == 'PYTHONPATH' :  # TODO: deal with python.zip files...
                        value = StripPath(value)
                        #value = StripPath(value,lambda x: ContainsFNmatch(x,['*.py','*.pyc']))
                    line = 'export %s="%s"'%(name,value)
                if input!=line : f.write('#ORIG: %s\n'%input)
                f.write(line+'\n')        
    else :
        print 'SetupProject did not generate %s... this is fine during a release build...' % output

def CreateSetupMoore( output, version , setupProjectOps=[]) :

    target_dir = dirname( output )
    if not exists( target_dir ) : makedirs( target_dir )

    from LbConfiguration.SetupProject import SetupProject

    print 'generating %s assuming version %s'%(output,version)
    SetupProject().main( [ '--dev-dir=/home/online/ONLINE','--shell=sh','--output='+output,'MooreOnline',version]+setupProjectOps )
    StripSetupMoore(output)

