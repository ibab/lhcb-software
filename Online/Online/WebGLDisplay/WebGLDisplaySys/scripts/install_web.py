#!/usr/bin/python

import os
target = '/group/online/dataflow/cmtuser/EventDisplay'
home   = os.environ['WEBGLDISPLAYSYSROOT']

web    = home+os.path.sep+'web'
indent = 3

def indentation(level):
  s = ''
  if level>1:
    s = (indent*(level-1))*' '
  return s
  
def copyFile(level,src,tar):
  data = open(src,'r').read()
  open(tar,'w').write(data)
  print indentation(level)+'---> ',tar

def copyDir(level,src,tar):
  try:
    os.stat(tar)
  except Exception,X:
    os.mkdir(tar)

  print indentation(level)+'===> Directory:',tar
  entries=os.listdir(src)
  for e in entries:
    ent = src+os.path.sep+e
    if e != '.svn' and not os.path.isdir(ent):
        copyFile(level+1,ent,tar+os.path.sep+e)

  for e in entries:
    ent = src+os.path.sep+e
    if e != '.svn' and os.path.isdir(ent):
        copyDir(level+1,ent,tar+os.path.sep+e)


os.chdir(web)
copyDir(0,web,target)
