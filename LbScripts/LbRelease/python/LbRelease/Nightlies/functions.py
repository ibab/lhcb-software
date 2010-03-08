import os, sys, shutil, re, time, pickle, string
from stat import S_IWRITE, ST_MODE, S_IMODE, S_IRWXU
from pprint import pformat
from LbUtils.Lock import Lock
from threading import Thread
from setup import *

sys.path = [os.path.abspath(os.environ['LCG_NIGHTLIES_SCRIPTDIR'])] + sys.path
import configuration

def timeout(func, args=(), kwargs={}, timeout_duration=1, default=None):
    """ http://code.activestate.com/recipes/473878/ """
    import threading
    class InterruptableThread(threading.Thread):
        def __init__(self):
            threading.Thread.__init__(self)
            self.result = None
        def run(self):
            try:
                self.result = func(*args, **kwargs)
            except:
                self.result = default
    it = InterruptableThread()
    it.start()
    it.join(timeout_duration)
    if it.isAlive():
        return default
    else:
        return it.result

def _fixPerms(path):
    """ change file/dir permissions to be able to remove them """
    try:
        mode = S_IMODE(os.stat(path)[ST_MODE])
        if (mode & S_IRWXU) != S_IRWXU:
            os.chmod(path, mode | S_IRWXU)
    except OSError:
        pass

def dirSize(dirPath): # in MB
    if not os.path.exists(dirPath):
        return None
    folder_size = 0.
    for (path, dirs, files) in os.walk(dirPath):
        for file in files:
            filename = os.path.join(path, file)
            folder_size += os.path.getsize(filename)
    return (folder_size/(1024*1024.0))

def rmtree(path, ignore_errors=False, onerror=None):
    """ change permissions of all files and directories and call shutil.rmtree """
    _fixPerms(path)
    for root, dirs, files in os.walk(path):
        for x in dirs+files:
            _fixPerms(os.path.join(root, x))
    shutil.rmtree(path, ignore_errors, onerror)

def copyLocal(sourceDir, targetDir):
    """ tar-pipe copying with locking from <sourceDir> to <targetDir>. Target directory is created if necessary.

        Locking is implemented on the level of the most deep sub-directory. Empty directories are not copied.
        Works only on Linux, both <sourceDir> and <targetDir> must be accessible locally.
    """
    if not os.path.exists(targetDir): os.makedirs(targetDir)
    os.chdir(targetDir) # .lock_* files are stored in CWD and they must by on shared file system
    sourceDir = os.path.abspath(sourceDir)
    targetDir = os.path.abspath(targetDir)
    dirList = [x for x in os.walk(sourceDir, topdown=False)]
    for root, dires, files in dirList:
        if files:
            path = (root+os.sep).replace(sourceDir, '')
            files = [x for x in files if x[-4:] != '.pyc' and x[-4:] != '.pyo']
            if len(' '.join(files)) > 30720: # to be on the safe side: 30K limit (Win: 32K, Red Hat 128K)
                maxFilesPerCopyCmd = 200 # files
                listOfLists = [files[i:i+maxFilesPerCopyCmd] for i in range(0, len(files), maxFilesPerCopyCmd)]
            else:
                listOfLists = [files[:]]
            for gFiles in listOfLists:
                files = gFiles[:]
                filesString = ' '.join(files)
                cwd = os.getcwd()
                commandLinux = 'mkdir -p '+ targetDir + path +' && cd '+ sourceDir + path +' && tar cf - '+filesString+' | (cd '+targetDir+path+'. ; tar xf -) && cd -'
                commandWindows = '(if not exist '+targetDir + path+' mkdir '+ targetDir + path +')'+' '.join([' & ( if not exist '+targetDir+path+x+' copy '+sourceDir + path +x+' '+targetDir+path+' > NUL )' for x in files]) + ' & (cd '+cwd+')'
                if systemType == 'windows':
                    Lock(commandWindows,str(os.getpid()), commandLinux, splitCommand='&')
                else:
                    Lock(commandLinux,str(os.getpid()), commandLinux)

def copySsh(sourceHost, sourceDir, targetDir):
    """ tar-pipe copying by SSH with locking from <sourceHost>:<sourceDir> to <targetDir>. Target directory is created if necessary. Procedure must be used on the target host. <sourceHost> must be accessible by SSH without password.

        Locking is implemented on the level of the most deep sub-directory. Empty directories are not copied.
        Works only on Linux. <targetDir> must be accessible locally, <sourceDir> must be accessible on <sourceHost>.
    """
    if not os.path.exists(targetDir): os.makedirs(targetDir)
    os.chdir(targetDir) # .lock_* files are stored in CWD and they must by on shared file system
    sourceDir = os.path.abspath(sourceDir)
    targetDir = os.path.abspath(targetDir)
    res = os.popen('ssh ' + sourceHost + ' "python -c \'import os; sourceDir=\\\"' + sourceDir + '\\\" ; print [x for x in os.walk(sourceDir, topdown=False)]; \'"').readlines()[-1].replace('\n','')
    exec('dirList = ' + res + '')
    for root, dires, files in dirList:
        if files:
            path = (root+os.sep).replace(sourceDir, '')
            files = [x for x in files if x[-4:] != '.pyc' and x[-4:] != '.pyo']
            filesString = ' '.join(files)
            Lock('mkdir -p '+ targetDir + path +' && ssh ' + sourceHost + ' "cd '+ sourceDir + path +' && tar cf - '+filesString+'" | (cd '+targetDir+path+'. ; tar xf -) && cd -',str(os.getpid()),path)

