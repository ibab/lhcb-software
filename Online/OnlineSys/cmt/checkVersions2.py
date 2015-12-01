#!/usr/bin/python
import os, sys, subprocess

def printCmd(cmd):
  #print cmd
  pass

def _exec(cmd):
  printCmd(cmd)
  p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
  return p.stdout
  ##return os.popen(cmd)

def _execErr(cmd):
  printCmd(cmd)
  p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
  return p.stderr
  ##return os.popen(cmd)

def _exec3(cmd):
  printCmd(cmd)
  p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,close_fds=True)
  return (None,p.stdout,p.stderr)
  ##return os.popen3(cmd)

with_versions = None
for arg in sys.argv:
  if arg.upper()[:2]=='-V':
    with_versions = 1
  if arg.upper()[:2]=='-H':
    print 'usage: checkVersion [-version]'
    print '    -version    use version directories in checkout (optional).'
    sys.exit(0)


#lines = _exec('cmt show uses | grep -v "#" | grep "use "').readlines()
lines = _execErr('cmt show uses | grep "CMT---> Warning: package "').readlines()
curr_dir = _exec('pwd').readlines()[0][:-1] ###os.getcwd()

dir2 = curr_dir[0:curr_dir.rfind('/')]
if with_versions: dir2 = dir2[0:dir2.rfind('/')-1]
dir2 = dir2[0:dir2.rfind('/')+1]
if with_versions:
  print 'Execution mode: WITH version directories'
else:
  print 'Execution mode: WITHOUT version directories'

#print dir2
#print lines
dir2 = dir2

for line in lines:
  #print line
  items = line.split()[3:6]
  ##print items
  package = items[0]
  version = items[1]
  head = ''
  if len(items) == 3:
    dir = os.path.realpath(curr_dir + '/../../Online/')
    path = dir+os.sep+package
    dir = os.path.realpath(curr_dir + '/../../')+os.sep
    pkg = package
    pkg_print = package
  else:
    head = items[2]
    dir = items[3][1:-1]
    path = dir+head+os.sep+package
    pkg = head+os.sep+package
    pkg_print = head+' '+os.sep+' '+package
  if with_versions: path = path+os.sep+version
  ##print dir,pkg,path,dir==dir2,dir2
  tags = ''
  trunk = ''
  if dir == dir2:
    last_tag = []
    os.chdir(curr_dir)
    os.chdir(path)
    proto = 'cvs'
    try:
      os.stat('.svn')
      proto = 'svn'
      l=open('.svn/entries').readlines()
      vsn = l[4][:-1]
      if vsn.rfind(os.sep+'v')>0:
        vsn = l[4][:l[4].rfind(os.sep)]
      tags  = vsn.replace('/trunk/','/tags/')
      trunk = vsn.replace('/tags/','/trunk/')
      #trunk = trunk[:trunk.rfind('/')]
      #print vsn, tags, trunk
      lines = _exec('svn list '+tags).readlines()
      l = len(lines)-1
      if l==1: last_tag=[lines[l][:-1]]
      if l==2: last_tag=[lines[l][:-1],lines[l-1][:-1]]
      if l==3: last_tag=[lines[l][:-1],lines[l-1][:-1],lines[l-2][:-1]]
      if l==4: last_tag=[lines[l][:-1],lines[l-1][:-1],lines[l-3][:-1],lines[l-4][:-1]]
      if l>4:  last_tag=[lines[l][:-1],lines[l-1][:-1],lines[l-3][:-1],lines[l-4][:-1],lines[l-5][:-1]]
    except OSError, X:
      lines = _exec('cvs status -v cmt/requirements').readlines()
      p = 0
      for line in lines:
        if p == 1:
          tag = line.replace(' ','').replace('(',' (').replace('\t','').replace('revision:','').split('(')
          if len(tag):
            last_tag.append(tag[0])
            if len(last_tag)>4: 
              last_tag = str(last_tag).replace('[','').replace(']','').replace("'",'')
              break
        if line.find('Existing Tags:')>0:
          p = 1
    if proto == 'svn':
      ##print 'svn diff '+trunk+' '+tags+'/'+version
      pipe = _exec3('svn diff '+trunk+' '+tags+'/'+version)
    else:
      pipe = _exec3('cvs diff -r HEAD -r '+version)
    diffs = pipe[1].readlines()
    new = pipe[2].readlines()
    changes = 0
    prefix = '%s %-28s %-10s'%(proto,pkg_print,'['+version+']',)
    if len(diffs) > 0:
      for d in diffs:
        if d[:2] == '? ': continue
        changes = 1
    if changes == 0:
      for n in new:
        if n.find('cvs diff: tag')==0:
          idx = n.find('not in file')
          print prefix,'New file:     ',pkg+os.sep+n[idx+len('not in file '):-1]
          changes = 1
        elif n.find('no longer exists') > 0:
          print prefix,'File removed: ',pkg+n.split(' ')[2]
          changes = 1
        elif n.find('cvs diff: Diffing') == 0:
          pass
        else:
          print 'Nothing abnormal:',n[:-1]
    vsn = "%-8s"%version
    if changes:
      tag = version.replace('v','').replace('r',' ').replace('p',' ').split(' ')
      print prefix,'==> HEAD differs from version '+vsn+' of package:',pkg
      print prefix,'==> See '+tags
      print prefix,'==> Last tags:',last_tag
    else:
      print prefix,'UNCHANGED Version '+vsn+'    package:',pkg
      # print prefix,'          Last tags:',last_tag
  else:
    #print 'REJECT',package,version,dir,'Head:',head
    pass

