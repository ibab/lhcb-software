#!/usr/bin/python
import os, sys

def _exec(cmd):
  print cmd
  return os.popen(cmd)

def _exec3(cmd):
  print cmd
  return os.popen3(cmd)

with_versions = None
for arg in sys.argv:
  if arg.upper()[:2]=='-V':
    with_versions = 1
  if arg.upper()[:2]=='-H':
    print 'usage: checkVersion [-version]'
    print '    -version    use version directories in checkout (optional).'
    sys.exit(0)


lines = os.popen('cmt show uses | grep -v "#" | grep "use "').readlines()
curr_dir = os.popen('pwd').readlines()[0] ###os.getcwd()

dir2 = curr_dir[0:curr_dir.rfind('/')]
if with_versions: dir2 = dir2[0:dir2.rfind('/')-1]
dir2 = dir2[0:dir2.rfind('/')+1]
if with_versions:
  print 'Execution mode: WITH version directories'
else:
  print 'Execution mode: WITHOUT version directories'

#print dir2
#print lines

for line in lines:
  items = line.split()[1:]
  package = items[0]
  version = items[1]
  head = ''
  if len(items) == 3:
    dir = items[2][1:-1]
    path = dir+package
    pkg = package
  else:
    head = items[2]
    dir = items[3][1:-1]
    path = dir+head+os.sep+package
    pkg = head+os.sep+package
  if with_versions: path = path+os.sep+version
  #print dir,pkg,path,dir==dir2,dir2
  if dir == dir2:
    last_tag = []
    lines = os.popen('cd '+path+'; cvs status -v cmt/requirements').readlines()
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
    pipe = os.popen3('cd '+path+'; cvs diff -r '+version)
    diffs = pipe[1].readlines()
    new = pipe[2].readlines()
    changes = 0
    prefix = '%-28s %-10s'%(pkg,'['+version+']',)
    if len(diffs) > 0:
      for d in diffs:
        if d[:2] == '? ': continue
        changes = 1
    if changes == 0:
      for n in new:
        if n.find('cvs diff: tag')==0:
	  idx = n.find('not in file')
	  print prefix,'New file:     ',pkg+n[idx+len('not in file '):-1]
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
      print prefix,'========> HEAD differs from version '+vsn+' of package:',pkg
      print prefix,'          Last tags:',last_tag
    else:
      print prefix,'UNCHANGED Version '+vsn+'    package:',pkg
      # print prefix,'          Last tags:',last_tag
  else:
    #print 'REJECT',package,version,dir,'Head:',head
    pass