def cleanPycPyoFiles(slotObject):
    print "############################"
    print "# REMOVED .pyc .pyo FILES: #"
    print "############################"
    if slotObject != None:
        for root, dirs, files in os.walk(slotObject.releaseDir()):
            for name in files:
                if name[-4:] == '.pyc' or name[-4:] == '.pyo':
                    print os.path.join(root, name)
                    os.remove(os.path.join(root, name))

class runThread(Thread):
    def __init__ (self,slotName,platform):
        Thread.__init__(self)
        self.slotName = slotName
        self.platform = platform
    def run(self):
        os.system('python ' + os.path.join(os.environ['LCG_NIGHTLIES_SCRIPTDIR'], 'doBuild.py') + ' --slots ' + self.slotName + ' --platforms ' + self.platform)

def run(slotName, minusj=None, minusl=None, platforms=None):
    """ bootstrap function. Starts the LHCb Nightlies for the given <slotName>

        If <minusj> option is passed to the function, LCG Nightlies "builders" will be generated with -j compiler option.
        If <platforms> list is given, Nightlies will be launched only for selected platforms (platforms need to be defined in configuration.xml anyway).
    """
    if minusj == None or minusl == None: raise
    minusj = int(minusj)
    minusl = float(minusl)

    os.environ['LCG_NIGHTLIES_XMLCONFIGCOPIES'] = configurationHistoryPath
    generalConfig, slotList = configuration.readConf(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'))
    slot = configuration.findSlot(slotName, slotList)
    if slot != None:
        for p in slot.getProjects():
            c = p.getDependences()
            cu = {}
            for x in c.keys():
                cu[x.upper()] = c[x]
            if cu.has_key('LCGCMT'):
                os.environ['LCGCMT_VERSION'] = cu['LCGCMT']
                break
        buildersToCopyDict = {}
        buildersToCopy = []
        for x in generalConfig.get('externalBuilders', {}).keys():
            buildersToCopyDict[x] = generalConfig['externalBuilders'].getEnvParsed(x)
            buildersToCopy.append(x)
        buildersToGenerate = [x for x in listOfProjectNames if x not in buildersToCopy]
        for x in buildersToCopy:
            bTCx = buildersToCopyDict[x]
            if not os.path.exists(bTCx): continue
            shutil.copytree(bTCx, os.path.sep.join([slot.buildersDir(), x]))
            shutil.copytree(bTCx, os.path.sep.join([slot.buildersDir(), x.lower()]))
            shutil.copytree(bTCx, os.path.sep.join([slot.buildersDir(), x.upper()]))
        generateBuilders(slot, buildersToGenerate, minusj, minusl)
        releasePath = slot.releaseDir()
        if not os.path.exists(os.path.join(releasePath, 'configuration.xml')):
            shutil.copy2(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'), os.path.join(releasePath,'configuration.xml'))
            confSumm = file(os.path.join(releasePath,'confSummary.py'), 'w')
            confSumm.write('cmtProjectPathList = ' + pformat(slot.getCmtProjectPaths()))
            confSumm.close()
        if os.path.exists(os.path.join(releasePath, 'configuration.xml')):
            os.environ['LCG_XMLCONFIGDIR'] = releasePath
        if not platforms:
            platformlist = slot.getPlatforms()
        else:
            platformlist = platforms.split()
        if slot.getRenice() > 0:
            reniceString = 'nice --adjustment=+%s ' % str(slot.getRenice())
        else:
            reniceString = ''
        while platformlist:
            tmp = False
            for x in platformlist:
                if sum([int(os.path.exists(q)) for q in slot.waitForFlag(x)])==len(slot.waitForFlag(x)):
                #if os.path.exists(slot.waitForFlag(x)):
                    os.system(reniceString + 'python ' + os.path.join(os.environ['LCG_NIGHTLIES_SCRIPTDIR'], 'doBuild.py') + ' --slots ' + slotName + ' --platforms ' + x)
                    platformlist.remove(x)
                    tmp = True
                    break
            if tmp == False:
                print 'waiting 5 minutes for LCG slot to be ready...'
                time.sleep(300)

        cleanPycPyoFiles(slot)

def runparallel(slotName, minusj=None):
    """ bootstrap function. Starts the LHCb Nightlies for the given <slotName> with all platforms in parallel.

        If <minusj> option is passed to the function, LCG Nightlies "builders" will be generated with -j compiler option.
    """
    if minusj == None: minusj = 0
    else: minusj = int(minusj)
    os.environ['LCG_NIGHTLIES_XMLCONFIGCOPIES'] = configurationHistoryPath
    generalConfig, slotList = configuration.readConf(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'))
    slot = configuration.findSlot(slotName, slotList)
    if slot != None:
        generateBuilders(slot, listOfProjectNames, minusj)
        runningPlatformList = []
        for p in slot.getPlatforms():
            current = runThread(slotName, p)
            runningPlatformList.append(current)
            current.start()
            time.sleep(60) # wait
        for t in runningPlatformList: t.join()
        cleanPycPyoFiles(slot)

def cleanAFSSpace(slotName):
    """ removes everything from today from AFS for the <slotName> """
    generalConfig, slotList = configuration.readConf(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'))
    slot = configuration.findSlot(slotName, slotList)
    if slot is not None:
        pathToRemoveFiles = os.path.join(slot.releaseDir())
        if os.path.exists(pathToRemoveFiles):
            for x in os.listdir(pathToRemoveFiles):
                if os.path.isdir(os.path.join(pathToRemoveFiles, x)): rmtree(os.path.join(pathToRemoveFiles, x), ignore_errors=True)
                else: os.remove(os.path.join(pathToRemoveFiles, x))
    #clear logs from web
    if slot is not None:
        webDir = os.path.join(slot.wwwDir())
        for f in os.listdir(webDir):
            if re.match(slotName + '.' + configuration.TODAY, f): os.remove(os.path.join(webDir, f))


def getProjectObject(slotObject, projectName):
    """ getProjectObject(slotObject, projectName) -> projectObject

        Finds projectObject in a given slotObject.
    """
    project = None
    for p in slotObject.getProjects():
        if p.getName().lower() == projectName.lower():
            project = p
            break
    if project == None:
        print "Project not found:", projectName
        sys.exit()
    return project

def getSlotAndProject(slotName, projectName):
    """ getSlotAndProject(slotName, projectName) -> slotObject, projectObject

        Finds slotObject first and than projectObject in the slot.
    """
    try:
        generalConfig, slotList = configuration.readConf(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'))
        slot = configuration.findSlot(slotName, slotList)
        project = getProjectObject(slot, projectName)
    except:
        print "ERROR, config from:", os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml')
        raise
    return slot, project

def setCmtProjectPath(slot):
    """ setCmtProjectPath(slotObject)

        Sets CMTPROJECTPATH according to the configuration of a given slot, Removes CMTPATH variable if set.
    """
    cmtProjectPathPreviuos = None
    if os.environ.has_key('CMTPROJECTPATH'):
        cmtProjectPathPrevious = os.environ['CMTPROJECTPATH']
    cmtProjectPathNew = ''
    cmtProjectPathEntities = []
    cmtProjectPathEntities.append(slot.buildDir())

    if slot.getCmtProjectPaths():
        cmtProjectPathEntities += slot.getCmtProjectPaths()
    else:
        cmtProjectPathEntities.append(os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'lhcb', 'software', 'releases']))
        cmtProjectPathEntities.append(os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'sw', 'Gaudi', 'releases']))
        cmtProjectPathEntities.append(os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'sw', 'lcg', 'app', 'releases']))
    for e in cmtProjectPathEntities:
        cmtProjectPathNew += e + os.pathsep
    cmtProjectPathNew = cmtProjectPathNew[:-len(os.pathsep)]
    os.environ['CMTPROJECTPATH'] = cmtProjectPathNew
    if os.environ.has_key('CMTPATH'): del os.environ['CMTPATH']

