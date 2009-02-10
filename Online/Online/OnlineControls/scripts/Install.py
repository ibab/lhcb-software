#!/usr/bin/python
import os, sys, time

def pvssDir():         return os.environ['PVSS_SYSTEM_ROOT']
def projectName():     return os.environ['PVSS_PROJECT_NAME']
def projectBaseDir():  return '/localdisk/pvss/'+projectName()
def componentsDir():   return os.environ['PVSS_COMPONENTS_DIR']
def sourceDir():       return os.environ['ONLINECONTROLSROOT']
def systemNumber():    return int(os.environ['PVSS_SYSTEM_NUMBER'])

def pvssCTRL():        return os.environ['PVSS_SYSTEM_ROOT']+'/bin/PVSS00ctrl -proj '+projectName()+' '
def pvssPMON():        return os.environ['PVSS_SYSTEM_ROOT']+'/bin/PVSS00pmon -proj '+projectName()+' '
#def pvssPMON():        return os.environ['PVSS_SYSTEM_ROOT']+'/bin/PVSS00pmon '
def pvssASCII():       return os.environ['PVSS_SYSTEM_ROOT']+'/bin/PVSS00ascii -proj '+projectName()+' '

def usage():
  print "usage: Install.py <action> -project <project-name> [-opt [-opt]]"
  print "  <action>:              Choose one of INSTALL INSTALLFILES COPYBACK"
  print "  -project       <name>: Choose one of: STORAGE MONITORING LBECS RECO RECOTEST"
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
      os.environ['PVSS_II'] = projectBaseDir()+'/config/config'
      i = i + 2
    elif a=='-I':
      os.environ['PVSS_INSTALL_NAME'] = args[i+1]
      i = i + 2
    elif a=='-C':
      os.environ['PVSS_COMPONENTS_DIR'] = args[i+1]
      i = i + 2
    elif a=='-N':
      os.environ['PVSS_SYSTEM_NUMBER'] = args[i+1]
      print 'PVSS System number: ',systemNumber()
      i = i + 2
    else:
      print 'Unknown option:'+args[i]
      usage()
  if not os.environ.has_key('PVSS_PROJECT_NAME'):
    print "No Project name given!"
    usage()
  print 'PVSS Project name: ',projectName()
  
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

  dir = (source_path+os.sep+sub_path).replace(os.sep+os.sep,os.sep)
  dirs = os.listdir(dir)
  os.system('mkdir -p '+target_path+os.sep+sub_path)
  os.chdir(target_path+os.sep+sub_path)
  #print '\n-----> %s'%os.getcwd()
  print '\n-----> %s'%dir
  for d in dirs:
    if not os.path.isdir(dir+os.sep+d):
      src = relative_path_up+rel_install_path+sub_path
      cmd = 'ln -fs '+src+os.sep+d+' '+d
      print '[Link] %-24s in %s'%(d,src)
      os.stat(src+os.sep+d)
      os.system(cmd)
      os.stat(d)
  for d in dirs:
    if os.path.isdir(dir+os.sep+d):
      if d != 'CVS':
        installDirectory(source_path,target_path,sub_path+os.sep+d,relative_path_up+'..'+os.sep,rel_install_path)

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
      pass
    except Exception,X:
      print X
    os.chdir(curr)
  print 'Now in:',os.getcwd()


def importDpList(fname):
  print 'Installing datapoint list...'
  dplist = sourceDir()+'/pvss/dplist/'+fname
  try:
    os.stat(dplist)
    execCmd(pvssASCII()+' -in '+dplist)
  except:
    print 'No datapoints to be imported ....:',dplist
  
def installDpList():
  if projectName()[:6]=='RECMON' or projectName()[:6]=='RECHLT':
    dplist = sourceDir()+'/pvss/dplist/ProcessorFarm.dpl'
  else:
    dplist = sourceDir()+'/pvss/dplist/'+projectName()+'.dpl'
  importDpList(dplist)
  
def importStreamDpLists():
  print '......... --> Importing datapoint lists....'
  importDpList('FSM_DimTask.dpl')
  importDpList('FSM_Tasks.dpl')
  importDpList('FSM_Slice.dpl')
  importDpList('FSM_Holder.dpl')
  importDpList('StreamControl.dpl')
  importDpList('StreamCluster.dpl')
  importDpList('StreamConfigurator.dpl')
  
