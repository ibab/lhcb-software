import os
target = '/group/online/dataflow/cmtuser/EventDisplay'
home = os.environ['WEBGLDISPLAYSYSROOT']
web = home+os.path.sep+'web'

def copyFile(src,tar):
  data = open(src,'r').read()
  open(tar,'w').write(data)
  print '---> ',tar

def copyDir(src,tar):
  try:
    os.stat(tar)
  except Exception,X:
    os.mkdir(tar)

  entries=os.listdir(src)
  for e in entries:
    if e != '.svn':
      ent = src+os.path.sep+e
      to  = tar+os.path.sep+e
      if os.path.isdir(ent):
        copyDir(ent,to)
      else:
        copyFile(ent,to)


os.chdir(web)
copyDir(web,target)