def containerPackage(projectRoot):
    projectName = None
    containerPkg = None #projectName + 'Sys'
    for l in open(os.path.join(projectRoot,"cmt","project.cmt")):
        l = l.strip().split()
        #print l
        if len(l) == 2:
            if l[0] == "container":
                containerPkg = l[1]
                break
            elif l[0] == "project":
                projectName = l[1]
    if containerPkg is None and projectName:
        projectName = projectNamesDict.get(projectName.upper(), projectName)
        containerPkg = projectName + 'Sys'
    return containerPkg

def generatePath(slot, project, type, projectRealName=None):
    """ generatePath(slotObject, projectObject, type[, projectRealName]) -> string

        <type> can be: "TAG" or "SYSPACKAGECMT".
        Depending on the type selected, function returns a string representing path for the slot and the project:
        - TAG:           /build/nightlies/..slot../..day../LHCB/LHCB_v23r4
        - SYSPACKAGECMT: /build/nightlies/..slot../..day../LHCB/LHCB_v23r4/LHCbSys/cmt
    """
    projectRoot = os.path.join(slot.buildDir(), project.getName().upper(), project.getTag())
    if type == 'TAG':
        return projectRoot
    elif type == 'SYSPACKAGECMT':
        return os.path.join(projectRoot, containerPackage(projectRoot), 'cmt')
    else:
        raise Exception, 'type parameter: ' + type + ' not implemented.'

