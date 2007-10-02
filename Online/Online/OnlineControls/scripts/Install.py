import os, sys

def pvssDir():         return os.environ['PVSS_SYSTEM_ROOT']
def projectName():     return os.environ['PVSS_PROJECT_NAME']
def projectBaseDir():  return '/localdisk/pvss/'+projectName()
def componentsDir():   return os.environ['PVSS_COMPONENTS_DIR']
def sourceDir():       return os.environ['ONLINECONTROLSROOT']

def pvssCTRL():        return os.environ['PVSS_SYSTEM_ROOT']+'/bin/PVSS00ctrl -proj '+projectName()+' '
def pvssASCII():       return os.environ['PVSS_SYSTEM_ROOT']+'/bin/PVSS00ascii -proj '+projectName()+' '

def usage():
  print "usage: Install.py -project <project-name> [-opt [-opt]]"
  print "  -project <name>: Choose one of: STORAGE MONITORING LBECS"
  print "  -install <name>: Choose one of: JOBOPTIONS"
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
  
def installFiles():
  print 'Install files to directory: '+componentsDir()
  print 'Copy files from '+sourceDir()+'/pvss to '+componentsDir()+' ...'
  execCmd('cp --recursive --force --symbolic-link '+sourceDir()+'/pvss/* '+componentsDir())
  if projectBaseDir() != componentsDir():
    print 'Copy files from '+sourceDir()+'/pvss to '+projectBaseDir()+' ...'
    execCmd('cp --recursive --force --symbolic-link '+sourceDir()+'/pvss/* '+projectBaseDir())

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
  parseOpts()
  if sys.argv[1].upper()=='INSTALL':
    install()
  if sys.argv[1].upper()=='INSTALLFILES':
    installFiles()
  if sys.argv[1].upper()=='COPYBACK':
    pass
