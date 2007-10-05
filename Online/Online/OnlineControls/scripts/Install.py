#!/usr/bin/python
import os, sys

def pvssDir():         return os.environ['PVSS_SYSTEM_ROOT']
def projectName():     return os.environ['PVSS_PROJECT_NAME']
def projectBaseDir():  return '/localdisk/pvss/'+projectName()
def componentsDir():   return os.environ['PVSS_COMPONENTS_DIR']
def sourceDir():       return os.environ['ONLINECONTROLSROOT']

def pvssCTRL():        return os.environ['PVSS_SYSTEM_ROOT']+'/bin/PVSS00ctrl -proj '+projectName()+' '
def pvssASCII():       return os.environ['PVSS_SYSTEM_ROOT']+'/bin/PVSS00ascii -proj '+projectName()+' '

def usage():
  print "usage: Install.py <action> -project <project-name> [-opt [-opt]]"
  print "  <action>:              Choose one of INSTALL INSTALLFILES COPYBACK"
  print "  -project       <name>: Choose one of: STORAGE MONITORING LBECS"
  print "  -install       <name>: Choose one of: JOBOPTIONS"
  print "  -componentsdir <name>: Installation directory of components"
  sys.exit(1)
  
def parseOpts():
  print sys.argv
  args = sys.argv[2:]
  i=0
  while(i<len(args)):
    a = str(args[i]).upper()[:2]
    if a=='-P':
      os.environ['PVSS_PROJECT_NAME'] = args[i+1]
      i = i + 2
    elif a=='-I':
      os.environ['PVSS_INSTALL_NAME'] = args[i+1]
      i = i + 2
    elif a=='-C':
      os.environ['PVSS_COMPONENTS_DIR'] = args[i+1]
      i = i + 2
    else:
      print 'Unknown option:'+args[i]
      usage()
  if not os.environ.has_key('PVSS_PROJECT_NAME'):
    print "No Project name given!"
    usage()
  print 'PVSS Project name: '+projectName()
  
  if os.environ.has_key('PVSS_INSTALL_NAME'):
    print 'PVSS Install name:'+os.environ['PVSS_INSTALL_NAME']

  if not os.environ.has_key('PVSS_COMPONENTS_DIR'):
    os.environ['PVSS_COMPONENTS_DIR'] = projectBaseDir()

def execCmd(cmd):
  print '......... --> '+cmd
  os.system(cmd)
  
def installDirectory(source_path,target_path,sub_path='',relative_path_up=None,rel_install_path=None):
  if rel_install_path is None:
    target_path = os.path.realpath(target_path)
    source_path = os.path.realpath(source_path)
    os.stat(target_path)  # Check if pathes exist
    os.stat(source_path)
    rel_install_path=''
    other = ''
    for i in xrange(len(source_path)):
      if source_path[i] != target_path[i]:
        rel_install_path = source_path[i:]
        break
      other = other + target_path[i]

    other = target_path[len(other):].split('/')
    relative_path_up = ''
    for i in other: relative_path_up = relative_path_up+'..'+os.sep
    print 'Target path:',target_path
    print 'Source path:',source_path
    print 'Relative difference:',relative_path_up

  dir = source_path+os.sep+sub_path
  dirs = os.listdir(dir)
  os.system('mkdir -p '+target_path+os.sep+sub_path)
  os.chdir(target_path+os.sep+sub_path)
  print '\n-----> %s'%os.getcwd()
  for d in dirs:
    if os.path.isdir(dir+os.sep+d):
      if d != 'CVS':
        installDirectory(source_path,target_path,sub_path+os.sep+d,relative_path_up+'..'+os.sep,rel_install_path)
    else:
      src = relative_path_up+rel_install_path+sub_path
      cmd = 'ln -fs '+src+os.sep+d+' '+d
      print '[Link] %-24s in %s'%(d,src)
      os.stat(src+os.sep+d)
      os.system(cmd)
      os.stat(d)

def installFiles():
  print 'Install files to directory: '+componentsDir()
  print 'Copy files from '+sourceDir()+os.sep+'pvss to '+projectBaseDir()+' ...'
  curr = os.getcwd()
  try:
    installDirectory(sourceDir()+os.sep+'pvss',projectBaseDir())
  except Exception,X:
    print X
  os.chdir(curr)
  #execCmd('cp --recursive --force --symbolic-link '+sourceDir()+'/pvss/* '+projectBaseDir())
  if projectBaseDir() != componentsDir():
    print 'Copy files from '+sourceDir()+'/pvss to '+componentsDir()+' ...'
    try:
      installDirectory(sourceDir()+os.sep+'pvss',componentsDir())
      #execCmd('cp --recursive --remove-destination '+sourceDir()+'/pvss/* '+componentsDir())
    except Exception,X:
      print X
    os.chdir(curr)
  print 'Now in:',os.getcwd()


def install():
  print 'Installing ....'
  installFiles()
  try:
    dplist = sourceDir()+'/pvss/dplist/'+projectName()+'.dpl'
    os.stat(dplist)
    execCmd(pvssASCII()+' -in '+dplist)
  except:
    print 'No datapoints to be imported ....'
  
  if projectName()=="MONITORING":
    print 'Executing PVSS setup controller for project '+projectName()
    execCmd(pvssCTRL()+'InstallMonitoring.cpp')
    print 'Executing python setup....'
    execCmd("""python -c "import Online.Streaming.MonitoringInstaller as IM; IM.install('Monitoring')";""")
    print 'Executing final PVSS setup controller for project '+projectName()
    execCmd(pvssCTRL()+'InstallMonitoring2.cpp')
  elif projectName()=="STORAGE":
    print 'Executing PVSS setup controller for project '+projectName()
    execCmd(pvssCTRL()+'InstallStorage.cpp')
    print 'Executing python setup....'
    execCmd("""python -c "import Online.Streaming.StorageInstaller as IM; IM.install('Storage')";""")
    print 'Executing final PVSS setup controller for project '+projectName()
    execCmd(pvssCTRL()+'InstallStorage2.cpp')
  elif projectName()=="LBECS":
    print 'Executing PVSS setup controller for project '+projectName()
    execCmd(pvssCTRL()+'InstallJobOptions.cpp')
    print 'Executing python setup....'
    execCmd('python '+sourceDir()+'/python/InstallLBECS.py')
  elif os.environ.has_key('PVSS_INSTALL_NAME') and os.environ['PVSS_INSTALL_NAME']:
    execCmd(pvssASCII()+' -in '+sourceDir()+'/pvss/dplist/JobOptionsControl.dpl')
    print 'Executing PVSS setup controller ...'
    execCmd(pvssCTRL()+'InstallJobOptionsControl.cpp')

if __name__ == "__main__":
  os.chdir(sourceDir()+'/cmt')
  parseOpts()
  if sys.argv[1].upper()=='INSTALL':
    install()
  if sys.argv[1].upper()=='INSTALLFILES':
    installFiles()
  if sys.argv[1].upper()=='COPYBACK':
    pass