def readRequirementsFile(filename, withHat=False):
    """ readRequirementsFile(filename[, withHat]) -> dict

        Returns dictionary containing all 'use' statements form the given file, with package names as dictionary keys.
        Depending on <withHat> parameter, keys will be preceded by "hat" names (if withHat == True, default: False)
    """
    previousDir = os.getcwd()
    os.chdir(os.path.dirname(filename))
    os.system(cmtCommand + ' filter ' + filename + ' ' + filename + '_filtered')
    readerfile = file(filename + '_filtered', 'r')
    reader = readerfile.readlines()
    readerfile.close()
    #TODO: erase filtered file
    values = {}
    for line in reader:
        if re.match('^\s*use\s+[a-zA-Z0-9_-]+', line):
            d = line.split()
            if len(d)<=3: d.append('')
            if len(d)<=4: d.append('')
            if len(d)>=4 and len(d[3])>0 and d[3][0] == '-': # -no_auto_imports
                d[4] = d[3]
                d[3] = ''
            values[d[1]] = [d[2], d[3]]
            for i in range(4, len(d)):
                values[d[1]].append(d[i])
    if withHat:
        newValues = {}
        for v in values.keys():
            if values[v][1] != '': newKey = values[v][1] + '/' + v
            else: newKey = v
            newValues[newKey] = values[v]
        values = newValues
    os.chdir(previousDir)
    return values

def changeRequirementsFile(filename, changesDict, addons=None, removes=None):
    """ changeRequirementsFile(filename, changesDict[, addons[,removes]])

        Function changes the given file, by replacing "use" lines with the ones generated from <changesDict> (result of readReaquirementsFile function).
        Additional parameters: <addons> and <removes> - TODO: need to be checked and probably fixed (after some other changes may not work properly now).
    """
    readerfile = file(filename,'r')
    reader = readerfile.readlines()
    readerfile.close()
    writer = file(filename,'w')
    for line in reader:
        newline = line
        for x in changesDict.keys():
            nameWithoutHat = changedPackageName(x)
            if changesDict[x][1] != '':
                addToRE = '\s+'+changesDict[x][1]
            else:
                addToRE = ''
            newline = re.sub(nameWithoutHat+'\s+([Hh][Ee][Aa][Dd]|v[\*0-9]+(?:r[\*0-9]+(?:p[\*0-9]+)?)?)'+addToRE, nameWithoutHat+'  '+changesDict[x][0]+'  '+changesDict[x][1], newline)
        # <remove>:
        if removes:
            pac = re.sub('^\s*use\s+([a-zA-Z0-9_-]+).*$', '\\1', line).replace('\n','')
            if pac in [changedPackageName(x) for x in removes]: continue
        writer.write(newline)
    # <addon>
    if addons:
        for a in addons.keys():
            addPackage = changedPackageName(a)
            addHat = ''
            if addPackage != a: addHat = a.replace('/'+addPackage, '')
            writer.write('use ' + addPackage + ' ' + addons[a][0] + ' ' + addHat + "\n")
    writer.close()

def changedPackageName(c):
    """ changedPackageName(packageName) -> package name without "hat" part

        Returns given <packageName> without preceding "hat" name, if the given one consists the "hat". Otherwise, returns the same value.
        - Hat/Name -> Name
        - Name -> Name
    """
    cha = c.split('/')
    if len(cha) == 1: cha = cha[0]
    else: cha = cha[1]
    return cha

def generateBuilders(slot, projectNames, minusj, minusl):
    """ generateBuilders(destinationPath, projectNames, minusj, minusl)

        Creates a set of files and directory structure to act as LCG Nightlies "builders" for each of the project given in the <projectNames> list.
        <minusj> parameter sets the value of "-j" for compiler.
        <minusl> parameter sets the value of "-l" for compiler.
    """
    destPath = os.path.abspath(slot.buildersDir())
    lbLinux = ''
    lbWin = ''
    if slot.getLbLogin(configuration.PLATFORM_LINUX) is not None:
        lbLinux = "%s ; " % slot.getLbLogin(configuration.PLATFORM_LINUX)
    if slot.getLbLogin(configuration.PLATFORM_WIN32) is not None:
        lbWin = " %s && " % slot.getLbLogin(configuration.PLATFORM_WIN32)
    for p in projectNames:
        for pdir in [ p, p.lower(), p.upper() ]:
            rmtree(os.path.join(destPath, pdir), ignore_errors=True)
            os.makedirs(os.path.join(destPath, pdir, 'cmt') )
            f = file(os.path.join(destPath, pdir, 'cmt', 'requirements'), 'w')
            f.write('package %s\n' % pdir)
            f.write('version v1r0\n')
            f.write('macro packageName "%s"\n' % p)
            f.write('use LbScriptsSys\n')
            f.write('use Python v* LCG_Interfaces\n')
            lines = """
action pkg_get "cmt show tags ; mkdir -p logs ; %(launcher)s get %(packageName)s 2>&1 | tee -a logs/$(package)_$(CMTCONFIG)_get.log" \
       WIN32 " ( if not exist logs mkdir logs ) & %(launcher)s get %(packageName)s 2>&1 | tee -a logs/$(package)_$(CMTCONFIG)_get.log & true"
action pkg_config " %(launcher)s config %(packageName)s 2>&1 | cat"
action pkg_make " %(lbLinux)s %(launcher)s make %(packageName)s %(minusjvalue)d %(minuslvalue)d 2>&1 ; true" \
       WIN32    " %(lbWin)s   %(launcher)s make %(packageName)s %(minusjvalue)d %(minuslvalue)d 2>&1 & true"
action pkg_install " %(launcher)s install %(packageName)s 2>&1 | cat"
action pkg_test " %(launcher)s test %(packageName)s 2>&1 " \
       WIN32 " "
    """ % { "launcher" : "runpy LbRelease.Nightlies.actionLauncher",
            "packageName" : p,
            "minusjvalue": max(0,minusj),
            "minuslvalue": max(0,minusl),
            "lbLinux" : lbLinux,
            "lbWin" : lbWin }
            f.write(lines)
            f.close()
    rmtree(os.path.join(destPath, 'cmt'), ignore_errors=True)
    os.makedirs(os.path.join(destPath, 'cmt'))
    f = file(os.path.join(destPath, 'cmt', 'project.cmt'), 'w')
    f.write("project builders\nuse LCGCMT LCGCMT*\nuse LBSCRIPTS LBSCRIPTS_*\n")
    f.close()

