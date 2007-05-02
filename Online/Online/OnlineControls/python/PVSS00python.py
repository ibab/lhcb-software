import os,sys
setup = "/home/frankm/cmtuser/Gaudi_v19r2/Online/PVSSInterface/cmt/pkg_setup.sh
print 'executing initialization of:',setup
env=open(setup,'r').readlines()
for l in env:
     itm=l.split(';')
     if len(itm)>1:
          key,val=itm[0].replace('"','').split('=')
          os.environ[key]=val
os.execve(os.environ['PKG_EXE'],sys.argv,os.environ)