def install():
  print 'Installing...'
  installFiles()
  #installDpList()
  pro = projectName()
  if pro=="MONITORING":
    print '......... --> Executing PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallMonitoring.cpp')
    importStreamDpLists()
    print '......... --> Executing python setup....'
    execCmd("""python -c "import Online.Streaming.MonitoringInstaller as IM; IM.install('Monitoring','"""+pro+"""')";""")
    print '......... --> Executing final PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallMonitoring2.cpp')
  elif pro=="MONITORING2":
    print '......... --> Executing PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallMonitoring.cpp')
    importStreamDpLists()
    print '......... --> Executing python setup....'
    execCmd("""python -c "import Online.Streaming.MonitoringInstaller as IM; IM.install('Monitoring2','"""+pro+"""')";""")
    print '......... --> Executing final PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallMonitoring2.cpp')
  elif pro=='RECONSTRUCTION':
    print '......... --> Executing PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallMonitoring.cpp')
    importStreamDpLists()
    print '......... --> Executing python setup....'
    execCmd("""python -c "import Online.Streaming.MonitoringInstaller as IM; IM.install('Reconstruction','"""+pro+"""')";""")
    print '......... --> Executing final PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallMonitoring2.cpp')
  elif pro=="STORAGE":
    print '......... --> Executing PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallStorage.cpp')
    importStreamDpLists()
    print '......... --> Executing python setup....'
    execCmd("""python -c "import Online.Streaming.StorageInstaller as IM; IM.install('Storage','"""+pro+"""')";""")
    print '......... --> Executing final PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallStorage2.cpp')
  elif pro=="RECSTORAGE":
    print '......... --> Executing PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallStorage.cpp')
    importStreamDpLists()
    print '......... --> Executing python setup....'
    execCmd("""python -c "import Online.Streaming.StorageInstaller as IM; IM.install('Storage','"""+pro+"""')";""")
    print '......... --> Executing final PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallRecStorage2.cpp')
  elif pro[:6]=='RECHLT':
    print '......... --> Importing datapoint lists....'
    importDpList('FSM_DimTask.dpl')
    importDpList('FSM_Tasks.dpl')
    importDpList('FSM_Slice.dpl')
    importDpList('StreamConfigurator.dpl')
    print '......... --> Executing PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallSubFarm.cpp')
    print '......... --> Executing python setup....'
    execCmd("""python -c "import Online.ProcessorFarm.FarmInstaller as IM; IM.installSubFarm('Farm','"""+pro+"""')";""")
    print '......... --> Executing final PVSS setup controller for project '+pro+'\n\n'
    execCmd(pvssCTRL()+'InstallSubFarm2.cpp')
  elif pro=='RECFARM':
    print '......... --> Executing PVSS setup controller for project '+pro
    importDpList('FSM_DimTask.dpl')
    importDpList('FSM_Farm.dpl')
    importDpList('FSM_Slice.dpl')
    #importDpList('FSM_Tasks.dpl')
    importDpList('StreamConfigurator.dpl')
    importDpList('StreamControl.dpl')
    importDpList('StreamCluster.dpl')
    execCmd(pvssCTRL()+'InstallFarm.cpp')
  elif pro=='RECCTRL':
    print '......... --> Executing PVSS setup controller for project '+pro
    importDpList('FSM_Farm.dpl')
    importDpList('FSM_Slice.dpl')
    importDpList('FSM_Alloc.dpl')
    importDpList('FSM_Holder.dpl')
    importDpList('FSM_FarmCtrl.dpl')
    execCmd(pvssCTRL()+'InstallRecCtrl.cpp')
  elif pro[:4]=='RECO':
    print '......... --> Executing PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallReco.cpp')
    print '......... --> Executing python setup....'
    execCmd("""python -c "import Online.Reco.FarmInstaller as IM; IM.install('Farm','"""+pro+"""')";""")
    print '......... --> Executing final PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallReco2.cpp')
  elif pro=="LBECS":
    print '......... --> Executing PVSS setup controller for project '+pro
    execCmd(pvssCTRL()+'InstallJobOptions.cpp')
    print '......... --> Executing python setup....'
    execCmd('python '+sourceDir()+'/python/InstallLBECS.py')
  elif os.environ.has_key('PVSS_INSTALL_NAME') and os.environ['PVSS_INSTALL_NAME']:
    execCmd(pvssASCII()+' -in '+sourceDir()+'/pvss/dplist/JobOptionsControl.dpl')
    print '......... --> Executing PVSS setup controller ...'
    execCmd(pvssCTRL()+'InstallJobOptionsControl.cpp')