def getpackget(package, version, withVersionDir=False, recursive=False, project=False):
    """ getpack(packageName, packageVersion[, withVersionDirectory])

        Function launches "getpack" with parameters.
    """
    cmd = getpackCommand
    if withVersionDir: cmd += ' -v'
    if recursive: cmd += ' -r'
    if project: cmd += ' -P'
    cmd += ' %s %s' % (package, version)
    print "*"*80
    print cmd
    print "*"*80
    os.system(cmd)

def disableLCG_NIGHTLIES_BUILD():
    """ Removes "LCG_NIGHTLIES_BUILD" from CMTEXTRATAGS variable. """
    cmtExtraTags = None
    if os.environ.has_key('CMTEXTRATAGS'): cmtExtraTags = os.environ['CMTEXTRATAGS']
    if cmtExtraTags != None:
        listOfCmtExtraTags = cmtExtraTags.split(',')
        if 'LCG_NIGHTLIES_BUILD' in listOfCmtExtraTags: listOfCmtExtraTags.remove('LCG_NIGHTLIES_BUILD')
        cmtExtraTags = ','.join(listOfCmtExtraTags)
        os.environ['CMTEXTRATAGS'] = cmtExtraTags

def setCMTEXTRATAGS(slotName):
    global configuration
    tmp = reload(configuration)
    generalConfig, slotList = configuration.readConf(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'))
    slot = configuration.findSlot(slotName, slotList)
    if slot is not None and slot.cmtExtraTags is not None:
        os.environ['CMTEXTRATAGS'] = slot.cmtExtraTags

def changeEnvVariables():
    if systemType != 'windows': return
    for x in ['CMTPROJECTPATH', 'CMTROOT', 'LD_LIBRARY_PATH', 'LIB', 'INCLUDE', 'PATH']:
        if not os.environ.has_key(x): continue
        pth = os.environ[x].split(';')
        newPth1 = []
        if x == 'CMTPROJECTPATH':
            # first position of CMTPROJECTPATH is always local build dir, and the order must not be changed
            newPth1.append(pth[0])
            pth = pth[1:]
        for y in pth:
            yy = y.lower()
            newY = yy
            for q in localDirsOnWindows.keys():
                if q.lower() in newY:
                    newY = newY.replace(q.lower(), localDirsOnWindows[q])
                    newY = newY.lower()
            if yy != newY:
                newPth1.append(newY)
            else:
                newPth1.append(y)
        if ';' in os.environ[x]:
            os.environ[x] = ';'.join([xx for xx in newPth1 if len(xx)>0])
        else:
            os.environ[x] = ([xx for xx in newPth1 if len(xx)>0])[0]

def config(slotName, projectName):
    """ config(slotName, projectName)

        action function. Starts cmt "config" action (launched from LCG Nightlies "builder").
    """
    disableLCG_NIGHTLIES_BUILD()
    setCMTEXTRATAGS(slotName)
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)
    changeEnvVariables()

