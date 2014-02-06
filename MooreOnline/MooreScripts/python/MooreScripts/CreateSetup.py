import os, os.path
import re
import sys
import string
from os import pathsep, listdir, makedirs
from os.path import exists, isdir, realpath, islink, dirname
from zipfile import is_zipfile
import fnmatch


def StripPath(path,predicate=None,exceptions=["MooreOnline_","Moore_","Online_"]):
    collected = []
    for p in path.split(pathsep):
        #print "checking", p
        rp = realpath(p)
        if len([True for m in exceptions if m in p]):
            #print p, "in exceptions"
            if p not in collected:
                collected.append(p)
        elif exists(rp) :
            #print p, "exists"
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
def ReFactorPath(path,order=["MooreOnline_","MOOREONLINE_","Moore_","MOORE_","Online_","ONLINE_"]):
    "re-order the path into the order specified, if more than one match to the ordering, the original order is preserverd for those matches"
    collected=[]
    #do all in the match statement
    for m in order:
        for p in path.split(pathsep):
            if m in p and p not in collected:
                collected.append(p)
    #move Gaudi after LHCb
    gaudi=[p for p in path.split(pathsep) if "GAUDI_" in p]
    lhcb=[p for p in path.split(pathsep) if "LHCB_" in p]
    for p in path.split(pathsep):
        if p in collected:
            continue
        if  p not in gaudi and p not in lhcb:
            collected.append(p)
        if p in lhcb:
            collected=collected+lhcb+gaudi
    return pathsep.join(collected)
#
def FixSetupMoore( output ):
    """
    Edit the output of SetupProject to fix certain rubbish
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
                        value=ReFactorPath(value)
                    if name == 'PYTHONPATH' :  # TODO: deal with python.zip files...
                        value = StripPath(value)
                        value=ReFactorPath(value)
                        #value = StripPath(value,lambda x: ContainsFNmatch(x,['*.py','*.pyc']))
                        
                    line = 'export %s="%s"'%(name,value)
                if input!=line : f.write('#ORIG: %s\n'%input)
                f.write(line+'\n')        
    else :
        print 'SetupProject did not generate %s... this is fine during a release build...' % output

#def CreateSetupMoore( output, version , setupProjectOps=[]) :
#
#    target_dir = dirname( output )
#    if not exists( target_dir ) : makedirs( target_dir )
#
#    from LbConfiguration.SetupProject import SetupProject
#
#    print 'generating %s assuming version %s'%(output,version)
#    SetupProject().main( [ '--dev-dir=/home/online/ONLINE','--shell=sh','--output='+output,'MooreOnline',version]+setupProjectOps )
#    StripSetupMoore(output)