def copyProject():
  start = time.time()
  src = sourceDir()+os.sep+'farmTemplate'
  nam = projectName()
  sysN = systemNumber()
  cfg = '/localdisk/pvss/'+nam+'/config/config'
  os.environ['PVSS_II']=cfg
  execCmd(pvssPMON()+' -config '+cfg+' -stopWait')
  execCmd('rm -rf /localdisk/pvss'+os.sep+nam)
  execCmd('cp -r '+src+' /localdisk/pvss'+os.sep+nam)
  print '......... --> Patching project configuration file'
  lines = open(cfg,'r').readlines()
  fout  = open(cfg,'w')
  for line in lines:
    content = line[:-1]
    if content.find('proj_path = "/localdisk/pvss/TEMPLATE"')==0:
      content = 'proj_path = "/localdisk/pvss/'+nam+'"'
    print >>fout,content
    if content=='distributed = 1':
      print >>fout, 'pmonPort = '+str(sysN+100)+'00'
      print >>fout, 'dataPort = '+str(sysN+100)+'01'
      print >>fout, 'eventPort = '+str(sysN+100)+'02'
      print >>fout, '[dist]'
      print >>fout, 'distPort = '+str(sysN+100)+'10'
      print >>fout, 'distPeer= "storectl01:49910" 399 # LBECS'
      print >>fout, 'distPeer= "mona07:40610"     306 # RECCTRL'
      print >>fout, 'distPeer= "mona07:40310"     303 # RECFARM'
    elif content.find('[dist]')==0:
      pass
    elif content.find('pmonPort = ')==0:
      pass
    elif content.find('dataPort = ')==0:
      pass
    elif content.find('eventPort = ')==0:
      pass
    elif content.find('distPort = ')==0:
      pass
  print '......... --> Patching project name and project number'
  execCmd('/opt/pvss/pvss2_v3.6/bin/PVSStoolSyncTypes -proj '+nam+' -config '+\
          cfg+' -report -autoreg -system '+str(sysN)+' '+nam)
  print '......... --> Project',nam,' System number',sysN,' is ready for farm installation'
  print '......... 1rst. Installation step finished. This took %d seconds.'%(time.time()-start,)

def copyProject2():
  start = time.time()
  src = sourceDir()+os.sep+'farmTemplate'
  nam = projectName()
  sysN = systemNumber()
  cfg = '/localdisk/pvss/'+nam+'/config/config'
  os.environ['PVSS_II']=cfg
  execCmd(pvssPMON()+' -config '+cfg+' -autoreg &')
  print '......... --> Project',nam,' System number',sysN,' started.'
  print '......... --> Project',nam,' Continue to install after the project is up....'
  time.sleep(5)
  execCmd('/usr/local/bin/startConsole')
  time.sleep(15)
  install()
  print '......... --> Project',nam,' All installation actions have finished. The subfarm should be operational.'
  print '......... 2nd. Installation step finished. This took %d seconds.'%(time.time()-start,)

def installSubFarm():
  nam = projectName()
  sysN = systemNumber()
  print 'python ../scripts/Install.py COPY -project '+nam+' -number '+str(sysN)
  print 'python ../scripts/Install.py COPY2 -project '+nam+' -number '+str(sysN)
  
def updateFarm():
  print 'Updateting...'
  if projectName()=='RECFARM':
    print '......... --> Executing PVSS setup controller for project '+projectName()
    execCmd(pvssCTRL()+'UpdateFarm.cpp')

def hostName():
  host = socket.gethostname()
  if host.find('.')>0:
    host = host[:host.find('.')]
  return host

def startFarm():
  print 'Starting...'
  h = hostname().upper()
  cfg = '/localdisk/pvss/REC'+h+'/config/config'
  os.environ['PVSS_II']=cfg
  execCmd(pvssPMON()+' -config '+cfg+' -autoreg&')

def run():
  os.chdir(sourceDir()+'/cmt')
  parseOpts()
  if sys.argv[1].upper()=='INSTALL':
    install()
  if sys.argv[1].upper()=='INSTALLFILES':
    installFiles()
  if sys.argv[1].upper()=='DPLIST':
    installDpList()
  if sys.argv[1].upper()=='COPY':
    copyProject()
  if sys.argv[1].upper()=='COPY2':
    copyProject2()
  if sys.argv[1].upper()=='SUBFARM':
    installSubFarm()
  if sys.argv[1].upper()=='UPDATEFARM':
    updateFarm()
  if sys.argv[1].upper()=='RECSTART':
    startFarm()
  if sys.argv[1].upper()=='COPYBACK':
    pass

if __name__ == "__main__":
  run()