def clean(slotName):
    """ clean(slotName)

        Removes contents of the:
        - build directory,
        - "builders" directory,
        - remove logs from web directory,
        for given slot.
    """
    generalConfig, slotList = configuration.readConf(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'))
    slot = configuration.findSlot(slotName, slotList)
    if slot is not None: setCmtProjectPath(slot)
    changeEnvVariables()
    disableLCG_NIGHTLIES_BUILD()
    setCMTEXTRATAGS(slotName)
    #remove contents of build directory:
    rmtree(slot.buildDir(), ignore_errors=True)
    os.makedirs(slot.buildDir())
    #remove contents of builders directory:
    rmtree(slot.buildersDir(), ignore_errors=True)

def install(slotName, projectName):
    """ install(slotName, projectName)

        action function. Starts cmt "install" action (launched from LCG Nightlies "builder").
    """
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)
    changeEnvVariables()
    disableLCG_NIGHTLIES_BUILD()
    setCMTEXTRATAGS(slotName)
    os.chdir(generatePath(slot, project, 'SYSPACKAGECMT', projectName))

    # copying logs
    os.chdir(os.path.join(slot.buildDir(), 'www'))
    files = os.listdir(os.getcwd())
    for f in files:
        if not os.path.isfile(f): continue
        shutil.copy2(os.path.join(slot.buildDir(), 'www', f), os.path.join(slot.wwwDir(), f))

    releasePath = slot.releaseDir()
    if releasePath != None:
        #instead of the previous isDone file copying:
        isDoneFrom = os.path.join(slot.buildDir(), 'isDone-'+os.environ['CMTCONFIG'])
        isDoneTo = os.path.join(slot.releaseDir(), 'isDone-'+os.environ['CMTCONFIG'])
        if os.path.exists(isDoneFrom):
            shutil.copy2(isDoneFrom, isDoneTo)
        isStartedTo = os.path.join(slot.releaseDir(), 'isStarted-'+os.environ['CMTCONFIG'])
        if not os.path.exists(isStartedTo): file(isStartedTo, "w").close()
        if os.path.exists(isDoneTo) and os.path.exists(isStartedTo): os.remove(isStartedTo)
        copyLocal(os.path.join(slot.buildDir(), projectName.upper(), project.getTag()), os.path.join(releasePath, projectName.upper(), project.getTag()))
        if not os.path.exists(os.path.join(releasePath, 'configuration.xml')):  shutil.copy2(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'), releasePath)

def docs(slotName, projectName):
    """ docs(slotName, projectName)

        action function.
    """
    disableLCG_NIGHTLIES_BUILD()
    setCMTEXTRATAGS(slotName)
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)
    changeEnvVariables()
    if not project.getDocsFlag(): return
    else:
        time.sleep(2)
        configuration.system('echo "Doxygen start: '+ time.strftime('%c', time.localtime()) +'"')
        os.chdir(generatePath(slot, project, 'TAG', projectName))
        configuration.system('make docs > ./docs.log')
        configuration.system('echo "' + '*'*80 + '"')
        configuration.system('echo "Doxygen finished: '+ time.strftime('%c', time.localtime()) +'"')
        releasePath = slot.releaseDir()
        if os.path.exists(os.path.join(slot.buildDir(), projectName.upper(), project.getTag(), 'InstallArea', 'doc')) and releasePath != None:
            configuration.system('echo "' + '*'*80 + '"')
            configuration.system('echo "Copying documentation to AFS started: '+ time.strftime('%c', time.localtime()) +'"')
            copyLocal(os.path.join(slot.buildDir(), projectName.upper(), project.getTag(), 'InstallArea', 'doc'), os.path.join(releasePath, projectName.upper(), project.getTag(), 'InstallArea', 'doc'))
            configuration.system('echo "Copying documentation to AFS finished: '+ time.strftime('%c', time.localtime()) +'"')

def make(slotName, projectName, minusj, minusl):
    """ make(slotName, projectName, minusj, minusl)

        action function. Starts cmt "make" action (launched from LCG Nightlies "builder").
    """
    if minusj > 1: minusjcmd = ' -j' + str(minusj)
    else: minusjcmd = ''
    if minusl > 0: minuslcmd = ' -l' + str(minusj)
    else: minuslcmd = ''
    disableLCG_NIGHTLIES_BUILD()
    setCMTEXTRATAGS(slotName)
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)

    if os.path.exists(os.sep.join([generatePath(slot, project, 'TAG', projectName), 'build.'+os.environ.get('CMTCONFIG','')+'.done'])):
        configuration.system('echo "Build already done for ' + os.environ.get('CMTCONFIG', '') + '. Skipped."')
        docs(slotName, projectName)
        return
    else:
        # removing rubbish from log file
        os.chdir(generatePath(slot, project, 'TAG', projectName))
        try:
            logFileToBeRemoved = file(os.sep.join(['logs', os.environ.get('CMTCONFIG', '')])+'.log', 'w')
            logFileToBeRemoved.close()
        except:
            pass
    changeEnvVariables()

    os.chdir(generatePath(slot, project, 'SYSPACKAGECMT', projectName))
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('echo "'+ time.strftime('%c', time.localtime()) +'"')
    configuration.system('echo "LCG Nightlies:    '+os.path.dirname(configuration.__file__)+'"')
    configuration.system('echo "SITEROOT:         '+os.environ.get('SITEROOT','')+'"')
    configuration.system('echo "CMTROOT:          '+os.environ.get('CMTROOT','')+'"')
    configuration.system('echo "CMTPROJECTPATH:   '+os.environ.get('CMTPROJECTPATH','')+'"')
    configuration.system('echo "CMTCONFIG:        '+os.environ.get('CMTCONFIG','')+'"')
    configuration.system('echo "CMTPATH:          '+os.environ.get('CMTPATH','')+'"')
    configuration.system('echo "CMTEXTRATAGS:     '+os.environ.get('CMTEXTRATAGS','')+'"')
    configuration.system('echo "LD_LIBRARY_PATH:  '+os.environ.get('LD_LIBRARY_PATH','')+'"')
    configuration.system('echo "LIB:              '+os.environ.get('LIB','')+'"')
    configuration.system('echo "INCLUDE:          '+os.environ.get('INCLUDE','')+'"')
    configuration.system('echo "PATH:             '+os.environ.get('PATH','')+'"')
    configuration.system('echo "LBUTILSROOT:      '+os.environ.get('LBUTILSROOT','')+'"')
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('cmt show macro LCG_releases')
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('cmt show projects')
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('cmt show uses')
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('cmt show tags')
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('printenv | sort')
    configuration.system('echo "' + '*'*80 + '"')

    if os.path.exists(os.sep.join(['..','..', 'Makefile'])):
        os.chdir(os.sep.join(['..']*2))
        if 'use-distcc' in os.environ.get('CMTEXTRATAGS', '') or 'force-distcc' in os.environ.get('CMTEXTRATAGS', ''):
            usedistcc = '-j'
        else:
            usedistcc = ''
        if 'CMTEXTRATAGS' in os.environ and len(os.environ['CMTEXTRATAGS'])>0:
            os.environ['CMTEXTRATAGS'] = 'no-pyzip,'+os.environ.get('CMTEXTRATAGS', '')
        else:
            os.environ['CMTEXTRATAGS'] = 'no-pyzip'
        configuration.system('make %s -k -j%d -l%d Package_failure_policy=ignore logging=enabled > make.%s.log' % (usedistcc, minusj, minusl, os.environ['CMTCONFIG']) )
        os.chdir(generatePath(slot, project, 'TAG', projectName))
        logFiles = []
        for r, d, f in os.walk("."):
            if r == ".": continue
            elif "cmt" in d: d[:] = ["cmt"]
            else:
                if "build.%(CMTCONFIG)s.log" % os.environ in f:
                    logFiles.append((os.stat(os.path.join(r, "build.%(CMTCONFIG)s.log" % os.environ)).st_mtime, os.path.join(r, "build.%(CMTCONFIG)s.log" % os.environ)))
        logFiles.sort()
        for x in logFiles:
            print "-"*80
            print "Logfile:", x[1]
            print "-"*80
            print file(x[1], 'r').read()
    else:
        makeCmd = '%s make %s' % (cmtCommand, minusjcmd)
        if slot.getQuickMode() is not None : makeCmd += ' QUICK=' + str(slot.getQuickMode())  # append
        if 'CMTEXTRATAGS' in os.environ and len(os.environ['CMTEXTRATAGS'])>0:                # prepend
            makeCmd = 'CMTEXTRATAGS=no-pyzip,%(CMTEXTRATAGS)s ' % os.environ + makeCmd
        else:
            makeCmd = 'CMTEXTRATAGS=no-pyzip ' + makeCmd
        cmtCmdForBroadcast = '%s all ; %s tests' % (makeCmd, makeCmd)
        fullCmd = '%s br - "%s"' % (cmtCommand, cmtCmdForBroadcast)
        configuration.system('echo "COMMAND: ' + fullCmd.replace('"','\\"') + '"')
        configuration.system(fullCmd)
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('echo "Build finished: '+ time.strftime('%c', time.localtime()) +'"')

    os.chdir(generatePath(slot, project, 'TAG', projectName))
    buildFlagFile = open('./build.'+os.environ.get('CMTCONFIG','')+'.done', 'w')

    docs(slotName, projectName)

def test(slotName, projectName):
    """ test(slotName, projectName)

        action function. Starts cmt "test" action (launched from LCG Nightlies "builder").
    """
    disableLCG_NIGHTLIES_BUILD()
    setCMTEXTRATAGS(slotName)
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)
    changeEnvVariables()
    os.chdir(generatePath(slot, project, 'SYSPACKAGECMT', projectName))

    configuration.system('echo "'+ time.strftime('%c', time.localtime()) +'" >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
    configuration.system('cmt br - cmt TestPackage')
    configuration.system('cmt qmtest_summarize 2>&1 >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
    configuration.system('echo "'+ time.strftime('%c', time.localtime()) +'" >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')

    #log files copying:
    testSummaryFrom = os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), '..', '..', 'logs', os.environ['CMTCONFIG'] + '-qmtest.log')
    testSummaryTo = os.path.join(slot.wwwDir(), slotName + '.' + configuration.TODAY + '_' + project.getTag() + '-' + os.environ['CMTCONFIG'] + '-qmtest.log')
    if os.path.exists(testSummaryFrom): shutil.copy2(testSummaryFrom, testSummaryTo)

def get(slotName, projectName):
    """ get(slotName, projectName)

        action function. Starts cmt "get" action (launched from LCG Nightlies "builder").
    """
    disableLCG_NIGHTLIES_BUILD()
    setCMTEXTRATAGS(slotName)
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)
    changeEnvVariables()

    os.chdir(slot.buildDir())
    if not os.path.exists('www'): os.mkdir('www')
    if os.path.exists(project.getName()):
        # create symbolic link: LHCB --> LHCb, LBCOM --> Lbcom, ... [ only on Linux ? ]
        if not os.path.exists(project.getName().upper()): os.symlink(project.getName(), project.getName().upper())
    elif os.path.exists(project.getName().lower()):
        # create symbolic link: LHCB --> lhcb, LBCOM --> lbcom, ... [ only on Linux ? ]
        if not os.path.exists(project.getName().upper()): os.symlink(project.getName().lower(), project.getName().upper())

    os.chdir(slot.buildDir())
    getpackget(project.getName().upper(), project.getVersion(), project=True)
    os.chdir(generatePath(slot, project, 'TAG', projectName))
    containerPackageName = containerPackage('.')
    getpackget(containerPackageName, project.getVersion())

    # copy the 'requirements' file
    shutil.copy2(os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), 'requirements'), os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), 'requirements_copy'))

    changesMade = {}
    #change also ProjectSys package in all requirements files later (next projects in the slot) !!!
    changesMade[containerPackageName] = project.getVersion()
    sysPackageRF = readRequirementsFile(os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), 'requirements'), withHat=True)

####################################################################
####################################################################

    if project.getHoeFlag():
        # add every package from Sys package's requirements file to the list of changes (to HEAD)
        # but IN ORDER! Order is important because of the possibility of over-write sth with <change> tag in XML file
        # --> first add HEAD to the dict, later over-write everything what was in XML file as a <change>
        changesBefore = project.getChanges()
        newChanges = {}
        for x in sysPackageRF.keys(): newChanges[x] = 'HEAD'              # key: Hat/Package
        for x in changesBefore.keys(): newChanges[x] = changesBefore[x]    # key: Hat/Package (in config file it must be with HAT from now if the hat exists)
        project.setChanges(newChanges)

    if len(project.getChanges()) > 0:
        # apply changes to the main `requirements` file
        for change in project.getChanges().items():
            #cha = changedPackageName(change[0])
            cha = change[0]
            if cha in sysPackageRF.keys():
                sysPackageRF[cha][0] = change[1]
            changesMade[change[0]] = change[1]
        changeRequirementsFile(os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), 'requirements'), sysPackageRF, project.getAddons(), project.getRemoves())

    os.chdir(generatePath(slot, project, 'TAG', projectName))

    getpackget(containerPackageName, project.getVersion(), recursive=True)

    # save the `changesMade` data to a file. It will be used by next projects in the slot
    os.chdir(generatePath(slot, project, 'TAG', projectName))
    pklFile = open('changesMade-' + os.environ['CMTCONFIG'] + '.pkl', 'wb')
    pickle.dump(changesMade, pklFile)
    pklFile.close()

    # read current project's project.cmt file
    projectCmtFileName = os.path.join(generatePath(slot, project, 'TAG', projectName), 'cmt', 'project.cmt')

    readerfile = file(projectCmtFileName, 'r')
    reader = readerfile.readlines()
    readerfile.close()
    writer = file(projectCmtFileName, 'w')

    # apply changes to current project's 'project.cmt' file according to previous project in the slot and <dependence> entities
    deps = project.getDependences()
    projs = {}
    for p in slot.getProjects():
        if p.getName().upper() == project.getName().upper(): break
        projs[p.getName().upper()] = p.getTag()
    for line in reader:
        newline = line
        for proj in projs.keys():
            newline = re.sub('^(\s*use\s+' + proj + '\s+).*$', '\\1' + projs[proj], newline)
        for change in deps.keys():
            newline = re.sub('^(\s*use\s+' + change.upper() + '\s+).*$', '\\1' + deps[change], newline)
        writer.write(newline)
    writer.close()

    # apply changes of previous projects AND the CURRENT PROJECT in the slot to current project's 'requirements' file
    previousChangesMade = {}
    for p in slot.getProjects():
        if p.getName().upper() == 'LCGCMT': continue
        if p.getName().upper() == project.getName().upper(): break
        pklFile = file(os.path.join(generatePath(slot, p, 'TAG', projectName), 'changesMade-' + os.environ['CMTCONFIG'] + '.pkl'), 'rb')
        tmpChangesMade = pickle.load(pklFile)
        pklFile.close()
        previousChangesMade.update(tmpChangesMade)

    changesMadeDict = previousChangesMade.copy()
    changesMadeDict.update(changesMade)

    parentDir = generatePath(slot, project, 'TAG', projectName)
    os.chdir(parentDir)
    requirementsFilesList = []
    for x in sysPackageRF.keys():
        fileNameTmp = os.sep.join([parentDir, x.replace('/', os.sep), 'cmt', 'requirements'])
        if os.path.exists(fileNameTmp):
            requirementsFilesList.append(fileNameTmp)
        else:
            os.system('echo "ERROR: file not found: ' + fileNameTmp + '"')

    for f in requirementsFilesList:
        f = f.replace('\n','')
        shutil.copy2(f, f + '_copy')
        d = readRequirementsFile(f, withHat=True)
        for x in d.keys():
            nameWithoutHat = changedPackageName(x)
            if x in changesMadeDict.keys():
                d[x][0] = changesMadeDict[x]
        changeRequirementsFile(f, d)

    os.chdir(generatePath(slot, p, 'SYSPACKAGECMT', projectName))
    configuration.system(cmtCommand + ' br "' + cmtCommand + ' config"')

